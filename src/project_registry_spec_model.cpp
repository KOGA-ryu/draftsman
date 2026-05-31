#include "project_registry_spec_model.h"

#include <algorithm>

namespace DexProjects {
namespace {

WorkerRegistryEntry workerById(const QVector<WorkerRegistryEntry> &workers, const QString &workerId) {
    for (const WorkerRegistryEntry &worker : workers) {
        if (worker.workerId == workerId) {
            return worker;
        }
    }
    return {workerId, workerId, workerId, "unknown"};
}

bool projectUsesWorker(const QVector<ProjectRegistryEntry> &projects, const QString &workerId) {
    return std::any_of(projects.begin(), projects.end(), [&workerId](const ProjectRegistryEntry &project) {
        return project.workerIds.contains(workerId);
    });
}

bool draftHasWorker(const QVector<ProjectRegistrySpecWorkerRow> &workers, const QString &workerId) {
    return std::any_of(workers.begin(), workers.end(), [&workerId](const ProjectRegistrySpecWorkerRow &worker) {
        return worker.workerId == workerId;
    });
}

int projectIndex(const QVector<ProjectRegistryEntry> &projects, const QString &projectId) {
    for (int i = 0; i < projects.size(); ++i) {
        if (projects.at(i).projectId == projectId) {
            return i;
        }
    }
    return -1;
}

int workerIndex(const QVector<WorkerRegistryEntry> &workers, const QString &workerId) {
    for (int i = 0; i < workers.size(); ++i) {
        if (workers.at(i).workerId == workerId) {
            return i;
        }
    }
    return -1;
}

} // namespace

ProjectRegistrySpecDraft projectRegistrySpecDraft(
    const ProjectRegistry &registry,
    const QString &selectedProjectId) {
    ProjectRegistrySpecDraft draft;
    draft.originalProjectId = selectedProjectId;
    if (const ProjectRegistryEntry *project = findProjectById(registry.projects, selectedProjectId)) {
        draft.project = *project;
        for (const QString &workerId : project->workerIds) {
            const WorkerRegistryEntry worker = workerById(registry.workers, workerId);
            draft.workers.push_back({worker.workerId, worker.role, worker.displayName, worker.status});
        }
        return draft;
    }

    draft.project.fileInspectSupported = false;
    draft.project.fileEditSupported = false;
    return draft;
}

ProjectRegistry applyProjectRegistrySpecDraft(
    const ProjectRegistry &registry,
    const ProjectRegistrySpecDraft &draft) {
    ProjectRegistry result = registry;
    if (result.sourcePath.isEmpty()) {
        result.sourcePath = registry.sourcePath;
    }

    ProjectRegistryEntry project = draft.project;
    project.workerIds.clear();
    for (const ProjectRegistrySpecWorkerRow &worker : draft.workers) {
        if (!worker.workerId.isEmpty() && !project.workerIds.contains(worker.workerId)) {
            project.workerIds.push_back(worker.workerId);
        }
    }

    const int existingIndex = projectIndex(result.projects, draft.originalProjectId);
    if (existingIndex >= 0) {
        result.projects[existingIndex] = project;
    } else if (!project.projectId.isEmpty()) {
        result.projects.push_back(project);
    }

    QVector<WorkerRegistryEntry> keptWorkers;
    for (const WorkerRegistryEntry &worker : result.workers) {
        if (worker.workerId.isEmpty()) {
            continue;
        }
        if (projectUsesWorker(result.projects, worker.workerId) || draftHasWorker(draft.workers, worker.workerId)) {
            keptWorkers.push_back(worker);
        }
    }
    result.workers = keptWorkers;

    for (const ProjectRegistrySpecWorkerRow &row : draft.workers) {
        if (row.workerId.isEmpty()) {
            continue;
        }
        WorkerRegistryEntry worker{row.workerId, row.role, row.displayName, row.status};
        if (worker.role.isEmpty()) {
            worker.role = row.workerId;
        }
        if (worker.displayName.isEmpty()) {
            worker.displayName = row.workerId;
        }
        if (worker.status.isEmpty()) {
            worker.status = "unknown";
        }

        const int existingWorkerIndex = workerIndex(result.workers, worker.workerId);
        if (existingWorkerIndex >= 0) {
            result.workers[existingWorkerIndex] = worker;
        } else {
            result.workers.push_back(worker);
        }
    }

    result.loaded = true;
    return result;
}

} // namespace DexProjects
