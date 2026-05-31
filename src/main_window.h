#pragma once

#include <QMainWindow>
#include <QString>

#include "app_state.h"
#include "rust_cockpit_backend.h"

class DetailLensRail;
class QLabel;
class LedgerView;
class ProjectRail;
class QToolBar;
class QToolButton;
class RightContextPanel;
class SheetStackBody;

class DraftsmanWindow final : public QMainWindow {
public:
    DraftsmanWindow(QString repoRoot, QString binaryPath, QString projectRegistryPath, QString proofReceiptPath);

    void setProjectRailVisible(bool visible);
    void setRightContextVisible(bool visible);
    void setTopTab(const QString &tabName);
    void setDetailLens(const QString &lensName);
    void setRepoBinderMode(bool enabled);
    void setSelectedWorker(const QString &workerId);
    void setSelectedProject(const QString &projectId);
    void setSettingsMode(bool enabled);

private:
    static QString resolveProjectRegistryPath(const QString &requestedPath);
    static QString resolveShellLayoutPath();
    static QString resolveProofReceiptPath(const QString &requestedPath);
    static QString resolveBinderTemplateDirPath(const QString &projectRegistryPath);
    static QString resolvePromotionReportPath();

    void buildToolbar();
    void buildBody();
    void reloadState();
    void loadProjectRegistryIntoState();
    void loadShellLayoutIntoState();
    void loadBinderTemplatesIntoState();
    void loadProofReceiptIntoState();
    void loadPromotionReportIntoState();
    void refreshSelectedWorkerStats();
    void refreshSelectedProjectScan();
    void refreshViews();
    void syncSelectedWorkerToSelectedProject();
    void exitSettingsMode();
    void saveProjectRegistryFromSettings(DexProjects::ProjectRegistry registry, const QString &selectedProjectId, bool exitAfterSave = false);
    void saveShellLayoutFromSettings(DraftsmanShell::ShellLayout layout, bool exitAfterSave = false);
    void revertProjectRegistrySettings();
    void addGradeRecord();
    void addReviewVerdict(const QString &decision);
    void appendLedgerAction(
        const QString &kind,
        const QString &status,
        const QString &grade,
        const QString &decision,
        const QString &summary);

    RustCockpitBackend backend_;
    CockpitState state_;
    QString selectedWorkerId_;
    QString selectedProjectId_;
    QString projectRegistryPath_;
    QString shellLayoutPath_;
    QString binderTemplateDirPath_;
    QString proofReceiptPath_;
    QString promotionReportPath_;
    bool repoMode_ = true;
    bool settingsMode_ = false;
    QString selectedTopTab_ = "Profile";
    QString selectedDetailLens_ = "Dashboard";
    SheetStackBody *body_ = nullptr;
    ProjectRail *projectRail_ = nullptr;
    LedgerView *ledger_ = nullptr;
    DetailLensRail *detailLensRail_ = nullptr;
    RightContextPanel *rightContext_ = nullptr;
    QToolBar *bottomHost_ = nullptr;
    QLabel *bottomShelf_ = nullptr;
    QLabel *appTitleLabel_ = nullptr;
    QLabel *chromeLocationLabel_ = nullptr;
    QToolButton *railToggle_ = nullptr;
    QToolButton *bottomToggle_ = nullptr;
    QToolButton *repoModeToggle_ = nullptr;
    QToolButton *contextToggle_ = nullptr;
};
