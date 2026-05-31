#include <QFileInfo>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QProcess>
#include <QTemporaryDir>
#include <QtTest/QtTest>

#include "binder_state.h"
#include "app_state_helpers.h"
#include "project_registry.h"
#include "project_registry_spec_model.h"
#include "repo_binder_template.h"
#include "shell_layout.h"
#include "ui_theme.h"

class StateBackendSmoke final : public QObject {
    Q_OBJECT

private:
    void runJsonCommand(const QString &binary, const QStringList &arguments, QJsonDocument *document) {
        QProcess process;
        process.start(binary, arguments);
        QVERIFY(process.waitForStarted(3000));
        QVERIFY(process.waitForFinished(8000));
        QCOMPARE(process.exitStatus(), QProcess::NormalExit);
        QCOMPARE(process.exitCode(), 0);

        QJsonParseError parseError;
        *document = QJsonDocument::fromJson(process.readAllStandardOutput(), &parseError);
        QCOMPARE(parseError.error, QJsonParseError::NoError);
    }

private slots:
    void derivesStateFromRustCore() {
        const QString binary = QString::fromUtf8(DEX_COCKPIT_CORE_BIN);
        if (!QFileInfo::exists(binary)) {
            QSKIP(qPrintable("Missing dex-cockpit-core binary: " + binary));
        }

        QTemporaryDir repoRoot;
        QVERIFY(repoRoot.isValid());

        QJsonDocument document;
        runJsonCommand(binary, {"state", "derive", "--repo-root", repoRoot.path()}, &document);

        const QJsonObject root = document.object();
        QCOMPARE(root.value("selected_project_id").toString(), QString("draftsman"));
        QCOMPARE(root.value("selected_worker_id").toString(), QString("planner"));
        QCOMPARE(root.value("projects").toArray().size(), 1);
        QVERIFY(root.value("workers").toArray().size() >= 4);
        QVERIFY(!root.value("ledger_entries").toArray().isEmpty());
        QCOMPARE(root.value("sessions").toArray().size(), 0);
        QCOMPARE(root.value("packets").toArray().size(), 0);
        QVERIFY(root.value("command_plans").toArray().size() >= 4);
        QVERIFY(!root.value("profiles").toArray().isEmpty());
        QCOMPARE(root.value("selected_profile_id").toString(), QString("dex_terminal_default"));
        QCOMPARE(root.value("profile_graph").toObject().value("nodes").toArray().size(), 6);
        QCOMPARE(root.value("launch_plan_preview").toObject().value("instances").toArray().size(), 6);

        bool sawBuildProtocol = false;
        for (const QJsonValue &value : root.value("bench_candidates").toArray()) {
            if (value.toObject().value("candidate_id").toString() == "build_protocol") {
                sawBuildProtocol = true;
            }
            QVERIFY(value.toObject().contains("expected_artifact_outputs"));
        }
        QVERIFY(sawBuildProtocol);

        QVERIFY(root.contains("stats_snapshot"));
        QVERIFY(root.contains("grade_records"));
        QVERIFY(root.contains("transcript_records"));
        QVERIFY(root.contains("evidence_records"));
        const DexBinder::BinderState binder = DexBinder::parseBinderState(root);
        QCOMPARE(binder.statsSnapshot.confidence, QString("no_data"));
        QCOMPARE(binder.gradeRecords.size(), 0);
        QCOMPARE(binder.transcriptRecords.size(), 0);
        QCOMPARE(binder.evidenceRecords.size(), 0);
    }

