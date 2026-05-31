#pragma once

#include <QString>

#include "app_state.h"

class QWidget;

namespace DexTextEditorWorkbench {

QWidget *buildTextEditorWorkbenchPage(
    const CockpitState &state,
    const QString &topTab,
    const QString &detailLens);

} // namespace DexTextEditorWorkbench
