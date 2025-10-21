#!/usr/bin/env python3
"""
Live plotter for CSV files with lines "y1,y2,y3,y4,...".
X axis = line number (starting at 1). Auto-refreshes when file changes.

Usage:
  python3 plot_multi_y_live.py path/to/file.csv
Optional args:
  --interval, -i  poll interval in seconds (default 1.0)
  --delimiter, -d CSV delimiter (default ',')
"""
import argparse
import os
import time
import numpy as np
import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation

def read_columns(path, delimiter=','):
    try:
        data = np.loadtxt(path, delimiter=delimiter)
        if data.size == 0:
            return None
        if data.ndim == 1:
            # single line -> treat as single row
            data = data.reshape(1, -1)
        return data
    except Exception:
        # fallback: robust line-by-line parse
        cols = []
        try:
            with open(path, 'r') as f:
                for ln in f:
                    ln = ln.strip()
                    if not ln:
                        continue
                    parts = [p for p in ln.split(delimiter) if p != ""]
                    try:
                        nums = [float(p) for p in parts]
                    except ValueError:
                        continue
                    cols.append(nums)
            if not cols:
                return None
            # pad rows to same length if needed
            maxc = max(len(r) for r in cols)
            arr = np.full((len(cols), maxc), np.nan, dtype=float)
            for i, r in enumerate(cols):
                arr[i, :len(r)] = r
            return arr
        except Exception:
            return None

def main():
    p = argparse.ArgumentParser()
    p.add_argument("file", help="CSV file path (each line: y1,y2,y3,y4,...)")
    p.add_argument("--interval", "-i", type=float, default=1.0, help="poll interval seconds")
    p.add_argument("--delimiter", "-d", default=",", help="CSV delimiter")
    args = p.parse_args()

    filepath = args.file
    interval = max(0.05, args.interval)
    delim = args.delimiter

    fig, ax = plt.subplots()
    lines = []
    labels = []
    last_mtime = 0.0

    def update(frame):
        nonlocal last_mtime, lines, labels
        try:
            mtime = os.path.getmtime(filepath)
        except OSError:
            ax.clear()
            ax.set_title(f"File not found: {filepath}")
            return tuple(lines)

        if mtime <= last_mtime and last_mtime != 0:
            return tuple(lines)

        last_mtime = mtime
        data = read_columns(filepath, delimiter=delim)
        if data is None or data.size == 0:
            ax.clear()
            ax.set_title(f"No valid data: {os.path.basename(filepath)}")
            lines = []
            labels = []
            return tuple(lines)

        nrows, ncols = data.shape
        x = np.arange(1, nrows + 1)

        # ensure we have as many line artists as columns
        if len(lines) < ncols:
            # remove old artists and recreate to avoid mismatch
            ax.clear()
            lines = []
            labels = []
            colors = plt.rcParams['axes.prop_cycle'].by_key().get('color', None)
            for ci in range(ncols):
                ln, = ax.plot([], [], marker='o', label=f"y{ci+1}", color=(colors[ci % len(colors)] if colors else None))
                lines.append(ln)
                labels.append(f"y{ci+1}")
            ax.legend(loc='upper right')
        elif len(lines) > ncols:
            # too many lines: hide extras
            for extra in lines[ncols:]:
                extra.set_data([], [])

        # update data for each column (handle NaNs)
        for ci in range(ncols):
            col = data[:, ci]
            valid = ~np.isnan(col)
            if valid.any():
                lines[ci].set_data(x[valid], col[valid])
            else:
                lines[ci].set_data([], [])

        ax.relim()
        ax.autoscale_view()
        ax.set_xlabel("Line number")
        ax.set_ylabel("y")
        ax.set_title(f"{os.path.basename(filepath)} — updated {time.ctime(mtime)}")
        ax.legend(labels, loc='upper right')
        return tuple(lines)

    ani = FuncAnimation(fig, update, interval=int(interval * 1000), blit=True)
    plt.tight_layout()
    plt.show()

if __name__ == "__main__":
    main()