---
title: User Repair Guide
type: hcde-artifact
phase: 3
week: 8
date: 2026-05-20
status: draft — repair times pending golden-sample measurement
tags: [repair, hcde, user-guide, troubleshooting, portfolio]
---

# User Repair Guide — Infinity Mirror v1.0

**From Layered Logic**

---

I design products to be opened, repaired, and kept. This guide is the practical side of that promise: if something on your mirror stops working, this is how you fix it.

Most problems here are small, and most have a fix you can do yourself in under half an hour with a screwdriver. A few need a soldering iron. A couple need me — and where that's true, I say so plainly and tell you what to do instead. Nothing on this mirror is a dead end.

If you ever get stuck, or you'd rather not do a repair yourself, email me. The address that sold you the mirror is the same one that helps you fix it.

---

## 1. Before You Start

**Always unplug the mirror from the wall before opening it.** Then give yourself a clean, soft surface to work on so you don't scratch the mirror face.

### Tools

| Tool | Needed for |
|---|---|
| Phillips-head screwdriver | Opening the mirror — every repair starts here |
| A paperclip or SIM-eject pin | The recessed button (pairing / factory reset) |
| Suction-cup handle | Lifting mirror panels without touching the faces |
| Soldering iron + solder | A few repairs only — strip, button, and barrel-jack work |
| Multimeter | Optional — helps diagnose power problems |

If a repair needs a tool you don't have, that's a fine moment to email me instead. You won't be turned away.

### How this guide is organized

- **Section 3** is a symptom finder — start there if you're not sure what's wrong.
- **Section 4** has the step-by-step repair for each part.
- **Section 5** explains the recessed button, which recovers most software problems with no tools at all.
- **Section 6** lists every replacement part and where to buy it.

---

## 2. Opening Your Mirror

