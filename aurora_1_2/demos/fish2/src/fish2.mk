# This file is to be included in the top-level Makefile.
# This file is owned by the integration lead programmer, but *tentative* changes
# may be made by others.

FISH2_DEMO_INCLUDE_FLAGS= \
	-I$(FISH2_DEMO_BASE_DIR)/src \
	-I$(AURORA_INCLUDE_DIR) \
	-I$(UTIL_INCLUDE_DIR) \
	-I$(QTDIR)/include 

FISH2_DEMO_OBJECTS= \
	$(FISH2_DEMO_BASE_DIR)/src/datafile.o       \
	$(FISH2_DEMO_BASE_DIR)/src/DataFeeder.o     \
	$(FISH2_DEMO_BASE_DIR)/src/DataFileReader.o \
	$(FISH2_DEMO_BASE_DIR)/src/DeqRunnable.o    \
	$(FISH2_DEMO_BASE_DIR)/src/main.o           \
	$(FISH2_DEMO_BASE_DIR)/src/Model.o          \
	$(FISH2_DEMO_BASE_DIR)/src/ModelObserver.o  \
	$(FISH2_DEMO_BASE_DIR)/src/moc_AlertBox.o   \
	$(FISH2_DEMO_BASE_DIR)/src/moc_FishTable.o  \
	$(FISH2_DEMO_BASE_DIR)/src/moc_FishWidget.o \
	$(FISH2_DEMO_BASE_DIR)/src/moc_LabelBox.o   \
	$(FISH2_DEMO_BASE_DIR)/src/moc_WaterTable.o \
	$(FISH2_DEMO_BASE_DIR)/src/AlertBox.o       \
	$(FISH2_DEMO_BASE_DIR)/src/FishTable.o      \
	$(FISH2_DEMO_BASE_DIR)/src/FishWidget.o     \
	$(FISH2_DEMO_BASE_DIR)/src/LabelBox.o       \
	$(FISH2_DEMO_BASE_DIR)/src/WaterTable.o     \
	$(FISH2_DEMO_BASE_DIR)/src/FishObserver.o

# Override the normal compilation / linkage rules, because Qt apps require some
# extra stuff that no one else does...
MOC = $(QTDIR)/bin/moc

FISH2_MOC_HEADERS= \
	$(FISH2_DEMO_BASE_DIR)/src/AlertBox.H   \
	$(FISH2_DEMO_BASE_DIR)/src/FishTable.H  \
	$(FISH2_DEMO_BASE_DIR)/src/FishWidget.H \
	$(FISH2_DEMO_BASE_DIR)/src/LabelBox.H   \
	$(FISH2_DEMO_BASE_DIR)/src/WaterTable.H


$(FISH2_DEMO_BASE_DIR)/src/moc_AlertBox.C: $(FISH2_DEMO_BASE_DIR)/src/AlertBox.H
	$(MOC) $< -o $@

$(FISH2_DEMO_BASE_DIR)/src/moc_FishTable.C: $(FISH2_DEMO_BASE_DIR)/src/FishTable.H
	$(MOC) $< -o $@

$(FISH2_DEMO_BASE_DIR)/src/moc_FishWidget.C: $(FISH2_DEMO_BASE_DIR)/src/FishWidget.H
	$(MOC) $< -o $@

$(FISH2_DEMO_BASE_DIR)/src/moc_LabelBox.C: $(FISH2_DEMO_BASE_DIR)/src/LabelBox.H
	$(MOC) $< -o $@

$(FISH2_DEMO_BASE_DIR)/src/moc_WaterTable.C: $(FISH2_DEMO_BASE_DIR)/src/WaterTable.H
	$(MOC) $< -o $@


ALL_CXX_OBJECTS += \
	$(FISH2_DEMO_OBJECTS)

# Use a custom compilation rule for this subproject, because it needs different
# #include flags than are needed for building aurora.
$(FISH2_DEMO_BASE_DIR)/src/%.o: $(FISH2_DEMO_BASE_DIR)/src/%.C
	@mkdir -p $(@D)/.deps
	$(CXX) $(CFLAGS) $(FISH2_DEMO_INCLUDE_FLAGS) -c $< -o $@ \
	-MT $@ -MMD -MF $(@D)/.deps/$(basename $(@F)).d

$(FISH2_DEMO_APP): \
	$(ALL_AURORA_CXX_API_SOS) \
	$(FISH2_DEMO_OBJECTS)
	$(CXX) $(CFLAGS) $(INCLUDE_FLAGS) -o $@ \
	$(FISH2_DEMO_OBJECTS) \
	$(ALL_AURORA_CXX_API_SOS) \
	-l pthread \
	$(BERKELEY_DB_4_LIB_LINKAGE) \
	$(XERCES_LIB_LINKAGE) \
	$(XFORMS_LIB_LINKAGE) \
	-L$(QTDIR)/lib \
	-Xlinker -R$(QTDIR)/lib \
	-lqt-mt \
	$(GLOBAL_APPLICATION_LFLAGS)

clean_fish2_demo: 
	rm -f $(FISH2_DEMO_OBJECTS)
	rm -f $(FISH2_DEMO_APP)
