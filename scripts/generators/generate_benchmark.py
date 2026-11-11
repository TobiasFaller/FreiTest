import argparse
import itertools
import json
import re
import sys
import math

from typing import Dict, List, Tuple, Any, Union
from random import Random
from dataclasses import dataclass, field
from jinja2 import Environment, DictLoader

@dataclass
class Gate:
	name: str
	type: str
	num_inputs: int = 1
	num_clocks: int = 1
	num_resets: int = 1
	num_outputs: int = 1
	inputs: List[str] = None
	clocks: List[str] = None
	resets: List[str] = None
	outputs: List[str] = None

	def __init__(self, name, type, cell):
		self.name = name
		self.type = type
		self.num_inputs = len(cell.get('inputs', []))
		self.num_clocks = len(cell.get('clocks', []))
		self.num_resets = len(cell.get('resets', []))
		self.num_outputs = len(cell.get('outputs', []))
		self.inputs = [None] * len(cell.get('inputs', []))
		self.clocks = [None] * len(cell.get('clocks', []))
		self.resets = [None] * len(cell.get('resets', []))
		self.outputs = [None] * len(cell.get('outputs', []))

@dataclass
class Circuit:
	name: str
	inputs: List[str]
	outputs: List[str]
	clocks: List[str]
	resets: List[str]
	wires: List[str]
	assignments: Dict[str, str]
	gates: Dict[str, Gate]

	type_sequential: List[Tuple[str, str]]
	type_combinational: List[Tuple[str, str]]
	type_submodule: List[Tuple[str, str]]

