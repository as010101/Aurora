# This file is to be included in the top-level Makefile.
# This file is owned by the integration lead programmer, but *tentative* changes
# may be made by others.

RUNTIME_MISC_LIB_OBJECTS= \
	$(RUNTIME_MISC_BASE_DIR)/src/RuntimeGlobals.o    \
	$(RUNTIME_MISC_BASE_DIR)/src/StatsImage.o        \
	$(RUNTIME_MISC_BASE_DIR)/src/DelayedDataMgr.o    \
	$(RUNTIME_MISC_BASE_DIR)/src/DataManager.o       \
	$(RUNTIME_MISC_BASE_DIR)/src/ImportDelayedData.o \
	$(RUNTIME_MISC_BASE_DIR)/src/ExportDelayedData.o

ALL_CXX_OBJECTS += \
	$(RUNTIME_MISC_LIB_OBJECTS)

$(RUNTIME_MISC_LIB_SO): \
	$(RUNTIME_MISC_LIB_OBJECTS)
	$(CXX) -fPIC -shared -o $@ \
	$(RUNTIME_MISC_LIB_OBJECTS)

clean_runtime_misc_lib:
	rm -f $(RUNTIME_MISC_LIB_OBJECTS)
	rm -f $(RUNTIME_MISC_LIB_SO)