    void exposesProfilePlannerCommands() {
        const QString binary = QString::fromUtf8(DEX_COCKPIT_CORE_BIN);
        if (!QFileInfo::exists(binary)) {
            QSKIP(qPrintable("Missing dex-cockpit-core binary: " + binary));
        }

        QTemporaryDir repoRoot;
        QVERIFY(repoRoot.isValid());

        QJsonDocument listDocument;
        runJsonCommand(binary, {"profile", "list", "--repo-root", repoRoot.path()}, &listDocument);
        const QJsonArray profiles = listDocument.array();
        QVERIFY(!profiles.isEmpty());
        const QString profileId = profiles.first().toObject().value("profile_id").toString();
        QCOMPARE(profileId, QString("dex_terminal_default"));

        QJsonDocument graphDocument;
        runJsonCommand(
            binary,
            {"profile", "graph", "--repo-root", repoRoot.path(), "--profile-id", profileId},
            &graphDocument);
        const QJsonObject graph = graphDocument.object();
        QCOMPARE(graph.value("nodes").toArray().size(), profiles.first().toObject().value("roles").toArray().size());
        QVERIFY(!graph.value("edges").toArray().isEmpty());

        QJsonDocument launchDocument;
        runJsonCommand(
            binary,
            {"profile", "launch-plan", "--repo-root", repoRoot.path(), "--profile-id", profileId, "--count", "6"},
            &launchDocument);
        const QJsonObject launchPlan = launchDocument.object();
        QCOMPARE(launchPlan.value("instances").toArray().size(), 6);
        QVERIFY(launchPlan.value("instances").toArray().first().toObject().value("startup_brief").toString().contains("Dex Role:"));
    }

    void binderTopTabsAreStable() {
        QCOMPARE(
            DexBinder::binderTopTabs(),
            QStringList({"Profile", "Stats", "Relationship", "Grade", "Transcript", "Evidence"}));
    }

    void parsesDefaultRepoBinderTemplate() {
        const QString path = QString::fromUtf8(DRAFTSMAN_SOURCE_DIR)
            + "/data/binder_templates/repo_default_binder_v1.json";
        const DexRepoBinderTemplate::BinderTemplate templateRecord =
            DexRepoBinderTemplate::loadBinderTemplateFile(path);

        QVERIFY2(templateRecord.loaded, qPrintable(templateRecord.error));
        QCOMPARE(templateRecord.templateId, QString("repo_default_binder_v1"));
        QVERIFY(templateRecord.topTabs.contains("Profile"));
        QVERIFY(templateRecord.detailLenses.value("Inventory").contains("Files"));
        QCOMPARE(templateRecord.contextLines.size(), 0);
        QCOMPARE(templateRecord.sections.size(), 0);
    }

    void productionProjectRegistryIsBlankSlate() {
        const QString path = QString::fromUtf8(DRAFTSMAN_SOURCE_DIR) + "/data/projects.json";
        const DexProjects::ProjectRegistry registry = DexProjects::loadProjectRegistryFile(path);

        QVERIFY2(registry.loaded, qPrintable(registry.error));
        QCOMPARE(registry.projects.size(), 0);
        QCOMPARE(registry.workers.size(), 0);
    }

    void productionShellLayoutIsCustomizable() {
        const QString path = QString::fromUtf8(DRAFTSMAN_SOURCE_DIR) + "/data/shell_layout.json";
        const DraftsmanShell::ShellLayout layout = DraftsmanShell::loadShellLayoutFile(path);

        QVERIFY2(layout.loaded, qPrintable(layout.error));
        QCOMPARE(layout.appTitle, QString("Draftsman"));
        QVERIFY(DraftsmanShell::enabledTabLabels(layout).contains("Overview"));
        QVERIFY(DraftsmanShell::enabledTabLabels(layout).contains("Reviews"));
        QVERIFY(!DraftsmanShell::enabledPanelsForTab(layout, "Overview").isEmpty());
        QVERIFY(!DraftsmanShell::enabledInspectorPanels(layout, "Overview", "Dashboard").isEmpty());
    }

