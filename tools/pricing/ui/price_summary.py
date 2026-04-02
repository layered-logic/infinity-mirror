"""Price summary cards and waterfall chart."""

from __future__ import annotations

import streamlit as st

from models.pricing import PricingResult


def render_summary_cards(result: PricingResult) -> None:
    col1, col2, col3, col4 = st.columns(4)
    col1.metric("COGS", f"${result.cogs:.2f}")
    col2.metric("Sale Price", f"${result.sale_price:.2f}")
    col3.metric("Customer Pays", f"${result.customer_price:.2f}")
    col4.metric("Net Margin", f"{result.margin_pct:.1f}%")


def render_cost_breakdown(result: PricingResult) -> None:
    st.subheader("Cost Breakdown")

    col1, col2 = st.columns(2)

    with col1:
        st.markdown("**COGS Components**")
        st.markdown(f"- Materials: **${result.material_cost:.2f}**")
        st.markdown(f"- Labor: **${result.labor_cost:.2f}**")
        st.markdown(f"- Energy: **${result.energy_cost:.2f}**")
        st.markdown(f"- Overhead: **${result.overhead_cost:.2f}**")
        st.markdown(f"- **Total COGS: ${result.cogs:.2f}**")

    with col2:
        st.markdown("**From COGS to Customer**")
        st.markdown(f"- COGS: ${result.cogs:.2f}")
        st.markdown(f"- Markup ({result.markup_multiplier}x): **${result.sale_price:.2f}**")
        st.markdown(f"- Platform fees ({result.platform}): -${result.platform_fees:.2f}")
        st.markdown(f"- Shipping: +${result.shipping_cost:.2f}")
        st.markdown(f"- **Customer pays: ${result.customer_price:.2f}**")
        st.markdown(f"- **You keep: ${result.net_revenue:.2f}**")


def render_waterfall_chart(result: PricingResult) -> None:
    """Simple horizontal bar showing cost composition."""
    import pandas as pd

    data = pd.DataFrame(
        {
            "Category": ["Materials", "Labor", "Energy", "Overhead", "Markup", "Platform Fees"],
            "Amount": [
                result.material_cost,
                result.labor_cost,
                result.energy_cost,
                result.overhead_cost,
                result.sale_price - result.cogs,
                -result.platform_fees,
            ],
        }
    )
    st.bar_chart(data, x="Category", y="Amount", horizontal=True)
