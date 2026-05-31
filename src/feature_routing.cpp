#include "feature_routing.h"

#include <QJsonArray>
#include <QJsonValue>

namespace DexFeatures {

const FeatureRecord *enabledFeatureByRenderer(const FeatureRegistry &registry, const QString &rendererType) {
    for (const FeatureRecord &feature : registry.features) {
        if (feature.enabled && feature.rendererType == rendererType) {
            return &feature;
        }
    }
    return nullptr;
}

QStringList featureStringListSetting(const FeatureRecord &feature, const QString &key) {
    QStringList values;
    const QJsonValue value = feature.settings.value(key);
    if (!value.isArray()) {
        return values;
    }
    for (const QJsonValue &entry : value.toArray()) {
        if (entry.isString() && !entry.toString().trimmed().isEmpty()) {
            values.push_back(entry.toString().trimmed());
        }
    }
    return values;
}

QString featureStringSetting(const FeatureRecord &feature, const QString &key, const QString &fallback) {
    const QJsonValue value = feature.settings.value(key);
    return value.isString() && !value.toString().trimmed().isEmpty() ? value.toString().trimmed() : fallback;
}

bool featureBoolSetting(const FeatureRecord &feature, const QString &key, bool fallback) {
    const QJsonValue value = feature.settings.value(key);
    return value.isBool() ? value.toBool() : fallback;
}

int featureIntSetting(const FeatureRecord &feature, const QString &key, int fallback) {
    const QJsonValue value = feature.settings.value(key);
    return value.isDouble() ? value.toInt() : fallback;
}

bool featureTargetsTab(const FeatureRecord &feature, const QString &tab) {
    const QStringList tabs = featureStringListSetting(feature, "target_tabs");
    return tabs.contains(tab);
}

} // namespace DexFeatures
