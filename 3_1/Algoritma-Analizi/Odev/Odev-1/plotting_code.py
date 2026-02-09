import numpy as np
import matplotlib.pyplot as plt
import matplotlib.ticker as mticker
import os

# -----------------------------
# AYARLAR
# -----------------------------
FILE_PATH = "try_array_k_1_custom.txt"  # try_array/try_k_1000.txt gibi
WINDOW_SIZE = 1000  # her kaç index için ortalama alınacak
K = "1_custom"

output_path = f"Plots/k{K}.png"


# -----------------------------
# VERI OKUMA
# -----------------------------
def load_try_counts(path):
    try_counts = []
    with open(path, "r", encoding="utf-8") as f:
        for line in f:
            if line.startswith("#") or not line.strip():
                continue
            parts = line.split()
            try_counts.append(int(parts[2]))  # try_count
    return np.array(try_counts, dtype=np.int32)


try_counts = load_try_counts(FILE_PATH)

# negatifleri (bulunamayanları) ayıkla
valid_counts = try_counts[try_counts >= 0]

mean_probe = valid_counts.mean()
max_probe = valid_counts.max()

# -----------------------------
# WINDOW AVERAGING
# -----------------------------
usable_len = (len(valid_counts) // WINDOW_SIZE) * WINDOW_SIZE
windowed = valid_counts[:usable_len].reshape(-1, WINDOW_SIZE)

avg_per_window = windowed.mean(axis=1)
x_axis = np.arange(len(avg_per_window)) * WINDOW_SIZE

# -----------------------------
# GRAFIK 1: WINDOW AVERAGE
# -----------------------------
plt.figure(figsize=(26, 6))

plt.scatter(x_axis, avg_per_window, s=8, alpha=0.6)  # nokta boyutu  # şeffaflık

plt.xlabel("Index")
plt.ylabel(f"Average probing (per {WINDOW_SIZE} elements)")
plt.title(f"K = {K}")

stats_text = f"Overall Mean: {mean_probe:.1f}\nMaximum: {max_probe}"
plt.text(
    0.99,
    0.90,
    stats_text,
    transform=plt.gca().transAxes,
    ha="right",
    va="top",
    bbox=dict(boxstyle="round", fc="white", ec="black"),
)

plt.tight_layout()

ax = plt.gca()
ax.set_xlim(left=0)
ax.xaxis.set_major_locator(mticker.MultipleLocator(100_000))
ax.xaxis.set_major_formatter(mticker.StrMethodFormatter("{x:,.0f}"))

plt.savefig(
    output_path, dpi=300, bbox_inches="tight", pad_inches=0.2  # 🔑 kırpmayı düzeltir
)

plt.show()
