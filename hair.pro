QT += core gui opengl

TARGET = hair
TEMPLATE = app

unix:!macx {
    LIBS += -lGLU
    QMAKE_CXXFLAGS += -std=c++11
}
macx {
    QMAKE_MACOSX_DEPLOYMENT_TARGET=10.9
    QMAKE_CFLAGS_X86_64 += -mmacosx-version-min=10.7
    QMAKE_CXXFLAGS_X86_64 = $$QMAKE_CFLAGS_X86_64
    CONFIG += c++11
}

INCLUDEPATH += \
    glm \
    src \
    src/mike \
    src/shaderPrograms \
    src/ui

DEPENDPATH += \
    glm \
    src \
    src/mike \
    src/shaderPrograms \
    src/ui

SOURCES += \
    src/ui/mainwindow.cpp \
    src/main.cpp \
    src/glwidget.cpp \
    src/resourceloader.cpp \
    src/openglshape.cpp \
    src/errorchecker.cpp \
    src/hairpatch.cpp \
    src/hairobject.cpp \
    src/mike/hair.cpp \
    src/mike/simulation.cpp \
    src/shaderPrograms/shaderprogram.cpp \
    src/mike/integrator.cpp \
    src/objloader.cpp \
    src/objmesh.cpp \
    src/shaderPrograms/hairshaderprogram.cpp \
    src/shaderPrograms/meshshaderprogram.cpp \
    src/ui/hairinterface.cpp


HEADERS += \
    src/ui/mainwindow.h \
    src/glwidget.h \
    src/resourceloader.h \
    src/sphere.h \
    src/openglshape.h \
    src/errorchecker.h \
    src/hairpatch.h \
    src/hairobject.h \
    src/hairCommon.h \
    src/mike/hair.h \
    src/mike/simulation.h \
    src/shaderPrograms/shaderprogram.h \
    src/mike/integrator.h \
    src/objloader.hpp \
    src/objmesh.h \
    src/shaderPrograms/hairshaderprogram.h \
    src/shaderPrograms/meshshaderprogram.h \
    src/ui/hairinterface.h


FORMS += src/mainwindow.ui

OTHER_FILES += \
    shaders/mesh.frag \
    shaders/mesh.vert \
    shaders/hair.frag \
    shaders/hair.geom \
    shaders/hair.tcs \
    shaders/hair.tes \
    shaders/hair.vert


RESOURCES += \
    shaders/shaders.qrc \
    models/models.qrc \
    images/images.qrc

QMAKE_CXXFLAGS_WARN_ON += -Wno-unknown-pragmas

LIBS += "-L$$PWD/glew-1.10.0/lib" -lGLEW

INCLUDEPATH += glew-1.10.0/include
DEPENDPATH += glew-1.10.0/include
