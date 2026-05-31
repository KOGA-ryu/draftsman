#pragma once

#include <QButtonGroup>
#include <QHBoxLayout>
#include <QPushButton>
#include <QStackedWidget>
#include <QString>
#include <QStringList>
#include <QTabWidget>
#include <QVBoxLayout>
#include <QWidget>

#include <algorithm>
#include <functional>
#include <utility>

#include "agent_binder_pages.h"
#include "app_state.h"
#include "app_state_helpers.h"
#include "binder_navigation.h"
#include "feature_settings_page.h"
#include "render_helpers.h"
#include "repo_binder_template.h"
#include "repo_binder_pages.h"
#include "project_registry_spec_page.h"
#include "shell_layout.h"
#include "shell_layout_settings_page.h"
#include "theme_settings_page.h"
#include "ui_rules.h"

class LedgerView final : public QWidget {
public:
    explicit LedgerView(std::function<void(QString)> onTabSelected, QWidget *parent = nullptr)
        : QWidget(parent), onTabSelected_(std::move(onTabSelected)) {
        setObjectName("centerLedger");
        auto *outer = new QVBoxLayout(this);
        outer->setContentsMargins(dex_ui::metrics::sheet_overlap, 0, dex_ui::metrics::sheet_overlap, 0);
        outer->setSpacing(0);

        auto *tabRow = new QWidget;
        tabLayout_ = new QHBoxLayout(tabRow);
        tabLayout_->setContentsMargins(0, 0, 0, 0);
        tabLayout_->setSpacing(0);

        pages_ = new QStackedWidget;
        tabGroup_ = new QButtonGroup(this);
        tabGroup_->setExclusive(true);

        connect(tabGroup_, &QButtonGroup::idClicked, this, [this](int index) {
            pages_->setCurrentIndex(index);
            currentTab_ = currentTabs_.value(index, "Profile");
            onTabSelected_(currentTab_);
        });

        outer->addWidget(tabRow);
        outer->addWidget(pages_, 1);
    }

    void setState(
        const CockpitState &state,
        const QString &workerId,
        const QString &selectedProjectId,
        const QString &topTab,
        const QString &detailLens,
        bool repoMode) {
        const QStringList tabs = repoMode ? DraftsmanShell::enabledTabLabels(state.shellLayout) : topTabsFor(false);
        if (currentTabs_ != tabs) {
            rebuildTabs(tabs);
        }
        const int requestedIndex = std::max(0, static_cast<int>(tabs.indexOf(topTab)));
        const int previousIndex = pages_->count() == 0 ? requestedIndex : std::max(0, pages_->currentIndex());
        while (pages_->count() > 0) {
            QWidget *page = pages_->widget(0);
            pages_->removeWidget(page);
            page->deleteLater();
        }

        if (repoMode) {
            for (const QString &tab : tabs) {
                pages_->addWidget(DexBinderPages::buildRepoBlankPage(state, tab, detailLens));
            }
        } else {
            pages_->addWidget(DexBinderPages::buildProfilePage(state, workerId, detailLens));
            pages_->addWidget(DexBinderPages::buildStatsPage(state, workerId, detailLens));
            pages_->addWidget(DexBinderPages::buildRelationshipPage(state, workerId, detailLens));
            pages_->addWidget(DexBinderPages::buildGradePage(state, workerId, detailLens));
            pages_->addWidget(DexBinderPages::buildTranscriptPage(state, workerId, detailLens));
            pages_->addWidget(DexBinderPages::buildEvidencePage(state, workerId, detailLens));
        }
        const int nextIndex = requestedIndex >= 0
            ? requestedIndex
            : std::min(previousIndex, pages_->count() - 1);
        pages_->setCurrentIndex(std::clamp(nextIndex, 0, pages_->count() - 1));
        currentTab_ = tabs.value(pages_->currentIndex(), "Profile");
        if (auto *button = tabGroup_->button(pages_->currentIndex())) {
            button->setChecked(true);
        }
    }

