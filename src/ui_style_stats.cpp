#include "ui_style_sheet_sections.h"

namespace dex_ui::style_sheet_sections {

QString statsQss() {
    return QString(R"(        QFrame#statsSection, QFrame#statsSubtleSection {
            background: %16;
            border: 1px solid %14;
            border-top-left-radius: 0px;
            border-bottom-left-radius: 0px;
            border-top-right-radius: 8px;
            border-bottom-right-radius: 8px;
        }
        QFrame#statsSubtleSection {
            background: %13;
        }
        QFrame#statsRow {
            background: %16;
            border: 1px solid %14;
            border-top-left-radius: 0px;
            border-bottom-left-radius: 0px;
            border-top-right-radius: 3px;
            border-bottom-right-radius: 3px;
        }
        QFrame#statsRiskRow {
            background: %16;
            border: 1px solid %25;
            border-top-left-radius: 0px;
            border-bottom-left-radius: 0px;
            border-top-right-radius: 3px;
            border-bottom-right-radius: 3px;
        }
        QFrame#statsMiniCell {
            background: %16;
            border: 1px solid %14;
            border-top-left-radius: 0px;
            border-bottom-left-radius: 0px;
            border-top-right-radius: 3px;
            border-bottom-right-radius: 3px;
        }
        QLabel#statsTinyLabel {
            color: %2;
            font-size: 8px;
            background: transparent;
        }
        QLabel#statsRiskText {
            color: %30;
            font-size: 8px;
            background: transparent;
        }
        QLabel#statsGoodText {
            color: %31;
            font-size: 8px;
            background: transparent;
        }
        QLabel#statsSectionTitle {
            color: %2;
            font-size: 12px;
            font-weight: 800;
            background: transparent;
        }
    )");
}

} // namespace dex_ui::style_sheet_sections