def generate_circuit(rand: Random, config: Dict[str, int]) -> Circuit:
	sequential = list(config.get('cells_sequential').items())
	combinational = list(config.get('cells_combinational').items())
	submodule = list(config.get('cells_submodule').items())

	type_sequential = []
	type_combinational = []
	type_submodule = []

	primary_clock_names = []
	primary_reset_names = []

	# Create flip-flops with clock connections
	gates: Dict[str, Gate] = {}
	for ff_index in range(config['num_flipflops']):
		gate_name = f'ff{ff_index+1}'
		gate_type, cell = rand.choice(sequential)
		gates[gate_name] = Gate(gate_name, gate_type, cell)
		type_sequential += [(gate_name, gate_type)]
		for index, clock in enumerate(cell.get('clocks', [])):
			clock = random.choice(config.get('names_clocks', ['CLOCK']))
			if clock not in primary_clock_names:
				primary_clock_names += [ clock ]
			gates[gate_name].clocks[index] = clock
		for index, reset in enumerate(cell.get('resets', [])):
			reset = random.choice(config.get('names_resets', ['RESET']))
			if reset not in primary_reset_names:
				primary_reset_names += [ reset ]
			gates[gate_name].resets[index] = reset

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
	secondary_outputs_choices = primary_inputs + secondary_inputs[0 : len(secondary_outputs) - len(secondary_inputs)]
	if len(secondary_inputs) > len(secondary_inputs_choices):
		secondary_inputs_choices += itertools.islice(itertools.cycle([None]),
			len(secondary_inputs) - len(secondary_inputs_choices))
	if len(secondary_outputs) > len(secondary_outputs_choices):
		secondary_outputs_choices += itertools.islice(itertools.cycle(secondary_outputs_choices),
			len(secondary_outputs) - len(secondary_outputs_choices))
	secondary_inputs_chosen = rand.sample(secondary_inputs_choices, k=len(secondary_inputs))
	secondary_outputs_chosen = rand.sample(secondary_outputs_choices, k=len(secondary_outputs))

	# Create initial connections from (primary / secondary) inputs to (primary / secondary)  outputs
	insertion_points: List[List[Tuple[str, str, int]]] = [] # sink
	connections: Dict[Tuple[str, str, int], Tuple[str, str, int]] = {} # sink: source
	for input, output in zip(secondary_inputs, secondary_inputs_chosen):
		if output is not None:
			connections[output] = input
	for output, input in zip(secondary_outputs, secondary_outputs_chosen):
		connections[output] = input
	for output in primary_outputs:
		if output not in connections:
			connections[output] = rand.choice(primary_inputs + secondary_inputs)

	# One source can drive multiple outputs at once -> Group them to single insertion points
	sortkey = lambda item: (item[1] if item[1][1] is not None else (item[1][0], "", item[1][2]))
	connections_by_source = itertools.groupby(sorted(connections.items(), key=sortkey), key=sortkey)
	insertion_points.extend([[key for key, value in elem] for _, elem in connections_by_source])

	def _connect(cell):
		output_indices = rand.sample(range(len(insertion_points)), k=min(len(cell['outputs']), len(insertion_points)))
		outputs = [insertion_points[index] for index in output_indices]
		inputs = [connections[out[0]] for out in outputs][0 : len(cell['inputs'])]
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
			for out in output:
				connections[out] = ('out', gate_name, index)
		for index, input in enumerate(inputs):
			connections[('in', gate_name, index)] = input
			insertion_points.append([('in', gate_name, index)])

	# Insert gates and submodules into paths
	choices = ['gate' for i in range(config['num_gates'])]
	choices += ['module' for i in range(config['num_submodules'])]
	rand.shuffle(choices)

	gate_index = 0
	module_index = 0
	for choice in choices:
		if choice == 'gate':
			gate_name = f'g{gate_index+1}'
			gate_type, cell = rand.choice(combinational)
			gates[gate_name] = Gate(gate_name, gate_type, cell)
			type_combinational += [(gate_name, gate_type)]
			gate_index += 1
		elif choice == 'module':
			gate_name = f'm{module_index+1}'
			gate_type, cell = rand.choice(submodule)
			gates[gate_name] = Gate(gate_name, gate_type, cell)
			type_submodule += [(gate_name, gate_type)]
			for index, clock in enumerate(cell.get('clocks', [])):
				clock = random.choice(config.get('names_clocks', ['CLOCK']))
				if clock not in primary_clock_names:
					primary_clock_names += [ clock ]
				gates[gate_name].clocks[index] = clock
			for index, reset in enumerate(cell.get('resets', [])):
				reset = random.choice(config.get('names_resets', ['RESET']))
				if reset not in primary_reset_names:
					primary_reset_names += [ reset ]
				gates[gate_name].resets[index] = reset
			module_index += 1
		_connect(cell)

	wires = set()
	assignments = {}

	# Connect primary outputs first
	for (dtype, dgate, dindex), (stype, sgate, sindex) in connections.items():
		if dgate is not None:
			continue

		if sgate is not None and gates[sgate].outputs[sindex] is not None:
			# Gate drives multiple primary outputs => reuse the assigned name
			assignments[primary_output_names[dindex]] = gates[sgate].outputs[sindex]
		elif sgate is None:
			# Input is directly connected to an output (no gate) => use assignment
			assignments[primary_output_names[dindex]] = primary_input_names[sindex]
		else:
			gates[sgate].outputs[sindex] = primary_output_names[dindex]

	# Connect all gates
	for (dtype, dgate, dindex), (stype, sgate, sindex) in connections.items():
		if dgate is None:
			continue

		if sgate is None:
			source_name = primary_input_names[sindex]
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
		gates[dgate].inputs[dindex] = source_name

	unconnected = []
	for gate_name, gate in gates.items():
		for output in range(gate.num_outputs):
			if gate.outputs[output] is None:
				# A flip-flop has an unconnected output
				source_name = f'u{len(unconnected) + 1}'
				unconnected.append(source_name)
				wires.add(source_name)
				gate.outputs[output] = source_name

	circuit_name = config.get('name', 'benchmark')
	return Circuit(
		name=circuit_name,
		inputs=primary_input_names,
		outputs=primary_output_names,
		clocks=primary_clock_names,
		resets=primary_reset_names,
		wires=list(wires),
		assignments=assignments,
		gates=gates,
		type_sequential=type_sequential,
		type_combinational=type_combinational,
		type_submodule=type_submodule
	)


def _convert_circuit(circuit: Circuit, cells: List[Any]) -> Dict[str, Any]:
	ports = [('input', port[0], True, port[1], port[2]) if isinstance(port, tuple) else ('input', port, False, 0, 0) for port in circuit.inputs]
	ports += [('input', port[0], True, port[1], port[2]) if isinstance(port, tuple) else ('input', port, False, 0, 0) for port in circuit.clocks]
	ports += [('input', port[0], True, port[1], port[2]) if isinstance(port, tuple) else ('input', port, False, 0, 0) for port in circuit.resets]
	ports += [('output', port[0], True, port[1], port[2]) if isinstance(port, tuple) else ('output', port, False, 0, 0) for port in circuit.outputs]

	single_wires = [wire for wire in circuit.wires if not isinstance(wire, tuple)]
	bus_wires = [wire for wire in circuit.wires if isinstance(wire, tuple)]

	gates = []
	for gate in circuit.gates.values():
		gate_ports = list(zip(
			itertools.chain(*[cells[gate.type].get(type, []) for type in ['inputs', 'clocks', 'resets', 'outputs']]),
			itertools.chain(*[gate.inputs, gate.clocks, gate.resets, gate.outputs])
		))
		gate_ports = [(port, isinstance(connections, list), connections) for port, connections in gate_ports]
		gates += [(gate.name, gate.type, gate_ports)]

	assignments = []
	for left, right in circuit.assignments.items():
		assignments += [(isinstance(left, tuple), left, right)]

	return {
		'name': circuit.name,
		'ports': ports,
		'single_wires': single_wires,
		'bus_wires': bus_wires,
		'gates': gates,
		'assignments': assignments,
	}