    void productionUiThemeUsesEditableColors() {
        const QString path = QString::fromUtf8(DRAFTSMAN_SOURCE_DIR) + "/data/ui_theme.json";
        const dex_ui::UiTheme theme = dex_ui::loadUiThemeFile(path);

        QVERIFY2(theme.loaded, qPrintable(theme.error));
        QVERIFY(dex_ui::isValidColor(theme.base));
        QVERIFY(dex_ui::isValidColor(theme.surface));
        QVERIFY(dex_ui::isValidColor(theme.accent));
        QVERIFY(dex_ui::isValidColor(theme.text));
        QVERIFY(theme.themeMode == "light" || theme.themeMode == "dark" || theme.themeMode == "system");
        QVERIFY(theme.uiFontSize >= 9);
        QVERIFY(theme.codeFontSize >= 9);
        const QJsonObject serialized = dex_ui::uiThemeToJson(theme);
        QCOMPARE(serialized.keys().size(), 9);
        QVERIFY(serialized.contains("theme_mode"));
        QVERIFY(serialized.contains("base"));
        QVERIFY(serialized.contains("surface"));
        QVERIFY(serialized.contains("accent"));
        QVERIFY(serialized.contains("text"));
        QVERIFY(serialized.contains("ui_font"));
        QVERIFY(serialized.contains("code_font"));
        QVERIFY(serialized.contains("ui_font_size"));
        QVERIFY(serialized.contains("code_font_size"));
    }

    void shellLayoutParsesAgentEditableLines() {
        const QJsonDocument document(QJsonObject{
            {"app_title", "Agent Filled Binder"},
            {"left_rail", QJsonObject{{"sections", QJsonArray{QJsonObject{
                {"id", "projects"},
                {"title", "Projects"},
                {"items", QJsonArray{QJsonObject{{"id", "sample"}, {"label", "Sample"}}}},
            }}}}},
            {"tabs", QJsonArray{QJsonObject{{"id", "overview"}, {"label", "Overview"}, {"enabled", true}}}},
            {"panels", QJsonArray{QJsonObject{
                {"id", "repo_purpose"},
                {"label", "Repo Purpose"},
                {"tab", "Overview"},
                {"lines", QJsonArray{"fact one", "fact two"}},
                {"enabled", true},
            }}},
            {"inspector", QJsonObject{{"panels", QJsonArray{QJsonObject{
                {"id", "agent_receipt"},
                {"label", "Agent Receipt"},
                {"tab", "*"},
                {"text", "session key: sample\nsource: scan"},
                {"enabled", true},
            }}}}},
        });

        const DraftsmanShell::ShellLayout layout = DraftsmanShell::parseShellLayoutDocument(document);
        const QVector<DraftsmanShell::ShellPanel> panels =
            DraftsmanShell::enabledPanelsForTab(layout, "Overview");
        const QVector<DraftsmanShell::ShellPanel> inspector =
            DraftsmanShell::enabledInspectorPanels(layout, "Overview", "Dashboard");

        QCOMPARE(layout.appTitle, QString("Agent Filled Binder"));
        QCOMPARE(panels.size(), 1);
        QCOMPARE(panels.first().lines, QStringList({"fact one", "fact two"}));
        QCOMPARE(inspector.size(), 1);
        QCOMPARE(inspector.first().lines, QStringList({"session key: sample", "source: scan"}));

        const QJsonObject serialized = DraftsmanShell::shellLayoutToJson(layout);
        QCOMPARE(serialized.value("panels").toArray().first().toObject().value("lines").toArray().size(), 2);
    }

    void binderTemplateParserHandlesMissingOptionalsAndUnknownFields() {
        const QJsonDocument document(QJsonObject{
            {"template_id", "minimal_template"},
            {"unknown_future_field", "ignored"},
            {"sections", QJsonArray{QJsonObject{
                {"tab", "Profile"},
                {"title", "minimal section"},
                {"unknown_section_field", "ignored"},
            }}},
        });

        const DexRepoBinderTemplate::BinderTemplate templateRecord =
            DexRepoBinderTemplate::parseBinderTemplateDocument(document);

        QVERIFY(templateRecord.loaded);
        QCOMPARE(templateRecord.templateId, QString("minimal_template"));
        QCOMPARE(templateRecord.topTabs.size(), 0);
        QCOMPARE(templateRecord.contextLines.size(), 0);
        QCOMPARE(templateRecord.sections.size(), 1);
        QCOMPARE(templateRecord.sections.first().rows.size(), 0);
        QCOMPARE(templateRecord.sections.first().lenses.size(), 0);
    }

