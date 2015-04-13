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
    src/lib \
    src/mike \
    src/shaderPrograms \
    src/ui

DEPENDPATH += \
    glm \
    src \
    src/lib \
    src/mike \
    src/shaderPrograms \
    src/ui

SOURCES += \
    src/ui/mainwindow.cpp \
    src/main.cpp \
    src/glwidget.cpp \
    src/lib/resourceloader.cpp \
    src/lib/openglshape.cpp \
    src/lib/errorchecker.cpp \
    src/hairobject.cpp \
    src/mike/hair.cpp \
    src/mike/simulation.cpp \
    src/shaderPrograms/shaderprogram.cpp \
    src/mike/integrator.cpp \
    src/lib/objloader.cpp \
    src/objmesh.cpp \
    src/shaderPrograms/hairshaderprogram.cpp \
    src/shaderPrograms/meshshaderprogram.cpp \
    src/ui/hairinterface.cpp \
    src/shaderPrograms/texturedquadshaderprogram.cpp \
    src/texture.cpp \
    src/framebuffer.cpp \
    src/quad.cpp \
    src/shaderPrograms/hairopacityshaderprogram.cpp \
    src/meshocttree.cpp


HEADERS += \
    src/ui/mainwindow.h \
    src/glwidget.h \
    src/lib/resourceloader.h \
    src/lib/openglshape.h \
    src/lib/errorchecker.h \
    src/hairobject.h \
    src/hairCommon.h \
    src/mike/hair.h \
    src/mike/simulation.h \
    src/shaderPrograms/shaderprogram.h \
    src/mike/integrator.h \
    src/lib/objloader.hpp \
    src/objmesh.h \
    src/shaderPrograms/hairshaderprogram.h \
    src/shaderPrograms/meshshaderprogram.h \
    src/ui/hairinterface.h \
    src/shaderPrograms/texturedquadshaderprogram.h \
    src/texture.h \
    src/framebuffer.h \
    src/quad.h \
    src/shaderPrograms/hairopacityshaderprogram.h \
    src/meshocttree.h


FORMS += src/mainwindow.ui

OTHER_FILES += \
    shaders/mesh.frag \
    shaders/mesh.vert \
    shaders/hair.frag \
    shaders/hair.geom \
    shaders/hair.tcs \
    shaders/hair.tes \
    shaders/hair.vert \
    shaders/texturedquad.vert \
    shaders/texturedquad.frag \
    shaders/hairOpacity.frag


RESOURCES += \
    shaders/shaders.qrc \
    models/models.qrc \
    images/images.qrc

QMAKE_CXXFLAGS_WARN_ON += -Wno-unknown-pragmas

LIBS += "-L$$PWD/glew-1.10.0/lib" -lGLEW

INCLUDEPATH += glew-1.10.0/include
DEPENDPATH += glew-1.10.0/include
