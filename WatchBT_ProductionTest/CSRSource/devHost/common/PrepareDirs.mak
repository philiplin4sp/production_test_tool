RESULT = $(HOSTBUILD_RESULT)
INCLUDE = $(RESULT)\include

# For win32 directories
RESULT_WIN32 = $(RESULT)\x86win32
BIN_WIN32 = $(RESULT_WIN32)\bin
LIB_WIN32 = $(RESULT_WIN32)\lib
IMAGE_WIN32 = $(RESULT_WIN32)\image

# For Pocket PC 2003 (ARMV4) directories
RESULT_POCKET_PC_2003 = $(RESULT)\armv4ce4
BIN_POCKET_PC_2003 = $(RESULT_POCKET_PC_2003)\bin
LIB_POCKET_PC_2003 = $(RESULT_POCKET_PC_2003)\lib
IMAGE_POCKET_PC_2003 = $(RESULT_POCKET_PC_2003)\image

# For Windows Mobile 5.0 Pocket PC SDK (ARMV4I) directories
RESULT_WIN_MOBILE_5 = $(RESULT)\armv4iwinm5
BIN_WIN_MOBILE_5 = $(RESULT_WIN_MOBILE_5)\bin
LIB_WIN_MOBILE_5 = $(RESULT_WIN_MOBILE_5)\lib
IMAGE_WIN_MOBILE_5 = $(RESULT_WIN_MOBILE_5)\image

# For Windows Mobile 6 Professional SDK (ARMV4I) directories
RESULT_WIN_MOBILE_6 = $(RESULT)\armv4iwinm6
BIN_WIN_MOBILE_6 = $(RESULT_WIN_MOBILE_6)\bin
LIB_WIN_MOBILE_6 = $(RESULT_WIN_MOBILE_6)\lib
IMAGE_WIN_MOBILE_6 = $(RESULT_WIN_MOBILE_6)\image

ALL = $(INCLUDE) $(RESULT_WIN32) $(BIN_WIN32) $(BIN_WIN32)\debug $(BIN_WIN32)\release $(LIB_WIN32) $(LIB_WIN32)\debug $(LIB_WIN32)\release $(IMAGE_WIN32) 
#		$(RESULT_POCKET_PC_2003) $(BIN_POCKET_PC_2003) $(BIN_POCKET_PC_2003)\debug $(BIN_POCKET_PC_2003)\release $(LIB_POCKET_PC_2003) $(LIB_POCKET_PC_2003)\debug $(LIB_POCKET_PC_2003)\release $(IMAGE_POCKET_PC_2003) \
#		$(RESULT_WIN_MOBILE_5) $(BIN_WIN_MOBILE_5) $(BIN_WIN_MOBILE_5)\debug $(BIN_WIN_MOBILE_5)\release $(LIB_WIN_MOBILE_5) $(LIB_WIN_MOBILE_5)\debug $(LIB_WIN_MOBILE_5)\release $(IMAGE_WIN_MOBILE_5) \
#		$(RESULT_WIN_MOBILE_6) $(BIN_WIN_MOBILE_6) $(BIN_WIN_MOBILE_6)\debug $(BIN_WIN_MOBILE_6)\release $(LIB_WIN_MOBILE_6) $(LIB_WIN_MOBILE_6)\debug $(LIB_WIN_MOBILE_6)\release $(IMAGE_WIN_MOBILE_6)

all: $(ALL)

$(RESULT):
	if not exist $@ mkdir $@

$(ALL): $(@D)
	if not exist $@ mkdir $@
