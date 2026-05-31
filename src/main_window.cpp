#include "main_window.h"

#include <QSize>

#include <utility>

#include "app_state_helpers.h"
#include "binder_navigation.h"
#include "repo_contract_check_state.h"
#include "repo_diff_scan_state.h"
#include "project_rail.h"
#include "right_context_panel.h"
#include "sheet_stack_body.h"
#include "shell_layout.h"
#include <QToolButton>
#include "ui_rules.h"

namespace {

constexpr int kDesignWidth = dex_ui::metrics::design_width;
constexpr int kDesignHeight = dex_ui::metrics::design_height;
constexpr int kMinWidth = dex_ui::metrics::min_width;
constexpr int kMinHeight = dex_ui::metrics::min_height;

} // namespace

DraftsmanWindow::DraftsmanWindow(QString repoRoot, QString binaryPath, QString projectRegistryPath, QString proofReceiptPath)
    : backend_(std::move(repoRoot), std::move(binaryPath)),
      projectRegistryPath_(resolveProjectRegistryPath(std::move(projectRegistryPath))),
      shellLayoutPath_(resolveShellLayoutPath()),
      uiThemePath_(resolveUiThemePath()),
      featureRegistryPath_(resolveFeatureRegistryPath()),
      binderTemplateDirPath_(resolveBinderTemplateDirPath(projectRegistryPath_)),
      proofReceiptPath_(resolveProofReceiptPath(std::move(proofReceiptPath))),
      promotionReportPath_(resolvePromotionReportPath()) {
    setWindowTitle("Draftsman");
    setMinimumSize(kMinWidth, kMinHeight);
    resize(kDesignWidth, kDesignHeight);
    setUnifiedTitleAndToolBarOnMac(true);

    buildToolbar();
    buildBody();
    reloadState();
}

void DraftsmanWindow::setProjectRailVisible(bool visible) {
    projectRail_->setVisible(visible);
    if (railToggle_) {
        railToggle_->setChecked(visible);
    }
    if (body_) {
        body_->relayoutSheets();
    }
}

void DraftsmanWindow::setRightContextVisible(bool visible) {
    rightContext_->setVisible(visible);
    if (contextToggle_) {
        contextToggle_->setChecked(visible);
    }
    if (body_) {
        body_->relayoutSheets();
    }
}

void DraftsmanWindow::setTopTab(const QString &tabName) {
    const QStringList tabs = repoMode_ ? DraftsmanShell::enabledTabLabels(state_.shellLayout) : topTabsFor(false);
    if (tabs.contains(tabName)) {
        settingsMode_ = false;
        selectedTopTab_ = tabName;
        selectedDetailLens_ = detailLensTabsFor(selectedTopTab_, repoMode_).value(0, "Summary");
        refreshViews();
    }
}

void DraftsmanWindow::setDetailLens(const QString &lensName) {
    const QStringList lenses = detailLensTabsFor(selectedTopTab_, repoMode_);
    if (lenses.contains(lensName)) {
        settingsMode_ = false;
        selectedDetailLens_ = lensName;
        refreshViews();
    }
}

void DraftsmanWindow::setRepoBinderMode(bool enabled) {
    if (repoMode_ == enabled) {
        return;
    }
    repoMode_ = enabled;
    settingsMode_ = false;
    selectedTopTab_ = "Profile";
    selectedDetailLens_ = "Dashboard";
    if (repoModeToggle_) {
        repoModeToggle_->setChecked(repoMode_);
    }
    if (repoMode_) {
        syncSelectedWorkerToSelectedProject();
    }
    refreshViews();
}

void DraftsmanWindow::setSelectedWorker(const QString &workerId) {
    if (workerId.isEmpty()) {
        return;
    }
    settingsMode_ = false;
    selectedWorkerId_ = workerId;
    if (!repoMode_ && !state_.workers.isEmpty() && !hasWorker(state_, selectedWorkerId_)) {
        selectedWorkerId_ = state_.workers.first().id;
    }
    if (repoMode_) {
        syncSelectedWorkerToSelectedProject();
    }
    refreshSelectedWorkerStats();
    refreshViews();
}

void DraftsmanWindow::setSelectedProject(const QString &projectId) {
    if (projectId.isEmpty()) {
        return;
    }
    settingsMode_ = false;
    selectedProjectId_ = projectId;
    state_.selectedProjectId = selectedProjectId_;
    state_.repoDiffScan = DexRepoDiffScan::unavailableScanState("scan pending");
    state_.repoContractCheck = DexRepoContractCheck::unavailableContractState("contract-check pending");
    if (repoMode_) {
        syncSelectedWorkerToSelectedProject();
    }
    refreshSelectedProjectScan();
    refreshViews();
}

void DraftsmanWindow::setSettingsMode(bool enabled) {
    if (!enabled) {
        exitSettingsMode();
        return;
    }
    settingsMode_ = enabled;
    if (settingsMode_) {
        repoMode_ = true;
        if (repoModeToggle_) {
            repoModeToggle_->setChecked(true);
        }
    }
    refreshViews();
}
