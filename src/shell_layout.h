#pragma once

#include <QString>
#include <QStringList>
#include <QVector>

class QJsonDocument;
class QJsonObject;

namespace DraftsmanShell {

struct ShellItem {
    QString id;
    QString label;
    bool enabled = true;
};

struct RailSection {
    QString id;
    QString title;
    QVector<ShellItem> items;
    bool enabled = true;
};

struct ShellPanel {
    QString id;
    QString label;
    QString tab;
    int minHeight = 96;
    bool subtle = false;
    bool enabled = true;
};

struct ShellLayout {
    QString sourcePath;
    bool loaded = false;
    QString error;
    QString appTitle = "Draftsman";
    QVector<RailSection> railSections;
    QVector<ShellItem> tabs;
    QVector<ShellPanel> panels;
    QVector<ShellPanel> inspectorPanels;
};

ShellLayout defaultShellLayout();
ShellLayout parseShellLayoutDocument(const QJsonDocument &document);
ShellLayout loadShellLayoutFile(const QString &path);
bool saveShellLayoutFile(const ShellLayout &layout, QString *error);
QJsonObject shellLayoutToJson(const ShellLayout &layout);

QStringList enabledTabLabels(const ShellLayout &layout);
QVector<ShellPanel> enabledPanelsForTab(const ShellLayout &layout, const QString &tab);
QVector<ShellPanel> enabledInspectorPanels(const ShellLayout &layout, const QString &tab, const QString &lens);

} // namespace DraftsmanShell
