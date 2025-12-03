import sys
import matplotlib.pyplot as plt
import csv

if len(sys.argv) < 2:
    print("Usage: python3 plot_electrical.py <csvfile>")
    sys.exit(1)

filename = sys.argv[1]
x_data = []
y_data = []
label = ""

with open(filename, 'r') as f:
    reader = csv.reader(f)
    header = next(reader)
    label = header[1] if len(header) > 1 else "Voltage"
    
    for row in reader:
        x_data.append(float(row[0]))
        y_data.append(float(row[1]))

plt.figure(figsize=(10, 6))
plt.plot(x_data, y_data, 'r-', linewidth=2, label=label)
plt.xlabel('Time Step', fontsize=12)
plt.ylabel(label, fontsize=12)
plt.title('Electrical Circuit Response', fontsize=14, fontweight='bold')
plt.grid(True, alpha=0.3)
plt.legend()

output = filename.replace('.csv', '.png')
plt.savefig(output, dpi=150, bbox_inches='tight')
print(f"Graph saved as {output}")