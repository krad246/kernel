#!/usr/bin/env python3
#
# Dump info about linker script symbols that pertain to addresses and sizes.
#
# Author: Kareem Khazem <karkhaz@amazon.com>
# Copyright Amazon, Inc. 2017


import ast
import argparse
import json
import logging
from   logging import error, warning, info, debug
import operator
import os
import regex as re
import subprocess
import sys
import textwrap
import traceback

# ast.Num was deprecated in python 3.8
if sys.version_info.major > 3 or \
        (sys.version_info.major == 3 and sys.version_info.minor > 7):
    ast_num = ast.Constant
else:
    ast_num = ast.Num

def epilog():
    return textwrap.dedent("""
    This script generates a C file containing two kinds of information:

    - The values of symbols that are defined in a linker script; these
      are printed as C definitions, like
        char *bss_start = (char *)4070047185u;

    - The extent of ELF sections that are defined in a linker script;
      these are printed as CPROVER annotations, like
        __CPROVER_allocated_memory(0xe9fda44b, 4096);

    A goto-binary of this C file can be linked into the rest of the
    codebase that you wish to analyse. This provides CPROVER with
    definitions that it otherwise would not have access to, since they
    are defined in a linker script rather than C code. This information
    can also be printed in JSON rather than as a C file, so that CPROVER
    can invoke and use this script without user intervention.

    This script needs a list of symbols declared but never defined in C
    code. The hacky way of supplying this list is by passing the path to
    the codebase with the --dir flag; this script will scan the codebase
    for extern-declared variables. A better way is to generate that list
    with CPROVER, and pass that list in using --sym-file. The argument
    to --sym-file can be a filename, or '-' for stdin.
    """)


