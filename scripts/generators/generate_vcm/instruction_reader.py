#!/bin/env python3

import json
import re
import subprocess
import sys


REGEX_FLAGS = re.DOTALL
# Matches "NAME { encoding: ENCODING; ... }"
INSTRUCTION_REGEX = r'''([A-Z_.0-9]+)\s*(?:\(.*?\))?\s*\{\s*encoding:(.*?);(?:.*?;|.*?\{.*?\})*?\s*\}'''

# Matches bit-pattern "bPATTERN"
BINARY_DATA_REGEX = r'''b?([01]+)'''

# Matches operand-part "NAME[UPPER:LOWER]s"
OPERAND_DATA_REGEX = r'''(\w+)\[(\d+):(\d+)\](s?)'''


class Instruction:

	def __init__(self, name):
		self.name = name
		self.matches = []
		self.syntax = None

	def add_match(self, match):
		self.matches.append(match)

	def get_matches(self):
		return self.matches

	def get_name(self):
		return self.name

	def get_syntax(self):
		return self.syntax

	def get_operands(self, exclude=None, include=None):
		'''Combines all operand parts to a complete operand list'''
		operands = []
		for match in self.matches:
			if not isinstance(match, OperandMatch):
				continue

			# Check if operand was already mentioned in this instruction
			operand = next((operand for operand in operands if operand.get_name() == match.get_name()), None)
			if operand is None:
				operand = Operand(match.get_name())
				operands.append(operand)

			# Increase upper and lower limit of the operand with new part
			operand.extend_bits(match.get_bits_top(), match.get_bits_bottom())

		if exclude is not None:
			operands = [operand for operand in operands if operand.get_name() not in exclude]

		if include is not None:
			operands = [operand for operand in operands if operand.get_name() in include]

		return operands

	def has_operand(self, name):
		for match in self.matches:
			if not isinstance(match, OperandMatch):
				continue

			if match.get_name() == name:
				return True

		return False

	def get_operand_names(self):
		return [operand.get_name() for operand in self.get_operands()]

	def get_patterns(self):
		'''Returns the binary patterns to match in the opcode'''
		start = 0
		for match in reversed(self.matches):
			end = start + match.get_bits() - 1
			if isinstance(match, BinaryMatch):
				# Return the start and end position in the opcode
				# and then the pattern to match
				yield (end, start, match.get_pattern())

			start = end + 1

	def get_operand_parts(self, name):
		'''Returns the operand parts of the opcode which are then assigned to the output ports'''
		start = 0
		for match in reversed(self.matches):
			end = start + match.get_bits() - 1
			if isinstance(match, OperandMatch) and match.get_name() == name:
				# Return the start and end position in the opcode.
				# Then return the name and then return the start and end position
				# of the assignment to the operand output
				yield (end, start, name, match.get_bits_top(), match.get_bits_bottom())

			start = end + 1

	def __repr__(self):
		return f'Instruction {self.name} with operands {self.get_operands()}'


class BinaryMatch:

	def __init__(self, pattern):
		self.pattern = pattern

	def get_bits(self):
		return len(self.pattern)

	def get_pattern(self):
		return self.pattern

	def __repr__(self):
		return f'b{self.pattern}'


class OperandMatch:

	def __init__(self, name, bits_top, bits_bottom):
		self.name = name
		self.bits_top = bits_top
		self.bits_bottom = bits_bottom

	def get_name(self):
		return self.name

	def get_bits_top(self):
		return self.bits_top

	def get_bits_bottom(self):
		return self.bits_bottom

	def get_bits(self):
		return self.bits_top - self.bits_bottom + 1

	def __repr__(self):
		return f'{self.name}[{self.bits_top}:{self.bits_bottom}]'

