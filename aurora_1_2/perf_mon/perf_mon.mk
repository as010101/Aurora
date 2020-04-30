# This file is to be included in the top-level Makefile.
# This file is owned by the integration lead programmer, but *tentative* changes
# may be made by others.

PERFMON_INCLUDE_FLAGS:= \
	-I$(PERFMON_BASE_DIR) \
	-I$(AURORA_INCLUDE_DIR) \
	-I$(UTIL_INCLUDE_DIR) \
	-I$(CATALOG_MGR_INCLUDE_DIR) \
	-I$(PREDPARSER_INCLUDE_DIR) \
	-I$(PREDPARSER_INCLUDE_DIR)/PredicatesAndExpressions \
	-I$(SCHED_INCLUDE_DIR) \
	-I$(RUNTIME_MISC_INCLUDE_DIR) \
	-I$(PERFMON_BASE_DIR) \
	-I$(PERFMON_BASE_DIR)/network \
	-I$(QTDIR)/include 


# Use a custom compilation rule for this subproject, because it needs different
# #include flags than are needed for building aurora.
$(PERFMON_BASE_DIR)/%.o: $(PERFMON_BASE_DIR)/%.C
	@mkdir -p $(@D)/.deps
	$(CXX) $(CFLAGS) $(PERFMON_INCLUDE_FLAGS) -c $< -o $@ \
	-MT $@ -MMD -MF $(@D)/.deps/$(basename $(@F)).d

PERFMON_CLIENT_APP_OBJECTS= \
	$(PERFMON_BASE_DIR)/main.o \
	$(PERFMON_BASE_DIR)/LoadShedPage.o \
	$(PERFMON_BASE_DIR)/NetworkGuiPage.o \
	$(PERFMON_BASE_DIR)/QueryArc.o \
	$(PERFMON_BASE_DIR)/QueryBox.o \
	$(PERFMON_BASE_DIR)/LSGraph.o \
	$(PERFMON_BASE_DIR)/LoadShedWidget.o \
	$(PERFMON_BASE_DIR)/MainWindow.o \
	$(PERFMON_BASE_DIR)/QOSMonitor.o

PERFMON_SERVER_APP_OBJECTS= \
	$(PERFMON_BASE_DIR)/network/server.o

PERFMON_QTOBJECTS= \
	$(PERFMON_BASE_DIR)/LSGraph.H \
	$(PERFMON_BASE_DIR)/LoadShedWidget.H \
	$(PERFMON_BASE_DIR)/MainWindow.H \
	$(PERFMON_BASE_DIR)/QOSMonitor.H \
	$(PERFMON_BASE_DIR)/LoadShedPage.H

PERFMON_MOC_C_FILES= \
	$(PERFMON_BASE_DIR)/moc_LSGraph.C \
	$(PERFMON_BASE_DIR)/moc_LoadShedWidget.C \
	$(PERFMON_BASE_DIR)/moc_MainWindow.C \
	$(PERFMON_BASE_DIR)/moc_QOSMonitor.C \
	$(PERFMON_BASE_DIR)/moc_LoadShedPage.C

PERFMON_MOC_OBJECTS= \
	$(PERFMON_BASE_DIR)/moc_LSGraph.o \
	$(PERFMON_BASE_DIR)/moc_LoadShedWidget.o \
	$(PERFMON_BASE_DIR)/moc_MainWindow.o \
	$(PERFMON_BASE_DIR)/moc_QOSMonitor.o \
	$(PERFMON_BASE_DIR)/moc_LoadShedPage.o

ALL_CXX_OBJECTS += \
	$(PERFMON_CLIENT_APP_OBJECTS)

# Override the normal compilation / linkage rules, because Qt apps require some
# extra stuff that no one else does...
MOC = $(QTDIR)/bin/moc

$(PERFMON_BASE_DIR)/moc_LSGraph.C: $(PERFMON_BASE_DIR)/LSGraph.H
	$(MOC) $< -o $@

$(PERFMON_BASE_DIR)/moc_LoadShedWidget.C: $(PERFMON_BASE_DIR)/LoadShedWidget.H
	$(MOC) $< -o $@

$(PERFMON_BASE_DIR)/moc_MainWindow.C: $(PERFMON_BASE_DIR)/MainWindow.H
	$(MOC) $< -o $@

$(PERFMON_BASE_DIR)/moc_QOSMonitor.C: $(PERFMON_BASE_DIR)/QOSMonitor.H
	$(MOC) $< -o $@

$(PERFMON_BASE_DIR)/moc_LoadShedPage.C: $(PERFMON_BASE_DIR)/LoadShedPage.H
	$(MOC) $< -o $@

$(PERFMON_CLIENT_APP): \
	$(ALL_AURORA_CXX_API_SOS) \
	$(PERFMON_MOC_OBJECTS) \
	$(PERFMON_CLIENT_APP_OBJECTS)
	$(CXX) -o $@ \
	$(PERFMON_CLIENT_APP_OBJECTS) \
	$(PERFMON_MOC_OBJECTS) \
	-L$(AURORA_LIB_DIR) \
	-laurora_util \
	-laurora_storage_mgr \
	-laurora_runtime_misc \
	-laurora_predparser \
	-laurora_sched \
	-laurora_catalog_mgr \
	-laurora_load_shedder \
	-laurora_cxx \
	-L$(QTDIR)/lib \
	-Xlinker -R$(QTDIR)/lib \
	-lqt-mt \
	-l pthread \
	$(BERKELEY_DB_4_LIB_LINKAGE) \
	$(XERCES_LIB_LINKAGE) \
	$(XFORMS_LIB_LINKAGE)

$(PERFMON_SERVER_APP): \
	$(ALL_AURORA_CXX_API_SOS) \
	$(PERFMON_SERVER_APP_OBJECTS) \
	$(PERFMON_MOC_OBJECTS)
	$(CXX) -o $@ \
	$(PERFMON_SERVER_APP_OBJECTS) \
	$(PERFMON_MOC_OBJECTS) \
	-L$(AURORA_LIB_DIR) \
	-laurora_util \
	-laurora_storage_mgr \
	-laurora_runtime_misc \
	-laurora_predparser \
	-laurora_sched \
	-laurora_catalog_mgr \
	-laurora_load_shedder \
	-laurora_cxx \
	-Xlinker -R$(QTDIR)/lib \
	-lqt-mt \
	-l pthread \
	$(BERKELEY_DB_4_LIB_LINKAGE) \
	$(XERCES_LIB_LINKAGE) \
	$(XFORMS_LIB_LINKAGE)

clean_perfmon_client_app:
	rm -f $(PERFMON_CLIENT_APP_OBJECTS)
	rm -f $(PERFMON_MOC_OBJECTS)
	rm -f $(PERFMON_CLIENT_APP)
