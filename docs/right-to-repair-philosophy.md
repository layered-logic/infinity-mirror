---
title: Right-to-Repair Philosophy
type: hcde-artifact
phase: 2
week: 4
date: 2026-04-20
status: draft
tags: [hcde, repair, philosophy, values]
---

# Right-to-Repair Philosophy

**Prepared by:** William White, Founder — Layered Logic LLC
**Date:** April 20, 2026
**Status:** Working draft — Week 4 deliverable

---

## 0. The Principle

I design products to be opened, repaired, and kept.

That's the whole philosophy in one sentence. The rest of this document says *why*, what the commitment specifically covers, and where its honest limits are. The User Repair Guide (in development) is the practical companion — this doc is the reasoning behind it.

---

## 1. Why

Three reasons, ordered by how much they actually drive my decisions.

### The first is pro-consumer conviction.

When someone pays for something I made, that's the beginning of the relationship — not the end of the transaction. A product that can't be opened is a product that tells its owner "this was never really yours." I'd rather make fewer units that people keep for a decade than more units that pile up in closets waiting for the landfill.

This is the posture I want the brand to have by default: buying is joining, not consuming. Repair is a form of respect — for the buyer's money, for their time, for the materials that went into the thing, and for the idea that good objects should be lived with, not rotated out.

### The second is environmental honesty.

The single biggest lever on a product's lifetime carbon is how long it stays in service. Everything else — recycled packaging, efficient PSUs, right-sized components — is secondary to simply keeping the object in use.

The [Sustainability Audit](sustainability-audit.md) puts the numbers on this: the LED strip has a 14+ year service life at worst-case usage, and the electronics fabrication locks in the largest share of the product's embodied carbon at the moment it's built. That carbon is already spent. The environmentally coherent move after that is to keep the unit working for as long as physically possible — and to make sure every failure mode is a repair event, not an end-of-life event.

A product designed to be repaired is a product designed to be *kept*. That connects back to **Living** as a brand adjective — the mirror isn't a static object that decays on a schedule. It's a system that can be updated, refreshed, and carried forward.

### The third is that the law is arriving.

The EU Right-to-Repair Directive, the Washington and California state bills tracking alongside it, and similar legislation in other major markets are all converging on the same requirement: electronics must be repairable using standard tools and parts for 7–10 years from date of sale. Sealed-unit products are becoming illegal to sell in the markets I want to sell into.

I'm listing this third on purpose. The legal argument reinforces the values argument, but if the law were my only reason I'd be doing the minimum and calling it compliance. I'd rather build a product that *would have been* repairable even if nobody was watching — and then discover that the regulators happen to agree.

---

## 2. What Repair Means Here

Concretely, for the v1.0 infinity mirror:

- **The product opens with standard tools.** Philips-head screwdriver. No proprietary bits, no glued enclosures, no ultrasonic-welded clamshells, no destructive entry.
- **Every failure mode I can imagine has a user-serviceable path.** LED burns out → replace the strip (commodity WS2812B 5M/60, stocked by every electronics supplier). Controller dies → swap the module. PSU fails → replace the 5V barrel-jack brick from any supplier. Enclosure cracks → reprint the PETG part from the STL I ship with every unit.
- **Firmware is updateable without opening the product.** Over-the-air updates mean a software fix never requires hardware work. Bugs I ship in 2026 can be fixed in 2030 without anyone touching a screwdriver.
- **Nothing is bonded that doesn't need to be.** Friction-fit where possible, screwed where necessary, glued nowhere. If a future owner needs to swap a component, the object cooperates.
- **Parts stay findable.** Every component in the BOM is either a commodity (available from dozens of suppliers) or a printable file (available from me). "Available" here doesn't depend on my warehouse alone — the ecosystem is the supply chain.

---

## 3. What I Commit To

As long as I'm the one building these, these are the commitments the buyer can hold me to:

- I will publish the User Repair Guide as a standalone document — not as a buried FAQ entry, not hidden behind an account wall.
- I will supply STL files on request to any owner reporting a specific repair need. They're not published by default — the commitment is to service, not to free distribution — but no one with a cracked enclosure will be turned away from the file to reprint it.
- I will publish schematic references for each shipped PCB revision in a public GitHub folder, updated as revisions ship. Anyone wanting to understand what's inside the box can read along.
- I will version every physical PCB clearly, with the revision letter on the silkscreen and visible when the enclosure is opened. An owner and I should always be able to line up the board in their hand with the schematic they're reading.
- I will maintain OTA firmware update support for the life of each unit, so software-side issues get fixed without the owner ever needing to flash anything themselves.
- I will answer repair questions directly. While I'm the assembler, I'm also the support line — there is no handoff between who built it and who helps fix it.
- I will honor the warranty by repair first, replacement second, refund third — not as a dodge, but because repair is the option most aligned with the values of the product.
- I will maintain replacement-part availability for a minimum of **7 years from ship date**, aligned with the floor emerging in right-to-repair legislation. In practice, commodity parts (LED strips, PSUs, screws, acrylic) will outlive that window on their own. The commitment is meaningful mainly for the PCB and controller module.

