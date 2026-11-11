import argparse
import itertools
import re
from instruction_reader import *

# Matches bit-pattern "bPATTERN"
BINARY_DATA_REGEX = r'''b?([01]+)'''

# Matches operand-part "NAME[UPPER:LOWER]s"
OPERAND_DATA_REGEX = r'''(\w+)\[((?:\d+:\d+)|(?:\d+:\d+(?:\|\d+:\d+)+))\](s?)'''
OPERAND_RANGE_REGEX = r'''(\d+):(\d+)\|?'''


REGEX_FLAGS = re.DOTALL


if __name__ == '__main__':
	parser = argparse.ArgumentParser()
	parser.add_argument('--output-file', required=True, action='store', dest='output_file', help='Output file for the instructions')
	parser.add_argument('--custom-descriptions', required=True, nargs='+', dest='source_files', help='Source ND specification file(s)')
	arguments = parser.parse_args()

	binary_data_regex  = re.compile(BINARY_DATA_REGEX,  flags=REGEX_FLAGS)
	operand_data_regex = re.compile(OPERAND_DATA_REGEX, flags=REGEX_FLAGS)
	operand_range_regex = re.compile(OPERAND_RANGE_REGEX, flags=REGEX_FLAGS)

	instructions = []
	for source_file in arguments.source_files:
		with open(source_file,'r',encoding='utf-8') as source:

			for line in source:
				line = line.strip()
				if not line: continue

				line_part, syntax = line.split('%')
				print(line_part)
				instruction_name = line_part.strip().split(' ')[-1].lower().replace('.','_')
				print(f"Generating instruction \033[1m\033[96m{instruction_name}\033[0m")

				instruction  = Instruction(instruction_name)
				instruction.syntax = syntax.strip()

				instructions += [ instruction ]

				for part in line_part.split()[:-1]:
					# Check if part is a binary pattern
					part_match = binary_data_regex.fullmatch(part)
					if part_match:
						binary_pattern, = part_match.groups()
						instruction.add_match(BinaryMatch(binary_pattern))
						continue

					# Check if part is an operand section
					part_match = operand_data_regex.fullmatch(part)
					if part_match:
						operand_name, operand_range, signed = part_match.groups()
						range_match = operand_range_regex.finditer(operand_range)
						for match in range_match:
							operand_start, operand_end = match.groups()
							instruction.add_match(OperandMatch(operand_name, int(operand_start), int(operand_end)))
						continue

					print(f'Invalid part in instruction {instruction.get_name()}: {part}')
					exit(1)

	write_json_file(arguments.output_file, instructions)