    void setSettingsState(
        const CockpitState &state,
        const QString &selectedProjectId,
        std::function<void(DexProjects::ProjectRegistry, QString)> onSave,
        std::function<void()> onRevert,
        std::function<void()> onBack,
        std::function<void(DexProjects::ProjectRegistry, QString)> onSaveAndBack,
        std::function<void(DraftsmanShell::ShellLayout)> onSaveShellLayout,
        std::function<void(DraftsmanShell::ShellLayout)> onSaveShellLayoutAndBack,
        std::function<void(dex_ui::UiTheme)> onSaveTheme,
        std::function<void(dex_ui::UiTheme)> onSaveThemeAndBack,
        std::function<void(DexFeatures::FeatureRegistry)> onSaveFeatures,
        std::function<void(DexFeatures::FeatureRegistry)> onSaveFeaturesAndBack) {
        clearLayout(tabLayout_);
        currentTabs_.clear();
        while (pages_->count() > 0) {
            QWidget *page = pages_->widget(0);
            pages_->removeWidget(page);
            page->deleteLater();
        }

        DexProjects::ProjectRegistry registry;
        registry.loaded = state.projectRegistryLoaded;
        registry.sourcePath = state.projectRegistrySource;
        registry.error = state.projectRegistryError;
        registry.projects = state.registryProjects;
        registry.workers = state.registryWorkers;
        auto *settingsTabs = new QTabWidget;
        settingsTabs->addTab(new ShellLayoutSettingsPage(
            state.shellLayout,
            std::move(onSaveShellLayout),
            onRevert,
            onBack,
            std::move(onSaveShellLayoutAndBack)),
            "Shell Layout");
        settingsTabs->addTab(new ProjectRegistrySpecPage(
            registry,
            selectedProjectId,
            std::move(onSave),
            std::move(onRevert),
            std::move(onBack),
            std::move(onSaveAndBack)),
            "Project Registry");
        settingsTabs->addTab(new ThemeSettingsPage(
            state.uiTheme,
            std::move(onSaveTheme),
            onRevert,
            onBack,
            std::move(onSaveThemeAndBack)),
            "Theme");
        settingsTabs->addTab(new FeatureSettingsPage(
            state.featureRegistry,
            std::move(onSaveFeatures),
            onRevert,
            onBack,
            std::move(onSaveFeaturesAndBack)),
            "Features");
        pages_->addWidget(settingsTabs);
        pages_->setCurrentIndex(0);
        currentTab_ = "Settings";
    }

private:
    void rebuildTabs(const QStringList &tabs) {
        clearLayout(tabLayout_);
        delete tabGroup_;
        tabGroup_ = new QButtonGroup(this);
        tabGroup_->setExclusive(true);
        connect(tabGroup_, &QButtonGroup::idClicked, this, [this](int index) {
            pages_->setCurrentIndex(index);
            currentTab_ = currentTabs_.value(index, "Profile");
            onTabSelected_(currentTab_);
        });
        currentTabs_ = tabs;
        for (int i = 0; i < currentTabs_.size(); ++i) {
            auto *button = new QPushButton(currentTabs_.at(i));
            button->setObjectName("ledgerTab");
            button->setProperty("tabName", currentTabs_.at(i));
            button->setProperty("tabIndex", i);
            button->setProperty("firstTab", i == 0);
            button->setProperty("lastTab", i == currentTabs_.size() - 1);
            button->setCheckable(true);
            if (i == 0) {
                button->setChecked(true);
            }
            tabGroup_->addButton(button, i);
            tabLayout_->addWidget(button);
        }
        tabLayout_->addStretch(1);
    }

    QString currentTab_ = "Profile";
    QStringList currentTabs_;
    std::function<void(QString)> onTabSelected_;
    QStackedWidget *pages_ = nullptr;
    QButtonGroup *tabGroup_ = nullptr;
    QHBoxLayout *tabLayout_ = nullptr;
};
