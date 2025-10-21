#!/usr/bin/env python3
import argparse
import os
import time
import numpy as np
import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation

def read_xy(path, delimiter=','):
    try:
        data = np.loadtxt(path, delimiter=delimiter)
        if data.size == 0:
            return np.array([]), np.array([])
        if data.ndim == 1:
            # single line or single column fallback
            if data.size >= 2:
                # treat alternating values as x,y pairs
                x = data[0::2]
                y = data[1::2]
            else:
                return np.array([]), np.array([])
        else:
            x = data[:, 0]
            y = data[:, 1]
        return x, y
    except Exception:
        return np.array([]), np.array([])

def main():
    p = argparse.ArgumentParser(description="Live-plot x,y CSV and auto-refresh on file changes.")
    p.add_argument("file", help="CSV file path (each line: x,y)")
    p.add_argument("--interval", "-i", type=float, default=1.0, help="poll interval in seconds (default 1.0)")
    p.add_argument("--delimiter", "-d", default=",", help="CSV delimiter (default ',')")
    args = p.parse_args()

    filepath = args.file
    interval = max(0.1, args.interval)
    delim = args.delimiter

    fig, ax = plt.subplots()
    line, = ax.plot([], [], marker='o')
    ax.set_xlabel("x")
    ax.set_ylabel("y")
    last_mtime = 0.0

    def update(frame):
        nonlocal last_mtime
        try:
            mtime = os.path.getmtime(filepath)
        except OSError:
            ax.set_title(f"File not found: {filepath}")
            line.set_data([], [])
            return line,

        if mtime <= last_mtime and last_mtime != 0:
            return line,

        last_mtime = mtime
        x, y = read_xy(filepath, delimiter=delim)
        if x.size == 0 or y.size == 0:
            ax.set_title(f"{os.path.basename(filepath)} — no/invalid data")
            line.set_data([], [])
            return line,

        line.set_data(x, y)
        ax.set_xlim(np.min(x), np.max(x))
        ax.set_ylim(np.min(y), np.max(y))
        ax.autoscale_view()
        ax.set_title(f"{os.path.basename(filepath)} — updated {time.ctime(mtime)}")
        return line,

    # start animation (interval in ms)
    ani = FuncAnimation(fig, update, interval=int(interval * 1000), blit=True)
    plt.show()

if __name__ == "__main__":
    main()