#include "ui_style_sheet_sections.h"

namespace dex_ui::style_sheet_sections {

QString surfaceAndLabelQss() {
    return QString(R"(        #projectRail {
            background: %6;
            border-right: 2px solid %7;
            border-top-left-radius: 0px;
            border-bottom-left-radius: 0px;
            border-top-right-radius: %5px;
            border-bottom-right-radius: %5px;
        }
        #centerLedger {
            background: %8;
            border: 0;
            border-top-left-radius: 0px;
            border-bottom-left-radius: 0px;
            border-top-right-radius: %5px;
            border-bottom-right-radius: %5px;
        }
        #rightContext {
            background: %9;
            border-left: 1px solid %14;
            border-top-left-radius: 0px;
            border-bottom-left-radius: 0px;
            border-top-right-radius: %5px;
            border-bottom-right-radius: %5px;
        }
        QScrollArea {
            background: transparent;
            border: 0;
        }
        QScrollBar:vertical, QScrollBar:horizontal {
            background: %1;
            border: 1px solid %14;
            margin: 0px;
        }
        QScrollBar:vertical {
            width: 14px;
        }
        QScrollBar:horizontal {
            height: 14px;
        }
        QScrollBar::handle:vertical, QScrollBar::handle:horizontal {
            background: %13;
            border: 1px solid %25;
            border-radius: 4px;
            min-height: 20px;
            min-width: 20px;
        }
        QScrollBar::add-line, QScrollBar::sub-line {
            background: %24;
            border: 1px solid %14;
            width: 14px;
            height: 14px;
        }
        QScrollBar::add-page, QScrollBar::sub-page {
            background: %1;
        }
        QTableWidget, QTableView {
            background: %1;
            alternate-background-color: %10;
            color: %2;
            gridline-color: %14;
            selection-background-color: %13;
            selection-color: %2;
        }
        QHeaderView::section {
            background: %24;
            color: %2;
            border: 1px solid %14;
            padding: 2px 4px;
        }
        QLineEdit {
            background: %1;
            color: %2;
            border: 1px solid %14;
            padding: 2px 4px;
        }
        QPlainTextEdit, QTextEdit, QTextBrowser {
            background: %1;
            color: %2;
            border: 1px solid %14;
            font-family: "%32";
            font-size: %33px;
        }
        QLabel#sectionLabel {
            color: %2;
            font-size: 12px;
            font-weight: 700;
            background: %10;
            padding-left: 0px;
        }
        QLabel#mutedLabel {
            color: %11;
            font-size: 12px;
            background: transparent;
        }
        QLabel#smallLabel {
            color: %2;
            font-size: %12px;
            background: transparent;
        }
    )");
}

} // namespace dex_ui::style_sheet_sections
