#author:>---Ocean Zhao
#created:>--2015/03/20

CMAKE_MINIMUM_REQUIRED(VERSION 2.8) #cmake 最低版本要求，低于2.6 构建过程会被终止。

PROJECT(graphics_task) #定义工程名称
MESSAGE(STATUS "Project: graphics_task") #打印相关消息消息
MESSAGE(STATUS "Project Directory: ${PROJECT_SOURCE_DIR}")

SET(CMAKE_BUILE_TYPE DEBUG) #指定编译类型

#SET(CMAKE_C_FLAGS_DEBUG "-g -Wall") #指定编译器

ADD_SUBDIRECTORY(src) #添加子目录