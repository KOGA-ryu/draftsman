#pragma once

#include <QJsonObject>
#include <QString>
#include <QStringList>
#include <QVector>

class QJsonDocument;

namespace DexFeatures {

struct FeatureRecord {
    QString featureId;
    QString label;
    QString status;
    QString rendererType;
    bool enabled = false;
    QStringList targetSurfaces;
    QJsonObject settings;
};

struct FeatureRegistry {
    QString sourcePath;
    bool loaded = false;
    QString error;
    int schemaVersion = 1;
    QVector<FeatureRecord> features;
};

FeatureRegistry defaultFeatureRegistry();
FeatureRecord parseFeatureRecord(const QJsonObject &object);
FeatureRegistry parseFeatureRegistryDocument(const QJsonDocument &document, const QString &sourcePath = QString());
FeatureRegistry loadFeatureRegistryFile(const QString &path);
bool saveFeatureRegistryFile(const FeatureRegistry &registry, QString *error = nullptr);
QJsonObject featureRegistryToJson(const FeatureRegistry &registry);
const FeatureRecord *findFeatureById(const FeatureRegistry &registry, const QString &featureId);

} // namespace DexFeatures
