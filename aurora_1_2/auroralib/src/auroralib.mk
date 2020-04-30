# This file is to be included in the top-level Makefile.
# This file is owned by the integration lead programmer, but *tentative* changes
# may be made by others.

AURORA_CXX_API_OBJECTS= \
	$(AURORA_API_BASE_DIR)/src/AuroraNode.o \
	$(AURORA_API_BASE_DIR)/src/AuroraNodeImpl.o \
	$(AURORA_API_BASE_DIR)/src/SchedulerRunnable.o \
	$(AURORA_API_BASE_DIR)/src/InputRateMonitorRunnable.o

AURORA_C_API_OBJECTS= \
	$(AURORA_API_BASE_DIR)/src/aurora.o

TESTLOADGEN_APP_OBJECTS= \
	$(AURORA_API_BASE_DIR)/src/testloadgen/main.o

TEST_CAPI1_APP_OBJECTS= \
	$(AURORA_API_BASE_DIR)/src/test_capi1/main.o

RUN_A_NETWORK_APP_OBJECTS= \
	$(AURORA_API_BASE_DIR)/src/run_a_network/main.o

FILE_CLIENT_APP_OBJECTS= \
	$(AURORA_API_BASE_DIR)/src/file-client/file-client.o


ALL_CXX_OBJECTS += \
	$(AURORA_CXX_API_OBJECTS) \
	$(AURORA_C_API_OBJECTS) \
	$(TESTLOADGEN_APP_OBJECTS) \
	$(TEST_CAPI1_APP_OBJECTS) \
	$(RUN_A_NETWORK_APP_OBJECTS) \
	$(FILE_CLIENT_APP_OBJECTS)


$(AURORA_CXX_API_SO): \
	$(AURORA_CXX_API_OBJECTS)
	$(CXX) -fPIC -shared -o $@ \
	$(AURORA_CXX_API_OBJECTS)

$(AURORA_C_API_SO): \
	$(AURORA_CXX_API_OBJECTS) \
	$(AURORA_C_API_OBJECTS)
	$(CXX) -fPIC -shared -o $@ \
	$(AURORA_CXX_API_OBJECTS) \
	$(AURORA_C_API_OBJECTS)

$(TESTLOADGEN_APP): \
	$(ALL_AURORA_CXX_API_SOS) \
	$(TESTLOADGEN_APP_OBJECTS)
	$(CXX) $(CFLAGS) $(INCLUDE_FLAGS) -o $@ \
	$(TESTLOADGEN_APP_OBJECTS) \
	-L$(AURORA_LIB_DIR) \
	-laurora_util \
	-laurora_storage_mgr \
	-laurora_runtime_misc \
	-laurora_predparser \
	-laurora_sched \
	-laurora_catalog_mgr \
	-laurora_load_shedder \
	-laurora_cxx \
	-l pthread \
	$(BERKELEY_DB_4_LIB_LINKAGE) \
	$(XERCES_LIB_LINKAGE) \
	$(XFORMS_LIB_LINKAGE) \
	$(GLOBAL_APPLICATION_LFLAGS)

$(TEST_CAPI1_APP): \
	$(ALL_AURORA_C_API_SOS) \
	$(TEST_CAPI1_APP_OBJECTS)
	$(CXX) $(CFLAGS) $(INCLUDE_FLAGS) -o $@ \
	$(TEST_CAPI1_APP_OBJECTS) \
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
	-l pthread \
	$(BERKELEY_DB_4_LIB_LINKAGE) \
	$(XERCES_LIB_LINKAGE) \
	$(XFORMS_LIB_LINKAGE) \
	$(GLOBAL_APPLICATION_LFLAGS)

$(RUN_A_NETWORK_APP): \
	$(ALL_AURORA_CXX_API_SOS) \
	$(RUN_A_NETWORK_APP_OBJECTS)
	$(CXX) $(CFLAGS) $(INCLUDE_FLAGS) -o $@ \
	$(RUN_A_NETWORK_APP_OBJECTS) \
	-L$(AURORA_LIB_DIR) \
	-laurora_util \
	-laurora_storage_mgr \
	-laurora_runtime_misc \
	-laurora_predparser \
	-laurora_sched \
	-laurora_catalog_mgr \
	-laurora_load_shedder \
	-laurora_cxx \
	-l pthread \
	$(BERKELEY_DB_4_LIB_LINKAGE) \
	$(XERCES_LIB_LINKAGE) \
	$(XFORMS_LIB_LINKAGE) \
	$(GLOBAL_APPLICATION_LFLAGS)

$(FILE_CLIENT_APP): \
	$(ALL_AURORA_CXX_API_SOS) \
	$(FILE_CLIENT_APP_OBJECTS)
	$(CXX) $(CFLAGS) $(INCLUDE_FLAGS) -o $@ \
	$(FILE_CLIENT_APP_OBJECTS) \
	-L$(AURORA_LIB_DIR) \
	-laurora_util \
	-laurora_storage_mgr \
	-laurora_runtime_misc \
	-laurora_predparser \
	-laurora_sched \
	-laurora_catalog_mgr \
	-laurora_load_shedder \
	-laurora_cxx \
	-l pthread \
	$(BERKELEY_DB_4_LIB_LINKAGE) \
	$(XERCES_LIB_LINKAGE) \
	$(XFORMS_LIB_LINKAGE) \
	$(GLOBAL_APPLICATION_LFLAGS)

clean_aurora_cxx_api_lib:
	rm -f $(AURORA_CXX_API_OBJECTS)
	rm -f $(AURORA_CXX_API_SO)

clean_aurora_c_api_lib:
	rm -f $(AURORA_C_API_OBJECTS)
	rm -f $(AURORA_C_API_SO)

clean_testloadgen_app:
	rm -f $(TESTLOADGEN_APP_OBJECTS)
	rm -f $(TESTLOADGEN_APP)

clean_test_capi1_app:
	rm -f $(TEST_CAPI1_APP_OBJECTS)
	rm -f $(TEST_CAPI1_APP)

clean_file_client_app:
	rm -f $(FILE_CLIENT_APP_OBJECTS)
	rm -f $(FILE_CLIENT_APP)

clean_run_a_network_app:
	rm -f $(RUN_A_NETWORK_APP_OBJECTS)
	rm -f $(RUN_A_NETWORK_APP)

