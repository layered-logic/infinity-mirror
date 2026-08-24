/*
 * stm8s_conf.h -- deliberately empty.
 *
 * ST's stm8s.h self-defines USE_STDPERIPH_DRIVER and then pulls in this
 * file, which in a normal SPL project lists every peripheral driver header.
 * This firmware uses the SPL for register definitions ONLY -- no SPL .c
 * file is compiled or linked -- so there is nothing to include, and this
 * stub is what keeps vendor/stm8s.h byte-identical to ST's original.
 *
 * (The header's own comment at that #define says to comment it out "if you
 * will not use the peripherals drivers".  Same intent, without editing the
 * vendored copy.)
 *
 * assert_param() lives here in a real SPL project.  Nothing in this build
 * calls it, because nothing here is an SPL function.
 */
#ifndef STM8S_CONF_H
#define STM8S_CONF_H

#define assert_param(expr) ((void)0)

#endif /* STM8S_CONF_H */
