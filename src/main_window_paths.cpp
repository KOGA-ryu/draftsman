#include "main_window.h"

#include <QCoreApplication>
#include <QDir>
#include <QFileInfo>
#include <QStringList>

QString DraftsmanWindow::resolveProjectRegistryPath(const QString &requestedPath) {
    if (!requestedPath.isEmpty()) {
        return QFileInfo(requestedPath).absoluteFilePath();
    }

    const QStringList starts = {QDir::currentPath(), QCoreApplication::applicationDirPath()};
    for (const QString &start : starts) {
        QDir dir(start);
        for (int depth = 0; depth < 8; ++depth) {
            const QString candidate = dir.filePath("data/projects.json");
            if (QFileInfo::exists(candidate)) {
                return QFileInfo(candidate).absoluteFilePath();
            }
            if (!dir.cdUp()) {
                break;
            }
        }
    }
    return QDir(QCoreApplication::applicationDirPath()).filePath("../data/projects.json");
}

QString DraftsmanWindow::resolveProofReceiptPath(const QString &requestedPath) {
    if (!requestedPath.isEmpty()) {
        return QFileInfo(requestedPath).absoluteFilePath();
    }

    const QStringList starts = {QDir::currentPath(), QCoreApplication::applicationDirPath()};
    for (const QString &start : starts) {
        QDir dir(start);
        for (int depth = 0; depth < 8; ++depth) {
            const QString candidate = dir.filePath("proof_reference/workcopy_current/proof_manifest.json");
            if (QFileInfo::exists(candidate)) {
                return QFileInfo(candidate).absoluteFilePath();
            }
            if (!dir.cdUp()) {
                break;
            }
        }
    }
    return QDir(QCoreApplication::applicationDirPath()).filePath("../proof_reference/workcopy_current/proof_manifest.json");
}

QString DraftsmanWindow::resolveBinderTemplateDirPath(const QString &projectRegistryPath) {
    if (!projectRegistryPath.isEmpty()) {
        return QFileInfo(projectRegistryPath).absoluteDir().filePath("binder_templates");
    }
    return QDir(QCoreApplication::applicationDirPath()).filePath("../data/binder_templates");
}

QString DraftsmanWindow::resolvePromotionReportPath() {
    const QStringList starts = {QDir::currentPath(), QCoreApplication::applicationDirPath()};
    for (const QString &start : starts) {
        QDir dir(start);
        for (int depth = 0; depth < 8; ++depth) {
            const QString candidate = dir.filePath("proof_reference/workcopy_current/promotion_dry_run_report.json");
            if (QFileInfo::exists(candidate)) {
                return QFileInfo(candidate).absoluteFilePath();
            }
            if (!dir.cdUp()) {
                break;
            }
        }
    }
    return QDir(QCoreApplication::applicationDirPath()).filePath("../proof_reference/workcopy_current/promotion_dry_run_report.json");
}
