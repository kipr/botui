botui
=====
[![Build](https://github.com/kipr/botui/actions/workflows/build.yml/badge.svg)](https://github.com/kipr/botui/actions/workflows/build.yml)


Botui is the touchscreen interface of the KIPR Wombat robot controller. It was designed initially for the Kovan controller.

The icons used throughout botui are from the [Font Awesome](https://fontawesome.com/icons?d=gallery) by © Fonticons, Inc.

Requirements
============
* [libkar](https://github.com/kipr/libkar), [pcompiler](https://github.com/kipr/pcompiler), and [libwallaby](https://github.com/kipr/libwallaby)
* CMake 3.x (CMake 4 rejects this project's `cmake_minimum_required`)
* Qt 6, OpenSSL, and zlib development files


Build with Docker
=======
```
docker compose build
docker compose run --rm build-botui
```

This builds libkar, pcompiler, and libwallaby into the image, then builds botui to `deploy/botui`. Use `run` rather than `up`: `up` reports success even when the build fails. The result runs inside the container (see [AGENTS.md](AGENTS.md#verifying-a-change)) but not on a Wombat.


Installation
=======
```
git clone https://github.com/kipr/botui
cd botui
mkdir build
cd build
cmake .. or /home/<container name>/qt-raspi/bin/qt-cmake -Ddocker_cross=ON .. (for docker cross compilation)
make -j4
sudo make install
```

License
=======

Botui is released under the terms of the GPLv3. For more information, see the LICENSE file.

Want to Contribute? Start Here!: 
https://github.com/kipr/KIPR-Development-Toolkit

Then read [CONTRIBUTING.md](CONTRIBUTING.md) for how we work, including with AI coding agents.
