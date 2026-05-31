#include "feature_registry.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>

namespace DexFeatures {
namespace {

QString readString(const QJsonObject &object, const char *key, const QString &fallback = QString()) {
    const QJsonValue value = object.value(QString::fromLatin1(key));
    return value.isString() ? value.toString() : fallback;
}

bool readBool(const QJsonObject &object, const char *key, bool fallback = false) {
    const QJsonValue value = object.value(QString::fromLatin1(key));
    return value.isBool() ? value.toBool() : fallback;
}

int readInt(const QJsonObject &object, const char *key, int fallback) {
    const QJsonValue value = object.value(QString::fromLatin1(key));
    return value.isDouble() ? value.toInt() : fallback;
}

QStringList readStringList(const QJsonObject &object, const char *key) {
    QStringList result;
    const QJsonValue value = object.value(QString::fromLatin1(key));
    if (!value.isArray()) {
        return result;
    }
    for (const QJsonValue &item : value.toArray()) {
        if (item.isString() && !item.toString().trimmed().isEmpty()) {
            result.push_back(item.toString().trimmed());
        }
    }
    return result;
}

} // namespace

FeatureRegistry defaultFeatureRegistry() {
    FeatureRegistry registry;
    registry.loaded = true;
    registry.schemaVersion = 1;
    return registry;
}

FeatureRecord parseFeatureRecord(const QJsonObject &object) {
    FeatureRecord record;
    record.featureId = readString(object, "feature_id");
    record.label = readString(object, "label", record.featureId);
    record.status = readString(object, "status", "planned");
    record.rendererType = readString(object, "renderer_type");
    record.enabled = readBool(object, "enabled", false);
    record.targetSurfaces = readStringList(object, "target_surfaces");
    const QJsonValue settings = object.value("settings");
    record.settings = settings.isObject() ? settings.toObject() : QJsonObject();
    return record;
}

FeatureRegistry parseFeatureRegistryDocument(const QJsonDocument &document, const QString &sourcePath) {
    FeatureRegistry registry = defaultFeatureRegistry();
    registry.sourcePath = sourcePath;

    QJsonArray features;
    if (document.isArray()) {
        features = document.array();
    } else if (document.isObject()) {
        const QJsonObject root = document.object();
        registry.schemaVersion = readInt(root, "schema_version", registry.schemaVersion);
        features = root.value("features").toArray();
    }

    for (const QJsonValue &value : features) {
        if (!value.isObject()) {
            continue;
        }
        FeatureRecord record = parseFeatureRecord(value.toObject());
        if (!record.featureId.isEmpty()) {
            registry.features.push_back(record);
        }
    }
    return registry;
}

const FeatureRecord *findFeatureById(const FeatureRegistry &registry, const QString &featureId) {
    for (const FeatureRecord &feature : registry.features) {
        if (feature.featureId == featureId) {
            return &feature;
        }
    }
    return nullptr;
}

} // namespace DexFeatures
