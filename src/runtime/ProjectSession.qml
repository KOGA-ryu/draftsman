import QtQuick

QtObject {
    id: projectSession

    property string activityMode: "blank"
    property string selectedLocalTab: "overview"
    property string selectedShelfTab: ""
    property string selectedSettingsPage: "theme"
    property bool writeDisabled: true
    property string projectProfilePath: ""
    property var projectProfileDocument: ({})
    property string projectId: "draftsman_blank"
    property string projectTitle: "Draftsman"
    property string projectType: "blank_shell"
    property string projectRootPath: ""
    property string projectSummary: "Reusable blank Draftsman shell."
    property string settingsNavLabel: "Theme and layout"
    property string mainWorkspaceFeature: "blank_canvas"
    property string rightInspectorSource: "none"
    property var leftProjectRows: []
    property var projectPanelDefaults: ({})
    property var activityModes: [
        { id: "blank", label: "Blank", icon: "B", tooltip: "Blank workspace" },
        { id: "settings", label: "Settings", icon: "S", tooltip: "Settings surface" }
    ]
    property var shelfTabs: []

    signal changed()

    function defaultActivityModes() {
        return [
            { id: "blank", label: "Blank", icon: "B", tooltip: "Blank workspace" },
            { id: "review", label: "Review", icon: "R", tooltip: "Review gate workspace" },
            { id: "settings", label: "Settings", icon: "S", tooltip: "Settings surface" },
            { id: "proof", label: "Proof", icon: "P", tooltip: "Proof and receipts" }
        ]
    }

    function asArray(value) {
        if (!value) {
            return []
        }
        if (Array.isArray(value)) {
            return value
        }
        if (typeof value.length === "number") {
            var result = []
            for (var index = 0; index < value.length; ++index) {
                result.push(value[index])
            }
            return result
        }
        return []
    }

    function normalizeActivityModes(source) {
        var modes = []
        var sourceModes = asArray(source)
        for (var index = 0; index < sourceModes.length; ++index) {
            var mode = sourceModes[index]
            if (mode && mode.enabled !== false && String(mode.id || "").length > 0) {
                modes.push({
                    id: String(mode.id),
                    label: String(mode.label || mode.id),
                    icon: String(mode.icon || String(mode.label || mode.id).charAt(0).toUpperCase()),
                    tooltip: String(mode.tooltip || mode.label || mode.id)
                })
            }
        }
        return modes.length > 0 ? modes : defaultActivityModes()
    }

    function normalizeProjectRows(source) {
        var rows = []
        var sourceRows = asArray(source)
        for (var index = 0; index < sourceRows.length; ++index) {
            var row = sourceRows[index]
            if (row && String(row.label || "").length > 0) {
                rows.push({
                    label: String(row.label),
                    meta: String(row.meta || "")
                })
            }
        }
        return rows.length > 0 ? rows : [
            { label: "Project slot", meta: "blank" },
            { label: "Scratch", meta: "workflow" },
            { label: "Final", meta: "workflow" }
        ]
    }

    function normalizeShelfTabs(source) {
        var result = []
        if (Array.isArray(source) && source.length === 0) {
            return result
        }
        var tabs = asArray(source)
        for (var index = 0; index < tabs.length; ++index) {
            var tab = String(tabs[index] || "").trim()
            if (tab.length > 0) {
                result.push(tab)
            }
        }
        return result.length > 0 ? result : ["Output", "Proof", "Receipts", "Log"]
    }

    function activityModeAvailable(modeId) {
        for (var index = 0; index < activityModes.length; ++index) {
            if (activityModes[index].id === modeId) {
                return true
            }
        }
        return false
    }

    function hasActivityMode(modeId) {
        return activityModeAvailable(modeId)
    }

    function loadProjectProfile(document) {
        var profile = document && document.profile ? document.profile : ({})
        var leftPanel = document && document.left_panel ? document.left_panel : ({})
        var mainWorkspace = document && document.main_workspace ? document.main_workspace : ({})
        var rightInspector = document && document.right_inspector ? document.right_inspector : ({})
        var bottomPanel = document && document.bottom_panel ? document.bottom_panel : ({})
        var panelDefaults = document && document.panel_defaults ? document.panel_defaults : ({})
        var writePolicy = document && document.write_policy ? document.write_policy : ({})

        projectProfileDocument = document
        projectId = String(profile.profile_id || "draftsman_blank")
        projectTitle = String(profile.label || "Draftsman")
        projectType = String(profile.type || "blank_shell")
        projectRootPath = String(profile.root_path || "")
        projectSummary = String(profile.summary || "Reusable blank Draftsman shell.")
        settingsNavLabel = String(leftPanel.settings_label || "Theme and layout")
        mainWorkspaceFeature = String(mainWorkspace.feature || "blank_canvas")
        rightInspectorSource = String(rightInspector.source || "none")
        projectPanelDefaults = panelDefaults
        writeDisabled = typeof writePolicy.writes_enabled === "boolean" ? !writePolicy.writes_enabled : true
        activityModes = normalizeActivityModes(document && document.activity_modes)
        leftProjectRows = normalizeProjectRows(leftPanel.project_rows)
        shelfTabs = normalizeShelfTabs(bottomPanel.tabs)
        selectedShelfTab = shelfTabs.length > 0 && shelfTabs.indexOf(selectedShelfTab) >= 0 ? selectedShelfTab : (shelfTabs[0] || "")
        var defaultActivity = String(profile.default_activity || activityMode)
        activityMode = activityModeAvailable(defaultActivity) ? defaultActivity : activityModes[0].id
        changed()
    }

    function setActivityMode(modeId) {
        if (!activityModeAvailable(modeId)) {
            return
        }
        activityMode = modeId
        if (modeId === "settings" && !selectedSettingsPage.length) {
            selectedSettingsPage = "theme"
        }
        changed()
    }

    function setSettingsPage(pageId) {
        selectedSettingsPage = pageId
        activityMode = "settings"
        changed()
    }
}
