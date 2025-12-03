import sys
import time

def countdown(minutes, subject):
    total = minutes * 60
    elapsed = 0
    
    print(f"\nTimer: {subject} ({minutes} min)")
    print("-" * 40)
    
    try:
        for rem in range(total, -1, -1):
            m = rem // 60
            s = rem % 60
            pct = ((total - rem) / total) * 100
            bars = int(30 * pct / 100)
            bar = "#" * bars + "-" * (30 - bars)
            
            print(f"\r  {m:02d}:{s:02d}  [{bar}] {pct:5.1f}%", end="", flush=True)
            
            if rem > 0:
                time.sleep(1)
                elapsed += 1
        
        print("\n" + "-" * 40)
        print(f"Done. {minutes} minutes completed.")
        
    except KeyboardInterrupt:
        m = elapsed // 60
        s = elapsed % 60
        print("\n" + "-" * 40)
        print(f"Stopped. Completed {m:02d}:{s:02d} of {minutes} min")

if __name__ == "__main__":
    if len(sys.argv) >= 3:
        mins = int(sys.argv[1])
        subj = " ".join(sys.argv[2:])
        countdown(mins, subj)
    else:
        print("Usage: python3 pomodoro.py <minutes> <subject>")