"""`Running-regex' linker script parser. We don't currently use a full
grammar, as we only need a fraction of the information contained in
linker scripts to give to CBMC. If in the future we need a more
sophisticated parser, we should use an actual grammar from a real
parser. GNU LD uses a YACC/Flex setup and has a very complete grammar,
but we cannot use it (GPL 3). LLD (the LLVM project's linker script
parser) is hand-written (so no explicit grammar), but they do not aim to
support all of GNU LD's syntax, so LLD doesn't work on some real linker
scripts. So in summary: use this regex parser while it's practical;
switch to LLD when needed, and possibly contribute to LLD development to
support parsing your use case."""
def get_linker_script_data(script):
    try:
        with open(script) as f:
            lines = f.read().splitlines()
    except IOError:
        error("Linker script '%s' not found", script)
        exit(1)

    text = " ".join(lines)
    text = re.sub(r"\s+", " ", text)

    # In these regexes, we always start by matching a whitespace. This
    # is so that we don't match every substring of an identifier. i.e.
    # if we have a section definition .text : { ..., then we only want
    # to recognise a section called ".text", and not also "text", "ext",
    # "xt", and "t".
    #
    # Just to be safe, ensure that the first character of the linker
    # script is a whitespace.
    text = " %s" % text

    # Lex out comments
    text = re.sub(r"/\*.*?\*/", " ", text)

    close_brace     = re.compile(r"\s*}(\s*>\s*\w+)?")
    uwnknown_cmd    = re.compile(r"\sPHDRS\s*{")  # only this pattern for now, more might follow!
    memory_cmd      = re.compile(r"\sMEMORY\s*{")
    sections_cmd    = re.compile(r"\sSECTIONS\s*{")
    assign_current  = re.compile(r"(?P<sym>\*\((?>[^()]+|(?1))*\))")
    sec_def         = re.compile(r"\s(?P<sec>([-\.\w]+)|(/DISCARD/))"
                                 r"\s+([^:{};]*?):([^:{};])*?{")
    assign_size     = re.compile(r"\s(?P<sym>\w+)\s*=\s*SIZEOF\("
                                     r"(?P<sec>\.\w+)\)\s*;")
    memory_block    = re.compile(r"\s(?P<name>\w+)\s*:\s*ORIGIN\s*=\s*"
                                     r"(?P<orig>0[xX][a-fA-F0-9]+|\d+)\s*,\s*"
                                     r"LENGTH\s*=\s*(?P<len>0[xX][a-fA-F0-9]+|\d+)\s*"
                                     r"(?P<unit>[KMG]*)")

    exp = r"(ORIGIN\(\w+\)|LENGTH\(\w+\))"
    op  = r"(\+|\-)"
    assign_expr     = re.compile(r"\s(?P<sym>\w+)\s*=\s*"
                                  r"(?P<expr>{exp}(\s*{op}\s*{exp})*)"
                                  r"\s*;".format(exp=exp, op=op))
    provide_expr	= re.compile(r"PROVIDE\(\s*(?P<sym>[\w]+)\s*=\s*"
                                 r"(?P<val>0[xX][a-fA-F0-9]+|\d+)\s*\);")

    # If we match a regex, call the right function to update the state
    # with the info gleaned from the matched string.
    jump_table = {
        close_brace     : close_brace_fun,
        uwnknown_cmd    : unknown_cmd_fun,
        memory_cmd      : memory_cmd_fun,
        sections_cmd    : sections_cmd_fun,
        assign_current  : assign_current_fun,
        memory_block    : memory_block_fun,
        sec_def         : sec_def_fun,
        assign_size     : assign_size_fun,
        assign_expr     : assign_expr_fun,
        provide_expr    : provide_expr_fun,
    }

    # Whenever we match an interesting regex, we'll update the state
    # with whatever information we want to rip from that bit of text.
    state = {}

    # The section definition that we were last in.
    state["cur-sec"] = None

    # If we know what section *start* the current address (.) points to,
    # then this will not be None. It's used to match an assignment to
    # the start of a section.
    state["start-valid"] = None

    # If we have just seen an assignment, then this will not be None.
    # It's used to match up an assignment with the end of a section.
    state["end-valid"] = None

    # Each entry here maps a section name to a map. That map maps "size"
    # to the symbol pointing to the size of the section, and "start"
    # to the symbol pointing to the start address of the section. One of
    # "start" or "size" may be absent, if we couldn't get that bit of
    # information from the linker script.
    state["sections"] = {}

    # We can use the list of valid addresses to sanity-check that the
    # start addresses of sections are genuinely addresses.
    state["valid-addresses"] = []

    # Symbols that get some expression assigned to them, either inside
    # or outside a section definition. We'll match them up later.
    state["expr-assigns"] = []

    # These are to sanity-check the parsing.
    state["MEM"] = False
    state["SEC"] = False
    state["DEF"] = False
    state["UNKNOWN"] = False

    i = 0
    while i < len(text):
        buf = text[i:]
        i += 1
        asrt(not (state["MEM"] and state["SEC"]),
             "memory & sections", buf)
        asrt(not state["DEF"] or state["SEC"],
             "def outside SECTION", buf)

        jump_fun = None
        matched_str = None
        matched_re = None
        match = None
        for regex, fun in jump_table.items():
            m = regex.match(buf)
            if m:
                if jump_fun is not None:
                    error("matched multiple regexes\n%s", buf)
                    exit(1)
                jump_fun = fun
                match = m
                matched_str = buf[m.span()[0]:m.span()[1]]
                for s, v in locals().items():
                    if v is regex and s != "regex":
                        matched_re = s
        if jump_fun is not None:
            info("regex '%s' matched '%s'", matched_re, matched_str)
            jump_fun(state, match, buf)
            i = i + match.span()[1] - 1
        else:
            debug("Clobbering due to '%s'...", buf[:20])
            # There may have been some intermediate command between the
            # start of a section definition and where we are. So we have
            # no idea what address the current address pointer refers to
            state["start-valid"] = None
            # There may have been an intermediate command between the
            # last assignment and the end of the section.
            state["end-valid"] = None
    match_up_expr_assigns(state)
    return state


