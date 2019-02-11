LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

$(call import-add-path,$(LOCAL_PATH))
$(call import-add-path,$(LOCAL_PATH)/../../../cocos2d)
$(call import-add-path,$(LOCAL_PATH)/../../../cocos2d/external)
$(call import-add-path,$(LOCAL_PATH)/../../../cocos2d/cocos)
$(call import-add-path,$(LOCAL_PATH)/../../../cocos2d/cocos/audio/include)

LOCAL_MODULE := MyGame_shared

LOCAL_MODULE_FILENAME := libMyGame

CLASSES_DIRECTORY := $(LOCAL_PATH)/../../../Classes
# Get a list of source files
SOURCE_FILES := $(shell find $(CLASSES_DIRECTORY) -name *.cpp)
SOURCE_FILES := $(sort $(SOURCE_FILES))
SOURCE_FILES := $(subst $(LOCAL_PATH)/,,$(SOURCE_FILES))

LOCAL_SRC_FILES := $(LOCAL_PATH)/hellocpp/main.cpp \
$(SOURCE_FILES)

LOCAL_C_INCLUDES := $(LOCAL_PATH)/../../../Classes \
$(LOCAL_PATH)/../../../Classes/sb \
$(LOCAL_PATH)/../../../Classes/base \
$(LOCAL_PATH)/../../../Classes/content \
$(LOCAL_PATH)/../../../Classes/gift \
$(LOCAL_PATH)/../../../Classes/menu \
$(LOCAL_PATH)/../../../Classes/popup \
$(LOCAL_PATH)/../../../Classes/ranking \
$(LOCAL_PATH)/../../../Classes/shop \
$(LOCAL_PATH)/../../../Classes/test \
$(LOCAL_PATH)/../../../Classes/ui \

# _COCOS_HEADER_ANDROID_BEGIN
# _COCOS_HEADER_ANDROID_END


LOCAL_STATIC_LIBRARIES := cocos2dx_static

# _COCOS_LIB_ANDROID_BEGIN
# _COCOS_LIB_ANDROID_END

include $(BUILD_SHARED_LIBRARY)

$(call import-module,.)

# _COCOS_LIB_IMPORT_ANDROID_BEGIN
# _COCOS_LIB_IMPORT_ANDROID_END
