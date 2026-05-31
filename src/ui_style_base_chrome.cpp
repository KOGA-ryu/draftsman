#include "ui_style_sheet_sections.h"

namespace dex_ui::style_sheet_sections {

QString baseChromeQss() {
    return QString(R"(        QMainWindow, QWidget {
            background: %1;
            color: %2;
            font-family: "%23";
            font-size: %3px;
        }
        QToolBar {
            background: %24;
            border: 0;
            border-bottom: %4px solid %25;
            spacing: 8px;
            min-height: 36px;
            max-height: 36px;
        }
        QToolButton {
            background: transparent;
            border: 1px solid transparent;
            border-radius: 7px;
            min-width: 28px;
            min-height: 22px;
            padding: 1px 8px;
        }
        QToolButton:hover, QToolButton:checked {
            background: %13;
            border-color: %25;
        }
        QLabel#chromeTitle {
            background: transparent;
            color: %2;
            font-size: 13px;
            font-weight: 800;
            padding-left: 8px;
            padding-right: 10px;
        }
        QLabel#chromeLocation {
            background: transparent;
            color: %11;
            font-size: 12px;
            font-weight: 650;
            padding-left: 10px;
            border-left: 1px solid %14;
        }
    )");
}

} // namespace dex_ui::style_sheet_sections
