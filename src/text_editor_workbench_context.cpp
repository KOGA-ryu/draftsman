#include "text_editor_workbench_context.h"

#include <QVBoxLayout>

#include "right_context_render_helpers.h"
#include "text_editor_workbench_state.h"

namespace DexTextEditorWorkbench {

void addTextEditorWorkbenchContext(
    QVBoxLayout *contentLayout,
    const CockpitState &state,
    const QString &topTab,
    const QString &detailLens) {
    const DexFeatures::FeatureRecord *feature =
        activeTextEditorFeature(state.featureRegistry, topTab);
    if (!feature) {
        return;
    }
    const TextEditorWorkbenchState workbench = textEditorWorkbenchState(*feature);

    DexRightContext::addContextSection(contentLayout, "text editor options");
    DexRightContext::addWrappedLine(contentLayout, "document                       " + workbench.documentName);
    DexRightContext::addWrappedLine(contentLayout, "language                       " + workbench.language);
    DexRightContext::addWrappedLine(contentLayout, "preview chars                  " + QString::number(workbench.maxPreviewChars));

    DexRightContext::addContextSection(contentLayout, "workspace state");
    DexRightContext::addWrappedLine(contentLayout, "renderer                       " + rendererType());
    DexRightContext::addWrappedLine(contentLayout, "active tab                     " + topTab);
    DexRightContext::addWrappedLine(contentLayout, "detail lens                    " + detailLens);
    DexRightContext::addWrappedLine(contentLayout, "target tabs                    " + workbench.targetTabs.join(" | "));

    DexRightContext::addContextSection(contentLayout, "last result");
    DexRightContext::addWrappedLine(contentLayout, "status                         idle");
    DexRightContext::addWrappedLine(contentLayout, "runner                         not connected");

    DexRightContext::addContextSection(contentLayout, "receipt");
    DexRightContext::addWrappedLine(contentLayout, "execution                      none");
    DexRightContext::addWrappedLine(contentLayout, "clipboard write                disabled");
    DexRightContext::addWrappedLine(contentLayout, "fixture runner                 inactive");
}

} // namespace DexTextEditorWorkbench
