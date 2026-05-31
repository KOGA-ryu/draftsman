#include "rabbit_hole_review_page.h"

#include <QGridLayout>
#include <QFrame>
#include <QHBoxLayout>
#include <QPushButton>
#include <QScrollArea>
#include <QStyle>
#include <QVBoxLayout>

#include <algorithm>
#include <utility>

#include "binder_page_helpers.h"
#include "rabbit_hole_review_model.h"
#include "rabbit_hole_review_state.h"
#include "render_helpers.h"

namespace DexRabbitHoleReview {
namespace {

QString statusLabel(const QString &status) {
    if (status == "needs_rework") {
        return "needs rework";
    }
    return status.isEmpty() ? QString("pending") : status;
}

QPushButton *actionButton(const QString &label, bool primary = false) {
    auto *button = new QPushButton(label);
    button->setObjectName(primary ? "primaryAction" : "statsContextAction");
    return button;
}

} // namespace

class RabbitHoleReviewPage final : public QWidget {
public:
    RabbitHoleReviewPage(ReviewFixture fixture, RabbitHoleReviewState settings, QString topTab, QString detailLens, QWidget *parent = nullptr)
        : QWidget(parent),
          fixture_(std::move(fixture)),
          settings_(std::move(settings)),
          topTab_(std::move(topTab)),
          detailLens_(std::move(detailLens)),
          currentRouteId_(defaultRouteId(fixture_)) {
        auto *outer = new QVBoxLayout(this);
        outer->setContentsMargins(0, 0, 0, 0);
        outer->setSpacing(0);

        auto *scroll = new QScrollArea;
        scroll->setWidgetResizable(true);
        scroll->setFrameShape(QFrame::NoFrame);
        scroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

        auto *body = new QWidget;
        auto *layout = new QVBoxLayout(body);
        layout->setContentsMargins(10, 8, 10, 8);
        layout->setSpacing(6);

        headerSection_ = DexBinderPages::makeStatsSection("rabbit-hole review", true);
        layout->addWidget(headerSection_);

        auto *controls = new QWidget;
        auto *controlsLayout = new QHBoxLayout(controls);
        controlsLayout->setContentsMargins(0, 0, 0, 0);
        back_ = actionButton("Back");
        forward_ = actionButton("Forward");
        home_ = actionButton("Home", true);
        connect(back_, &QPushButton::clicked, this, [this]() { goBack(); });
        connect(forward_, &QPushButton::clicked, this, [this]() { goForward(); });
        connect(home_, &QPushButton::clicked, this, [this]() { goHome(); });
        controlsLayout->addWidget(back_);
        controlsLayout->addWidget(forward_);
        controlsLayout->addWidget(home_);
        controlsLayout->addStretch(1);
        layout->addWidget(controls);

        auto *tabs = new QWidget;
        auto *tabsLayout = new QHBoxLayout(tabs);
        tabsLayout->setContentsMargins(0, 0, 0, 0);
        for (const QString &tab : {"Overview", "Components", "Validation", "Comments"}) {
            auto *button = actionButton(tab, tab == localTab_);
            connect(button, &QPushButton::clicked, this, [this, tab]() {
                localTab_ = tab;
                render();
            });
            localTabs_.push_back(button);
            tabsLayout->addWidget(button);
        }
        tabsLayout->addStretch(1);
        layout->addWidget(tabs);

        content_ = new QWidget;
        contentLayout_ = new QVBoxLayout(content_);
        contentLayout_->setContentsMargins(0, 0, 0, 0);
        contentLayout_->setSpacing(6);
        layout->addWidget(content_);
        layout->addStretch(1);

        scroll->setWidget(body);
        outer->addWidget(scroll, 1);
        render();
    }

private:
    void setRoute(const QString &routeId) {
        if (routeId == currentRouteId_ || !findRouteById(fixture_, routeId)) {
            return;
        }
        backStack_.push_back(currentRouteId_);
        forwardStack_.clear();
        currentRouteId_ = routeId;
        render();
    }

