"""BOM breakdown table display."""

from __future__ import annotations

import pandas as pd
import streamlit as st

from models.pricing import PricingResult


def render_bom_table(result: PricingResult) -> None:
    st.subheader("Bill of Materials")

    rows = []
    for item in result.line_items:
        rows.append(
            {
                "Component": item.component_name,
                "Qty": item.quantity,
                "Unit": item.unit,
                "$/Unit": f"${item.unit_price:.4f}" if item.unit_price < 1 else f"${item.unit_price:.2f}",
                "Subtotal": f"${item.subtotal:.2f}",
                "Supplier": item.supplier_name,
                "Note": item.note,
            }
        )

    df = pd.DataFrame(rows)
    st.dataframe(df, use_container_width=True, hide_index=True)
