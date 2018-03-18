TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp \
    Shader.cpp \
    Camera.cpp \
    TextureLoader.cpp \
    TerrainTile.cpp \
    InputController.cpp \
    MapGenerator.cpp \
    WaterMapGenerator.cpp \
    HillsMapGenerator.cpp \
    UnderwaterQuadMapGenerator.cpp \
    BaseMapGenerator.cpp \
    Skybox.cpp


unix:!macx: LIBS += -L$$PWD/../../../../../usr/lib/x86_64-linux-gnu/ -lGL

INCLUDEPATH += $$PWD/../../../../../usr/lib/x86_64-linux-gnu
DEPENDPATH += $$PWD/../../../../../usr/lib/x86_64-linux-gnu

unix:!macx: LIBS += -L$$PWD/../../../../../usr/lib/x86_64-linux-gnu/ -lGLEW

INCLUDEPATH += $$PWD/../../../../../usr/lib/x86_64-linux-gnu
DEPENDPATH += $$PWD/../../../../../usr/lib/x86_64-linux-gnu

unix:!macx: LIBS += -L$$PWD/../../../../../usr/lib/x86_64-linux-gnu/ -lglfw

INCLUDEPATH += $$PWD/../../../../../usr/lib/x86_64-linux-gnu
DEPENDPATH += $$PWD/../../../../../usr/lib/x86_64-linux-gnu

unix:!macx: LIBS += -L$$PWD/../../../../../usr/lib/x86_64-linux-gnu/ -lIL

INCLUDEPATH += $$PWD/../../../../../usr/lib/x86_64-linux-gnu
DEPENDPATH += $$PWD/../../../../../usr/lib/x86_64-linux-gnu

unix:!macx: PRE_TARGETDEPS += $$PWD/../../../../../usr/lib/x86_64-linux-gnu/libIL.a

HEADERS += \
    Timer.h \
    Settings.h \
    Shader.h \
    Camera.h \
    TextureLoader.h \
    TerrainTile.h \
    InputController.h \
    MapGenerator.h \
    WaterMapGenerator.h \
    HillsMapGenerator.h \
    UnderwaterQuadMapGenerator.h \
    BaseMapGenerator.h \
    Skybox.h

DISTFILES += \
    scene.vs \
    textures/grass_test.jpg \
    textures/sand.jpg \
    textures/water.jpg \
    textures/water.png \
    textures/water2.png \
    textures/water2.jpg \
    textures/underwater_sand.jpg \
    textures/sand2.jpg \
    textures/cubemap1/back.png \
    textures/cubemap1/down.png \
    textures/cubemap1/front.png \
    textures/cubemap1/left.png \
    textures/cubemap1/right.png \
    textures/cubemap1/up.png \
    shaders/skybox.vs \
    shaders/skybox.fs \
    textures/cubemap1scaled/back.png \
    textures/cubemap1scaled/down.png \
    textures/cubemap1scaled/front.png \
    textures/cubemap1scaled/left.png \
    textures/cubemap1scaled/right.png \
    textures/cubemap1scaled/up.png \
    shaders/water.vs \
    shaders/water.fs \
    textures/water2spec.png \
    shaders/hills.vs \
    shaders/hills.fs \
    textures/grass.jpg \
    textures/grass_specular.jpg \
    textures/hill.jpg \
    textures/hill_specular.jpg \
    textures/hill_test.jpg \
    shaders/sand.vs \
    shaders/sand.fs \
    shaders/base.vs \
    shaders/base.fs \
    textures/grass_normal.jpg \
    shaders/underwater.vs \
    shaders/underwater.fs \
    textures/cubemap1fx/back.png \
    textures/cubemap1fx/down.png \
    textures/cubemap1fx/front.png \
    textures/cubemap1fx/left.png \
    textures/cubemap1fx/right.png \
    textures/cubemap1fx/up.png \
    textures/grass2.png \
    shaders/grass.vs \
    shaders/grass.fs