def _replace_names(circuit: Circuit, forward_map: Dict[str, str]):
	for gate_name, gate in circuit.gates.items():
		gate.inputs = [(forward_map[name] if name in forward_map else name) for name in gate.inputs]
		gate.clocks = [(forward_map[name] if name in forward_map else name) for name in gate.clocks]
		gate.resets = [(forward_map[name] if name in forward_map else name) for name in gate.resets]
		gate.outputs = [(forward_map[name] if name in forward_map else name) for name in gate.outputs]

	assignments = {}
	for right, left in circuit.assignments.items():
		right = forward_map[right] if right in forward_map else right
		left = forward_map[left] if left in forward_map else left
		assignments[right] = left
	circuit.assignments = assignments

	return circuit

def _replace_port_names(circuit_map: Dict[str,  Circuit], port_maps: Dict[str, Dict[str, str]], reverse_port_maps: Dict[str, Dict[str, List[str]]], old_cell_map: Dict[str, Dict[str, Any]]) -> Dict[str, Circuit]:
	for circuit_name, circuit in circuit_map.items():
		for gate_name, gate in circuit.gates.items():
			if gate.type not in port_maps:
				continue

			subcircuit = circuit_map[gate.type]
			port_map = port_maps[gate.type]
			reverse_port_map = reverse_port_maps[gate.type]

			old_inputs = dict(zip(old_cell_map[gate.type]['inputs'], gate.inputs))
			old_outputs = dict(zip(old_cell_map[gate.type]['outputs'], gate.outputs))

			gate.inputs = []
			gate.outputs = []
			for input in subcircuit.inputs:
				input = input[0] if isinstance(input, tuple) else input
				target = reverse_port_map[input]
				if isinstance(target, tuple):
					gate.inputs.append([old_inputs[t] for t in target[2]])
				else:
					gate.inputs.append(old_inputs[target])

			for output in subcircuit.outputs:
				output = output[0] if isinstance(output, tuple) else output
				target = reverse_port_map[output]
				if isinstance(target, tuple):
					gate.outputs.append([old_outputs[t] for t in target[2]])
				else:
					gate.outputs.append(old_outputs[target])

	return circuit_map


def _create_wire_busses(rand: Random, circuit: Circuit, config: Dict[str, Any]) -> Circuit:
	wires = []
	busses = [[] for i in range(config.get('num_busses', 0))]
	for wire in circuit.wires:
		assignment = random.randint(0, len(busses))
		if assignment == 0:
			wires.append(wire)
		else:
			busses[assignment - 1].append(wire)

	wire_map = { }
	reverse_wire_map = { }
	circuit.wires = []
	for index, wire in enumerate(wires):
		name = f'w{index + 1:04}'
		circuit.wires.append(name)
		wire_map[wire] = name
		reverse_wire_map[name] = wire

	wire_index = 0
	for bus in busses:
		if len(bus) == 0:
			continue

		name = f'bus{wire_index + 1:04}'
		bottom = rand.randint(0, 10)
		top = bottom + len(bus) - 1
		if rand.choice([0, 1]):
			top, bottom = bottom, top

		circuit.wires.append((name, top, bottom))
		for index, wire in enumerate(bus):
			next_index = bottom + int(math.copysign(index, top - bottom))
			assert next_index >= min(top, bottom) and next_index <= max(top, bottom)
			wire_map[wire] = f'{name} [{next_index}]'
		reverse_wire_map[name] = (top, bottom, bus)

		wire_index += 1

	return _replace_names(circuit, wire_map), wire_map, reverse_wire_map


