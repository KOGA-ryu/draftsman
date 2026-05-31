#include "feature_settings_page.h"

#include <QCheckBox>
#include <QHeaderView>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QScrollArea>
#include <QTableWidget>
#include <QVBoxLayout>

#include <utility>

#include "binder_page_helpers.h"

namespace {

QTableWidgetItem *readOnlyItem(const QString &value) {
    auto *item = new QTableWidgetItem(value);
    item->setFlags(item->flags() & ~Qt::ItemIsEditable);
    return item;
}

QWidget *centeredCheckBox(bool checked) {
    auto *host = new QWidget;
    auto *layout = new QHBoxLayout(host);
    layout->setContentsMargins(0, 0, 0, 0);
    auto *check = new QCheckBox;
    check->setChecked(checked);
    check->setProperty("featureEnabledControl", true);
    layout->addStretch(1);
    layout->addWidget(check);
    layout->addStretch(1);
    return host;
}

QString cellText(const QTableWidget *table, int row, int column) {
    const QTableWidgetItem *item = table->item(row, column);
    return item ? item->text().trimmed() : QString();
}

bool enabledAt(const QTableWidget *table, int row) {
    QWidget *host = table->cellWidget(row, 0);
    if (!host) {
        return false;
    }
    QCheckBox *check = host->findChild<QCheckBox *>();
    return check && check->isChecked();
}

} // namespace

FeatureSettingsPage::FeatureSettingsPage(
    DexFeatures::FeatureRegistry registry,
    std::function<void(DexFeatures::FeatureRegistry)> onSave,
    std::function<void()> onRevert,
    std::function<void()> onBack,
    std::function<void(DexFeatures::FeatureRegistry)> onSaveAndBack,
    QWidget *parent)
    : QWidget(parent),
      registry_(std::move(registry)),
      onSave_(std::move(onSave)),
      onRevert_(std::move(onRevert)),
      onBack_(std::move(onBack)),
      onSaveAndBack_(std::move(onSaveAndBack)) {
    auto *outer = new QVBoxLayout(this);
    outer->setContentsMargins(0, 0, 0, 0);
    outer->setSpacing(0);

    auto *scroll = new QScrollArea;
    scroll->setWidgetResizable(true);
    scroll->setFrameShape(QFrame::NoFrame);
    scroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    auto *body = new QWidget;
    auto *layout = new QVBoxLayout(body);
    layout->setContentsMargins(12, 12, 12, 12);
    layout->setSpacing(10);

    auto *title = DexBinderPages::makeStatsSection("feature registry", true);
    auto *titleLayout = static_cast<QVBoxLayout *>(title->layout());
    titleLayout->addWidget(DexBinderPages::makeStatsRow({
        "registry",
        registry_.sourcePath.isEmpty() ? QString("unknown") : registry_.sourcePath,
        "features",
        QString::number(registry_.features.size()),
        "loaded",
        registry_.loaded ? QString("true") : registry_.error,
    }, false, false));
    layout->addWidget(title);

    auto *actions = new QWidget;
    auto *actionsLayout = new QHBoxLayout(actions);
    actionsLayout->setContentsMargins(0, 0, 0, 0);
    auto *back = new QPushButton("Back to Binder");
    auto *saveButton = new QPushButton("Save Features");
    auto *saveAndBack = new QPushButton("Save and Back");
    auto *revert = new QPushButton("Cancel / Revert");
    back->setObjectName("statsContextAction");
    saveButton->setObjectName("primaryAction");
    saveAndBack->setObjectName("primaryAction");
    revert->setObjectName("statsContextAction");
    connect(back, &QPushButton::clicked, this, [this]() { if (onBack_) onBack_(); });
    connect(saveButton, &QPushButton::clicked, this, [this]() { save(false); });
    connect(saveAndBack, &QPushButton::clicked, this, [this]() { save(true); });
    connect(revert, &QPushButton::clicked, this, [this]() { if (onRevert_) onRevert_(); });
    actionsLayout->addWidget(back);
    actionsLayout->addWidget(saveButton);
    actionsLayout->addWidget(saveAndBack);
    actionsLayout->addWidget(revert);
    actionsLayout->addStretch(1);
    layout->addWidget(actions);

    auto *section = DexBinderPages::makeStatsSection("features", true);
    auto *sectionLayout = static_cast<QVBoxLayout *>(section->layout());
    features_ = new QTableWidget(0, 5);
    features_->setHorizontalHeaderLabels({"enabled", "feature id", "label", "status", "renderer"});
    features_->horizontalHeader()->setStretchLastSection(true);
    features_->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    features_->verticalHeader()->hide();
    features_->setMinimumHeight(180);
    for (const DexFeatures::FeatureRecord &feature : registry_.features) {
        const int row = features_->rowCount();
        features_->insertRow(row);
        features_->setCellWidget(row, 0, centeredCheckBox(feature.enabled));
        features_->setItem(row, 1, readOnlyItem(feature.featureId));
        features_->setItem(row, 2, readOnlyItem(feature.label));
        features_->setItem(row, 3, readOnlyItem(feature.status));
        features_->setItem(row, 4, readOnlyItem(feature.rendererType));
    }
    sectionLayout->addWidget(features_);
    layout->addWidget(section);

    layout->addStretch(1);
    scroll->setWidget(body);
    outer->addWidget(scroll, 1);
}

DexFeatures::FeatureRegistry FeatureSettingsPage::collectRegistry() const {
    DexFeatures::FeatureRegistry next = registry_;
    for (int row = 0; row < features_->rowCount(); ++row) {
        const QString featureId = cellText(features_, row, 1);
        for (DexFeatures::FeatureRecord &feature : next.features) {
            if (feature.featureId == featureId) {
                feature.enabled = enabledAt(features_, row);
                break;
            }
        }
    }
    next.loaded = true;
    return next;
}

void FeatureSettingsPage::save(bool exitAfterSave) {
    const DexFeatures::FeatureRegistry next = collectRegistry();
    if (exitAfterSave) {
        if (onSaveAndBack_) {
            onSaveAndBack_(next);
        }
    } else if (onSave_) {
        onSave_(next);
    }
}
