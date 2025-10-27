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
    # Names are swapped in code
    error_positive = float(element['limits-negative']['max-absolute'])
    error_negative = float(element['limits-positive']['max-absolute'])
    error_mean = float(element['stats']['error-mean-norm'])
    error_mean = min(1.0, max(-1.0, error_mean))
    error_positive = min(1.0, max(-1.0, error_positive))
    error_negative = min(1.0, max(-1.0, error_negative))

    return {
        'limit-positive': error_positive,
        'limit-negative': error_negative,
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
    parser.add_argument('--error-mean-max', required=True, type=float)
    parser.add_argument('--error-limit-max', required=True, type=float)
    args = parser.parse_args()

    with open(args.source_path, 'rt', encoding='utf-8') as stream:
        data = json.load(stream)

    fault_free = get_data('fault-free', data['fault-free'])
    faulty = [get_data(str(i), data[str(i)]) for i in range(int(data['faults']))]
    ranges = { key: get_range([elem[key] for elem in [fault_free] + faulty]) for key in fault_free.keys() }

    datasets = { key: {} for key in fault_free.keys() }
    def add_key(deg_limit, deg_mean):
        lightness = 1.0 - (deg_limit / 10.0 * 0.6) if (deg_mean != 10 and deg_limit != 10) else 0.0
        value = (0.2 + deg_mean / 10.0 * 0.6)
        colour = [(value * lightness if index <= 2 else 1.0) for index, value in enumerate(matplotlib.colormaps['rainbow'](value))]
        for key in fault_free.keys():
            datasets[key][(deg_limit, deg_mean)] = (colour, [])

    for deg_limit in range(0, 10):
        for deg_mean in range(0, 10):
            add_key(deg_limit, deg_mean)

    # Add excluded values last to have them together
    for deg_limit in range(0, 11):
        add_key(deg_limit, 10)
    for deg_mean in range(0, 11):
        add_key(10, deg_mean)

    def classify_degradation_limit(faulty):
        deg_positive = abs(faulty['limit-positive'] - fault_free['limit-positive'])
        deg_negative = abs(faulty['limit-negative'] - fault_free['limit-negative'])
        return min(max(deg_positive, deg_negative) / args.error_limit_max, 1.0)

    def classify_degradation_mean(faulty):
        deg_mean = abs(faulty['error-mean'] - fault_free['error-mean'])# / ranges['error-mean']['range']
        return min(deg_mean / args.error_mean_max, 1.0)

    for element in faulty:
        deg_limit = int(classify_degradation_limit(element) * 10.0)
        deg_mean = int(classify_degradation_mean(element) * 10.0)
        for key in fault_free.keys():
            datasets[key][(deg_limit, deg_mean)][1].append(element[key])

    output_path = pathlib.Path(args.output_path)
    for (title, filename, stat, low, high, use_limits, num_bins) in [
        ('Faults vs Error Mean', 'MEAN', 'error-mean', 0.25, 0.75, 'use-percentile', 99),
        ('Faults vs Error +Max', 'LARGER', 'limit-positive', 0.0, 1.0, 'use-limits', 49),
        ('Faults vs Error -Max ', 'SMALLER', 'limit-negative', 0.0, 1.0, 'use-limits', 49),
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

        if stat == 'error-mean':
            factor = args.error_mean_max
            labels = [patches.Patch(color=matplotlib.colormaps['rainbow'](0.2 + deg_mean / 10.0 * 0.6), label=f'{deg_mean+1:>3}0% Limit: {sum(len(datasets["error-mean"][(deg_limit, deg_mean)][1]) for deg_limit in range(0, 10)):>5} Faults') for deg_mean in range(0, 10)]
        elif stat == 'limit-positive' or stat == 'limit-negative':
            factor = args.error_limit_max
            labels = [patches.Patch(color=matplotlib.colormaps['binary'](0.2 + deg_limit / 10.0 * 0.8), label=f'{deg_limit+1:>3}0% Limit: {sum(len(datasets["error-mean"][(deg_limit, deg_mean)][1]) for deg_mean in range(0, 10)):>5} Faults') for deg_limit in range(0, 10)]

        in_limits = sum(len(datasets["error-mean"][(deg_limit, deg_mean)][1]) for deg_mean in range(0, 10) for deg_limit in range(0, 10))
        labels.append(patches.Patch(facecolor='white', edgecolor='white', label=f'  In Limits: {in_limits:>5} Faults'))
        labels.append(patches.Patch(facecolor='white', edgecolor='white', label=f'             {in_limits/len(faulty)*100:>5.2f} %'))
        labels.append(patches.Patch(facecolor='white', edgecolor='white', label=f'      Total: {len(faulty):>5} Faults'))
        ax.legend(handles=labels)

        colours = [element[0] for element in datasets[stat].values()]
        dataset = [element[1] for element in datasets[stat].values()]

        values = [element[stat] for element in faulty]
        if use_limits == 'use-percentile':
            percentile_min, percentile_low, percentile_high, percentile_max = numpy.percentile(values, (0, int(low * 100), int(high * 100), 100))
        else:
            percentile_min, percentile_low, percentile_high, percentile_max = None, None, None, None

        plot_range = (percentile_low, percentile_high) if use_limits == 'use-percentile' else (low, high)
        ax.hist(dataset, bins=num_bins, range=plot_range, color=colours, stacked=True, rwidth=1.0, histtype='barstacked')

        pyplot.vlines(fault_free[stat], 0.0, 100000000, color='red', linewidth=2)
        if stat == 'error-mean' or fault_free[stat] - factor >= 0.0:
            pyplot.vlines(fault_free[stat] - factor, 0.0, 100000000, color='red', linestyles='dashed', linewidth=2)
        if stat == 'error-mean' or fault_free[stat] - factor <= 1.0:
            pyplot.vlines(fault_free[stat] + factor, 0.0, 100000000, color='red', linestyles='dashed', linewidth=2)

        fig.tight_layout()
        pyplot.savefig(output_path / f'{args.name}_{filename}.png', dpi=300)
        pyplot.close()

        print(f"Plotted {args.name}_{filename}.png")