Every internal repair starts by removing the back panel. The full step-by-step is in the [Assembly & Teardown Guide §7](assembly-guide.md#7-teardown--access-procedure). In short:

1. Unplug the mirror. Lay it face-down on a towel.
2. Remove the Phillips screws around the back panel. Keep them — they go back in.
3. Lift off the back panel and the back reflector.
4. The components are now reachable: front and back mirror panels, the LED strip on its spacer, and the controller in its small printed housing.

The mirror opens with a screwdriver and nothing else — no glue, no special bits. The wood frame itself is one solid glued piece and never needs to come apart; everything you service is reached through the back.

**Two panels, and which is which:** the **front panel** is the one you look at — it's the see-through half-mirror that lets the light out while still reflecting. The **back panel** behind the LEDs is the full mirror. Light bounces between them, and that's the infinity tunnel. When a panel needs replacing, identifying it by position (front vs. back) is the reliable way.

---

## 3. Troubleshooting by Symptom

Find your symptom, then go to the linked repair section.

### The mirror is completely dark — no lights at all

1. **Check the power first.** Is the power supply plugged into both the wall and the mirror? Unplug and firmly replug the barrel connector at the mirror. The power brick is normally slightly warm in use — if it's stone cold, suspect the power supply → **§4.2**.
2. **Wiggle test.** If the mirror flickers or briefly lights when you move the power cable, the barrel-jack connector is loose → **§4.2**.
3. **Power is good but still dark?** The issue is the LED strip connection or the controller → **§4.1**, then **§4.3**.

### One LED is dark, stuck, or flickering — or everything past a point is dark

The LED strip is a daisy chain, so one failed LED can black out everything after it → **§4.1**.

### The lights flicker, dim on their own, or the mirror reboots itself

Usually power. Reseat the barrel connector, try another wall outlet, remove any extension cords. If it persists → **§4.2**. If it only flickers when you touch the cable → **§4.2** (barrel jack).

### The mirror lights up but behaves strangely — random flashing, frozen color, reboot loop

A software problem. Try a **factory reset** (Section 5) — it fixes most of these with no tools → **§4.3**.

### The app can't find the mirror / it shows offline

The mirror still works from the button — only the network link is down. Almost always a changed Wi-Fi password or router → **§4.3** (Wi-Fi).

### The mirror forgot my settings after a power cut

Harmless. Just set your color, pattern, and brightness again — they'll save → **§4.3** (NVS).

### A button stopped working

Try a power cycle first. If the button is still dead → **§4.8**.

### A mirror panel is cracked, scratched, or hazy

→ **§4.4**.

### The wood frame is chipped, cracked, or warped

→ **§4.5**.

### Something inside is cracked — a printed part, the LED holder

→ **§4.6**.

---

## 4. Repair Procedures

Repair-time estimates below are working figures and will be refined once the first production "golden sample" units are measured.

### 4.1 LED Strip — Dead Pixel, Total Failure, or Fading

All three LED problems share one fix: the strip is a single continuous part, so you replace the whole strip rather than one light.

- **Dead or stuck pixel(s):** one LED is dark, wrong-colored, or flickering. Because the strip is a chain, a failed LED also blacks out everything downstream of it.
- **Total failure:** the whole strip is dark even though the controller has power. *Check this first* — it's usually just a loose connection, not a dead strip. Reseat the strip's connector at the controller before replacing anything.
- **Color degradation:** after many years the strip dims or shifts color as the LEDs age. Normal — and a long way off (the LEDs are rated past a decade of daily use).

**Replacement steps:**

1. Unplug the mirror and open the back (§2).
2. Disconnect the LED strip from the controller — a 3-pin connector, or three solder joints on some builds.
3. Lift the strip out of its printed spacer channel.
4. Seat the new strip in the channel, matching the data-direction arrow.
5. Reconnect to the controller (5V, GND, Data) and reassemble.

**Difficulty:** Easy if the strip is connectorized; moderate if it's soldered. **Time:** 15–25 minutes.
**Part:** WS2812B strip, 60 LEDs/m — see §6.

If a brand-new strip *also* shows dead pixels, the problem is likely the controller's data pin, not the strip → §4.3.

### 4.2 Power — Power Supply and Barrel Jack

**Power supply, no output (mirror totally dark):** the simplest repair on the whole product. The power brick is external — just unplug the dead one and plug in a replacement. No tools, no opening the mirror. Match a 5V supply with a 5.5×2.1mm barrel plug, rated 2A or more.

**Power supply, unstable (flicker, dimming, spontaneous reboots):** first reseat the barrel connector, try a different outlet, and remove any extension cords. If it still misbehaves, swap the power brick as above.

**Barrel jack, intermittent (flicker only when the cable is moved):** the give-away is that the trouble follows cable movement.
1. First, rule out the power supply — try a different 5V brick with a matching plug. If the trouble goes away, replace the power supply.
2. If the supply is fine, the socket on the mirror is worn. Open the back, inspect the socket's solder joints, and reflow or replace the socket (a standard 5.5×2.1mm barrel-jack socket). This step needs a soldering iron.

**Difficulty:** Trivial (power-brick swap) to moderate (socket replacement). **Time:** under a minute to ~20 minutes.

### 4.3 Controller — Hardware, Firmware, Wi-Fi, and Settings

The controller is a small board in a printed housing. It has four distinct problems, and three of the four you can handle yourself.

**Firmware acting up (random flashing, frozen color, reboot loop) — try this first, no tools:**
Do a **factory reset** (Section 5): hold the recessed button for 10 seconds. This clears the controller's stored data and returns it to factory defaults. It resolves the large majority of "the mirror is behaving strangely" problems. If the mirror works normally afterward, re-pair it through the app.

**Lost settings after a power cut (mirror came back on the default indigo color):**
Nothing is broken. Set your color, pattern, and brightness again with the primary button — single press for color, double for pattern, triple for brightness. They save automatically. If Wi-Fi was also forgotten, re-pair (Section 5). If settings vanish *repeatedly*, tell me — it's worth investigating and may be fixable with a software update.

**Wi-Fi dropped / app says offline:**
The mirror still works from the button — only the network link is down. The usual cause is a changed Wi-Fi password or a new router.
1. Hold the recessed button 3 seconds to enter pairing mode (slow indigo pulse).
2. Open the Layered Logic app and follow the setup flow to enter your Wi-Fi details.
3. If the network hasn't changed: restart the mirror and the router, and confirm your router still has a **2.4GHz** network — the mirror can't use 5GHz-only Wi-Fi.

**Hardware failure (power confirmed, but the board is unresponsive):**
This is the one controller problem that needs me. The controller runs a closed firmware, so a replacement board has to be flashed before it works. **Contact me** — I'll send a pre-flashed replacement board (cost of the board plus shipping), or you can send your board in and I'll reflash it for the cost of shipping. The physical swap itself is straightforward: disconnect the LED strip and power wiring, lift the old board out of its housing, seat the new one, reconnect. The board's revision letter is printed on it, and the matching schematic is public on GitHub.

**Difficulty:** Easy (factory reset, re-provision) to moderate (board swap). **Time:** under a minute to ~30 minutes.

### 4.4 Mirror Panels — Cracks, Scratches, and Haze

The acrylic panels sit in milled channels in the frame — not glued. They slide out and back in.

- **Crack or deep scratch:** from an impact or drop. Replace the affected panel.
- **Coating degradation (haze, cloudiness, dark spots):** from years of UV or from cleaning with the wrong product. Same fix — panel replacement. *Prevention:* keep the mirror out of direct sun, and clean the face only with a soft microfiber cloth and water. Never use glass cleaner, ammonia, alcohol, or paper towels — they strip the coating.

**Replacement steps:**

1. Unplug and open the back (§2).
2. Lift out the back reflector and the LED-strip-and-spacer assembly to reach the panels.
3. Slide the damaged panel out of its frame channel.
4. Slide the replacement in. The reflective side faces *inward*, toward the LED cavity.
5. Reassemble: LED assembly, back reflector, back panel.

Replacement panels need to be cut to 150×150mm. If you have a laser cutter I can send the cut file; otherwise email me and I'll send a pre-cut panel.

**Difficulty:** Moderate — handle the panels carefully. **Time:** 20–30 minutes.

### 4.5 Wood Frame — Physical Damage

**Cosmetic damage (nicks, dings, surface scratches):** sand the spot lightly with 220-grit, re-apply matching stain or finish, done. No disassembly.

**Structural damage (a crack, a warp, a loosened panel channel):** this is the most involved repair in the guide, because the panel channel has to be re-cut. Fully disassemble the mirror, replace the damaged frame piece with matching 2×4 lumber, and route the panel channel to match. If you don't have woodworking tools, email me — I can send a pre-cut replacement frame set.

**Difficulty:** Easy (cosmetic) to advanced (structural). **Time:** 10 minutes to a couple of hours.

### 4.6 Printed Parts — Breakage

The mirror has two printed PETG parts: the **LED spacer** that holds the strip, and the **controller housing**. If one cracks:

1. Unplug and open the back (§2).
2. Lift out the broken part (the spacer lifts from the frame; the housing is friction-fit).
3. Drop in the replacement and reassemble.

**Getting the part:** if you have a 3D printer, email me your order number and which part broke, and I'll send the STL file — any standard PETG filament prints it, about 44g for the full set. No 3D printer? I'll send a pre-printed part, or a local library/makerspace print service can do it. **Difficulty:** Easy. **Time:** 5–10 minutes to swap.

### 4.7 Wiring — Solder Joint Failure

The tell-tale sign of a bad solder joint is **intermittent behavior**: the mirror works, then cuts out when it's touched, moved, or bumped; or pressing on the back panel temporarily restores it.

1. Unplug and open the back (§2).
2. Under good light, inspect the solder joints — there are only two soldered areas: the LED strip connection and the controller board connections, plus the barrel-jack socket. Look for dull, grainy, or cracked solder, or a wire loose in its pad.
3. Reflow the suspect joint: heat it with a soldering iron (~350°C), add a little fresh solder, let it cool undisturbed. A good joint looks shiny and smooth.
4. Test before reassembling.

**Difficulty:** Moderate — basic soldering. If you've never soldered, this is learnable, but emailing me is a perfectly good choice. **Time:** 10–20 minutes.

### 4.8 Buttons — Primary and Recessed

**Primary button (the main one) unresponsive:**
1. First, power-cycle the mirror — unplug, wait 5 seconds, replug. This restarts the button software and fixes a glitch.
2. Still dead? Open the back and check the button: press it by hand for a tactile click, and inspect its solder joints.
3. If the switch itself is dead, desolder it and solder in a replacement — a generic 6×6mm tactile momentary switch. On Wi-Fi models, the app does everything the button does, so a dead button is an inconvenience, not a breakdown.

**Recessed button (the pinhole) unresponsive:**
1. Shine a light into the pinhole — can you see the button? Try a thinner pin, and press firmly; the recessed mount needs a bit more force.
2. If you recently reassembled the mirror, the printed housing may have shifted out of line with the pinhole. Open the back and nudge it back into alignment.
3. If the switch is genuinely dead, replace it — same 6×6mm tactile switch as the primary button.

**Difficulty:** Easy (power cycle, alignment) to moderate (switch replacement). **Time:** 1 minute to ~20 minutes.

---

## 5. The Recessed Button — Your Recovery Tool

The small pinhole button is the recovery lever for almost every software problem. It needs no tools but a paperclip, and you never have to open the mirror to use it.

| Hold the recessed button for… | What happens | Use it when |
|---|---|---|
| **3 seconds** | Slow indigo pulse — the mirror enters **pairing mode** | Connecting to Wi-Fi, or moving to a new network / new password |
| **10 seconds** | Red flash, then a green hold — **factory reset** | The mirror is misbehaving, stuck, or you want to wipe it back to out-of-box state |

The light feedback is also a test: if you see the indigo pulse at 3 seconds, the recessed button is working. A factory reset returns the mirror to its defaults — Indigo Signal color, no saved Wi-Fi — and clears most software trouble. It's safe to use any time.

---

## 6. Replacement Parts & Where to Buy Them

Almost every part is a commodity you can buy from many suppliers — your repair doesn't depend on Layered Logic staying in business. The few exceptions are noted.

| Part | Spec | Where | Approx. cost |
|---|---|---|---|
| LED strip | WS2812B, 60 LEDs/m (32 LEDs per mirror) | Amazon, AliExpress, Adafruit | $1.50–$5.00 |
| Power supply | 5V DC, barrel plug 5.5×2.1mm, 2A+ | Amazon, AliExpress, Adafruit | $3–$8 |
| Barrel-jack socket | 5.5×2.1mm DC socket | Amazon, DigiKey, Mouser, AliExpress | under $1 |
| Front panel (half-mirror acrylic) | 150×150×3.1mm | TT Plastic Land, or pre-cut from me | ~$5 |
| Back panel (full-mirror acrylic) | 150×150×3.1mm | Makerstock, or pre-cut from me | ~$2.50 |
| Tactile switch (either button) | 6×6mm through-hole momentary | Amazon, Adafruit, DigiKey, Mouser | pennies |
| Hookup wire | ~22 AWG silicone | any electronics supplier | minimal |
| Frame lumber | standard 2×4 | any hardware store | $3–$5/board |
| Printed parts (spacer, housing) | PETG, ~44g total | print from STL (I'll send it), or pre-printed from me | under $1 in filament |
| Controller board | per published schematic — **pre-flashed, from Layered Logic** | contact me | board cost + shipping |

I commit to keeping replacement controller boards available for **at least 7 years** from your mirror's ship date. The commodity parts above will outlast that on their own.

---

## 7. When to Contact Me

Email me — with your order number if you have it — for any of these:

- You'd rather not do a repair yourself. That's always a valid choice.
- A repair needs soldering and you're not comfortable with it.
- You need a **pre-flashed controller board**, or want to use the **reflash service** (send your board in, I reflash it, you pay only shipping).
- You need a pre-cut acrylic panel, a pre-printed part, or an STL file.
- A repair didn't fix the problem, or you can't tell what's wrong. Send photos — most things can be diagnosed by email.

I'd always rather help you fix the mirror than see you replace it. Before anything else: just ask.

---

## Open Items

- **Repair-time estimates** in Section 4 are working figures, to be confirmed against the first production golden-sample builds (Weeks 9–11).
- ~~**Panel labeling:** the failure-mode reference [acrylic-crack-scratch.md](../Failure_Modes/acrylic-crack-scratch.md) inverted the front/back panels.~~ Reconciled 2026-05-20: the failure-mode doc's "Which panel?" table now matches the assembly guide and this guide — 2-way mirror = front (viewer-facing half-mirror), 1-way mirror = back (full mirror).

---

## Related

- [Right-to-Repair Philosophy](right-to-repair-philosophy.md) — the values behind this guide
- [Assembly & Teardown Guide](assembly-guide.md) — §7 is the teardown procedure every repair here builds on
- [Failure Mode Inventory](../Failure_Modes/README.md) — the full technical analysis of all 17 failure modes
- [Repair Index](repair-index.md) — the entry point to the whole repair-doc set
- [Terms of Service & Warranty Policy](terms-of-service-and-warranty.md) — warranty coverage and the RMA process
- [Sprint Plan](../sprint_plan.md) — Week 8 deliverable [LL-042](../tasks.md#LL-042)
