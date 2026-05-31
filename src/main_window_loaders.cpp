#include "main_window.h"

#include "project_registry.h"
#include "repo_binder_template.h"
#include "repo_proof_receipt_state.h"
#include "repo_promotion_report_state.h"
#include "shell_layout.h"
#include "ui_theme.h"
#include "render_helpers.h"
#include "ui_rules.h"

#include <QApplication>

#include <algorithm>

void DraftsmanWindow::loadProjectRegistryIntoState() {
    const DexProjects::ProjectRegistry registry = DexProjects::loadProjectRegistryFile(projectRegistryPath_);
    state_.projectRegistrySource = registry.sourcePath;
    state_.projectRegistryLoaded = registry.loaded;
    state_.projectRegistryError = registry.error;
    if (state_.projectRegistryLoaded) {
        state_.registryProjects = registry.projects;
        state_.registryWorkers = registry.workers;
        for (const DexProjects::WorkerRegistryEntry &registryWorker : registry.workers) {
            auto found = std::find_if(
                state_.workers.begin(),
                state_.workers.end(),
                [&registryWorker](const WorkerSummary &worker) {
                    return worker.id == registryWorker.workerId;
                });
            const WorkerSummary worker = {
                registryWorker.workerId,
                registryWorker.role,
                registryWorker.displayName,
                registryWorker.status,
            };
            if (found == state_.workers.end()) {
                state_.workers.push_back(worker);
            } else {
                *found = worker;
            }
        }
    } else {
        state_.registryProjects.clear();
        state_.registryWorkers.clear();
    }
}

void DraftsmanWindow::loadBinderTemplatesIntoState() {
    state_.binderTemplateStore = DexRepoBinderTemplate::loadBinderTemplateStore(binderTemplateDirPath_);
}

void DraftsmanWindow::loadShellLayoutIntoState() {
    state_.shellLayout = DraftsmanShell::loadShellLayoutFile(shellLayoutPath_);
}

void DraftsmanWindow::loadUiThemeIntoState() {
    state_.uiTheme = dex_ui::loadUiThemeFile(uiThemePath_);
    dex_ui::set_active_theme(state_.uiTheme);
    qApp->setFont(dex_ui::app_font());
    qApp->setStyleSheet(appStyleSheet());
}

void DraftsmanWindow::loadProofReceiptIntoState() {
    state_.repoProofReceipt = DexRepoProofReceipt::loadRepoProofReceiptFile(proofReceiptPath_);
}

void DraftsmanWindow::loadPromotionReportIntoState() {
    state_.repoPromotionReport = DexRepoPromotionReport::loadRepoPromotionReportFile(promotionReportPath_);
}
