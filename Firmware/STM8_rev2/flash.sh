#!/bin/sh
# Flash the built image over SWIM with stm8flash.
#
#   ./flash.sh                    # build/ll_basic_rev2.ihx, stlinkv2
#   PROG=stlinkv21 ./flash.sh     # ST-Link/V2-1 (Nucleo-style)
#   ./flash.sh path/to/other.ihx
#
# The SWIM cluster on the board (its RELATIVE geometry is frozen, because
# Bill's programming jig mates to it):
#
#       GND  ----  NRST
#       SWIM ----  +5V          columns 3.75 mm apart, rows 2.50 mm apart
#
#   J2 = GND, J3 = SWIM, J4 = NRST, J5 = +5V.
#
# Board power: the jig's +5V pad (J5) feeds the same +5V pour as VBUS, so
# the board can be powered from the programmer.  Note what that means for
# the CC sense -- with no USB-C cable attached both CC lines read ~0, so the
# firmware will report tier 0 and hold the dim default cap.  That is correct
# behaviour, not a fault; test the tiers with a real charger.
#
# DO NOT let anything write the option bytes.  Option bit AFR1 remaps
# TIM2_CH3 from PA3 to PD2 (which is AIN3 here), and that would break both
# IR capture and CC sense.  stm8flash only touches them if you pass
# `-s opt`, so don't.
set -eu

HERE=$(cd "$(dirname "$0")" && pwd)
IMG=${1:-"$HERE/build/ll_basic_rev2.ihx"}
PROG=${PROG:-stlinkv2}
PART=${PART:-stm8s003f3}

if command -v stm8flash >/dev/null 2>&1; then
    FLASH=stm8flash
else
    FLASH=$(ls "$HOME/AppData/Local/Arduino15/packages/sduino/tools/STM8Tools"/*/win/stm8flash.exe 2>/dev/null | sort | tail -1 || true)
    if [ -z "${FLASH:-}" ]; then
        echo "error: stm8flash not found." >&2
        echo "  Debian/Raspberry Pi OS:  sudo apt install stm8flash" >&2
        exit 1
    fi
fi

[ -f "$IMG" ] || { echo "error: $IMG does not exist -- run ./build.sh first" >&2; exit 1; }

echo "flashing $IMG  ($PART via $PROG)"
"$FLASH" -c "$PROG" -p "$PART" -s flash -w "$IMG"
echo "done."
