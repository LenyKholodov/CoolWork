all: build

###################################################################################################
#Путь к корню
###################################################################################################
ROOT ?= .

####################################################################################################
#Проверка версии утилиты make
####################################################################################################
ifneq (3.80,$(MAKE_VERSION))
  $(warning This makefile has written for GNU Make 3.80. Your version is $(MAKE_VERSION))
endif

###################################################################################################
#Подключение базовых настроек
###################################################################################################
include $(ROOT)/config/base.mak

###################################################################################################
#Определения
###################################################################################################
BIN_DIR         := $(ROOT)/$(BIN_DIR)
TMP_DIR         := $(ROOT)/$(TMP_DIR)
SRC_CORE_DIR    := $(ROOT)/$(SRC_CORE_DIR)
SRC_UI_DIR      := $(ROOT)/$(SRC_UI_DIR)
INCLUDE_DIR     := $(ROOT)/$(INCLUDE_DIR)
EXTERN_DIR      := $(ROOT)/$(EXTERN_DIR)
DIRS            := $(TMP_DIR) $(BIN_DIR)
DIRS            := $(sort $(DIRS))
CONFIG_DIR      := $(ROOT)/config
CORE_SOURCES    := $(wildcard $(DEFAULT_SOURCES:%=$(SRC_CORE_DIR)/%))
UI_SOURCES      := $(wildcard $(DEFAULT_SOURCES:%=$(SRC_UI_DIR)/%))
CORE_OBJS       := $(notdir $(basename $(CORE_SOURCES)))
CORE_OBJS       := $(CORE_OBJS:%=$(TMP_DIR)/%.obj)
UI_RES_SOURCES  := $(wildcard $(DEFAULT_RES_SOURCES:%=$(SRC_UI_DIR)/%))
UI_RESOURCES    := $(notdir $(basename $(UI_RES_SOURCES)))
UI_RESOURCES    := $(UI_RESOURCES:%=$(TMP_DIR)/%.resources)
UI_OBJS         := $(notdir $(basename $(UI_SOURCES)))
UI_OBJS         := $(UI_OBJS:%=$(TMP_DIR)/%.obj)
CUSTOM_H_DIRS   := $(SRC_CORE_DIR) $(SRC_UI_DIR) $(INCLUDE_DIR) $(foreach i,$(wildcard $(EXTERN_DIR)/*/),$(i)h)
CUSTOM_LIB_DIRS := $(foreach i,$(filter %/,$(wildcard $(EXTERN_DIR)/*/)),$(i)lib)
DLLS            := $(wildcard $(CUSTOM_LIB_DIRS:%=%/*.dll))
GOAL            := $(BIN_DIR)/$(GOAL).exe
CORE_DLL				:= $(BIN_DIR)/core.dll
CORE_PCH_NAME	  := $(wildcard $(SRC_CORE_DIR)/$(PCH_NAME))
UI_PCH_NAME     := $(wildcard $(SRC_UI_DIR)/$(PCH_NAME))
CORE_CFLAGS     += $(foreach dir,$(CUSTOM_H_DIRS),/I"$(dir)" )
CORE_LINK_FLAGS += $(foreach dir,$(CUSTOM_LIB_DIRS),/LIBPATH:"$(dir)" )
CORE_CFLAGS     += /nologo /c $(if $(CORE_PCH_NAME),/Yc$(notdir $(CORE_PCH_NAME)) /Fp"$(TMP_DIR)/core.pch" /FI$(notdir $(CORE_PCH_NAME)))
UI_CFLAGS			  += $(foreach dir,$(CUSTOM_LIB_DIRS),/AI"$(dir)" ) /AI"$(BIN_DIR)"
UI_CFLAGS       += $(foreach dir,$(CUSTOM_H_DIRS),/I"$(dir)" )
UI_CFLAGS       += /nologo /c $(if $(UI_PCH_NAME),/Yc$(notdir $(UI_PCH_NAME)) /Fp"$(TMP_DIR)/ui.pch" /FI$(notdir $(UI_PCH_NAME)))
#/SUBSYSTEM:WINDOWS /ENTRY:"main" /MACHINE:X86
UI_LINK_FLAGS   += $(foreach dir,$(CUSTOM_LIB_DIRS),/LIBPATH:"$(dir)" ) /LIBPATH:$(TMP_DIR) $(TMP_DIR)/$(notdir $(basename $(CORE_DLL))).lib

###################################################################################################
#Правила сборки исходников
###################################################################################################
#define scan_new_sources
#$(if $(2),for i in $(notdir $(basename $(2))); do \
#    if [ $$i.$(1) -nt $(TMP_DIR)/$$i.obj ] || \
#       $(if $(PCH_NAME),[ $(PCH_NAME) -nt $(TMP_DIR)/$$i.obj ] ||) \
#       ! [ -x $(TMP_DIR)/$$i.$(OBJ_SUFFIX) ]; \
#    then echo $(SRC_DIR)/$$i.$(1); fi; done;)
#endef

#NEW_SOURCES := $(shell $(call scan_new_sources,cpp,$(SOURCES)))
#NEW_OBJS    := $(basename $(NEW_SOURCES))
#NEW_OBJS    := $(NEW_OBJS:$(SRC_DIR)/%=$(TMP_DIR)/%.obj)

#ifdef NEW_OBJS

#$(NEW_OBJS): $(NEW_SOURCES)
#	@cl /nologo $(CPPFLAGS) $(CFLAGS) /Fo"$(TMP_DIR)\\" $(NEW_SOURCES)
#
#endif

$(TMP_DIR)/%.obj: $(SRC_CORE_DIR)/%.cpp
	@cl $< /c /nologo $(CORE_CFLAGS) $(CORE_CPPFLAGS) /Fo"$@"
	
$(TMP_DIR)/%.obj: $(SRC_UI_DIR)/%.cpp
	@cl $< /clr /c /nologo $(UI_CFLAGS) $(UI_CPPFLAGS) /Fo"$@"

$(TMP_DIR)/%.resources: $(SRC_UI_DIR)/%.resx
	@cd $(SRC_UI_DIR) && resgen $(notdir $<) $(notdir $@)
	@mv $(SRC_UI_DIR)/$(notdir $@) $@
	
$(CORE_DLL): $(CORE_OBJS)
	@echo Create $@...
	@link /nologo /dll /out:"$@" $^ $(CORE_LINK_FLAGS)
	@mv -t $(TMP_DIR) $(basename $@).lib $(basename $@).exp
	
$(GOAL): $(CORE_DLL) $(UI_OBJS) $(UI_RESOURCES)
	@echo Linking $@...
	@cp -uv $(DLLS) $(BIN_DIR)
	@link /nologo /ASSEMBLYRESOURCE:$(UI_RESOURCES) $(UI_LINK_FLAGS) /out:"$@" $(UI_OBJS) $()

###################################################################################################
#Цели сборки
###################################################################################################
.PHONY: all build rebuild clean mostlyclean prebuild run

build: prebuild $(GOAL)

run: build
#	@cd $(ROOT) && cmd /C bin\\$(notdir $(GOAL))
	@cd $(ROOT) && bin/$(notdir $(GOAL))

prebuild:
	@mkdir -p $(DIRS)
	
mostlyclean:
	@$(RM) $(TMP_DIR)	

clean:
	@$(RM) -r $(DIRS)

rebuild:
	@$(MAKE) -s clean
	@$(MAKE) -s build
