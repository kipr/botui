botui
=====
[![Bob the Builder](https://github.com/kipr/botui/actions/workflows/bob.yml/badge.svg)](https://github.com/kipr/botui/actions/workflows/bob.yml)


Botui is a device-independent graphical interface designed initially for the Kovan controller.

The icons used throughout botui are from the [Font Awesome](https://fontawesome.com/icons?d=gallery) by © Fonticons, Inc.

Requirements
============
* [pcompiler ](https://github.com/kipr/pcompiler)
* CMake 2.6.0 or later
* [Qt >= 4.7.4](https://www.qt.io/download-qt-installer)
* Note: for Qt6, the apt packages needed are `qt6-base-dev qt6-declarative libqt6svg6-dev`

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
