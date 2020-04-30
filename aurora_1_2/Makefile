# A default value for when we're building this subsystem independently...
ifndef CFLAGS_SET_TO_USE
   CFLAGS_SET_TO_USE=USE_NORMAL_DEV_CFLAGS
endif

include $(AURORA_SRC_DIR)/makedefs_locations.mk	
include $(AURORA_SRC_DIR)/makedefs_cflags.mk


all: all_targets

# Caveats:
# - This makefile doesn't right now build the unit test programs that people show
#   no interest in.  They can be added later as desired.


#################################################################################
#                  GENARAL DEFINITIONS FOR COMPILING / LINKING                  #
#################################################################################

AURORA_INT_INCLUDE_DIR=$(AURORA_API_BASE_DIR)/int_include

INCLUDE_FLAGS= \
	-I. \
	$(BERKELEY_DB_4_INCLUDE_CFLAG) \
	$(XERCES_INCLUDE_CFLAG) \
	-I$(UTIL_INCLUDE_DIR) \
	-I$(RUNTIME_MISC_INCLUDE_DIR) \
	-I$(PREDPARSER_INCLUDE_DIR) \
	-I$(PREDPARSER_INCLUDE_DIR)/PredicatesAndExpressions \
	-I$(CATALOG_MGR_INCLUDE_DIR) \
	-I$(STORAGE_MGR_INCLUDE_DIR) \
	-I$(STORAGE_MGR_INCLUDE_DIR)/sm/include \
	-I$(SCHED_INCLUDE_DIR) \
	-I$(AURORA_INCLUDE_DIR) \
	-I$(AURORA_INT_INCLUDE_DIR) \
	$(XFORMS_INCLUDE_CFLAG) \
	-I$(LOAD_SHEDDER_INCLUDE_DIR)

#################################################################################
#                         BUILD TARGET DEFINITIONS                              #
#################################################################################

UTIL_LIB_SO          = $(AURORA_LIB_DIR)/libaurora_util.so
STORAGE_MGR_LIB_SO   = $(AURORA_LIB_DIR)/libaurora_storage_mgr.so
RUNTIME_MISC_LIB_SO  = $(AURORA_LIB_DIR)/libaurora_runtime_misc.so
PREDPARSER_LIB_SO    = $(AURORA_LIB_DIR)/libaurora_predparser.so
SCHED_LIB_SO         = $(AURORA_LIB_DIR)/libaurora_sched.so
CATALOG_MGR_LIB_SO   = $(AURORA_LIB_DIR)/libaurora_catalog_mgr.so
LOAD_SHEDDER_LIB_SO  = $(AURORA_LIB_DIR)/libaurora_load_shedder.so
AURORA_CXX_API_SO    = $(AURORA_LIB_DIR)/libaurora_cxx.so
AURORA_C_API_SO      = $(AURORA_LIB_DIR)/libaurora_c.so
AURORA_SOCKET_API_SO = $(AURORA_LIB_DIR)/libaurora_socket.so
WLGEN_LIB_SO         = $(AURORA_LIB_DIR)/libaurora_wlgen.so

CATALOG_MGR_MAIN_APP = $(AURORA_BIN_DIR)/catalog_mgr_main
NETGEN_APP           = $(AURORA_BIN_DIR)/netgen
TESTLOADGEN_APP      = $(AURORA_BIN_DIR)/testloadgen
TEST_CAPI1_APP       = $(AURORA_BIN_DIR)/test_capi1
RUN_A_NETWORK_APP    = $(AURORA_BIN_DIR)/run_a_network
FILE_CLIENT_APP      = $(AURORA_BIN_DIR)/file-client
WLGEN_APP            = $(AURORA_BIN_DIR)/wlgen
WLREC_APP            = $(AURORA_BIN_DIR)/wlrec
AREC_APP             = $(AURORA_BIN_DIR)/arec
AURORA_SOCKET_API_SIMPLE_APP = $(AURORA_BIN_DIR)/wlgen_api_daemon
PERFMON_CLIENT_APP   = $(AURORA_BIN_DIR)/perf_mon_gui
GUI_BYACCJ_APP       = $(GUI_BYACCJ_BASE_DIR)/gui_byaccj

ALL_AURORA_CXX_API_SOS= \
	$(UTIL_LIB_SO) \
	$(STORAGE_MGR_LIB_SO) \
	$(RUNTIME_MISC_LIB_SO) \
	$(PREDPARSER_LIB_SO) \
	$(SCHED_LIB_SO) \
	$(CATALOG_MGR_LIB_SO) \
	$(LOAD_SHEDDER_LIB_SO) \
	$(AURORA_CXX_API_SO)

ALL_AURORA_C_API_SOS= \
	$(ALL_AURORA_CXX_API_SOS) \
	$(AURORA_C_API_SO)

FISH2_DEMO_APP = $(FISH2_DEMO_BASE_DIR)/bin/fish2

MITRE_DEMO_SGEN_APP  = $(MITRE_DEMO_BASE_DIR)/inputgen/sgen
MITRE_DEMO_SSEND_APP = $(MITRE_DEMO_BASE_DIR)/inputgen/ssend
MITRE_DEMO_ISEND_APP = $(MITRE_DEMO_BASE_DIR)/inputgen/isend

LROAD_DEMO_HARNESS_APP = $(LROAD_DEMO_BASE_DIR)/harness/lrh

#################################################################################
#                             BUILD TARGET RULES                                #
#################################################################################

