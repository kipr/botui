# Stages:
#   toolchain  aarch64 cross toolchain, Qt 6 (arm64) and the KIPR libraries
#   dev        toolchain + dev tools and agent CLIs (used by .devcontainer/)
#   builder    the default target, used by the README's Docker build
FROM debian:13.6 AS toolchain

RUN dpkg --add-architecture arm64 \
    && apt update \
    && apt install -y \
        cmake \
        dpkg-dev \
        file \
        g++-aarch64-linux-gnu \
        gcc-aarch64-linux-gnu \
        make \
        qt6-base-dev:arm64 \
        qt6-declarative-dev:arm64 \
        libssl-dev:arm64 \
        wget \
        zlib1g-dev:arm64 \
    && rm -rf /var/lib/apt/lists/*

# TODO Don't hardcode
RUN wget 'https://github.com/twells46/libkar/releases/download/v1.0.1/libkar-1.0.1-Linux.deb' 'https://github.com/twells46/pcompiler/releases/download/v1.0.1/pcompiler-1.0.1-Linux.deb' 'https://github.com/twells46/libwallaby/releases/download/v1.2.4/kipr-1.2.4-Linux.deb' \
    && printf 'b925d91433a08cd38d90bb4dfc9cb765e143f45ec7b6c87f177d1b82b16a1ebd libkar-1.0.1-Linux.deb\naee3d1385138a4a2eea339283a226db02c1efb08949e52bb919d92b32db37308 pcompiler-1.0.1-Linux.deb\n29dbfe1425fd84f8d974930f25fcccd64cd858a5ae5f12ecc65974c03bbcc27b kipr-1.2.4-Linux.deb' > pkgs.CHECKSUM \
    && sha256sum -c pkgs.CHECKSUM \
    && apt install ./libkar-1.0.1-Linux.deb ./pcompiler-1.0.1-Linux.deb ./kipr-1.2.4-Linux.deb \
    && rm libkar-1.0.1-Linux.deb pcompiler-1.0.1-Linux.deb kipr-1.2.4-Linux.deb pkgs.CHECKSUM


FROM toolchain AS dev

# gnupg: signing commits/tags inside the container
RUN apt-get update \
    && apt-get install -y --no-install-recommends \
        bubblewrap ca-certificates curl fd-find fish fzf git gnupg jq openssh-client ripgrep wget \
    && ln -s "$(command -v fdfind)" /usr/local/bin/fd \
    && rm -rf /var/lib/apt/lists/*

RUN groupadd --gid 1000 code \
    && useradd --uid 1000 --gid 1000 --create-home --shell /usr/bin/fish code

USER code
# Agent state lives in named volumes; a new volume inherits this ownership.
# CLAUDE_CONFIG_DIR also moves ~/.claude.json inside the volume.
ENV CLAUDE_CONFIG_DIR=/home/code/.claude \
    CODEX_HOME=/home/code/.codex \
    PATH=/home/code/.local/bin:$PATH
RUN mkdir -p "$CLAUDE_CONFIG_DIR" "$CODEX_HOME" \
    && curl -fsSL https://claude.ai/install.sh | bash \
    && claude --version \
    # The Codex installer puts its package under CODEX_HOME; point it outside
    # the volume so the binary stays in the image.
    && curl -fsSL https://chatgpt.com/codex/install.sh \
        | CODEX_HOME=/home/code/.local/share/codex CODEX_NON_INTERACTIVE=1 sh \
    && codex --version


# Keep this stage last: it is the default target for `docker build .`
FROM toolchain AS builder

RUN groupadd --gid 1000 kipr \
    && useradd --uid 1000 --gid 1000 kipr

USER kipr:kipr
