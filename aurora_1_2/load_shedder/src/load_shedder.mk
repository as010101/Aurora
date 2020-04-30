# This file is to be included in the top-level Makefile.
# This file is owned by the integration lead programmer, but *tentative* changes
# may be made by others.

LOAD_SHEDDER_LIB_OBJECTS= \
	$(LOAD_SHEDDER_BASE_DIR)/src/AccuracyQoS.o \
	$(LOAD_SHEDDER_BASE_DIR)/src/DropLocations.o \
	$(LOAD_SHEDDER_BASE_DIR)/src/LSRM.o \
	$(LOAD_SHEDDER_BASE_DIR)/src/LoadShedder.o

ALL_CXX_OBJECTS += \
	$(LOAD_SHEDDER_LIB_OBJECTS)

$(LOAD_SHEDDER_LIB_SO): \
	$(LOAD_SHEDDER_LIB_OBJECTS)
	$(CXX) -fPIC -shared -o $@ \
	$(LOAD_SHEDDER_LIB_OBJECTS)

clean_load_shedder_lib:
	rm -f $(LOAD_SHEDDER_LIB_OBJECTS)
	rm -f $(LOAD_SHEDDER_LIB_SO)
