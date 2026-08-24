# Layered Logic Basic -- STM8 rev-2 firmware
#
# Same build as ./build.sh, for hosts that have make (the Pi flashing rig).
# On the Windows box use ./build.sh -- there is no make there, but SDCC ships
# inside the sduino Arduino package and build.sh finds it.
#
#   make                     # default image
#   make LEDS=60             # different strip length
#   make IR=0                # drop the NEC decoder
#   make BRINGUP=1 flash     # bring-up image straight onto a new board
#   make test                # host tests for the pure-logic headers
#   make clean

NAME     := ll_basic_rev2
OUT      := build

LEDS     ?= 32
IR       ?= 0      # J8 is DNP -- no receiver fitted on any board yet
UART     ?= 1
BRINGUP  ?= 0

# STM8S003F3P6: 8 KB flash, 1 KB RAM, 128 B data EEPROM.
CODE_MAX := 8192
RAM_MAX  := 1024

SDCC     ?= sdcc
STM8FLASH?= stm8flash
PROG     ?= stlinkv2
PART     ?= stm8s003f3

CFLAGS := -mstm8 --std-sdcc99 \
          -DSTM8S003 -DUSE_STDINT \
          -DNUM_LEDS=$(LEDS) -DENABLE_IR=$(IR) -DENABLE_UART=$(UART) -DENABLE_BRINGUP=$(BRINGUP) \
          --opt-code-size \
          --code-size $(CODE_MAX) --iram-size $(RAM_MAX) \
          -Isrc -Ivendor

.PHONY: all clean flash size map test
all: $(OUT)/$(NAME).ihx

$(OUT)/$(NAME).ihx: src/main.c src/board.h src/config.h src/button_logic.h \n                    src/stm8s_it.h src/stm8s_conf.h vendor/stm8s.h
	@mkdir -p $(OUT)
	$(SDCC) $(CFLAGS) -o $@ src/main.c
	@cp $@ $(OUT)/$(NAME).hex      # ST Visual Programmer filters on .hex
	@$(MAKE) --no-print-directory size

size:
	@awk '/^Area/{a=1;next} a&&NF>=4&&$$1~/^[A-Z]/{ \
	    n=$$1; s=strtonum("0x" $$3); \
	    if(n=="HOME"||n=="GSINIT"||n=="GSFINAL"||n=="CODE"||n=="CONST"||n=="INITIALIZER"||n=="XINIT") c+=s; \
	    else if(n=="DATA"||n=="INITIALIZED"||n=="OVERLAY") r+=s } \
	  END{ printf "flash: %d / %d (%.1f%%)   ram: %d / %d\n", c, $(CODE_MAX), 100.0*c/$(CODE_MAX), r, $(RAM_MAX); \
	       if(c>$(CODE_MAX)||r>$(RAM_MAX)) exit 1 }' $(OUT)/$(NAME).map

# See flash.sh for the SWIM pad map and the "never write the option bytes"
# warning -- AFR1 would move TIM2_CH3 onto PD2, which is the CC-sense ADC pin.
flash: all
	$(STM8FLASH) -c $(PROG) -p $(PART) -s flash -w $(OUT)/$(NAME).ihx

map: all
	@grep -nE "TIM2_CAP_COM_IRQHandler|_main\b" $(OUT)/$(NAME).map

clean:
	rm -rf $(OUT)
