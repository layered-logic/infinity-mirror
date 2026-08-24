#!/bin/sh
# Host tests for the parts of the firmware that are pure logic.
#
# These compile the REAL headers with the host compiler -- not copies -- so a
# change to src/button_logic.h is what gets tested.  Run before every flash;
# it takes a second and it caught a long-press bug that would otherwise have
# been found by an EEPROM wearing out.
set -eu

HERE=$(cd "$(dirname "$0")" && pwd)
CC=${CC:-cc}
mkdir -p "$HERE/build"

echo "== button_logic =="
$CC -O2 -Wall -Wextra -std=c99 -o "$HERE/build/test_button" "$HERE/tools/test_button_logic.c"
"$HERE/build/test_button"
