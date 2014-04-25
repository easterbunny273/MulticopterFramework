QT       += core gui widgets

TARGET = cube
TEMPLATE = app

SOURCES += main.cpp \
    SerialReader.cpp \
    ReaderThread.cpp

#qtHaveModule(opengl) {
    QT += opengl

    SOURCES += mainwidget.cpp \
       geometryengine.cpp

    HEADERS += \
        mainwidget.h \
        geometryengine.h \
    SerialReader.h \
    ReaderThread.h

    RESOURCES += \
        shaders.qrc \
        textures.qrc
#}

# install
target.path = $$[QT_INSTALL_EXAMPLES]/opengl/cube
INSTALLS += target

simulator: warning(This example might not fully work on Simulator platform)