    void binderTemplateStoreFallsBackToDefault() {
        QTemporaryDir emptyDir;
        QVERIFY(emptyDir.isValid());

        const DexRepoBinderTemplate::BinderTemplateStore store =
            DexRepoBinderTemplate::loadBinderTemplateStore(emptyDir.path());
        const DexRepoBinderTemplate::BinderTemplate resolved =
            DexRepoBinderTemplate::resolveTemplateForProject(store, "missing_template");

        QCOMPARE(resolved.templateId, QString("repo_default_binder_v1"));
    }

    void selectedProjectResolvesExpectedBinderTemplate() {
        const QJsonDocument registryDocument(QJsonObject{{"projects", QJsonArray{
            QJsonObject{{"project_id", "plain"}, {"name", "Plain Repo"}},
            QJsonObject{
                {"project_id", "custom"},
                {"name", "Custom"},
                {"binder_template", "custom_binder_v1"},
                {"worker_ids", QJsonArray{"organizer", "planner", "stager"}},
            },
        }}});
        const DexProjects::ProjectRegistry registry = DexProjects::parseProjectRegistryDocument(registryDocument);

        const DexProjects::ProjectRegistryEntry *plain = DexProjects::findProjectById(registry.projects, "plain");
        const DexProjects::ProjectRegistryEntry *custom = DexProjects::findProjectById(registry.projects, "custom");
        QVERIFY(plain != nullptr);
        QVERIFY(custom != nullptr);
        QCOMPARE(plain->binderTemplate, QString());
        QCOMPARE(custom->binderTemplate, QString("custom_binder_v1"));
        QCOMPARE(custom->workerIds, QStringList({"organizer", "planner", "stager"}));
    }

    void projectRegistryParsesWorkerCatalog() {
        const QJsonDocument registryDocument(QJsonObject{
            {"workers", QJsonArray{
                QJsonObject{
                    {"worker_id", "organizer"},
                    {"role", "repo_organizer"},
                    {"display_name", "Organizer"},
                    {"status", "ready"},
                },
            }},
            {"projects", QJsonArray{}},
        });
        const DexProjects::ProjectRegistry registry = DexProjects::parseProjectRegistryDocument(registryDocument);

        QVERIFY(registry.loaded);
        QCOMPARE(registry.workers.size(), 1);
        QCOMPARE(registry.workers.first().workerId, QString("organizer"));
        QCOMPARE(registry.workers.first().role, QString("repo_organizer"));
        QCOMPARE(registry.workers.first().displayName, QString("Organizer"));
        QCOMPARE(registry.workers.first().status, QString("ready"));
    }

    void projectRegistrySaveRoundTripsWorkersAndProjects() {
        QTemporaryDir dir;
        QVERIFY(dir.isValid());

        DexProjects::ProjectRegistry registry;
        registry.loaded = true;
        registry.sourcePath = dir.filePath("projects.json");
        registry.workers.push_back({"organizer", "repo_organizer", "Organizer", "ready"});
        DexProjects::ProjectRegistryEntry project;
        project.projectId = "sample";
        project.name = "Sample";
        project.path = "/tmp/sample";
        project.role = "sample binder";
        project.status = "active";
        project.pinned = true;
        project.workerIds = {"organizer"};
        registry.projects.push_back(project);

        QString error;
        QVERIFY2(DexProjects::saveProjectRegistryFile(registry, &error), qPrintable(error));
        const DexProjects::ProjectRegistry loaded = DexProjects::loadProjectRegistryFile(registry.sourcePath);

        QVERIFY2(loaded.loaded, qPrintable(loaded.error));
        QCOMPARE(loaded.workers.size(), 1);
        QCOMPARE(loaded.workers.first().workerId, QString("organizer"));
        QCOMPARE(loaded.projects.size(), 1);
        QCOMPARE(loaded.projects.first().projectId, QString("sample"));
        QCOMPARE(loaded.projects.first().workerIds, QStringList{"organizer"});
    }

