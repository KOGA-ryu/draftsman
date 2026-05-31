#pragma once

#include <QMap>
#include <QString>
#include <QStringList>
#include <QVector>

class QJsonDocument;
class QJsonObject;

namespace DexRabbitHoleReview {

struct ReviewSubject {
    QString subjectId;
    QString label;
    QString status = "candidate";
    QString summary;
    QString previewKind = "placeholder";
    QString previewLabel = "Focused preview";
    QString previewDescription;
};

struct ReviewRoute {
    QString routeId;
    QString label;
    QString summary;
    QString status = "pending";
    QStringList children;
};

struct ReviewComponent {
    QString componentId;
    QString label;
    QString routeId;
    QString status = "pending";
    QString summary;
};

struct ReviewComment {
    QString commentId;
    QString routeId;
    QString componentId;
    QString selectedTab;
    QString status = "pending";
    QString comment;
    QString createdAt;
};

struct ReviewFixture {
    QString sourcePath;
    bool loaded = false;
    QString error;
    int schemaVersion = 1;
    ReviewSubject subject;
    QVector<ReviewRoute> routes;
    QVector<ReviewComponent> components;
    QVector<ReviewComment> comments;
};

ReviewFixture defaultReviewFixture();
ReviewFixture parseReviewFixtureDocument(const QJsonDocument &document, const QString &sourcePath = QString());
ReviewFixture loadReviewFixtureFile(const QString &path);
const ReviewRoute *findRouteById(const ReviewFixture &fixture, const QString &routeId);
QVector<ReviewComponent> componentsForRoute(const ReviewFixture &fixture, const QString &routeId);
QVector<ReviewComment> commentsForRoute(const ReviewFixture &fixture, const QString &routeId);
QStringList routeBreadcrumbLabels(const ReviewFixture &fixture, const QString &routeId);
QMap<QString, int> statusCounts(const ReviewFixture &fixture);
QString defaultRouteId(const ReviewFixture &fixture);

} // namespace DexRabbitHoleReview
