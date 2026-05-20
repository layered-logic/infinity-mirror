---
title: Outreach List
type: research
phase: 2
status: active
tags: [outreach, user-research, marketing, hcde]
---

# Outreach List — Layered Logic Infinity Mirror

Working contact list for [LL-043](../tasks.md#LL-043) and the content-creator branch [LL-043-1](../tasks.md#LL-043-1). Most outreach is **cold**. Tracking reply rates by lane will tell us where to invest the next round.

Gates:
- [LL-048](../tasks.md#LL-048) — end-buyer interviews
- [LL-049](../tasks.md#LL-049) — coffee shop / showcase partner interviews
- [LL-043-1](../tasks.md#LL-043-1) — content-creator + streamer outreach campaign

## Conventions
- **Sent** = day Bill actually pushed the message.
- **Status** = `sent` / `replied` / `passed` (they declined) / `meeting-set` / `done` / `no-reply` (declare after 14 days of silence — don't re-poke).
- **Reach via** = the platform/address used so we can audit reply rates by channel.
- Mark `no-reply` instead of deleting — it's a baseline for next round.

## Decisions locked (May 14)
1. **Script-driven outreach** — per-lane variants, not one-size-fits-all. See [Outreach scripts](#outreach-scripts).
2. **The ask** — 15–20 min conversation about how they think about studio backgrounds and what they'd find: want / need / cool / problematic. (Tighter than "talk to me about the product.")
3. **No-reply window** — 14 days, no re-poke.
4. **LL-NNN** — content-creator outreach is now [LL-043-1](../tasks.md#LL-043-1), child of LL-043.

---

## Sent

### YouTube creators

| Name | Channel | Lane fit | Reach via | Sent | Status |
|---|---|---|---|---|---|
| Philip DeFranco | News commentary, ~6M | **Weak** — news/talk audience, sponsorships are tax/finance/lifestyle. Brand fit thin. | text line | 2026-05-14 | sent |
| Corridor Digital | VFX studio, ~6M | **Strong** — maker-aesthetic studio, prop/lighting builds. Long shot at their size, story-aligned. | support ticket | 2026-05-14 | sent |

### Streamers (Discord DM)

| Name | Platform | Lane fit | Reach via | Sent | Status |
|---|---|---|---|---|---|
| Aztecross | Twitch (Destiny) | Strong for "gaming desk vibe" — ambient RGB is native to the audience. | Discord DM | 2026-05-14 | sent |
| Jake the Alright | Twitch | Same lane, smaller channel → higher reply odds than Aztecross. | Discord DM | 2026-05-14 | sent |

### Smart-home creators (Lane A)

| Name | Channel | Lane fit | Reach via | Sent | Status |
|---|---|---|---|---|---|
| Reed Kleinman | Smart Home Solver | Strongest Lane A pick — direct personal email, audience = "Spec-Head" archetype, ESPHome bootstrap connection. | reed@smarthomesolver.com | 2026-05-14 | sent |
| Robert Tait | The Hook Up | Unsponsored, engineer-driven reviews — ethos match for the right-to-repair angle. | smarthomehookup@gmail.com | 2026-05-14 | sent |
| Lewis | Everything Smart Home | Reviews Govee/Nanoleaf-class — direct competitor coverage. UK-based, Home Assistant focus. | everythingsmarthome.co.uk/contact | 2026-05-14 | sent |
| Brandon Hopkins | TechHut | Home Assistant / homelab, open-source-leaning. | brandon@techhut.tv | 2026-05-14 | sent |

### Maker / DIY creators (Lane B)

| Name | Channel | Lane fit | Reach via | Sent | Status |
|---|---|---|---|---|---|
| Matt Perks | DIY Perks | Premium DIY lighting builds, ~4.9M subs. Direct personal email. Reply odds low at his size but signal from a yes is high. | diyperks@gmail.com | 2026-05-14 | sent |
| Hardware Haven (Colten) | — | Newer / growing, hardware re-use focus. Direct email = good reply odds. | colten@hardwarehaven.media | 2026-05-14 | **replied 2026-05-15**, swamped through end of month. Followup sent same day: UW HCDE framing + smart-home-indicator positioning + studio-curation question (transposed from [user-interview-outline §1](user-interview-outline.md)) + AI render of mirror with channel logo and real prototype photo attached. Awaiting his read. |

### Indie-founder / philosophy angle

| Name | Channel | Lane fit | Reach via | Sent | Status |
|---|---|---|---|---|---|
| Joe Barnard | BPS.space | Founder-to-founder ask, not audience-fit. Resonance with his personal-engineering-project philosophy videos. Different ask: insight on the project-to-venture transition + user feedback. | joe@bps.space | 2026-05-14 | sent |

> Aztecross business fallback (if Discord goes nowhere): aztecrossgaming@gmail.com — agent at aztecross@loaded.gg ([source](https://www.youtube.com/channel/UClbllR4Tx-lhYJyrpu1sA4A)). Loaded.gg agent contact only worth using if the message is materially different from the Discord DM.

---

## Drafted — ready to send (Wed May 20 batch)

The 5-target Monday queue, drafted and ready for Bill to review and send. Send window is Mon–Thu morning recipient-local; **Wed May 20 is a valid day**. After sending, move each row to the [Sent](#sent) tables and strike it from [Candidates](#candidates--next-round).

All five contacts were email-verified via web search on May 15. Re-confirm the address on the channel's About / contact page if it's been longer.

### 1. Hackaday (Lane D — press tip-line)

**To:** `tips@hackaday.com` · **Subject:** ESP32-C3 infinity mirror, built repair-first

```
Hi Hackaday team,

Sharing a project in case it's a fit for the blog: a 6x6 ESP32-C3 infinity
mirror I built around repairability. It comes apart with no tools, the LED
strips and controller swap out on JST connectors, and it runs self-hosted
OTA so the firmware stays serviceable.

A couple of things that might be column-shaped: the whole control surface is
a local WebSocket protocol with an app, a device-hosted webapp, and physical
buttons, each meant for a different situation. And the repair philosophy is
operationalized down in the BOM, not just stated as a value.

Happy to send photos, a short demo video, and the failure-mode and repair
documentation if you'd like a closer look.

Thanks so much,
Bill
```

*Optional attachment:* one prototype photo. Hackaday tips are stronger with a visual.

### 2. iFixit (Lane E — repair-voices editorial)

**To:** `press@ifixit.com` · **Subject:** An infinity mirror designed to be repaired

```
Hi iFixit team,

I'm an engineering student, and for the last few months I've been building
an infinity mirror with repairability as the starting constraint, not an
afterthought.

In practice that meant it comes apart with no tools, the LED strips and
controller swap out on connectors instead of solder, and I wrote the full
failure-mode and repair documentation before the product ships rather than
after.

I thought it might interest you as a small example of designing a consumer
light product the way iFixit argues products should be designed. Happy to
send photos, the repair docs, and a teardown if you'd like a look.

Thanks so much,
Bill
```

*Optional attachment:* one prototype photo.

### 3. Tested (Lane B/D — indie maker story)

**To:** `tips@tested.com` · **Subject:** Infinity mirror, one-person hardware venture

```
Hi Tested team,

I've been following the indie-maker stories you cover, and I wanted to share
one in case it fits.

I'm an engineering student building an ESP32 infinity mirror, and I've been
taking it the whole distance a real product needs: brand, firmware, an app,
repair documentation, packaging, and the business side, all as a one-person
venture. It started as a single-button desk gadget and turned into something
I'm trying to make legitimate.

No pitch here, just thought the solo build-it-all-yourself angle is the kind
of thing Tested tends to champion. Happy to send photos and a short video if
you'd like to see it.

Thanks so much,
Bill
```

*Optional attachment:* one prototype photo.

### 4. Strange Parts — Scotty Allen (Lane B — indie-founder)

**To:** `scotty@strangeparts.com` · **Subject:** Quick question from another hardware tinkerer

```
Hey Scotty,

Realize you probably get a lot of these, so I'll keep it short.

I'm an engineering student turning an infinity mirror from a hobby project
into a small hardware product, doing the whole thing solo: firmware, app,
brand, the business side. You've documented that messy hobby-to-product jump
in a way most people skip past, which is why I'm writing to you specifically.

Would you have 15 or 20 minutes sometime in the next couple weeks for a call?
I'd genuinely value your read on where this kind of project tends to go wrong
on the way to becoming something real. If a call is too much, even a few
sentences back would help.

Thanks so much,
Bill
```

*Opener alternatives:* if Bill has a specific Strange Parts video to reference, swap the hedge line for a one-sentence honest reference to it (don't fake familiarity). Otherwise the hedge stands. No attachment — the ask is about the project journey, not visual.

### 5. Hardware Canucks — Dmitry (Lane C — gaming/desk)

**To:** `dmitry@hardwarecanucks.com` · **Subject:** Ambient lighting piece for desk setups

```
Hey Dmitry,

Realize you probably get a lot of these, so I'll be quick.

I'm an engineering student building an ESP32 infinity mirror meant to live in
a setup as ambient lighting, the kind of thing that ends up behind a desk or
on a shelf in a build shot. It runs custom patterns and works on its own
without needing a smart-home hub.

Hardware Canucks works ambient lighting into build content now and then, so
before I lock the v1 feature set I'd love your read on how you'd size it up
for that kind of audience. Even a few sentences back would be a big help.

Thanks so much,
Bill
```

*Optional attachment:* one prototype photo (the question is partly visual). *Opener alternatives:* swap the hedge for a specific-build reference if Bill has one in mind.

> **Drafting notes:** No public repo link included — the codebase is closed-binary (open-source-on-founder-exit), so "documented publicly at {repo}" from the Variant 3 template is dropped; press contacts get an offer to send docs instead. No LLC signature per [feedback_cold_outreach_drafts](../.claude/projects/C--Users-bowhi-Desktop-Independent-Study/memory/feedback_cold_outreach_drafts.md). No em dashes. Subject lines all under 8 words.

---

## Candidates — next round

> **Verification status legend:** ✅ contact info confirmed via web search; ⚠️ channel confirmed but contact is form-only or social DM; ❓ name needs verification before sending. Verify the email is still current via the channel's About page before pasting into n8n.

### Lane A — Smart-home / ESP32 (highest fit, highest reply odds)

Bullseye audience overlap with the "Spec-Head" archetype from [stakeholder-map.md §3.1](stakeholder-map.md#31-the-self-buyer). Smaller channels than the news/film mega-creators, less inbound competition, and indie ESP32 hardware is their bread and butter.

| Name | Channel / handle | Contact | Notes |
|---|---|---|---|
| ~~Smart Home Solver — Reed Kleinman~~ | — | — | **Sent 2026-05-14.** Moved to [Sent table](#youtube-creators). |
| ~~The Hook Up — Robert Tait~~ | — | — | **Sent 2026-05-14.** Moved to [Sent table](#youtube-creators). |
| ~~Everything Smart Home — Lewis~~ | — | — | **Sent 2026-05-14.** Moved to [Sent table](#youtube-creators). |
| ~~TechHut — Brandon Hopkins~~ | — | — | **Sent 2026-05-14** via `brandon@techhut.tv`. Moved to [Sent table](#youtube-creators). |
| ~~bitluni — Matthias Balwierz~~ | — | — | **Channel-blocked.** Only surfaced contacts are X / Mastodon / Tindie. Per [outreach channel rule](../.claude/projects/C--Users-bowhi-Desktop-Independent-Study/memory/feedback_outreach_channels.md): no X/Twitter/Mastodon. Drop unless an email or accepted DM channel surfaces. |

### Lane B — Maker / DIY tech (story-aligned)

The HCDE narrative (right-to-repair, solo founder, JST-modular, open-source-on-exit) is content-shaped for these channels.

| Name | Channel / handle | Contact | Notes |
|---|---|---|---|
| ~~DIY Perks — Matt Perks~~ | — | — | **Sent 2026-05-14.** Moved to [Sent table](#maker--diy-creators-lane-b). |
| ~~Hardware Haven~~ | — | — | **Sent 2026-05-14.** Moved to [Sent table](#maker--diy-creators-lane-b). |
| Adam Savage / Tested | YouTube — Tested | ❓ editorial tips — verify via [tested.com](https://www.tested.com/) | Champions indie maker stories. Very long shot but the brand value of "covered by Tested" is enormous. |
| Strange Parts — Scotty Allen | YouTube — Strange Parts | ❓ verify contact | Indie-hardware founder stories with personality. |
| Big Clive | YouTube — bigclivedotcom | ❓ verify contact | LED-strip + cheap-electronics teardowns. Technical-credibility audience. |

### Lane C — Gaming-setup / desk-aesthetic (extends the streamer push)

If the streamer hypothesis is right, these video creators are the YouTube equivalent.

| Name | Channel / handle | Contact | Notes |
|---|---|---|---|
| Optimum Tech | [@optimum](https://www.youtube.com/channel/UCRYOj4DmyxhBVrdvbsUwmAA) | ⚠️ About-tab email not surfaced · [Facebook page](https://www.facebook.com/youtubeoptimumtech/) | Ultra-clean gaming/desk setups, PC hardware. Strong aesthetic match. |
| Hardware Canucks | YouTube — Hardware Canucks | ❓ verify | Occasionally covers ambient lighting in builds. |

> **Name correction:** I earlier listed "TheBitWit (Kevin Kennedy)" — that combo doesn't exist. There's **Bitwit** (Kyle Hansen, PC builds), and a "Kevin Kennedy" channel that's a separate person. Removed rather than chase a confabulation. If you want a desk-setup-flavored YouTuber here, Optimum Tech is the closest match in my search.

### Lane D — Press / blog tip-lines (different mechanic)

Lower commitment for them = higher hit rate for us. Written coverage indexes into search.

| Outlet | Contact | Notes |
|---|---|---|
| Hackaday | ✅ tips@hackaday.com · [submission guide](https://hackaday.com/submit-a-tip/) | Title clearly ("Layered Logic — ESP32-driven infinity mirror, right-to-repair-first"), include photos + video + repo link. Don't pitch — describe. |
| Adafruit "Show and Tell" | ⚠️ weekly Wed 7:30pm ET on YouTube live + Discord — join chat to show in person | Hosted by [John Park](https://blog.adafruit.com/author/john/). Live show; bring the prototype and demo. Higher-effort but high-quality audience. |
| Hackster.io | ❓ editorial contacts public — verify | Indie hardware editorial coverage. |

### Lane E — Right-to-repair voices (brand decision required)

| Name | Platform | Notes |
|---|---|---|
| iFixit editorial | Blog | Corporate, well-aligned, professional. Safe yes. Press contact on iFixit.com. |
| Louis Rossmann | YouTube | The loudest repair-advocacy voice. Controversial / abrasive — brand-association decision required before sending. |

### Lane F — Coffee shops / showcase partners ([LL-049](../tasks.md#LL-049))

Distinct from the creator lanes: these contacts can't be named from training data, and most are reached in person rather than by email. **Bill builds the named list from a local pass** — the cafe names are local knowledge, and guessing them here would just risk a wrong name. This section is the scaffold and the method; the table below is Bill's to fill.

**How to build the list — target 5-7 venues:**
1. **Maps walk-through** of U-District, Capitol Hill, Ballard. Note any cafe that already rotates local-artist work — they have a real process to describe, which makes the highest-value interview.
2. **3-5 cafes Bill visits regularly** — warm-by-familiarity even though the ask is cold. The easiest yeses.
3. **1-2 cafes that don't currently showcase art** — the control case: "why not?" is as informative as "how?"
4. **Stretch:** a co-working space or barber shop — same ambient-display dynamic, different economics.

**What to leave each conversation with** (from [stakeholder-map.md §2.6](stakeholder-map.md#26-local-showroom-partners-coffee-shops--small-businesses) + the [showcase-partner interview outline](user-interview-outline.md)): how they source artists, the yes/no signals, the economics (commission / wall rent / goodwill), install + power constraints, and whether an LED piece is the wrong vibe for the space.

**Approach + message:** [Outreach scripts → Variant 4](#variant-4--cafe--showcase-partner). In person is the default; an email / Instagram-DM fallback is there for venues a walk-in doesn't fit.

| Cafe / venue | Neighborhood | Shows art now? | Familiarity | Reach via | Sent | Status |
|---|---|---|---|---|---|---|
| _(one row per venue from the walk-through)_ | | | | | | |

### Lane G — End-buyer interview slots ([LL-048](../tasks.md#LL-048))

1. **UW HCDE cohort outside immediate friend group** — end-buyers in archetype *and* can critique the interview method itself.
2. **Subreddit recruiting** — short post in r/InfinityMirror, r/Hue, r/SmartHome, r/battlestations offering $20–25 gift card for 20-min Zoom feedback. Read each sub's rules first.
3. **HCDE faculty / cross-discipline grad students** — design-literate, will engage substantively.

---

## Outreach scripts

The ask is the same across all variants — **15–20 min on how they think about studio backgrounds and what's want / need / cool / problematic.** The opener and the "why you" change per lane.

### Variant 1 — Streamer / gaming-creator (Bill's draft, lightly tightened)

For: Aztecross, Jake the Alright, future streamers and gaming-setup YouTubers (Optimum Tech, Hardware Canucks).

**Hook:** AI-modified prototype render with their channel logo replacing the default pattern (see [n8n workflow](#n8n-workflow-outline)).

```
Hey {first_name},

{personalized_opener — 1–2 sentences referencing something specific they made.
Example for Cross: "I came across your marathon vids pre-launch and really
enjoyed following your Bungie content."}

Not sure if this is the right channel to reach out through, but figured I'd
try. I'm an engineering student building a product targeted at streamers —
ambient infinity-mirror art that can run a channel's branding as a live light
pattern. Would you have 15–20 min sometime in the next couple weeks for a
quick call? I'd love to hear how you think about what goes into your studio
background — specifically what you'd find a *need*, a *want*, *cool*, vs.
*problematic*.

Quick teaser — here's an AI render of my prototype with your logo in place of
the default pattern: {attached_image}

Totally fine if it's not your thing or not the right time.

— Bill (Layered Logic, layeredlogic.cc)
```

### Variant 2 — Smart-home / ESP32 creator

For: Smart Home Solver, The Hook Up, Everything Smart Home, TechHut, bitluni.

**Why different from V1:** these audiences don't care about logo personalization — they care about technical credibility, open-source posture, and how it slots into Home Assistant. The logo-render hook is *weaker* here (a few will find it gimmicky); the spec teaser is *stronger*. Optional photo: the prototype as-shipped, not logo-modified.

```
Hey {first_name},

{personalized_opener — reference a specific video. Example for Reed:
"Your video on {specific topic} was the reason I switched to {thing} —
been a subscriber since."}

I'm an engineering student building an ESP32-C3-based infinity mirror, designed
from the start around right-to-repair (toolless JST disassembly, replaceable
LED strips and controllers, self-hosted OTA, open-source on founder exit). It
works standalone out of the box — Home Assistant / smart-home integration is
opt-in, not a gate on the basic product.

Before I lock the v1 feature set, I'd love 15–20 min of your time to hear how
you'd think about it from your audience's perspective — what's a *need*, a
*want*, *cool*, vs. *problematic*. No pitch, no review ask — just want the
read.

If 20 min is too much, even a couple of paragraphs back would be hugely
helpful.

— Bill (Layered Logic, layeredlogic.cc)
```

### Variant 3 — Press / blog tip-line

For: Hackaday, Hackster.io. (Adafruit Show-and-Tell is a live show, not a tip-line — different workflow.) **Subject line is the whole game.**

```
Subject: Layered Logic — ESP32-C3 infinity mirror with right-to-repair-first build

Hi {outlet} team,

Sharing a project I've been working on: a 6×6 ESP32-C3 infinity mirror
designed around toolless disassembly, JST-modular electronics, self-hosted
OTA, and an open-source-on-founder-exit commitment. The build is documented
publicly at {repo URL}.

Highlights that might be column-shaped:
- {bullet 1 — a specific technical or design choice that's not obvious}
- {bullet 2 — the right-to-repair philosophy + how it's operationalized in the BOM}
- {bullet 3 — the app + webapp + standalone-button triad and what each is for}

Photos: {links}
Demo video: {link, if available}

Happy to answer questions or send a unit out for closer look.

— Bill (Layered Logic, layeredlogic.cc)
```

### Variant 4 — Cafe / showcase partner

For: [Lane F](#lane-f--coffee-shops--showcase-partners-ll-049) — local cafes, co-working spaces, any venue that hosts makers on its walls. The audience is an owner or manager, not a creator with a following, so the ask is different: not "would you cover this" but "would you tell me how you decide." The [showcase-partner interview outline](user-interview-outline.md) is the conversation guide once someone says yes.

**Default: in person.** Walk in at a quiet hour (mid-afternoon, not the morning rush). Ask whoever's at the counter whether the owner or manager is around, or when they usually are. Spoken opener:

> Hi, I'm Bill, a UW student working on a small lighting and art project. I'm not selling anything. I'm trying to learn how cafes like yours decide who gets to show work on the walls. Could I ask whoever handles that a few questions sometime, fifteen or twenty minutes?

If yes, run the interview outline. If not now, get the best time or channel to reach the manager and come back.

**Fallback: email / Instagram DM** — for venues where a walk-in doesn't fit, or that list a contact and would rather hear first.

```
Subject: Quick question from a UW student

Hi {cafe name} team,

I'm a UW student working on a small lighting and art project, and I'm
trying to learn how cafes choose the local makers and artists they show
on their walls.

Could I stop by and ask whoever handles that a few questions sometime?
Fifteen or twenty minutes, no pitch, I just want to understand how the
decision actually gets made.

If a visit doesn't work, even a couple of sentences on how you pick
would be a real help.

Thanks so much,
Bill
```

Same tone as the creator variants: one-sentence project description, no spec list, no LLC signature, no em dashes. Both the spoken opener and the email keep one self-disclosure — "I'm not selling anything" / "no pitch" — because a cafe owner's default read of a stranger asking about wall space is a sales pitch, and naming that up front clears it.

---

## n8n workflow outline

The streamer/gaming variant is the most automatable: it has a personalized image generation step that benefits from a pipeline. Smart-home + press variants don't need an image step and might be better as a Google Doc + manual send to keep tone right.

### Inputs per recipient (one row per send)

| Field | Source | Notes |
|---|---|---|
| `first_name` | manual | "Cross", "Reed", "Lewis", etc. |
| `recipient_handle` | manual | Channel/@ handle for record-keeping |
| `lane` | manual | `streamer` / `smart-home` / `maker` / `press` — selects script variant |
| `personalized_opener` | manual or LLM-assisted | 1–2 sentences referencing specific content. **Don't fully automate this — auto-generated openers read as spam.** |
| `contact_endpoint` | manual | Email, Discord URL, contact form URL |
| `logo_asset_url` | manual or scraper | Square channel logo PNG (only needed for streamer variant) |
| `prototype_image_url` | constant | The base prototype photo to feed nano-banana |
| `script_variant` | derived from lane | Template selector |

### Steps

1. **Sheet trigger** — new row in Google Sheet or Airtable with the fields above.
2. **Branch on lane:**
   - `streamer`: call nano-banana / Gemini image-edit API with `{prototype_image_url, logo_asset_url, prompt: "Replace the triangular pattern in the LED with this logo, preserve the blue glow and frame"}`. Save output.
   - `smart-home` / `maker` / `press`: skip image step.
3. **Template render** — fill the variant-appropriate script with `{first_name}`, `{personalized_opener}`.
4. **Human approval gate** — drop the rendered message + attachment into a Slack/Discord/email-to-self channel. **Do not auto-send.** Review and send manually. (The personalization step is the entire reason this gets read — auto-sending would cargo-cult away the value.)
5. **Status write-back** — once Bill marks "sent" in the queue, append a row to this doc's Sent table (or the equivalent sheet).
6. **14-day timer** — n8n schedule trigger checks `sent_date + 14d`. If no reply marked, auto-flips status to `no-reply`.

### Where to be lazy

- Don't try to scrape personalized openers from YouTube transcripts automatically. The signal-to-noise is bad and a wrong reference is worse than a generic opener. Manual fill, 30 sec per recipient.
- Don't try to auto-detect replies. Email/Discord reply detection across channels is brittle. Manual status updates.
- Don't try to image-edit for smart-home / maker / press. They don't want a logo render.

### Where automation actually saves time

- Logo scraping + nano-banana batch — turn 10 logos into 10 rendered prototypes overnight, queued for human review the next morning.
- Script templating — eliminates copy-paste error and inconsistent signature/links.
- 14-day timer — closes the loop without manual ticklers.

---

## Open decisions remaining

1. **Personalized opener — manual or LLM-drafted?** Manual is safer (no hallucinated references) but slower. LLM-drafted with human review may strike the right balance for batches of 5+.
2. **For streamers reached via Discord DM** — the message format above assumes email/text. Discord 2000-char limit + no real attachment workflow means the image gets uploaded separately. Test the flow on Aztecross's reply (if any) before scaling.
3. **Brand decision on Louis Rossmann outreach** — defer until at least one Lane A/B reply lands and we have a feel for the brand reception.

---

## Related

- [Stakeholder Map](stakeholder-map.md) — archetypes and influence-matrix this list draws from
- [Service Blueprint](service-blueprint.md) — Stage 1 (Discover) names the channels these creators feed
- [Interview Outline](user-interview-outline.md) — the conversation guide once a contact replies
- [Task Registry](../tasks.md) — [LL-043](../tasks.md#LL-043) / [LL-043-1](../tasks.md#LL-043-1) / [LL-048](../tasks.md#LL-048) / [LL-049](../tasks.md#LL-049)
