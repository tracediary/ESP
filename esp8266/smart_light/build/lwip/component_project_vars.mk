# Automatically generated build file. Do not edit.
COMPONENT_INCLUDES += $(IDF_PATH)/components/lwip/include $(IDF_PATH)/components/lwip/include/lwip/apps $(IDF_PATH)/components/lwip/include/lwip $(IDF_PATH)/components/lwip/lwip/src/include $(IDF_PATH)/components/lwip/lwip/src/include/posix $(IDF_PATH)/components/lwip/port/esp8266/include $(IDF_PATH)/components/lwip/port/esp8266/include/port
COMPONENT_LDFLAGS += -L$(BUILD_DIR_BASE)/lwip -llwip
COMPONENT_LINKER_DEPS += 
COMPONENT_SUBMODULES += 
COMPONENT_LIBRARIES += lwip
COMPONENT_LDFRAGMENTS += $(IDF_PATH)/components/lwip/linker.lf
component-lwip-build: 
