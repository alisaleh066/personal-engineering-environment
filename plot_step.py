import sys
import matplotlib.pyplot as plt

def plot_step(filename):
    t = []
    y = []
    
    with open(filename, 'r') as f:
        next(f)
        for line in f:
            parts = line.strip().split(',')
            t.append(float(parts[0]))
            y.append(float(parts[1]))
    
    plt.figure(figsize=(10, 6))
    plt.plot(t, y, 'b-', linewidth=2, label='Step Response')
    plt.axhline(y=1.0, color='g', linestyle='--', label='Setpoint')
    plt.axhline(y=1.02, color='r', linestyle=':', alpha=0.5)
    plt.axhline(y=0.98, color='r', linestyle=':', alpha=0.5, label='2% Band')
    
    plt.xlabel('Time (s)')
    plt.ylabel('Response')
    plt.title('Second-Order System Step Response')
    plt.legend()
    plt.grid(True)
    plt.xlim(0, max(t))
    plt.ylim(0, max(y) * 1.1)
    
    outfile = filename.replace('.csv', '.png')
    plt.savefig(outfile, dpi=150)
    print(f"Saved plot to {outfile}")
    plt.close()

if __name__ == "__main__":
    if len(sys.argv) >= 2:
        plot_step(sys.argv[1])
    else:
        print("Usage: python3 plot_step.py <csv_file>")


