#include "text_editor_workbench_state.h"

#include <QtGlobal>

#include "feature_routing.h"

namespace DexTextEditorWorkbench {

QString rendererType() {
    return "text_editor_workbench";
}

TextEditorWorkbenchState textEditorWorkbenchState(const DexFeatures::FeatureRecord &feature) {
    TextEditorWorkbenchState state;
    state.enabled = feature.enabled;
    state.targetTabs = DexFeatures::featureStringListSetting(feature, "target_tabs");
    state.showRightContext = DexFeatures::featureBoolSetting(feature, "show_right_context", true);
    state.documentName = DexFeatures::featureStringSetting(feature, "default_document_name", "scratch.txt");
    state.language = DexFeatures::featureStringSetting(feature, "default_language", "text");
    state.maxPreviewChars = qBound(100, DexFeatures::featureIntSetting(feature, "max_preview_chars", 12000), 1000000);
    return state;
}

const DexFeatures::FeatureRecord *activeTextEditorFeature(const DexFeatures::FeatureRegistry &registry, const QString &tab) {
    const DexFeatures::FeatureRecord *feature =
        DexFeatures::enabledFeatureByRenderer(registry, rendererType());
    if (!feature || !DexFeatures::featureTargetsTab(*feature, tab)) {
        return nullptr;
    }
    return feature;
}

bool textEditorWorkbenchActive(const DexFeatures::FeatureRegistry &registry, const QString &tab) {
    return activeTextEditorFeature(registry, tab) != nullptr;
}

bool textEditorWorkbenchContextActive(const DexFeatures::FeatureRegistry &registry, const QString &tab) {
    const DexFeatures::FeatureRecord *feature = activeTextEditorFeature(registry, tab);
    return feature && textEditorWorkbenchState(*feature).showRightContext;
}

} // namespace DexTextEditorWorkbench
