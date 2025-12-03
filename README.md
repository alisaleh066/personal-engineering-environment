# Personal Engineering Environment

A multi-domain toolkit I built that combines mechanical, electrical, control systems, forex trading, and productivity tools. Basically everything I need in one place instead of switching between apps.

## Features

### Engineering Calculators

#### Mechanical Calculator
- Beam Deflection (point/uniform loads)
- Spring-Mass-Damper analysis
- Simple Pendulum simulation
- Material presets (Steel, Aluminum, Titanium, etc.)
- Unit converter

#### Electrical Calculator
- RLC Transient Response
- RC Charge Curves
- Buck Converter Designer
- Component presets
- Ohm's Law calculator

#### Control Systems Suite
- **Second-Order System Designer** - design from damping ratio/natural frequency or specs
- **Step Response Calculator** - rise time, peak time, settling time, overshoot
- **PID Tuner** - Ziegler-Nichols method with config saving
- **Equivalence Bridge** - converts mechanical/electrical to control parameters 

### Trading Tools

#### Forex Suite
- Trade Calculator (position size, pip value, margin)
- Risk Analyzer (risk-reward ratio, trade quality rating)
- Trade Journal (log trades, track win rate, export)
- Quick Reference (lot sizes, pip values, basic rules)

### Productivity Suite
- **Pomodoro Timer** - 25 minute focus sessions with streak tracking
- **Assignment Tracker** - deadlines, progress bars, priority levels
- **Grade Calculator** - weighted grades, GPA, "what do I need to pass?"
- **Study Analytics** - tracks sessions, hours, streaks, exports to CSV

### Data & Reference

#### Formula Library
- 54 built-in formulas (Electrical, Mechanical, Control, Forex, Statistics, Physics)
- Search functionality
- Browse by category
- Add your own custom formulas

#### CSV Analyzer
- Load any CSV file
- Statistical analysis (mean, median, std dev, min, max)
- Outlier detection
- Export results

#### Data Analysis Lab
- Store up to 3 simulations
- ASCII terminal plotting
- Multi-series comparison (this is really useful for seeing mechanical vs electrical equivalence)

### Utilities
- **Settings** - colors, defaults, output directory
- **Help** - guides for each module
- **Self-Test** - system diagnostics to verify everything works

## Installation

### What you need
- GCC compiler
- Python 3.x with matplotlib (for graphs)

### Compile
```bash
chmod +x compile.sh
./compile.sh
```

### Run
```bash
./apex
```

## Project Structure
```
├── main.c                 # Main menu
├── mechanical.c/h         # Mechanical simulations
├── electrical.c/h         # Electrical simulations
├── control.c/h            # Control systems suite
├── finance.c/h            # Forex trading tools
├── productivity.c/h       # Study & productivity
├── formula.c/h            # Formula library
├── csv_analyzer.c/h       # CSV analysis
├── analytics.c/h          # Data analysis lab
├── settings.c/h           # Settings/help/self-test
├── utils.c/h              # Helper functions
├── file_import.c/h        # File loading
├── colors.h               # Terminal colors
├── types.h                # Data structures
├── plot_mechanical.py     # Mechanical graphs
├── plot_electrical.py     # Electrical graphs
├── plot_comparison.py     # Comparison plots
├── plot_step.py           # Step response plots
├── plot_forex.py          # Trading graphs
├── pomodoro.py            # Visual timer
└── compile.sh             # Build script
```

## File Outputs

| File | What generates it |
|------|--------------|
| simulation_log.txt | All simulations |
| step_response.csv | Control systems |
| trades.csv | Forex journal |
| forex_analysis.png | Trade graphs |
| study_log.txt | Pomodoro sessions |
| assignments.txt | Assignment tracker |
| study_stats.csv | Study analytics |
| custom_formulas.txt | Formula library |
| pid_configs.txt | PID tuner |
| config.txt | Settings |

## Something I found interesting

The toolkit shows how different engineering domains are mathematically equivalent:
- Spring-mass-damper ↔ RLC circuits ↔ Control systems
- They all follow second-order differential equations
- Same damping ratio means identical behavior

This is something I didn't fully appreciate until I built the analytics lab and could plot them side by side.

## Project Info

- **Course:** ELEC2645 Embedded Systems Project
- **Language:** C (Unit 1 standards)
- **Year:** 2025/26

Built this because I was tired of switching between MATLAB, online calculators, and my phone for different tasks. Wanted everything integrated in one tool that actually helps with coursework.

---

**Ali Saleh**  
Second-year Mechatronics Engineering  
University of Leeds
