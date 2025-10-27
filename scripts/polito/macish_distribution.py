#!/bin/env python3

import argparse
import json
import pathlib
import matplotlib
import numpy

matplotlib.use('Agg')
import matplotlib.pyplot as pyplot
import matplotlib.patches as patches

matplotlib.rcParams["font.family"] = "monospace"

def get_data(key, element):
    error_mean = float(element['stats']['error-mean-norm'])
    error_mean = min(1.0, max(-1.0, error_mean))

    return {
        'error-mean': error_mean,
    }

def get_range(data):
    min_value = min(data)
    max_value = max(data)
    return {
        'low': numpy.percentile(data, 25),
        'high': numpy.percentile(data, 75),
        'min': min_value,
        'max': max_value,
        'range': max_value - min_value
    }

if __name__ == '__main__':
    parser = argparse.ArgumentParser('macish_plot.py')
    parser.add_argument('--name', required=True)
    parser.add_argument('--source-path', required=True)
    parser.add_argument('--output-path', required=True)
    args = parser.parse_args()

    with open(args.source_path, 'rt', encoding='utf-8') as stream:
        data = json.load(stream)

    fault_free = get_data('fault-free', data['fault-free'])
    faulty = [get_data(str(i), data[str(i)]) for i in range(int(data['faults']))]

    output_path = pathlib.Path(args.output_path)
    for (title, filename, stat, low, high, use_limits, num_bins) in [
        ('Faults vs Error Mean', 'MEAN', 'error-mean', 0.25, 0.75, 'use-percentile', 99),
    ]:
        fig, ax = pyplot.subplots()
        fig.set_figwidth(20)
        fig.set_figheight(10)

        ax.set_title(title)
        ax.set_xlabel('Error')
        ax.set_ylabel('Faults')

        ax.set_xscale('linear')
        ax.set_yscale('log')
        ax.margins(0.0, 0.0)

        values = [element[stat] for element in faulty]
        if use_limits == 'use-percentile':
            percentile_min, percentile_low, percentile_high, percentile_max = numpy.percentile(values, (0, int(low * 100), int(high * 100), 100))
        else:
            percentile_min, percentile_low, percentile_high, percentile_max = None, None, None, None

        plot_range = (percentile_low, percentile_high) if use_limits == 'use-percentile' else (low, high)
        ax.hist(values, bins=num_bins, range=plot_range, rwidth=1.0, histtype='barstacked')

        distance = abs(plot_range[1] - plot_range[0])
        digits = 2
        if distance > 0.0:
            while distance < 1.0:
                distance *= 10
                digits += 1
        ax.get_xaxis().set_major_formatter(
            matplotlib.ticker.FuncFormatter(lambda x, p: f'{x:.{digits}f}'))

        pyplot.vlines(fault_free[stat], 0.0, 100000000, color='red', linewidth=2)

        fig.tight_layout()
        pyplot.savefig(output_path / f'{args.name}_{filename}.png', dpi=300)
        pyplot.close()

        print(f"Plotted {args.name}_{filename}.png")
