# botui devcontainer

A Debian 13 container that cross-compiles botui for the Wombat (aarch64) and
packages it as a `.deb`, with Claude Code and Codex installed.

## What's in the image

The image is the `dev` stage of the repo's root [`Dockerfile`](../Dockerfile):

- **`toolchain` stage** (shared with the default `builder` target): the aarch64
  cross compilers, CMake, Qt 6 and OpenSSL/zlib for arm64, and the KIPR
  packages libkar, pcompiler and kipr (libwallaby).
- **`dev` stage**: everyday tools (git, fish, ripgrep, fd, fzf, jq, gnupg, ssh
  client), bubblewrap for the agent sandboxes, Claude Code, and Codex.
- User `code` (UID 1000, home `/home/code`, shell fish).

`docker build .` with no target still builds the `builder` stage the main README uses.

## Opening it

- VS Code: **Dev Containers: Reopen in Container**.
- CLI: `devcontainer up --workspace-folder .` then
  `devcontainer exec --workspace-folder . fish`.

Build the image alone with:

```bash
docker build --target dev -t botui-dev .
```

Rebuild the container after changing the `Dockerfile` or `devcontainer.json`,
or to pick up new Claude Code / Codex releases.

## Building botui

Inside the container, from the workspace root:

```bash
cmake -Bbuild -DCMAKE_TOOLCHAIN_FILE=toolchain/aarch64-linux-gnu.cmake .
cmake --build build -j "$(nproc)" --target package-debian
```

This writes the binaries to `deploy/` and the package to `build/botui-2.0.1-Linux.deb`.
If `build/` was configured on the host or with the plain Docker build (source at `/src`),
CMake will complain that the source directory changed. Delete `build/` or use another
build directory.

The tests build for aarch64 too, so `ctest` can't run them in this x86_64 container.

## Agent CLIs

Claude Code and Codex keep their state in two shared named volumes:

| Volume          | Mounted at           |
|-----------------|----------------------|
| `claude-config` | `/home/code/.claude` |
| `codex-config`  | `/home/code/.codex`  |

Every devcontainer that uses these volume names shares them, so you log in once per
machine and the login survives rebuilds. Your host's `~/.claude` and `~/.codex`
(settings, skills, plugins, memories, sessions) are not shared. Project config
(`.claude/`, `AGENTS.md`) comes with the workspace.

Containers use the volumes at the same time, so sessions running in two containers
write to the same files.

First-time login and smoke test:

```bash
claude          # follow the login prompt, then: claude -p "say hi"
codex login     # then: codex exec "say hi"
codex sandbox -- true   # checks the sandbox works
```

## Caveats

- **Sandbox security options.** The agent sandboxes use bubblewrap, so the
  container runs with `seccomp=unconfined` and `systempaths=unconfined`. This turns
  off Docker's syscall filter and its masking of `/proc` paths for this container.
- **Podman / SELinux.** Tested with Docker on Fedora, where the workspace mount works
  without changes. If the workspace is denied under Podman or an SELinux-enabled
  daemon, add `--security-opt=label=disable` to `runArgs`.
- **Flashing a Wombat.** The container has no USB device access. Copy
  the `.deb` from `build/` to the robot from the host.
- **KIPR package versions** are hard-coded with checksums in the `Dockerfile`
  (see its TODO).
