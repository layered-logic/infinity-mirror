/*
 * board.h -- pin map for the Infinity Mirror STM8 controller, board rev 2
 *            (D:\PCBs\Infinity_Mirror_STM8, 25.000 x 47.000 mm, 2-layer)
 *
 * EVERY line below is transcribed from that project's exported netlist
 * (checks.net, commit 5ce9f82).  Do not "remember" a pin -- re-derive it:
 *
 *     /LED_SIG   R1.1,  U1.16 [PC6]      <- LED data, through R1 = 330R
 *     /Mode      C1.1,  R2.2, SW1.1, U1.3  [PD6]
 *     /NRST      J4.1,  SW2.1, U1.4     <- hardware reset, no firmware
 *     /SWIM      J3.1,  U1.18 [PD1]     <- leave alone, this is the flash port
 *     /CC1_SNS   R7.2,  U1.19 [PD2] = AIN3
 *     /CC2_SNS   R8.2,  U1.20 [PD3] = AIN4
 *     /UART_TX   TP1.1, U1.2  [PD5] = UART1_TX
 *     /DBG_AUX   TP3.1, U1.1  [PD4]     <- bare pad, floats
 *     /IR_OUT    J8.1,  U1.10 [PA3] = TIM2_CH3   (J8 is DNP)
 *     /I2C_SCL   J6.4,  U1.12 [PB4]     (J6 is DNP, and its pin 2 is +5V)
 *     /I2C_SDA   J6.3,  U1.11 [PB5]
 *     unconnected: PA1(5) PA2(6) PC3(13) PC4(14) PC5(15) PC7(17)
 *
 * The old 150 mm firmware drove PC7.  This board has never routed PC7 --
 * that is the bug the rev-2 rework found, and it is why the strip stayed
 * dark.  If you change one define in this file, change it against the
 * netlist, not against a comment.
 *
 * There is NO regulator on this board: VDD is raw VBUS.  Two consequences
 * that reach into the code below -- the ADC is ratiometric (CC thresholds
 * must be fractions of full scale, never volts), and the LED strip and the
 * MCU share one current budget.
 */
#ifndef BOARD_H
#define BOARD_H

/* ---- WS2812 data ------------------------------------------------------ */
#define LED_PORT        GPIOC
#define LED_PIN         6                       /* PC6, TSSOP20 pin 16     */
#define LED_MASK        ((uint8_t)(1u << LED_PIN))

/* ---- Mode button (SW1), active low ------------------------------------ *
 * R2 10k pull-up to +5V and C1 100n on the pin, so no internal pull-up is
 * strictly needed -- we enable it anyway so an unstuffed SW1 still reads
 * high.  This is also why UART1_RX is unusable on this design: RX is
 * physically PD6 and the button owns it.                                  */
#define BTN_PORT        GPIOD
#define BTN_PIN         6                       /* PD6, pin 3              */
#define BTN_MASK        ((uint8_t)(1u << BTN_PIN))

/* ---- USB-C CC sense ---------------------------------------------------- *
 * /CC1 -> R7 10k -> PD2 (AIN3),  /CC2 -> R8 10k -> PD3 (AIN4).
 * R5/R6 = 5.1k Rd pulldowns sit on the connector side of R7/R8.           */
#define CC1_ADC_CH      3
#define CC2_ADC_CH      4
#define CC_PORT         GPIOD
#define CC_MASK         ((uint8_t)((1u << 2) | (1u << 3)))

/* ---- UART1 TX debug pad TP1 (TP2 = GND, TP3 = PD4 spare) --------------- */
#define UART_TX_PORT    GPIOD
#define UART_TX_PIN     5                       /* PD5, pin 2              */

/* ---- IR receiver J8 (DNP) OUT -> PA3 = TIM2_CH3 ------------------------ *
 * Option byte AFR1 remaps TIM2_CH3 from PA3 to PD2.  The datasheet's
 * bracket notation is an exclusive choice, not a duplicate, and PD2 is now
 * AIN3 -- so AFR1 MUST be left unprogrammed.  stm8flash never touches the
 * option bytes unless you ask it to (-s opt); don't ask it to.            */
#define IR_PORT         GPIOA
#define IR_PIN          3                       /* PA3, pin 10             */
#define IR_MASK         ((uint8_t)(1u << IR_PIN))

/* ---- Pins that float on every assembled board -------------------------- *
 * J6 and J8 are DNP and TP3 is a bare pad, so PB4/PB5/PA3/PD4 and the six
 * genuinely unconnected pins are floating CMOS inputs unless we tie them
 * off.  Floating inputs burn current and pick up noise.
 *
 * Deliberately NOT in these masks:
 *   PD1  -- SWIM.  Reconfiguring it kills the programming interface.
 *   PD2, PD3 -- CC sense.  An internal pull-up corrupts the CC divider and
 *               misreads even a good 3 A source.  These stay FLOATING.
 *   PC6  -- LED data (output).      PD5 -- UART TX (driven by UART1).
 *   PD6  -- button (own init).      PA3 -- IR (own init, see ir_init).    */
#define IDLE_PU_PORTA   ((uint8_t)((1u << 1) | (1u << 2)))            /* PA1 PA2       */
#define IDLE_PU_PORTB   ((uint8_t)((1u << 4) | (1u << 5)))            /* PB4 PB5 (I2C) */
#define IDLE_PU_PORTC   ((uint8_t)((1u << 3) | (1u << 4) | (1u << 5) | (1u << 7)))
#define IDLE_PU_PORTD   ((uint8_t)(1u << 4))                          /* PD4 = TP3     */

#endif /* BOARD_H */
