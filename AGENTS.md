# gridlock-808 Agent Notes

## Workflow

### Build / Install
- `make` or `git build` — compiles `src/*.cpp` and `src/*.c` into `gridlock-808.gba`
- Requires devkitPro at `/opt/devkitpro` (or set `DEVKITPRO` env var)
- `make clean` removes build artifacts

### Committing Files
- You are the **sole author**. All commits use: **Neo Phukubye** `<nephujhb025@student.wethinkcode.co.za>`
- Commit a single file: `./commit-file <filename> [message]` or `git commit-file <filename> [message]`
- Example: `./commit-file src/main.cpp "add hardware init loop"`
- Stage and commit in one step — no need to run `git add` separately
