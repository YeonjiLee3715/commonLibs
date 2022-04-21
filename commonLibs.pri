isEmpty(COMMONLIBS_PATH) {
    message( 'COMMONLIBS_PATH is empty. use default value.' )
    COMMONLIBS_PATH = $$(commonLibs)
    message( $${COMMONLIBS_PATH} )
} else {
    message( 'COMMONLIBS_PATH is not empty.' )
    message( $${COMMONLIBS_PATH} )
}

INCLUDEPATH += $${COMMONLIBS_PATH}
DEPENDPATH += $${COMMONLIBS_PATH}

for (var, $$list($${COMMON_LIBS})) {
    HEADERS += $${COMMONLIBS_PATH}/$${var}.h
    exists( $${COMMONLIBS_PATH}/$${var}.cpp ) {
        SOURCES += $${COMMONLIBS_PATH}/$${var}.cpp
    }
    equals(var, "CStrManager") {
        equals( CMAKE_CXX_COMPILER_ID, "GNU|Clang" ) {
            QMAKE_CXXFLAGS += -fpermissive
        }
    }
}

HEADERS += \
    $${COMMONLIBS_PATH}/commonLibsDef.h
