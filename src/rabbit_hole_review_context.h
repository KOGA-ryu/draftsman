#pragma once

#include <QString>

#include "app_state.h"

class QVBoxLayout;

namespace DexRabbitHoleReview {

void addRabbitHoleReviewContext(
    QVBoxLayout *contentLayout,
    const CockpitState &state,
    const QString &topTab,
    const QString &detailLens);

} // namespace DexRabbitHoleReview
