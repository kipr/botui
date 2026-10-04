#!/bin/bash
# Launches the built botui on a virtual 800x480 display inside the Docker
# image, optionally taps the screen, and saves a screenshot after each step.
#
#   docker compose run --rm build-botui scripts/screenshot.sh [X,Y ...]
#
# Each X,Y argument is a tap in screen pixels. Screenshots and botui's log go
# to build/screenshots/. Exits non-zero if botui isn't built or exits early.
set -u

if [ ! -f /.dockerenv ] && [ ! -f /run/.containerenv ]; then
  echo "Run this inside the Docker image; see AGENTS.md." >&2
  exit 2
fi

for tap in "$@"; do
  if [[ ! $tap =~ ^[0-9]+,[0-9]+$ ]]; then
    echo "Taps are X,Y in pixels, for example 400,126; got '$tap'." >&2
    exit 2
  fi
done

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
BOTUI="$ROOT/deploy/botui"
OUT="$ROOT/build/screenshots"
SETTLE="${SETTLE:-3}"

if [ ! -x "$BOTUI" ]; then
  echo "$BOTUI not found; build first with ./build.sh." >&2
  exit 2
fi

# botui crashes at startup without this file; the image provides a stub.
if [ ! -f /home/kipr/wombat-os/configFiles/wifiConnectionMode.txt ]; then
  echo "wifiConnectionMode.txt is missing; rebuild the image." >&2
  exit 2
fi

rm -rf "$OUT"
mkdir -p "$OUT"

BOTUI_PID=
Xvfb :99 -screen 0 800x480x24 -nolisten tcp > /dev/null 2>&1 &
XVFB_PID=$!
export DISPLAY=:99

cleanup() {
  kill ${BOTUI_PID:+"$BOTUI_PID"} "$XVFB_PID" 2> /dev/null
  wait 2> /dev/null
}
trap cleanup EXIT

for _ in $(seq 50); do
  xdotool getdisplaygeometry > /dev/null 2>&1 && break
  sleep 0.2
done
if ! xdotool getdisplaygeometry > /dev/null 2>&1; then
  echo "Xvfb didn't start on $DISPLAY." >&2
  exit 1
fi

"$BOTUI" > "$OUT/botui.log" 2>&1 &
BOTUI_PID=$!

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
  if ! xdotool mousemove "${tap%,*}" "${tap#*,}" click 1; then
    echo "Tap $step ($tap) failed." >&2
    exit 1
  fi
  sleep "$SETTLE"
  check_running "after tap $step ($tap)"
  import -window root "$OUT/$step.png"
  echo "$OUT/$step.png"
done
