#pragma once

#include <QString>

#include "app_state.h"

class QVBoxLayout;

namespace DexTextEditorWorkbench {

void addTextEditorWorkbenchContext(
    QVBoxLayout *contentLayout,
    const CockpitState &state,
    const QString &topTab,
    const QString &detailLens);

} // namespace DexTextEditorWorkbench
