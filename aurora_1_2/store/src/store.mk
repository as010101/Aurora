# This file is to be included in the top-level Makefile.
# This file is owned by the integration lead programmer, but *tentative* changes
# may be made by others.

STORAGE_MGR_LIB_OBJECTS=\
	$(STORAGE_MGR_BASE_DIR)/src/sm/cpp/PagePool.o \
	$(STORAGE_MGR_BASE_DIR)/src/sm/cpp/TupleQueue.o

ALL_CXX_OBJECTS += \
	$(STORAGE_MGR_LIB_OBJECTS)

$(STORAGE_MGR_LIB_SO): \
	$(STORAGE_MGR_LIB_OBJECTS)
	$(CXX) -fPIC -shared -o $@ \
	$(STORAGE_MGR_LIB_OBJECTS)

clean_storage_mgr_lib:
	rm -f $(STORAGE_MGR_LIB_OBJECTS)
	rm -f $(STORAGE_MGR_LIB_SO)
