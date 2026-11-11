#!/bin/env python3

import argparse
import itertools
import jinja2
import os

from instruction_reader import load_json_file

if __name__ == '__main__':
	parser = argparse.ArgumentParser()
	parser.add_argument('--output-file', required=True, action='store', dest='output_file', help='Output file for the VCM')
	parser.add_argument('--output-module', required=True, action='store', dest='output_module', help='Top level Verilog module name for the VCM')
	parser.add_argument('--input-files', required=True, nargs='+', dest='source_files', help='Source instruction JSON file(s)')
	arguments = parser.parse_args()

	instructions = list(itertools.chain(*[load_json_file(file) for file in arguments.source_files]))
	for instruction in instructions:
		print(instruction)

	# Now render the parsed instruction specification as Verilog code
	environment = jinja2.Environment(loader=jinja2.FileSystemLoader(searchpath=os.path.dirname(os.path.abspath(__file__))))
	environment.trim_blocks = False
	environment.lstrip_blocks = False

	operands = list(sorted(set([operand.get_name() for instruction in instructions for operand in instruction.get_operands()])))
	operands_fixed_size = [(name, bits) for name, bits in [('rd', 5), ('rs1', 5), ('rs2', 5), ('csr', 12), ('rm', 3), ('aq', 1), ('rl', 1)] if name in operands]

	top_module_template = environment.get_template('vcm_top_verilog.jinja2')
	with open(arguments.output_file, 'wt', encoding='UTF-8') as output:
		output.write(top_module_template.render(
			vcm_name=arguments.output_module,
			instructions=instructions,
			operands=operands,
			constant_operand_names=[name for name, bits in operands_fixed_size],
			constant_operands=operands_fixed_size
		))
