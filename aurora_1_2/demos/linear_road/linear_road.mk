# This file is to be included in the top-level Makefile.


LROAD_DEMO_HARNESS_OBJECTS =  \
	$(LROAD_DEMO_BASE_DIR)/harness/LRHarness.o \
	$(LROAD_DEMO_BASE_DIR)/harness/LRSender.o \
	$(LROAD_DEMO_BASE_DIR)/harness/LRReceiver.o \
	$(LROAD_DEMO_BASE_DIR)/harness/CarMap.o \
	$(LROAD_DEMO_BASE_DIR)/harness/main.o 

ALL_CXX_OBJECTS += \
	$(LROAD_DEMO_HARNESS_OBJECTS)

# Use a custom compilation rule for this subproject, because it needs different
# #include flags than are needed for building aurora.
LROAD_DEMO_HARNESS_INCLUDE_FLAGS= \
	-I$(LROAD_DEMO_BASE_DIR)/harness \
	-I$(AURORA_INCLUDE_DIR) \
	-I$(UTIL_INCLUDE_DIR)

$(LROAD_DEMO_BASE_DIR)/harness/%.o: $(LROAD_DEMO_BASE_DIR)/harness/%.C
	@mkdir -p $(@D)/.deps
	$(CXX) $(CFLAGS) $(LROAD_DEMO_HARNESS_INCLUDE_FLAGS) -c $< -o $@ \
	-MT $@ -MMD -MF $(@D)/.deps/$(basename $(@F)).d

$(LROAD_DEMO_HARNESS_APP): \
	$(LROAD_DEMO_HARNESS_OBJECTS)
	$(CXX) $(CFLAGS) $(INCLUDE_FLAGS) -o $@ \
	$(LROAD_DEMO_HARNESS_OBJECTS) \
	$(UTIL_LIB_OBJECTS) \
	$(XERCES_LIB_LINKAGE) \
	-l pthread \
	$(GLOBAL_APPLICATION_LFLAGS)

clean_lroad_demo_harness_app:
	rm -f $(LROAD_DEMO_HARNESS_OBJECTS)
	rm -f $(LROAD_DEMO_HARNESS_APP)
