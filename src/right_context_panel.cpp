#include "right_context_panel.h"

#include <QLabel>
#include <QPushButton>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QWidget>

#include <utility>

#include "agent_context_renderer.h"
#include "app_state_helpers.h"
#include "project_registry.h"
#include "render_helpers.h"
#include "repo_binder_template.h"
#include "repo_context_renderer.h"
#include "right_context_render_helpers.h"
#include "shell_layout.h"
#include "ui_rules.h"

namespace {

void addBlankContextPanel(QVBoxLayout *layout, const QString &title, const QStringList &lines, int minHeight = 72) {
    auto *panel = new QFrame;
    panel->setObjectName("statsSubtleSection");
    panel->setMinimumHeight(minHeight);
    auto *panelLayout = new QVBoxLayout(panel);
    panelLayout->setContentsMargins(8, 6, 8, 6);
    panelLayout->setSpacing(3);
    panelLayout->addWidget(makeLabel(title, "sectionLabel"));
    for (const QString &line : lines) {
        auto *label = makeLabel(line, "smallLabel");
        label->setWordWrap(true);
        panelLayout->addWidget(label);
    }
    panelLayout->addStretch(1);
    layout->addWidget(panel);
}

} // namespace

RightContextPanel::RightContextPanel(
    std::function<void()> onAddGrade,
    std::function<void(QString)> onReviewVerdict,
    QWidget *parent)
    : QFrame(parent),
      onAddGrade_(std::move(onAddGrade)),
      onReviewVerdict_(std::move(onReviewVerdict)) {
    setObjectName("rightContext");
    setFixedWidth(dex_ui::metrics::right_context_width);

    auto *outer = new QVBoxLayout(this);
    outer->setContentsMargins(10, 12, 10, 10);
    outer->setSpacing(10);
    content_ = new QWidget;
    contentLayout_ = new QVBoxLayout(content_);
    contentLayout_->setContentsMargins(0, 0, 0, 0);
    contentLayout_->setSpacing(9);

    auto *scroll = new QScrollArea;
    scroll->setWidgetResizable(true);
    scroll->setFrameShape(QFrame::NoFrame);
    scroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scroll->setWidget(content_);
    outer->addWidget(scroll, 1);
}

void RightContextPanel::setState(
    const CockpitState &state,
    const QString &selectedWorkerId,
    const QString &selectedProjectId,
    const QString &selectedTopTab,
    const QString &selectedDetailLens,
    bool repoMode) {
    clearLayout(contentLayout_);

    if (repoMode) {
        const QVector<DexProjects::ProjectRegistryEntry> projects = registryProjectsForState(state);
        const DexProjects::ProjectRegistryEntry *selected = DexProjects::findProjectById(projects, selectedProjectId);
        if (!selected) {
            const QVector<DraftsmanShell::ShellPanel> panels =
                DraftsmanShell::enabledInspectorPanels(state.shellLayout, selectedTopTab, selectedDetailLens);
            for (const DraftsmanShell::ShellPanel &panel : panels) {
                addBlankContextPanel(contentLayout_, panel.label, panel.lines, panel.minHeight);
            }
            contentLayout_->addStretch(1);
            return;
        }
        const DexProjects::ProjectRegistryEntry project = *selected;
        const DexRepoBinderTemplate::BinderTemplate binderTemplate =
            DexRepoBinderTemplate::resolveTemplateForProject(state.binderTemplateStore, project.binderTemplate);
        DexRightContext::addRepoContext(
            contentLayout_,
            state,
            selectedWorkerId,
            project,
            binderTemplate,
            state.repoDiffScan,
            state.repoContractCheck,
            state.repoProofReceipt,
            state.repoPromotionReport,
            selectedTopTab,
            selectedDetailLens);
        contentLayout_->addStretch(1);
        return;
    }

    if (selectedTopTab == "Profile" || selectedTopTab == "Stats" || selectedTopTab == "Relationship" || selectedTopTab == "Grade" || selectedTopTab == "Transcript" || selectedTopTab == "Evidence") {
        DexRightContext::addAgentContext(contentLayout_, state, selectedWorkerId, selectedTopTab, selectedDetailLens);
        contentLayout_->addStretch(1);
        return;
    }

    contentLayout_->addWidget(makeLabel(selectedTopTab + " context", "reportTitle"));
    DexRightContext::addWrappedLine(contentLayout_, "selected worker: " + workerDisplayName(state, selectedWorkerId));
    DexRightContext::addWrappedLine(contentLayout_, "active section: " + selectedTopTab);
    DexRightContext::addWrappedLine(contentLayout_, "detail lens: " + selectedDetailLens);

    auto *gradeButton = new QPushButton("Add legacy grade");
    gradeButton->setObjectName("primaryAction");
    gradeButton->setToolTip("Add legacy grade: append a grade_recorded event to worker_ledger.jsonl.");
    connect(gradeButton, &QPushButton::clicked, this, [this]() {
        onAddGrade_();
    });
    contentLayout_->addWidget(gradeButton);
    DexRightContext::addWrappedLine(contentLayout_, "legacy source: worker_ledger.jsonl");
    DexRightContext::addWrappedLine(contentLayout_, "typed grade editor: pending");

    DexRightContext::addAgentContext(contentLayout_, state, selectedWorkerId, selectedTopTab, selectedDetailLens);
    contentLayout_->addStretch(1);
}

