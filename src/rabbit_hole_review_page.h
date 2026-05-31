#pragma once

#include <QString>

#include "app_state.h"

class QWidget;

namespace DexRabbitHoleReview {

QWidget *buildRabbitHoleReviewPage(
    const CockpitState &state,
    const QString &topTab,
    const QString &detailLens);

} // namespace DexRabbitHoleReview