    void projectRegistrySpecDraftRoundTripsProjectFields() {
        DexProjects::ProjectRegistry registry;
        registry.loaded = true;
        DexProjects::ProjectRegistrySpecDraft draft =
            DexProjects::projectRegistrySpecDraft(registry, QString());
        draft.project.projectId = "sample";
        draft.project.name = "Sample";
        draft.project.path = "/tmp/sample";
        draft.project.role = "sample binder";
        draft.project.status = "active";
        draft.project.authority = "sandbox";
        draft.project.projectType = "sample_project";
        draft.project.pinned = true;
        draft.project.binderTemplate = "custom_binder_v1";
        draft.project.safeEditZones = {"artifacts/**"};
        draft.project.protectedZones = {"README.md", "CMakeLists.txt"};
        draft.project.generatedZones = {"target/**"};
        draft.project.scratchZones = {".dex/**"};
        draft.project.sourceDocs = {"README.md"};
        draft.project.buildCommands = {"cmake --build build"};
        draft.project.testCommands = {"ctest --test-dir build"};
        draft.project.proofCommands = {"ctest --test-dir build --output-on-failure"};
        draft.project.editorCoreStatus = "reserved";
        draft.project.editorCorePath = "reserved-adapter";
        draft.project.fileInspectSupported = false;
        draft.project.fileEditSupported = false;

        const DexProjects::ProjectRegistry next =
            DexProjects::applyProjectRegistrySpecDraft(registry, draft);

        QCOMPARE(next.projects.size(), 1);
        const DexProjects::ProjectRegistryEntry project = next.projects.first();
        QCOMPARE(project.projectId, QString("sample"));
        QCOMPARE(project.safeEditZones, QStringList{"artifacts/**"});
        QCOMPARE(project.protectedZones, QStringList({"README.md", "CMakeLists.txt"}));
        QCOMPARE(project.buildCommands, QStringList{"cmake --build build"});
        QCOMPARE(project.binderTemplate, QString("custom_binder_v1"));
    }

    void blankProjectRegistrySpecDraftHasNoProjectDefaults() {
        DexProjects::ProjectRegistry registry;
        registry.loaded = true;
        const DexProjects::ProjectRegistrySpecDraft draft =
            DexProjects::projectRegistrySpecDraft(registry, QString());

        QCOMPARE(draft.project.projectId, QString());
        QCOMPARE(draft.project.name, QString());
        QCOMPARE(draft.project.path, QString());
        QCOMPARE(draft.project.status, QString());
        QCOMPARE(draft.project.authority, QString());
        QCOMPARE(draft.project.projectType, QString());
        QVERIFY(!draft.project.pinned);
        QCOMPARE(draft.workers.size(), 0);
    }

    void projectRegistrySpecSaveWithoutWorkersAddsNoWorkerRows() {
        DexProjects::ProjectRegistry registry;
        registry.loaded = true;
        DexProjects::ProjectRegistrySpecDraft draft =
            DexProjects::projectRegistrySpecDraft(registry, QString());
        draft.project.projectId = "sample";
        draft.project.name = "Sample";

        const DexProjects::ProjectRegistry next =
            DexProjects::applyProjectRegistrySpecDraft(registry, draft);

        QCOMPARE(next.projects.size(), 1);
        QCOMPARE(next.projects.first().workerIds.size(), 0);
        QCOMPARE(next.workers.size(), 0);
    }

    void projectRegistrySpecWorkersUpdateCatalogAndProjectIds() {
        DexProjects::ProjectRegistry registry;
        registry.loaded = true;
        DexProjects::ProjectRegistrySpecDraft draft =
            DexProjects::projectRegistrySpecDraft(registry, QString());
        draft.project.projectId = "sample";
        draft.project.name = "Sample";
        draft.workers.push_back({"curator", "artifact_curator", "Artifact Curator", "ready"});
        draft.workers.push_back({"builder", "artifact_builder", "Artifact Builder", "hold"});

        const DexProjects::ProjectRegistry next =
            DexProjects::applyProjectRegistrySpecDraft(registry, draft);

        QCOMPARE(next.projects.first().workerIds, QStringList({"curator", "builder"}));
        QCOMPARE(next.workers.size(), 2);
        QCOMPARE(next.workers.at(0).workerId, QString("curator"));
        QCOMPARE(next.workers.at(0).role, QString("artifact_curator"));
        QCOMPARE(next.workers.at(1).displayName, QString("Artifact Builder"));
    }

