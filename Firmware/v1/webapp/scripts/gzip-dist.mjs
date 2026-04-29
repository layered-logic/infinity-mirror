// Postbuild step: gzip dist/{index.html,app.js,app.css} for the firmware embed.
//
// The ESP-IDF webapp_assets component embeds the .gz outputs via EMBED_FILES
// and serves them with Content-Encoding: gzip. Pre-gzipping at compile-best
// (level 9) costs nothing at firmware build time and shaves a few percent off
// the on-the-wire bytes vs. on-device gzipping (which we don't do anyway).
//
// Run automatically by `npm run build` via the package.json script.

import { readFileSync, writeFileSync, statSync } from 'node:fs';
import { gzipSync, constants } from 'node:zlib';
import { resolve, dirname } from 'node:path';
import { fileURLToPath } from 'node:url';

const here = dirname(fileURLToPath(import.meta.url));
const dist = resolve(here, '..', 'dist');

const targets = ['index.html', 'app.js', 'app.css'];

for (const name of targets) {
  const src = resolve(dist, name);
  const dst = `${src}.gz`;
  const raw = readFileSync(src);
  const gz = gzipSync(raw, { level: constants.Z_BEST_COMPRESSION });
  writeFileSync(dst, gz);
  const ratio = ((gz.length / raw.length) * 100).toFixed(1);
  console.log(
    `gzip: ${name.padEnd(11)} ${String(raw.length).padStart(6)}B -> ${String(gz.length).padStart(5)}B  (${ratio}%)`,
  );
}
