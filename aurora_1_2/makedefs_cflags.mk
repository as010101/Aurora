# Customization:
# Before including this file, set the variable CFLAGS_SET_TO_USE to one of the
# following values:
#    USE_NORMAL_DEV_CFLAGS
#    USE_BASELINE_DEV_CFLAGS
#    USE_CORRECTNESS_SCRUTINY_CFLAGS_1
#    USE_CORRECTNESS_SCRUTINY_CFLAGS_2
#    USE_OPTIMIZED_CFLAGS
#    USE_DEBUG_AND_PROFILE_CFLAGS
#
# CC_VERSION must be set to one of the following values, to properly indicate
# which version of the C++ compiler these flags will be passed to:
#    GCC_3_2
#
# Notes:
#
# This file ultimately defines the following variables, which should be used in
# the prescribed build steps:
# CFLAGS : This variable provides the minimal set of cflags that should be passed
#   to the C/C++ compiler whenever building one of Aurora's .C files.
#
# GLOBAL_APPLICATION_LFLAGS : This variable provides linkage flags that should 
#   be passed to the C/C++ compiler whenever linking an application program 
#   that's part of Aurora.

#################################################################################
# GROUPS OF C++ COMPILER FLAGS, BY APPLICATION...
#################################################################################

# Make our programs / libraries be analyzed at runtime for problems (hopefully
# without much performance penalty)
#
# Flags that turned out to be not such a good idea...
# 	-ftrapv       : STL seems to trip this flag's abort() code
#	-fstack-check : Program seems to work fine without this check, but the
#                       check kills it. I think it's being overly restrictive
#                       about how much stack space it allows.
RUNTIME_SAFETY_CFLAGS=

# Ensures our code follows reasonable standards of style
CODING_QUALITY_CFLAGS=-fno-operator-names -Wimplicit -Wmissing-braces \
	-Wparentheses -Wswitch -Wtrigraphs -Wunused -Wmultichar -Wunused-label


# Basic checking for our code's correctness, at compile time.
LOOSE_COMPILETIME_SCRUTINY_CFLAGS=-Wctor-dtor-privacy -Wnon-virtual-dtor \
	-Wreorder -Woverloaded-virtual -Wsign-promo -Wsynth \
	-Wchar-subscripts -Wformat -Wmain \
	-Wpointer-arith -Wcast-qual -Wcast-align -Wwrite-strings \
	-Wno-long-long \
	-Wsequence-point \
	-D_GLIBCPP_CONCEPT_CHECKS

# A more rigorous version of compile-time checking. Warnings raised by these 
# flags might be unavoidable.
TIGHT_COMPILETIME_SCRUTINY_CFLAGS=-W -Wfloat-equal -Wundef -Wshadow \
	-Wpacked

RIDICULOUSLY_NOISY_SCRUTINY_CFLAGS=-Weffc++ -Wpadded -Wold-style-cast

# Flags that I'm not convinced really work right (i.e,. lots of seemingly false
# positive warnings.
EXPERIMENTAL_COMPILETIME_SCRUTINY_CFLAGS=-Wunreachable-code

# Flags to help us know that our code is portable
PORTABILITY_SCRUTINY_CFLAGS=-pedantic

# Flags to help us know about opportunitise for using GCC-specific language
# extensions.
GCC_EXTENSION_OPPORTUNITIES_FINDING_CFLAGS=-Wmissing-noreturn \
	-Wmissing-format-attribute


# Note: When we're really squeazing the the code, check out g++'s "-fprofile-arcs" option
# Note: For code coverage, check out "gcov" and "-ftest-coverage".

# Flags for when we want to optimize the code.
#OPTIMIZER_SPEED_CFLAGS=-fomit-frame-pointer -foptimize-sibling-calls \
#	-finline-functions -fkeep-inline-functions -O3 -s

#
# is it -fkeep-inline-functions vs. -frepo? Is there even a point in doing frepo?
#	-fkeep-inline-functions \
#	-foptimize-sibling-calls does not work with 2.95 compiler

OPTIMIZER_SPEED_CFLAGS= \
	-fomit-frame-pointer \
	-finline-functions \
	-O3 -s

# Checking that can only be done when optimizing...
OPTIMIZER_SCRUTINY_FLAGS=-Wuninitialized

# Flags when we want debugging info in our code...
DEBUG_CFLAGS=-g
ifeq ($(AURORA_BUILD_ENV),BRANDEIS) # at 'deis, i'll take anything i can thank you
DEBUG_CFLAGS=-g
endif 
#DEBUG_CFLAGS=-g -ggdb3 -g3 These extra flags add code bloat, w/out any clear benefit.

# Flags we need to / want to define because of the particulars of our code.
# [I'm not sure if -pthread is necessary. -cjc]
#
# Flags that seemed a good idea at the time, but caused problems:
# -fno-strict-aliasing
# -fshort-enums
HACK_CFLAGS=-m32 -pthread -D_REENTRANT

#################################################################################
# C++ *LINKER* FLAGS...
#################################################################################


#################################################################################
# Groupings of flag groups, for our particular needs...
#################################################################################

# Normal development, when we want to catch stuff early rather than have 
# highly-optimized code.

