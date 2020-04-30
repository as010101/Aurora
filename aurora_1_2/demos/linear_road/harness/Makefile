# Makefile for the linear road harness (lrh)
#

# A default value for when we're building this subsystem independently...
ifndef CFLAGS_SET_TO_USE
   CFLAGS_SET_TO_USE=USE_NORMAL_DEV_CFLAGS
endif

include ../../../makedefs_locations.mk
include ../../../makedefs_cflags.mk
include ../../../makedefs_util.mk

include $(UTIL_BASE_DIR)/src/makedefs_util_lib_objects.mk

INCLUDE_FLAGS= \
	-I. \
	-I$(UTIL_INCLUDE_DIR) 

LRH_APP = lrh

LRH_OBJECTS= \
	LRHarness.o \
	LRSender.o \
	LRReceiver.o \
	CarMap.o \
	main.o 

all: $(LRH_APP) 

$(LRH_APP): \
	$(LRH_OBJECTS)
	$(CC) $(CFLAGS) $(INCLUDE_FLAGS) -o $@ \
	$(LRH_OBJECTS) \
	$(UTIL_LIB_OBJECTS) \
	$(XERCES_LIB_LINKAGE) \
	-l pthread \
	$(GLOBAL_APPLICATION_LFLAGS)

%.o: %.C
	$(CC) $(CFLAGS) $(INCLUDE_FLAGS) -c $<

clean: 
	rm -f  *.o core *~
	rm -f $(LRH_APP)
