#pragma once

#include <QWidget>

#include <functional>

#include "ui_theme.h"

class QLineEdit;
class QPushButton;

class ThemeSettingsPage final : public QWidget {
public:
    ThemeSettingsPage(
        dex_ui::UiTheme theme,
        std::function<void(dex_ui::UiTheme)> onSave,
        std::function<void()> onRevert,
        std::function<void()> onBack,
        std::function<void(dex_ui::UiTheme)> onSaveAndBack,
        QWidget *parent = nullptr);

private:
    void chooseColor(QLineEdit *field);
    void refreshSwatches();
    dex_ui::UiTheme collectTheme() const;
    void save(bool exitAfterSave);

    dex_ui::UiTheme theme_;
    std::function<void(dex_ui::UiTheme)> onSave_;
    std::function<void()> onRevert_;
    std::function<void()> onBack_;
    std::function<void(dex_ui::UiTheme)> onSaveAndBack_;
    QLineEdit *base_ = nullptr;
    QLineEdit *surface_ = nullptr;
    QLineEdit *accent_ = nullptr;
    QPushButton *baseSwatch_ = nullptr;
    QPushButton *surfaceSwatch_ = nullptr;
    QPushButton *accentSwatch_ = nullptr;
};
