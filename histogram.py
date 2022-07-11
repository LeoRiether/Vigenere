#!/usr/bin/env python3
# Makes histograms of bytes for the purpose of frequency analysis

import matplotlib.pyplot as plt
import numpy as np
from matplotlib import colors
from matplotlib.ticker import PercentFormatter
import sys

if len(sys.argv) <= 1:
    print("Usage: ./histogram.py file_you_want_to_make_a_histogram_of")
    exit(0)

hist = np.zeros(256, dtype=np.int64)
with open(sys.argv[-1], 'rb') as f:
    for byte in f.read():
        hist[byte] += 1

print("{")
for i in range(256):
    print(f"    {{ {i}, {hist[i]} }},")
print("}")

plt.bar(np.arange(256), hist)
plt.show()
