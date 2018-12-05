LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE := spine_static

LOCAL_MODULE_FILENAME := libspine

LOCAL_ARM_MODE := arm

CLASSES_DIRECTORY := $(LOCAL_PATH)
# Get a list of source files
SOURCE_FILES := $(shell find $(CLASSES_DIRECTORY) -name *.cpp)
SOURCE_FILES += $(shell find $(CLASSES_DIRECTORY) -name *.c)
SOURCE_FILES := $(sort $(SOURCE_FILES))
SOURCE_FILES := $(subst $(LOCAL_PATH)/,,$(SOURCE_FILES))

LOCAL_SRC_FILES := \
$(SOURCE_FILES)

LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/..

LOCAL_C_INCLUDES := $(LOCAL_PATH)/..

LOCAL_STATIC_LIBRARIES := cocos2dx_internal_static

include $(BUILD_STATIC_LIBRARY)
