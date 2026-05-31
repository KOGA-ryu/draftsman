#include "main_window.h"

#include <QStatusBar>

#include <algorithm>
#include <utility>

#include "app_state_helpers.h"
#include "binder_state.h"
#include "detail_lens_rail.h"
#include "ledger_view.h"
#include "project_rail.h"
#include "repo_contract_check_state.h"
#include "repo_diff_scan_state.h"
#include "project_registry.h"
#include "right_context_panel.h"
#include "sheet_stack_body.h"
#include "shell_layout.h"
#include "ui_rules.h"

void DraftsmanWindow::reloadState() {
    state_ = backend_.deriveState();
    loadProjectRegistryIntoState();
    loadShellLayoutIntoState();
    loadUiThemeIntoState();
    loadFeatureRegistryIntoState();
    loadBinderTemplatesIntoState();
    loadProofReceiptIntoState();
    loadPromotionReportIntoState();
    if (selectedProjectId_.isEmpty()) {
        selectedProjectId_ = state_.selectedProjectId;
    }
    const QVector<DexProjects::ProjectRegistryEntry> projects = registryProjectsForState(state_);
    if (projects.isEmpty()) {
        selectedProjectId_.clear();
    } else if (!DexProjects::findProjectById(projects, selectedProjectId_)) {
        auto pinned = std::find_if(
            projects.begin(),
            projects.end(),
            [](const DexProjects::ProjectRegistryEntry &project) {
                return project.pinned;
            });
        if (pinned != projects.end()) {
            selectedProjectId_ = pinned->projectId;
        } else if (!projects.isEmpty()) {
            selectedProjectId_ = projects.first().projectId;
        }
    }
    state_.selectedProjectId = selectedProjectId_;
    if (!repoMode_ && (selectedWorkerId_.isEmpty() || !hasWorker(state_, selectedWorkerId_))) {
        selectedWorkerId_ = state_.selectedWorkerId;
    }
    if (!repoMode_ && !hasWorker(state_, selectedWorkerId_) && !state_.workers.isEmpty()) {
        selectedWorkerId_ = state_.workers.first().id;
    }
    if (repoMode_) {
        syncSelectedWorkerToSelectedProject();
    }
    refreshSelectedProjectScan();
    refreshSelectedWorkerStats();
    refreshViews();
}

void DraftsmanWindow::refreshSelectedWorkerStats() {
    if (settingsMode_ || !state_.backendAvailable || selectedWorkerId_.isEmpty()) {
        return;
    }
    QString error;
    const DexBinder::StatsSnapshotView stats = backend_.deriveStatsForWorker(selectedWorkerId_, &error);
    if (!error.isEmpty()) {
        statusBar()->showMessage(error, 5000);
        return;
    }
    state_.binder.statsSnapshot = stats;
}

void DraftsmanWindow::refreshSelectedProjectScan() {
    if (settingsMode_) {
        state_.repoDiffScan = DexRepoDiffScan::unavailableScanState("settings mode");
        state_.repoContractCheck = DexRepoContractCheck::unavailableContractState("settings mode");
        return;
    }
    if (selectedProjectId_.isEmpty()) {
        state_.repoDiffScan = DexRepoDiffScan::unavailableScanState("no project selected");
        state_.repoContractCheck = DexRepoContractCheck::unavailableContractState("no project selected");
        return;
    }
    const QVector<DexProjects::ProjectRegistryEntry> projects = registryProjectsForState(state_);
    if (!DexProjects::findProjectById(projects, selectedProjectId_)) {
        state_.repoDiffScan = DexRepoDiffScan::unavailableScanState("selected project is not registered");
        state_.repoContractCheck = DexRepoContractCheck::unavailableContractState("selected project is not registered");
        return;
    }
    if (!state_.backendAvailable) {
        state_.repoDiffScan = DexRepoDiffScan::unavailableScanState("backend unavailable");
        state_.repoContractCheck = DexRepoContractCheck::unavailableContractState("backend unavailable");
        return;
    }
    QString error;
    state_.repoDiffScan = backend_.runRepoDiffScan(selectedProjectId_, projectRegistryPath_, &error);
    if (!error.isEmpty()) {
        statusBar()->showMessage(error, 5000);
    }
    QString contractError;
    state_.repoContractCheck = backend_.runRepoContractCheck(selectedProjectId_, projectRegistryPath_, &contractError);
    if (!contractError.isEmpty()) {
        statusBar()->showMessage(contractError, 5000);
    }
}