class Operand:

	def __init__(self, name):
		self.name = name
		self.bits_top = -sys.maxsize - 1
		self.bits_bottom = sys.maxsize

	def get_name(self):
		return self.name

	def get_bits_top(self):
		return self.bits_top

	def get_bits_bottom(self):
		return self.bits_bottom

	def extend_bits(self, top, bottom):
		self.bits_top = max(self.bits_top, top)
		self.bits_bottom = min(self.bits_bottom, bottom)

	def get_bits(self):
		return self.bits_top - self.bits_bottom + 1

	def __repr__(self):
		return f'{self.name} ({self.get_bits()} bits)'


def load_core_file(file):
	instruction_regex = re.compile(INSTRUCTION_REGEX, flags=REGEX_FLAGS)
	binary_data_regex = re.compile(BINARY_DATA_REGEX, flags=REGEX_FLAGS)
	operand_data_regex = re.compile(OPERAND_DATA_REGEX, flags=REGEX_FLAGS)

	# Run the preprocessor over the source data to remove comments
	source_data = subprocess.run(f'gcc -E -P -x c {file} -nostdinc', shell=True, check=True, universal_newlines=True, stdout=subprocess.PIPE, stderr=subprocess.DEVNULL).stdout

	# Parse the instructions from the original core_desc file
	instructions = []
	for match in instruction_regex.finditer(source_data):
		instruction_name, instruction_encoding = match.groups()

		# Convert the name to a valid Verilog identifier
		instruction_name = instruction_name.lower().replace('.', '_')

		instruction = Instruction(instruction_name)
		instructions.append(instruction)

		for part in [part.strip() for part in instruction_encoding.split('|')]:
			# Check if part is a binary pattern
			part_match = binary_data_regex.fullmatch(part)
			if part_match:
				binary_pattern, = part_match.groups()
				instruction.add_match(BinaryMatch(binary_pattern))
				continue

			# Check if part is an operand section
			part_match = operand_data_regex.fullmatch(part)
			if part_match:
				operand_name, operand_start, operand_end = part_match.groups()
				instruction.add_match(OperandMatch(operand_name, int(operand_start), int(operand_end)))
				continue

			print(f'Invalid part in instruction {instruction_name}: {part}')
			exit(1)

	return instructions


def load_json_file(file):
	with open(file, 'rt', encoding='UTF-8') as file:
		instructions_json = json.loads(file.read())

	instructions = []
	for instruction_json in instructions_json:
		instruction = Instruction(instruction_json['name'])
		instructions += [ instruction ]

		for operand_json in instruction_json['encoding']:
			if operand_json['type'] == 'operand':
				instruction.add_match(OperandMatch(
					operand_json['name'],
					operand_json['bits_top'],
					operand_json['bits_bottom']
				))
			elif operand_json['type'] == 'pattern':
				instruction.add_match(BinaryMatch(
					operand_json['pattern']
				))
			else:
				raise ValueError(f'Invalid match type {operand_json["type"]}')

		if 'syntax' in instruction_json:
			instruction.syntax = instruction_json['syntax']

	return instructions


def write_json_file(file, instructions):
	instructions_json = []
	for instruction in instructions:
		instructions_json += [{
			'name': instruction.get_name(),
			'operands': [
				{
					'name': operand.get_name(),
					'bits': operand.get_bits(),
					'bits_top': operand.get_bits_top(),
					'bits_bottom': operand.get_bits_bottom(),
				} for operand in instruction.get_operands()],
			'encoding': [
				{
					OperandMatch: lambda: {
						'type': 'operand',
						'name': pattern.get_name(),
						'bits': pattern.get_bits(),
						'bits_top': pattern.get_bits_top(),
						'bits_bottom': pattern.get_bits_bottom(),
					},
					BinaryMatch: lambda: {
						'type': 'pattern',
						'bits': pattern.get_bits(),
						'pattern': pattern.get_pattern()
					},
				}.get(type(pattern))() for pattern in instruction.get_matches()
			],
			'syntax': instruction.syntax
		}]

	with open(file, 'wt', encoding='UTF-8') as file:
		file.write(json.dumps(instructions_json, indent=4))
