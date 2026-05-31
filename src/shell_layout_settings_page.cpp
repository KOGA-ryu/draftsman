#include "shell_layout_settings_page.h"

#include <QCheckBox>
#include <QFormLayout>
#include <QFrame>
#include <QHeaderView>
#include <QLineEdit>
#include <QPushButton>
#include <QScrollArea>
#include <QTableWidget>
#include <QVBoxLayout>

#include "binder_page_helpers.h"

#include <algorithm>

namespace {

QTableWidgetItem *item(const QString &value) {
    auto *cell = new QTableWidgetItem(value);
    cell->setFlags(cell->flags() | Qt::ItemIsEditable);
    return cell;
}

QString cellText(const QTableWidget *table, int row, int column) {
    const QTableWidgetItem *cell = table->item(row, column);
    return cell ? cell->text().trimmed() : QString();
}

bool cellBool(const QTableWidget *table, int row, int column, bool fallback = true) {
    const QString value = cellText(table, row, column).toLower();
    if (value == "false" || value == "0" || value == "no") {
        return false;
    }
    if (value == "true" || value == "1" || value == "yes") {
        return true;
    }
    return fallback;
}

int cellInt(const QTableWidget *table, int row, int column, int fallback) {
    bool ok = false;
    const int value = cellText(table, row, column).toInt(&ok);
    return ok ? value : fallback;
}

QTableWidget *makeTable(const QStringList &headers, int minHeight) {
    auto *table = new QTableWidget(0, headers.size());
    table->setHorizontalHeaderLabels(headers);
    table->horizontalHeader()->setStretchLastSection(true);
    table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    table->verticalHeader()->hide();
    table->setMinimumHeight(minHeight);
    return table;
}

} // namespace

ShellLayoutSettingsPage::ShellLayoutSettingsPage(
    DraftsmanShell::ShellLayout layout,
    std::function<void(DraftsmanShell::ShellLayout)> onSave,
    std::function<void()> onRevert,
    std::function<void()> onBack,
    std::function<void(DraftsmanShell::ShellLayout)> onSaveAndBack,
    QWidget *parent)
    : QWidget(parent),
      layout_(std::move(layout)),
      onSave_(std::move(onSave)),
      onRevert_(std::move(onRevert)),
      onBack_(std::move(onBack)),
      onSaveAndBack_(std::move(onSaveAndBack)) {
    auto *outer = new QVBoxLayout(this);
    outer->setContentsMargins(0, 0, 0, 0);
    auto *scroll = new QScrollArea;
    scroll->setWidgetResizable(true);
    scroll->setFrameShape(QFrame::NoFrame);

    auto *body = new QWidget;
    auto *layoutBody = new QVBoxLayout(body);
    layoutBody->setContentsMargins(12, 12, 12, 12);
    layoutBody->setSpacing(10);

    auto *title = DexBinderPages::makeStatsSection("shell layout");
    auto *titleLayout = static_cast<QVBoxLayout *>(title->layout());
    appTitle_ = new QLineEdit(layout_.appTitle);
    titleLayout->addWidget(appTitle_);
    layoutBody->addWidget(title);

    auto *actions = new QWidget;
    auto *actionsLayout = new QHBoxLayout(actions);
    actionsLayout->setContentsMargins(0, 0, 0, 0);
    auto *back = new QPushButton("Back to Binder");
    auto *saveButton = new QPushButton("Save Layout");
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
    layoutBody->addWidget(actions);

    railSections_ = makeTable({"id", "title", "enabled"}, 100);
    for (const DraftsmanShell::RailSection &section : layout_.railSections) {
        addRow(railSections_, {section.id, section.title, section.enabled ? "true" : "false"});
    }
    railItems_ = makeTable({"section id", "item id", "label", "enabled"}, 130);
    for (const DraftsmanShell::RailSection &section : layout_.railSections) {
        for (const DraftsmanShell::ShellItem &railItem : section.items) {
            addRow(railItems_, {section.id, railItem.id, railItem.label, railItem.enabled ? "true" : "false"});
        }
    }
    tabs_ = makeTable({"id", "label", "enabled"}, 130);
    for (const DraftsmanShell::ShellItem &tab : layout_.tabs) {
        addRow(tabs_, {tab.id, tab.label, tab.enabled ? "true" : "false"});
    }
    panels_ = makeTable({"id", "label", "tab", "min height", "subtle", "enabled"}, 150);
    for (const DraftsmanShell::ShellPanel &panel : layout_.panels) {
        addRow(panels_, {panel.id, panel.label, panel.tab, QString::number(panel.minHeight), panel.subtle ? "true" : "false", panel.enabled ? "true" : "false"});
    }
    inspectorPanels_ = makeTable({"id", "label", "tab", "min height", "subtle", "enabled"}, 130);
    for (const DraftsmanShell::ShellPanel &panel : layout_.inspectorPanels) {
        addRow(inspectorPanels_, {panel.id, panel.label, panel.tab, QString::number(panel.minHeight), panel.subtle ? "true" : "false", panel.enabled ? "true" : "false"});
    }

    const QVector<QPair<QString, QTableWidget *>> tables = {
        {"left rail sections", railSections_},
        {"left rail items", railItems_},
        {"tabs", tabs_},
        {"center panels", panels_},
        {"inspector panels", inspectorPanels_},
    };
    for (const auto &entry : tables) {
        auto *section = DexBinderPages::makeStatsSection(entry.first, true);
        auto *sectionLayout = static_cast<QVBoxLayout *>(section->layout());
        sectionLayout->addWidget(entry.second);
        auto *row = new QWidget;
        auto *rowLayout = new QHBoxLayout(row);
        rowLayout->setContentsMargins(0, 0, 0, 0);
        auto *add = new QPushButton("Add row");
        auto *remove = new QPushButton("Remove selected rows");
        add->setObjectName("statsContextAction");
        remove->setObjectName("statsContextAction");
        connect(add, &QPushButton::clicked, this, [this, table = entry.second]() { addRow(table, {}); });
        connect(remove, &QPushButton::clicked, this, [this, table = entry.second]() { removeSelectedRows(table); });
        rowLayout->addWidget(add);
        rowLayout->addWidget(remove);
        rowLayout->addStretch(1);
        sectionLayout->addWidget(row);
        layoutBody->addWidget(section);
    }

    layoutBody->addStretch(1);
    scroll->setWidget(body);
    outer->addWidget(scroll, 1);
}

