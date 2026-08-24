/*
 * stm8s_it.h -- interrupt handler declarations.
 *
 * WHY THIS FILE EXISTS, and why the old sduino build could never have had
 * working IR:
 *
 *   SDCC emits an interrupt vector table entry only for handlers it has
 *   SEEN DECLARED while compiling the translation unit that contains
 *   main().  A handler defined anywhere else compiles cleanly, links
 *   cleanly, and is simply never wired to a vector -- it silently never
 *   runs.  Under sduino, main() lives in the core's main.c, which does not
 *   include this header (the include is commented out there, and the SPL
 *   copy is #if'd out for SDCC anyway).  So a TIM2 handler written in the
 *   .ino sketch was dead code by construction.
 *
 *   Here main() and the handlers are in one TU, and stm8s.h pulls this
 *   header in on the SDCC path, so the declaration is unavoidably visible.
 *   Verify it landed: `make map` and look for the vector at 0x8000+.
 *
 * Vector numbers are ST's (SPL stm8s_it.h): TIM2 CAP/COM = 14.
 */
#ifndef STM8S_IT_H
#define STM8S_IT_H

#include "config.h"

#if ENABLE_IR
INTERRUPT_HANDLER(TIM2_CAP_COM_IRQHandler, 14);
#endif

#endif /* STM8S_IT_H */
