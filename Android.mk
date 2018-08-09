LOCAL_PATH:= $(call my-dir)


include $(CLEAR_VARS)
LOCAL_C_INCLUDES:= external/telnetServer
LOCAL_SRC_FILES:=    \
            SystemMain.c   \
            SystemThread.c \
            TelnetServer.c
LOCAL_MODULE := TelnetServer
LOCAL_SHARED_LIBRARIES:= libcutils libutils
LOCAL_MODULE_TAGS := optional

include $(BUILD_EXECUTABLE)