def _create_port_busses(rand: Random, circuit: Circuit, config: Dict[str, Any]) -> Tuple[Circuit, Dict[str, List[str]]]:
	inputs = []
	outputs = []
	input_busses = []
	output_busses = []
	for port_type in [rand.choice(['input', 'output']) for i in range(config.get('num_busses', 0))]:
		if port_type == 'input':
			input_busses.append([])
		elif port_type == 'output':
			output_busses.append([])

	for port in circuit.inputs:
		assignment = random.randint(0, len(input_busses))
		if assignment == 0:
			inputs.append(port)
		else:
			input_busses[assignment - 1].append(port)

	for port in circuit.outputs:
		assignment = random.randint(0, len(output_busses))
		if assignment == 0:
			outputs.append(port)
		else:
			output_busses[assignment - 1].append(port)

	port_map = { }
	reverse_port_map = { }
	circuit.inputs = []
	circuit.outputs = []
	for index, input in enumerate(inputs):
		name = f'IN{index+1}'
		circuit.inputs.append(name)
		port_map[input] = name
		reverse_port_map[name] = input
	for index, output in enumerate(outputs):
		name = f'OUT{index+1}'
		circuit.outputs.append(name)
		port_map[output] = name
		reverse_port_map[name] = output

	bus_index = 0
	for bus in input_busses:
		if len(bus) == 0:
			continue

		name = f'INB{bus_index + 1}'
		bottom = rand.randint(0, 10)
		top = bottom + len(bus) - 1
		if rand.choice([0, 1]):
			top, bottom = bottom, top

		circuit.inputs.append((name, top, bottom))
		for index, input in enumerate(bus):
			next_index = bottom + int(math.copysign(index, top - bottom))
			assert next_index >= min(top, bottom) and next_index <= max(top, bottom)
			port_map[input] = f'{name} [{next_index}]'
		reverse_port_map[name] = (top, bottom, bus)

		bus_index += 1

	bus_index = 0
	for bus in output_busses:
		if len(bus) == 0:
			continue

		name = f'OUTB{bus_index + 1}'
		bottom = rand.randint(0, 10)
		top = bottom + len(bus) - 1
		if rand.choice([0, 1]):
			top, bottom = bottom, top

		circuit.outputs.append((name, top, bottom))
		for index, output in enumerate(bus):
			next_index = bottom + int(math.copysign(index, top - bottom))
			assert next_index >= min(top, bottom) and next_index <= max(top, bottom)
			port_map[output] = f'{name} [{next_index}]'
		reverse_port_map[name] = (top, bottom, bus)

		bus_index += 1

	return _replace_names(circuit, port_map), port_map, reverse_port_map


def _combine_busses(elements: Union[str, List[str]], forward_map: Dict[str, List[str]], backward_map: Dict[str, Any]) -> Union[List[str], str]:
	if isinstance(elements, str):
		return elements

	regex_bus = re.compile(r'(.*) \[(\d+)\]')
	index = 0
	while index < len(elements):
		bus_name = None
		bus_index = None
		bus_size = None
		bus_top = None
		bus_bottom = None
		start_index = index

		while index < len(elements):
			match = regex_bus.match(elements[index])
			if not match:
				index += 1
				break

			if bus_name is None:
				bus_name = match.group(1)
				if bus_name not in backward_map:
					index += 1
					break

				bus_top, bus_bottom, _ = backward_map[bus_name]
				bus_index = int(match.group(2))
				if bus_index != bus_top:
					index += 1
					break

			elif bus_name != match.group(1):
				index += 1
				break

			elif bus_index != int(match.group(2)):
				index += 1
				break

			# Use if here because top == bottom for some busses
			if bus_index == bus_bottom:
				elements = elements[0:start_index] + [bus_name] + elements[index + 1:]
				index = start_index + 1
				break

			bus_index += int(math.copysign(1, bus_bottom - bus_top))
			index += 1
			continue

		if len(elements) == 1:
			return elements[0]
		else:
			return elements


def _get_ports(circuit: Circuit, type: str):
	for port in getattr(circuit, type):
		if isinstance(port, tuple):
			name, top, bottom = port
			sign = int(math.copysign(1, bottom - top))
			for index in range(top, bottom + sign, sign):
				yield f'{name} [{index}]'
		else:
			yield port

def _get_full_port(port):
	if isinstance(port, tuple):
		return {
			'name': port[0],
			'top': port[1],
			'bottom': port[2],
		}
	else:
		return {
			'name': port,
			'top': 0,
			'bottom': 0,
		}

TEMPLATE = """
module {{name}}(
{% for (type, name, is_bus, top, bottom) in ports %}
{% if is_bus %}
	{{type}} [{{top}}:{{bottom}}] {{name}}
{%- else %}
	{{type}} {{name}}
{%- endif %}
{% if not loop.last %},{% endif +%}
{% endfor %}
);
{% if single_wires|length > 0 %}
	wire {% for name in single_wires %}{{name}}{% if not loop.last %}, {% if not loop.index % 10 +%}\n\t\t{%+ endif %}{% endif %}{% endfor %};
{% endif %}
{% for (name, top, bottom) in bus_wires %}
	wire [{{top}}:{{bottom}}] {{name}};
{% endfor %}

{% for (name, type, ports) in gates %}
	{{type}} {{name}} (
{%- for (port, is_bus, connections) in ports %}
{% if is_bus -%}
		.{{port}}({{ '{' }}
{%- for connection in connections -%}
			{{connection}}{% if not loop.last +%}, {%+ endif %}
{%- endfor -%}
		{{ '}' }}){% if not loop.last %}, {% endif %}
{% else -%}
		.{{port}}({{connections}}){% if not loop.last %}, {% endif %}
{% endif %}
{% endfor -%}
	);
{% endfor %}

{% for (is_bus, left, right) in assignments %}
{% if is_bus %}
	assign {{ '{' }}
{%- for connection in left -%}
		{{connection}}
{%- endfor -%}
	{{ '}' }} = {{ '{' }}
{%- for connection in right -%}
		{{connection}}
{%- endfor -%}
	{{ '}' }};
{% else %}
	assign {{left}} = {{right}};
{% endif %}
{% endfor %}
endmodule
"""

