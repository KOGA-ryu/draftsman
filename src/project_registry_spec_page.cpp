#include "project_registry_spec_page.h"

#include <QCheckBox>
#include <QFormLayout>
#include <QGridLayout>
#include <QHeaderView>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QScrollArea>
#include <QTableWidget>
#include <QVBoxLayout>

#include <utility>

#include "binder_page_helpers.h"
#include "render_helpers.h"

namespace {

QLineEdit *lineEdit(const QString &value = QString()) {
    auto *edit = new QLineEdit;
    edit->setText(value);
    edit->setMinimumHeight(22);
    return edit;
}

QPlainTextEdit *linesEdit(const QStringList &values = {}) {
    auto *edit = new QPlainTextEdit;
    edit->setPlainText(values.join('\n'));
    edit->setMinimumHeight(56);
    return edit;
}

QTableWidgetItem *tableItem(const QString &value) {
    auto *item = new QTableWidgetItem(value);
    item->setFlags(item->flags() | Qt::ItemIsEditable);
    return item;
}

QString cellText(const QTableWidget *table, int row, int column) {
    const QTableWidgetItem *item = table->item(row, column);
    return item ? item->text().trimmed() : QString();
}

void addFormField(QFormLayout *form, const QString &label, QWidget *field) {
    form->addRow(new QLabel(label), field);
}

} // namespace

