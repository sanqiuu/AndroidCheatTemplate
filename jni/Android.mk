LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
LOCAL_MODULE := libcheat
LOCAL_CPPFLAGS += -w -s -Wno-error=format-security -fvisibility=hidden -Werror -std=c++17
LOCAL_CPPFLAGS += -Wno-error=c++11-narrowing -fpermissive -Wall -fexceptions

LOCAL_C_INCLUDES += $(LOCAL_PATH)/include
LOCAL_C_INCLUDES += $(LOCAL_PATH)/include/Aim
LOCAL_C_INCLUDES += $(LOCAL_PATH)/include/Android_draw
LOCAL_C_INCLUDES += $(LOCAL_PATH)/include/Android_shm
LOCAL_C_INCLUDES += $(LOCAL_PATH)/include/Android_touch
LOCAL_C_INCLUDES += $(LOCAL_PATH)/include/Android_check
LOCAL_C_INCLUDES += $(LOCAL_PATH)/include/ImGui
LOCAL_C_INCLUDES += $(LOCAL_PATH)/include/Tools
LOCAL_C_INCLUDES += $(LOCAL_PATH)/include/native_surface

FILE_LIST += $(wildcard $(LOCAL_PATH)/src/*.c*)
FILE_LIST += $(wildcard $(LOCAL_PATH)/src/Aim/*.c*)
FILE_LIST += $(wildcard $(LOCAL_PATH)/src/Android_draw/*.c*)
FILE_LIST += $(wildcard $(LOCAL_PATH)/src/Android_read/*.c*)
FILE_LIST += $(wildcard $(LOCAL_PATH)/src/Android_shm/*.c*)
FILE_LIST += $(wildcard $(LOCAL_PATH)/src/Android_touch/*.c*)
FILE_LIST += $(wildcard $(LOCAL_PATH)/src/Android_check/*.c*)
FILE_LIST += $(wildcard $(LOCAL_PATH)/src/ImGui/*.c*)
FILE_LIST += $(wildcard $(LOCAL_PATH)/src/native_surface/*.c*)
LOCAL_SRC_FILES += $(FILE_LIST:$(LOCAL_PATH)/%=%)

LOCAL_LDFLAGS += -lEGL -lGLESv2 -lGLESv3 -landroid -llog
include $(BUILD_EXECUTABLE)
