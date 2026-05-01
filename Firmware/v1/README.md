---
title: Firmware v1 — Layered Logic Infinity Mirror
type: engineering
phase: 2
status: scaffold — blink-level only
tags: [firmware, esp32, esp-idf, scaffold]
---

# Firmware v1

Greenfield proprietary firmware for the Layered Logic Infinity Mirror. Built on ESP-IDF with C. Replaces the ESPHome YAML prototype in `../ESP32/prototype_ESPHome_Version.yaml`.

Currently **scaffold only** — directory tree + blink-level `main.c` per variant. Real implementation work begins week 5.

## Architecture

See the spec docs in [../../docs/](../../docs/):
- [firmware-architecture-scoping.md](../../docs/firmware-architecture-scoping.md) — locked decisions
- [firmware-spec.md](../../docs/firmware-spec.md) — component-level spec
- [firmware-security.md](../../docs/firmware-security.md) — signing, OTA, auth, telemetry
- [control-protocol-spec.md](../../docs/control-protocol-spec.md) — wire protocol
- [button-interface.md](../../docs/button-interface.md) — UX contract

## Layout

```
Firmware/v1/
  core/                    # 80%+ of codebase — shared across variants
    led_driver/            # WS2812 via RMT
    pattern_interp/        # pattern runtime + interpreter
    button/                # debounce, gesture state machine
    state_bus/             # unified state + pub/sub
    provisioning/          # wifi_prov_mgr wrapper (BLE + SoftAP)
    transport/             # WebSocket + HTTP + BLE
    mdns/                  # discovery
    ota/                   # signed OTA + A/B rollback
    nvs/                   # persistence
    auth/                  # HMAC envelope (paired mode)
    telemetry/             # opt-in beacon
  variants/
    standard/              # Pro — no Matter
    matter/                # Pro+ — adds esp-matter stack
  boards/                  # compile-time hardware abstraction
    board.h                # abstract interface
    c6_devkit.h            # ESP32-C6-DevKitC-1 (both demo units)
    c3_devkit.h            # ESP32-C3-DevKitM-1 (V0 reference)
    prod_v1_pro.h          # Pro shipping PCB (stub — TBD at layout)
    prod_v1_pro_plus.h     # Pro+ shipping PCB (stub — TBD at layout)
  webapp/                  # device-hosted browser UI (embedded at build)
  tests/                   # host-side unit tests for pure-C modules
```

## Build targets (planned)

- `standard-c6_devkit` — Pro firmware on C6 dev board (demo)
- `standard-c3_devkit` — Pro firmware on C3 dev board (legacy V0 hardware)
- `matter-c6_devkit` — Pro+ firmware on C6 dev board (demo)
- `standard-prod_v1_pro` — Pro shipping (pending PCB)
- `matter-prod_v1_pro_plus` — Pro+ shipping (pending PCB)

## Status

Every source file is currently a stub. This scaffold exists to:
- Lock the directory structure so spec docs have a concrete target
- Make `idf.py build` work from day one (even if all it does is print "hello")
- Let CI spin up per-variant build jobs as modules get real implementations

## Building

PowerShell only — see [reference_esp_idf_setup memory](../../.claude/projects/C--Users-bowhi-Desktop-Independent-Study/memory/reference_esp_idf_setup.md). git-bash isn't supported by the IDF Windows installer's wrapper scripts.

```powershell
$env:IDF_TOOLS_PATH = "C:\Espressif"
& "C:\Espressif\frameworks\esp-idf-v5.5.4\export.ps1"
cd Firmware\v1
```

**First build in a fresh worktree** — two prerequisites the build won't auto-handle:

```powershell
# 1. Webapp dist must exist before idf.py build runs (webapp_assets component
#    embeds the gzipped output). Skip this and CMake fails with
#    "Webapp asset missing: webapp/dist/index.html.gz".
cd webapp
npm install
npm run build
cd ..

# 2. Pick the board + chip target. CMakeLists defaults to c6_devkit / esp32c6.
#    The XIAO ESP32-C3 prototype needs c3_devkit / esp32c3 instead — and
#    set-target must be passed BOTH the board flag and the chip target;
#    skipping it means the build silently uses the previous target from
#    the local sdkconfig. Verify before assuming:
idf.py -DLL_BOARD=c3_devkit set-target esp32c3
Select-String "^CONFIG_IDF_TARGET=" sdkconfig    # should print esp32c3

# 3. Build. Pass PROJECT_VER explicitly when iterating over OTA — esp_https_ota
#    rejects images whose version string equals the running version
#    (ESP_ERR_INVALID_VERSION), and `git describe --dirty` produces the same
#    suffix when only uncommitted changes differ between iterations.
idf.py -DLL_BOARD=c3_devkit -DPROJECT_VER="iter-$(Get-Date -Format yyMMdd-HHmm)" build
```

**Subsequent builds** in the same worktree — just `idf.py build`. The board + target persist in the (gitignored) sdkconfig.

**Switching chip targets** (e.g. C3 ↔ C6) — must `Remove-Item sdkconfig` first; `set-target` doesn't reliably overwrite a sdkconfig from a previous chip.

## Flashing

USB: `idf.py -p COM<n> flash monitor`

OTA: see [docs/ota-dev-runbook.md](../../docs/ota-dev-runbook.md) — host the bin via `python -m http.server 8000` in `build/`, send `start_ota` from the app or wscat.
