import sys
import matplotlib.pyplot as plt
import csv

if len(sys.argv) < 3:
    print("Usage: python3 plot_comparison.py <file1.csv> <file2.csv> [file3.csv]")
    sys.exit(1)

fig, ax = plt.subplots(figsize=(12, 7))
colors = ['blue', 'red', 'green']

for i, filename in enumerate(sys.argv[1:]):
    x_data = []
    y_data = []
    
    with open(filename, 'r') as f:
        reader = csv.reader(f)
        header = next(reader)
        label = header[1] if len(header) > 1 else f"Series {i+1}"
        
        for row in reader:
            x_data.append(float(row[0]))
            y_data.append(float(row[1]))
    
    ax.plot(x_data, y_data, color=colors[i % 3], linewidth=2, label=label, alpha=0.8)

ax.set_xlabel('Index', fontsize=12)
ax.set_ylabel('Value', fontsize=12)
ax.set_title('Multi-Domain Comparison', fontsize=14, fontweight='bold')
ax.grid(True, alpha=0.3)
ax.legend()

plt.savefig('comparison_plot.png', dpi=150, bbox_inches='tight')
print("Comparison graph saved as comparison_plot.png")