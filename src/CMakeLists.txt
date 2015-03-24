#author:>---Ocean Zhao
#created:>--2015/03/20

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)

SET(SOURCE_FILES #设置变量，表示所有的源文件
utils.cpp
utils.h
matrix.cpp
matrix.h
graphicsAPI.h
graphicsAPI.cpp
myRenderer.h
myRenderer.cpp
camera.h
camera.cpp
test.cpp
)



INCLUDE_DIRECTORIES( #相关头文件的目录

${PROJET_SOURCE_DIR}
)

LINK_DIRECTORIES( #相关库文件的目录

)


ADD_EXECUTABLE(${PROJECT_NAME} utils.cpp
utils.h
matrix.cpp
matrix.h
graphicsAPI.h
graphicsAPI.cpp
myRenderer.h
myRenderer.cpp
camera.h
camera.cpp
test.cpp)