# This file is to be included in the top-level Makefile.
# This file is owned by the integration lead programmer, but *tentative* changes
# may be made by others.

CATALOG_MGR_LIB_OBJECTS= \
	$(CATALOG_MGR_BASE_DIR)/src/AggregateBox.o \
	$(CATALOG_MGR_BASE_DIR)/src/Application.o \
	$(CATALOG_MGR_BASE_DIR)/src/Arc.o \
	$(CATALOG_MGR_BASE_DIR)/src/ArcRecord.o \
	$(CATALOG_MGR_BASE_DIR)/src/Box.o \
	$(CATALOG_MGR_BASE_DIR)/src/BoxRecord.o \
	$(CATALOG_MGR_BASE_DIR)/src/BSortBox.o \
	$(CATALOG_MGR_BASE_DIR)/src/CatalogManager.o \
	$(CATALOG_MGR_BASE_DIR)/src/CompositeType.o \
	$(CATALOG_MGR_BASE_DIR)/src/CompositeTypeRecord.o \
	$(CATALOG_MGR_BASE_DIR)/src/DataInputStream.o \
	$(CATALOG_MGR_BASE_DIR)/src/DataOutputStream.o \
	$(CATALOG_MGR_BASE_DIR)/src/DropBox.o \
	$(CATALOG_MGR_BASE_DIR)/src/FilterBox.o \
	$(CATALOG_MGR_BASE_DIR)/src/HelloWorldBox.o \
	$(CATALOG_MGR_BASE_DIR)/src/IntegerArrayDbt.o \
	$(CATALOG_MGR_BASE_DIR)/src/IntegerDbt.o \
	$(CATALOG_MGR_BASE_DIR)/src/JoinBox.o \
	$(CATALOG_MGR_BASE_DIR)/src/LRReadRelationBox.o \
	$(CATALOG_MGR_BASE_DIR)/src/LRUpdateRelationBox.o \
	$(CATALOG_MGR_BASE_DIR)/src/MapBox.o \
	$(CATALOG_MGR_BASE_DIR)/src/PortRecord.o \
	$(CATALOG_MGR_BASE_DIR)/src/QoS.o \
	$(CATALOG_MGR_BASE_DIR)/src/QueryNetwork.o \
	$(CATALOG_MGR_BASE_DIR)/src/RandomPoint.o \
	$(CATALOG_MGR_BASE_DIR)/src/ReadRelationBox.o \
	$(CATALOG_MGR_BASE_DIR)/src/Relation.o \
	$(CATALOG_MGR_BASE_DIR)/src/RelationEnvironment.o \
	$(CATALOG_MGR_BASE_DIR)/src/ResampleBox.o \
	$(CATALOG_MGR_BASE_DIR)/src/RestreamBox.o \
	$(CATALOG_MGR_BASE_DIR)/src/SourceStream.o \
	$(CATALOG_MGR_BASE_DIR)/src/Stream.o \
	$(CATALOG_MGR_BASE_DIR)/src/TypeFieldRecord.o \
	$(CATALOG_MGR_BASE_DIR)/src/TypeManager.o \
	$(CATALOG_MGR_BASE_DIR)/src/UnionBox.o \
	$(CATALOG_MGR_BASE_DIR)/src/UpdateRelationBox.o

NETGEN_APP_OBJECTS= \
	$(CATALOG_MGR_BASE_DIR)/src/netgen.o

CATALOG_MGR_MAIN_APP_OBJECTS= \
	$(CATALOG_MGR_BASE_DIR)/src/Main.o

ALL_CXX_OBJECTS += \
	$(CATALOG_MGR_LIB_OBJECTS) \
	$(NETGEN_APP_OBJECTS) \
	$(CATALOG_MGR_MAIN_APP_OBJECTS)

$(CATALOG_MGR_LIB_SO): \
	$(CATALOG_MGR_LIB_OBJECTS)
	$(CXX) -fPIC -shared -o $@ \
	$(CATALOG_MGR_LIB_OBJECTS)

$(CATALOG_MGR_MAIN_APP): \
	$(UTIL_LIB_SO) \
	$(STORAGE_MGR_LIB_SO) \
	$(RUNTIME_MISC_LIB_SO) \
	$(PREDPARSER_LIB_SO) \
	$(SCHED_LIB_SO) \
	$(LOAD_SHEDDER_LIB_SO) \
	$(CATALOG_MGR_LIB_SO) \
	$(CATALOG_MGR_MAIN_APP_OBJECTS)
	$(CXX) $(CFLAGS) $(INCLUDE_FLAGS) -o $@ \
	$(CATALOG_MGR_MAIN_APP_OBJECTS) \
	-L$(AURORA_LIB_DIR) \
	-laurora_util \
	-laurora_storage_mgr \
	-laurora_runtime_misc \
	-laurora_predparser \
	-laurora_sched \
	-laurora_catalog_mgr \
	-laurora_load_shedder \
	-l pthread \
	$(BERKELEY_DB_4_LIB_LINKAGE) \
	$(XERCES_LIB_LINKAGE) \
	$(XFORMS_LIB_LINKAGE) \
	$(GLOBAL_APPLICATION_LFLAGS)

$(NETGEN_APP): \
	$(UTIL_LIB_SO) \
	$(STORAGE_MGR_LIB_SO) \
	$(RUNTIME_MISC_LIB_SO) \
	$(PREDPARSER_LIB_SO) \
	$(SCHED_LIB_SO) \
	$(LOAD_SHEDDER_LIB_SO) \
	$(CATALOG_MGR_LIB_SO) \
	$(NETGEN_APP_OBJECTS)
	$(CXX) $(CFLAGS) $(INCLUDE_FLAGS) -o $@ \
	$(NETGEN_APP_OBJECTS) \
	-L$(AURORA_LIB_DIR) \
	-laurora_util \
	-laurora_storage_mgr \
	-laurora_runtime_misc \
	-laurora_predparser \
	-laurora_sched \
	-laurora_catalog_mgr \
	-laurora_load_shedder \
	-l pthread \
	$(BERKELEY_DB_4_LIB_LINKAGE) \
	$(XERCES_LIB_LINKAGE) \
	$(XFORMS_LIB_LINKAGE) \
	$(GLOBAL_APPLICATION_LFLAGS)

clean_catalog_mgr_lib:
	rm -f $(CATALOG_MGR_LIB_OBJECTS)
	rm -f $(CATALOG_MGR_LIB_SO)

clean_catalog_mgr_main_app:
	rm -f $(CATALOG_MGR_MAIN_APP_OBJECTS)
	rm -f $(CATALOG_MGR_MAIN_APP)

clean_netgen_app:
	rm -f $(NETGEN_APP_OBJECTS)
	rm -f $(NETGEN_APP)