environment = Environment(loader=DictLoader({ 'circuit.jinja2': TEMPLATE }))
environment.trim_blocks = True
environment.lstrip_blocks = True

if __name__ == '__main__':
	parser = argparse.ArgumentParser(description='Generate Verilog benchmark circuits.')
	parser.add_argument('--seed', dest='seed', type=int, default=0, help='The seed for the random generator')
	parser.add_argument('--library-path', dest='library_path', required=True, help='The json file describing the cells of the library')
	parser.add_argument('--library-exclude', dest='library_exclude', default='', required=False, help='A regex that when matched excludes the cell from the library')
	parser.add_argument('--circuit-path', dest='circuit_path', required=True, help='The output file to write the circuit to')
	parser.add_argument('--circuit-name', dest='circuit_name', default='benchmark', help='The top-level name for the circuit')
	parser.add_argument('--circuit-inputs', dest='circuit_inputs', type=int, default=10, help='The number of inputs for the circuit')
	parser.add_argument('--circuit-outputs', dest='circuit_outputs', type=int, default=5, help='The number of outputs for the circuit')
	parser.add_argument('--circuit-flipflops', dest='circuit_flipflops', type=int, default=10, help='The number of flip-flops for the circuit')
	parser.add_argument('--circuit-gates', dest='circuit_gates', type=int, default=50, help='The number of gates for the circuit')
	parser.add_argument('--circuit-submodules', dest='circuit_submodules', type=int, default=0, help='The number of submodules for the circuit')
	parser.add_argument('--circuit-nosubmodules', dest='circuit_nosubmodules', type=int, default=0, help='The number of modules that don\'t have submodules')
	parser.add_argument('--circuit-modules', dest='circuit_modules', type=int, default=1, help='The number of modules with equal size to generate')
	parser.add_argument('--circuit-wire-busses', dest='circuit_wire_busses', type=int, default=0, help='Number of busses to create from wires')
	parser.add_argument('--circuit-port-busses', dest='circuit_port_busses', type=int, default=0, help='Number of busses to create from ports')
	parser.add_argument('--debug-remove-comb', dest='debug_remove_comb', action='append', default=[], help='The modules to remove all combinatorial gates of')
	parser.add_argument('--debug-remove-seq', dest='debug_remove_seq', action='append', default=[], help='The modules to remove all sequential gates of')
	parser.add_argument('--debug-remove-out', dest='debug_remove_out', action='append', default=[], help='The modules to remove all outputs of')
	parser.add_argument('--debug-remove-gate', dest='debug_remove_gate', action='append', default=[], help='The modules to remove all gates')
	parser.add_argument('--debug-remove-subout', dest='debug_remove_subout', action='append', default=[], help='The modules to remove all driven outputs of')
	parser.add_argument('--debug-remove-subin', dest='debug_remove_subin', action='append', default=[], help='The modules to remove all driven outputs of')
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

	def is_cell_allowed(cell):
		if args.library_exclude == '':
			return True
		return not re.match(args.library_exclude, cell[0])

	library['cells'] = dict(filter(is_cell_allowed, library['cells'].items()))

	cells_combinational = dict([(name, cell) for name, cell in library['cells'].items() if cell.get('combinational', False)])
	cells_sequential = dict([(name, cell) for name, cell in library['cells'].items() if cell.get('sequential', False)])

	random = Random(args.seed)
	template = environment.get_template('circuit.jinja2')

	meta = {}
	meta['command'] = sys.argv

	if args.circuit_path is not None:
		meta['modules'] = {}

		circuits = {}
		cells_submodule = {}
		for module_index in range(args.circuit_modules):
			if  module_index == args.circuit_modules - 1:
				circuit_name = args.circuit_name
			else:
				circuit_name = f'sub{module_index + 1}'

			config = {
				'name': circuit_name,
				'seed': args.seed,
				'num_inputs': args.circuit_inputs,
				'num_outputs': args.circuit_outputs,
				'num_flipflops': args.circuit_flipflops,
				'num_gates': args.circuit_gates,
				'num_submodules': 0 if module_index < args.circuit_nosubmodules else args.circuit_submodules,
				'names_clocks': ['CLOCK'],
				'names_reset': ['RESET'],
				'names_inputs': [f'IN{i+1}' for i in range(args.circuit_inputs)] ,
				'names_outputs': [f'OUT{i+1}' for i in range(args.circuit_outputs)],
				'cells_combinational': cells_combinational,
				'cells_sequential': cells_sequential,
				'cells_submodule': cells_submodule,
			}
			circuit = generate_circuit(random, config)
			circuits[circuit_name] = circuit
			cells_submodule[circuit_name] = {
				"inputs": circuit.inputs,
				"outputs": circuit.outputs,
				**({ "clocks": circuit.clocks } if len(circuit.clocks) > 0 else { }),
				**({ "resets": circuit.resets } if len(circuit.resets) > 0 else { }),
				**({ "sequential": True } if len(circuit.clocks) + len(circuit.resets) > 0 else { "combinational": True }),
			}

			circuit_meta = {
				'name': circuit_name,
				'config': config,
				'inputs': circuit.inputs,
				'outputs': circuit.outputs,
				'clocks': circuit.clocks,
				'resets': circuit.resets,
				'inputs_full': [{ 'name': input, 'top': 0, 'bottom': 0 } for input in circuit.inputs],
				'outputs_full': [{ 'name': output, 'top': 0, 'bottom': 0 } for output in circuit.outputs],
				'clocks_full': [{ 'name': clock, 'top': 0, 'bottom': 0 } for clock in circuit.clocks],
				'resets_full': [{ 'name': reset, 'top': 0, 'bottom': 0 } for reset in circuit.resets],
				'gates_combinational': circuit.type_combinational,
				'gates_sequential': circuit.type_sequential,
				'gates_submodule': circuit.type_submodule,
			}
			del circuit_meta['config']['cells_combinational']
			del circuit_meta['config']['cells_sequential']
			del circuit_meta['config']['cells_submodule']

			meta['modules'][circuit_name] = circuit_meta
			if circuit_name == args.circuit_name:
				meta['circuit'] = circuit_meta

		if args.circuit_wire_busses > 0:
			new_circuits = {}
			wire_maps = {}
			reverse_wire_maps = {}
			for circuit_name, circuit in circuits.items():
				new_circuit, wire_map, reverse_wire_map = _create_wire_busses(random, circuit, {
					'num_busses': args.circuit_wire_busses,
				})
				new_circuits[circuit_name] = new_circuit
				wire_maps[circuit_name] = wire_map
				reverse_wire_maps[circuit_name] = reverse_wire_map
			circuits = new_circuits

			for circuit_name, circuit in circuits.items():
				for gate_name, gate in circuit.gates.items():
					for index in range(len(gate.inputs)):
						gate.inputs[index] = _combine_busses(gate.inputs[index], wire_maps[circuit.name], reverse_wire_maps[circuit.name])
					for index in range(len(gate.outputs)):
						gate.outputs[index] = _combine_busses(gate.outputs[index], wire_maps[circuit.name], reverse_wire_maps[circuit.name])

				old_assignments = circuit.assignments
				circuit.assignments = { }
				for left, right in old_assignments.items():
					left = _combine_busses(left, wire_maps[circuit.name], reverse_wire_maps[circuit.name])
					right = _combine_busses(right, wire_maps[circuit.name], reverse_wire_maps[circuit.name])
					circuit.assignments[left] = right

		if args.circuit_port_busses > 0:
			new_circuits = {}
			port_maps = {}
			reverse_port_maps = {}
			for circuit_name, circuit in circuits.items():
				new_circuit, port_map, reverse_port_map = _create_port_busses(random, circuit, {
					'num_busses': args.circuit_port_busses,
				})
				new_circuits[circuit_name] = new_circuit
				port_maps[circuit_name] = port_map
				reverse_port_maps[circuit_name] = reverse_port_map
			circuits = new_circuits

			circuits = _replace_port_names(circuits, port_maps, reverse_port_maps, cells_submodule)

			for circuit_name, circuit in circuits.items():
				for gate_name, gate in circuit.gates.items():
					for index in range(len(gate.inputs)):
						gate.inputs[index] = _combine_busses(gate.inputs[index], port_maps[circuit.name], reverse_port_maps[circuit.name])
					for index in range(len(gate.outputs)):
						gate.outputs[index] = _combine_busses(gate.outputs[index], port_maps[circuit.name], reverse_port_maps[circuit.name])

				old_assignments = circuit.assignments
				circuit.assignments = { }
				for left, right in old_assignments.items():
					left = _combine_busses(left, port_maps[circuit.name], reverse_port_maps[circuit.name])
					right = _combine_busses(right, port_maps[circuit.name], reverse_port_maps[circuit.name])
					circuit.assignments[left] = right

			for circuit_name, circuit in circuits.items():
				meta['modules'][circuit_name]['inputs'] = list(_get_ports(circuit, 'inputs'))
				meta['modules'][circuit_name]['outputs'] = list(_get_ports(circuit, 'outputs'))
				meta['modules'][circuit_name]['clocks'] = list(_get_ports(circuit, 'clocks'))
				meta['modules'][circuit_name]['resets'] = list(_get_ports(circuit, 'resets'))
				meta['modules'][circuit_name]['inputs_full'] = [_get_full_port(input) for input in circuit.inputs]
				meta['modules'][circuit_name]['outputs_full'] = [_get_full_port(output) for output in circuit.outputs]
				meta['modules'][circuit_name]['clocks_full'] = [_get_full_port(clock) for clock in circuit.clocks]
				meta['modules'][circuit_name]['resets_full'] = [_get_full_port(reset) for reset in circuit.resets]

			cells_submodule = {
				name: {
					"inputs": [(input[0] if isinstance(input, tuple) else input) for input in circuit.inputs],
					"outputs": [(output[0] if isinstance(output, tuple) else output) for output in circuit.outputs],
					**({ "clocks": circuit.clocks } if len(circuit.clocks) > 0 else { }),
					**({ "resets": circuit.resets } if len(circuit.resets) > 0 else { }),
					**({ "sequential": True } if len(circuit.clocks) + len(circuit.resets) > 0 else { "combinational": True }),
				} for name, circuit in circuits.items()
			}

		for circuit_name in args.debug_remove_comb:
			if circuit_name not in circuits:
				continue

			circuit = circuits[circuit_name]
			default_input = default_input = next(_get_ports(circuit, 'inputs'))

			for gate_name, gate_type in circuit.type_combinational:
				outputs = circuit.gates[gate_name].outputs
				for output in outputs:
					circuit.assignments[output] = default_input
				del circuit.gates[gate_name]
			circuit.type_combinational = []

		for circuit_name in args.debug_remove_seq:
			if circuit_name not in circuits:
				continue

			circuit = circuits[circuit_name]
			default_input = default_input = next(_get_ports(circuit, 'inputs'))

			wire_index = 0
			for gate_name, gate_type in circuit.type_sequential:
				inputs = circuit.gates[gate_name].inputs
				outputs = circuit.gates[gate_name].outputs
				for index in range(len(inputs)):
					inputs[index] = default_input
				for index in range(len(outputs)):
					circuit.assignments[outputs[index]] = default_input

					name = f'flipflop_u{wire_index + 1:04}'
					outputs[index] = name
					circuit.wires += [name]
					wire_index += 1

		for circuit_name in args.debug_remove_out:
			if circuit_name not in circuits:
				continue

			circuit = circuits[circuit_name]
			default_input = next(_get_ports(circuit, 'inputs'))

			output_set = set(_get_ports(circuit, 'outputs'))
			for output in output_set:
				circuit.assignments[output] = default_input

			wire_index = 0
			for gate_name, gate in circuit.gates.items():
				outputs = gate.outputs
				for index in range(len(outputs)):
					if isinstance(outputs[index], str):
						if outputs[index] in output_set:
							name = f'gate_u{wire_index + 1:04}'
							outputs[index] = name
							circuit.wires += [name]
							wire_index += 1
					else:
						for i in range(len(outputs[index])):
							if outputs[index][i] in output_set:
								name = f'gate_u{wire_index + 1:04}'
								outputs[index][i] = name
								circuit.wires += [name]
								wire_index += 1

		filter_regex = re.compile(f'^(?:(?:{")|(?:".join(args.debug_remove_subout)}))$')
		for circuit_name, circuit in circuits.items():
			default_input = next(_get_ports(circuit, 'inputs'))
			wire_index = 0

			for gate_name, gate in circuit.gates.items():
				match = filter_regex.match(f'{circuit_name}/{gate_name}')
				if not match:
					continue

				gate = circuit.gates[gate_name]
				outputs = gate.outputs
				for index in range(len(outputs)):
					if isinstance(outputs[index], str):
						circuit.assignments[outputs[index]] = default_input
						name = f'sub_u{wire_index + 1:04}'
						outputs[index] = name
						circuit.wires += [name]
						wire_index += 1
					else:
						for i in range(len(outputs[index])):
							circuit.assignments[outputs[index][i]] = default_input
							name = f'sub_u{wire_index + 1:04}'
							outputs[index][i] = name
							circuit.wires += [name]
							wire_index += 1

		filter_regex = re.compile(f'^(?:(?:{")|(?:".join(args.debug_remove_subin)}))$')
		for circuit_name, circuit in circuits.items():
			default_input = next(_get_ports(circuit, 'inputs'))

			for gate_name, gate in circuit.gates.items():
				match = filter_regex.match(f'{circuit_name}/{gate_name}')
				if not match:
					continue

				inputs = gate.inputs
				for index in range(len(inputs)):
					if isinstance(inputs[index], str):
						inputs[index] = default_input
					else:
						for i in range(len(inputs[index])):
							inputs[index][i] = default_input

		filter_regex = re.compile(f'^(?:(?:{")|(?:".join(args.debug_remove_gate)}))$')
		for circuit_name, circuit in circuits.items():
			default_input = next(_get_ports(circuit, 'inputs'))
			wire_index = 0

			gates_to_remove = []
			for gate_name, gate in circuit.gates.items():
				match = filter_regex.match(f'{circuit_name}/{gate_name}')
				if not match:
					continue

				gate = circuit.gates[gate_name]
				gates_to_remove += [gate_name]
				outputs = gate.outputs
				for index in range(len(outputs)):
					if isinstance(outputs[index], str):
						circuit.assignments[outputs[index]] = default_input
					else:
						for i in range(len(outputs[index])):
							circuit.assignments[outputs[index][i]] = default_input

			for gate_name in gates_to_remove:
				if (gate_name, circuit.gates[gate_name].type) in circuit.type_combinational:
					circuit.type_combinational.remove((gate_name, circuit.gates[gate_name].type))
				if (gate_name, circuit.gates[gate_name].type) in circuit.type_sequential:
					circuit.type_sequential.remove((gate_name, circuit.gates[gate_name].type))
				if (gate_name, circuit.gates[gate_name].type) in circuit.type_submodule:
					circuit.type_submodule.remove((gate_name, circuit.gates[gate_name].type))
				del circuit.gates[gate_name]

		def _prefix(prefix: str, name: str) -> str:
			if prefix == '':
				return name
			else:
				return f'{prefix}/{name}'

		def _walk_module_for_ffs(module: str, prefix: str, ffs_out: List[str]):
			for gate_name, gate_type in circuits[module].type_sequential:
				ffs_out += [_prefix(prefix, gate_name)]
			for gate_name, gate_type in circuits[module].type_submodule:
				_walk_module_for_ffs(gate_type, _prefix(prefix, gate_name), ffs_out)

		flipflops = []
		_walk_module_for_ffs(args.circuit_name, '', flipflops)
		meta['circuit']['flipflops'] = flipflops

		cells_all = { **cells_combinational, **cells_sequential, **cells_submodule }
		circuits = [_convert_circuit(circuit, cells_all) for circuit in circuits.values()]
		with open(args.circuit_path, 'wt', encoding='UTF-8') as stream:
			for circuit in circuits:
				stream.write(template.render(circuit))
				stream.write('\n\n')


	if args.vcm_path is not None:
		miter_ports = [f'IN{i+1}_g' for i in range(args.circuit_inputs)] + [f'OUT{i+1}_g' for i in range(args.circuit_outputs)]
		config = {
			'name': args.vcm_name,
			'seed': args.seed,
			'num_inputs': args.vcm_inputs,
			'num_outputs': args.vcm_constraints,
			'num_flipflops': args.vcm_flipflops,
			'num_gates': args.vcm_gates,
			'num_submodules': 0,
			'names_clocks': ['CLOCK_g'],
			'names_resets': ['RESET_g'],
			'names_inputs': random.choices(miter_ports, k=args.vcm_inputs),
			'names_outputs': [f'CONSTRAINT{i+1}_{random.choice(["i", "c", "c", "c", "c", "c", "t"])}' for i in range(args.vcm_constraints)],
			'cells_combinational': cells_combinational,
			'cells_sequential': cells_sequential,
			'cells_submodule': { },
		}
		circuit = generate_circuit(random, config)

		cells_all = { **cells_combinational, **cells_sequential }
		circuit = _convert_circuit(circuit, cells_all)
		with open(args.vcm_path, 'wt', encoding='UTF-8') as stream:
			stream.write(template.render(circuit))

		del config['cells_combinational']
		del config['cells_sequential']
		del config['cells_submodule']
		meta['vcm'] = config

		meta['cells_combinational'] = cells_combinational
		meta['cells_sequential'] = cells_sequential
		meta['cells_submodule'] = cells_submodule

	if args.meta_path is not None:
		with open(args.meta_path, 'wt', encoding='UTF-8') as stream:
			json.dump(meta, stream, indent=4)
