#pragma once

#include <QWidget>

#include <functional>

#include "shell_layout.h"

class QLineEdit;
class QTableWidget;

class ShellLayoutSettingsPage final : public QWidget {
public:
    ShellLayoutSettingsPage(
        DraftsmanShell::ShellLayout layout,
        std::function<void(DraftsmanShell::ShellLayout)> onSave,
        std::function<void()> onRevert,
        std::function<void()> onBack,
        std::function<void(DraftsmanShell::ShellLayout)> onSaveAndBack,
        QWidget *parent = nullptr);

private:
    void addRow(QTableWidget *table, const QStringList &cells);
    void removeSelectedRows(QTableWidget *table);
    DraftsmanShell::ShellLayout collectLayout() const;
    void save(bool exitAfterSave);

    DraftsmanShell::ShellLayout layout_;
    std::function<void(DraftsmanShell::ShellLayout)> onSave_;
    std::function<void()> onRevert_;
    std::function<void()> onBack_;
    std::function<void(DraftsmanShell::ShellLayout)> onSaveAndBack_;
    QLineEdit *appTitle_ = nullptr;
    QTableWidget *railSections_ = nullptr;
    QTableWidget *railItems_ = nullptr;
    QTableWidget *tabs_ = nullptr;
    QTableWidget *panels_ = nullptr;
    QTableWidget *inspectorPanels_ = nullptr;
};