def assign_expr_fun(state, match, _):
    # Do NOT invalidate 'start-valid' here. Assignments from expressions
    # do not actually advance the current address pointer.
    sym, expr = match.group("sym"), match.group("expr")
    origin_pat = r"ORIGIN\((?P<block>\w+?)\)"
    origins = re.findall(origin_pat, expr)
    if len(origins) != 1:
        info("assign with %d origins, skipping: %s", len(origins),
               match.string())
        return
    ret = {"origin": origins[0], "sym": sym}
    for block_name, data in state["blocks"].items():
        for op in ["ORIGIN", "LENGTH"]:
            old_expr = str(expr)
            expr = re.sub(r"%s\(%s\)" % (op, block_name), str(data[op]),
                          expr)
            if expr != old_expr:
                info("Subbed %s(%s) with %d", op, block_name, data[op])
    try:
        ret["addr"] = safe_eval(expr)
    except RuntimeError:
        error("Unable to evaluate '%s'", expr)
        sys.exit(1)
    info("Evaluated '%s' to %d", expr, ret["addr"])
    state["expr-assigns"].append(ret)

def provide_expr_fun(state, match, _):
    sym, val = match.group("sym"), match.group("val")
    
    ret = {"origin":"", "sym": sym}
    try:
        ret["addr"] = val
    except RuntimeError:
        error("Unable to evaluate '%s'", sym)
        sys.exit(1)
    info("Evaluated '%s' to %d", sym, ret["addr"])
    state["expr-assigns"].append(ret)

def sec_def_fun(state, match, buf):
    asrt(not state["DEF"], "nested sec def", buf)
    state["DEF"] = True
    sec = match.group("sec")
    info("Current section is now '%s'", sec)
    state["cur-sec"] = sec
    state["start-valid"] = True


def assign_size_fun(state, match, buf):
    asrt(state["SEC"], "assignment outside SECTIONS", buf)
    sec = match.group("sec")
    if sec not in state["sections"]:
        state["sections"][sec] = {}
    sym = match.group("sym")
    info("'%s' marks the size of section '%s'", sym, sec)
    state["sections"][sec]["size"] = sym


def assign_current_fun(state, match, buf):
    asrt(state["SEC"], "assignment outside SECTIONS", buf)
    sec = state["cur-sec"]
    state["end-valid"] = match

    if sec not in state["sections"]:
        state["sections"][sec] = {}
    sym = match.group("sym")

    if not state["sections"][sec]:
        info("'%s' marks the start of section '%s'", sym, sec)
        state["sections"][sec]["start"] = sym
    
    state["sections"][sec]["end"] = sym


def close_brace_fun(state, _, buf):
    # We might have seen an assignment immediately before this.
    if state["end-valid"]:
        asrt(state["DEF"], "end-valid outside sec-def", buf)
        sec = state["cur-sec"]
        if sec in state["sections"]:
            sym = state["end-valid"].group("sym")
            info("'%s' marks the end of section '%s'", sym, sec)
            state["sections"][sec]["end"] = sym
            state["end-valid"] = None
        else:
            # Linker script assigned end-of-section to a symbol, but not
            # the start. this is useless to us.
            pass

    if state["DEF"]:
        info("Closing sec-def")
        state["DEF"] = False
    elif state["SEC"]:
        info("Closing sections")
        state["SEC"] = False
    elif state["MEM"]:
        info("Closing memory command")
        state["MEM"] = False
    elif state["UNKNOWN"]:
        info("Closing unknown command")
        state["UNKNOWN"] = False
    else:
        error("Not in block\n%s", buf)
        traceback.print_stack()
        exit(1)


