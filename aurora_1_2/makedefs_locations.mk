#--------------------------------------------------------------------------------
# Customization:
# The AURORA_BUILD_ENV variable must be set to one of the following values:
#   BROWN
#   CJC_AT_HOME
#   BRANDEIS
#   DPC_ON_LAPTOP
#   REDHAT8_AT_MIT
#
# The AURORA_SRC_DIR variable must be set to the 'aurora' directory that is 
# the parent of all our subproject directories. All building occurs relative
# to this variable.
#
# ANT_HOME and JAVA_HOME variables must be set to the values implied by the value 
# of your AURORA_BUILD_ENV variable.
#
# Probably the best place to do this is in your login scripts, since they're
# most likely site-specific.
#
#--------------------------------------------------------------------------------
#
# Comments:
# (1) Here's why I have two variables such as GNU_MP_INCLUDEDIR and
#     GNU_MP_INCLUDE_CFLAG where you'ld normally expect only one: If
#     GNU_MP_INCLUDEDIR is one of the directories that the g++ automatically
#     searches through, then g++ gives a warning/error if you explicitely list
#     that directory as a -I cmd line option. So using two variables lets us
#     omit the "-I$(GNU_MP_INCLUDEDIR)" in such a scenario.
#
# (2) CPLUSPLUS_STDLIB_MAKEDEP_INCLUDE_FLAGS isn't something we pass to the 
#     compiler. The 'makedep' tool apparently doesn't know to search through
#     these directories, so this lists the extra flags we need to pass to 
#     makedeps to avoid that problem.
#
#
# (3) In a makefile, this file must be 'include'd before makedefs_cflags.mk,
#     if makedefs_cflags.mk is used by that Makefile.
#--------------------------------------------------------------------------------


#--------------------------------------------------------------------------------
# Where our software resides...
#--------------------------------------------------------------------------------

ifndef AURORA_SRC_DIR
   $(error AURORA_SRC_DIR must be defined as the top-level directory of Aurora \
	source code. I.e., the directory created by checking out the 'aurora' \
	module from CVS.)
endif

# The root directories of where the various subystems reside...
UTIL_BASE_DIR=$(AURORA_SRC_DIR)/util
RUNTIME_MISC_BASE_DIR=$(AURORA_SRC_DIR)/runtime_misc
PREDPARSER_BASE_DIR=$(AURORA_SRC_DIR)/predparser
CATALOG_MGR_BASE_DIR=$(AURORA_SRC_DIR)/catalogmgr
STORAGE_MGR_BASE_DIR=$(AURORA_SRC_DIR)/store
SCHED_BASE_DIR=$(AURORA_SRC_DIR)/sched
GUI_BASE_DIR=$(AURORA_SRC_DIR)/gui
GUI_BYACCJ_BASE_DIR=$(GUI_BASE_DIR)/edu/brown/aurora/gui/types/yacc/src
AURORA_API_BASE_DIR=$(AURORA_SRC_DIR)/auroralib
PERFMON_BASE_DIR=$(AURORA_SRC_DIR)/perf_mon
LOAD_SHEDDER_BASE_DIR=$(AURORA_SRC_DIR)/load_shedder
SOCKET_API_BASE_DIR=$(AURORA_SRC_DIR)/loadgen/daemon
WLGEN_BASE_DIR=$(AURORA_SRC_DIR)/loadgen/wlgen
FISH2_DEMO_BASE_DIR=$(AURORA_SRC_DIR)/demos/fish2
MITRE_DEMO_BASE_DIR=$(AURORA_SRC_DIR)/demos/mitre
LROAD_DEMO_BASE_DIR=$(AURORA_SRC_DIR)/demos/linear_road

# What you need to include in your include/link paths to use the respective
# subsystems, once they've been built...
UTIL_INCLUDE_DIR=$(UTIL_BASE_DIR)/include
UTIL_LIB_DIR=$(UTIL_BASE_DIR)/lib

RUNTIME_MISC_INCLUDE_DIR=$(RUNTIME_MISC_BASE_DIR)/include
RUNTIME_MISC_LIB_DIR=$(RUNTIME_MISC_BASE_DIR)/lib

PREDPARSER_INCLUDE_DIR=$(PREDPARSER_BASE_DIR)/src
PREDPARSER_LIB_DIR=$(PREDPARSER_BASE_DIR)/lib

CATALOG_MGR_INCLUDE_DIR=$(CATALOG_MGR_BASE_DIR)/src
CATALOG_MGR_LIB_DIR=$(CATALOG_MGR_BASE_DIR)/lib
CATALOG_MGR_BIN_DIR=$(CATALOG_MGR_BASE_DIR)/bin

STORAGE_MGR_INCLUDE_DIR=$(STORAGE_MGR_BASE_DIR)/src
STORAGE_MGR_LIB_DIR=$(STORAGE_MGR_BASE_DIR)/lib

