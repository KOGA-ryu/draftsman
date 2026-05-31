#include "ui_rules.h"

#include "ui_style_sheet_sections.h"

namespace dex_ui {

QString app_qss() {
    const QString qss =
        style_sheet_sections::baseChromeQss()
        + style_sheet_sections::surfaceAndLabelQss()
        + style_sheet_sections::buttonQss()
        + style_sheet_sections::railRowQss()
        + style_sheet_sections::statsQss()
        + style_sheet_sections::tabQss()
        + style_sheet_sections::reportAndBadgeQss();

    return QString(qss)
        .arg(colors::bg_root())
        .arg(colors::text_primary())
        .arg(app_font_size())
        .arg(metrics::divider_width)
        .arg(metrics::border_radius)
        .arg(colors::bg_rail_mid())
        .arg(colors::bg_rail_dark())
        .arg(colors::bg_center())
        .arg(colors::bg_context())
        .arg(colors::bg_rail_light())
        .arg(colors::text_muted())
        .arg(qMax(8, app_font_size() - 2))
        .arg(colors::hover_bg())
        .arg(colors::border_soft())
        .arg(metrics::tab_height)
        .arg(colors::selected_bg())
        .arg(app_font_size() + 2)
        .arg(colors::risk_normal())
        .arg(colors::risk_bulky())
        .arg(colors::risk_split_candidate())
        .arg(colors::risk_generated())
        .arg(colors::risk_inspect_first())
        .arg(app_font_family())
        .arg(colors::toolbar_bg())
        .arg(colors::border_strong())
        .arg(colors::primary_action_bg())
        .arg(colors::primary_action_text())
        .arg(colors::primary_action_hover())
        .arg(colors::tab_hover_bg())
        .arg(colors::risk_text())
        .arg(colors::good_text())
        .arg(code_font_family())
        .arg(code_font_size());
}

} // namespace dex_ui
