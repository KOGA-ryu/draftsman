#include "shell_layout.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

namespace DraftsmanShell {
namespace {

QString readString(const QJsonObject &object, const char *key, const QString &fallback = {}) {
    const QJsonValue value = object.value(QString::fromLatin1(key));
    return value.isString() ? value.toString() : fallback;
}

bool readBool(const QJsonObject &object, const char *key, bool fallback = true) {
    const QJsonValue value = object.value(QString::fromLatin1(key));
    return value.isBool() ? value.toBool() : fallback;
}

int readInt(const QJsonObject &object, const char *key, int fallback) {
    const QJsonValue value = object.value(QString::fromLatin1(key));
    return value.isDouble() ? value.toInt() : fallback;
}

QStringList readStringList(const QJsonObject &object, const char *key) {
    QStringList values;
    const QJsonValue value = object.value(QString::fromLatin1(key));
    if (value.isString()) {
        for (QString line : value.toString().split('\n')) {
            line = line.trimmed();
            if (!line.isEmpty()) {
                values.push_back(line);
            }
        }
        return values;
    }
    for (const QJsonValue &entry : value.toArray()) {
        if (entry.isString() && !entry.toString().trimmed().isEmpty()) {
            values.push_back(entry.toString().trimmed());
        }
    }
    return values;
}

QStringList readPanelLines(const QJsonObject &object) {
    QStringList lines = readStringList(object, "lines");
    if (lines.isEmpty()) {
        lines = readStringList(object, "text");
    }
    return lines;
}

ShellItem parseItem(const QJsonObject &object) {
    ShellItem item;
    item.id = readString(object, "id");
    item.label = readString(object, "label", item.id);
    item.enabled = readBool(object, "enabled", true);
    return item;
}

ShellPanel parsePanel(const QJsonObject &object) {
    ShellPanel panel;
    panel.id = readString(object, "id");
    panel.label = readString(object, "label", panel.id);
    panel.tab = readString(object, "tab", "Overview");
    panel.lines = readPanelLines(object);
    panel.minHeight = readInt(object, "min_height", 96);
    panel.subtle = readBool(object, "subtle", false);
    panel.enabled = readBool(object, "enabled", true);
    return panel;
}

QJsonObject itemToJson(const ShellItem &item) {
    return QJsonObject{
        {"id", item.id},
        {"label", item.label},
        {"enabled", item.enabled},
    };
}

QJsonObject panelToJson(const ShellPanel &panel) {
    QJsonArray lines;
    for (const QString &line : panel.lines) {
        lines.push_back(line);
    }
    return QJsonObject{
        {"id", panel.id},
        {"label", panel.label},
        {"tab", panel.tab},
        {"lines", lines},
        {"min_height", panel.minHeight},
        {"subtle", panel.subtle},
        {"enabled", panel.enabled},
    };
}

} // namespace

ShellLayout defaultShellLayout() {
    ShellLayout layout;
    layout.loaded = true;
    layout.appTitle = "Draftsman";
    layout.railSections = {
        {"projects", "Projects", {{"project_slot", "Project slot", true}}, true},
        {"workflow", "Workflow", {{"scratch", "Scratch", true}, {"final", "Final", true}, {"archive", "Archive", false}}, true},
    };
    layout.tabs = {
        {"overview", "Overview", true},
        {"artifacts", "Artifacts", true},
        {"research", "Research", true},
        {"plans", "Plans", true},
        {"reviews", "Reviews", true},
        {"sessions", "Sessions", true},
        {"sources", "Sources", true},
        {"timeline", "Timeline", true},
    };
    layout.panels = {
        {"project_overview", "Project Overview", "Overview", {}, 96, true, true},
        {"pinned_artifacts", "Pinned Artifacts", "Overview", {}, 132, false, true},
        {"review_queue", "Review Queue", "Overview", {}, 112, true, true},
        {"session_notes", "Session Notes", "Overview", {}, 112, false, true},
        {"decisions", "Decisions", "Overview", {}, 88, true, true},
    };
    layout.inspectorPanels = {
        {"inspector", "Inspector", "*", {}, 72, true, true},
        {"selection", "Selection", "*", {}, 72, true, true},
        {"properties", "Properties", "*", {}, 86, true, true},
        {"actions", "Actions", "*", {}, 64, true, true},
    };
    return layout;
}

ShellLayout parseShellLayoutDocument(const QJsonDocument &document) {
    ShellLayout layout = defaultShellLayout();
    const QJsonObject root = document.object();
    layout.appTitle = readString(root, "app_title", layout.appTitle);
    layout.railSections.clear();
    for (const QJsonValue &sectionValue : root.value("left_rail").toObject().value("sections").toArray()) {
        const QJsonObject object = sectionValue.toObject();
        RailSection section;
        section.id = readString(object, "id");
        section.title = readString(object, "title", section.id);
        section.enabled = readBool(object, "enabled", true);
        for (const QJsonValue &itemValue : object.value("items").toArray()) {
            section.items.push_back(parseItem(itemValue.toObject()));
        }
        if (!section.id.isEmpty()) {
            layout.railSections.push_back(section);
        }
    }
    layout.tabs.clear();
    for (const QJsonValue &value : root.value("tabs").toArray()) {
        const ShellItem item = parseItem(value.toObject());
        if (!item.id.isEmpty()) {
            layout.tabs.push_back(item);
        }
    }
    layout.panels.clear();
    for (const QJsonValue &value : root.value("panels").toArray()) {
        const ShellPanel panel = parsePanel(value.toObject());
        if (!panel.id.isEmpty()) {
            layout.panels.push_back(panel);
        }
    }
    layout.inspectorPanels.clear();
    for (const QJsonValue &value : root.value("inspector").toObject().value("panels").toArray()) {
        const ShellPanel panel = parsePanel(value.toObject());
        if (!panel.id.isEmpty()) {
            layout.inspectorPanels.push_back(panel);
        }
    }
    layout.loaded = true;
    return layout;
}

ShellLayout loadShellLayoutFile(const QString &path) {
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) {
        ShellLayout layout = defaultShellLayout();
        layout.sourcePath = path;
        layout.loaded = false;
        layout.error = "missing shell layout";
        return layout;
    }
    QJsonParseError parseError;
    const QJsonDocument document = QJsonDocument::fromJson(file.readAll(), &parseError);
    ShellLayout layout = parseShellLayoutDocument(document);
    layout.sourcePath = path;
    if (parseError.error != QJsonParseError::NoError) {
        layout.loaded = false;
        layout.error = parseError.errorString();
    }
    return layout;
}

