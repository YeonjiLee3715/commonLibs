# you might need to define commonLibs=(drive:/own commonLibs path) before calling the ndk-build
#jni에서 COMMON_LIBS를 선언해야 함.
#COMMON_LIBS:= CBaseClass CCallbackMacro CLogger cmnOpenCVDef cmnTypeDef CStrManager CVideoCapture CVideoStreamer
# jni에서 CLogger를 사용하려면 Android.mk에서 LOCAL_LDLIBS에 -llog를 추가해야 함

define add_common_module
    LOCAL_SRC_FILES+=$(commonLibs)/$1.cpp
endef

LOCAL_C_INCLUDES+=$(commonLibs)
$(foreach module,${COMMON_LIBS},$(eval $(call add_common_module,$(module))))