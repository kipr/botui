#!/bin/bash
# Launches the built botui on a virtual 800x480 display inside the Docker
# image, optionally taps the screen, and saves a screenshot after each step.
#
#   docker compose run --rm build-botui scripts/screenshot.sh [X,Y ...]
#
# Each X,Y argument is a tap in screen pixels. Screenshots and botui's log go
# to build/screenshots/. Exits non-zero if botui isn't built, shows no window
# within STARTUP_TIMEOUT seconds, exits early, or a screenshot capture fails.
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
STARTUP_TIMEOUT="${STARTUP_TIMEOUT:-30}"

if [ ! -x "$BOTUI" ]; then
  echo "$BOTUI not found; build first with ./build.sh." >&2
  exit 2
fi

rm -rf "$OUT"
mkdir -p "$OUT"

BOTUI_PID=
Xvfb :99 -screen 0 800x480x24 -nolisten tcp > /dev/null 2>&1 &
XVFB_PID=$!
export DISPLAY=:99

# botui's UI thread can be blocked in a system command and never act on
# SIGTERM, so fall back to SIGKILL instead of waiting on it forever.
stop() {
  kill "$1" 2> /dev/null || return
  for _ in $(seq 25); do
    kill -0 "$1" 2> /dev/null || return
    sleep 0.2
  done
  kill -9 "$1" 2> /dev/null
}

# stderr is dropped because bash reports how each job died, and botui aborts
# on SIGTERM even after a good run.
cleanup() {
  # botui first, so its log doesn't end with an X11 connection error.
  [ -n "$BOTUI_PID" ] && stop "$BOTUI_PID"
  stop "$XVFB_PID"
  wait
} 2> /dev/null
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

deadline=$((SECONDS + STARTUP_TIMEOUT))
until xdotool search --onlyvisible --pid "$BOTUI_PID" > /dev/null 2>&1; do
  check_running "at startup"
  if [ "$SECONDS" -ge "$deadline" ]; then
    echo "botui showed no window within ${STARTUP_TIMEOUT}s; log: $OUT/botui.log" >&2
    grep -v SPI_IOC_MESSAGE "$OUT/botui.log" | tail -20 >&2
    exit 1
  fi
  sleep 0.2
done
sleep "$SETTLE"
check_running "at startup"
if ! import -window root "$OUT/0.png"; then
  echo "Screenshot capture failed: $OUT/0.png" >&2
  exit 1
fi
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
  if ! import -window root "$OUT/$step.png"; then
    echo "Screenshot capture failed: $OUT/$step.png" >&2
    exit 1
  fi
  echo "$OUT/$step.png"
done
