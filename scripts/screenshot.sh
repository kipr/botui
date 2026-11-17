#!/bin/bash
# Launches the built botui on a virtual 800x480 display inside the Docker
# image, optionally taps the screen, and saves a screenshot after each step.
#
#   docker compose run --rm build-botui scripts/screenshot.sh [X,Y ...]
#
# Each X,Y argument is a tap in screen pixels. Screenshots and botui's log go
# to build/screenshots/. Exits non-zero if botui isn't built or exits early.
set -u

if [ ! -f /.dockerenv ]; then
  echo "Run this inside the Docker image; see AGENTS.md." >&2
  exit 2
fi

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
BOTUI="$ROOT/deploy/botui"
OUT="$ROOT/build/screenshots"
SETTLE="${SETTLE:-3}"

if [ ! -x "$BOTUI" ]; then
  echo "$BOTUI not found; build first with ./build.sh." >&2
  exit 2
fi

# botui crashes at startup if wombat-os's network mode file is missing.
MODE_FILE=/home/kipr/wombat-os/configFiles/wifiConnectionMode.txt
if [ ! -f "$MODE_FILE" ]; then
  mkdir -p "$(dirname "$MODE_FILE")" /home/kipr/Documents/KISS
  printf 'MODE 2\nEVENT_MODE false\n' > "$MODE_FILE"
fi

rm -rf "$OUT"
mkdir -p "$OUT"

Xvfb :99 -screen 0 800x480x24 -nolisten tcp > /dev/null 2>&1 &
XVFB_PID=$!
export DISPLAY=:99
sleep 1

"$BOTUI" > "$OUT/botui.log" 2>&1 &
BOTUI_PID=$!

cleanup() {
  kill "$BOTUI_PID" "$XVFB_PID" 2> /dev/null
  wait 2> /dev/null
}
trap cleanup EXIT

check_running() {
  if ! kill -0 "$BOTUI_PID" 2> /dev/null; then
    wait "$BOTUI_PID"
    echo "botui exited early with status $? $1; log: $OUT/botui.log" >&2
    grep -v SPI_IOC_MESSAGE "$OUT/botui.log" | tail -20 >&2
    exit 1
  fi
}

sleep "$SETTLE"
check_running "at startup"
import -window root "$OUT/0.png"
echo "$OUT/0.png"

step=0
for tap in "$@"; do
  step=$((step + 1))
  xdotool mousemove "${tap%,*}" "${tap#*,}" click 1
  sleep "$SETTLE"
  check_running "after tap $step ($tap)"
  import -window root "$OUT/$step.png"
  echo "$OUT/$step.png"
done