SCHED_INCLUDE_DIR=$(SCHED_BASE_DIR)/src
SCHED_LIB_DIR=$(SCHED_BASE_DIR)/lib

AURORA_INCLUDE_DIR=$(AURORA_API_BASE_DIR)/include
AURORA_LIB_DIR    =$(AURORA_SRC_DIR)/lib
AURORA_BIN_DIR    =$(AURORA_SRC_DIR)/bin

PERFMON_INCLUDE_DIR=$(PERFMON_BASE_DIR)
PERFMON_NETWORK_INCLUDE_DIR=$(PERFMON_INCLUDE_DIR)/network

LOAD_SHEDDER_INCLUDE_DIR=$(LOAD_SHEDDER_BASE_DIR)/include
LOAD_SHEDDER_LIB_DIR=$(LOAD_SHEDDER_BASE_DIR)/lib

SOCKET_API_INCLUDE_DIR=$(SOCKET_API_BASE_DIR)/src
SOCKET_API_LIB_DIR=$(SOCKET_API_BASE_DIR)/lib

WLGEN_INCLUDE_DIR=$(WLGEN_BASE_DIR)/src
WLGEN_LIB_DIR=$(WLGEN_BASE_DIR)/lib

FISH2_DEMO_SRC_DIR=$(FISH2_DEMO_BASE_DIR)/src
FISH2_DEMO_BIN_DIR=$(FISH2_DEMO_BASE_DIR)/bin

#--------------------------------------------------------------------------------
# External software locations...
#--------------------------------------------------------------------------------

ifeq ($(AURORA_BUILD_ENV),BROWN)
   CC_VERSION=GCC_3_2

   CXX = /pro/aurora/tools/linux/distcc-2.5.1/bin/distcc \
        /pro/aurora/tools/linux/gcc-3.2.2/bin/g++

   CC = /pro/aurora/tools/linux/distcc-2.5.1/bin/distcc \
        /pro/aurora/tools/linux/gcc-3.2.2/bin/gcc

   GNU_MP_INCLUDE_CFLAG=-I/pro/aurora/tools/linux/built_with_gcc3.2/gmp-4.1.2/include
   GNU_MP_LIB_LINKAGE=-L/pro/aurora/tools/linux/built_with_gcc3.2/gmp-4.1.2/lib -lgmp -lgmpxx

   XERCESCROOT=/pro/aurora/tools/linux/built_with_gcc3.2/xerces-c-src2_1_0
   XERCES_INCLUDE_CFLAG=-I$(XERCESCROOT)/include
   XERCES_LIB_LINKAGE=-L$(XERCESCROOT)/lib -lxerces-c

   BERKELEY_DB_4_LIB_DIR=/pro/aurora/tools/linux/built_with_gcc3.2/BerkeleyDB.4.0/lib
   BERKELEY_DB_4_INCLUDE_CFLAG=-I/pro/aurora/tools/linux/built_with_gcc3.2/BerkeleyDB.4.0/include
   BERKELEY_DB_4_LIB_LINKAGE=-L/pro/aurora/tools/linux/built_with_gcc3.2/BerkeleyDB.4.0/lib -ldb-4.0 -ldb_cxx-4.0

   XFORMS_INCLUDE_CFLAG=-I/pro/aurora/linux/xforms/xforms/FORMS
   XFORMS_LIB_LINKAGE=/pro/aurora/linux/xforms/xforms/FORMS/libforms.a -lnsl -L/usr/X11R6/lib -lX11

   QTDIR=/pro/aurora/tools/linux/built_with_gcc3.2/qt-x11-free-3.1.1

   EFENCE_LIB_LINKAGE=-lefence

   LEX = flex -F
   LEX_LIB_LINKAGE=-lfl

   # The Makefile for the GUI needs to produce a shell script. The script is
   # written for execution in 'tcsh'.
   TCSH_PATH=/usr/local/bin/tcsh

   ANT_HOME=/pro/aurora/tools/linux/jakarta-ant-1.5.1
   ANT_CMD=$(ANT_HOME)/bin/ant

   # This must be one of the valid values for Ant's build.compiler global
   # property. This defines the compiler Ant will use to build the GUI.
   GUI_ANT_BUILD_COMPILER=modern

   # I think these need to be consistent with the compiler implied by the value
   # of GUI_ANT_BUILD_COMPILER, but I'm not positive. -cjc...
   JAVA_HOME=/pro/aurora/tools/linux/j2sdk1.4.0_01
   JAVA_SRC_PATH=$(JAVA_HOME)/src.zip
