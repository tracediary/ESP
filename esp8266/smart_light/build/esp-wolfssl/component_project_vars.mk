# Automatically generated build file. Do not edit.
COMPONENT_INCLUDES += $(IDF_PATH)/components/esp-wolfssl/wolfssl/include $(IDF_PATH)/components/esp-wolfssl/wolfssl/wolfssl $(IDF_PATH)/components/esp-wolfssl/wolfssl/wolfssl/wolfssl
COMPONENT_LDFLAGS += -L$(BUILD_DIR_BASE)/esp-wolfssl -L $(IDF_PATH)/components/esp-wolfssl/wolfssl/lib -lwolfssl
COMPONENT_LINKER_DEPS += $(IDF_PATH)/components/esp-wolfssl/wolfssl/lib/libwolfssl.a
COMPONENT_SUBMODULES += 
COMPONENT_LIBRARIES += esp-wolfssl
COMPONENT_LDFRAGMENTS += 
component-esp-wolfssl-build: 
