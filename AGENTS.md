# AGENTS.md

Instructions for AI coding agents working in this repository. The contribution
workflow for everyone, human or agent, is in [CONTRIBUTING.md](CONTRIBUTING.md).

## What this is

botui is the touchscreen interface of the KIPR Wombat robot controller: a
Qt 6 Widgets app that runs full screen on the Wombat's 800×480 display. It's
the first thing every team and classroom sees. It lists and runs student
programs, shows motors, servos, sensors, and the camera, configures Wi-Fi, and
starts updates, backups, and firmware reflashes. A regression here reaches
every Wombat that updates.

The Wombat is a Raspberry Pi 3B or 3B+ (arm64) running KIPR's
[wombat-os](https://github.com/kipr/wombat-os) image. botui reaches the robot
hardware through libkipr from [libwallaby](https://github.com/kipr/libwallaby),
which talks to the STM32 co-processor running
[Wombat-Firmware](https://github.com/kipr/Wombat-Firmware). It reaches the
network through NetworkManager over D-Bus, and most of the operating system
through shell commands and wombat-os scripts. The code started on the Kovan
controller and went through the Wallaby, so all three names appear in it.

## How we work

Follow [CONTRIBUTING.md](CONTRIBUTING.md). In particular:

- Before editing files, present a plan (what will change, which files, how
  you'll verify it) and wait for approval from the person you're working with.
- Keep the change to what was asked, within the existing architecture below.
  Don't refactor, reformat, or rename code you weren't asked to touch, and
  don't add code for hypothetical future needs.
- Comment only what the code can't say for itself: why a system command or
  path is needed, Qt or NetworkManager quirks, the reason behind a non-obvious
  choice. Don't narrate what the code does, and don't leave commented-out code.
- Update README.md, this file, and CONTRIBUTING.md when your change makes them
  wrong.
- Branch names start with the name of the person who owns the change:
  `<name>/<topic>`.
- Report verification honestly. Say what you built, launched, and looked at,
  and state plainly what wasn't tested on a Wombat.

## Build

The canonical build runs in Docker, so no local toolchain is needed:

```bash
docker compose build
docker compose run --rm build-botui
```

Use `run`, not `up`: `docker compose up` doesn't return the build's exit code,
so a failed build looks like success. The executable lands in `deploy/botui`
(set by `EXECUTABLE_OUTPUT_PATH`); intermediate files are in `build/`. CI runs
the same commands on every pull request.

The image is Ubuntu 24.04 with its Qt 6.4 packages. It builds botui's three
KIPR dependencies from source at the versions wombat-os ships, set by the
`ARG`s in the `Dockerfile`: [libkar](https://github.com/kipr/libkar) (`.kar`
archives), [pcompiler](https://github.com/kipr/pcompiler), and libwallaby.
libwallaby downloads and builds OpenCV and libav, so the first
`docker compose build` takes several minutes; later builds reuse the cached
layers.

This build checks that botui compiles and lets you run it in the container
(see below). **Its binary doesn't run on a Wombat**, which has its own Qt 6
build and older system libraries. Release packages are cross-compiled
elsewhere, with the README's `qt-cmake -Ddocker_cross=ON` command and
`cpack`; the shipped binary was built with GCC 9.4 against a Qt 6 tree at
`/home/qtpi/qt-raspi`. That environment isn't in this repository or
wombat-os. The package version comes from `KIPR_VERSION_*` in
`CMakeLists.txt`, and wombat-os ships the result as
`updateFiles/pkgs/botui.deb`. The `debian/` directory is unused: it describes
a Qt 4 armhf package from 2020.

To build natively you need Linux, CMake 3.x, the Qt 6 packages and libraries
listed in the `Dockerfile`, and the three KIPR libraries installed to
`/usr/local`. CMake 4 rejects this project's
`cmake_minimum_required(VERSION 2.8.11)`, which is one reason the image is
pinned.

There's no clean target; delete `build/` and `deploy/` instead. On Linux the
container creates them as root, so delete them from inside it:
`docker compose run --rm build-botui rm -rf build deploy`. Also delete
`build/` when switching between Docker and native builds, because the CMake
cache records absolute paths. The devcontainer mounts the repository at the
same path as `docker compose`, so the two share `build/`. Sources are
collected with `file(GLOB)`, so a new file is only picked up when CMake
re-runs; `build.sh` re-runs it every time.

The build has 13 compiler warnings. Don't add new ones in files you touch.

## Verifying a change

There are no automated tests. Before calling a change done:

1. The Docker build succeeds without new warnings.
2. botui launches and the screens you touched look right. Run it on a virtual
   display inside the image and look at the screenshots:

   ```bash
   docker compose run --rm build-botui scripts/screenshot.sh 400,383 400,30
   ```

   Each `X,Y` argument taps the 800×480 screen, and a screenshot is saved
   before the first tap and after each one, to `build/screenshots/0.png`,
   `1.png`, and so on, with botui's output in `botui.log`. The script fails if
   botui exits early. On the home screen, Programs is at `400,126`, File
   Manager `400,211`, Motors and Sensors `400,297`, Settings `400,383`, and
   About `137,30`. Screens one level down have Home at `400,30`.

   Programs lists executables at
   `/home/kipr/Documents/KISS/<user>/<project>/bin/botball_user_program`
   inside the container. To have something to run, create one in the same
   `docker compose run` with `bash -c` before calling the script.

   There's no hardware, NetworkManager, or wombat-os in the container. libkipr
   logs `SPI_IOC_MESSAGE` errors continuously, the battery reads empty, network
   actions do nothing, and About shows `??` for the version. A screen that
   depends on those may misbehave before your change too; compare with
   `master` before blaming it.
3. Changes to behavior on the Wombat need testing on a Wombat: hardware,
   networking, updates, backups, and anything that runs a system command.
   Agents don't deploy to hardware. Tell the person you're working with what
   to test, and say in the pull request whether it was tested.

On a Wombat, botui runs as the system service `botui.service` from
`/usr/local/bin/botui` on display `:0`. The unit sets `HOME=/home/kipr` but no
`User=`, so botui runs as root. Follow its log with
`sudo journalctl -b -f -u botui.service`.

## Architecture

### Startup and navigation

`main.cpp` installs the custom `MechanicalStyle`, loads a translation if one
is set, creates the `Wombat::Device`, starts `NetworkManager` and
`KovanSerialBridge`, and presents `HomeWidget`.

Every screen is a separate top-level widget. `RootController` (a singleton)
keeps them on a stack: `presentWidget()` pushes a screen, fixes it at 800×480,
shows it full screen, and hides the one below; `dismissWidget()` pops it and
deletes it if it's owned. `presentDialog()` runs a modal dialog. Most screens
derive from `StandardWidget` and call `performStandardSetup(title)`, which adds
the `MenuBar` (Home, Back on screens more than one level deep, and
per-screen actions) and the `StatusBar` (battery, network, Event Mode
label).

Each screen is a class in `src/<Name>.cpp` and `include/botui/<Name>.h` with a
Qt Designer form in `ui/<Name>.ui`, compiled by `uic` into `ui_<Name>.h`. Text
input goes through the on-screen `KeyboardDialog` and `NumpadDialog`; there's
no physical keyboard.

### Device layer (`devices/wombat/`)

`Device` (`include/botui/Device.h`) is the hardware interface, implemented by
`Wombat::Device`. Battery level and the A/B/C and X/Y/Z program buttons come
from libkipr. Settings such as `fullscreen` and the battery type are a
`QDataStream` map in `~/botui_settings`; separately, `QSettings` stores the
locale and whether networking is on. The version and copyright year are read
from `/usr/share/kipr/`, and the ID and serial come from
`/usr/bin/wallaby_get_id.sh` and `wallaby_get_serial.sh`. CMake defines
`WOMBAT` (the default `wombat` option); nothing defines `WALLABY`, so code
under `#ifdef WALLABY` is never compiled.

### Running programs

botui doesn't compile programs; the KIPR IDE on the Wombat does.
`ArchivesModel` lists projects under `/home/kipr/Documents/KISS/` (`pathToKISS`
in `Config.cpp`) that have a `bin/botball_user_program`. `ProgramsWidget::run()`
starts it through `/usr/bin/script` so the program gets a pseudo-terminal and
its output isn't held in a buffer. `Program` (a singleton) owns the `QProcess`;
`ProgramWidget` shows its output in a `ConsoleWidget`, which treats `\f` as
clear and `\a` as a flash, and shows the program's A/B/C/X/Y/Z buttons.
`KovanSerialBridge` also listens on the local socket `org.kipr.botui.Run` and
runs any executable path a client writes to it.

The older path that compiled `.kar` archives with pcompiler
(`KissCompileProvider`, `ConcurrentCompile`, the compile buttons) is disabled
with `FIXME` comments. Don't revive it as part of an unrelated change.

### Networking

`NetworkManager` (a singleton in `src/NetworkManager.cpp`) drives
NetworkManager over the system D-Bus. The proxy classes
(`OrgFreedesktopNetworkManager*Interface`) are generated at build time from
`dbus/*.xml` into the `network_manager_dbus` library; only the interfaces
listed in `dbus/CMakeLists.txt` are generated. `main.cpp` starts networking
under `#ifdef QT_DBUS_LIB`, which is defined because `network_manager_dbus`
links Qt DBus publicly.

The Wombat's access point is named `<serial>-wombat`, and its password is the
first six hex digits of SHA-256 of the device ID followed by `00`
(`AP_NAME` and `AP_PASSWORD`). Changing either changes the network name or
password on every Wombat that updates, and wombat-os relies on the name.

The Wi-Fi mode (`MODE 0` access point, `1` client, `2` off) and Event Mode are
stored in `/home/kipr/wombat-os/configFiles/wifiConnectionMode.txt`, which
botui reads with `grep` and `awk` and writes with `sed`. Event Mode, toggled on
the About screen, turns the access point off for competitions; its UI state
lives in globals (`eventModeEnabled` in `StandardWidget.cpp`, `eventModeLabel`
in `StatusBar.cpp`, `backgroundImageLabel` in `HomeWidget.cpp`).

### Files and scripts shared with other repositories

Changing one of these needs a matching change in the other repository:

| What | Shared with |
|---|---|
| Access point connection name `<serial>-wombat` | wombat-os `configFiles/balancer.sh` brings the connection up by that name |
| `/home/kipr/wombat-os/configFiles/wifiConnectionMode.txt` (format above) | wombat-os provides the default file |
| `/home/kipr/updateMe.sh`, run by Update for USB and network updates | wombat-os `updateFiles/files/updateMe.sh` |
| `/home/kipr/wombat-os/Backup/backup.sh` and `restore.sh` | wombat-os `Backup/` |
| `wallaby_set_serial.sh` and `wallaby_flash` in `/home/kipr/wombat-os/flashFiles/`, run by the Factory screen | wombat-os `flashFiles/`; `wallaby_flash` flashes Wombat-Firmware |
| `/usr/bin/wallaby_get_id.sh` and `wallaby_get_serial.sh` | on the Wombat image; wombat-os keeps copies in `flashFiles/` |
| `/usr/share/kipr/board_fw_version.txt` and `board_copyright_year.txt` | copied by wombat-os's update script |
| Camera channel configs in `/etc/botui/channels/` | libwallaby's camera API reads them (`ConfigPath`) |
| `/usr/local/bin/botui` and the service environment | wombat-os `configFiles/botui.service` |

Because botui runs as root, these scripts and its own commands (some
through `sudo`) can change anything on the Wombat.

### Resources

`rc/*.qrc` compiles icons, fonts, and QML into the binary. Font Awesome icons
resolve to a doubled path because the `.qrc` prefix repeats the directory:
`:/icons/fontawesome/solid/icons/fontawesome/solid/<name>.svg`. The QML
files are the lock and loading screens. The `ts/` translations aren't
compiled: `QM_FILES` is never set, so the build installs no `.qm` files to
`/etc/botui/locale`.

### Legacy code

Every `.cpp` in `src/` is compiled, but some classes are never shown or
instantiated: `FirstRunWizard`, `TestWizard` and its pages, `PairWidget`,
`PidTunerWidget`, `AccessoriesWidget`, `CreateWidget`, `DepthSensorWidget`,
`MotorsWidget`, `SerialIODevice`, `ConnectionedARDroneInputProvider`,
`CommunicationSettingsWidget`, and `KissIdeSettingsWidget`. `SystemPrefix`
(rooted at `/kovan`) and the other `/kovan` paths are Kovan leftovers that do
nothing useful on a Wombat. Check that a screen is reachable from `HomeWidget`
before changing it to fix a bug.

## Conventions and gotchas

- **Don't reformat.** Files mix tabs and two- or four-space indentation and
  several brace styles, and there's no formatter config. Whole-file
  reformatting has landed alongside small fixes (`e3dd18a`, for example),
  which buries the change in review. Match each file's existing style and
  check `git diff --stat` for whole-file churn.
- **String-based connections fail at runtime.** Most connections use
  `SIGNAL()` and `SLOT()`. A wrong signature compiles, then logs
  `QObject::connect: No such signal` (or slot) and does nothing. Check
  `botui.log` after exercising the screen. Function-pointer `connect()` calls
  are checked by the compiler.
- **The UI thread blocks.** Many handlers wait on `QProcess` or D-Bus calls,
  and the Factory screen spins on `processEvents()` while flashing. The
  touchscreen freezes for as long as the command runs.
- **Paths are the Wombat's.** `/home/kipr`, wombat-os, and the system paths
  above are hard-coded. Off the Wombat, most of them don't exist; the image
  provides a stub Wi-Fi mode file and an empty KISS directory.
- **Some functions fall off the end.** The `-Wreturn-type` warnings mark
  functions such as `NetworkManager::eventModeState()` that return nothing
  when their file read fails. If `wifiConnectionMode.txt` is missing, the
  Docker build crashes at startup on a trap instruction (`SIGTRAP` on arm64,
  `SIGILL` on x86_64).
- **Fixed 800×480 layout.** `RootController::constrain()` fixes every screen at
  that size. Design `.ui` files for it, with touch-sized controls.

## Reference

- [wombat-os](https://github.com/kipr/wombat-os): the Wombat image's services,
  scripts, and update packages.
- [libwallaby](https://github.com/kipr/libwallaby): libkipr, the hardware API
  botui and student programs use.
- [Wombat-Firmware](https://github.com/kipr/Wombat-Firmware): the co-processor
  firmware the Factory screen flashes.
- The [KIPR Development Toolkit](https://github.com/kipr/KIPR-Development-Toolkit)
  has the Wombat Developer Manual.
