ifndef CFLAGS_SET_TO_USE
   CFLAGS_SET_TO_USE=USE_NORMAL_DEV_CFLAGS
endif

include ../../../../makedefs_locations.mk
include ../../../../makedefs_cflags.mk
include ../../../../makedefs_util.mk
include ../../sm_makedefs.mk

INCLUDE_FLAGS= \
	-I$(UTIL_INCLUDE_DIR) \
	-I$(SM_PROJBASE)/src \
	$(XERCES_INCLUDE_CFLAG)

CFLAGS += $(INCLUDE_FLAGS)

all: \
	$(OBJ_STATS) \
	install_obj

install_obj:
	cp $(OBJ_STATS) $(SM_TARGETBASE)/obj

clean:
	rm -f *.o

# makedepend checks file dependencies and makes sure that everything
# that needs to be compiled is compiled. It's not an entirely speedy
# process, but you should run it after adding header files and/or
# making significant changes to your code base.
# Commented out because these cause such pain for people. -cjc
#dependencies:
#	makedepend $(INCLUDE_FLAGS) $(CPLUSPLUS_STDLIB_MAKEDEP_INCLUDE_FLAGS) *.C
