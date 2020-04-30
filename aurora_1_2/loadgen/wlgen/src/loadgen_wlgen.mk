# This file is to be included in the top-level Makefile.
# This file is owned by the integration lead programmer, but *tentative* changes
# may be made by others.

WLGEN_LIB_OBJECTS=\
	$(WLGEN_BASE_DIR)/src/AggStreamSource.o \
	$(WLGEN_BASE_DIR)/src/AggSourceFileReader.o \
	$(WLGEN_BASE_DIR)/src/RandomNumGenerator.o \
	$(WLGEN_BASE_DIR)/src/DataType.o \
	$(WLGEN_BASE_DIR)/src/AttributeSource.o \
	$(WLGEN_BASE_DIR)/src/StreamTuple.o \
	$(WLGEN_BASE_DIR)/src/TimeStampGenerator.o \
	$(WLGEN_BASE_DIR)/src/SingleSource.o \
	$(WLGEN_BASE_DIR)/src/SourcePool.o \
	$(WLGEN_BASE_DIR)/src/SourceGenerator.o \
	$(WLGEN_BASE_DIR)/src/StreamSender.o \
	$(WLGEN_BASE_DIR)/src/StreamBuffer.o \
	$(WLGEN_BASE_DIR)/src/WGController.o \
	$(WLGEN_BASE_DIR)/src/WorkloadGenerator.o \
	$(WLGEN_BASE_DIR)/src/WGServer.o \
	$(WLGEN_BASE_DIR)/src/StreamReceiver.o \
	$(WLGEN_BASE_DIR)/src/AuroraOutputReceiver.o \

WLGEN_APP_OBJECTS= \
	$(WLGEN_BASE_DIR)/src/workloadGenerator.o

AREC_APP_OBJECTS= \
	$(WLGEN_BASE_DIR)/src/auroraOutputReceiver.o

WLREC_APP_OBJECTS= \
	$(WLGEN_BASE_DIR)/src/workloadReceiver.o 

ALL_CXX_OBJECTS += \
	$(WLGEN_LIB_OBJECTS) \
	$(WLGEN_APP_OBJECTS) \
	$(AREC_APP_OBJECTS) \
	$(WLREC_APP_OBJECTS)

$(WLGEN_LIB_SO): \
	$(WLGEN_LIB_OBJECTS)
	$(CXX) -fPIC -shared -o $@ \
	$(WLGEN_LIB_OBJECTS)

$(WLGEN_APP): \
	$(ALL_AURORA_CXX_API_SOS) \
	$(AURORA_C_API_SO) \
	$(AURORA_SOCKET_API_SO) \
	$(WLGEN_LIB_SO) \
	$(WLGEN_APP_OBJECTS)
	$(CXX) $(CFLAGS) $(INCLUDE_FLAGS) -o $@ \
	$(WLGEN_APP_OBJECTS) \
	-L$(AURORA_LIB_DIR) \
	-laurora_util \
	-laurora_storage_mgr \
	-laurora_runtime_misc \
	-laurora_predparser \
	-laurora_sched \
	-laurora_catalog_mgr \
	-laurora_load_shedder \
	-laurora_cxx \
	-laurora_c \
	-laurora_socket \
	-laurora_wlgen \
	-l pthread \
	$(BERKELEY_DB_4_LIB_LINKAGE) \
	$(XERCES_LIB_LINKAGE) \
	$(XFORMS_LIB_LINKAGE) \
	$(GLOBAL_APPLICATION_LFLAGS)

$(WLREC_APP): \
	$(ALL_AURORA_CXX_API_SOS) \
	$(AURORA_C_API_SO) \
	$(AURORA_SOCKET_API_SO) \
	$(WLGEN_LIB_SO) \
	$(WLREC_APP_OBJECTS)
	$(CXX) $(CFLAGS) $(INCLUDE_FLAGS) -o $@ \
	$(WLREC_APP_OBJECTS) \
	-L$(AURORA_LIB_DIR) \
	-laurora_util \
	-laurora_storage_mgr \
	-laurora_runtime_misc \
	-laurora_predparser \
	-laurora_sched \
	-laurora_catalog_mgr \
	-laurora_load_shedder \
	-laurora_cxx \
	-laurora_c \
	-laurora_socket \
	-laurora_wlgen \
	-l pthread \
	$(BERKELEY_DB_4_LIB_LINKAGE) \
	$(XERCES_LIB_LINKAGE) \
	$(XFORMS_LIB_LINKAGE) \
	$(GLOBAL_APPLICATION_LFLAGS)

$(AREC_APP): \
	$(ALL_AURORA_CXX_API_SOS) \
	$(AURORA_C_API_SO) \
	$(AURORA_SOCKET_API_SO) \
	$(WLGEN_LIB_SO) \
	$(AREC_APP_OBJECTS)
	$(CXX) $(CFLAGS) $(INCLUDE_FLAGS) -o $@ \
	$(AREC_APP_OBJECTS) \
	-L$(AURORA_LIB_DIR) \
	-laurora_util \
	-laurora_storage_mgr \
	-laurora_runtime_misc \
	-laurora_predparser \
	-laurora_sched \
	-laurora_catalog_mgr \
	-laurora_load_shedder \
	-laurora_cxx \
	-laurora_c \
	-laurora_socket \
	-laurora_wlgen \
	-l pthread \
	$(BERKELEY_DB_4_LIB_LINKAGE) \
	$(XERCES_LIB_LINKAGE) \
	$(XFORMS_LIB_LINKAGE) \
	$(GLOBAL_APPLICATION_LFLAGS)

clean_wlgen_lib:
	rm -f $(WLGEN_LIB_OBJECTS)
	rm -f $(WLGEN_LIB_SO)

clean_wlgen_app:
	rm -f $(WLGEN_APP_OBJECTS)
	rm -f $(WLGEN_APP)

clean_wlrec_app:
	rm -f $(WLREC_APP_OBJECTS)
	rm -f $(WLREC_APP)

clean_arec_app:
	rm -f $(AREC_APP_OBJECTS)
	rm -f $(AREC_APP)