ifeq ($(AURORA_BUILD_ENV),BRANDEIS) # At Brandeis, we debug by default
NORMAL_DEV_CFLAGS= \
	$(HACK_CFLAGS) \
	-DSM_VERIFY_LOCKS \
	$(DEBUG_CFLAGS)
else
ifeq ($(AURORA_BUILD_ENV),REDHAT8_AT_MIT) # MIT likes that too.
NORMAL_DEV_CFLAGS= \
	$(HACK_CFLAGS) \
	-DSM_VERIFY_LOCKS \
	$(DEBUG_CFLAGS)
else
NORMAL_DEV_CFLAGS= \
	$(HACK_CFLAGS) \
	-DSM_VERIFY_LOCKS
#	$(CODING_QUALITY_CFLAGS) \
#	$(LOOSE_COMPILETIME_SCRUTINY_CFLAGS) \
#	$(OPTIMIZER_SPEED_CFLAGS) \
#	$(OPTIMIZER_SCRUTINY_FLAGS) \

endif
endif

NORMAL_DEV_LFLAGS=


# When the other flags cause problems, go pretty bare-bones...
BASELINE_DEV_CFLAGS= \
	$(DEBUG_CFLAGS) \
	-DSM_VERIFY_LOCKS

BASELINE_DEV_LFLAGS=

# One way of building the code to really shake it down...
CORRECTNESS_SCRUTINY_CFLAGS_1= \
	$(HACK_CFLAGS) \
	$(RUNTIME_SAFETY_CFLAGS) \
	$(CODING_QUALITY_CFLAGS) \
	$(LOOSE_COMPILETIME_SCRUTINY_CFLAGS) \
	$(TIGHT_COMPILETIME_SCRUTINY_CFLAGS) \
	$(DEBUG_CFLAGS) \
	-DSM_VERIFY_LOCKS

CORRECTNESS_SCRUTINY_LFLAGS_1=

# Another way of building the code to really shake it down. Note that this 
# is complementary to CORRECTNESS_SCRUTINY_CFLAGS_1, so both tests should be
# done.
CORRECTNESS_SCRUTINY_CFLAGS_2= \
	$(HACK_CFLAGS) \
	$(RUNTIME_SAFETY_CFLAGS) \
	$(OPTIMIZER_SPEED_CFLAGS) \
	$(OPTIMIZER_SCRUTINY_FLAGS) \
	-DSM_VERIFY_LOCKS

CORRECTNESS_SCRUTINY_LFLAGS_2=

# add hacks?
OPTIMIZED_CFLAGS= \
	$(HACK_CFLAGS) \
	$(OPTIMIZER_SPEED_CFLAGS)

OPTIMIZED_LFLAGS=

# For when you want debug code to be usable by gprof...
DEBUG_AND_PROFILE_CFLAGS= \
	-fno-inline \
	$(DEBUG_CFLAGS) \
	$(HACK_CFLAGS) \
	$(CODING_QUALITY_CFLAGS) \
	$(LOOSE_COMPILETIME_SCRUTINY_CFLAGS) \
	-DSM_VERIFY_LOCKS -pg

DEBUG_AND_PROFILE_LFLAGS= \
	$(NOMRAL_DEV_LFLAGS)

#################################################################################
# Where the rubber hits the road...
#################################################################################

ifeq ($(CFLAGS_SET_TO_USE),USE_NORMAL_DEV_CFLAGS)
   CFLAGS=$(NORMAL_DEV_CFLAGS) -fPIC
   GLOBAL_APPLICATION_LFLAGS=$(NOMRAL_DEV_LFLAGS) 
else
ifeq ($(CFLAGS_SET_TO_USE),USE_BASELINE_DEV_CFLAGS)
   CFLAGS=$(BASELINE_DEV_CFLAGS) -fPIC
   GLOBAL_APPLICATION_LFLAGS=$(BASELINE_DEV_LFLAGS)
else
ifeq ($(CFLAGS_SET_TO_USE),USE_CORRECTNESS_SCRUTINY_CFLAGS_1)
   CFLAGS=$(CORRECTNESS_SCRUTINY_CFLAGS_1) -fPIC
   GLOBAL_APPLICATION_LFLAGS=$(CORRECTNESS_SCRUTINY_LFLAGS_1)
else
ifeq ($(CFLAGS_SET_TO_USE),USE_CORRECTNESS_SCRUTINY_CFLAGS_2)
   CFLAGS=$(CORRECTNESS_SCRUTINY_CFLAGS_2) -fPIC
   GLOBAL_APPLICATION_LFLAGS=$(CORRECTNESS_SCRUTINY_LFLAGS_2)
else
ifeq ($(CFLAGS_SET_TO_USE),USE_OPTIMIZED_CFLAGS)
   CFLAGS=$(OPTIMIZED_CFLAGS) -fPIC
   GLOBAL_APPLICATION_LFLAGS=$(OPTIMIZED_LFLAGS)
else
ifeq ($(CFLAGS_SET_TO_USE),USE_DEBUG_AND_PROFILE_CFLAGS)
   CFLAGS=$(DEBUG_AND_PROFILE_CFLAGS) -fPIC
   GLOBAL_APPLICATION_LFLAGS=$(DEBUG_AND_PROFILE_LFLAGS)
else
   $(error You need to set the env/make variable CFLAGS_SET_TO_USE to a valid value)
endif
endif
endif
endif
endif
endif

