# This file is to be included in the top-level Makefile.
# This file is owned by the integration lead programmer, but *tentative* changes
# may be made by others.

AURORA_SOCKET_API_OBJECTS= \
	$(SOCKET_API_BASE_DIR)/src/InputReceiver.o \
	$(SOCKET_API_BASE_DIR)/src/OutputSender.o \
	$(SOCKET_API_BASE_DIR)/src/ClientPool.o \
	$(SOCKET_API_BASE_DIR)/src/AuroraSocketAPI.o 

AURORA_SOCKET_API_SIMPLE_APP_OBJECTS= \
	$(SOCKET_API_BASE_DIR)/src/main.o

ALL_CXX_OBJECTS += \
	$(AURORA_SOCKET_API_OBJECTS) \
	$(WLGEN_API_DAEMON_OBJECTS)

$(AURORA_SOCKET_API_SO): \
	$(AURORA_SOCKET_API_OBJECTS)
	$(CXX) -fPIC -shared -o $@ \
	$(AURORA_SOCKET_API_OBJECTS)

$(AURORA_SOCKET_API_SIMPLE_APP): \
	$(AURORA_SOCKET_API_SO) \
	$(ALL_AURORA_C_API_SOS) \
	$(AURORA_SOCKET_API_SIMPLE_APP_OBJECTS)
	$(CXX) -o $@ \
	$(AURORA_SOCKET_API_SIMPLE_APP_OBJECTS) \
	$(AURORA_SOCKET_API_SO) \
	$(ALL_AURORA_C_API_SOS) \
	-l pthread \
	$(BERKELEY_DB_4_LIB_LINKAGE) \
	$(XERCES_LIB_LINKAGE) \
	$(XFORMS_LIB_LINKAGE) \
	$(GLOBAL_APPLICATION_LFLAGS)

clean_aurora_sock_api_simple_app:
	rm -f $(AURORA_SOCKET_API_SIMPLE_APP)
	rm -f $(AURORA_SOCKET_API_OBJECTS)
	rm -f $(AURORA_SOCKET_API_SO)

clean_aurora_socket_api_lib:
	rm -f $(AURORA_SOCKET_API_OBJECTS)
	rm -f $(AURORA_SOCKET_API_SO)

