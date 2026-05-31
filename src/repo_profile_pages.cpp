#include "repo_binder_pages.h"

#include <QGridLayout>
#include <QHBoxLayout>
#include <QSizePolicy>
#include <QVBoxLayout>
#include <QtGlobal>

#include "app_state_helpers.h"
#include "binder_page_helpers.h"
#include "render_helpers.h"
#include "repo_binder_page_helpers.h"
#include "repo_cleanup_queue_state.h"
#include "shell_layout.h"

namespace DexBinderPages {
namespace {

QString notSet(const QString &value) {
    return value.trimmed().isEmpty() ? QString("not set") : value;
}

void addProjectListRows(QVBoxLayout *layout, const QString &label, const QStringList &values, bool risk = false) {
    const QStringList compact = compactLines(values);
    if (compact.isEmpty()) {
        layout->addWidget(makeStatsRow({label, "not set"}, false, false));
        return;
    }
    for (const QString &value : compact) {
        layout->addWidget(makeStatsRow({label, value}, risk, false));
    }
}

void addRepoProfilePage(
    QVBoxLayout *layout,
    const CockpitState &state,
    const DexProjects::ProjectRegistryEntry &project,
    const QString &lens,
    const DexRepoBinderTemplate::BinderTemplate &binderTemplate,
    const DexRepoDiffScan::RepoDiffScanState &scan,
    const DexRepoContractCheck::RepoContractCheckState &contracts,
    const DexRepoProofReceipt::RepoProofReceiptState &proofReceipt) {
    layout->addWidget(makeStatsText("Purpose: repo identity, operating envelope, authority status, commands, and the model-agent team that may work here."));
    addTemplateSections(layout, binderTemplate, "Profile", lens, project, scan, contracts, proofReceipt);
    if (lens == "Dashboard" || lens == "Identity") {
        auto *section = makeStatsSection("repo identity", true);
        auto *sectionLayout = static_cast<QVBoxLayout *>(section->layout());
        sectionLayout->addWidget(makeStatsRow({
            "repo: " + DexProjects::displayName(project),
            "path: " + notSet(project.path),
            "role: " + notSet(project.role),
            "status: " + notSet(project.status),
        }, false, false));
        sectionLayout->addWidget(makeStatsRow({
            "authority: " + notSet(project.authority),
            "type: " + notSet(project.projectType),
            "registry source: data/projects.json",
            "file inspect: " + QString(project.fileInspectSupported ? "supported" : "reserved"),
        }, project.authority.contains("authority", Qt::CaseInsensitive), false));
        sectionLayout->addWidget(makeStatsRow({
            "binder template: " + (project.binderTemplate.isEmpty() ? QString("repo_default_binder_v1") : project.binderTemplate),
            project.binderTemplate.isEmpty() ? QString("custom binder: no") : QString("custom binder: yes"),
            "template sections",
            binderTemplate.sections.isEmpty() ? QString("none") : QString::number(binderTemplate.sections.size()),
        }, false, false));
        layout->addWidget(section);
    }
    if (lens == "Dashboard" || lens == "Team") {
        auto *section = makeStatsSection("project worker team");
        auto *sectionLayout = static_cast<QVBoxLayout *>(section->layout());
        const QVector<WorkerSummary> workers = workersForProject(state, project.projectId);
        if (workers.isEmpty()) {
            sectionLayout->addWidget(makeStatsText("No workers assigned. Use the left rail to add the worker roles this repo needs."));
        }
        for (const WorkerSummary &worker : workers) {
            sectionLayout->addWidget(makeStatsRow({
                worker.displayName,
                "id: " + worker.id,
                "role: " + worker.role,
                "status: " + worker.status,
            }, false, false));
        }
        layout->addWidget(section);
    }
    if (lens == "Dashboard" || lens == "Commands") {
        auto *section = makeStatsSection("important commands", true);
        auto *sectionLayout = static_cast<QVBoxLayout *>(section->layout());
        addProjectListRows(sectionLayout, "build", project.buildCommands);
        addProjectListRows(sectionLayout, "test", project.testCommands);
        addProjectListRows(sectionLayout, "proof", project.proofCommands);
        addProjectListRows(sectionLayout, "source doc", project.sourceDocs);
        layout->addWidget(section);
    }
    if (lens == "Dashboard" || lens == "Boundaries") {
        auto *section = makeStatsSection("edit boundaries");
        auto *sectionLayout = static_cast<QVBoxLayout *>(section->layout());
        addProjectListRows(sectionLayout, "safe edit", project.safeEditZones);
        addProjectListRows(sectionLayout, "protected", project.protectedZones, true);
        addProjectListRows(sectionLayout, "generated", project.generatedZones, true);
        addProjectListRows(sectionLayout, "scratch", project.scratchZones);
        layout->addWidget(section);
    }
    if (lens == "Dashboard") {
        auto *section = makeStatsSection("ide seam reserved", true);
        auto *sectionLayout = static_cast<QVBoxLayout *>(section->layout());
        sectionLayout->addWidget(makeStatsRow({
            "editor core status: " + notSet(project.editorCoreStatus),
            "inspect: " + QString(project.fileInspectSupported ? "enabled" : "disabled"),
            "edit: " + QString(project.fileEditSupported ? "enabled" : "disabled"),
        }, true, false));
        sectionLayout->addWidget(makeStatsText(project.editorCorePath.isEmpty()
            ? "editor_core_path: not wired in this slice"
            : "editor_core_path: " + project.editorCorePath));
        layout->addWidget(section);
    }
}

void addBlankShellPanel(QVBoxLayout *layout, const QString &title, const QStringList &lines, int minHeight, bool subtle = false) {
    auto *panel = makeStatsSection(title, subtle);
    panel->setMinimumHeight(minHeight);
    auto *panelLayout = static_cast<QVBoxLayout *>(panel->layout());
    for (const QString &line : lines) {
        panelLayout->addWidget(makeStatsText(line));
    }
    layout->addWidget(panel);
}

} // namespace

QWidget *buildRepoProfilePage(
    const CockpitState &state,
    const QString &workerId,
    const DexProjects::ProjectRegistryEntry &project,
    const QString &detailLens,
    const DexRepoBinderTemplate::BinderTemplate &binderTemplate) {
    return buildCompactPage([project,
                             state,
                             workerId,
                             detailLens,
                             binderTemplate,
                             scan = state.repoDiffScan,
                             contracts = state.repoContractCheck,
                             proofReceipt = state.repoProofReceipt](QVBoxLayout *layout) {
        addSelectedWorkerLens(layout, state, workerId, project, "Profile", detailLens);
        addRepoProfilePage(layout, state, project, detailLens, binderTemplate, scan, contracts, proofReceipt);
    });
}

QWidget *buildRepoBlankPage(
    const CockpitState &state,
    const QString &topTab,
    const QString &detailLens) {
    return buildCompactPage([state, topTab, detailLens](QVBoxLayout *layout) {
        const QVector<DraftsmanShell::ShellPanel> panels =
            DraftsmanShell::enabledPanelsForTab(state.shellLayout, topTab);
        if (panels.isEmpty()) {
            addBlankShellPanel(layout, topTab + " / " + detailLens, {}, 96, true);
            return;
        }
        for (const DraftsmanShell::ShellPanel &panel : panels) {
            addBlankShellPanel(layout, panel.label, panel.lines, panel.minHeight, panel.subtle);
        }
    });
}

} // namespace DexBinderPages
