#include "rabbit_hole_review_model.h"

#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QJsonValue>

namespace DexRabbitHoleReview {
namespace {

QString readString(const QJsonObject &object, const char *key, const QString &fallback = QString()) {
    const QJsonValue value = object.value(QString::fromLatin1(key));
    return value.isString() ? value.toString() : fallback;
}

int readInt(const QJsonObject &object, const char *key, int fallback) {
    const QJsonValue value = object.value(QString::fromLatin1(key));
    return value.isDouble() ? value.toInt() : fallback;
}

QStringList readStringList(const QJsonObject &object, const char *key) {
    QStringList values;
    const QJsonValue value = object.value(QString::fromLatin1(key));
    if (!value.isArray()) {
        return values;
    }
    for (const QJsonValue &entry : value.toArray()) {
        if (entry.isString() && !entry.toString().trimmed().isEmpty()) {
            values.push_back(entry.toString().trimmed());
        }
    }
    return values;
}

QString resolveFixturePath(const QString &path) {
    if (path.isEmpty()) {
        return path;
    }
    const QFileInfo direct(path);
    if (direct.isAbsolute()) {
        return direct.absoluteFilePath();
    }

    const QStringList starts = {QDir::currentPath(), QCoreApplication::applicationDirPath()};
    for (const QString &start : starts) {
        QDir dir(start);
        for (int depth = 0; depth < 8; ++depth) {
            const QString candidate = dir.filePath(path);
            if (QFileInfo::exists(candidate)) {
                return QFileInfo(candidate).absoluteFilePath();
            }
            if (!dir.cdUp()) {
                break;
            }
        }
    }
    return QFileInfo(path).absoluteFilePath();
}

ReviewRoute parseRoute(const QJsonObject &object) {
    ReviewRoute route;
    route.routeId = readString(object, "route_id");
    route.label = readString(object, "label", route.routeId);
    route.summary = readString(object, "summary");
    route.status = readString(object, "status", "pending");
    route.children = readStringList(object, "children");
    return route;
}

ReviewComponent parseComponent(const QJsonObject &object) {
    ReviewComponent component;
    component.componentId = readString(object, "component_id");
    component.label = readString(object, "label", component.componentId);
    component.routeId = readString(object, "route_id");
    component.status = readString(object, "status", "pending");
    component.summary = readString(object, "summary");
    return component;
}

ReviewComment parseComment(const QJsonObject &object) {
    ReviewComment comment;
    comment.commentId = readString(object, "comment_id");
    comment.routeId = readString(object, "route_id");
    comment.componentId = readString(object, "component_id");
    comment.selectedTab = readString(object, "selected_tab");
    comment.status = readString(object, "status", "pending");
    comment.comment = readString(object, "comment");
    comment.createdAt = readString(object, "created_at");
    return comment;
}

} // namespace

ReviewFixture defaultReviewFixture() {
    ReviewFixture fixture;
    fixture.loaded = true;
    fixture.subject.subjectId = "empty_review_subject";
    fixture.subject.label = "Review Subject";
    fixture.subject.summary = "No review fixture loaded.";
    fixture.routes.push_back({"overview", "Overview", "No routes loaded.", "pending", {}});
    return fixture;
}

ReviewFixture parseReviewFixtureDocument(const QJsonDocument &document, const QString &sourcePath) {
    ReviewFixture fixture = defaultReviewFixture();
    fixture.sourcePath = sourcePath;
    fixture.routes.clear();

    const QJsonObject root = document.object();
    fixture.schemaVersion = readInt(root, "schema_version", fixture.schemaVersion);

    const QJsonObject subject = root.value("subject").toObject();
    fixture.subject.subjectId = readString(subject, "subject_id", fixture.subject.subjectId);
    fixture.subject.label = readString(subject, "label", fixture.subject.label);
    fixture.subject.status = readString(subject, "status", fixture.subject.status);
    fixture.subject.summary = readString(subject, "summary", fixture.subject.summary);
    const QJsonObject preview = subject.value("preview").toObject();
    fixture.subject.previewKind = readString(preview, "kind", fixture.subject.previewKind);
    fixture.subject.previewLabel = readString(preview, "label", fixture.subject.previewLabel);
    fixture.subject.previewDescription = readString(preview, "description", fixture.subject.previewDescription);

    for (const QJsonValue &value : root.value("routes").toArray()) {
        if (!value.isObject()) {
            continue;
        }
        const ReviewRoute route = parseRoute(value.toObject());
        if (!route.routeId.isEmpty()) {
            fixture.routes.push_back(route);
        }
    }
    for (const QJsonValue &value : root.value("components").toArray()) {
        if (!value.isObject()) {
            continue;
        }
        const ReviewComponent component = parseComponent(value.toObject());
        if (!component.componentId.isEmpty()) {
            fixture.components.push_back(component);
        }
    }
    for (const QJsonValue &value : root.value("comments").toArray()) {
        if (!value.isObject()) {
            continue;
        }
        const ReviewComment comment = parseComment(value.toObject());
        if (!comment.commentId.isEmpty()) {
            fixture.comments.push_back(comment);
        }
    }
    if (fixture.routes.isEmpty()) {
        fixture.routes.push_back({"overview", "Overview", "No routes loaded.", "pending", {}});
    }
    fixture.loaded = true;
    return fixture;
}

ReviewFixture loadReviewFixtureFile(const QString &path) {
    ReviewFixture fixture = defaultReviewFixture();
    fixture.sourcePath = resolveFixturePath(path);
    QFile file(fixture.sourcePath);
    if (!file.open(QIODevice::ReadOnly)) {
        fixture.loaded = false;
        fixture.error = "review fixture not found: " + fixture.sourcePath;
        return fixture;
    }

    QJsonParseError parseError;
    const QJsonDocument document = QJsonDocument::fromJson(file.readAll(), &parseError);
    if (parseError.error != QJsonParseError::NoError) {
        fixture.loaded = false;
        fixture.error = "review fixture JSON error: " + parseError.errorString();
        return fixture;
    }
    return parseReviewFixtureDocument(document, QFileInfo(fixture.sourcePath).absoluteFilePath());
}

const ReviewRoute *findRouteById(const ReviewFixture &fixture, const QString &routeId) {
    for (const ReviewRoute &route : fixture.routes) {
        if (route.routeId == routeId) {
            return &route;
        }
    }
    return nullptr;
}

QVector<ReviewComponent> componentsForRoute(const ReviewFixture &fixture, const QString &routeId) {
    QVector<ReviewComponent> result;
    for (const ReviewComponent &component : fixture.components) {
        if (component.routeId == routeId) {
            result.push_back(component);
        }
    }
    return result;
}

QVector<ReviewComment> commentsForRoute(const ReviewFixture &fixture, const QString &routeId) {
    QVector<ReviewComment> result;
    for (const ReviewComment &comment : fixture.comments) {
        if (comment.routeId == routeId) {
            result.push_back(comment);
        }
    }
    return result;
}

QStringList routeBreadcrumbLabels(const ReviewFixture &fixture, const QString &routeId) {
    QStringList labels;
    labels.push_back(fixture.subject.label);
    if (routeId.isEmpty()) {
        return labels;
    }
    QString prefix;
    for (const QString &part : routeId.split('/', Qt::SkipEmptyParts)) {
        prefix = prefix.isEmpty() ? part : prefix + "/" + part;
        if (const ReviewRoute *route = findRouteById(fixture, prefix)) {
            labels.push_back(route->label);
        } else {
            labels.push_back(part);
        }
    }
    return labels;
}

QMap<QString, int> statusCounts(const ReviewFixture &fixture) {
    QMap<QString, int> counts;
    for (const QString &status : {"pending", "accepted", "needs_rework", "rejected"}) {
        counts.insert(status, 0);
    }
    for (const ReviewRoute &route : fixture.routes) {
        counts[route.status] = counts.value(route.status) + 1;
    }
    for (const ReviewComponent &component : fixture.components) {
        counts[component.status] = counts.value(component.status) + 1;
    }
    for (const ReviewComment &comment : fixture.comments) {
        counts[comment.status] = counts.value(comment.status) + 1;
    }
    return counts;
}

QString defaultRouteId(const ReviewFixture &fixture) {
    return fixture.routes.isEmpty() ? QString("overview") : fixture.routes.first().routeId;
}

} // namespace DexRabbitHoleReview
