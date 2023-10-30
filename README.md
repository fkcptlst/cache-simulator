# L1 Cache Simulator

First, `cd src`.

To validate the correctness of the simulator, run:

```bash
make && ./test.sh
```

For the performance evaluation, run:

```bash
./profiler.sh
```

To plot the result, run:

```bash
ll | grep gcc_trace.result.txt | awk '{print $9}' | grep -oE '[a-z_]+_vs_[a-z_]+' | xargs -n 1 python plot_result.py
```