#include "text_editor_workbench_page.h"

#include <QFrame>
#include <QHBoxLayout>
#include <QLabel>
#include <QPlainTextEdit>
#include <QTextCursor>
#include <QVBoxLayout>

#include "binder_page_helpers.h"
#include "render_helpers.h"
#include "text_editor_workbench_state.h"
#include "ui_rules.h"

namespace DexTextEditorWorkbench {
namespace {

QFont editorFont() {
    QFont font(dex_ui::code_font_family());
    font.setPointSize(dex_ui::code_font_size());
    return font;
}

QString cursorFacts(const QPlainTextEdit *editor) {
    const QString text = editor->toPlainText();
    const QTextCursor cursor = editor->textCursor();
    return QString("lines %1 | chars %2 | cursor line %3 col %4")
        .arg(editor->document()->blockCount())
        .arg(text.size())
        .arg(cursor.blockNumber() + 1)
        .arg(cursor.positionInBlock() + 1);
}

QPlainTextEdit *makeReadOnlyShelf(const QString &text, int maxHeight) {
    auto *edit = new QPlainTextEdit;
    edit->setReadOnly(true);
    edit->setPlainText(text);
    edit->setFont(editorFont());
    edit->setMinimumHeight(72);
    edit->setMaximumHeight(maxHeight);
    return edit;
}

} // namespace

QWidget *buildTextEditorWorkbenchPage(
    const CockpitState &state,
    const QString &topTab,
    const QString &detailLens) {
    const DexFeatures::FeatureRecord *feature =
        activeTextEditorFeature(state.featureRegistry, topTab);
    const TextEditorWorkbenchState workbench =
        feature ? textEditorWorkbenchState(*feature) : TextEditorWorkbenchState();

    auto *page = new QWidget;
    auto *layout = new QVBoxLayout(page);
    layout->setContentsMargins(10, 8, 10, 8);
    layout->setSpacing(6);

    auto *header = DexBinderPages::makeStatsSection("text editor workbench", true);
    auto *headerLayout = static_cast<QVBoxLayout *>(header->layout());
    headerLayout->addWidget(DexBinderPages::makeStatsRow({
        "document",
        workbench.documentName,
        "language",
        workbench.language,
        "tab",
        topTab,
        "lens",
        detailLens,
    }, false, false));
    layout->addWidget(header);

    auto *editor = new QPlainTextEdit;
    editor->setObjectName("textEditorWorkbenchEditor");
    editor->setProperty("uiPath", QString("features.text_editor_workbench.center.editor"));
    editor->setProperty("componentState", QString("local_draft"));
    editor->setFont(editorFont());
    editor->setPlaceholderText(workbench.documentName);
    editor->setMinimumHeight(260);
    layout->addWidget(editor, 1);

    auto *status = makeLabel(QString(), "smallLabel");
    status->setProperty("uiPath", QString("features.text_editor_workbench.center.status"));
    auto refreshStatus = [editor, status]() {
        status->setText(cursorFacts(editor));
    };
    QObject::connect(editor, &QPlainTextEdit::textChanged, editor, refreshStatus);
    QObject::connect(editor, &QPlainTextEdit::cursorPositionChanged, editor, refreshStatus);
    refreshStatus();
    layout->addWidget(status);

    auto *shelfRow = new QWidget;
    auto *shelfLayout = new QHBoxLayout(shelfRow);
    shelfLayout->setContentsMargins(0, 0, 0, 0);
    shelfLayout->setSpacing(6);
    auto *output = makeReadOnlyShelf(
        QString("last result: none\npreview limit: %1 chars").arg(workbench.maxPreviewChars),
        112);
    output->setProperty("uiPath", QString("features.text_editor_workbench.center.output"));
    auto *receipt = makeReadOnlyShelf(
        "receipt: local editor only\nrunner: not connected\nclipboard: disabled",
        112);
    receipt->setProperty("uiPath", QString("features.text_editor_workbench.center.receipt"));
    shelfLayout->addWidget(output, 1);
    shelfLayout->addWidget(receipt, 1);
    layout->addWidget(shelfRow);

    return page;
}

} // namespace DexTextEditorWorkbench
