# Automatically generated build file. Do not edit.
COMPONENT_INCLUDES += $(IDF_PATH)/components/newlib/include $(IDF_PATH)/components/newlib/newlib/port/include $(IDF_PATH)/components/newlib/newlib/include
COMPONENT_LDFLAGS += -L$(BUILD_DIR_BASE)/newlib $(IDF_PATH)/components/newlib/newlib/lib/libc.a $(IDF_PATH)/components/newlib/newlib/lib/libm.a -lnewlib
COMPONENT_LINKER_DEPS += $(IDF_PATH)/components/newlib/newlib/lib/libc.a $(IDF_PATH)/components/newlib/newlib/lib/libm.a
COMPONENT_SUBMODULES += 
COMPONENT_LIBRARIES += newlib
COMPONENT_LDFRAGMENTS += 
component-newlib-build: 
