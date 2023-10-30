# 一级Cache仿真器

首先进入`src`目录。

验证结果正确性：

```bash
make && ./test.sh
```

运行profiler：

```bash
./profiler.sh
```

绘制结果：

```bash
ll | grep gcc_trace.result.txt | awk '{print $9}' | grep -oE '[a-z_]+_vs_[a-z_]+' | xargs -n 1 python plot_result.py
```