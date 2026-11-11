import argparse
import json
import random
import pathlib

from typing import Dict, List, Tuple, Any
from dataclasses import dataclass, field

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Generate stimuli for benchmark circuits.')
    parser.add_argument('--circuit-metadata', dest='circuit_metadata', type=str, required=True, help='The circuit metadata for creating stimuli')
    parser.add_argument('--stimuli-seed', dest='stimuli_seed', type=int, default=0, help='The seed for the random number generator')
    parser.add_argument('--stimuli-timeframes', dest='stimuli_timeframes', type=int, required=True, help='The number of timeframes to generate as inputs')
    parser.add_argument('--output-txt', dest='output_txt', type=str, required=True, help='The output path to store the stimuli to')
    parser.add_argument('--output-json', dest='output_json', type=str, required=True, help='The output path to store the stimuli to')
    args = parser.parse_args()

    rand = random.Random(args.stimuli_seed)

    with open(args.circuit_metadata, 'rt', encoding='utf-8') as stream:
        meta = json.load(stream)

    name = meta['circuit']['name']
    flipflops = meta['circuit']['flipflops']
    inputs = meta['circuit']['inputs']
    outputs = meta['circuit']['outputs']
    clocks = meta['circuit']['clocks']
    resets = meta['circuit']['resets']

    ff_init = ''.join([str(rand.randint(0, 1)) for _ in flipflops])
    in_assignments = [''.join([str(rand.randint(0, 1)) for _ in inputs]) for _ in range(args.stimuli_timeframes)]

    with open(args.output_txt, 'wt', encoding='utf-8') as stream:
        print(f'FF 0 {ff_init}', file=stream)
        for timeframe, assignment in enumerate(in_assignments):
            print(f'IN {timeframe} {assignment}', file=stream)

    with open(args.output_json, 'wt', encoding='utf-8') as stream:
        json.dump({
            'circuit': {
                'name': name,
                'flipflops': flipflops,
                'inputs': inputs,
                'outputs': outputs,
                'clocks': clocks,
                'resets': resets,
            },
            'stimuli': {
                'flipflops': ff_init,
                'inputs': in_assignments
            }
        }, stream)