    void projectRegistrySpecRemovedWorkerLeavesCatalogOnlyWhenShared() {
        DexProjects::ProjectRegistry registry;
        registry.loaded = true;
        registry.workers = {
            {"curator", "artifact_curator", "Artifact Curator", "ready"},
            {"shared", "shared_role", "Shared Worker", "ready"},
        };
        DexProjects::ProjectRegistryEntry sample;
        sample.projectId = "sample";
        sample.name = "Sample";
        sample.workerIds = {"curator", "shared"};
        DexProjects::ProjectRegistryEntry other;
        other.projectId = "other";
        other.name = "other";
        other.workerIds = {"shared"};
        registry.projects = {sample, other};

        DexProjects::ProjectRegistrySpecDraft draft =
            DexProjects::projectRegistrySpecDraft(registry, "sample");
        draft.workers = {{"shared", "shared_role", "Shared Worker", "ready"}};

        const DexProjects::ProjectRegistry next =
            DexProjects::applyProjectRegistrySpecDraft(registry, draft);

        QCOMPARE(next.projects.first().workerIds, QStringList{"shared"});
        QCOMPARE(next.workers.size(), 1);
        QCOMPARE(next.workers.first().workerId, QString("shared"));
    }

    void projectScopedWorkersUseRegistryWorkerIds() {
        CockpitState state;
        state.projectRegistryLoaded = true;
        DexProjects::ProjectRegistryEntry sample;
        sample.projectId = "sample";
        sample.name = "Sample";
        sample.workerIds = {"organizer", "stager"};
        state.registryProjects = {sample};
        state.workers = {
            {"organizer", "organizer", "Organizer", "ready"},
            {"planner", "planner", "Planner", "ready"},
            {"stager", "stager", "Stager", "hold"},
        };

        const QVector<WorkerSummary> workers = workersForProject(state, "sample");
        QCOMPARE(workers.size(), 2);
        QCOMPARE(workers.at(0).id, QString("organizer"));
        QCOMPARE(workers.at(1).id, QString("stager"));
    }

    void loadedProjectRegistryDoesNotInjectBackendWorkersWhenUnset() {
        CockpitState state;
        state.projectRegistryLoaded = true;
        DexProjects::ProjectRegistryEntry plain;
        plain.projectId = "plain";
        plain.name = "Plain Repo";
        state.registryProjects = {plain};
        state.workers = {
            {"planner", "planner", "Planner", "ready"},
            {"builder", "builder", "Builder", "busy"},
        };

        const QVector<WorkerSummary> workers = workersForProject(state, "plain");
        QCOMPARE(workers.size(), 0);
    }

    void missingProjectRegistryCanStillFallbackToBackendWorkersWhenUnset() {
        CockpitState state;
        state.projectRegistryLoaded = false;
        ProjectSummary plain;
        plain.id = "plain";
        plain.name = "Plain Repo";
        state.projects = {plain};
        state.workers = {
            {"planner", "planner", "Planner", "ready"},
            {"builder", "builder", "Builder", "busy"},
        };

        const QVector<WorkerSummary> workers = workersForProject(state, "plain");
        QCOMPARE(workers.size(), 2);
        QCOMPARE(workers.at(0).id, QString("planner"));
        QCOMPARE(workers.at(1).id, QString("builder"));
    }

    void registryWorkerIdsWithoutCatalogStillRenderStatefulPlaceholders() {
        CockpitState state;
        state.projectRegistryLoaded = true;
        DexProjects::ProjectRegistryEntry plain;
        plain.projectId = "plain";
        plain.name = "Plain Repo";
        plain.workerIds = {"organizer"};
        state.registryProjects = {plain};
        state.workers = {
            {"planner", "planner", "Planner", "ready"},
        };

        const QVector<WorkerSummary> workers = workersForProject(state, "plain");
        QCOMPARE(workers.size(), 1);
        QCOMPARE(workers.at(0).id, QString("organizer"));
        QCOMPARE(workers.at(0).role, QString("organizer"));
        QCOMPARE(workers.at(0).status, QString("unknown"));
    }

