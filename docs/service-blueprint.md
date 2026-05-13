---
title: Service Blueprint
type: hcde-artifact
phase: 3
week: 7
date: 2026-05-13
status: draft
tags: [hcde, service-blueprint, customer-journey, custom-order, operations]
---

# Service Blueprint — Layered Logic Infinity Mirror

**Prepared by:** William White, Founder & Lead Designer
**Date:** May 13, 2026
**Scope:** End-to-end customer journey from first impression to end-of-life, including the bespoke custom-order branch ([LL-054](../tasks.md#LL-054)).

---

## What this document is

A service blueprint maps the entire **system of interactions** around a product — every customer-visible touchpoint, every behind-the-scenes action that supports it, every supplier and tool the system depends on, and the moments where the experience can succeed or fail. It is the HCDE counterpart to a technical architecture diagram: where the architecture documents what the *product* does, the blueprint documents what the *service* does, with the product as one component inside it.

For Layered Logic, the blueprint is also the proof of the venture thesis: the infinity mirror isn't being designed as a gadget but as a *system to keep* — built, sold, supported, repaired, and recycled with intent at every stage. Documents that already exist in this vault each cover one slice of that system ([stakeholder map](stakeholder-map.md), [right-to-repair philosophy](right-to-repair-philosophy.md), [Assembly_docs](../Assembly_docs/), [supply-chain map](supply-chain-map.md), [repair-index](repair-index.md), [sustainability audit](sustainability-audit.md), [button-interface](button-interface.md), [brand-positioning](brand-positioning.md)). This blueprint stitches them into a single journey view.

**What's deliberately not duplicated here:** the production-line detail (lives in [Assembly_docs/](../Assembly_docs/)), the failure-mode catalog (lives in [Failure_Modes/](../Failure_Modes/)), the repair scorecard (lives in [repairability-scorecard.md](repairability-scorecard.md)). The blueprint links into those docs at each relevant stage rather than restating them.

---

## Reading the blueprint

The journey runs across **ten stages**. At each stage, four lanes describe the system:

| Lane | What it captures |
|---|---|
| **Customer journey** | What the buyer is doing, where they are, who they're with, what they feel |
| **Frontstage** | What Layered Logic puts in front of them — listings, packaging, app screens, emails |
| **Backstage** | What Bill (and any future team) does that the customer doesn't see |
| **Support systems / partners** | The tools, suppliers, and platforms the system depends on |

Each stage ends with a **moments of truth** callout — the points where success or failure disproportionately shapes brand perception — and a **fail points** list naming what can break and what the existing system does about it.

The custom-order branch (Stages 2b–3b) is the venture's primary value differentiator and gets its own deeper treatment in **§Custom Order Deep Dive** below the main blueprint.

---

## Stage 1 — Discover

The buyer encounters the product for the first time. They didn't come looking for Layered Logic; they came looking for *atmosphere*, a gift, or a vibe. The discovery channel sets a frame the rest of the journey either rewards or breaks.

**Customer journey.** Scrolling Etsy / Instagram / Pinterest for ambient decor. Walking through a coffee shop with a mirror on the wall. A friend says "you should see this thing." Watching a YouTuber unbox something in the indie-hardware lane. They are looking, not buying — the mental cost is curiosity, not commitment.

**Frontstage.**

| Touchpoint | What it carries |
|---|---|
| Etsy listing | First-frame photo, price, "made-to-order" framing, 1-line tagline |
| Shopify storefront (`layeredlogic.cc`) | Full brand surface — typography, palette, story |
| Coffee-shop installation | The mirror itself, ambient, no UI visible, no sales pressure |
| Creator review video | Third-party framing — most credible discovery channel |
| Instagram / Pinterest post | Aesthetic context, in-room placement |

**Backstage.** Photography ([LL-058](../tasks.md#LL-058)) and listing copy maintenance; relationship cultivation with creators and showcase partners (see [stakeholder map §2.5–2.6](stakeholder-map.md)); placement and rotation of in-the-wild units in partner coffee shops.

**Support systems / partners.** Etsy (discovery + trust intermediary), Shopify (brand surface), Instagram/Pinterest (visual top-of-funnel), creators (asymmetric leverage — one good review > months of paid ads), coffee shops (physical discovery, see [LL-049](../tasks.md#LL-049) interviews). [Supply-chain map](supply-chain-map.md) lists the platform fee structure that prices into the listing.

**Moments of truth.**
- **First-frame photo** on Etsy / IG — three-second decision whether the buyer clicks.
- **Coffee-shop encounter** — the mirror has to be visually compelling without any interaction, in a space that already has its own ambient lighting.

**Fail points.**
- Listing photo doesn't convey scale → buyer assumes a small desk piece → mismatch with $200+ price → bounce.
- Coffee-shop unit fails (flicker, dead pixel) → damages both shop and brand reputation. [Failure_Modes/led-dead-pixel.md](../Failure_Modes/led-dead-pixel.md) and [Failure_Modes/controller-firmware-corruption.md](../Failure_Modes/controller-firmware-corruption.md) are the relevant failure paths; showcase-partner SLA terms ([LL-049](../tasks.md#LL-049) open question) determine recovery speed.
- Creator review is negative or wrong-audience → permanent public artifact. No retraction path.

---

## Stage 2 — Consider (standard branch)

The buyer is on a Layered Logic listing or storefront page and is deciding whether to buy what they see, configure something different, or close the tab. This is where the standard-vs-custom fork lives.

**Customer journey.** Reading the description, checking the photo set, scrolling reviews if any, reading the FAQ, doing a mental "where would this go in my house" sim. Comparing against the Etsy alternatives in the $50–120 hobbyist band and asking whether the price gap is justified. May click into the "Custom" path if a standard SKU isn't quite right.

**Frontstage.**

| Touchpoint | What it carries |
|---|---|
| Product page | Photos, dimensions, color palette options, lead-time disclosure |
| "Why this costs what it costs" copy | Pricing transparency — counter to Etsy commodity framing |
| Repair / sustainability callout | First exposure to the [right-to-repair](right-to-repair-philosophy.md) story |
| Customization preview | The "if this doesn't fit, here's how custom works" link |
| FAQ / shipping / return policy | Trust signals — 30–60 day return per [stakeholder map §5.2](stakeholder-map.md) |

**Backstage.** Listing maintenance, inventory status (made-to-order — "lead time" not "stock"), price + supplier-cost sync via the [pricing calculator](../tools/pricing/), customer-question turnaround (Etsy + Shopify message queues).

**Support systems / partners.** Pricing calculator MVP ([LL-003](../tasks.md#LL-003)), BOM breakdown ([LL-010](../tasks.md#LL-010)) — both feed listing prices. Etsy / Shopify messaging is the pre-sale support channel.

**Moments of truth.**
- The "why this costs what it costs" surface. Without it, the buyer reads $200+ against $50 Etsy and bounces. With it, the buyer reads it against bespoke + repairable + decade-plus + maker-direct.
- The customization link — if it's hidden, the buyer who almost-fit walks away.

**Fail points.**
- Lead-time disclosure missing or buried → buyer thinks it ships in 2 days → angry after week one.
- Pre-sale question goes unanswered for 24h → the buyer has already moved on.
- Return policy too restrictive → seller-protection-tier customer doesn't trust the unknown brand.

---

## Stage 2b — Consult & co-design (custom branch)

The buyer wants something the standard line doesn't carry — a specific size, a logo, a custom cutout pattern, a frame in a wood that matches their space. This is the highest-margin, highest-touch, highest-risk path. See **§Custom Order Deep Dive** below for the full sub-blueprint.

**One-paragraph summary:** Inquiry form → discovery conversation (async via Etsy/email, optionally a 15-min call) → rough concept + price band → design proof (laser cut layer mockup or 3D render) → revision cycles → final approval + deposit → enters Stage 3 manufacture with a custom one-way-mirror plane. Re-work risk is real but bounded: [stakeholder map §5.2](stakeholder-map.md) notes the custom mirror plane is the *only* component that differs, so a cancelled or returned custom unit reworks back to a standard frame + a discounted one-of-a-kind resale or a destroyed plane.

---

## Stage 3 — Purchase

The transaction. Card-present at a coffee-shop event would be a future story; today it's online checkout.

**Customer journey.** Adds to cart (or completes custom-order quote acceptance), enters address, picks a payment method, sees the lead-time confirmation, and clicks pay. Receives an order confirmation. Then... waits, which is the part the made-to-order model has to actively manage.

**Frontstage.**

| Touchpoint | What it carries |
|---|---|
| Checkout flow | Etsy- or Shopify-managed; trust is borrowed from the platform |
| Order confirmation email | First piece of post-purchase comms — sets expectations |
| Lead-time email | "Here's what happens in the next 7–10 days" — the made-to-order story |
| (Custom only) Final approval doc + deposit receipt | Closes the design loop before manufacturing starts |

**Backstage.** Order intake (manual triage of Etsy + Shopify queues), variant selection captured, custom-order metadata attached (mirror plane spec, color palette, frame finish), production scheduling entered into the build queue (see [Assembly_docs](../Assembly_docs/) — traveler card per unit), supplier component check ("do I have enough WS2812B strips and 6×6 panels in stock for this batch?").

**Support systems / partners.** Stripe / PayPal / Shop Pay (payments + chargeback exposure per [stakeholder map §2.3](stakeholder-map.md)); Etsy/Shopify order webhooks; build-queue spreadsheet (today) or workflow system (future).

**Moments of truth.**
- The first email after "thanks for your order." If it's a template that says "we'll be in touch," the buyer reads silence as neglect. If it's a real "here's the next week" note from Bill, the made-to-order framing becomes a feature, not a delay excuse.

**Fail points.**
- Stripe / PayPal chargeback rate creeping toward 1% → account review → revenue stop ([stakeholder map §2.3](stakeholder-map.md)).
- Order metadata lost between Etsy and the build queue → wrong variant manufactured → costly rework.
- Custom-order deposit not collected before manufacturing starts → cancelled-after-build risk.

---

## Stage 4 — Manufacture & QC

The buyer is not present. This is where the production system documented in [Assembly_docs](../Assembly_docs/) earns its keep.

**Customer journey.** Silent, unless the customer reaches out asking for status. Their expectation is calibrated by the Stage-3 lead-time email.

**Frontstage.** None in the traditional sense — but a brief "in production" or "shipping soon" update keeps the silence from feeling like neglect.

**Backstage.** Three parallel lanes per [Assembly_docs/](../Assembly_docs/) — Frame, Mirrors, Electronics — feeding final assembly across 20 stations. Batch-by-operation strategy: a week of 3D prints overnight, a session of frame cuts, an LED-strip pre-test pass at receiving, etc. Physical traveler card per unit through all stations. Post-assembly QC: power-on check, color cycling, button-gesture sweep, brightness levels, soft-off / hard-off, factory-reset path. App-pair test before boxing for app-enabled SKUs.

**Support systems / partners.** Hakko FX-888D soldering station, Dewalt 12" sliding miter, 7" wet tile saw for mirror channels, bench PSU, Fluke 115; JLCPCB for the controller PCB ([stakeholder map §1.4](stakeholder-map.md) — the most critical external partner); Amazon / AliExpress / Temu for commodity components per [supply-chain map](supply-chain-map.md) cascade.

**Moments of truth.** None visible to the customer during manufacture *unless* something goes wrong — at which point the silence breaks and Bill has to send a "your shipment is delayed by 5 days because [honest reason]" email. The made-to-order brand survives that email; a sealed-mass-market brand wouldn't.

**Fail points.**
- Supplier inconsistency (LED color binning, wrong barrel-jack stock) → mitigated by sourcing cascade ([supply-chain map](supply-chain-map.md)) and pre-test at receiving.
- Solder joint failure at QC → [Failure_Modes/wiring-solder-joint-failure.md](../Failure_Modes/wiring-solder-joint-failure.md) is the catch-and-fix-now path; same failure post-ship is the catch-and-RMA path.
- Solder bottleneck at scale → already flagged in [LL-012](../tasks.md#LL-012) as the first outsource candidate.
- Custom mirror plane re-cut needed → laser time is constrained; a re-cut costs ~30 min of station time but the buyer's lead-time email may need updating.

---

## Stage 5 — Fulfill (pack & ship)

The product transitions from Layered Logic's space to the carrier's space. Packaging is doing two jobs simultaneously: physical protection for a fragile acrylic-and-LED stack, and the *first unboxing impression* that sets the tone for everything after.

**Customer journey.** Receives a shipping notification with a tracking number. Watches the dot move across the country / across town. Sees the box on their porch.

**Frontstage.**

| Touchpoint | What it carries |
|---|---|
| Shipping confirmation email | Tracking link, ETA, what to do if damaged |
| Outer shipping box | Generic — protects the brand layer underneath |
| Inner unboxing layer | First branded surface (packaging concept pending — [LL-051](../tasks.md#LL-051)) |
| Documentation card / QR code | Points to setup, app, and the [repair-index](repair-index.md) (placement blocked by [LL-044](../tasks.md#LL-044), waiting on [LL-051](../tasks.md#LL-051)) |

**Backstage.** Pack-and-ship station (workshop layout per [LL-012](../tasks.md#LL-012) — fully isolated from wood-dust zone). Per-unit label printing, insurance threshold check, carrier handoff. Final photo captured for the build-archive (per-unit traveler closes here).

**Support systems / partners.** USPS / UPS / FedEx (the [stakeholder map §2.4](stakeholder-map.md) last-touchpoint partner that can undo weeks of work in a single dropped box). Cardboard stock + die-cut packaging vendor (open action from [supply-chain map](supply-chain-map.md)).

**Moments of truth.**
- The outer box arriving intact. Damage at this stage is mostly carrier-driven but is read by the buyer as a Layered Logic failure regardless.
- The inner unboxing — see Stage 6.

**Fail points.**
- Damage in transit → acrylic is the primary risk surface. Packaging concept ([LL-051](../tasks.md#LL-051)) needs to assume worst-case carrier handling. [Failure_Modes/acrylic-crack-scratch.md](../Failure_Modes/acrylic-crack-scratch.md) is the matching failure path.
- Wrong-address / lost package → carrier insurance covers cost, but the customer experience cost is borne by Layered Logic.
- Gift-recipient address mismatch → [stakeholder map §3.2–3.3](stakeholder-map.md) flags the gift-giver / gift-recipient dual-stakeholder problem here.

---

## Stage 6 — Unbox & set up

The buyer is opening the box. This is the highest-leverage moment in the entire journey: every prior decision (brand, packaging, button interface, default state, provisioning UX) compounds into a two-minute impression. The gift-recipient case — [stakeholder map §3.3](stakeholder-map.md), "the ultimate test of intuitive design" — is the hardest version of this stage because the recipient didn't choose the product and won't read a manual.

**Customer journey.** Opens the shipping box. Lifts out the mirror. Reads (or doesn't read) the documentation card. Finds an outlet. Plugs it in. Watches the welcome sequence ([LL-038](../tasks.md#LL-038) — additive fill, breakpoints derived from `led_count`, cyan @ 100% brightness). Decides whether to install the app or just use the button.

**Frontstage.**

| Touchpoint | What it carries |
|---|---|
| Welcome sequence on first power-on | The "this thing is alive and well" canary |
| Documentation card | One side: 4 button gestures from [button-interface](button-interface.md); other side: app QR + setup link |
| Default factory state | Brand-palette default color, ambient brightness, on-state (cord-pull = on-with-nice-thing-happening) |
| Primary button | Single = on/advance base color, double = pattern, triple = brightness, hold = off ([LL-025](../tasks.md#LL-025)) |
| (App path) Find-mirror flow | mDNS-based discovery ([LL-040](../tasks.md#LL-040)) — no manual IP entry |
| (App path) SoftAP provisioning | LL_SOFTAP_PROVISIONING captive portal ([LL-035-5](../tasks.md#LL-035-5)) |
| (App path) Setup screen | SSID/password form, 15s fallback timer if creds are bad |

**Backstage.** None visible. Bill is not on the call. Every recovery has to be designed into the firmware: bad-password fallback ([LL-035-5-4](../tasks.md#LL-035-5-4)), recessed-button factory reset ([LL-025](../tasks.md#LL-025)), captive-portal redirect from any HTTP request ([LL-035-5-3](../tasks.md#LL-035-5-3)), USB-recovery as the ultimate backstop.

**Support systems / partners.** Apple App Store / Google Play ([stakeholder map §5.4](stakeholder-map.md) — the platform gatekeepers); the device's hosted webapp as fallback when app stores are unavailable or the buyer doesn't want to install an app; mDNS for discovery.

**Moments of truth.**
- The welcome sequence. If the box arrived undamaged and the LEDs light up in the brand-locked indigo, the buyer's "did I make a good decision" question is settled in ten seconds.
- The gift-recipient case. If they can get to *daily use* without an app install, the product is intuitive. If they can't, the gift becomes a chore.

**Fail points.**
- Mirror doesn't power on → PSU dead-on-arrival ([Failure_Modes/psu-no-output.md](../Failure_Modes/psu-no-output.md)) or barrel-jack intermittent ([Failure_Modes/barrel-jack-intermittent.md](../Failure_Modes/barrel-jack-intermittent.md)). RMA-tier — recoverable but expensive.
- Buyer types Wi-Fi password wrong → 15s fallback returns to SoftAP, not stranded. ✅ Designed in.
- Buyer's phone uses a VPN that scopes off the LAN → "Find mirror" fails. Today's error copy mentions VPN as the likely cause ([LL-046](../tasks.md#LL-046) UX polish). Documentation card should reinforce.
- Buyer ignores the app and uses the button only → fine. Documented intent: "Works out of the box with no required smart-home connection" ([LL-027](../tasks.md#LL-027) §5.1).
- Multi-mirror household — buyer can't tell which mirror is which → [LL-039](../tasks.md#LL-039) shipped: user-set name + MAC-suffix id surfaced everywhere.

---

## Stage 7 — Daily use

The mirror is on a wall, doing its job. The buyer is interacting with it directly, indirectly, or not at all — and so is everyone else in the space. This is the longest stage by far (years) and the one with the lowest moment-to-moment visibility. The product needs to disappear into the room and reward the moments when the user *does* want to engage.

**Customer journey.** Cord-pull = on. Hold = off. Single-press to change base color. Double to change pattern. App for finer control or pattern editing. Most of the time: the mirror is just *there*, ambient.

**Frontstage.**

| Touchpoint | What it carries |
|---|---|
| The mirror itself, on the wall | The actual product experience |
| Primary button | Daily-use UI per [button-interface](button-interface.md) |
| LL mobile app — Controls page | Color, pattern, brightness, on/off, find-mirror |
| LL mobile app — Settings page | Multi-network Wi-Fi, mirror rename, factory reset, firmware update |
| Device-hosted webapp | Functional parity with the app; for laptop / guest / no-install cases |

**Backstage.** None per-event. Aggregate: OTA fleet management — staged rollout (1% → 10% → 50% → 100%) for any firmware push, opt-in cohort gets builds first, anti-rollback eFuse on production units (per [LL-027 §5.4](../tasks.md#LL-027) — currently parked as [LL-071](../tasks.md#LL-071) post-quarter). Pattern dictionary maintenance ([docs/pattern-dictionary.md](pattern-dictionary.md)).

**Support systems / partners.** The user's home Wi-Fi (multi-SSID support via [LL-046](../tasks.md#LL-046)); Apple/Google for app updates; future OTA hosting on `ota.layeredlogic.cc` ([LL-071](../tasks.md#LL-071)); the user's own LAN router (the most variable infrastructure dependency).

**Moments of truth.**
- The first time a roommate or guest can turn the mirror off without knowing what it is. The button has to be self-evident, per the "designing for the passive stakeholder" frame in [stakeholder map §4.2](stakeholder-map.md).
- The first time the user wants to change something and reaches for the app. mDNS find-mirror has to work the first time; the failure mode here is "the user types in an IP" and we've already designed that out ([LL-040](../tasks.md#LL-040)).
- The first time the mirror updates itself (OTA). The cyan welcome sequence on next boot is the silent canary that the update landed (`LL-038`).

**Fail points.**
- LED color drift over time → [Failure_Modes/led-color-degradation.md](../Failure_Modes/led-color-degradation.md). Slow-burn — the user habituates. Repair path: LED-strip swap via JST.
- App can't find the mirror after a router change → multi-network SSID list ([LL-046](../tasks.md#LL-046)) covers the common case; new-router-no-known-SSIDs is the remaining hole, recoverable via recessed-button-hold → SoftAP.
- Firmware OTA bricks the mirror → rollback safety net wired ([LL-046](../tasks.md#LL-046) OTA-rollback). Auto-revert if new build fails health check.
- Roommate annoyed by the mirror → off via hold-on-button. ✅ Designed for.

---

## Stage 8 — Trouble & repair

Something goes wrong — or the user wants to change something only the recessed button can do (factory reset, BLE pairing). This stage is where the [right-to-repair philosophy](right-to-repair-philosophy.md) stops being a marketing claim and starts being a service contract.

**Customer journey.** Notices something wrong (a dead pixel, a flickering line, the mirror won't connect). Tries the obvious thing (power cycle). If that doesn't work, the journey forks: read the in-box card, scan the QR ([LL-044](../tasks.md#LL-044), pending packaging), open the app's Help section (future), or just email Layered Logic. The decision-point is *self-recover vs. RMA*, and the [repair-design-decisions matrix](repair-design-decisions.md) explicitly shapes how this fork plays out.

**Frontstage.**

| Touchpoint | What it carries |
|---|---|
| Recessed pinhole button | 3s hold = BLE pairing, 10s hold = factory reset, with LED feedback cues |
| QR / URL on documentation card | Lands on [repair-index](repair-index.md) → diagnostic walkthrough |
| App Settings → Danger → Factory Reset | Confirm dialog, then executes — "do what it was told" ([feedback_respect_explicit_actions](../.claude/projects/C--Users-bowhi-Desktop-Independent-Study/memory/feedback_respect_explicit_actions.md)) |
| Repair video library | Per-RMA filmed repair, growing 24/7 zero-marginal-cost support |
| RMA email / form | Triage by Bill; replacement parts shipped or whole-unit swap |

**Backstage.** Diagnostic intake (what's the failure mode? does it match one of the 17 cataloged in [Failure_Modes/](../Failure_Modes/)?); part lookup against the [BOM](bom-breakdown-basic-6x6.md); ship spare module if user-repairable, ship return label if whole-unit; **film the repair if it's a first-of-its-kind** so it becomes a video asset.

**Support systems / partners.** [Failure_Modes/](../Failure_Modes/) catalog (17 modes — 12 fully user-repairable, 5 partial, 0 non-repairable, per [repairability-scorecard](repairability-scorecard.md)); [repair-design-decisions](repair-design-decisions.md) matrix for "which spec choice covers which failure mode"; commodity supplier cascade ([supply-chain-map](supply-chain-map.md)) for replacement parts; RMA email + return shipping.

**Moments of truth.**
- The first RMA. The customer is already frustrated. The response either confirms the brand thesis ("it actually is repairable") or destroys it ("I have to ship the whole thing back?").
- The first time a user fixes something themselves with a part Layered Logic shipped. This is the highest-leverage brand moment in the whole journey — they're now an evangelist.

**Fail points.**
- User attempts self-repair without RMA → warranty voided ([stakeholder map §5.1](stakeholder-map.md)). Documentation has to make this boundary unambiguous.
- Replacement part not in stock → [supply-chain-map](supply-chain-map.md) commits to commodity-sourced backups for everything; 7-year minimum parts commitment per [repairability-scorecard](repairability-scorecard.md).
- Repair video doesn't exist for an uncommon failure → film it on the first RMA. The video library *is* the support team.
- Failure mode that isn't in the catalog → add it. The catalog itself is a living artifact.

---

## Stage 9 — Returns / dissatisfaction

The buyer wants their money back. Reasons vary: buyer's remorse, didn't fit the space, defect not caught in QC, or just changed their mind.

**Customer journey.** Initiates a return via Etsy / Shopify support (or directly). Packages the mirror back into the original packaging if they kept it; otherwise we ship one. Drops it at a carrier. Waits for refund.

**Frontstage.** Return policy ([stakeholder map §5.2](stakeholder-map.md): 30–60 day no-questions-asked); return label; refund confirmation.

**Backstage.** Inbound inspection on receipt → does the mirror still work? Is it a standard unit (resellable as-is) or custom (rework path)? If custom and the customer's design is non-proprietary, can it be resold as a discounted one-of-a-kind? Refund processed.

**Support systems / partners.** Etsy / Shopify return tooling; carriers for return shipping; payment processors for refund execution (with chargeback exposure if the return is disputed).

**Moments of truth.**
- The "yes" on the return request. A frictionless yes from a small brand outperforms a friction-heavy yes from a big one.

**Fail points.**
- Custom unit returned, design is proprietary (e.g., a company logo) → mirror plane scrap, frame re-fits a standard plane → rework cost per [stakeholder map §5.2](stakeholder-map.md) open question.
- High return rate → indicates a Stage-2 expectation gap (listing copy or photos misleading) or a Stage-4 QC gap; either way, signal not a fact.
- Chargeback dispute the customer wins → loss of both product and revenue. Mitigated by responsive Stage-6 / Stage-8 support that turns "I'm returning this" into "I just needed help."

---

## Stage 10 — End-of-life

Years later, the mirror has reached the end of its useful life — or the user has decided to part with it. The HCDE thesis says this stage matters as much as Stage 1.

**Customer journey.** Decides to move, redecorate, or upgrade. Tries to figure out what to do with it. Throws it in landfill / passes it on / sells it secondhand / pulls it apart for parts.

**Frontstage.** [Right-to-repair philosophy §3–4](right-to-repair-philosophy.md) commits to keep the product serviceable; [sustainability audit](sustainability-audit.md) maps the full cradle-to-grave path; documentation tells the user what's separable (cardboard packaging, PETG snap-fit, JST-modular electronics).

**Backstage.** Open-sourcing commitment ([stakeholder map §5.4 / §5.5](stakeholder-map.md)) — "the founder commits to open-sourcing all design files and firmware before ever stepping away." Until then, OTA support for the life of each unit ([right-to-repair §3](right-to-repair-philosophy.md)).

**Support systems / partners.** Local recycling (cardboard + plastic + e-waste streams); the open-source community (future); the user's own willingness to disassemble.

**Moments of truth.** None visible at the time the decision is made — but the *aggregate* end-of-life outcome (how many mirrors are landfilled vs. recycled vs. repaired) is the HCDE artifact the venture is ultimately judged on.

**Fail points.**
- Acrylic mirror panel — coated and not currently recyclable in standard streams ([sustainability audit](sustainability-audit.md) key hotspot). Open improvement for v2.
- PSU oversizing (25W brick at 11% load) → embodied carbon penalty per [sustainability audit](sustainability-audit.md). v2 right-sizing flagged.
- The user doesn't know the product is designed for disassembly → the in-box documentation has to plant this seed at Stage 6 so it's reachable at Stage 10.

---

## Custom Order Deep Dive

[LL-054](../tasks.md#LL-054) — the bespoke design process is the venture's primary value differentiator. Standard SKUs play in the $200+ "above Etsy commodity" band; custom orders are where Layered Logic earns hourly rates that justify the operations footprint.

### Why custom matters

The competitive landscape ([Market Analysis Report](Market%20Analysis%20Report.md)) shows the $50–120 hobbyist tier is saturated with 1,900+ Etsy listings. The $250–1.2k B2B-branding and $230–4.3k sensory/therapy tiers are dramatically less crowded. Custom is the on-ramp into those tiers: a logo mirror for a corporate lobby, a depth-shape for a sensory room, a co-designed piece for a private buyer. Almost every component is shared with the standard line — only the mirror plane (the laser-cut design layer) and optionally the frame dimensions change. From an operations standpoint, custom is high-margin precisely because it reuses the same supply chain, the same Assembly_docs production line, and the same firmware / app / OTA system as standard.

### The custom order journey

**1. Inquiry intake.** Buyer hits the "Custom" link from a standard product page or lands directly on the custom landing page. Submits a short form: rough size, intended use (home / office / public space), reference imagery if any, budget band, target delivery date. Async — no real-time chat required.

**2. Discovery conversation.** Bill replies within 24h with clarifying questions. For a simple resize, that's the entire conversation. For a logo mirror or a co-designed piece, it's a 1–2 email exchange or optional 15-min video call. The point of this step is to translate *vibe* into *spec*: what are they actually asking for, and is it buildable with the existing tooling and supply chain?

**3. Concept + price band.** Bill produces a rough concept (sketch or simple render of the proposed mirror plane) and a price band (not a final number — "$350–$425 depending on frame finish and complexity"). This is sent before any committed design work. Buyer can walk away here at zero cost to either side.

**4. Design proof.** If the band is acceptable, Bill produces a design proof — a flat 2D render of the mirror plane and a 3D-rendered preview of the unit in a representative space. Optionally, a physical laser-cut paper mockup is sent to the buyer for sizing reference.

**5. Revision cycle.** Typically 1–2 iterations. Capped at 3 to protect timeline; further revisions trigger a small revision fee. Each revision is logged.

**6. Final approval + deposit.** Buyer signs off on the design proof; pays a 50% deposit. Mirror plane spec is locked. Custom unit enters the build queue.

**7. Manufacture.** Same as standard Stage 4 — frame, mirrors, electronics, final assembly — except the laser-cut mirror plane is the custom variant. The traveler card carries the custom-order metadata. Lead time is typically +5–7 days over standard for the design phase, not the build phase.

**8. Pre-ship final review.** Photograph of the completed unit sent to the buyer for sign-off before packing. This is also the asset for the build archive (and, with buyer permission, future portfolio / marketing).

**9. Balance + ship.** Buyer pays the 50% balance. Unit ships per Stage 5.

**10. Post-purchase.** Identical to standard Stages 6–10 — same firmware, same app, same repair path, same end-of-life commitments. Custom doesn't fork the support model.

### Custom-order frontstage / backstage table

| Step | Customer sees | Bill does | System touched |
|---|---|---|---|
| Inquiry | Custom form | Triages inbox | Etsy/Shopify form + email |
| Discovery | Email exchange | Asks clarifying questions | Email + (optional) calendar booking |
| Concept | Sketch + price band | Drafts concept in 1–2h | Photoshop/Illustrator/Figma + sketchbook |
| Proof | 2D render + 3D preview | Produces proof | KiCad-class tooling for the laser path + render |
| Revisions | Updated proof | Logs revisions, holds to ≤3 free | Internal tracker (revision log) |
| Approval | Approval doc + deposit request | Locks spec; collects 50% | Stripe/PayPal invoice |
| Manufacture | "Your custom unit is in production" | Standard Assembly_docs flow | Traveler card + production line |
| Pre-ship review | Final-unit photo | Captures photo; awaits sign-off | Build archive |
| Balance + ship | Final invoice; tracking | Collects balance; ships | Stripe + carrier |

### Custom-order moments of truth

- **The concept reply.** First time the buyer sees the design rendered, not described. If it lands, the rest of the flow is administrative. If it misses, the revision cycle has to recover quickly.
- **The price band.** Anchoring is real — too high and the buyer walks before seeing what they could get; too low and the revisions eat the margin.
- **The deposit ask.** The transition from "we're chatting" to "you're committed." Has to feel natural — a deposit-on-a-good-design-proof, not a deposit-on-a-vague-idea.

### Custom-order fail points

- **Buyer wants something not buildable** (size exceeds 300×600mm laser bed, color exceeds palette firmware support, finish unavailable). Caught at discovery; redirected to standard or politely declined.
- **Buyer's reference imagery is copyrighted / trademarked.** Logo customizations require buyer attestation of rights. Liability rests with the buyer; documented in ToS ([LL-056](../tasks.md#LL-056), pending).
- **Revisions exceed cap.** Revision fee triggers; if buyer rejects, the order doesn't progress and no further design work happens.
- **Buyer disappears mid-cycle.** Concept and proof time are sunk; mitigated by collecting deposit only at approval, so the unrecoverable cost is design hours, not materials.
- **Cancelled after manufacturing starts.** Deposit covers materials; the custom mirror plane is scrap; frame and electronics are reusable. Worst-case cost ≈ one laser-cut plane plus packing/restock labor.

### Open research questions for the custom flow

These feed the user-interview agenda ([user-interview-outline §4](user-interview-outline.md) "Customization appetite"):

- What is the price elasticity in the custom band? Where does "yes, I'll pay $400" become "no, that's too much"?
- How much design back-and-forth do buyers actually want? Is one revision plenty, or does three feel collaborative?
- Is the 50% deposit at approval the right break point, or do buyers want a smaller initial commit (e.g., 25% at concept, 25% at approval)?
- Do corporate / B2B buyers expect a different cadence — proposal docs, NDAs, invoicing terms?
- What's the right canonical lead-time disclosure for custom? +5–7 days is the operations estimate; what does the buyer's expectation actually look like before we tell them?

---

## Cross-cutting system properties

Some properties don't live in a single stage — they thread through the whole journey. Listing them once here so the per-stage sections don't repeat them.

| Property | What it does | Where it shows up |
|---|---|---|
| **The product works without an app** | Out-of-box default state + button-only UI mean Stages 6–7 don't depend on app install | Stages 6, 7 |
| **mDNS-based discovery** | No manual IP entry; device finds itself on the LAN | Stages 6, 7 |
| **OTA + rollback safety net** | Firmware updates can land + auto-revert if they panic | Stages 7, 8 |
| **Toolless disassembly** | One Phillips driver is the entire required toolset | Stages 8, 10 |
| **JST-modular electronics** | Replacement is plug-not-solder for end users | Stages 8, 10 |
| **Commodity-sourceable parts** | 6 of 8 component classes 100% replaceable from non-LL sources | Stages 8, 10 |
| **Documented from day one** | Every spec doc, failure mode, and decision is a portfolio artifact and a future-employee onboarding asset | All stages |
| **Open-source on founder exit** | Long-tail support survives the founder | Stages 8, 10 |

---

## Service blueprint summary table

A compressed view of the ten stages and their dominant failure modes for the portfolio-skim case. Read the per-stage sections above for detail.

| # | Stage | Customer state | Primary frontstage | Primary backstage | Key moment of truth | Top fail point |
|---|---|---|---|---|---|---|
| 1 | Discover | Curious, scrolling | Etsy listing photo | Photography + creator outreach | First-frame photo | Showcase unit fails in public |
| 2 | Consider | Comparing, deciding | Product page + custom link | Listing + pricing maintenance | "Why this costs what it costs" copy | Lead-time hidden |
| 2b | Consult (custom) | Co-designing | Email + concept render | Design proof + revision log | Concept reply | Buyer disappears mid-cycle |
| 3 | Purchase | Committing | Checkout + confirmation email | Order intake + build-queue entry | First post-purchase email | Order metadata lost |
| 4 | Manufacture | Silent / waiting | (Optional) status update | Assembly_docs production line | Honest delay email if needed | Solder-joint failure at QC |
| 5 | Fulfill | Watching tracking | Shipping email + packaging | Pack-and-ship station | Box arrives intact | Acrylic crack in transit |
| 6 | Unbox & set up | First impression | Welcome sequence + doc card | (None visible — designed into firmware) | Welcome sequence lights up | PSU dead-on-arrival |
| 7 | Daily use | Background-ambient | Button + app + webapp | (None per-event) | First time a guest turns it off without instructions | OTA bricks the device |
| 8 | Trouble & repair | Frustrated / curious | Recessed button + repair-index + RMA | Diagnostic + part-ship + film-the-repair | First user-completed self-repair | Replacement part not available |
| 9 | Returns | Walking back | Return label + refund | Inbound inspection + rework | Frictionless "yes" | Custom unit + proprietary design |
| 10 | End-of-life | Disposing | Right-to-repair commitment + open-source promise | OTA support for unit lifetime | The aggregate outcome | Acrylic panel not recyclable |

---

## Open actions surfaced by the blueprint

Items that the blueprint exposed as gaps or open questions, sorted by stage and linked to existing tasks where applicable.

| Action | Stage | Owner | Existing task |
|---|---|---|---|
| Define showcase-partner SLA (replacement turnaround, liability) | 1 | Bill | [LL-049](../tasks.md#LL-049) |
| Land packaging concept (unblocks doc-card placement) | 5 | Bill | [LL-051](../tasks.md#LL-051) |
| Land in-box documentation card design (button + QR + repair) | 6 | Bill | [LL-044](../tasks.md#LL-044) (blocked on LL-051) |
| Land ToS + warranty + custom-order rights/liability language | 2b, 8, 9 | Bill | [LL-056](../tasks.md#LL-056) |
| Land production OTA infrastructure (signed binaries, anti-rollback, staged rollout) | 7 | Bill | [LL-071](../tasks.md#LL-071) (post-quarter) |
| Validate price elasticity + revision-cycle expectations in custom | 2b | Bill | [LL-048](../tasks.md#LL-048) (interviews) |
| Define repair-video pipeline (which RMAs trigger filming, where videos live) | 8 | Bill | new — surfaced by this blueprint |
| Decide custom-order deposit structure (50% at approval vs. 25%/25%) | 2b | Bill | new — surfaced by this blueprint |
| Define carrier insurance threshold (above $X always insure) | 5 | Bill | new — surfaced by this blueprint |
| Address mismatch between gift-giver and gift-recipient at fulfillment | 5 | Bill | new — surfaced by this blueprint |

The four "new" rows are not added to the task registry by this document — they're surfaced for triage. Promote to `LL-NNN` when they receive scoped work.

---

## Related

- [Index / MOC](../index.md)
- [Sprint Plan — Week 7](../sprint_plan.md) — where this artifact lives in the timeline
- [Stakeholder Map](stakeholder-map.md) — the cast of characters this blueprint puts on a journey
- [Right-to-Repair Philosophy](right-to-repair-philosophy.md) — the service commitments at Stages 8 & 10
- [Repair Index](repair-index.md) — the repair sub-system Stage 8 routes into
- [Repairability Scorecard](repairability-scorecard.md) — quantitative companion to Stage 8
- [Repair Design Decisions](repair-design-decisions.md) — matrix mapping decisions to failure modes
- [Sustainability Audit](sustainability-audit.md) — Stages 4, 5, 10 lifecycle backbone
- [Supply Chain Map](supply-chain-map.md) — Stage 4 and Stage 8 sourcing
- [BOM Breakdown](bom-breakdown-basic-6x6.md) — cost structure under every stage
- [Brand Positioning](brand-positioning.md) — voice + audience the blueprint speaks to
- [Button Interface](button-interface.md) — Stage 6/7/8 physical UI
- [Button Design Rationale](button-design-rationale.md) — HCDE justification feeding Stage 6
- [User Interview Outline](user-interview-outline.md) — feeds the open research questions at Stage 2b
- [Market Analysis Report](Market%20Analysis%20Report.md) — segment context Stage 2 lives in
- [Assembly_docs/](../Assembly_docs/) — Stage 4 production line
- [Failure_Modes/](../Failure_Modes/) — Stage 8 catalog (17 modes)

---

<p align="center"><em>Layered Logic LLC — Spring 2026</em></p>
