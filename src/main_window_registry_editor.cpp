#include "main_window.h"

#include <QMessageBox>
#include <QToolButton>

#include "project_registry.h"
#include "shell_layout.h"
#include "ui_theme.h"

void DraftsmanWindow::exitSettingsMode() {
    settingsMode_ = false;
    repoMode_ = true;
    if (repoModeToggle_) {
        repoModeToggle_->setChecked(true);
    }
    if (selectedTopTab_.isEmpty()) {
        selectedTopTab_ = "Profile";
    }
    if (selectedDetailLens_.isEmpty()) {
        selectedDetailLens_ = "Dashboard";
    }
    refreshSelectedProjectScan();
    refreshViews();
}

void DraftsmanWindow::saveProjectRegistryFromSettings(DexProjects::ProjectRegistry registry, const QString &selectedProjectId, bool exitAfterSave) {
    if (registry.sourcePath.isEmpty()) {
        registry.sourcePath = projectRegistryPath_;
    }
    QString error;
    if (!DexProjects::saveProjectRegistryFile(registry, &error)) {
        QMessageBox::warning(this, "Project Registry", error);
        return;
    }
    selectedProjectId_ = selectedProjectId;
    settingsMode_ = !exitAfterSave;
    repoMode_ = true;
    reloadState();
}

void DraftsmanWindow::saveShellLayoutFromSettings(DraftsmanShell::ShellLayout layout, bool exitAfterSave) {
    if (layout.sourcePath.isEmpty()) {
        layout.sourcePath = shellLayoutPath_;
    }
    QString error;
    if (!DraftsmanShell::saveShellLayoutFile(layout, &error)) {
        QMessageBox::warning(this, "Shell Layout", error);
        return;
    }
    settingsMode_ = !exitAfterSave;
    repoMode_ = true;
    reloadState();
}

void DraftsmanWindow::saveUiThemeFromSettings(dex_ui::UiTheme theme, bool exitAfterSave) {
    if (theme.sourcePath.isEmpty()) {
        theme.sourcePath = uiThemePath_;
    }
    QString error;
    if (!dex_ui::saveUiThemeFile(theme, &error)) {
        QMessageBox::warning(this, "UI Theme", error);
        return;
    }
    settingsMode_ = !exitAfterSave;
    repoMode_ = true;
    reloadState();
}

void DraftsmanWindow::revertProjectRegistrySettings() {
    settingsMode_ = true;
    reloadState();
}