include $(UTIL_BASE_DIR)/src/util.mk
include $(STORAGE_MGR_BASE_DIR)/src/store.mk
include $(RUNTIME_MISC_BASE_DIR)/src/runtime_misc.mk
include $(PREDPARSER_BASE_DIR)/src/predparser.mk
include $(SCHED_BASE_DIR)/src/sched.mk
include $(CATALOG_MGR_BASE_DIR)/src/catalogmgr.mk
include $(LOAD_SHEDDER_BASE_DIR)/src/load_shedder.mk
include $(AURORA_API_BASE_DIR)/src/auroralib.mk
include $(SOCKET_API_BASE_DIR)/src/loadgen_daemon.mk
include $(WLGEN_BASE_DIR)/src/loadgen_wlgen.mk
include $(PERFMON_BASE_DIR)/perf_mon.mk
include $(GUI_BASE_DIR)/gui.mk
include $(GUI_BYACCJ_BASE_DIR)/gui_byaccj.mk
include $(FISH2_DEMO_BASE_DIR)/src/fish2.mk
include $(MITRE_DEMO_BASE_DIR)/mitre.mk
include $(LROAD_DEMO_BASE_DIR)/linear_road.mk

ALL_DEPS = $(foreach obj,$(ALL_CXX_OBJECTS),$(dir $(obj)).deps/$(basename $(notdir $(obj))).d)
ALL_DEP_DIRS = $(sort $(dir $(ALL_DEPS)))

showalldeps:
	echo ALL_DEPS is ;      \
	ls -lh $(ALL_DEPS) ;    \
	echo ALL_DEP_DIRS is ;  \
	ls -ldh $(ALL_DEP_DIRS)

all_targets: \
	$(UTIL_LIB_SO) \
	$(STORAGE_MGR_LIB_SO) \
	$(RUNTIME_MISC_LIB_SO) \
	$(PREDPARSER_LIB_SO) \
	$(SCHED_LIB_SO) \
	$(CATALOG_MGR_LIB_SO) \
	$(LOAD_SHEDDER_LIB_SO) \
	$(AURORA_CXX_API_SO) \
	$(AURORA_C_API_SO) \
	$(AURORA_SOCKET_API_SO) \
	$(AURORA_SOCKET_API_SIMPLE_APP) \
	$(WLGEN_LIB_SO) \
	$(NETGEN_APP) \
	$(CATALOG_MGR_MAIN_APP) \
	$(TESTLOADGEN_APP) \
	$(TEST_CAPI1_APP) \
	$(RUN_A_NETWORK_APP) \
	$(FILE_CLIENT_APP) \
	$(WLGEN_APP) \
	$(WLREC_APP) \
	$(AREC_APP) \
	$(PERFMON_CLIENT_APP) \
	$(GUI_BYACCJ_APP) \
	gui \
	$(FISH2_DEMO_APP) \
	$(MITRE_DEMO_SGEN_APP) \
	$(MITRE_DEMO_SSEND_APP) \
	$(MITRE_DEMO_ISEND_APP) \
	$(LROAD_DEMO_HARNESS_APP)

# This runs the risk of deleting files the user wanted to keep (if the
# user was silly enough to put them into the .deps directories.)
# However, more sophisticated approaches ran into various problems
# that I don't have time to look into. -cjc
clean_deps:
	rm -rf `find . -name .deps`
#rm -f $(addsuffix *.d,$(ALL_DEP_DIRS))



# This could also be rm -rf $(ALL_DEP_DIRS), except that could cause
# some serious trouble if $(ALL_DEP_DIRS) got set incorrectly somehow!

clean: \
	clean_deps \
	clean_util_lib \
	clean_storage_mgr_lib \
	clean_runtime_misc_lib \
	clean_predparser_lib \
	clean_sched_lib \
	clean_catalog_mgr_lib \
	clean_load_shedder_lib \
	clean_aurora_cxx_api_lib \
	clean_aurora_c_api_lib \
	clean_aurora_socket_api_lib \
	clean_wlgen_lib \
	clean_netgen_app \
	clean_catalog_mgr_main_app \
	clean_testloadgen_app \
	clean_aurora_sock_api_simple_app \
	clean_test_capi1_app \
	clean_file_client_app \
	clean_run_a_network_app \
	clean_wlgen_app \
	clean_wlrec_app \
	clean_arec_app \
	clean_perfmon_client_app \
	clean_gui \
	clean_gui_byaccj \
	clean_fish2_demo \
	clean_mitre_demo_sgen_app \
	clean_mitre_demo_ssend_app \
	clean_mitre_demo_isend_app \
	clean_lroad_demo_harness_app


# Generic fules for compiling C/C++ files. More specific overrides may be given 
# as needed, such as when the INCLUDE_FLAGS value must be overridden.
#
# I believe the generic rules must be defined *after* all overrides, or else the
# overrides won't take effect.  Perhaps 'make' searches the file, from top to
# bottom, until it finds a matching rule? -cjc
%.o: %.C
	@mkdir -p $(@D)/.deps
	$(CXX) $(CFLAGS) $(INCLUDE_FLAGS) -c $< -o $@ \
	-MT $@ -MMD -MF $(@D)/.deps/$(basename $(@F)).d

showdeps:
	@echo $(ALL_DEPS)

-include $(ALL_DEPS)

#################################################################################
#                              CONVENIENCE TARGETS                              #
#################################################################################

perf_mon_gui: $(PERFMON_CLIENT_APP)

testloadgen:   $(TESTLOADGEN_APP)

test_capi1:    $(TEST_CAPI1_APP)

run_a_network: $(RUN_A_NETWORK_APP)

fish2: $(FISH2_DEMO_APP)

mitre: \
	$(MITRE_DEMO_SGEN_APP) \
	$(MITRE_DEMO_SSEND_APP) \
	$(MITRE_DEMO_ISEND_APP)
