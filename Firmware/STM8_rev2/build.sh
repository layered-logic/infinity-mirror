#!/bin/sh
# Build the rev-2 STM8 firmware with SDCC.
#
# Works in Git Bash on this Windows box (where SDCC ships inside the sduino
# Arduino package but `make` does not exist) and on the Raspberry Pi flashing
# rig (apt install sdcc stm8flash).  The Makefile is the same build; this
# script is the one that runs everywhere without make.
#
#   ./build.sh                 # defaults
#   LEDS=60 ./build.sh         # different strip length
#   IR=0 ./build.sh            # drop the NEC decoder
#   BRINGUP=1 ./build.sh       # board bring-up image, ignores the button
#
set -eu

HERE=$(cd "$(dirname "$0")" && pwd)
OUT="$HERE/build"

LEDS=${LEDS:-32}
IR=${IR:-0}          # J8 is DNP -- no receiver fitted on any board yet
UART=${UART:-1}
BRINGUP=${BRINGUP:-0}

# The bring-up image gets its own name.  These two must never be confused on
# a bench: one of them ignores the button entirely.
if [ "$BRINGUP" = "1" ]; then NAME=ll_basic_rev2_bringup; else NAME=ll_basic_rev2; fi

# --- locate sdcc ---------------------------------------------------------
if command -v sdcc >/dev/null 2>&1; then
    SDCC=sdcc
else
    # sduino's bundled toolchain; newest build wins.
    SDCC=$(ls -d "$HOME/AppData/Local/Arduino15/packages/sduino/tools/sdcc"/build.*/bin/sdcc.exe 2>/dev/null | sort | tail -1 || true)
    if [ -z "${SDCC:-}" ]; then
        echo "error: sdcc not found." >&2
        echo "  Debian/Raspberry Pi OS:  sudo apt install sdcc" >&2
        echo "  Windows: install the sduino board package, or put sdcc on PATH" >&2
        exit 1
    fi
fi
echo "sdcc:   $("$SDCC" --version 2>&1 | head -1)"

# --- flash/RAM ceilings for the STM8S003F3P6 -----------------------------
CODE_MAX=8192
RAM_MAX=1024

mkdir -p "$OUT"
rm -f "$OUT/$NAME".*

"$SDCC" -mstm8 --std-sdcc99 \
    -DSTM8S003 -DUSE_STDINT \
    -DNUM_LEDS="$LEDS" -DENABLE_IR="$IR" -DENABLE_UART="$UART" -DENABLE_BRINGUP="$BRINGUP" \
    --opt-code-size \
    --code-size "$CODE_MAX" --iram-size "$RAM_MAX" \
    -I "$HERE/src" -I "$HERE/vendor" \
    -o "$OUT/$NAME.ihx" \
    "$HERE/src/main.c"

echo "config: NUM_LEDS=$LEDS ENABLE_IR=$IR ENABLE_UART=$UART ENABLE_BRINGUP=$BRINGUP"

# --- size report ---------------------------------------------------------
# SDCC's stm8 map lists one line per area.  Flash is everything that has to
# live in the 8 KB image; RAM is the static data (SSEG is the stack, which
# is not statically allocated but is reported for context).
awk '
  /^Area/ { inareas=1; next }
  inareas && NF>=4 && $1 ~ /^[A-Z]/ {
      name=$1; size=strtonum("0x" $3);
      if (name=="HOME"||name=="GSINIT"||name=="GSFINAL"||name=="CODE"||name=="CONST"||name=="INITIALIZER"||name=="XINIT")
          code+=size;
      else if (name=="DATA"||name=="INITIALIZED"||name=="OVERLAY")
          ram+=size;
  }
  END {
      printf "flash:  %d / %d bytes (%.1f%%)\n", code, CODE_MAX, 100.0*code/CODE_MAX;
      printf "ram:    %d / %d bytes static (the rest is stack headroom)\n", ram, RAM_MAX;
      if (code > CODE_MAX) { print "ERROR: over flash budget" > "/dev/stderr"; exit 1 }
      if (ram  > RAM_MAX)  { print "ERROR: over RAM budget"   > "/dev/stderr"; exit 1 }
  }
' CODE_MAX="$CODE_MAX" RAM_MAX="$RAM_MAX" "$OUT/$NAME.map"

# --- the check that the old sduino build could not make ------------------
# Vector 14 (TIM2 CAP/COM) lives at 0x8000 + 4*(14+1) = 0x803C.  If the IR
# handler is enabled but nothing is vectored there, the decoder is dead code
# -- which is exactly what happened under sduino, where main() lives in the
# core and never sees the handler's declaration.
if [ "$IR" = "1" ]; then
    if grep -qi "TIM2_CAP_COM_IRQHandler" "$OUT/$NAME.map"; then
        echo "vector: TIM2 CAP/COM handler present in the map"
    else
        echo "ERROR: ENABLE_IR=1 but TIM2_CAP_COM_IRQHandler is not in the map." >&2
        echo "       The IR decoder would never run.  See src/stm8s_it.h." >&2
        exit 1
    fi
fi

# --- ST Visual Programmer wants .hex or .s19 by extension ----------------
# Same bytes, two names.  STVP's file dialog filters on the extension and
# does not recognise SDCC's .ihx.
cp "$OUT/$NAME.ihx" "$OUT/$NAME.hex"

echo "output: $OUT/$NAME.ihx"
echo "        $OUT/$NAME.hex   (for ST Visual Programmer)"
