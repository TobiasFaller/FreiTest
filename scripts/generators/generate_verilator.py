import argparse
import itertools
import json
import re
import math
import subprocess
import pathlib

from typing import Dict, List, Tuple, Any
from dataclasses import dataclass, field
from jinja2 import Environment, DictLoader

TEMPLATE = """
#include <cassert>
#include <string>
#include <iostream>

#include "V{{name}}.h"

auto circuit_set_flipflops(std::unique_ptr<V{{name}}>& top, const std::string& flipflops) -> void;
auto circuit_set_inputs(std::unique_ptr<V{{name}}>& top, const std::string& inputs) -> void;
auto circuit_set_outputs(std::unique_ptr<V{{name}}>& top, const std::string& outputs) -> void;
auto circuit_set_clocks(std::unique_ptr<V{{name}}>& top, uint8_t value) -> void;
auto circuit_set_resets(std::unique_ptr<V{{name}}>& top, uint8_t value) -> void;
auto circuit_get_flipflops(std::unique_ptr<V{{name}}>& top) -> std::string;
auto circuit_get_inputs(std::unique_ptr<V{{name}}>& top) -> std::string;
auto circuit_get_outputs(std::unique_ptr<V{{name}}>& top) -> std::string;

auto parse_line(std::istream& stream, std::string& assignment, size_t& timeframe) -> std::string;
auto write_line(std::ostream& stream, const std::string& type, const std::string& assignment, size_t timeframe) -> void;

auto main(int argc, char** argv, char** envp) -> int {
    Verilated::debug(0);

    auto context { std::make_unique<VerilatedContext>() };
    auto top { std::make_unique<V{{name}}>(context.get()) };
    // context->commandArgs(argc, argv);

    std::string type;
    std::string assignment;
    size_t timeframe;

    circuit_set_clocks(top, 0u);
    circuit_set_resets(top, 0u);
    top->eval();

    do {
        type = parse_line(std::cin, assignment, timeframe);
        if (type == "") goto cleanup;
    } while(type != "FF");
    assert(timeframe == 0u);
    circuit_set_flipflops(top, assignment);
    top->eval();

    for (size_t expectedTimeframe { 0u }; ; expectedTimeframe++) {
        do {
            type = parse_line(std::cin, assignment, timeframe);
            if (type == "") goto cleanup;
        } while(type != "IN");
        assert(timeframe == expectedTimeframe);
        circuit_set_inputs(top, assignment);
        top->eval();

        write_line(std::cout, "FF", circuit_get_flipflops(top), expectedTimeframe);
        write_line(std::cout, "IN", circuit_get_inputs(top), expectedTimeframe);
        write_line(std::cout, "OUT", circuit_get_outputs(top), expectedTimeframe);

        circuit_set_clocks(top, 1u);
        context->timeInc(1);
        top->eval();

        circuit_set_clocks(top, 0u);
        context->timeInc(1);
        top->eval();
    }

cleanup:
    top->final();
    return 0;
}

auto parse_line(std::istream& stream, std::string& assignment, size_t& timeframe) -> std::string {
    std::string marker;
    stream >> marker >> timeframe >> assignment;
    if (!stream.good() || stream.eof()) {
        return "";
    } else if (marker == "FF" || marker == "IN" || marker == "OUT") {
        return marker;
    }
    exit(1);
}

auto write_line(std::ostream& stream, const std::string& type, const std::string& assignment, size_t timeframe) -> void {
    stream << type << " " << timeframe << " " << assignment << std::endl;
}

auto circuit_set_flipflops(std::unique_ptr<V{{name}}>& top, const std::string& flipflops) -> void {
    auto get_value = [index = 0u, &flipflops](auto const& previous, size_t shift) mutable -> auto {
        auto const value { static_cast<std::remove_reference_t<decltype(previous)>>(flipflops[index++] - '0') };
        return (previous & ~(1u << shift)) | ((value & 1u) << shift);
    };
    assert (flipflops.size() == {{flipflops|length}});
{% for flipflop, shift in flipflops %}
    top->{{flipflop|join("->")}} = get_value(top->{{flipflop|join("->")}}, {{shift}});
{% endfor %}
}

auto circuit_set_inputs(std::unique_ptr<V{{name}}>& top, const std::string& inputs) -> void {
    auto get_value = [index = 0u, &inputs](auto const& previous, size_t shift) mutable -> auto {
        auto const value { static_cast<std::remove_reference_t<decltype(previous)>>(inputs[index++] - '0') };
        return (previous & ~(1u << shift)) | ((value & 1u) << shift);
    };
    assert (inputs.size() == {{inputs|length}});
{% for input, shift in inputs %}
    top->{{input|join("->")}} = get_value(top->{{input|join("->")}}, {{shift}});
{% endfor %}
}

auto circuit_set_outputs(std::unique_ptr<V{{name}}>& top, const std::string& outputs) -> void {
    auto get_value = [index = 0u, &outputs](auto const& previous, size_t shift) mutable -> auto {
        auto const value { static_cast<std::remove_reference_t<decltype(previous)>>(outputs[index++] - '0') };
        return (previous & ~(1u << shift)) | ((value & 1u) << shift);
    };
    assert (outputs.size() == {{outputs|length}});
{% for output, shift in outputs %}
    top->{{output|join("->")}} = get_value(top->{{output|join("->")}}, {{shift}});
{% endfor %}
}

auto circuit_set_clocks(std::unique_ptr<V{{name}}>& top, uint8_t value) -> void {
    auto get_value = [&value](auto const& previous, size_t shift) mutable -> auto {
        return (previous & ~(1u << shift)) | ((value & 1u) << shift);
    };
{% for clock, shift in clocks %}
    top->{{clock|join("->")}} = get_value(top->{{clock|join("->")}}, {{shift}});
{% endfor %}
}

auto circuit_set_resets(std::unique_ptr<V{{name}}>& top, uint8_t value) -> void {
    auto get_value = [&value](auto const& previous, size_t shift) mutable -> auto {
        return (previous & ~(1u << shift)) | ((value & 1u) << shift);
    };
{% for reset, shift in resets %}
    top->{{reset|join("->")}} = get_value(top->{{reset|join("->")}}, {{shift}});
{% endfor %}
}

auto circuit_get_flipflops(std::unique_ptr<V{{name}}>& top) -> std::string {
    std::string flipflops;
    auto put_value = [&](auto const& flipflop, size_t shift) mutable -> void {
        auto const value { (flipflop >> shift) & 1u };
        flipflops += static_cast<char>(value + '0');
    };
{% for flipflop, shift in flipflops %}
    put_value(top->{{flipflop|join("->")}}, {{shift}});
{% endfor %}
    return flipflops;
}

auto circuit_get_inputs(std::unique_ptr<V{{name}}>& top) -> std::string {
    std::string inputs;
    auto put_value = [&](auto const& input, size_t shift) mutable -> void {
        auto const value { (input >> shift) & 1u };
        inputs += static_cast<char>(value + '0');
    };
{% for input, shift in inputs %}
    put_value(top->{{input|join("->")}}, {{shift}});
{% endfor %}
    return inputs;
}

auto circuit_get_outputs(std::unique_ptr<V{{name}}>& top) -> std::string {
    std::string outputs;
    auto put_value = [&](auto const& output, size_t shift) mutable -> void {
        auto const value { (output >> shift) & 1u };
        outputs += static_cast<char>(value + '0');
    };
{% for output, shift in outputs %}
    put_value(top->{{output|join("->")}}, {{shift}});
{% endfor %}
    return outputs;
}
"""

