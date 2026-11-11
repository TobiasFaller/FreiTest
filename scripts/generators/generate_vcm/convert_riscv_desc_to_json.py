import sys
import pathlib
import importlib
import collections
import argparse
import itertools
import regex

from instruction_reader import Instruction, OperandMatch, BinaryMatch, write_json_file


sys.path.append(f'{pathlib.Path("riscv-opcodes").absolute()}')
constants = importlib.import_module('.constants', 'riscv-opcodes')
parse = importlib.import_module('.parse', 'riscv-opcodes')


if __name__ == '__main__':
	parser = argparse.ArgumentParser()
	parser.add_argument('--output-file', required=True, action='store', dest='output_file', help='Output file for the instructions')
	parser.add_argument('--riscv-opcodes', required=True, nargs='+', dest='source_files', help='Source opcode specification file(s)')
	arguments = parser.parse_args()

	mappings = {}
	for name, mapping in constants.latex_mapping.items():
		mappings[name] = mapping \
			.replace("\\,$'$", '') \
			.replace('$\\neq$0', '') \
			.replace('$\\vert$', '|') \
			.replace('rd/rs1', 'rd')

	instr_dict = parse.create_inst_dict(arguments.source_files, False)

	instructions = []
	for name, spec in instr_dict.items():
		# Fill in the arguments into the encoding
		encoding = list(spec['encoding'])
		for arg in spec['variable_fields']:
			(msb, lsb) = constants.arg_lut[arg]
			for ind in range(lsb, msb + 1):
				encoding[31 - ind] = arg

		def _get_key(item):
			return {
				'0': ('binary', None),
				'1': ('binary', None),
				'-': ('unused', None)
			}.get(item, ('operand', item))

		print(f"Generating instruction \033[1m\033[96m{name}\033[0m")
		instruction  = Instruction(name)
		instructions.append(instruction)
		for (group_type, operand), group in itertools.groupby(encoding, key=_get_key):
			if group_type == 'binary':
				instruction.add_match(BinaryMatch(''.join(group)))
			elif group_type == 'operand':
				if operand in mappings:
					match = regex.match(r'([a-z0-9]+)(?:\[((?:\d+(?::\d+)?\|?)+)\])?', mappings[operand])
					operand_name = match.captures(1)
					operand_indices = match.captures(2)
					if operand_indices is None:
						for indices in operand_indices.split('|'):
							split = indices.split(':')
							top, bottom = split[0], split[len(split) - 1]
							instruction.add_match(OperandMatch(operand_name, int(top), int(bottom)))
						continue

				top, bottom = constants.arg_lut[operand]
				instruction.add_match(OperandMatch(operand, top-bottom, 0))

	write_json_file(arguments.output_file, instructions)