    void emptyProjectRegistryParsesAsLoadedBlankSlate() {
        const QJsonDocument registryDocument(QJsonObject{{"projects", QJsonArray{}}});
        const DexProjects::ProjectRegistry registry = DexProjects::parseProjectRegistryDocument(registryDocument);

        QVERIFY(registry.loaded);
        QCOMPARE(registry.projects.size(), 0);
    }

    void loadedEmptyProjectRegistryDoesNotFallbackToRustProjects() {
        CockpitState state;
        ProjectSummary rustProject;
        rustProject.id = "draftsman";
        rustProject.name = "Draftsman";
        state.projects.push_back(rustProject);
        state.projectRegistryLoaded = true;
        state.registryProjects.clear();

        const QVector<DexProjects::ProjectRegistryEntry> projects = registryProjectsForState(state);
        QCOMPARE(projects.size(), 0);
    }

    void missingProjectRegistryStillFallsBackToRustProjects() {
        CockpitState state;
        ProjectSummary rustProject;
        rustProject.id = "draftsman";
        rustProject.name = "Draftsman";
        state.projects.push_back(rustProject);
        state.projectRegistryLoaded = false;

        const QVector<DexProjects::ProjectRegistryEntry> projects = registryProjectsForState(state);
        QCOMPARE(projects.size(), 1);
        QCOMPARE(projects.first().projectId, QString("draftsman"));
    }