environment = Environment(loader=DictLoader({ 'circuit.jinja2': TEMPLATE }))
environment.trim_blocks = True
environment.lstrip_blocks = True

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Generate Verilog benchmark circuits.')
    parser.add_argument('--circuit-metadata', dest='circuit_metadata', type=str, required=True, help='The circuit metadata for building the Verilator stub')
    parser.add_argument('--circuit-verilog', dest='circuit_verilog', type=str, required=True, help='The circuit Verilog for building the Verilator stub')
    parser.add_argument('--library-path', dest='library_path', required=True, help='The Verilog cell library to use for Verilator')
    parser.add_argument('--output-path', dest='output_path', type=str, required=True, help='The output directory to store the Verilator code to')
    args = parser.parse_args()

    with open(args.circuit_metadata, 'rt', encoding='utf-8') as stream:
        meta = json.load(stream)

    name = meta['circuit']['name']
    flipflops = meta['circuit']['flipflops']
    inputs = meta['circuit']['inputs']
    outputs = meta['circuit']['outputs']
    clocks = meta['circuit']['clocks']
    resets = meta['circuit']['resets']

    config_path = pathlib.Path(args.output_path) / f'V{name}.vlt'
    config_path.parent.mkdir(parents=True, exist_ok=True)
    with open(config_path, 'wt', encoding='utf-8') as stream:
        print('`verilator_config', file=stream)
        print('lint_off -rule ASCRANGE', file=stream)
        print('public -module "DL"', file=stream)
        print('public -module "DLR"', file=stream)
        print('public -module "DFF"', file=stream)
        print('public -module "DFFR"', file=stream)
        print('public -module "DL" -var "out_reg"', file=stream)
        print('public -module "DLR" -var "out_reg"', file=stream)
        print('public -module "DFF" -var "out_reg"', file=stream)
        print('public -module "DFFR" -var "out_reg"', file=stream)

    subprocess.run([
        'verilator',
        '--Mdir', args.output_path,
        '--cc',
        '--main',
        '--exe',
        '--x-initial', '0',
        '--no-timing',
        '--private',
        '--prefix', f'V{name}',
        '--top-module', name,
        *itertools.chain(*[['--clk', clock] for clock in clocks]),
        *itertools.chain(*[['--clk', reset] for reset in resets]),
        args.library_path,
        args.circuit_verilog,
        config_path,
    ])

    def _transform_flipflop(flipflop):
        """ Template: (flipflop path, ) """
        return ([name] + flipflop.split('/') + ['out_reg'], 0)

    input_map = dict([(input_dict['name'], (input_dict['top'], input_dict['bottom'])) for input_dict in meta['circuit']['inputs_full']])
    output_map = dict([(output_dict['name'], (output_dict['top'], output_dict['bottom'])) for output_dict in meta['circuit']['outputs_full']])
    clock_map = dict([(clock_dict['name'], (clock_dict['top'], clock_dict['bottom'])) for clock_dict in meta['circuit']['clocks_full']])
    reset_map = dict([(reset_dict['name'], (reset_dict['top'], reset_dict['bottom'])) for reset_dict in meta['circuit']['resets_full']])
    index_regex = re.compile(r'(.*) \[(\d+)\]')
    def _transform_port(port, port_map):
        match = index_regex.match(port)
        if match:
            port_name, port_index = match.group(1), match.group(2)
            port_index = int(port_index)
            top, bottom = port_map[port_name]
            # PORT[5:3] is stored as [2, 1, 0] in Verilator
            # PORT[5:7] is stored as [2, 1, 0] in Verilator
            # => Template parameter "shift" is the distance from the bottom index
            return ([port_name], int(abs(bottom - port_index)))
        else:
            return ([port], 0)

    flipflops_transformed = [_transform_flipflop(flipflop) for flipflop in flipflops]
    inputs_transformed = [_transform_port(input, input_map) for input in inputs]
    outputs_transformed = [_transform_port(output, output_map) for output in outputs]
    clocks_transformed = [_transform_port(clock, clock_map) for clock in clocks]
    resets_transformed = [_transform_port(reset, reset_map) for reset in resets]

    context = {
        'name': name,
        'flipflops': flipflops_transformed,
        'inputs': inputs_transformed,
        'outputs': outputs_transformed,
        'clocks': clocks_transformed,
        'resets': resets_transformed,
    }

    template = environment.get_template('circuit.jinja2')
    main_path = pathlib.Path(args.output_path) / f'V{name}__main.cpp'
    main_path.parent.mkdir(parents=True, exist_ok=True)
    with open(main_path, 'wt', encoding='UTF-8') as stream:
        stream.write(template.render(context))

    subprocess.run([
        'make',
        '-j',
        '-C', args.output_path,
        '-f', f'V{name}.mk',
        f'V{name}'
    ])