void RightContextPanel::setSettingsState(const CockpitState &state, const QString &selectedProjectId) {
    clearLayout(contentLayout_);

    DexRightContext::addContextSection(contentLayout_, "settings");
    DexRightContext::addWrappedLine(contentLayout_, "screen                         shell layout / project registry");
    DexRightContext::addWrappedLine(contentLayout_, "save behavior                  explicit save only");
    DexRightContext::addWrappedLine(contentLayout_, "shell layout                   " + state.shellLayout.sourcePath);
    DexRightContext::addWrappedLine(contentLayout_, "registry                       " + (state.projectRegistrySource.isEmpty() ? QString("unknown") : state.projectRegistrySource));
    DexRightContext::addWrappedLine(contentLayout_, "loaded                         " + QString(state.projectRegistryLoaded ? "yes" : "no"));
    if (!state.projectRegistryError.isEmpty()) {
        DexRightContext::addWrappedLine(contentLayout_, "registry warning               " + state.projectRegistryError);
    }

    DexRightContext::addContextSection(contentLayout_, "selected project");
    const QVector<DexProjects::ProjectRegistryEntry> projects = registryProjectsForState(state);
    if (const auto *project = DexProjects::findProjectById(projects, selectedProjectId)) {
        DexRightContext::addWrappedLine(contentLayout_, "project                        " + DexProjects::displayName(*project));
        DexRightContext::addWrappedLine(contentLayout_, "project id                     " + project->projectId);
        DexRightContext::addWrappedLine(contentLayout_, "path                           " + project->path);
        DexRightContext::addWrappedLine(contentLayout_, "assigned workers               " + QString::number(project->workerIds.size()));
    } else {
        DexRightContext::addWrappedLine(contentLayout_, "project                        new project / none selected");
    }

    DexRightContext::addContextSection(contentLayout_, "boundaries");
    DexRightContext::addWrappedLine(contentLayout_, "writes                         shell_layout.json or projects.json");
    DexRightContext::addWrappedLine(contentLayout_, "jsonl                          not touched");
    DexRightContext::addWrappedLine(contentLayout_, "backend scanner                not run by settings");
    DexRightContext::addWrappedLine(contentLayout_, "worker execution               not allowed");
    DexRightContext::addContextSection(contentLayout_, "navigation");
    DexRightContext::addWrappedLine(contentLayout_, "Back                           returns to repo binder without saving");
    DexRightContext::addWrappedLine(contentLayout_, "Save and Back                  writes active settings file, then returns");
    contentLayout_->addStretch(1);
}