def memory_block_fun(state, m, buf):
    asrt(state["MEM"], "memory block outside MEMORY", buf)
    start, length, unit = m.group("orig"), m.group("len"), m.group("unit")
    length = int(length, 16)
    dec_start = int(start, 16)
    mul = {"K": 1000, "M": 1000000, "G": 1000000000}
    if unit in mul:
        length = length * mul[unit]
    end = dec_start + length
    info("mem block %s from %d to %d (%d%s long)", start, dec_start, end,
            length, unit)
    state["valid-addresses"].append({"from": dec_start, "to": end})

    name = m.group("name")
    if "blocks" not in state:
        state["blocks"] = {}
    state["blocks"][name] = {"ORIGIN": int(start, 16), "LENGTH": length}


def sections_cmd_fun(state, _, buf):
    asrt(not state["SEC"], "encountered SECTIONS twice", buf)
    state["SEC"] = True


def memory_cmd_fun(state, _, buf):
    asrt(not state["MEM"], "encountered MEMORY twice", buf)
    state["MEM"] = True


def unknown_cmd_fun(state, _, buf):
    asrt(not state["MEM"], "encountered UNKNOWN twice", buf)
    state["UNKNOWN"] = True


def match_up_expr_assigns(state):
    blocks = set([data["origin"] for data in state["expr-assigns"]])
    for block in blocks:
        assigns = [a for a in state["expr-assigns"]
                   if a["origin"] == block]
        assigns = sorted(assigns, key=(lambda a: a["addr"]))
        if len(assigns) < 2:
            warning("Only 1 assignment to expr involving %s", block)
            continue
        start_addr, end_addr = assigns[0]["addr"], assigns[-1]["addr"]
        start_sym, end_sym = assigns[0]["sym"], assigns[-1]["sym"]
        info("Valid memory from %s (%d) to %s (%s) [%s block]",
                start_sym, start_addr, end_sym, end_addr, block)
        tmp = {"start": start_sym, "end": end_sym}
        sec_name = "BLOCK_%s_%s-%s" % (block, start_sym, end_sym)
        state["sections"][sec_name] = tmp


def asrt(cond, msg, buf):
    if not cond:
        error("%s\n%s", msg, buf)
        exit(1)

def safe_eval(expr):
    tree = ast.parse(expr, mode="eval").body

    def eval_single_node(node):
        logging.debug(node)
        if isinstance(node, ast_num):
            return node.n
        if isinstance(node, ast.BinOp):
            left = eval_single_node(node.left)
            right = eval_single_node(node.right)
            op = eval_single_node(node.op)
            return op(left, right)
        return {
            ast.Add: operator.add,
            ast.Sub: operator.sub,
            ast.Mult: operator.mul,
            # Use floordiv (i.e. //) so that we never need to
            # cast to an int
            ast.Div: operator.floordiv,
        }[type(node)]

    return eval_single_node(tree)

def main():
    pars = argparse.ArgumentParser(
        description="Generate info about linker-defined symbols and regions.",
        epilog=epilog(),
        formatter_class=argparse.RawDescriptionHelpFormatter)
    
    pars.add_argument("-s", "--script", metavar="S", required=True,
                      help="path to linker script")         
    pars.add_argument("-t", "--out-file", metavar="F",
                      help="default: stdout", default=None)

    verbs = pars.add_mutually_exclusive_group()
    verbs.add_argument("-v", "--verbose", action="store_true")
    verbs.add_argument("-w", "--very-verbose", action="store_true")

    args = pars.parse_args()

    if args.verbose:
        lvl = logging.INFO
    elif args.very_verbose:
        lvl = logging.DEBUG
    else:
        lvl = logging.WARNING
    form = "linkerscript parse %(levelname)s: %(message)s"
    logging.basicConfig(format=form, level=lvl)

    script_data = get_linker_script_data(args.script)
    info("script data %s" % json.dumps(script_data, indent=2))

    final = json.dumps(script_data,
                       indent=2)
    if args.out_file:
        with open(args.out_file, "w") as f:
            f.write(final)
        info(final)
    else:
        print(final)


if __name__ == "__main__":
    main()
