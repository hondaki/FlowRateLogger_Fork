# Repository Guidelines

## Project Structure & Module Organization
- `FlowRateLogger.ino` — main sketch: UI, SD logging, command handling.
- `FlowMeter.h` / `FlowMeter.ino` — pulse counter (PCNT) setup and flow calculations.
- `TimerController.ino` — Wi‑Fi + NTP time sync and RTC utilities.
- `README.md` — brief overview. Logs are written to SD as `/YYYYMMDD_HHMMSS00_flowrate.txt`.

## Build, Test, and Development Commands
- Requirements: Arduino IDE or Arduino CLI, ESP32 core (`esp32:esp32`), M5Stack `M5Core2` library, and SD support.
- Discover board FQBN: `arduino-cli board listall | rg -i core2`
- Compile: `arduino-cli compile --fqbn <FQBN> .` (e.g., `esp32:esp32:m5stack-core2`)
- Upload: `arduino-cli upload -p <PORT> --fqbn <FQBN> .`
- Serial monitor: `arduino-cli monitor -p <PORT> -c 115200`
- Arduino IDE: open `FlowRateLogger.ino`, select “M5Stack Core2”, correct port, then Upload.

## Coding Style & Naming Conventions
- Language: Arduino C++ (ESP32). Indentation: 2 spaces, no tabs.
- Naming: `snake_case` for functions/variables, `UPPER_SNAKE_CASE` for constants, file names match existing (e.g., keep `TimerController.ino`).
- Keep functions small and cohesive; prefer static/`internal` linkage where possible.
- Avoid heavy dynamic allocation; prefer stack or static buffers.

## Testing Guidelines
- No formal unit tests. Validate on device:
  - With SD inserted and flow sensors connected, observe display status: “Waiting Data”, “Data Logging”, or “NO SD CARD”.
  - Verify log file name pattern and contents on SD.
  - Use serial commands: `ut`/`updatetime` to sync time; `so`/`serialout` to toggle serial log output.
- For changes to flow math, print `count_diff`, `total_count`, `flow_rate`, `total_flow` to Serial and verify against expected pulses.

## Commit & Pull Request Guidelines
- Commits: use Conventional Commits (imperative tense). Examples:
  - `feat: add SD log rotation on inactivity`
  - `fix: correct PCNT rollover handling`
  - `refactor: extract RTC update into helper`
  - `docs: clarify serial commands in README`
- PRs: include description, linked issues, device screenshots (UI + Serial), sample SD log snippet, and clear test steps.

## Security & Configuration Tips
- Do not commit real Wi‑Fi credentials. Move them to `secrets.h` (e.g., `extern const char* ssid;`) and add to `.gitignore`; include `secrets_example.h` for reference.
- Adjust timezone/NTP in `TimerController.ino` for your region.