void ShellLayoutSettingsPage::addRow(QTableWidget *table, const QStringList &cells) {
    const int row = table->rowCount();
    table->insertRow(row);
    for (int column = 0; column < table->columnCount(); ++column) {
        table->setItem(row, column, item(cells.value(column)));
    }
}

void ShellLayoutSettingsPage::removeSelectedRows(QTableWidget *table) {
    QList<int> rows;
    for (const QModelIndex &index : table->selectionModel()->selectedRows()) {
        rows.push_back(index.row());
    }
    std::sort(rows.begin(), rows.end(), std::greater<int>());
    for (int row : rows) {
        table->removeRow(row);
    }
}

DraftsmanShell::ShellLayout ShellLayoutSettingsPage::collectLayout() const {
    DraftsmanShell::ShellLayout next;
    next.sourcePath = layout_.sourcePath;
    next.loaded = true;
    next.appTitle = appTitle_->text().trimmed().isEmpty() ? QString("Draftsman") : appTitle_->text().trimmed();
    for (int row = 0; row < railSections_->rowCount(); ++row) {
        DraftsmanShell::RailSection section;
        section.id = cellText(railSections_, row, 0);
        section.title = cellText(railSections_, row, 1);
        section.enabled = cellBool(railSections_, row, 2, true);
        if (!section.id.isEmpty()) {
            next.railSections.push_back(section);
        }
    }
    for (int row = 0; row < railItems_->rowCount(); ++row) {
        const QString sectionId = cellText(railItems_, row, 0);
        for (DraftsmanShell::RailSection &section : next.railSections) {
            if (section.id == sectionId) {
                section.items.push_back({cellText(railItems_, row, 1), cellText(railItems_, row, 2), cellBool(railItems_, row, 3, true)});
            }
        }
    }
    for (int row = 0; row < tabs_->rowCount(); ++row) {
        next.tabs.push_back({cellText(tabs_, row, 0), cellText(tabs_, row, 1), cellBool(tabs_, row, 2, true)});
    }
    for (int row = 0; row < panels_->rowCount(); ++row) {
        next.panels.push_back({cellText(panels_, row, 0), cellText(panels_, row, 1), cellText(panels_, row, 2), cellInt(panels_, row, 3, 96), cellBool(panels_, row, 4, false), cellBool(panels_, row, 5, true)});
    }
    for (int row = 0; row < inspectorPanels_->rowCount(); ++row) {
        next.inspectorPanels.push_back({cellText(inspectorPanels_, row, 0), cellText(inspectorPanels_, row, 1), cellText(inspectorPanels_, row, 2), cellInt(inspectorPanels_, row, 3, 72), cellBool(inspectorPanels_, row, 4, true), cellBool(inspectorPanels_, row, 5, true)});
    }
    return next;
}

void ShellLayoutSettingsPage::save(bool exitAfterSave) {
    const DraftsmanShell::ShellLayout next = collectLayout();
    if (exitAfterSave) {
        if (onSaveAndBack_) {
            onSaveAndBack_(next);
        }
    } else if (onSave_) {
        onSave_(next);
    }
}
