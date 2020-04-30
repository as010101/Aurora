## cjc: Adapted from...
##
## Makefile for Byacc/Java
## Date: 06 Aug 00
## Author: Bob Jamison
#################################################

#CC      = gcc
#CFLAGS  = -O
#LDFLAGS = -static
#VERS    = byaccj1.1
#JAVA    = /usr/java

GUI_BYACCJ_OBJS = \
	$(GUI_BYACCJ_BASE_DIR)/closure.o \
	$(GUI_BYACCJ_BASE_DIR)/error.o \
	$(GUI_BYACCJ_BASE_DIR)/lalr.o \
	$(GUI_BYACCJ_BASE_DIR)/lr0.o \
	$(GUI_BYACCJ_BASE_DIR)/main.o \
	$(GUI_BYACCJ_BASE_DIR)/mkpar.o \
	$(GUI_BYACCJ_BASE_DIR)/output.o \
	$(GUI_BYACCJ_BASE_DIR)/reader.o \
	$(GUI_BYACCJ_BASE_DIR)/skeleton.o \
	$(GUI_BYACCJ_BASE_DIR)/symtab.o \
	$(GUI_BYACCJ_BASE_DIR)/verbose.o \
	$(GUI_BYACCJ_BASE_DIR)/warshall.o

$(GUI_BYACCJ_APP): $(GUI_BYACCJ_OBJS)
	$(CC) -o $@ \
	$(GUI_BYACCJ_OBJS)

$(GUI_BYACCJ_BASE_DIR)/%.o: $(GUI_BYACCJ_BASE_DIR)/%.c
	$(CC) $(CFLAGS) $(INCLUDE_FLAGS) -c $< -o $@

clean_gui_byaccj:
	rm -f $(GUI_BYACCJ_APP)
	rm -f $(GUI_BYACCJ_OBJS)
	rm -f $(GUI_BYACCJ_BASE_DIR)/y.tab.c

###
$(GUI_BYACCJ_BASE_DIR)/closure.o: \
	$(GUI_BYACCJ_BASE_DIR)/closure.c \
	$(GUI_BYACCJ_BASE_DIR)/defs.h

$(GUI_BYACCJ_BASE_DIR)/error.o: \
	$(GUI_BYACCJ_BASE_DIR)/error.c \
	$(GUI_BYACCJ_BASE_DIR)/defs.h

$(GUI_BYACCJ_BASE_DIR)/lalr.o: \
	$(GUI_BYACCJ_BASE_DIR)/lalr.c \
	$(GUI_BYACCJ_BASE_DIR)/defs.h

$(GUI_BYACCJ_BASE_DIR)/lr0.o: \
	$(GUI_BYACCJ_BASE_DIR)/lr0.c \
	$(GUI_BYACCJ_BASE_DIR)/defs.h

$(GUI_BYACCJ_BASE_DIR)/main.o: \
	$(GUI_BYACCJ_BASE_DIR)/main.c \
	$(GUI_BYACCJ_BASE_DIR)/defs.h

$(GUI_BYACCJ_BASE_DIR)/mkpar.o: \
	$(GUI_BYACCJ_BASE_DIR)/mkpar.c \
	$(GUI_BYACCJ_BASE_DIR)/defs.h

$(GUI_BYACCJ_BASE_DIR)/output.o: \
	$(GUI_BYACCJ_BASE_DIR)/output.c \
	$(GUI_BYACCJ_BASE_DIR)/defs.h

$(GUI_BYACCJ_BASE_DIR)/reader.o: \
	$(GUI_BYACCJ_BASE_DIR)/reader.c \
	$(GUI_BYACCJ_BASE_DIR)/defs.h

$(GUI_BYACCJ_BASE_DIR)/skeleton.o: \
	$(GUI_BYACCJ_BASE_DIR)/skeleton.c \
	$(GUI_BYACCJ_BASE_DIR)/defs.h

$(GUI_BYACCJ_BASE_DIR)/symtab.o: \
	$(GUI_BYACCJ_BASE_DIR)/symtab.c \
	$(GUI_BYACCJ_BASE_DIR)/defs.h

$(GUI_BYACCJ_BASE_DIR)/verbose.o: \
	$(GUI_BYACCJ_BASE_DIR)/verbose.c \
	$(GUI_BYACCJ_BASE_DIR)/defs.h

$(GUI_BYACCJ_BASE_DIR)/warshall.o: \
	$(GUI_BYACCJ_BASE_DIR)/warshall.c \
	$(GUI_BYACCJ_BASE_DIR)/defs.h
