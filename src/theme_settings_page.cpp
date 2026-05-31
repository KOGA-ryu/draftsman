#include "theme_settings_page.h"

#include <QColorDialog>
#include <QComboBox>
#include <QFormLayout>
#include <QFrame>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QSpinBox>
#include <QVBoxLayout>

#include "binder_page_helpers.h"
#include "ui_rules.h"

#include <utility>

namespace {

QPushButton *makeSwatch(const QString &tooltip) {
    auto *button = new QPushButton;
    button->setFixedSize(42, 22);
    button->setToolTip(tooltip);
    button->setObjectName("statsContextAction");
    return button;
}

QSpinBox *makeFontSizeSpinBox(int value) {
    auto *spin = new QSpinBox;
    spin->setRange(9, 28);
    spin->setValue(value);
    spin->setSuffix(" px");
    return spin;
}

void setSwatch(QPushButton *button, const QString &color) {
    const QString normalized = dex_ui::normalizedColor(color, "#222222");
    button->setStyleSheet(QString("background:%1; border:1px solid %2; border-radius:4px;")
        .arg(normalized, dex_ui::colors::border_strong()));
}

QWidget *makeColorRow(QLineEdit *field, QPushButton *swatch) {
    auto *row = new QWidget;
    auto *layout = new QHBoxLayout(row);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(8);
    layout->addWidget(field, 1);
    layout->addWidget(swatch);
    return row;
}

} // namespace

ThemeSettingsPage::ThemeSettingsPage(
    dex_ui::UiTheme theme,
    std::function<void(dex_ui::UiTheme)> onSave,
    std::function<void()> onRevert,
    std::function<void()> onBack,
    std::function<void(dex_ui::UiTheme)> onSaveAndBack,
    QWidget *parent)
    : QWidget(parent),
      theme_(std::move(theme)),
      onSave_(std::move(onSave)),
      onRevert_(std::move(onRevert)),
      onBack_(std::move(onBack)),
      onSaveAndBack_(std::move(onSaveAndBack)) {
    auto *outer = new QVBoxLayout(this);
    outer->setContentsMargins(12, 12, 12, 12);
    outer->setSpacing(10);

    auto *section = DexBinderPages::makeStatsSection("ui theme");
    auto *sectionLayout = static_cast<QVBoxLayout *>(section->layout());

    themeMode_ = new QComboBox;
    themeMode_->addItem("Dark", "dark");
    themeMode_->addItem("Light", "light");
    themeMode_->addItem("System", "system");
    const int selectedMode = themeMode_->findData(theme_.themeMode);
    themeMode_->setCurrentIndex(selectedMode >= 0 ? selectedMode : 0);
    base_ = new QLineEdit(dex_ui::normalizedColor(theme_.base, dex_ui::defaultUiTheme().base));
    surface_ = new QLineEdit(dex_ui::normalizedColor(theme_.surface, dex_ui::defaultUiTheme().surface));
    accent_ = new QLineEdit(dex_ui::normalizedColor(theme_.accent, dex_ui::defaultUiTheme().accent));
    text_ = new QLineEdit(dex_ui::normalizedColor(theme_.text, dex_ui::defaultUiTheme().text));
    uiFont_ = new QLineEdit(theme_.uiFont);
    codeFont_ = new QLineEdit(theme_.codeFont);
    uiFontSize_ = makeFontSizeSpinBox(theme_.uiFontSize);
    codeFontSize_ = makeFontSizeSpinBox(theme_.codeFontSize);
    baseSwatch_ = makeSwatch("Choose base/background color");
    surfaceSwatch_ = makeSwatch("Choose surface/panel color");
    accentSwatch_ = makeSwatch("Choose accent/selection color");
    textSwatch_ = makeSwatch("Choose font/text color");

    auto *form = new QFormLayout;
    form->setContentsMargins(0, 4, 0, 0);
    form->setSpacing(8);
    form->addRow("Mode", themeMode_);
    form->addRow("Background", makeColorRow(base_, baseSwatch_));
    form->addRow("Surface", makeColorRow(surface_, surfaceSwatch_));
    form->addRow("Accent", makeColorRow(accent_, accentSwatch_));
    form->addRow("Text / foreground", makeColorRow(text_, textSwatch_));
    form->addRow("UI font", uiFont_);
    form->addRow("Code font", codeFont_);
    form->addRow("UI font size", uiFontSize_);
    form->addRow("Code font size", codeFontSize_);
    sectionLayout->addLayout(form);
    outer->addWidget(section);

    auto *actions = new QWidget;
    auto *actionsLayout = new QHBoxLayout(actions);
    actionsLayout->setContentsMargins(0, 0, 0, 0);
    auto *back = new QPushButton("Back to Binder");
    auto *saveButton = new QPushButton("Save Theme");
    auto *saveAndBack = new QPushButton("Save and Back");
    auto *revert = new QPushButton("Cancel / Revert");
    back->setObjectName("statsContextAction");
    saveButton->setObjectName("primaryAction");
    saveAndBack->setObjectName("primaryAction");
    revert->setObjectName("statsContextAction");
    connect(back, &QPushButton::clicked, this, [this]() { if (onBack_) onBack_(); });
    connect(saveButton, &QPushButton::clicked, this, [this]() { save(false); });
    connect(saveAndBack, &QPushButton::clicked, this, [this]() { save(true); });
    connect(revert, &QPushButton::clicked, this, [this]() { if (onRevert_) onRevert_(); });
    actionsLayout->addWidget(back);
    actionsLayout->addWidget(saveButton);
    actionsLayout->addWidget(saveAndBack);
    actionsLayout->addWidget(revert);
    actionsLayout->addStretch(1);
    outer->addWidget(actions);
    outer->addStretch(1);

    connect(baseSwatch_, &QPushButton::clicked, this, [this]() { chooseColor(base_); });
    connect(surfaceSwatch_, &QPushButton::clicked, this, [this]() { chooseColor(surface_); });
    connect(accentSwatch_, &QPushButton::clicked, this, [this]() { chooseColor(accent_); });
    connect(textSwatch_, &QPushButton::clicked, this, [this]() { chooseColor(text_); });
    connect(base_, &QLineEdit::textChanged, this, [this]() { refreshSwatches(); });
    connect(surface_, &QLineEdit::textChanged, this, [this]() { refreshSwatches(); });
    connect(accent_, &QLineEdit::textChanged, this, [this]() { refreshSwatches(); });
    connect(text_, &QLineEdit::textChanged, this, [this]() { refreshSwatches(); });
    refreshSwatches();
}

