#pragma once

#include <QWidget>

#include <functional>

#include "feature_registry.h"

class QTableWidget;

class FeatureSettingsPage final : public QWidget {
public:
    FeatureSettingsPage(
        DexFeatures::FeatureRegistry registry,
        std::function<void(DexFeatures::FeatureRegistry)> onSave,
        std::function<void()> onRevert,
        std::function<void()> onBack,
        std::function<void(DexFeatures::FeatureRegistry)> onSaveAndBack,
        QWidget *parent = nullptr);

private:
    DexFeatures::FeatureRegistry collectRegistry() const;
    void save(bool exitAfterSave);

    DexFeatures::FeatureRegistry registry_;
    std::function<void(DexFeatures::FeatureRegistry)> onSave_;
    std::function<void()> onRevert_;
    std::function<void()> onBack_;
    std::function<void(DexFeatures::FeatureRegistry)> onSaveAndBack_;
    QTableWidget *features_ = nullptr;
};
