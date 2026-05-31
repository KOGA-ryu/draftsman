#include "ui_rules.h"

#include <QColor>
#include <QFontDatabase>
#include <QtGlobal>

#include <cmath>

namespace dex_ui {
namespace {

UiTheme g_theme = defaultUiTheme();

QColor colorFrom(const QString &value, const QString &fallback) {
    const QColor color(value);
    return color.isValid() ? color : QColor(fallback);
}

QColor blend(const QColor &a, const QColor &b, double ratio) {
    const double t = qBound(0.0, ratio, 1.0);
    return QColor(
        qRound(a.red() * (1.0 - t) + b.red() * t),
        qRound(a.green() * (1.0 - t) + b.green() * t),
        qRound(a.blue() * (1.0 - t) + b.blue() * t));
}

double channel(double value) {
    value /= 255.0;
    return value <= 0.03928 ? value / 12.92 : std::pow((value + 0.055) / 1.055, 2.4);
}

double luminance(const QColor &color) {
    return 0.2126 * channel(color.red()) + 0.7152 * channel(color.green()) + 0.0722 * channel(color.blue());
}

QColor shiftedHue(QColor color, int degrees) {
    int h = color.hsvHue();
    if (h < 0) {
        h = 0;
    }
    color.setHsv((h + degrees) % 360, qMin(180, qMax(75, color.hsvSaturation())), qMin(210, qMax(80, color.value())));
    return color;
}

QString hex(const QColor &color) {
    return color.name(QColor::HexRgb);
}

QColor base() {
    return colorFrom(g_theme.base, defaultUiTheme().base);
}

QColor surface() {
    return colorFrom(g_theme.surface, defaultUiTheme().surface);
}

QColor accent() {
    return colorFrom(g_theme.accent, defaultUiTheme().accent);
}

QColor textColor() {
    return colorFrom(g_theme.text, defaultUiTheme().text);
}

} // namespace

void set_active_theme(const UiTheme &theme) {
    g_theme = theme;
    g_theme.base = normalizedColor(g_theme.base, defaultUiTheme().base);
    g_theme.surface = normalizedColor(g_theme.surface, defaultUiTheme().surface);
    g_theme.accent = normalizedColor(g_theme.accent, defaultUiTheme().accent);
    g_theme.text = normalizedColor(g_theme.text, defaultUiTheme().text);
}

UiTheme active_theme() {
    return g_theme;
}

QString colors::bg_root() { return hex(base()); }
QString colors::bg_rail_light() { return hex(blend(surface(), base(), 0.18)); }
QString colors::bg_rail_mid() { return hex(blend(base(), surface(), 0.42)); }
QString colors::bg_rail_dark() { return hex(blend(base(), accent(), 0.45)); }
QString colors::bg_center() { return hex(blend(base(), surface(), 0.32)); }
QString colors::bg_context() { return hex(blend(base(), surface(), 0.24)); }
QString colors::bg_section() { return hex(surface()); }
QString colors::border_soft() { return hex(blend(surface(), accent(), 0.35)); }
QString colors::text_primary() { return hex(textColor()); }
QString colors::text_muted() { return hex(blend(textColor(), surface(), 0.38)); }
QString colors::selected_bg() { return hex(blend(surface(), accent(), 0.18)); }
QString colors::hover_bg() { return hex(blend(surface(), accent(), 0.12)); }
QString colors::risk_normal() { return hex(blend(surface(), accent(), 0.20)); }
QString colors::risk_bulky() { return hex(blend(surface(), shiftedHue(accent(), 35), 0.22)); }
QString colors::risk_split_candidate() { return hex(blend(surface(), shiftedHue(accent(), 95), 0.20)); }
QString colors::risk_generated() { return hex(blend(surface(), shiftedHue(accent(), 200), 0.18)); }
QString colors::risk_inspect_first() { return hex(blend(surface(), shiftedHue(accent(), 65), 0.20)); }
QString colors::toolbar_bg() { return hex(blend(base(), surface(), 0.22)); }
QString colors::border_strong() { return hex(blend(surface(), accent(), 0.62)); }
QString colors::primary_action_bg() { return hex(blend(accent(), textColor(), luminance(surface()) < 0.42 ? 0.10 : 0.28)); }
QString colors::primary_action_text() { return hex(textColor()); }
QString colors::primary_action_hover() { return hex(blend(colorFrom(colors::primary_action_bg(), "#222222"), textColor(), 0.16)); }
QString colors::tab_hover_bg() { return QString("rgba(%1, %2, %3, 0.35)").arg(accent().red()).arg(accent().green()).arg(accent().blue()); }
QString colors::risk_text() { return hex(blend(textColor(), shiftedHue(accent(), 95), 0.34)); }
QString colors::good_text() { return hex(blend(textColor(), accent(), 0.30)); }

QString app_font_family() {
    const QStringList installedFamilies = QFontDatabase::families();
    const QStringList preferredFamilies = {
        "Helvetica Neue",
        "Avenir Next",
        "Arial",
        "Helvetica",
    };

    for (const QString &family : preferredFamilies) {
        if (installedFamilies.contains(family)) {
            return family;
        }
    }

    const QString systemFamily = QFontDatabase::systemFont(QFontDatabase::GeneralFont).family();
    if (!systemFamily.isEmpty() && systemFamily != "Sans Serif") {
        return systemFamily;
    }

    return "Helvetica";
}

QFont app_font() {
    QFont font(app_font_family());
    font.setPointSize(metrics::font_size_body);
    return font;
}

QString risk_kind_token(const QString &kind) {
    QString token = kind.toLower().trimmed();
    token.replace(' ', '_');
    token.replace('-', '_');
    return token;
}

} // namespace dex_ui
