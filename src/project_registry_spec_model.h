#pragma once

#include <QString>
#include <QStringList>
#include <QVector>

#include "project_registry.h"

namespace DexProjects {

struct ProjectRegistrySpecWorkerRow {
    QString workerId;
    QString role;
    QString displayName;
    QString status;
};

struct ProjectRegistrySpecDraft {
    QString originalProjectId;
    ProjectRegistryEntry project;
    QVector<ProjectRegistrySpecWorkerRow> workers;
};

ProjectRegistrySpecDraft projectRegistrySpecDraft(
    const ProjectRegistry &registry,
    const QString &selectedProjectId);

ProjectRegistry applyProjectRegistrySpecDraft(
    const ProjectRegistry &registry,
    const ProjectRegistrySpecDraft &draft);

} // namespace DexProjects
