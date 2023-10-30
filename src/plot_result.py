from argparse import ArgumentParser
import matplotlib.pyplot as plt
import numpy as np

parser = ArgumentParser()
parser.add_argument('prefix', type=str, help='prefix of the result files')
parser.add_argument('--title', type=str, default='', help='title of the plot')
parser.add_argument('--display', action='store_true', help='display the plot')

args = parser.parse_args()

traces = ['gcc_trace', 'go_trace', 'perl_trace']


def result_filename(trace):
    return f"{args.prefix}.{trace}.result.txt"


results_dict = {}
for trace in traces:
    results_dict[trace] = {'x': [], 'y': []}
    with open(result_filename(trace), 'r') as f:
        lines = f.readlines()
        for line in lines:
            x, y = line.split()
            results_dict[trace]['x'].append(float(x))
            results_dict[trace]['y'].append(float(y))

if not args.title:
    args.title = args.prefix.replace('_', ' ')

xlabel = args.prefix.split('_vs_')[0].replace('_', ' ')
ylabel = args.prefix.split('_vs_')[-1].replace('_', ' ')

fig, ax = plt.subplots(figsize=(8, 6))
ax.set_title(args.title)
ax.set_xlabel(xlabel)
ax.set_ylabel(ylabel)

for trace in traces:
    ax.plot(results_dict[trace]['y'], label=trace)

ax.set_xticks(np.arange(len(results_dict[trace]['x'])), results_dict[trace]['x'])
ax.legend()

# save the figure
print(f"Saving figure to {args.prefix}.png")
fig.savefig(f"{args.prefix}.png")

if args.display:
    plt.show()
