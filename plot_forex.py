import sys
import matplotlib.pyplot as plt

def plot_trades(filename):
    pairs = []
    pnls = []
    
    with open(filename, 'r') as f:
        next(f)
        for line in f:
            parts = line.strip().split(',')
            if len(parts) >= 6:
                pairs.append(parts[0])
                pnls.append(float(parts[5]))
    
    if len(pnls) == 0:
        print("No trades to plot")
        return
    
    fig, axes = plt.subplots(1, 2, figsize=(14, 5))
    
    equity = []
    total = 0
    for p in pnls:
        total += p
        equity.append(total)
    
    axes[0].plot(range(1, len(equity)+1), equity, 'b-', linewidth=2, marker='o', markersize=4)
    axes[0].axhline(y=0, color='black', linestyle='-', linewidth=0.5)
    axes[0].fill_between(range(1, len(equity)+1), equity, alpha=0.3, 
                         color='green' if equity[-1] >= 0 else 'red')
    axes[0].set_xlabel('Trade Number')
    axes[0].set_ylabel('Cumulative P/L ($)')
    axes[0].set_title('Equity Curve')
    axes[0].grid(True, alpha=0.3)
    
    pair_pnl = {}
    for i in range(len(pairs)):
        if pairs[i] in pair_pnl:
            pair_pnl[pairs[i]] += pnls[i]
        else:
            pair_pnl[pairs[i]] = pnls[i]
    
    pair_names = list(pair_pnl.keys())
    pair_totals = list(pair_pnl.values())
    colors = ['green' if p >= 0 else 'red' for p in pair_totals]
    
    axes[1].bar(pair_names, pair_totals, color=colors)
    axes[1].axhline(y=0, color='black', linestyle='-', linewidth=0.5)
    axes[1].set_xlabel('Currency Pair')
    axes[1].set_ylabel('Total P/L ($)')
    axes[1].set_title('P/L by Pair')
    axes[1].grid(True, alpha=0.3, axis='y')
    
    plt.tight_layout()
    plt.savefig('forex_analysis.png', dpi=150)
    print("Saved to forex_analysis.png")
    plt.close()

if __name__ == "__main__":
    if len(sys.argv) >= 2:
        plot_trades(sys.argv[1])
    else:
        plot_trades("trades.csv")