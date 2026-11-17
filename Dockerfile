# Pinned so the toolchain doesn't change underneath the build. Newer images ship
# CMake 4, which rejects the cmake_minimum_required(VERSION 2.8.x) in botui,
# libkar, and pcompiler. Ubuntu 24.04 provides CMake 3.28 and Qt 6.4.
FROM ubuntu:24.04 AS botui-base

ENV TZ=America/Chicago
RUN ln -snf /usr/share/zoneinfo/$TZ /etc/localtime && echo $TZ > /etc/timezone

RUN apt-get update && \
    apt-get install -y \
    build-essential \
    ca-certificates \
    cmake \
    git \
    imagemagick \
    libssl-dev \
    libx11-dev \
    qt6-base-dev \
    qt6-base-dev-tools \
    qt6-declarative-dev \
    xdotool \
    xvfb \
    zlib1g-dev && \
    apt-get clean && \
    rm -rf /var/lib/apt/lists/*

# The KIPR libraries botui links against, at the versions wombat-os ships.
# libwallaby's camera module downloads and builds OpenCV and libav, so this
# layer takes a while the first time.
ARG LIBKAR_REF=v1.0.0
ARG PCOMPILER_REF=v1.0.0
ARG LIBWALLABY_REF=v1.2.1

RUN git clone --depth 1 --branch $LIBKAR_REF https://github.com/kipr/libkar.git /tmp/libkar && \
    cmake -S /tmp/libkar -B /tmp/libkar/build && \
    cmake --build /tmp/libkar/build -j "$(nproc)" && \
    cmake --install /tmp/libkar/build && \
    rm -rf /tmp/libkar

RUN git clone --depth 1 --branch $PCOMPILER_REF https://github.com/kipr/pcompiler.git /tmp/pcompiler && \
    cmake -S /tmp/pcompiler -B /tmp/pcompiler/build && \
    cmake --build /tmp/pcompiler/build -j "$(nproc)" && \
    cmake --install /tmp/pcompiler/build && \
    rm -rf /tmp/pcompiler

# libwallaby's tests stay on: with_tests=OFF fails to configure at v1.2.1.
# Its bundled libav has x86 inline assembly that current binutils rejects, so
# libav is built without assembly; nothing here depends on its speed.
RUN git clone --depth 1 --branch $LIBWALLABY_REF https://github.com/kipr/libwallaby.git /tmp/libwallaby && \
    sed -i 's/--disable-filters/--disable-filters --disable-asm/' \
        /tmp/libwallaby/module/camera/dependencies/libav/CMakeLists.txt && \
    grep -q -- --disable-asm /tmp/libwallaby/module/camera/dependencies/libav/CMakeLists.txt && \
    cmake -S /tmp/libwallaby -B /tmp/libwallaby/build \
        -Dwith_python_binding=OFF \
        -Dwith_xml_binding=OFF \
        -Dwith_documentation=OFF && \
    cmake --build /tmp/libwallaby/build -j "$(nproc)" && \
    cmake --install /tmp/libwallaby/build && \
    ldconfig && \
    rm -rf /tmp/libwallaby

WORKDIR /home/dev/botui

CMD ["bash", "build.sh"]

FROM botui-base AS devcontainer

RUN apt-get update \
    && apt-get install -y \
        curl \
        fd-find \
        file \
        fish \
        fzf \
        gdb \
        gnupg \
        ripgrep \
    && apt-get clean \
    && rm -rf /var/lib/apt/lists/*

# Final stage is default, so compose-based build is unaffected
FROM botui-base AS botui