else
ifeq ($(AURORA_BUILD_ENV),REDHAT8_AT_MIT)
    # Submitted by jsalz@mit.edu
    #
    # Requires a bunch of RPMs installed (not all of which are included with RH8)
    # plus gmp installed under /opt/gmp (the RH8 GMP doesn't include C bindings)
    CC_VERSION=GCC_3_2

    ifndef CXX
        CXX=g++
    endif

    ifndef CC
        CC=gcc
    endif

    GNU_MP_INCLUDE_CFLAG=-I/opt/gmp/include
    GNU_MP_LIB_LINKAGE=-L/opt/gmp/lib -lgmpxx -lgmp

    XERCESCROOT=/opt/xerces
    XERCES_INCLUDE_CFLAG=-I$(XERCESCROOT)/include
    XERCES_LIB_LINKAGE=-L/opt/xerces/lib -lxerces-c

    BERKELEY_DB_4_LIB_DIR=/usr/lib
    BERKELEY_DB_4_INCLUDE_CFLAG=
    BERKELEY_DB_4_LIB_LINKAGE=-ldb_cxx-4.0 -ldb-4.0

    XFORMS_INCLUDE_CFLAG=-I/usr/X11R6/include/X11
    XFORMS_LIB_LINKAGE=-L/usr/X11R6/lib -lforms -lnsl -lX11

    QTDIR=/opt/qt

    EFENCE_LIB_LINKAGE=-lefence

    LEX = flex -F
    LEX_LIB_LINKAGE=-lfl

    # The Makefile for the GUI needs to produce a shell script. The script is
    # written for execution in 'tcsh'.
    TCSH_PATH=/bin/tcsh

    ANT_HOME=
    ANT_CMD=ant

    # This must be one of the valid values for Ant's build.compiler global
    # property. This defines the compiler Ant will use to build the GUI.
    GUI_ANT_BUILD_COMPILER=modern

    # I think these need to be consistent with the compiler implied by the value
    # of GUI_ANT_BUILD_COMPILER, but I'm not positive. -cjc...
    JAVA_HOME=/usr/java/j2sdk
    JAVA_SRC_PATH=$(JAVA_HOME)/src.zip

else
ifeq ($(AURORA_BUILD_ENV),BRANDEIS)
   # BRANDEIS FLAGS LAST VERIFIED MAY 20 2003 - NEW BOX!! YEEEHAAA
   CXX=ccache g++ -march=pentium4 -DNO_MEASUREMENT
   CC=ccache gcc -march=pentium4 -DNO_MEASUREMENT
   CC_VERSION=GCC_3_2

   GNU_MP_INCLUDE_CFLAG=-I/auroratools/gmp-4.1.2/include
   GNU_MP_LIB_LINKAGE=-L/auroratools/gmp-4.1.2/lib -lgmp -lgmpxx

  # We have xerces rpm installed in default location
   XERCESCROOT=
   XERCES_INCLUDE_CFLAG=
   XERCES_LIB_LINKAGE=-lxerces-c

   QTDIR=/usr/lib/qt3

   # We have Berkeley db rpm installed in default location
   # BERKELEY_DB_4_LIB_DIR=/usr/local/BerkeleyDB.4.0/lib
   BERKELEY_DB_4_LIB_DIR=/usr/lib
   BERKELEY_DB_4_INCLUDE_CFLAG=-I/usr/include/db4
   BERKELEY_DB_4_LIB_LINKAGE=-ldb_cxx-4.0 -ldb-4.0

   # We have xforms (1.0) rpm installed in default location
   XFORMS_INCLUDE_CFLAG=-I/usr/X11R6/include/X11
   XFORMS_LIB_LINKAGE=-lforms -lnsl -L/usr/X11R6/lib -lX11

#   EFENCE_LIB_LINKAGE=-lefence
   EFENCE_LIB_LINKAGE=

   LEX = flex -F
   LEX_LIB_LINKAGE=-lfl

   # The Makefile for the GUI needs to produce a shell script. The script is
   # written for execution in 'tcsh'.
   TCSH_PATH=/bin/tcsh

   ANT_HOME=/usr
   ANT_CMD=$(ANT_HOME)/bin/ant

   # This must be one of the valid values for Ant's build.compiler global
   # property. This defines the compiler Ant will use to build the GUI.
   GUI_ANT_BUILD_COMPILER=modern

   # I think these need to be consistent with the compiler implied by the value
   # of GUI_ANT_BUILD_COMPILER, but I'm not positive. -cjc...
   JAVA_HOME=/usr/java/j2sdk1.4.1_03
   JAVA_SRC_PATH=$(JAVA_HOME)/src.zip
