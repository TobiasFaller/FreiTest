import json
import numpy
import subprocess
import matplotlib

matplotlib.use('Agg')
import matplotlib.pyplot as pyplot

SIMULATION_SCALE = 10e-9
SIMULATION_OFFSET_INPUTS = SIMULATION_SCALE * 0.1
SIMULATION_OFFSET_OUTPUTS = SIMULATION_SCALE * 1.0
SIMULATION_ORDER = [ 'CLOCK', 'IN1', 'IN2', 'IN3', 'ARESET' ]
LEVEL_LOW = 0.2 * 1.8
LEVEL_HIGH = 0.8 * 1.8

EXPECTED = {
    'BUF': ['01'],
    'AND': ['0001'],
    'OR': ['0111'],
    'XOR': ['0110'],
    'INV': ['10'],
    'NAND': ['1110'],
    'NOR': ['1000'],
    'XNOR': ['1001'],
    'MUX': ['01010011'],
    'DL': ['X00110'],
    'DLR': ['000100011001100010001001'],
    'DFF': ['X00110'],
    'DFFR': ['000100011001100010001001'],
}

def to_bool(value: float) -> 'str':
    return '0' if value <= LEVEL_LOW else \
        '1' if value >= LEVEL_HIGH else \
        'X'

if __name__ == '__main__':
    subprocess.run('ngspice generic_spice_tb.spi', shell=True)

    with open('generic_meta.json', 'rt', encoding='utf-8') as stream:
        library = json.load(stream)

    all_gate_ok = True
    for name, attributes in [(name, { **library['cells'][name], 'expected': EXPECTED[name] }) for name in EXPECTED.keys()]:
        print(f'Checking gate {name}')
        sim_data = numpy.transpose(numpy.loadtxt(f'generic_sim_{name.lower()}.txt', dtype='f'))
        sim_inputs = attributes['inputs'] + attributes.get('clocks', []) + attributes.get('resets', [])
        sim_inputs.sort(key=lambda pin: SIMULATION_ORDER.index('CLOCK' if pin == 'ENABLE' else pin))
        sim_outputs = attributes['outputs']
        sim_expected = attributes['expected']

        sim_input_periods = [ 2**SIMULATION_ORDER.index('CLOCK' if pin == 'ENABLE' else pin) for pin in sim_inputs ]
        sim_input_max_period = 2 * max(sim_input_periods)
        sim_input_times = [ sum([sim_input_periods[index] for index in range(len(sim_input_periods)) if value & (1 << index)]) for value in range(len(sim_expected[0])) ]
        sim_input_times = [ time + (index // (2**len(sim_inputs))) * sim_input_max_period for index, time in enumerate(sim_input_times) ]
        sim_time_inputs = [ SIMULATION_OFFSET_INPUTS + time * SIMULATION_SCALE for time in sim_input_times ]
        sim_time_outputs = [ SIMULATION_OFFSET_OUTPUTS + time * SIMULATION_SCALE for time in sim_input_times ]

        fig, axs = pyplot.subplots(2, dpi=300)
        axs[0].tick_params(top=True, labeltop=True, bottom=False, labelbottom=False)
        axs[1].tick_params(top=False, labeltop=False, bottom=True, labelbottom=True)
        axs[0].get_xaxis().set_major_formatter(lambda x, pos=0: f'{x * 1e9:.0f} n')
        axs[1].get_xaxis().set_major_formatter(lambda x, pos=0: f'{x * 1e9:.0f} n')
        axs[0].get_yaxis().set_major_formatter(lambda y, pos=0: f'{y:.1f} V')
        axs[1].get_yaxis().set_major_formatter(lambda y, pos=0: f'{y:.1f} V')

        fig.set_figwidth(21)
        fig.set_figheight(9)
        fig.tight_layout()

        for index in range(len(sim_inputs)):
            axs[0].plot(sim_data[0], sim_data[index + 1], '--', label=sim_inputs[index])
        for index in range(len(sim_outputs)):
            axs[1].plot(sim_data[0], sim_data[index + len(sim_inputs) + 1], '--', label=sim_outputs[index])

        for time in sim_time_inputs:
            axs[0].vlines(time, -0.1, 2.5, color='gray', linestyle='--', clip_on=False)
        for time in sim_time_outputs:
            axs[1].vlines(time, -0.1, 2.5, color='red', linestyle='--', clip_on=False)

        axs[0].set_xlim(0.0, max(sim_time_inputs + sim_time_outputs))
        axs[0].set_ylim(-0.1, 2.5)
        axs[0].legend(sim_inputs, loc='upper right')

        axs[1].set_xlim(0.0, max(sim_time_inputs + sim_time_outputs))
        axs[1].set_ylim(-0.1, 2.5)
        axs[1].legend(sim_outputs, loc='upper right')

        for time_input, time_output in zip(sim_time_inputs, sim_time_outputs):
            arrow_tail = axs[0].transData.transform((time_input, -0.13))
            arrow_head = axs[1].transData.transform((time_output, 2.52))
            arrow_tail = (arrow_tail[0] - arrow_head[0], arrow_tail[1] - arrow_head[1])
            arrow_style = {
                'arrowstyle': '->',
                'color': 'gray',
                'linestyle': '--',
                'linewidth': 1.5
            }
            fig.add_artist(matplotlib.text.Annotation('', arrow_head, xytext=arrow_tail, \
                xycoords='figure pixels', textcoords='offset pixels', arrowprops=arrow_style))

        pyplot.savefig(f'generic_sim_{name.lower()}.png', dpi=300)
        pyplot.close()

        sim_result_inputs = numpy.transpose([numpy.interp(sim_time_inputs, sim_data[0], sim_data[index + 1]) for index in range(len(sim_inputs))])
        sim_result_outputs = numpy.transpose([numpy.interp(sim_time_outputs, sim_data[0], sim_data[index + len(sim_inputs) + 1]) for index in range(len(sim_outputs))])
        sim_result = list(zip(sim_time_inputs, range(len(sim_input_times)), sim_result_inputs, sim_result_outputs))
        sim_result.sort(key=lambda result: result[0])

        all_ok = True
        for time, input_index, input_values, output_values in sim_result:
            in_ok = True
            for index, input_value in enumerate(input_values):
                if sim_inputs[index] == 'ARESET':
                    in_ok &= to_bool(input_value) == ('1' if ((time // SIMULATION_SCALE + 1) % 17 <= 1) else '0')
                else:
                    in_ok &= to_bool(input_value) == ('1' if input_index & (1 << index) else '0')
            all_ok &= in_ok

            out_ok = True
            for index, output_value in enumerate(output_values):
                out_ok &= to_bool(output_value) ==  sim_expected[index][input_index] or sim_expected[index][input_index] == 'X'
            all_ok &= out_ok

            inputs_binary = ' '.join([f'{sim_inputs[index]}={to_bool(value)}' for index, value in enumerate(input_values)])
            outputs_binary = ' '.join([f'{sim_outputs[index]}={to_bool(value)}' for index, value in enumerate(output_values)])
            inputs_float = ' '.join([f'{sim_inputs[index]}={value:.3f}' for index, value in enumerate(input_values)])
            outputs_float = ' '.join([f'{sim_outputs[index]}={value:.3f}' for index, value in enumerate(output_values)])
            outputs_expected = ' '.join([f'{sim_outputs[index]}={sim_expected[index][input_index]}' for index in range(len(output_values))])
            print(f'  {time*1e9:5.0f}: {inputs_binary} {outputs_binary} vs {outputs_expected} {"OK" if in_ok & out_ok else ("!!" if not out_ok else "??")} ({inputs_float} {outputs_float})')
        all_gate_ok &= all_ok
        print('  PASSED' if all_ok else 'FAILED')

    print(f'Checkin all gates {"PASSED" if all_gate_ok else "FAILED"}')