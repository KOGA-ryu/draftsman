#include "rabbit_hole_review_state.h"

#include <QtGlobal>

#include "feature_routing.h"

namespace DexRabbitHoleReview {

QString rendererType() {
    return "rabbit_hole_review_workspace";
}

RabbitHoleReviewState rabbitHoleReviewState(const DexFeatures::FeatureRecord &feature) {
    RabbitHoleReviewState state;
    state.enabled = feature.enabled;
    state.targetTabs = DexFeatures::featureStringListSetting(feature, "target_tabs");
    state.showLeftSubs = DexFeatures::featureBoolSetting(feature, "show_left_subs", true);
    state.showRightContext = DexFeatures::featureBoolSetting(feature, "show_right_context", true);
    state.fixturePath = DexFeatures::featureStringSetting(feature, "fixture_path", state.fixturePath);
    state.persistReviewState = DexFeatures::featureBoolSetting(feature, "persist_review_state", false);
    state.enableComponentFocus = DexFeatures::featureBoolSetting(feature, "enable_component_focus", true);
    state.enableExports = DexFeatures::featureBoolSetting(feature, "enable_exports", false);
    state.maxCommentChars = qBound(100, DexFeatures::featureIntSetting(feature, "max_comment_chars", 4000), 100000);
    return state;
}

const DexFeatures::FeatureRecord *activeRabbitHoleReviewFeature(const DexFeatures::FeatureRegistry &registry, const QString &tab) {
    const DexFeatures::FeatureRecord *feature =
        DexFeatures::enabledFeatureByRenderer(registry, rendererType());
    if (!feature || !DexFeatures::featureTargetsTab(*feature, tab)) {
        return nullptr;
    }
    return feature;
}

bool rabbitHoleReviewActive(const DexFeatures::FeatureRegistry &registry, const QString &tab) {
    return activeRabbitHoleReviewFeature(registry, tab) != nullptr;
}

bool rabbitHoleReviewLeftSubsActive(const DexFeatures::FeatureRegistry &registry, const QString &tab) {
    const DexFeatures::FeatureRecord *feature = activeRabbitHoleReviewFeature(registry, tab);
    return feature && rabbitHoleReviewState(*feature).showLeftSubs;
}

bool rabbitHoleReviewContextActive(const DexFeatures::FeatureRegistry &registry, const QString &tab) {
    const DexFeatures::FeatureRecord *feature = activeRabbitHoleReviewFeature(registry, tab);
    return feature && rabbitHoleReviewState(*feature).showRightContext;
}

} // namespace DexRabbitHoleReview