QJsonObject shellLayoutToJson(const ShellLayout &layout) {
    QJsonArray sections;
    for (const RailSection &section : layout.railSections) {
        QJsonArray items;
        for (const ShellItem &item : section.items) {
            items.push_back(itemToJson(item));
        }
        sections.push_back(QJsonObject{
            {"id", section.id},
            {"title", section.title},
            {"enabled", section.enabled},
            {"items", items},
        });
    }
    QJsonArray tabs;
    for (const ShellItem &tab : layout.tabs) {
        tabs.push_back(itemToJson(tab));
    }
    QJsonArray panels;
    for (const ShellPanel &panel : layout.panels) {
        panels.push_back(panelToJson(panel));
    }
    QJsonArray inspectorPanels;
    for (const ShellPanel &panel : layout.inspectorPanels) {
        inspectorPanels.push_back(panelToJson(panel));
    }
    return QJsonObject{
        {"app_title", layout.appTitle},
        {"left_rail", QJsonObject{{"sections", sections}}},
        {"tabs", tabs},
        {"panels", panels},
        {"inspector", QJsonObject{{"panels", inspectorPanels}}},
    };
}

bool saveShellLayoutFile(const ShellLayout &layout, QString *error) {
    const QFileInfo info(layout.sourcePath);
    QDir().mkpath(info.absolutePath());
    QFile file(layout.sourcePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        if (error) {
            *error = file.errorString();
        }
        return false;
    }
    file.write(QJsonDocument(shellLayoutToJson(layout)).toJson(QJsonDocument::Indented));
    return true;
}

QStringList enabledTabLabels(const ShellLayout &layout) {
    QStringList tabs;
    for (const ShellItem &tab : layout.tabs) {
        if (tab.enabled && !tab.label.isEmpty()) {
            tabs.push_back(tab.label);
        }
    }
    return tabs.isEmpty() ? QStringList{"Overview"} : tabs;
}

QVector<ShellPanel> enabledPanelsForTab(const ShellLayout &layout, const QString &tab) {
    QVector<ShellPanel> panels;
    for (const ShellPanel &panel : layout.panels) {
        if (panel.enabled && (panel.tab == tab || panel.tab == "*")) {
            panels.push_back(panel);
        }
    }
    return panels;
}

QVector<ShellPanel> enabledInspectorPanels(const ShellLayout &layout, const QString &tab, const QString &lens) {
    Q_UNUSED(lens);
    QVector<ShellPanel> panels;
    for (const ShellPanel &panel : layout.inspectorPanels) {
        if (panel.enabled && (panel.tab == tab || panel.tab == "*")) {
            panels.push_back(panel);
        }
    }
    return panels;
}

} // namespace DraftsmanShell
