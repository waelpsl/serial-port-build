LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := serial_port
LOCAL_SRC_FILES := serial_port_jni.c
LOCAL_LDLIBS := -llog
LOCAL_CFLAGS := -D_GNU_SOURCE -fPIC
include $(BUILD_SHARED_LIBRARY)
