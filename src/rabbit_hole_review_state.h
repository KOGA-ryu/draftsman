#pragma once

#include <QString>
#include <QStringList>

#include "feature_registry.h"

namespace DexRabbitHoleReview {

struct RabbitHoleReviewState {
    bool enabled = false;
    bool showLeftSubs = true;
    bool showRightContext = true;
    bool persistReviewState = false;
    bool enableComponentFocus = true;
    bool enableExports = false;
    QString fixturePath = "docs/examples/rabbit_hole_review_fixture.json";
    int maxCommentChars = 4000;
    QStringList targetTabs;
};

QString rendererType();
RabbitHoleReviewState rabbitHoleReviewState(const DexFeatures::FeatureRecord &feature);
const DexFeatures::FeatureRecord *activeRabbitHoleReviewFeature(const DexFeatures::FeatureRegistry &registry, const QString &tab);
bool rabbitHoleReviewActive(const DexFeatures::FeatureRegistry &registry, const QString &tab);
bool rabbitHoleReviewLeftSubsActive(const DexFeatures::FeatureRegistry &registry, const QString &tab);
bool rabbitHoleReviewContextActive(const DexFeatures::FeatureRegistry &registry, const QString &tab);

} // namespace DexRabbitHoleReview
