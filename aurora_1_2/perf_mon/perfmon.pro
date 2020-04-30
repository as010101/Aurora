UTIL_BASE_DIR=$$(AURORA_SRC_DIR)/util
RUNTIME_MISC_BASE_DIR=$$(AURORA_SRC_DIR)/runtime_misc
PREDPARSER_BASE_DIR=$$(AURORA_SRC_DIR)/predparser
CATALOG_MGR_BASE_DIR=$$(AURORA_SRC_DIR)/catalogmgr
STORAGE_MGR_BASE_DIR=$$(AURORA_SRC_DIR)/store
SCHED_BASE_DIR=$$(AURORA_SRC_DIR)/sched
GUI_BASE_DIR=$$(AURORA_SRC_DIR)/gui
AURORA_API_BASE_DIR=$$(AURORA_SRC_DIR)/auroralib
UTIL_INCLUDE_DIR=$$UTIL_BASE_DIR/include
RUNTIME_MISC_INCLUDE_DIR=$$RUNTIME_MISC_BASE_DIR/include
PREDPARSER_INCLUDE_DIR=$$PREDPARSER_BASE_DIR/src
PREDPARSER_INCLUDE_DIR2=$$PREDPARSER_BASE_DIR/src/PredicatesAndExpressions
CATALOG_MGR_INCLUDE_DIR=$$CATALOG_MGR_BASE_DIR/src
STORAGE_MGR_INCLUDE_DIR=$$STORAGE_MGR_BASE_DIR/src
SCHED_INCLUDE_DIR=$$SCHED_BASE_DIR/src
AURORA_INCLUDE_DIR=$$AURORA_API_BASE_DIR/include

SOURCES	+= main.C LoadShedWidget.C MainWindow.C QOSMonitor.C LoadShedPage.C LSGraph.C NetworkGuiPage.C QueryBox.C QueryArc.C DataManager.C CatalogManager.C QueryNetwork.C
HEADERS	+= LoadShedWidget.H MainWindow.H QOSMonitor.H GUIConstants.H LoadShedPage.H LSGraph.H NetworkGuiPage.H QueryBox.H QueryArc.H DataManager.H CatalogManager.H QueryNetwork.H
TEMPLATE=app
CONFIG	+= qt warn_on debug
LANGUAGE	= C++
INCLUDEPATH	+= statsSpoof $$UTIL_INCLUDE_DIR $$RUNTIME_MISC_INCLUDE_DIR $$PREDPARSER_INCLUDE_DIR $$PREDPARSER_INCLUDE_DIR2 $$CATALOG_MGR_INCLUDE_DIR $$STORAGE_MGR_INCLUDE_DIR $$SCHED_INCLUDE_DIR $$AURORA_INCLUDE_DIR
DEPENDPATH      += statsSpoof $$UTIL_INCLUDE_DIR $$RUNTIME_MISC_INCLUDE_DIR $$PREDPARSER_INCLUDE_DIR $$PREDPARSER_INCLUDE_DIR2 $$CATALOG_MGR_INCLUDE_DIR $$STORAGE_MGR_INCLUDE_DIR $$SCHED_INCLUDE_DIR $$AURORA_INCLUDE_DIR
