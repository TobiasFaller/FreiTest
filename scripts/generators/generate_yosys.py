import argparse
import subprocess


if __name__ == '__main__':
	parser = argparse.ArgumentParser(description='Generate yosys script for processor.')
	parser.add_argument('--top-module', dest='top_module', required=True, help='The top-level module of the processor')
	parser.add_argument('--input-path', dest='input_paths', required=True, action='append', help='The list of source files for synthesis')
	parser.add_argument('--input-format', dest='input_format', choices=['verilog', 'blif'], default='verilog', help='Input format for source files (verilog, blif)')
	parser.add_argument('--input-include-path', dest='input_include_paths', required=False, default=[], action='append', help='Include directory for verilog synthesis')
	parser.add_argument('--output-circuit-path', dest='output_circuit_path', required=True, help='The target output file for the synthesized processor sources')
	parser.add_argument('--output-script-path', dest='output_script_path', required=True, help='The target script file')
	parser.add_argument('--library-path', dest='library_path', required=True, help='The cell library definition file in liberty format')
	parser.add_argument('--define', dest='defines', required=False, default=[], action='append', help='The verilog defines to use when parsing the sources')
	parser.add_argument('--define-path', dest='defines_path', required=False, help='The verilog defines source file to use when parsing the sources')
	parser.add_argument('--parameter', dest='parameters', required=False, default=[], action='append', help='The verilog parameters to use when parsing the sources')
	parser.add_argument('--parameter-path', dest='parameters_path', required=False, help='The verilog parameters source file to use when parsing the sources')
	parser.add_argument('--execute', dest='execute', required=False, default=False, action='store_true', help='Execute the generated script with yosys')
	parser.add_argument('--debug', dest='debug', required=False, default=False, action='store_true', help='Enable debug output of YoSys')
	args = parser.parse_args()

	debug = 'debug ' if args.debug else ''

	defines = args.defines
	if args.defines_path:
		with open(args.defines_path, 'rt', encoding='utf-8') as stream:
			for line in stream:
				line = line.strip()
				if line != "":
					defines.append(line)

	parameters = args.parameters
	if args.parameters_path:
		with open(args.parameters_path, 'rt', encoding='utf-8') as stream:
			for line in stream:
				line = line.strip()
				if line != "":
					parameters.append(line)

	with open(args.output_script_path, 'wt', encoding='UTF-8') as stream:
		for input_file in args.input_paths:
			if args.input_format == 'verilog':
				stream.write(f'{debug}read_verilog -sv {" ".join(f"-D{define}" for define in defines)} {" ".join(f"-I{path}" for path in args.input_include_paths)} {input_file}\n')
			elif args.input_format == 'blif':
				stream.write(f'{debug}read_blif {input_file}; rename -top {args.top_module}\n')
		stream.write('\n')
		stream.write(f'{debug}chparam {" ".join(f"-set {param.split('=', 1)[0]} \"{param.split('=', 1)[1]}\"" for param in parameters)} {args.top_module}\n')
		stream.write(f'{debug}hierarchy -top {args.top_module}\n')
		stream.write(f'{debug}proc\n')
		stream.write(f'{debug}flatten\n')
		stream.write(f'{debug}synth\n')
		stream.write('\n')
		stream.write(f'{debug}dfflibmap -liberty {args.library_path}\n')
		stream.write(f'{debug}abc -g -cmos3 -liberty {args.library_path}\n')
		stream.write(f'{debug}clean -purge\n')
		stream.write('\n')
		stream.write(f'{debug}write_verilog {args.output_circuit_path}\n')

	if args.execute:
		subprocess.run(f'yosys -s {args.output_script_path}', shell=True, check=True)
