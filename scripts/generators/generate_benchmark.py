import argparse
import json
import itertools

from typing import Dict, List, Tuple
from random import Random
from dataclasses import dataclass
from jinja2 import Environment, DictLoader

@dataclass
class Gate:
	name: str
	type: str
	num_inputs: int = 1
	num_clocks: int = 1
	num_outputs: int = 1
	inputs: List[str] = None
	clocks: List[str] = None
	outputs: List[str] = None

	def __init__(self, name, type, cell):
		self.name = name
		self.type = type
		self.num_inputs = len(cell.get('inputs', []))
		self.num_clocks = len(cell.get('clocks', []))
		self.num_outputs = len(cell.get('outputs', []))
		self.inputs = [None] * len(cell.get('inputs', []))
		self.clocks = [None] * len(cell.get('clocks', []))
		self.outputs = [None] * len(cell.get('outputs', []))

@dataclass
class Circuit:
	inputs: List[str]
	outputs: List[str]
	wires: List[str]
	assignments: Dict[str, str]
	gates: Dict[str, Gate]

def generate_circuit(rand: Random, config: Dict[str, int]) -> Circuit:
	sequential = list(config.get('cells_sequential').items())
	combinational = list(config.get('cells_combinational').items())

	# Create flip-flops with clock connections
	gates: Dict[str, Gate] = {}
	for ff_index in range(config['num_flipflops']):
		gate_name = f'ff{ff_index+1}'
		gate_type, cell = rand.choice(sequential)
		gates[gate_name] = Gate(gate_name, gate_type, cell)
		for index, clock in enumerate(cell['clocks']):
			gates[gate_name].clocks[index] = config.get('names_clock', 'CLOCK')

	primary_input_names = config.get('names_inputs', [f'IN{i+1}' for i in range(config['num_inputs'])])
	primary_output_names = config.get('names_outputs', [f'OUT{i+1}' for i in range(config['num_outputs'])])
	secondary_input_names = {
		ff_name: [f'{ff_name}_{port_name}' for port_name in config['cells_sequential'][gate.type]['outputs']]
		for ff_name, gate in gates.items()
	}
	secondary_output_names = {
		ff_name: [f'{ff_name}_{port_name}' for port_name in config['cells_sequential'][gate.type]['inputs']]
		for ff_name, gate in gates.items()
	}

	primary_inputs = [('out', None, index) for index in range(len(primary_input_names))]
	primary_outputs = [('in', None, index) for index in range(len(primary_output_names))]
	secondary_inputs = [('out', name, index) for name, gate in gates.items() for index in range(len(gate.outputs))]
	secondary_outputs = [('in', name, index) for name, gate in gates.items() for index in range(len(gate.inputs))]

	secondary_inputs_choices = primary_outputs + secondary_outputs[0 : len(secondary_inputs) - len(primary_outputs)]
	secondary_outputs_choices = primary_inputs + secondary_inputs[0:len(secondary_outputs)-len(secondary_inputs)]
	if len(secondary_inputs) > len(secondary_inputs_choices):
		secondary_inputs_choices += itertools.islice(itertools.cycle([None]),
			len(secondary_inputs) - len(secondary_inputs_choices))
	if len(secondary_outputs) > len(secondary_outputs_choices):
		secondary_outputs_choices += itertools.islice(itertools.cycle(secondary_outputs_choices),
			len(secondary_outputs) - len(secondary_outputs_choices))
	secondary_inputs_chosen = rand.sample(secondary_inputs_choices, k=len(secondary_inputs))
	secondary_outputs_chosen = rand.sample(secondary_outputs_choices, k=len(secondary_outputs))

	# Create initial connections from (primary / secondary) inputs to (primary / secondary)  outputs
	insertion_points: List[Tuple[str, str, int]] = [] # sink
	connections: Dict[Tuple[str, str, int], Tuple[str, str, int]] = {} # sink: source
	for input, output in zip(secondary_inputs, secondary_inputs_chosen):
		if output is not None:
			connections[output] = input
	for output, input in zip(secondary_outputs, secondary_outputs_chosen):
		connections[output] = input
	for output in primary_outputs:
		if output not in connections:
			connections[output] = rand.choice(primary_inputs + secondary_inputs)
	insertion_points.extend(connections.keys())

	# Insert gates into paths
	for gate_index in range(config['num_gates']):
		gate_name = f'g{gate_index+1}'
		gate_type, cell = rand.choice(combinational)
		gates[gate_name] = Gate(gate_name, gate_type, cell)

		output_indices = rand.choices(range(len(insertion_points)), k=len(cell['outputs']))
		outputs = [insertion_points[index] for index in output_indices]
		inputs = [connections[out] for out in outputs][0 : len(cell['inputs'])]
		if len(inputs) < len(cell['inputs']):
			# Additional side inputs for most gates that have more inputs than outputs
			inputs += rand.choices(primary_inputs + secondary_inputs, k=len(cell['inputs']) - len(inputs))
		rand.shuffle(inputs)

		# Faster version instead of using plain remove(output)
		output_indices.sort()
		for index in reversed(output_indices):
			insertion_points[index] = insertion_points[-1]
			insertion_points.pop()

		for index, output in enumerate(outputs):
			connections[output] = ('out', gate_name, index)
		for index, input in enumerate(inputs):
			connections[('in', gate_name, index)] = input
			insertion_points.append(('in', gate_name, index))

	wires = set()
	assignments = {}
	for (dtype, dgate, dindex), (stype, sgate, sindex) in connections.items():
		if sgate is None:
			source_name = primary_input_names[sindex]
		elif dgate is None:
			source_name = primary_output_names[dindex]
		elif gates[sgate].outputs[sindex] is not None:
			# Already connected from the other side => reuse the assigned name
			source_name = gates[sgate].outputs[sindex]
		elif gates[sgate].name in secondary_input_names:
			source_name = secondary_input_names[gates[sgate].name][sindex]
			wires.add(source_name)
		elif gates[dgate].name in secondary_output_names:
			source_name = secondary_output_names[gates[dgate].name][dindex]
			wires.add(source_name)
		else:
			# No name has been found => assign a new one
			source_name = f'w{len(wires) + 1:04}'
			wires.add(source_name)

		if sgate is not None:
			gates[sgate].outputs[sindex] = source_name
		if dgate is not None:
			gates[dgate].inputs[dindex] = source_name
		if sgate is None and dgate is None:
			# Input is directly connected to an output (no gate) => use assignment
			assignments[primary_output_names[dindex]] = source_name

	unconnected = []
	for gate_name, gate in gates.items():
		for output in range(gate.num_outputs):
			if gate.outputs[output] is None:
				# A flip-flop has an unconnected output
				source_name = f'u{len(unconnected) + 1}'
				unconnected.append(source_name)
				wires.add(source_name)
				gate.outputs[output] = source_name

	if config['num_flipflops'] > 0:
		primary_input_names.append(config.get('names_clock', 'CLOCK'))
	return Circuit(primary_input_names, primary_output_names, list(wires), assignments, gates)


