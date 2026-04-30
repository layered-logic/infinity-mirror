import { ConnState } from '../ws-client';
import { Route } from '../hooks';

interface HeaderProps {
  conn: ConnState;
  route: Route;
  onNavigate: (r: Route) => void;
  // When set, replaces the live connection-status badge with a static
  // labeled badge. Used during the post-submit "credentials sent" view
  // where the cycling closed/connecting/closed badge is just noise —
  // we already know the connection is intentionally going away.
  connLabelOverride?: { label: string; tone: 'idle' | 'warn' | 'ok' };
}

function statusClass(s: ConnState): string {
  if (s === 'open') return 'status status--ok';
  if (s === 'connecting') return 'status status--warn';
  if (s === 'closed') return 'status status--err';
  return 'status status--idle';
}

// Right-side affordance flips based on the current page. Setup has no
// affordance — provisioning is a one-way flow until the device joins.
export function Header({ conn, route, onNavigate, connLabelOverride }: HeaderProps) {
  return (
    <header class="bar">
      <span class="title">Layered Logic Mirror</span>
      <div class="bar-right">
        {connLabelOverride ? (
          <span class={`status status--${connLabelOverride.tone}`}>
            {connLabelOverride.label}
          </span>
        ) : (
          <span class={statusClass(conn)}>{conn}</span>
        )}
        {route === '/' && (
          <button type="button" class="nav-link" onClick={() => onNavigate('/settings')}>
            settings
          </button>
        )}
        {route === '/settings' && (
          <button type="button" class="nav-link" onClick={() => onNavigate('/')}>
            back
          </button>
        )}
      </div>
    </header>
  );
}
