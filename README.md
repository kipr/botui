# botui

Botui is a graphical interface for KIPR robots, including the Wombat controller.

The icons used throughout botui are from the [Font Awesome](https://fontawesome.com/icons?d=gallery) by © Fonticons, Inc.

# Requirements

- [libkar](https://github.com/kipr/libkar)
- [pcompiler ](https://github.com/kipr/pcompiler)
- CMake 3.10 or later
- Qt 6

# Build

# Cross-compile to the Wombat (Raspberry Pi 3b+)

Local build, tested on Debian 13.
Be aware that `:arm64` versions of packages often conflict with `x86_64` versions, so if these commands fail try the Docker build.

```bash
sudo dpkg --add-architecture arm64
sudo apt update
sudo apt install make cmake gcc-aarch64-linux-gnu g++-aarch64-linux-gnu qt6-base-dev:arm64  qt6-declarative-dev:arm64 libssl-dev:arm64 zlib1g-dev:arm64

cmake -Bbuild -DCMAKE_TOOLCHAIN_FILE=toolchain/aarch64-linux-gnu.cmake .
cmake --build build -j "$(nproc)" --target package-debian
```

Build with Docker:

```bash
docker build -t botui-builder .
docker run --rm --mount type=bind,source=.,destination=/src/ botui-builder sh -c 'cmake -B/src/build -DCMAKE_TOOLCHAIN_FILE=/src/toolchain/aarch64-linux-gnu.cmake /src && cmake --build /src/build -j "$(nproc)" --target package-debian'
```

## Old instructions

```bash
git clone https://github.com/kipr/botui
cd botui
mkdir build
cd build
cmake .. or /home/<container name>/qt-raspi/bin/qt-cmake -Ddocker_cross=ON .. (for docker cross compilation)
make -j4
sudo make install
```

## Touchscreen calibration on Wayland

Botui's calibration page updates the Labwc configuration at
`~/.config/labwc/rc.xml`. It calibrates the `TSC2007 Touchscreen` profile while
leaving Labwc's output mapping and mouse-emulation settings unchanged.

After five calibration taps, Botui saves the original configuration as
`~/.config/labwc/rc.xml.bak`, writes the candidate matrix, and runs
`labwc --reconfigure`. The new calibration must be confirmed by touching the
displayed target within 15 seconds. If it is not confirmed, Botui restores the
backup and reloads Labwc. The rollback timer runs inside Botui, so the backup
may need to be restored manually if Botui exits during confirmation.

## Screen inversion on Wayland

The GUI Settings button toggles HDMI-A-1 between normal and 180° orientation.
Botui updates both `~/.config/kanshi/config` and `config.init`. The device's
touch input follows output rotation, so screen inversion leaves the TSC2007
calibration in Labwc's `rc.xml` unchanged.

If `config` is empty, `config.init` supplies the initial output profile. Kanshi
and Labwc must already be running in the session, and `wlr-randr` must be
installed. When Botui is started with `sudo`, it uses the invoking user's
configuration directory and reloads that user's running Kanshi process.

The new orientation takes effect immediately. Confirm it on screen within 15
seconds or Botui restores the original files. The originals are saved as
`config.invert.pending.bak`, `config.init.invert.pending.bak`, and
`rc.xml.invert.pending.bak` beside
their respective files. If Botui exits during confirmation, it retries recovery
on its next launch until the Wayland session is ready. At startup Botui also
reapplies the saved output orientation if another boot step left it different.

# License

Botui is released under the terms of the GPLv3. For more information, see the LICENSE file.

Want to Contribute? Start Here!: 
https://github.com/kipr/KIPR-Development-Toolkit
