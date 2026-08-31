# Portfolio — "Process" (LL-061) · deploy & embed

Self-contained static page (`index.html` + `assets/`). Hosted on **Cloudflare Pages**.

- **Project:** `layered-logic-process` (account: bowhite9317@gmail.com)
- **Production URL:** https://layered-logic-process.pages.dev/
- **Intended custom domain:** `process.layeredlogic.cc` (see below)

## Redeploy (after editing the page)

From the repo root, with wrangler authenticated (`npx wrangler whoami`):

```bash
npx wrangler pages deploy portfolio --project-name=layered-logic-process --branch=main --commit-dirty=true
```

Only the `portfolio/` folder is uploaded — nothing else from the repo. Each run prints a unique
preview URL; the production URL above always serves the latest `main` deploy.

## Custom domain (process.layeredlogic.cc)

DNS for layeredlogic.cc is on Cloudflare, so this is one step:
Dashboard → Workers & Pages → `layered-logic-process` → Custom domains → **Set up a domain** →
`process.layeredlogic.cc` → Cloudflare auto-creates the CNAME and provisions the cert.

## Embed in Framer (auto-resizing)

Framer → Assets → Code → New Component. The page broadcasts its height via `postMessage`
(`{type:"ll-process-height", height}`) on load, resize, image load, and every node expand/collapse,
so the frame grows with the content (including when a visitor opens a node).

```tsx
import { useState, useEffect } from "react"

export default function ProcessEmbed() {
    const [height, setHeight] = useState(4000)
    useEffect(() => {
        function onMsg(e: MessageEvent) {
            if (e.data?.type === "ll-process-height" && typeof e.data.height === "number") {
                setHeight(e.data.height)
            }
        }
        window.addEventListener("message", onMsg)
        return () => window.removeEventListener("message", onMsg)
    }, [])
    return (
        <iframe
            src="https://layered-logic-process.pages.dev/"
            style={{ width: "100%", height, border: 0, display: "block" }}
            title="Layered Logic — Process"
            scrolling="no"
        />
    )
}
```

Set the component to full-width on the Framer page. Swap `src` to the custom domain once it's live.

### Note on sticky headers
With auto-height (frame grows to full content, parent page scrolls), the page's sticky lane/phase
headers won't pin — they scroll by like normal content. That's the right feel for an embedded story.
If you'd rather keep them pinned, give the iframe a fixed height (e.g. `height: "90vh"`) so it
scrolls internally instead.
