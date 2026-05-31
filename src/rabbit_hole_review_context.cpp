#include "rabbit_hole_review_context.h"

#include <QVBoxLayout>

#include "rabbit_hole_review_model.h"
#include "rabbit_hole_review_state.h"
#include "right_context_render_helpers.h"

namespace DexRabbitHoleReview {
namespace {

QString statusLabel(const QString &status) {
    return status == "needs_rework" ? QString("needs rework") : status;
}

} // namespace

void addRabbitHoleReviewContext(
    QVBoxLayout *contentLayout,
    const CockpitState &state,
    const QString &topTab,
    const QString &detailLens) {
    const DexFeatures::FeatureRecord *feature =
        activeRabbitHoleReviewFeature(state.featureRegistry, topTab);
    if (!feature) {
        return;
    }
    const RabbitHoleReviewState settings = rabbitHoleReviewState(*feature);
    const ReviewFixture fixture = loadReviewFixtureFile(settings.fixturePath);
    const QString routeId = defaultRouteId(fixture);
    const ReviewRoute *route = findRouteById(fixture, routeId);
    const QVector<ReviewComponent> components = componentsForRoute(fixture, routeId);
    const QMap<QString, int> counts = statusCounts(fixture);

    DexRightContext::addContextSection(contentLayout, "review route");
    DexRightContext::addWrappedLine(contentLayout, "subject                       " + fixture.subject.label);
    DexRightContext::addWrappedLine(contentLayout, "route                         " + (route ? route->label : routeId));
    DexRightContext::addWrappedLine(contentLayout, "status                        " + (route ? statusLabel(route->status) : QString("pending")));
    DexRightContext::addWrappedLine(contentLayout, "detail lens                   " + detailLens);

    DexRightContext::addContextSection(contentLayout, "component facts");
    if (components.isEmpty()) {
        DexRightContext::addWrappedLine(contentLayout, "selected component            none on default route");
    } else {
        const ReviewComponent component = components.first();
        DexRightContext::addWrappedLine(contentLayout, "selected component            " + component.label);
        DexRightContext::addWrappedLine(contentLayout, "component status              " + statusLabel(component.status));
        DexRightContext::addWrappedLine(contentLayout, "component route               " + component.routeId);
    }

    DexRightContext::addContextSection(contentLayout, "status summary");
    DexRightContext::addWrappedLine(contentLayout, "pending                       " + QString::number(counts.value("pending")));
    DexRightContext::addWrappedLine(contentLayout, "accepted                      " + QString::number(counts.value("accepted")));
    DexRightContext::addWrappedLine(contentLayout, "needs rework                  " + QString::number(counts.value("needs_rework")));
    DexRightContext::addWrappedLine(contentLayout, "rejected                      " + QString::number(counts.value("rejected")));

    DexRightContext::addContextSection(contentLayout, "receipt");
    DexRightContext::addWrappedLine(contentLayout, "fixture                       " + fixture.sourcePath);
    DexRightContext::addWrappedLine(contentLayout, "persistence                   " + QString(settings.persistReviewState ? "enabled" : "disabled"));
    DexRightContext::addWrappedLine(contentLayout, "exports                       " + QString(settings.enableExports ? "enabled" : "disabled"));
    DexRightContext::addWrappedLine(contentLayout, "writes                        none in this renderer pass");
}

} // namespace DexRabbitHoleReview