TEMPLATE = """module {{name}}(
{%- for input in inputs %}
	input {{input}},
{%- endfor %}
{%- for output in outputs %}
	output {{output}}{% if not loop.last %},{% endif %}
{%- endfor %}
);
{%- if wires %}
	wire {% for wire in wires %}{{wire}}{% if not loop.last %}, {% if not loop.index % 10 %}\n\t\t{% endif %}{% endif %}{% endfor %};
{%- endif %}
{%- for gate_name, gate in gates.items() %}
	{{gate.type}} {{gate.name}} (
		{%- for port, connection in cell_ports(gate) -%}
			.{{port}}({{connection}}){% if not loop.last %}, {% endif %}
		{%- endfor -%}
	);
{%- endfor %}
{%- for assignment, value in assignments.items() %}
	assign {{assignment}} = {{value}};
{%- endfor %}
endmodule
"""

environment = Environment(loader=DictLoader({ 'circuit.jinja2': TEMPLATE }))
environment.trim_blocks = False
environment.lstrip_blocks = False

if __name__ == '__main__':
	parser = argparse.ArgumentParser(description='Generate Verilog benchmark circuits.')
	parser.add_argument('--seed', dest='seed', type=int, default=0, help='The seed for the random generator')
	parser.add_argument('--library-path', dest='library_path', required=True, help='The json file describing the cells of the library')
	parser.add_argument('--circuit-path', dest='circuit_path', required=True, help='The output file to write the circuit to')
	parser.add_argument('--circuit-name', dest='circuit_name', default='benchmark', help='The top-level name for the circuit')
	parser.add_argument('--circuit-inputs', dest='circuit_inputs', type=int, default=10, help='The number of inputs for the circuit')
	parser.add_argument('--circuit-outputs', dest='circuit_outputs', type=int, default=5, help='The number of outputs for the circuit')
	parser.add_argument('--circuit-flipflops', dest='circuit_flipflops', type=int, default=10, help='The number of flip-flops for the circuit')
	parser.add_argument('--circuit-gates', dest='circuit_gates', type=int, default=50, help='The number of gates for the circuit')
	parser.add_argument('--vcm-path', dest='vcm_path', help='The output file to write the VCM to')
	parser.add_argument('--vcm-name', dest='vcm_name', default='benchmark', help='The top-level name for the VCM')
	parser.add_argument('--vcm-inputs', dest='vcm_inputs', type=int, default=3, help='The number of constraints for the VCM')
	parser.add_argument('--vcm-constraints', dest='vcm_constraints', type=int, default=3, help='The number of constraints for the VCM')
	parser.add_argument('--vcm-flipflops', dest='vcm_flipflops', type=int, default=2, help='The number of flip-flops for the VCM')
	parser.add_argument('--vcm-gates', dest='vcm_gates', type=int, default=10, help='The number of gates for the VCM')
	parser.add_argument('--meta-path', dest='meta_path', default=None, help='The output file to write the metadata of circuit and VCM to')
	args = parser.parse_args()

	with open(args.library_path, 'rt', encoding='utf-8') as stream:
		library = json.load(stream)

	cells_combinational = dict([(name, cell) for name, cell in library['cells'].items() if cell.get('combinational', False)])
	cells_sequential = dict([(name, cell) for name, cell in library['cells'].items() if cell.get('sequential', False)])

	def get_cell_ports(gate):
		return zip(
			list(itertools.chain(*[
				library['cells'][gate.type].get(type, [])
				for type in ['inputs', 'clocks', 'outputs']
			])),
			gate.inputs + gate.clocks + gate.outputs
		)

	random = Random(args.seed)
	template = environment.get_template('circuit.jinja2')

	meta = {}

	if args.circuit_path is not None:
		config = {
			'seed': args.seed,
			'num_inputs': args.circuit_inputs,
			'num_outputs': args.circuit_outputs,
			'num_flipflops': args.circuit_flipflops,
			'num_gates': args.circuit_gates,
			'names_clock': 'CLOCK',
			'names_inputs': [f'IN{i+1}' for i in range(args.circuit_inputs)] ,
			'names_outputs': [f'OUT{i+1}' for i in range(args.circuit_outputs)],
			'cells_combinational': cells_combinational,
			'cells_sequential': cells_sequential,
		}
		circuit = generate_circuit(random, config)
		with open(args.circuit_path, 'wt', encoding='UTF-8') as stream:
			stream.write(template.render({
				'name': args.circuit_name,
				'cells': library['cells'],
				'cell_ports': get_cell_ports,
				**circuit.__dict__
			}))
		meta['circuit'] = config

	if args.vcm_path is not None:
		miter_ports = [f'IN{i+1}_g' for i in range(args.circuit_inputs)] + [f'OUT{i+1}_g' for i in range(args.circuit_outputs)]
		config = {
			'seed': args.seed,
			'num_inputs': args.vcm_inputs,
			'num_outputs': args.vcm_constraints,
			'num_flipflops': args.vcm_flipflops,
			'num_gates': args.vcm_gates,
			'names_clock': 'CLOCK_g',
			'names_inputs': random.choices(miter_ports, k=args.vcm_inputs),
			'names_outputs': [f'CONSTRAINT{i+1}_{random.choice(["i", "c", "c", "c", "c", "c", "t"])}' for i in range(args.vcm_constraints)],
			'cells_combinational': cells_combinational,
			'cells_sequential': cells_sequential,
		}
		circuit = generate_circuit(random, config)
		with open(args.vcm_path, 'wt', encoding='UTF-8') as stream:
			stream.write(template.render({
				'name': args.vcm_name,
				'cells': library['cells'],
				'cell_ports': get_cell_ports,
				**circuit.__dict__
			}))
		meta['vcm'] = config

	if args.meta_path is not None:
		with open(args.meta_path, 'wt', encoding='UTF-8') as stream:
			json.dump(meta, stream, indent=4)
