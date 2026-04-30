import { useEffect, useState } from 'preact/hooks';

// Hash-based router: reads window.location.hash, listens for `hashchange`,
// returns the current route + a navigate function. Picked over a real
// history-API router because the device serves a single index.html for
// every path (the captive-portal 404 handler redirects everything to /),
// so deep links via path don't survive a refresh — hash routing dodges
// that entirely.
//
// Routes used:
//   #/         → control page (default)
//   #/setup    → provisioning form (auto-routed to when device is in AP mode)
//   #/settings → settings page (manual nav from header)

export type Route = '/' | '/setup' | '/settings';

function parseHash(hash: string): Route {
  const stripped = hash.replace(/^#/, '') || '/';
  if (stripped === '/setup') return '/setup';
  if (stripped === '/settings') return '/settings';
  return '/';
}

export function useHashRoute(): [Route, (r: Route) => void] {
  const [route, setRoute] = useState<Route>(() => parseHash(window.location.hash));

  useEffect(() => {
    const onHashChange = () => setRoute(parseHash(window.location.hash));
    window.addEventListener('hashchange', onHashChange);
    return () => window.removeEventListener('hashchange', onHashChange);
  }, []);

  const navigate = (r: Route) => {
    if (r === parseHash(window.location.hash)) return;
    window.location.hash = r === '/' ? '' : `#${r}`;
  };

  return [route, navigate];
}