    void goBack() {
        if (backStack_.isEmpty()) {
            return;
        }
        forwardStack_.push_back(currentRouteId_);
        currentRouteId_ = backStack_.takeLast();
        render();
    }

    void goForward() {
        if (forwardStack_.isEmpty()) {
            return;
        }
        backStack_.push_back(currentRouteId_);
        currentRouteId_ = forwardStack_.takeLast();
        render();
    }

    void goHome() {
        setRoute(defaultRouteId(fixture_));
    }

    void render() {
        clearLayout(headerSection_->layout());
        auto *headerLayout = static_cast<QVBoxLayout *>(headerSection_->layout());
        const ReviewRoute *route = findRouteById(fixture_, currentRouteId_);
        headerLayout->addWidget(DexBinderPages::makeStatsRow({
            "subject",
            fixture_.subject.label,
            "status",
            statusLabel(fixture_.subject.status),
            "route",
            route ? route->label : currentRouteId_,
            "tab",
            topTab_,
        }, false, false));
        headerLayout->addWidget(DexBinderPages::makeStatsText(fixture_.subject.summary));
        headerLayout->addWidget(DexBinderPages::makeStatsRow({
            "breadcrumb",
            routeBreadcrumbLabels(fixture_, currentRouteId_).join(" / "),
            "local tab",
            localTab_,
        }, false, false));

        back_->setEnabled(!backStack_.isEmpty());
        forward_->setEnabled(!forwardStack_.isEmpty());
        for (QPushButton *button : localTabs_) {
            button->setObjectName(button->text() == localTab_ ? "primaryAction" : "statsContextAction");
            button->style()->unpolish(button);
            button->style()->polish(button);
        }

        clearLayout(contentLayout_);
        addPreview();
        if (localTab_ == "Components") {
            addComponents();
        } else if (localTab_ == "Validation") {
            addValidation();
        } else if (localTab_ == "Comments") {
            addComments(true);
        } else {
            addRouteCards();
            addComments(false);
        }
    }

    void addPreview() {
        auto *preview = DexBinderPages::makeStatsSection("focused preview", true);
        preview->setMinimumHeight(120);
        auto *layout = static_cast<QVBoxLayout *>(preview->layout());
        layout->addWidget(DexBinderPages::makeStatsRow({
            "kind",
            fixture_.subject.previewKind,
            "focus",
            settings_.enableComponentFocus ? QString("enabled placeholder") : QString("disabled"),
            "route",
            currentRouteId_,
        }, false, false));
        layout->addWidget(DexBinderPages::makeStatsText(fixture_.subject.previewLabel + ": " + fixture_.subject.previewDescription));
        contentLayout_->addWidget(preview);
    }

    void addRouteCards() {
        const ReviewRoute *route = findRouteById(fixture_, currentRouteId_);
        auto *section = DexBinderPages::makeStatsSection("route drilldown");
        auto *layout = static_cast<QVBoxLayout *>(section->layout());
        if (!route || route->children.isEmpty()) {
            layout->addWidget(DexBinderPages::makeStatsText("No child routes. Review components and comments for this route."));
            contentLayout_->addWidget(section);
            return;
        }
        auto *gridHost = new QWidget;
        auto *grid = new QGridLayout(gridHost);
        grid->setContentsMargins(0, 0, 0, 0);
        grid->setSpacing(6);
        int index = 0;
        for (const QString &childId : route->children) {
            const ReviewRoute *child = findRouteById(fixture_, childId);
            if (!child) {
                continue;
            }
            auto *card = actionButton(child->label);
            card->setMinimumHeight(52);
            card->setToolTip(child->summary);
            connect(card, &QPushButton::clicked, this, [this, childId]() { setRoute(childId); });
            grid->addWidget(card, index / 2, index % 2);
            ++index;
        }
        layout->addWidget(gridHost);
        contentLayout_->addWidget(section);
    }

