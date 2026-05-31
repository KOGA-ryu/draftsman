#include "feature_registry.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>

namespace DexFeatures {
namespace {

QJsonArray stringListToJson(const QStringList &values) {
    QJsonArray array;
    for (const QString &value : values) {
        array.push_back(value);
    }
    return array;
}

QJsonObject featureToJson(const FeatureRecord &feature) {
    QJsonObject object;
    object["feature_id"] = feature.featureId;
    object["label"] = feature.label;
    object["status"] = feature.status;
    object["renderer_type"] = feature.rendererType;
    object["enabled"] = feature.enabled;
    object["target_surfaces"] = stringListToJson(feature.targetSurfaces);
    object["settings"] = feature.settings;
    return object;
}

} // namespace

FeatureRegistry loadFeatureRegistryFile(const QString &path) {
    FeatureRegistry registry = defaultFeatureRegistry();
    registry.sourcePath = path;
    if (path.isEmpty()) {
        registry.loaded = false;
        registry.error = "feature registry path is empty";
        return registry;
    }

    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) {
        registry.loaded = false;
        registry.error = "missing feature registry";
        return registry;
    }

    QJsonParseError parseError;
    const QJsonDocument document = QJsonDocument::fromJson(file.readAll(), &parseError);
    registry = parseFeatureRegistryDocument(document, QFileInfo(path).absoluteFilePath());
    if (parseError.error != QJsonParseError::NoError) {
        registry.loaded = false;
        registry.error = "feature registry JSON error: " + parseError.errorString();
    }
    return registry;
}

QJsonObject featureRegistryToJson(const FeatureRegistry &registry) {
    QJsonArray features;
    for (const FeatureRecord &feature : registry.features) {
        if (!feature.featureId.isEmpty()) {
            features.push_back(featureToJson(feature));
        }
    }
    return QJsonObject{
        {"schema_version", registry.schemaVersion <= 0 ? 1 : registry.schemaVersion},
        {"features", features},
    };
}

bool saveFeatureRegistryFile(const FeatureRegistry &registry, QString *error) {
    if (registry.sourcePath.isEmpty()) {
        if (error) {
            *error = "feature registry path is empty";
        }
        return false;
    }
    const QFileInfo info(registry.sourcePath);
    QDir().mkpath(info.absolutePath());
    QFile file(registry.sourcePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        if (error) {
            *error = "feature registry could not be opened for writing: " + file.errorString();
        }
        return false;
    }
    file.write(QJsonDocument(featureRegistryToJson(registry)).toJson(QJsonDocument::Indented));
    return true;
}

} // namespace DexFeatures
