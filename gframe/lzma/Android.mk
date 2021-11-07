LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE    := clzma
LOCAL_SRC_FILES := ./lib/$(TARGET_ARCH_ABI)/libclzma.a

include $(PREBUILT_STATIC_LIBRARY)

