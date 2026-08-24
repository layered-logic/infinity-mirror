#!/bin/sh
# Flash from THIS Windows box with the ST-LINK/V2 via ST Visual Programmer's
# command-line driver.  Uses ST's own ST-Link driver, so nothing has to be
# rebound to WinUSB and the STVP GUI keeps working.
#
#   ./flash_stvp.sh
#   DEVICE=STM8S003F3 ./flash_stvp.sh
#
# On the Pi use flash.sh (stm8flash) instead -- STVP is Windows-only.
#
# ---------------------------------------------------------------------------
# THE TWO FLAGS THAT MATTER, and why they are not optional:
#
#   -no_progOption   STVP programs the OPTION BYTE area by default, from
#                    whatever Option.hex it last had.  Option bit AFR1 remaps
#                    TIM2_CH3 from PA3 to PD2 -- and PD2 is the CC-sense ADC
#                    input on this board.  Writing option bytes blind can
#                    take out the current budget (and IR, when J8 is fitted).
#                    Option bytes must stay unprogrammed.
#
#   -no_progData     DATA MEMORY is the 128-byte EEPROM the firmware uses for
#                    saved colour/pattern/brightness.  Nothing here should
#                    write it; the firmware manages it.
# ---------------------------------------------------------------------------
set -eu

HERE=$(cd "$(dirname "$0")" && pwd)
IMG=${1:-"$HERE/build/ll_basic_rev2.hex"}

# Must match STVP's device dropdown exactly.  Open the GUI once and read it
# off if this is rejected -- STVP's device database is an obfuscated .cnf, so
# the list is not greppable.
DEVICE=${DEVICE:-STM8S003F3}
STVP=${STVP:-"/c/Program Files (x86)/STMicroelectronics/st_toolset/stvp/STVP_CmdLine.exe"}

[ -f "$IMG" ]  || { echo "error: $IMG missing -- run ./build.sh first" >&2; exit 1; }
[ -x "$STVP" ] || { echo "error: STVP_CmdLine not at $STVP" >&2; exit 1; }

case "$IMG" in
  *.hex|*.s19) ;;
  *) echo "error: STVP loads .hex or .s19 by extension, not $IMG" >&2; exit 1 ;;
esac

echo "flashing $IMG  ($DEVICE, ST-LINK/SWIM)"
"$STVP" \
    -BoardName=ST-LINK -Port=USB -ProgMode=SWIM -Device="$DEVICE" \
    -no_loop -verbose -progress \
    -erase -verif \
    -no_progData -no_progOption \
    -FileProg="$(cygpath -w "$IMG" 2>/dev/null || echo "$IMG")"
