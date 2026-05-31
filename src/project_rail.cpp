#include "project_rail.h"

#include <QHBoxLayout>
#include <QFrame>
#include <QLabel>
#include <QMouseEvent>
#include <QScrollArea>
#include <QStyle>
#include <QVBoxLayout>

#include "app_state_helpers.h"
#include "project_rail_rows.h"
#include "render_helpers.h"
#include "ui_rules.h"

namespace {

class SettingsRow final : public QFrame {
public:
    explicit SettingsRow(std::function<void()> onSelected, QWidget *parent = nullptr)
        : QFrame(parent), onSelected_(std::move(onSelected)) {
        setObjectName("settingsRow");
        setFixedHeight(32);
        setCursor(Qt::PointingHandCursor);
        auto *settingsLayout = new QHBoxLayout(this);
        settingsLayout->setContentsMargins(12, 4, 10, 4);
        settingsLayout->setSpacing(8);
        auto *icon = new QLabel;
        icon->setFixedSize(14, 14);
        icon->setStyleSheet("background:#fbfcfd; border:1px solid #8d969f; border-radius:3px;");
        settingsLayout->addWidget(icon);
        settingsLayout->addWidget(makeLabel("Settings"));
    }

protected:
    void mousePressEvent(QMouseEvent *event) override {
        if (event->button() == Qt::LeftButton && onSelected_) {
            onSelected_();
            event->accept();
            return;
        }
        QFrame::mousePressEvent(event);
    }

private:
    std::function<void()> onSelected_;
};

QFrame *makeEmptyRailPanel(const QString &title) {
    auto *panel = new QFrame;
    panel->setObjectName("candidateRow");
    panel->setMinimumHeight(54);
    auto *layout = new QVBoxLayout(panel);
    layout->setContentsMargins(10, 7, 10, 7);
    layout->setSpacing(4);
    layout->addWidget(makeLabel(title, "smallLabel"));
    layout->addStretch(1);
    return panel;
}

} // namespace

ProjectRail::ProjectRail(
    std::function<void(QString)> onWorkerSelected,
    std::function<void(QString)> onProjectSelected,
    std::function<void()> onSettingsSelected,
    QWidget *parent)
    : QFrame(parent),
      onWorkerSelected_(std::move(onWorkerSelected)),
      onProjectSelected_(std::move(onProjectSelected)),
      onSettingsSelected_(std::move(onSettingsSelected)) {
    setObjectName("projectRail");
    setFixedWidth(dex_ui::metrics::rail_width);

    auto *outer = new QVBoxLayout(this);
    outer->setContentsMargins(dex_ui::metrics::rail_inset, 12, dex_ui::metrics::rail_inset, 10);
    outer->setSpacing(8);

    auto *scroll = new QScrollArea;
    scroll->setWidgetResizable(true);
    scroll->setFrameShape(QFrame::NoFrame);
    scroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    auto *body = new QWidget;
    listLayout_ = new QVBoxLayout(body);
    listLayout_->setContentsMargins(0, 0, 0, 0);
    listLayout_->setSpacing(8);
    scroll->setWidget(body);
    outer->addWidget(scroll, 1);

    settingsRow_ = new SettingsRow(onSettingsSelected_, this);
    outer->addWidget(settingsRow_);
}

void ProjectRail::setState(
    const CockpitState &state,
    const QString &selectedWorkerId,
    const QString &selectedProjectId,
    bool repoMode,
    bool settingsMode) {
    clearLayout(listLayout_);
    settingsRow_->setProperty("active", settingsMode);
    settingsRow_->style()->unpolish(settingsRow_);
    settingsRow_->style()->polish(settingsRow_);

    const QVector<DexProjects::ProjectRegistryEntry> projects = registryProjectsForState(state);

    if (projects.isEmpty()) {
        listLayout_->addWidget(makeLabel("Projects", "sectionLabel"));
        listLayout_->addWidget(makeEmptyRailPanel("Project slot"));
        if (!repoMode) {
            listLayout_->addWidget(makeLabel("Workers", "sectionLabel"));
            listLayout_->addWidget(makeEmptyRailPanel("Worker slot"));
        }
        listLayout_->addStretch(1);
        return;
    }

    listLayout_->addWidget(makeLabel("Pinned", "sectionLabel"));
    addProjectRows(state, selectedProjectId, true);
    if (repoMode) {
        const auto *selectedProject = DexProjects::findProjectById(projects, selectedProjectId);
        if (selectedProject) {
            listLayout_->addWidget(makeLabel("Workers", "sectionLabel"));
            addWorkerRows(state, selectedWorkerId, selectedProjectId, repoMode);
        }
    } else {
        addWorkerRows(state, selectedWorkerId, selectedProjectId, repoMode);
    }

    const bool hasNonPinnedProject = std::any_of(
        projects.begin(),
        projects.end(),
        [](const DexProjects::ProjectRegistryEntry &project) {
            return !project.pinned;
        });
    if (hasNonPinnedProject) {
        listLayout_->addWidget(makeLabel("All Projects", "sectionLabel"));
        addProjectRows(state, selectedProjectId, false);
    }
    listLayout_->addStretch(1);
}

void ProjectRail::addProjectRows(const CockpitState &state, const QString &selectedProjectId, bool pinned) {
    bool added = false;
    const QVector<DexProjects::ProjectRegistryEntry> projects = registryProjectsForState(state);
    for (const DexProjects::ProjectRegistryEntry &project : projects) {
        if (project.pinned == pinned) {
            added = true;
            listLayout_->addWidget(DexProjectRailRows::makeProjectRow(
                project.projectId,
                DexProjects::displayName(project),
                project.path,
                project.status,
                project.projectId == selectedProjectId,
                onProjectSelected_));
        }
    }
    if (!added) {
        auto *empty = makeLabel(pinned ? "No pinned projects" : "No projects", "mutedLabel");
        empty->setFixedHeight(22);
        listLayout_->addWidget(empty);
    }
}

void ProjectRail::addWorkerRows(const CockpitState &state, const QString &selectedWorkerId, const QString &selectedProjectId, bool repoMode) {
    auto *workerContainer = new QWidget;
    auto *workerLayout = new QVBoxLayout(workerContainer);
    workerLayout->setContentsMargins(12, 2, 0, 12);
    workerLayout->setSpacing(6);
    const QVector<WorkerSummary> workers = state.projectRegistryLoaded
        ? workersForProject(state, selectedProjectId)
        : (repoMode ? workersForProject(state, selectedProjectId) : state.workers);
    if (workers.isEmpty()) {
        auto *empty = makeLabel("No workers assigned", "mutedLabel");
        empty->setFixedHeight(22);
        workerLayout->addWidget(empty);
    }
    for (const WorkerSummary &worker : workers) {
        workerLayout->addWidget(DexProjectRailRows::makeWorkerRow(
            worker.id,
            worker.role,
            worker.displayName,
            worker.status,
            worker.id == selectedWorkerId,
            onWorkerSelected_));
    }
    listLayout_->addWidget(workerContainer);
}
