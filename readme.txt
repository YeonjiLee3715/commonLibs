# 환경변수에 commonLibs의 경로를 commonLibs의 이름으로 등록 해 주어야 함.
# makelist나 .pro 파일 등에서 COMMON_LIBS를 선언하여 사용 할 모듈을 지정해주어야 함

# cmake를 사용하는 프로젝트에서는 .mk파일을 include
예시) Android.mk

...
COMMON_LIBS:= CBaseClass CCallbackMacro CLogger CStrManager CVideoCapture CVideoStreamer
include $(commonLibs)\commonLibs.mk
...

# qt 프로젝트에서는 commonLibs.pri 파일을 include
예시) temp.pro

...
COMMON_LIBS = CLogger CStrManager
win32:include($(COMMONLIBS)/commonLibs.pri)
else:unix:include($(commonLibs)/commonLibs.pri)
,,,


# 프로젝트에 cmnDef.h 파일을 만들어 commonLibs에 필요한 정의를 해주어야 함.

예시) cmnDef.h

#ifndef CMNDEF_H
#define CMNDEF_H

//common library용 정의파일
#define __QT_DEBUG //QLOG 사용: QT프로젝트
#define LOG_LV       0//4//_INFO

#endif // CMNDEF_H