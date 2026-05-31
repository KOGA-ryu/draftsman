#include "ui_theme.h"

#include <QColor>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QJsonDocument>
#include <QJsonObject>

namespace dex_ui {
namespace {

QString readString(const QJsonObject &object, const char *key, const QString &fallback) {
    const QJsonValue value = object.value(QString::fromLatin1(key));
    return value.isString() ? value.toString() : fallback;
}

int readInt(const QJsonObject &object, const char *key, int fallback) {
    const QJsonValue value = object.value(QString::fromLatin1(key));
    return value.isDouble() ? value.toInt() : fallback;
}

QString normalizedMode(QString value, const QString &fallback) {
    value = value.trimmed().toLower();
    return value == "light" || value == "dark" || value == "system" ? value : fallback;
}

} // namespace

bool isValidColor(const QString &value) {
    return QColor(value.trimmed()).isValid();
}

QString normalizedColor(const QString &value, const QString &fallback) {
    const QColor color(value.trimmed());
    if (!color.isValid()) {
        return fallback;
    }
    return color.name(QColor::HexRgb);
}

UiTheme defaultUiTheme() {
    UiTheme theme;
    theme.loaded = true;
    return theme;
}

UiTheme parseUiThemeDocument(const QJsonDocument &document) {
    UiTheme theme = defaultUiTheme();
    const QJsonObject root = document.object();
    theme.themeMode = normalizedMode(readString(root, "theme_mode", theme.themeMode), theme.themeMode);
    theme.base = normalizedColor(readString(root, "base", theme.base), theme.base);
    theme.surface = normalizedColor(readString(root, "surface", theme.surface), theme.surface);
    theme.accent = normalizedColor(readString(root, "accent", theme.accent), theme.accent);
    theme.text = normalizedColor(readString(root, "text", theme.text), theme.text);
    theme.uiFont = readString(root, "ui_font", theme.uiFont).trimmed();
    theme.codeFont = readString(root, "code_font", theme.codeFont).trimmed();
    theme.uiFontSize = qBound(9, readInt(root, "ui_font_size", theme.uiFontSize), 28);
    theme.codeFontSize = qBound(9, readInt(root, "code_font_size", theme.codeFontSize), 28);
    theme.loaded = true;
    return theme;
}

UiTheme loadUiThemeFile(const QString &path) {
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) {
        UiTheme theme = defaultUiTheme();
        theme.sourcePath = path;
        theme.loaded = false;
        theme.error = "missing UI theme";
        return theme;
    }

    QJsonParseError parseError;
    const QJsonDocument document = QJsonDocument::fromJson(file.readAll(), &parseError);
    UiTheme theme = parseUiThemeDocument(document);
    theme.sourcePath = path;
    if (parseError.error != QJsonParseError::NoError) {
        theme.loaded = false;
        theme.error = parseError.errorString();
    }
    return theme;
}

QJsonObject uiThemeToJson(const UiTheme &theme) {
    return QJsonObject{
        {"theme_mode", normalizedMode(theme.themeMode, defaultUiTheme().themeMode)},
        {"base", normalizedColor(theme.base, defaultUiTheme().base)},
        {"surface", normalizedColor(theme.surface, defaultUiTheme().surface)},
        {"accent", normalizedColor(theme.accent, defaultUiTheme().accent)},
        {"text", normalizedColor(theme.text, defaultUiTheme().text)},
        {"ui_font", theme.uiFont.trimmed()},
        {"code_font", theme.codeFont.trimmed()},
        {"ui_font_size", qBound(9, theme.uiFontSize, 28)},
        {"code_font_size", qBound(9, theme.codeFontSize, 28)},
    };
}

bool saveUiThemeFile(const UiTheme &theme, QString *error) {
    const QFileInfo info(theme.sourcePath);
    QDir().mkpath(info.absolutePath());
    QFile file(theme.sourcePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        if (error) {
            *error = file.errorString();
        }
        return false;
    }
    file.write(QJsonDocument(uiThemeToJson(theme)).toJson(QJsonDocument::Indented));
    return true;
}

} // namespace dex_ui
