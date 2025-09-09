# FlowRateLogger

## English

### Overview
FlowRateLogger runs on M5Stack Core2 (ESP32) and logs two flow‑meter pulse inputs to SD. It shows live values on the display and can sync time via Wi‑Fi/NTP.

### Hardware
- Board: M5Stack Core2
- Sensors: 2x pulse‑output flow meters
- Wiring: connect sensor outputs to `SCL` and `SDA` pins (Grove port). Power sensors per specs; use level shifting if required.

### Build & Upload
- Arduino IDE: open `FlowRateLogger.ino`, select board “M5Stack Core2”, choose the correct port, then Upload.
- Arduino CLI (example FQBN):
  - Compile: `arduino-cli compile --fqbn esp32:esp32:m5stack-core2 .`
  - Upload: `arduino-cli upload -p <PORT> --fqbn esp32:esp32:m5stack-core2 .`
  - Monitor: `arduino-cli monitor -p <PORT> -c 115200`
Dependencies: ESP32 core, M5Stack `M5Core2` library, SD support.

### Usage
- Display shows time, per‑port counts, flow rate, and total flow.
- SD logs start automatically on first pulses and rotate after inactivity.
- Serial commands (newline‑terminated): `ut`/`updatetime` (sync time via Wi‑Fi/NTP), `so`/`serialout` (toggle serial log output).

### Log Format (SD)
File name: `/YYYYMMDD_HHMMSS00_flowrate.txt`
Line example: `$FLOWRATE,YYYYMMDD,HHMMSS00,c1,tc1,fr1,tf1,c2,tc2,fr2,tf2`
Where: `c`=count/sec, `tc`=total pulses, `fr`=flow rate (mL/s), `tf`=total flow (mL). Conversion uses `FLOW_RATE_PER_PULSE` in `FlowMeter.h`.

### Configuration
- Edit Wi‑Fi SSID/PASS and NTP server in `TImerController.ino` (recommend moving secrets to `secrets.h`, not committed).
- Timezone: adjust `gmtOffset_sec`.

---

## 日本語

### 概要
FlowRateLogger は M5Stack Core2（ESP32）上で動作し、2系統のフローメータのパルス入力をSDに記録します。画面にリアルタイム値を表示し、Wi‑Fi/NTP で時刻同期が可能です。

### ハードウェア
- 基板: M5Stack Core2
- センサー: パルス出力フローメータ ×2
- 配線: センサー出力を `SCL` と `SDA`（Grove ポート）へ接続。電源はセンサー仕様に従い、必要ならレベル変換を使用。

### ビルドと書き込み
- Arduino IDE: `FlowRateLogger.ino` を開き、ボード「M5Stack Core2」を選択して書き込み。
- Arduino CLI（例）:
  - ビルド: `arduino-cli compile --fqbn esp32:esp32:m5stack-core2 .`
  - 書き込み: `arduino-cli upload -p <PORT> --fqbn esp32:esp32:m5stack-core2 .`
  - シリアル: `arduino-cli monitor -p <PORT> -c 115200`
依存: ESP32 Core、M5Stack `M5Core2` ライブラリ、SD。

### 使い方
- 画面に時刻、各ポートのカウント、流量、積算流量を表示。
- パルス検出で自動的にSDへ記録を開始し、無流時はファイルをクローズ。
- シリアルコマンド（改行で送信）: `ut`/`updatetime`（Wi‑Fi/NTP 同期）、`so`/`serialout`（シリアル出力切替）。

### ログ形式（SD）
ファイル名: `/YYYYMMDD_HHMMSS00_flowrate.txt`
行例: `$FLOWRATE,YYYYMMDD,HHMMSS00,c1,tc1,fr1,tf1,c2,tc2,fr2,tf2`
意味: `c`=秒間カウント、`tc`=総パルス数、`fr`=流量[mL/s]、`tf`=積算[mL]。係数は `FlowMeter.h` の `FLOW_RATE_PER_PULSE` を使用。

### 設定
- `TImerController.ino` の SSID/PASS と NTP サーバを編集（機密は `secrets.h` へ分離推奨）。
- タイムゾーン: `gmtOffset_sec` を調整。