ProjectRegistrySpecPage::ProjectRegistrySpecPage(
    DexProjects::ProjectRegistry registry,
    QString selectedProjectId,
    std::function<void(DexProjects::ProjectRegistry, QString)> onSave,
    std::function<void()> onRevert,
    std::function<void()> onBack,
    std::function<void(DexProjects::ProjectRegistry, QString)> onSaveAndBack,
    QWidget *parent)
    : QWidget(parent),
      registry_(std::move(registry)),
      draft_(DexProjects::projectRegistrySpecDraft(registry_, selectedProjectId)),
      onSave_(std::move(onSave)),
      onRevert_(std::move(onRevert)),
      onBack_(std::move(onBack)),
      onSaveAndBack_(std::move(onSaveAndBack)) {
    auto *outer = new QVBoxLayout(this);
    outer->setContentsMargins(0, 0, 0, 0);
    outer->setSpacing(0);

    auto *scroll = new QScrollArea;
    scroll->setWidgetResizable(true);
    scroll->setFrameShape(QFrame::NoFrame);
    scroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    auto *body = new QWidget;
    auto *layout = new QVBoxLayout(body);
    layout->setContentsMargins(12, 12, 12, 12);
    layout->setSpacing(10);

    auto *title = DexBinderPages::makeStatsSection("project registry spec sheet", true);
    auto *titleLayout = static_cast<QVBoxLayout *>(title->layout());
    titleLayout->addWidget(DexBinderPages::makeStatsText(
        "Settings edits the selected repo binder spec. Nothing is written until Save Registry is pressed."));
    titleLayout->addWidget(DexBinderPages::makeStatsRow({
        "registry",
        registry_.sourcePath.isEmpty() ? QString("unknown") : registry_.sourcePath,
        "selected project",
        draft_.originalProjectId.isEmpty() ? QString("new project") : draft_.originalProjectId,
        "workers",
        QString::number(draft_.workers.size()),
    }, false, false));
    layout->addWidget(title);

    auto *actions = new QWidget;
    auto *actionsLayout = new QHBoxLayout(actions);
    actionsLayout->setContentsMargins(0, 0, 0, 0);
    auto *back = new QPushButton("Back to Binder");
    back->setObjectName("statsContextAction");
    auto *save = new QPushButton("Save Registry");
    save->setObjectName("primaryAction");
    auto *saveAndBack = new QPushButton("Save and Back");
    saveAndBack->setObjectName("primaryAction");
    auto *revert = new QPushButton("Cancel / Revert");
    revert->setObjectName("statsContextAction");
    connect(back, &QPushButton::clicked, this, [this]() {
        if (onBack_) {
            onBack_();
        }
    });
    connect(save, &QPushButton::clicked, this, [this]() {
        saveDraft(false);
    });
    connect(saveAndBack, &QPushButton::clicked, this, [this]() {
        saveDraft(true);
    });
    connect(revert, &QPushButton::clicked, this, [this]() {
        if (onRevert_) {
            onRevert_();
        }
    });
    actionsLayout->addWidget(back);
    actionsLayout->addWidget(save);
    actionsLayout->addWidget(saveAndBack);
    actionsLayout->addWidget(revert);
    actionsLayout->addStretch(1);
    layout->addWidget(actions);

    auto *identity = DexBinderPages::makeStatsSection("project identity");
    auto *identityForm = new QFormLayout;
    identityForm->setContentsMargins(0, 0, 0, 0);
    identityForm->setSpacing(6);
    projectId_ = lineEdit(draft_.project.projectId);
    name_ = lineEdit(draft_.project.name);
    path_ = lineEdit(draft_.project.path);
    role_ = lineEdit(draft_.project.role);
    status_ = lineEdit(draft_.project.status);
    authority_ = lineEdit(draft_.project.authority);
    projectType_ = lineEdit(draft_.project.projectType);
    binderTemplate_ = lineEdit(draft_.project.binderTemplate);
    pinned_ = new QCheckBox("Pinned in left rail");
    pinned_->setChecked(draft_.project.pinned);
    addFormField(identityForm, "project id", projectId_);
    addFormField(identityForm, "name", name_);
    addFormField(identityForm, "path", path_);
    addFormField(identityForm, "role", role_);
    addFormField(identityForm, "status", status_);
    addFormField(identityForm, "authority", authority_);
    addFormField(identityForm, "project type", projectType_);
    addFormField(identityForm, "binder template", binderTemplate_);
    addFormField(identityForm, "left rail", pinned_);
    static_cast<QVBoxLayout *>(identity->layout())->addLayout(identityForm);
    layout->addWidget(identity);

    auto *workerSection = DexBinderPages::makeStatsSection("worker roster", true);
    auto *workerLayout = static_cast<QVBoxLayout *>(workerSection->layout());
    workerLayout->addWidget(DexBinderPages::makeStatsText(
        "Workers listed here become the stateful left-rail worker lenses for this project."));
    workers_ = new QTableWidget(0, 4);
    workers_->setHorizontalHeaderLabels({"worker id", "role", "display name", "status"});
    workers_->horizontalHeader()->setStretchLastSection(true);
    workers_->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    workers_->verticalHeader()->hide();
    workers_->setMinimumHeight(118);
    for (const DexProjects::ProjectRegistrySpecWorkerRow &worker : draft_.workers) {
        addWorkerRow(worker);
    }
    workerLayout->addWidget(workers_);
    auto *workerActions = new QWidget;
    auto *workerActionsLayout = new QHBoxLayout(workerActions);
    workerActionsLayout->setContentsMargins(0, 0, 0, 0);
    auto *addWorker = new QPushButton("Add worker row");
    addWorker->setObjectName("statsContextAction");
    auto *removeWorker = new QPushButton("Remove selected rows");
    removeWorker->setObjectName("statsContextAction");
    connect(addWorker, &QPushButton::clicked, this, [this]() {
        addWorkerRow();
    });
    connect(removeWorker, &QPushButton::clicked, this, [this]() {
        removeSelectedWorkerRows();
    });
    workerActionsLayout->addWidget(addWorker);
    workerActionsLayout->addWidget(removeWorker);
    workerActionsLayout->addStretch(1);
    workerLayout->addWidget(workerActions);
    layout->addWidget(workerSection);

    auto *boundaries = DexBinderPages::makeStatsSection("boundaries and commands", true);
    auto *boundaryForm = new QFormLayout;
    boundaryForm->setContentsMargins(0, 0, 0, 0);
    boundaryForm->setSpacing(6);
    safeEditZones_ = linesEdit(draft_.project.safeEditZones);
    protectedZones_ = linesEdit(draft_.project.protectedZones);
    generatedZones_ = linesEdit(draft_.project.generatedZones);
    scratchZones_ = linesEdit(draft_.project.scratchZones);
    sourceDocs_ = linesEdit(draft_.project.sourceDocs);
    buildCommands_ = linesEdit(draft_.project.buildCommands);
    testCommands_ = linesEdit(draft_.project.testCommands);
    proofCommands_ = linesEdit(draft_.project.proofCommands);
    const QVector<QPair<QString, QPlainTextEdit *>> textFields = {
        {"safe edit zones", safeEditZones_},
        {"protected zones", protectedZones_},
        {"generated zones", generatedZones_},
        {"scratch zones", scratchZones_},
        {"source docs", sourceDocs_},
        {"build commands", buildCommands_},
        {"test commands", testCommands_},
        {"proof commands", proofCommands_},
    };
    for (const auto &field : textFields) {
        addFormField(boundaryForm, field.first, field.second);
    }
    static_cast<QVBoxLayout *>(boundaries->layout())->addLayout(boundaryForm);
    layout->addWidget(boundaries);

    auto *ide = DexBinderPages::makeStatsSection("ide seam");
    auto *ideForm = new QFormLayout;
    ideForm->setContentsMargins(0, 0, 0, 0);
    ideForm->setSpacing(6);
    editorCoreStatus_ = lineEdit(draft_.project.editorCoreStatus);
    editorCorePath_ = lineEdit(draft_.project.editorCorePath);
    fileInspectSupported_ = new QCheckBox("file inspect supported");
    fileInspectSupported_->setChecked(draft_.project.fileInspectSupported);
    fileEditSupported_ = new QCheckBox("file edit supported");
    fileEditSupported_->setChecked(draft_.project.fileEditSupported);
    addFormField(ideForm, "editor core status", editorCoreStatus_);
    addFormField(ideForm, "editor core path", editorCorePath_);
    addFormField(ideForm, "inspect", fileInspectSupported_);
    addFormField(ideForm, "edit", fileEditSupported_);
    static_cast<QVBoxLayout *>(ide->layout())->addLayout(ideForm);
    layout->addWidget(ide);

    layout->addStretch(1);

    scroll->setWidget(body);
    outer->addWidget(scroll, 1);
}

