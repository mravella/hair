QT += core gui opengl

TARGET = hair
TEMPLATE = app

unix:!macx: LIBS += -lGLU

INCLUDEPATH += src glm
DEPENDPATH += src glm

SOURCES += \
    src/mainwindow.cpp \
    src/main.cpp \
    src/glwidget.cpp \
    src/resourceloader.cpp \
    src/openglshape.cpp \
    src/errorchecker.cpp


HEADERS += \
    src/mainwindow.h \
    src/glwidget.h \
    src/resourceloader.h \
    src/sphere.h \
    src/openglshape.h \
    src/errorchecker.h

FORMS += src/mainwindow.ui

OTHER_FILES += \
    shaders/full.vert \
    shaders/full.frag \
    shaders/full.tes \
    shaders/full.tcs \
    shaders/full.geom \
    shaders/basic.vert \
    shaders/basic.frag


RESOURCES += \
    shaders/shaders.qrc

QMAKE_CXXFLAGS_WARN_ON += -Wno-unknown-pragmas

LIBS += "-L$$PWD/glew-1.10.0/lib" -lGLEW

INCLUDEPATH += glew-1.10.0/include
DEPENDPATH += glew-1.10.0/include
