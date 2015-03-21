#author:>---Ocean Zhao
#created:>--2015/03/20

SET(SOURCE_FILES #设置变量，表示所有的源文件
utils.cpp
utils.h
matrix.cpp
matrix.h
test.cpp
)

INCLUDE_DIRECTORIES( #相关头文件的目录

${PROJET_SOURCE_DIR}
)

LINK_DIRECTORIES( #相关库文件的目录

)


ADD_EXECUTABLE(${PROJECT_NAME} ${PROJECT_BINARY_DIR})