void DraftsmanWindow::syncSelectedWorkerToSelectedProject() {
    const QVector<WorkerSummary> workers = workersForProject(state_, selectedProjectId_);
    if (workers.isEmpty()) {
        selectedWorkerId_.clear();
        return;
    }
    const bool selectedWorkerIsAssigned = std::any_of(
        workers.begin(),
        workers.end(),
        [this](const WorkerSummary &worker) {
            return worker.id == selectedWorkerId_;
        });
    if (!selectedWorkerIsAssigned) {
        selectedWorkerId_ = workers.first().id;
    }
}

void DraftsmanWindow::refreshViews() {
    if (repoMode_ && !settingsMode_) {
        const QStringList tabs = DraftsmanShell::enabledTabLabels(state_.shellLayout);
        if (!tabs.contains(selectedTopTab_)) {
            selectedTopTab_ = tabs.value(0, "Overview");
            selectedDetailLens_ = "Dashboard";
        }
    }
    projectRail_->setState(state_, selectedWorkerId_, selectedProjectId_, repoMode_, settingsMode_);
    detailLensRail_->setVisible(!settingsMode_);
    if (settingsMode_) {
        ledger_->setSettingsState(
            state_,
            selectedProjectId_,
            [this](DexProjects::ProjectRegistry registry, QString projectId) {
                saveProjectRegistryFromSettings(std::move(registry), projectId, false);
            },
            [this]() {
                revertProjectRegistrySettings();
            },
            [this]() {
                exitSettingsMode();
            },
            [this](DexProjects::ProjectRegistry registry, QString projectId) {
                saveProjectRegistryFromSettings(std::move(registry), projectId, true);
            },
            [this](DraftsmanShell::ShellLayout layout) {
                saveShellLayoutFromSettings(std::move(layout), false);
            },
            [this](DraftsmanShell::ShellLayout layout) {
                saveShellLayoutFromSettings(std::move(layout), true);
            },
            [this](dex_ui::UiTheme theme) {
                saveUiThemeFromSettings(std::move(theme), false);
            },
            [this](dex_ui::UiTheme theme) {
                saveUiThemeFromSettings(std::move(theme), true);
            },
            [this](DexFeatures::FeatureRegistry registry) {
                saveFeatureRegistryFromSettings(std::move(registry), false);
            },
            [this](DexFeatures::FeatureRegistry registry) {
                saveFeatureRegistryFromSettings(std::move(registry), true);
            });
        rightContext_->setSettingsState(state_, selectedProjectId_);
    } else {
        detailLensRail_->setTopTab(selectedTopTab_, selectedDetailLens_, repoMode_);
        selectedDetailLens_ = detailLensRail_->currentLens();
        ledger_->setState(state_, selectedWorkerId_, selectedProjectId_, selectedTopTab_, selectedDetailLens_, repoMode_);
        rightContext_->setState(state_, selectedWorkerId_, selectedProjectId_, selectedTopTab_, selectedDetailLens_, repoMode_);
    }
    if (chromeLocationLabel_) {
        QString projectName = selectedProjectId_.isEmpty() ? QString("Blank shell") : selectedProjectId_;
        if (const auto *project = DexProjects::findProjectById(registryProjectsForState(state_), selectedProjectId_)) {
            projectName = DexProjects::displayName(*project);
        }
        const QString modeLabel = settingsMode_
            ? QString("Settings")
            : repoMode_ && selectedProjectId_.isEmpty()
            ? QString("Binder")
            : workerDisplayName(state_, selectedWorkerId_);
        if (settingsMode_) {
            chromeLocationLabel_->setText(QString("%1 / Settings").arg(projectName));
        } else {
            chromeLocationLabel_->setText(QString("%1 / %2 / %3 / %4")
                .arg(projectName, modeLabel, selectedTopTab_, selectedDetailLens_));
        }
    }
    if (appTitleLabel_) {
        appTitleLabel_->setText(state_.shellLayout.appTitle);
    }
    if (bottomShelf_) {
        bottomShelf_->setStyleSheet(QString("background:%1; border-top:1px solid %2;")
            .arg(dex_ui::colors::hover_bg(), dex_ui::colors::border_strong()));
    }
    setWindowTitle(state_.shellLayout.appTitle);
    body_->relayoutSheets();
}