    void addComponents() {
        auto *section = DexBinderPages::makeStatsSection("components");
        auto *layout = static_cast<QVBoxLayout *>(section->layout());
        const QVector<ReviewComponent> components = componentsForRoute(fixture_, currentRouteId_);
        if (components.isEmpty()) {
            layout->addWidget(DexBinderPages::makeStatsText("No components attached to this route."));
        }
        for (const ReviewComponent &component : components) {
            layout->addWidget(DexBinderPages::makeStatsRow({
                component.label,
                "id: " + component.componentId,
                "status: " + statusLabel(component.status),
                component.summary,
            }, false, false));
        }
        contentLayout_->addWidget(section);
    }

    void addValidation() {
        auto *section = DexBinderPages::makeStatsSection("status summary", true);
        auto *layout = static_cast<QVBoxLayout *>(section->layout());
        const QMap<QString, int> counts = statusCounts(fixture_);
        layout->addWidget(DexBinderPages::makeStatsRow({
            "pending",
            QString::number(counts.value("pending")),
            "accepted",
            QString::number(counts.value("accepted")),
            "needs rework",
            QString::number(counts.value("needs_rework")),
            "rejected",
            QString::number(counts.value("rejected")),
        }, false, false));
        layout->addWidget(DexBinderPages::makeStatsRow({
            "persistence",
            settings_.persistReviewState ? QString("enabled") : QString("disabled"),
            "exports",
            settings_.enableExports ? QString("enabled") : QString("disabled"),
            "max comment chars",
            QString::number(settings_.maxCommentChars),
        }, false, false));
        contentLayout_->addWidget(section);
    }

    void addComments(bool allComments) {
        auto *section = DexBinderPages::makeStatsSection(allComments ? "comments" : "route comments");
        auto *layout = static_cast<QVBoxLayout *>(section->layout());
        QVector<ReviewComment> comments = allComments ? fixture_.comments : commentsForRoute(fixture_, currentRouteId_);
        if (comments.isEmpty()) {
            layout->addWidget(DexBinderPages::makeStatsText("No comments attached to this route."));
        }
        for (const ReviewComment &comment : comments) {
            layout->addWidget(DexBinderPages::makeStatsRow({
                statusLabel(comment.status),
                "route: " + comment.routeId,
                "component: " + (comment.componentId.isEmpty() ? QString("none") : comment.componentId),
                "tab: " + comment.selectedTab,
            }, comment.status == "rejected" || comment.status == "needs_rework", false));
            layout->addWidget(DexBinderPages::makeStatsText(comment.comment));
        }
        contentLayout_->addWidget(section);
    }

    ReviewFixture fixture_;
    RabbitHoleReviewState settings_;
    QString topTab_;
    QString detailLens_;
    QString currentRouteId_;
    QString localTab_ = "Overview";
    QStringList backStack_;
    QStringList forwardStack_;
    QFrame *headerSection_ = nullptr;
    QWidget *content_ = nullptr;
    QVBoxLayout *contentLayout_ = nullptr;
    QPushButton *back_ = nullptr;
    QPushButton *forward_ = nullptr;
    QPushButton *home_ = nullptr;
    QVector<QPushButton *> localTabs_;
};

QWidget *buildRabbitHoleReviewPage(
    const CockpitState &state,
    const QString &topTab,
    const QString &detailLens) {
    const DexFeatures::FeatureRecord *feature =
        activeRabbitHoleReviewFeature(state.featureRegistry, topTab);
    const RabbitHoleReviewState settings =
        feature ? rabbitHoleReviewState(*feature) : RabbitHoleReviewState();
    return new RabbitHoleReviewPage(
        loadReviewFixtureFile(settings.fixturePath),
        settings,
        topTab,
        detailLens);
}

} // namespace DexRabbitHoleReview