void ThemeSettingsPage::chooseColor(QLineEdit *field) {
    const QColor current(dex_ui::normalizedColor(field->text(), "#222222"));
    const QColor color = QColorDialog::getColor(current, this, "Choose theme color");
    if (color.isValid()) {
        field->setText(color.name(QColor::HexRgb));
    }
}

void ThemeSettingsPage::refreshSwatches() {
    setSwatch(baseSwatch_, base_->text());
    setSwatch(surfaceSwatch_, surface_->text());
    setSwatch(accentSwatch_, accent_->text());
    setSwatch(textSwatch_, text_->text());
}

dex_ui::UiTheme ThemeSettingsPage::collectTheme() const {
    dex_ui::UiTheme next;
    next.sourcePath = theme_.sourcePath;
    next.loaded = true;
    next.themeMode = themeMode_->currentData().toString();
    next.base = dex_ui::normalizedColor(base_->text(), dex_ui::defaultUiTheme().base);
    next.surface = dex_ui::normalizedColor(surface_->text(), dex_ui::defaultUiTheme().surface);
    next.accent = dex_ui::normalizedColor(accent_->text(), dex_ui::defaultUiTheme().accent);
    next.text = dex_ui::normalizedColor(text_->text(), dex_ui::defaultUiTheme().text);
    next.uiFont = uiFont_->text().trimmed();
    next.codeFont = codeFont_->text().trimmed();
    next.uiFontSize = uiFontSize_->value();
    next.codeFontSize = codeFontSize_->value();
    return next;
}

void ThemeSettingsPage::save(bool exitAfterSave) {
    const dex_ui::UiTheme next = collectTheme();
    if (exitAfterSave) {
        if (onSaveAndBack_) {
            onSaveAndBack_(next);
        }
    } else if (onSave_) {
        onSave_(next);
    }
}
