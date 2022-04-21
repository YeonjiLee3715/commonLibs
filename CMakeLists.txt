##common sources
#SET( COMMON_LIBS CLogger CStrManager )
#include( $ENV{commonLibs}/CMakeLists.txt )
#add_executable(confnetserver ${PROJECT_SRC} ${NETWORK_SRC} ${COMMON_SRC} )
if("COMMON_LIBS_PATH" STREQUAL "")
    # this branch will be taken
    message("COMMON_LIBS_PATH empty get path from env")
    SET( COMMON_LIBS_PATH $ENV{commonLibs} )
endif()

string(REPLACE "\\" "/" COMMON_LIBS_PATH ${COMMON_LIBS_PATH})

foreach( arg IN LISTS COMMON_LIBS )
    file(GLOB COMMON_MODULE
         ${COMMON_LIBS_PATH}/${arg}.h
         ${COMMON_LIBS_PATH}/${arg}.cpp
    )

    SET(COMMON_SRC ${COMMON_SRC} ${COMMON_MODULE} )
endforeach()

include_directories( ${COMMON_LIBS_PATH} )
