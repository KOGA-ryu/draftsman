#include "rust_cockpit_backend.h"

#include <QCoreApplication>
#include <QDir>
#include <QFileInfo>
#include <QStringList>

QString RustCockpitBackend::defaultRepoRoot() {
    const QString envRoot = qEnvironmentVariable("DRAFTSMAN_REPO_ROOT");
    if (!envRoot.isEmpty()) {
        return QDir(envRoot).absolutePath();
    }

    const QStringList starts = {QDir::currentPath(), QCoreApplication::applicationDirPath()};
    for (const QString &start : starts) {
        QDir dir(start);
        for (int depth = 0; depth < 8; ++depth) {
            if (QFileInfo::exists(dir.filePath("CMakeLists.txt")) &&
                QFileInfo::exists(dir.filePath("data/projects.json")) &&
                QFileInfo::exists(dir.filePath("src/main.cpp"))) {
                return dir.absolutePath();
            }
            if (!dir.cdUp()) {
                break;
            }
        }
    }
    return QDir::currentPath();
}

QString RustCockpitBackend::defaultBinaryPath(const QString &repoRoot) {
    const QString envBinary = qEnvironmentVariable("DEX_COCKPIT_CORE_BIN");
    if (!envBinary.isEmpty()) {
        return envBinary;
    }

    const QStringList candidates = {
        QDir(repoRoot).filePath("rust/target/debug/dex-cockpit-core"),
        QDir(QCoreApplication::applicationDirPath()).filePath("../../../rust/target/debug/dex-cockpit-core"),
    };
    for (const QString &candidate : candidates) {
        if (QFileInfo::exists(candidate)) {
            return QDir::cleanPath(candidate);
        }
    }
    return QDir(repoRoot).filePath("rust/target/debug/dex-cockpit-core");
}
