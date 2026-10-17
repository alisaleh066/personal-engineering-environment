# personal-engineering-environment

C command-line toolkit: mechanical, electrical and control calculators, forex tools, productivity (pomodoro), plus a few Python plotting scripts.

## Build
`./compile.sh` (gcc with -Wall -Wextra, produces `apex`). Run `./apex`.

## Rules
- Fix all compiler warnings; no new warnings.
- Check buffer sizes and return values on every input/parse call.
- Forex features are calculators only; no live trading.