QStringList ProjectRegistrySpecPage::textLines(QPlainTextEdit *edit) const {
    QStringList values;
    for (QString line : edit->toPlainText().split('\n')) {
        line = line.trimmed();
        if (!line.isEmpty()) {
            values.push_back(line);
        }
    }
    return values;
}

void ProjectRegistrySpecPage::setTextLines(QPlainTextEdit *edit, const QStringList &values) {
    edit->setPlainText(values.join('\n'));
}

void ProjectRegistrySpecPage::addWorkerRow(const DexProjects::ProjectRegistrySpecWorkerRow &worker) {
    const int row = workers_->rowCount();
    workers_->insertRow(row);
    workers_->setItem(row, 0, tableItem(worker.workerId));
    workers_->setItem(row, 1, tableItem(worker.role));
    workers_->setItem(row, 2, tableItem(worker.displayName));
    workers_->setItem(row, 3, tableItem(worker.status));
}

void ProjectRegistrySpecPage::removeSelectedWorkerRows() {
    QList<int> rows;
    for (const QModelIndex &index : workers_->selectionModel()->selectedRows()) {
        rows.push_back(index.row());
    }
    std::sort(rows.begin(), rows.end(), std::greater<int>());
    for (const int row : rows) {
        workers_->removeRow(row);
    }
}

DexProjects::ProjectRegistrySpecDraft ProjectRegistrySpecPage::collectDraft() const {
    DexProjects::ProjectRegistrySpecDraft draft = draft_;
    draft.project.projectId = projectId_->text().trimmed();
    draft.project.name = name_->text().trimmed();
    draft.project.path = path_->text().trimmed();
    draft.project.role = role_->text().trimmed();
    draft.project.status = status_->text().trimmed();
    draft.project.authority = authority_->text().trimmed();
    draft.project.projectType = projectType_->text().trimmed();
    draft.project.binderTemplate = binderTemplate_->text().trimmed();
    draft.project.pinned = pinned_->isChecked();
    draft.project.safeEditZones = textLines(safeEditZones_);
    draft.project.protectedZones = textLines(protectedZones_);
    draft.project.generatedZones = textLines(generatedZones_);
    draft.project.scratchZones = textLines(scratchZones_);
    draft.project.sourceDocs = textLines(sourceDocs_);
    draft.project.buildCommands = textLines(buildCommands_);
    draft.project.testCommands = textLines(testCommands_);
    draft.project.proofCommands = textLines(proofCommands_);
    draft.project.editorCoreStatus = editorCoreStatus_->text().trimmed();
    draft.project.editorCorePath = editorCorePath_->text().trimmed();
    draft.project.fileInspectSupported = fileInspectSupported_->isChecked();
    draft.project.fileEditSupported = fileEditSupported_->isChecked();
    draft.workers.clear();
    for (int row = 0; row < workers_->rowCount(); ++row) {
        DexProjects::ProjectRegistrySpecWorkerRow worker;
        worker.workerId = cellText(workers_, row, 0);
        worker.role = cellText(workers_, row, 1);
        worker.displayName = cellText(workers_, row, 2);
        worker.status = cellText(workers_, row, 3);
        if (!worker.workerId.isEmpty()) {
            draft.workers.push_back(worker);
        }
    }
    return draft;
}

bool ProjectRegistrySpecPage::validateDraft(const DexProjects::ProjectRegistrySpecDraft &draft) const {
    if (draft.project.projectId.isEmpty()) {
        QMessageBox::warning(const_cast<ProjectRegistrySpecPage *>(this), "Project Registry", "Project id is required.");
        return false;
    }
    for (const DexProjects::ProjectRegistryEntry &project : registry_.projects) {
        if (project.projectId == draft.project.projectId && project.projectId != draft.originalProjectId) {
            QMessageBox::warning(const_cast<ProjectRegistrySpecPage *>(this), "Project Registry", "Project id already exists.");
            return false;
        }
    }
    QStringList workerIds;
    for (const DexProjects::ProjectRegistrySpecWorkerRow &worker : draft.workers) {
        if (workerIds.contains(worker.workerId)) {
            QMessageBox::warning(const_cast<ProjectRegistrySpecPage *>(this), "Project Registry", "Duplicate worker id: " + worker.workerId);
            return false;
        }
        workerIds.push_back(worker.workerId);
    }
    return true;
}

void ProjectRegistrySpecPage::saveDraft(bool exitAfterSave) {
    const DexProjects::ProjectRegistrySpecDraft draft = collectDraft();
    if (!validateDraft(draft)) {
        return;
    }
    DexProjects::ProjectRegistry next = DexProjects::applyProjectRegistrySpecDraft(registry_, draft);
    if (exitAfterSave && onSaveAndBack_) {
        onSaveAndBack_(next, draft.project.projectId);
        return;
    }
    if (onSave_) {
        onSave_(next, draft.project.projectId);
    }
}
