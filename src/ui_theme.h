#pragma once

#include <QString>

class QJsonDocument;
class QJsonObject;

namespace dex_ui {

struct UiTheme {
    QString sourcePath;
    bool loaded = false;
    QString error;
    QString base = "#141719";
    QString surface = "#24282c";
    QString accent = "#7fa8b8";
    QString text = "#e9eee9";
};

UiTheme defaultUiTheme();
UiTheme parseUiThemeDocument(const QJsonDocument &document);
UiTheme loadUiThemeFile(const QString &path);
bool saveUiThemeFile(const UiTheme &theme, QString *error);
QJsonObject uiThemeToJson(const UiTheme &theme);

bool isValidColor(const QString &value);
QString normalizedColor(const QString &value, const QString &fallback);

} // namespace dex_ui
