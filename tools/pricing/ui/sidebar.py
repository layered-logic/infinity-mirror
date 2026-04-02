"""Sidebar controls — mirror size, tier, platform, markup, shipping."""

from __future__ import annotations

from dataclasses import dataclass

import streamlit as st


@dataclass
class SidebarInputs:
    width: float
    height: float
    tier: str
    platform_key: str
    markup: float
    shipping: float


def render_sidebar(platforms: dict, defaults: dict) -> SidebarInputs:
    st.sidebar.title("Mirror Configuration")

    st.sidebar.subheader("Dimensions (inches)")
    width = st.sidebar.slider("Width", min_value=3.0, max_value=24.0, value=6.0, step=0.5)
    height = st.sidebar.slider("Height", min_value=3.0, max_value=24.0, value=6.0, step=0.5)

    st.sidebar.subheader("Product Tier")
    tier = st.sidebar.radio("Tier", ["basic", "pro"], index=1, horizontal=True)

    st.sidebar.subheader("Sales Channel")
    platform_labels = {k: v["label"] for k, v in platforms.items()}
    platform_key = st.sidebar.selectbox(
        "Platform",
        options=list(platform_labels.keys()),
        format_func=lambda k: platform_labels[k],
    )

    st.sidebar.subheader("Pricing")
    markup = st.sidebar.slider(
        "Markup multiplier",
        min_value=1.0,
        max_value=5.0,
        value=defaults.get("markup_multiplier", 2.5),
        step=0.1,
    )
    shipping = st.sidebar.number_input(
        "Shipping cost ($)",
        min_value=0.0,
        max_value=50.0,
        value=defaults.get("shipping_cost", 8.0),
        step=0.50,
    )

    return SidebarInputs(
        width=width,
        height=height,
        tier=tier,
        platform_key=platform_key,
        markup=markup,
        shipping=shipping,
    )
