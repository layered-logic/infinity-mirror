# Sprint Log — Spring 2026

Progress tracker for the Infinity Mirror sprint plan. Organized by sprint week.
Each entry is a human-written description with a date. The sprint scheduler LLM
fuzzy-matches these against `sprint_plan.md` to determine remaining work.

---

## Week 1 (Mar 30 – Apr 4): Competitive Landscape & Stakeholders

### Done
- [x] LLC Certificate of Formation filed (Mar 23, pre-quarter)
- [x] EIN received from IRS (Apr 2) — 41-5282028
- [x] Operating agreement — finalized, printed, and signed (Apr 6)
- [x] Pricing calculator MVP built — Streamlit dashboard with dynamic BOM, size scaling, tier/platform selection, and margin analysis (`tools/pricing/`) (Apr 2)
- [x] Stakeholder mapping exercise completed (Apr 2) — 45-min coached sprint with Gemini acting as investor/board member. Produced comprehensive 5-phase stakeholder map covering 24 stakeholders across Creation, Distribution, Acquisition, Lived Experience, and Support/Legacy. Formalized as `docs/stakeholder-map.md`.

- [x] ESP32 module selected — ESP32-C3-MINI-1 (Apr 6). Chosen for smallest footprint, lowest cost (~$2 LCSC), wide availability, and built-in USB Serial/JTAG (no external UART chip needed on PCB). Researched all current Espressif modules; full comparison in `docs/esp32-module-comparison.md`, shortlist analysis in `docs/esp32-module-shortlist.md`. Note: ESP32-C6-MINI-1 (adds Zigbee/Thread/Matter) flagged as a potential future SKU variant — decision deferred to customer research (survey early buyers on smart home usage).

### In Progress

### Done (continued)
- [x] Business bank account application submitted (Apr 9). NAICS 335129 (Other Lighting Equipment Manufacturing), calendar fiscal year (12/31), established 03/2026.

- [x] Market analysis — competitive landscape research completed (Apr 6). Cross-platform analysis of 30+ listings across Etsy, Amazon, Pinterest, and commercial signage vendors. Produced `docs/Market Analysis Report.md`. Key findings: market is bifurcated into 4 segments (Bespoke Luxury $800–$5k+, B2B Branding $250–$1.2k, Sensory/Therapy $230–$4.3k, Lifestyle/Gaming $40–$250). The $50–$120 bracket has 1,900+ Etsy listings — saturation risk if industrial design doesn't signal $200+ value. Matter/Thread protocol fragmentation flagged as a product risk for wi-fi-only devices. Repairability legislation (EU + WA/CA) is both a compliance requirement and a differentiator. FCC Part 15 compliance noted for LED driver EMI.

### Blocked

---

## Week 2 (Apr 7 – Apr 11): Pricing & The Sustainability Audit

### Done
- [x] Sustainability audit completed (Apr 9). Full 7-stage cradle-to-grave lifecycle analysis produced as `docs/sustainability-audit.md`. All power data measured via smart plug (6,368 samples): idle 0.1W, rainbow cycling 1.25W avg, full white 2.8W (firmware-capped at ~29% of WS2812B rated max). Annual energy at worst-case usage: 4.91 kWh (typical) – 10.34 kWh (peak). LED lifespan projected at 14+ years. Embodied carbon ~2.5–2.7 kg CO2 cradle-to-door. Key hotspots: coated acrylic panels (non-recyclable at EOL), PSU oversizing (25W brick at 11% load — flagged for v2 right-sizing). Cardboard packaging and PETG separability are sustainability strengths.

- [x] BOM breakdown completed (Apr 9). Full material + labor + fixed cost analysis at 1/50/100/1,000 units for basic 6x6 model. Produced `docs/bom-breakdown-basic-6x6.md`. Key findings: variable COGS ranges from $81.62 (1 unit) to $49.80 (1,000 units); labor is the dominant cost at all volumes (exceeds materials). $50 retail only reaches $60/hr effective rate at 1,000 units — $65 recommended as base price for early volumes. Fixed capital (laser + printer) totals $7,153.11.

- [x] JLCPCB PCB cost analysis (Apr 9). Analyzed basic STM8 PCB BOM (7 component lines, 9 parts, 40 joints). JLCPCB estimate: $3.61/board at qty 5, $1.04/board at qty 50, $0.90/board at qty 100. Pricing_config.yaml $5.00 controller placeholder is significantly overstated — actual assembled PCB is ~$1/board at production volume. Extended parts: STM8 (C52717) + USB-C connector (C456012) each add $3 setup. TTP223 capacitive touch sensor evaluated as potential add-on: ~$0.10/board all-in, basic part (no extended fee), senses through PETG at 1–2mm. ESP32 PCB BOM pending refresh.

### In Progress

### Blocked

---

## Week 3 (Apr 14 – Apr 18): Operations & Critical Path

### Done

### In Progress

### Blocked

---

## Week 4 (Apr 21 – Apr 25): Brand Identity & Repairability

### Done

### In Progress

### Blocked

---

## Week 5 (Apr 28 – May 2): App UX/UI Design

### Done

### In Progress

### Blocked

---

## Week 6 (May 5 – May 9): Hardware Integration & Supply Chain

### Done

### In Progress

### Blocked

---

## Week 7 (May 12 – May 16): Service Blueprinting & The Custom Experience

### Done

### In Progress

### Blocked

---

## Week 8 (May 19 – May 23): Sustainability Deep Dive & Legal

### Done

### In Progress

### Blocked

---

## Week 9 (May 26 – May 30): Marketing Storytelling & Assets

### Done

### In Progress

### Blocked

---

## Week 10 (Jun 2 – Jun 6): Portfolio Construction

### Done

### In Progress

### Blocked

---

## Week 11 (Jun 9 – Jun 12): Launch Readiness Audit

### Done

### In Progress

### Blocked