    void parsesBinderStateRecordsAndIgnoresUnknownFields() {
        QJsonObject root;
        root["selected_worker_id"] = "builder";
        root["unknown_future_field"] = "ignored";
        root["stats_snapshot"] = QJsonObject{
            {"worker_id", "builder"},
            {"worker_role", "builder"},
            {"generated_from", QJsonObject{{"grade_records", 1}, {"transcript_records", 1}, {"evidence_records", 1}}},
            {"routing_summary", QJsonObject{{"current_trust", "conditional"}, {"best_scope", "bounded_execution"}, {"weak_scope", "visual_teardown"}, {"sample_size", 1}, {"confidence", "medium"}}},
            {"outcome_by_task_shape", QJsonArray{QJsonObject{{"task_shape", "bounded_ui_primitive"}, {"runs", 1}, {"pass", 1}, {"partial", 0}, {"fail", 0}, {"recommended_route", QJsonArray{"builder"}}, {"confidence", "low"}}}},
            {"model_fit", QJsonArray{QJsonObject{{"model_id", "fast-code"}, {"best_task_shapes", QJsonArray{"bounded_ui_primitive"}}, {"weak_task_shapes", QJsonArray{"visual_teardown"}}}}},
            {"input_quality_impact", QJsonArray{QJsonObject{{"input_condition", "complete_packet"}, {"runs", 1}, {"observed_outcome", "high_pass_rate"}}}},
            {"evidence_health", QJsonObject{{"graded_runs", 1}, {"runs_with_proof_refs", 1}, {"data_confidence", "medium"}}},
            {"routing_recommendation", QJsonObject{{"default_route", "builder_direct"}, {"require_planner_when", QJsonArray{"ambiguity"}}, {"require_reviewer_when", QJsonArray{"proof_gap"}}, {"block_builder_when", QJsonArray{"no_stop_condition"}}}},
        };
        root["grade_records"] = QJsonArray{QJsonObject{
            {"record_id", "grade-1"},
            {"timestamp", "2026-05-15T23:45:00-06:00"},
            {"worker_id", "builder"},
            {"worker_role", "builder"},
            {"model_profile", QJsonObject{{"model_id", "fast-code"}}},
            {"task_scope", QJsonObject{{"task_shape", "bounded_ui_primitive"}}},
            {"input_quality", QJsonObject{{"input_verdict", "complete_packet"}}},
            {"adjusted_verdict", QJsonObject{{"worker_fault", "low"}}},
            {"scores", QJsonObject{{"scope_control", "B"}}},
            {"evidence", QJsonObject{{"proof_refs", QJsonArray{"proof.png"}}}},
            {"routing_decision", QJsonObject{{"routing_verdict", "builder_direct"}}},
            {"learned_rules", QJsonArray{QJsonObject{{"rule_id", "bounded_builder"}}}},
            {"final_grade", "B"},
            {"outcome", "pass"},
            {"grade_confidence", "medium"},
        }};
        root["transcript_records"] = QJsonArray{QJsonObject{
            {"record_id", "transcript-1"},
            {"session_id", "session-1"},
            {"worker_id", "builder"},
            {"worker_role", "builder"},
            {"model_id", "fast-code"},
            {"source_type", "chat_thread"},
            {"record_status", "partial"},
            {"time_metadata", QJsonObject{{"local_timestamp", "2026-05-15T23:43:40-06:00"}, {"timezone", "America/Regina"}}},
            {"environment_metadata", QJsonObject{{"tool_access_state", "partial"}}},
            {"segments", QJsonArray{QJsonObject{{"segment_id", "seg_001"}, {"raw_excerpt", "raw evidence"}}}},
            {"tool_receipts", QJsonArray{QJsonObject{{"receipt_id", "tool_001"}, {"result", "passed"}}}},
            {"artifact_refs", QJsonObject{{"screenshots", QJsonArray{"after.png"}}}},
            {"missing_data", QJsonArray{"exact_model_id"}},
            {"collection_warnings", QJsonArray{"visual_evidence_partial"}},
        }};
        root["evidence_records"] = QJsonArray{QJsonObject{
            {"record_id", "evidence-1"},
            {"session_id", "session-1"},
            {"worker_id", "builder"},
            {"evidence_id", "ev_001"},
            {"proof_refs", QJsonArray{QJsonObject{{"type", "screenshot"}, {"path", "after.png"}, {"status", "attached"}}}},
            {"failure", QJsonObject{{"failure_id", "fail_001"}, {"type", "scope_drift"}, {"severity", "high"}}},
            {"correction", QJsonObject{{"correction_id", "corr_001"}, {"status", "accepted"}}},
            {"acceptance", QJsonObject{{"accepted", true}}},
        }};
        root["profiles"] = QJsonArray{QJsonObject{{"profile_id", "p1"}, {"name", "Profile One"}, {"roles", QJsonArray{QJsonObject{{"role_id", "builder"}, {"label", "Builder"}, {"role_kind", "builder"}}}}}};
        root["selected_profile_id"] = "p1";
        root["launch_plan_preview"] = QJsonObject{{"count", 1}};

        const DexBinder::BinderState binder = DexBinder::parseBinderState(root);
        QCOMPARE(binder.statsSnapshot.sampleSize, 1);
        QCOMPARE(binder.statsSnapshot.confidence, QString("medium"));
        QCOMPARE(binder.gradeRecords.size(), 1);
        QCOMPARE(binder.gradeRecords.first().routingVerdict, QString("builder_direct"));
        QCOMPARE(binder.transcriptRecords.size(), 1);
        QCOMPARE(binder.transcriptRecords.first().missingData.size(), 1);
        QCOMPARE(binder.evidenceRecords.size(), 1);
        QCOMPARE(binder.evidenceRecords.first().acceptedState, QString("true"));
        QCOMPARE(binder.profileState.name, QString("Profile One"));
    }

    void parserHandlesMissingStatsAndEmptyArrays() {
        QJsonObject root;
        root["selected_worker_id"] = "builder";
        root["grade_records"] = QJsonArray{};
        root["transcript_records"] = QJsonArray{};
        root["evidence_records"] = QJsonArray{};

        const DexBinder::BinderState binder = DexBinder::parseBinderState(root);
        QCOMPARE(binder.statsSnapshot.confidence, QString("no_data"));
        QCOMPARE(binder.statsSnapshot.sampleSize, 0);
        QCOMPARE(binder.gradeRecords.size(), 0);
        QCOMPARE(binder.transcriptRecords.size(), 0);
        QCOMPARE(binder.evidenceRecords.size(), 0);
        QVERIFY(!binder.profileState.present);
    }
};

QTEST_MAIN(StateBackendSmoke)
#include "state_backend_smoke.moc"
