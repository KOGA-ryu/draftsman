#pragma once

#include <QString>
#include <QStringList>

#include "feature_registry.h"

namespace DexTextEditorWorkbench {

struct TextEditorWorkbenchState {
    bool enabled = false;
    bool showRightContext = true;
    QString documentName = "scratch.txt";
    QString language = "text";
    int maxPreviewChars = 12000;
    QStringList targetTabs;
};

QString rendererType();
TextEditorWorkbenchState textEditorWorkbenchState(const DexFeatures::FeatureRecord &feature);
const DexFeatures::FeatureRecord *activeTextEditorFeature(const DexFeatures::FeatureRegistry &registry, const QString &tab);
bool textEditorWorkbenchActive(const DexFeatures::FeatureRegistry &registry, const QString &tab);
bool textEditorWorkbenchContextActive(const DexFeatures::FeatureRegistry &registry, const QString &tab);

} // namespace DexTextEditorWorkbench
