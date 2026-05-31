#pragma once

#include <QWidget>

#include <functional>

#include "project_registry.h"
#include "project_registry_spec_model.h"

class QCheckBox;
class QLineEdit;
class QPlainTextEdit;
class QTableWidget;

class ProjectRegistrySpecPage final : public QWidget {
public:
    ProjectRegistrySpecPage(
        DexProjects::ProjectRegistry registry,
        QString selectedProjectId,
        std::function<void(DexProjects::ProjectRegistry, QString)> onSave,
        std::function<void()> onRevert,
        std::function<void()> onBack,
        std::function<void(DexProjects::ProjectRegistry, QString)> onSaveAndBack,
        QWidget *parent = nullptr);

private:
    QStringList textLines(QPlainTextEdit *edit) const;
    void setTextLines(QPlainTextEdit *edit, const QStringList &values);
    void addWorkerRow(const DexProjects::ProjectRegistrySpecWorkerRow &worker = {});
    void removeSelectedWorkerRows();
    DexProjects::ProjectRegistrySpecDraft collectDraft() const;
    bool validateDraft(const DexProjects::ProjectRegistrySpecDraft &draft) const;
    void saveDraft(bool exitAfterSave);

    DexProjects::ProjectRegistry registry_;
    DexProjects::ProjectRegistrySpecDraft draft_;
    std::function<void(DexProjects::ProjectRegistry, QString)> onSave_;
    std::function<void()> onRevert_;
    std::function<void()> onBack_;
    std::function<void(DexProjects::ProjectRegistry, QString)> onSaveAndBack_;

    QLineEdit *projectId_ = nullptr;
    QLineEdit *name_ = nullptr;
    QLineEdit *path_ = nullptr;
    QLineEdit *role_ = nullptr;
    QLineEdit *status_ = nullptr;
    QLineEdit *authority_ = nullptr;
    QLineEdit *projectType_ = nullptr;
    QLineEdit *binderTemplate_ = nullptr;
    QCheckBox *pinned_ = nullptr;
    QPlainTextEdit *safeEditZones_ = nullptr;
    QPlainTextEdit *protectedZones_ = nullptr;
    QPlainTextEdit *generatedZones_ = nullptr;
    QPlainTextEdit *scratchZones_ = nullptr;
    QPlainTextEdit *sourceDocs_ = nullptr;
    QPlainTextEdit *buildCommands_ = nullptr;
    QPlainTextEdit *testCommands_ = nullptr;
    QPlainTextEdit *proofCommands_ = nullptr;
    QLineEdit *editorCoreStatus_ = nullptr;
    QLineEdit *editorCorePath_ = nullptr;
    QCheckBox *fileInspectSupported_ = nullptr;
    QCheckBox *fileEditSupported_ = nullptr;
    QTableWidget *workers_ = nullptr;
};
