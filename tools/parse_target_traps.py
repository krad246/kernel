import re

def is_cpu_interrupt(sym):
	return re.search("__interrupt_vector_[0-9]+", sym[0])

def is_valid_cpu_interrupt(sym):
	return re.search("__interrupt_vector_[a-z]+", sym[1]["end"])

def should_exclude_interrupt(sym, exclude):
	return re.search(exclude, sym[1]["end"])

def interrupt_number(sym):
	return int(re.search("[0-9]+", sym[1]["start"])[0])

def interrupt_source(sym):
	# first match should be the numeric alias, next should be the label
	matches = re.findall("__interrupt_vector_([a-z0-9_]+)", sym[1]["end"])
	return matches[0]

def to_vector_name(source):
	return source.upper() + "_VECTOR"

if __name__ == "__main__":
	pass