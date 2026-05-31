#pragma once

#include <QString>
#include <QStringList>

#include "feature_registry.h"

namespace DexFeatures {

const FeatureRecord *enabledFeatureByRenderer(const FeatureRegistry &registry, const QString &rendererType);
QStringList featureStringListSetting(const FeatureRecord &feature, const QString &key);
QString featureStringSetting(const FeatureRecord &feature, const QString &key, const QString &fallback = QString());
bool featureBoolSetting(const FeatureRecord &feature, const QString &key, bool fallback = false);
int featureIntSetting(const FeatureRecord &feature, const QString &key, int fallback);
bool featureTargetsTab(const FeatureRecord &feature, const QString &tab);

} // namespace DexFeatures
