# This file is to be included in the top-level Makefile.
# This file is owned by the integration lead programmer, but *tentative* changes
# may be made by others.


MITRE_DEMO_SGEN_OBJECTS = \
	$(MITRE_DEMO_BASE_DIR)/inputgen/sgen.o

MITRE_DEMO_SSEND_OBJECTS = \
	$(MITRE_DEMO_BASE_DIR)/inputgen/ssend.o

MIRE_DEMO_ISEND_OBJECTS =  \
	$(MITRE_DEMO_BASE_DIR)/inputgen/isend.o

ALL_CXX_OBJECTS += \
	$(MITRE_DEMO_SGEN_OBJECTS) \
	$(MITRE_DEMO_SSEND_OBJECTS) \
	$(MIRE_DEMO_ISEND_OBJECTS)

# Use a custom compilation rule for this subproject, because it needs different
# #include flags than are needed for building aurora.
MITRE_DEMO_INPUTGEN_INCLUDE_FLAGS= \
	-I$(MITRE_DEMO_BASE_DIR)/inputgen \
	-I$(AURORA_INCLUDE_DIR) \
	-I$(UTIL_INCLUDE_DIR)

$(MITRE_DEMO_BASE_DIR)/inputgen/%.o: $(MITRE_DEMO_BASE_DIR)/inputgen/%.C
	@mkdir -p $(@D)/.deps
	$(CXX) $(CFLAGS) $(MITRE_DEMO_INPUTGEN_INCLUDE_FLAGS) -c $< -o $@ \
	-MT $@ -MMD -MF $(@D)/.deps/$(basename $(@F)).d

$(MITRE_DEMO_SGEN_APP): \
	$(MITRE_DEMO_SGEN_OBJECTS)
	$(CXX) $(CFLAGS) $(INCLUDE_FLAGS) -o $@ \
	$(MITRE_DEMO_SGEN_OBJECTS) \
	$(GLOBAL_APPLICATION_LFLAGS)

$(MITRE_DEMO_SSEND_APP): \
	$(MITRE_DEMO_SSEND_OBJECTS)
	$(CXX) $(CFLAGS) $(INCLUDE_FLAGS) -o $@ \
	$(MITRE_DEMO_SSEND_OBJECTS) \
	$(GLOBAL_APPLICATION_LFLAGS)

$(MITRE_DEMO_ISEND_APP): \
	$(MIRE_DEMO_ISEND_OBJECTS)
	$(CXX) $(CFLAGS) $(INCLUDE_FLAGS) -o $@ \
	$(MIRE_DEMO_ISEND_OBJECTS) \
	$(GLOBAL_APPLICATION_LFLAGS)

clean_mitre_demo_sgen_app:
	rm -f $(MITRE_DEMO_SGEN_OBJECTS)
	rm -f $(MITRE_DEMO_SGEN_APP)

clean_mitre_demo_ssend_app:
	rm -f $(MITRE_DEMO_SSEND_OBJECTS)
	rm -f $(MITRE_DEMO_SSEND_APP)

clean_mitre_demo_isend_app:
	rm -f $(MIRE_DEMO_ISEND_OBJECTS)
	rm -f $(MITRE_DEMO_ISEND_APP)
