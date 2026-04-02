"""Layered Logic — Pricing Calculator Dashboard.

Launch: streamlit run tools/pricing/app.py
"""

from pathlib import Path

import streamlit as st

from models.bom import build_bom, load_config
from models.mirror import compute_mirror_spec
from models.pricing import full_pricing
from ui.sidebar import render_sidebar
from ui.bom_table import render_bom_table
from ui.price_summary import render_summary_cards, render_cost_breakdown, render_waterfall_chart

# --- Page config ---
st.set_page_config(page_title="Layered Logic — Pricing", layout="wide")
st.title("Layered Logic — Pricing Calculator")

# --- Load config ---
CONFIG_PATH = Path(__file__).resolve().parent.parent.parent / "Business" / "pricing_config.yaml"

if not CONFIG_PATH.exists():
    st.error(f"Config not found at {CONFIG_PATH}. Create Business/pricing_config.yaml first.")
    st.stop()

config = load_config(CONFIG_PATH)
bom = build_bom(config)

# --- Sidebar ---
inputs = render_sidebar(
    platforms=config["platforms"],
    defaults=config.get("defaults", {}),
)

# --- Compute ---
spec = compute_mirror_spec(
    width_in=inputs.width,
    height_in=inputs.height,
    led_density_per_meter=config.get("led_density_per_meter", 60),
    led_power_draw_amps=config.get("led_power_draw_amps", 0.06),
    led_voltage=config.get("led_voltage", 5),
)

result = full_pricing(
    spec=spec,
    bom=bom,
    tier=inputs.tier,
    platform_key=inputs.platform_key,
    platform_config=config["platforms"][inputs.platform_key],
    markup_multiplier=inputs.markup,
    shipping_cost=inputs.shipping,
)

# --- Display ---
render_summary_cards(result)

st.divider()

# Mirror specs sidebar info
with st.expander("Mirror Specs"):
    col1, col2, col3 = st.columns(3)
    col1.metric("Dimensions", f'{inputs.width}" x {inputs.height}"')
    col2.metric("LED Count", spec.led_count)
    col3.metric("PSU Required", f"{spec.psu_watts:.1f}W / {spec.psu_amps:.1f}A")

render_bom_table(result)

st.divider()

render_cost_breakdown(result)

st.divider()

render_waterfall_chart(result)