---

## 4. The Honest Limits

A philosophy doc is only useful if it states what it *doesn't* promise.

- **The acrylic mirror panels cannot be recycled as pure PMMA.** The vacuum-deposited metallic coating disqualifies them from standard PMMA recycling streams. At ~166g per unit, this is the product's largest end-of-life gap, and I'm not going to pretend otherwise. [Full analysis in Sustainability Audit §7.] Glass panels are on the v2 investigation list, along with a possible panel take-back program for custom units.
- **I can't guarantee the exact ESP32 module will be manufacturable in ten years.** Espressif's roadmap is theirs, not mine. If the C3-MINI-1 goes end-of-life, I will document the pin-compatible successor and publish an updated PCB footprint — but that's a best-effort commitment, not an absolute one. What I *can* guarantee is that *some* module will be able to drive the LEDs; the architecture is deliberately generic.
- **The firmware is a closed binary.** This is the one place where the philosophy and a business decision don't fully agree, and I'd rather name the tension than paper over it. The hardware is documented enough — schematic, BOM, versioned PCBs, STLs on request — that the circuit is in principle reproducible. The firmware is not. In practice this means a bare replacement board has to come through me to be flashed; an owner can rebuild the hardware around a dead controller but can't boot a blank one without my help. What I commit to instead is: **a reflash service** for the cost of shipping, and **ongoing OTA updates** so software issues never require opening the product. This is a position I'm willing to revisit as the business evolves — including the question of firmware escrow if I ever discontinue the product line or shutter the business.
- **I am one person.** The support response time is "a human who built your mirror, replying when they can" — not a 24/7 helpdesk. That's a feature until the day the volume grows past what one person can hold, at which point the commitment transitions with the brand. First-person "I" becomes first-person plural "we" only when the person on the other end of an email actually changes.

---

## 5. Compliance as Reinforcement

The relevant legislation at time of writing:

- **EU Right-to-Repair Directive** — adopted 2024, phasing into national law across member states through 2026–2027. Mandates repair information, spare parts availability, and standard-tool repair for covered electronics.
- **Washington HB-1933** and **California SB-244** — state-level analogues of the EU framework, both in force or phasing in, mandating repair access for consumer electronics sold in-state.
- **Emerging FTC guidance** on warranty tying and repair-restriction practices.

The v1.0 infinity mirror, as designed, already meets the repairability requirements in all three. This is not a retrofit. It's what happens when the values and the design brief agree with each other from the start.

I note this here for completeness — compliance matters, and the legal ceiling will only rise from here — but the commitments in §3 are not downstream of legislation. They'd be the commitments regardless.

---

## 6. What This Asks Of The Buyer

Two things, neither of them large:

- **Before you throw it out, ask me.** I'd rather fix it than replace it, and I'd rather replace it than see it in a landfill. The email address that sold it to you is the same one that can still help.
- **Don't seal it shut.** Every modification the enclosure allows is one I want you to make — mount it differently, swap the LEDs for a different density, print a variant frame. But please leave it reopenable. That's the whole point.

The buyer reaches this document through a QR code laser-etched (or perforated) into the outer cardboard, produced in the same cutting pass that forms the folded box. Zero extra material, and the path to the repair philosophy is the same piece of cardboard the owner would otherwise throw away. Repair promises that only live on a marketing page are easy to miss; repair promises on the box are not.

---

## 7. Why This Is A Document

Repair commitments are easy to claim in marketing copy and easy to walk back when they get expensive. Writing the reasoning down — publicly, in first-person, with the limitations stated alongside the promises — is what makes the commitment real.

This document is the version of the argument that gets tested when something does go wrong and a specific buyer emails me about a specific broken mirror. If what I do in that moment doesn't match what's written here, the document is wrong — and I'd rather rewrite the document to match reality than let the two drift apart quietly.

So: if you're holding one of these mirrors and something on this page reads as aspirational rather than real, tell me. That's the only way it stays honest.

---

## Related

- [Brand Positioning](brand-positioning.md) — voice and values this doc inherits (§4 voice, §3 "Living" adjective)
- [Sustainability Audit](sustainability-audit.md) — lifecycle evidence behind the repair claims (Stage 6)
- [Market Analysis Report](Market%20Analysis%20Report.md) — regulatory context (§3.E Repairability Liability)
- [Stakeholder Map](stakeholder-map.md) — end-of-life and repair stakeholders this philosophy is written for
- [Firmware Architecture Scoping](firmware-architecture-scoping.md) — where the closed-binary + self-hosted OTA + reflash-service commitments get technical definition
- User Repair Guide *(pending — Week 4 → Week 8 final)* — the practical how-to companion
- [Sprint Plan](../sprint_plan.md) — Week 4 deliverables