else
ifeq ($(AURORA_BUILD_ENV),CJC_AT_HOME)
   CXX=ccache g++
   CC=ccache gcc
   CC_VERSION=GCC_3_2

   GNU_MP_INCLUDE_CFLAG=-I/opt/gmp-4.1.2/include
   GNU_MP_LIB_LINKAGE=-L/opt/gmp-4.1.2/lib -lgmp -lgmpxx

   XERCES_INCLUDE_CFLAG=-I/opt/xerces/include
   XERCES_LIB_LINKAGE=-L/opt/xerces/lib -lxerces-c

   BERKELEY_DB_4_LIB_DIR=/opt/db-4.0.14/lib
   BERKELEY_DB_4_INCLUDE_CFLAG=-I/opt/db-4.0.14/include
   BERKELEY_DB_4_LIB_LINKAGE=-L$(BERKELEY_DB_4_LIB_DIR) -ldb_cxx-4.0 -ldb-4.0

   XFORMS_INCLUDE_CFLAG=-I/usr/X11R6/include/X11
   XFORMS_LIB_LINKAGE=-L/usr/X11R6/lib -lforms -lnsl -lX11

   QTDIR=/usr/lib/qt3

   EFENCE_LIB_LINKAGE=-lefence

   LEX = flex -F
   LEX_LIB_LINKAGE=-lfl

   # The Makefile for the GUI needs to produce a shell script. The script is
   # written for execution in 'tcsh'.
   TCSH_PATH=/bin/tcsh

   ANT_HOME=/opt/apache-ant-1.5.3
   ANT_CMD=$(ANT_HOME)/bin/ant

   # This must be one of the valid values for Ant's build.compiler global
   # property. This defines the compiler Ant will use to build the GUI.
   GUI_ANT_BUILD_COMPILER=modern

   # I think these need to be consistent with the compiler implied by the value
   # of GUI_ANT_BUILD_COMPILER, but I'm not positive. -cjc...
   JAVA_HOME=/usr/java/j2sdk1.4.1_02
   JAVA_SRC_PATH=$(JAVA_HOME)/src.zip
else
ifeq ($(AURORA_BUILD_ENV),DPC_ON_LAPTOP)
   CC_VERSION=GCC_3_2

   #CXX=/usr/bin/g++
   CXX=/pro/aurora/tools/linux/ccache-2.2/bin/ccache /usr/bin/g++

   CC=/usr/bin/gcc

   GNU_MP_INCLUDE_CFLAG=-I/pro/aurora/tools/linux/built_with_gcc3.2/gmp-4.1.2/include
   GNU_MP_LIB_LINKAGE=-L/pro/aurora/tools/linux/built_with_gcc3.2/gmp-4.1.2/lib -lgmp -lgmpxx

   XERCESCROOT=/pro/aurora/tools/linux/built_with_gcc3.2/xerces-c-src2_1_0
   XERCES_INCLUDE_CFLAG=-I$(XERCESCROOT)/include
   XERCES_LIB_LINKAGE=-L$(XERCESCROOT)/lib -lxerces-c

   #QTDIR=/usr/lib/qt3
   QTDIR=/pro/aurora/tools/linux/built_with_gcc3.2/qt-x11-free-3.1.1/

   BERKELEY_DB_4_LIB_DIR=/pro/aurora/tools/linux/built_with_gcc3.2/BerkeleyDB.4.0/lib
   BERKELEY_DB_4_INCLUDE_CFLAG=-I/pro/aurora/tools/linux/built_with_gcc3.2/BerkeleyDB.4.0/include
   BERKELEY_DB_4_LIB_LINKAGE=-L/pro/aurora/tools/linux/built_with_gcc3.2/BerkeleyDB.4.0/lib -ldb-4.0 -ldb_cxx-4.0

   XFORMS_INCLUDE_CFLAG=-I/pro/aurora/linux/xforms/xforms/FORMS
   XFORMS_LIB_LINKAGE=/pro/aurora/linux/xforms/xforms/FORMS/libforms.a -lnsl -L/usr/X11R6/lib -lX11

   EFENCE_LIB_LINKAGE=-lefence

   LEX = flex -F
   LEX_LIB_LINKAGE=-lfl

   # The Makefile for the GUI needs to produce a shell script. The script is
   # written for execution in 'tcsh'.
   TCSH_PATH=/bin/tcsh

   ANT_HOME=/usr/local/ant
   ANT_CMD=$(ANT_HOME)/bin/ant

   # This must be one of the valid values for Ant's build.compiler global
   # property. This defines the compiler Ant will use to build the GUI.
   GUI_ANT_BUILD_COMPILER=modern

   # I think these need to be consistent with the compiler implied by the value
   # of GUI_ANT_BUILD_COMPILER, but I'm not positive. -cjc...
   JAVA_HOME=/usr/java/j2sdk1.4.1
   JAVA_SRC_PATH=$(JAVA_HOME)/src.zip
else
   $(error You need to define which build environment you are using)
endif
endif
endif
endif
endif
