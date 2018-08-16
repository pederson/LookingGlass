# you can compile this with the following command:

CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
PROJECT(geometry2d)

#SET(CMAKE_CXX_COMPILER #"${PETSC_DIR}/${PETSC_ARCH}/bin/mpicxx")
SET(CMAKE_CXX_COMPILER "/usr/bin/g++")
SET(CMAKE_CXX_FLAGS "-std=c++14")
LIST(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/Modules/")
MESSAGE(STATUS "  *************** ")
FOREACH(item ${CMAKE_MODULE_PATH})
    MESSAGE(STATUS "  path: " ${item})
ENDFOREACH()
MESSAGE(STATUS "  *************** ")

# package config
FIND_PACKAGE( PkgConfig REQUIRED )

# GLFW
PKG_SEARCH_MODULE( GLFW3 REQUIRED glfw3 )
INCLUDE_DIRECTORIES(${GLFW3_INCLUDE_DIR} )
SET(ALL_LIBRARIES ${ALL_LIBRARIES} ${GLFW3_STATIC_LIBRARIES})

# OpenGL
SET(ALL_LIBRARIES ${ALL_LIBRARIES} ${OPENGL_gl_LIBRARY})

FILE(GLOB lg_SRC "../../src/*.cpp")

SET(INCL_FOLDER "../../include")
SET(CSG_FOLDER "../../../CompGeometry")
SET(TINYXML_FOLDER "../../../tinyxml2")

INCLUDE_DIRECTORIES(${INCL_FOLDER} ${CSG_FOLDER} ${TINYXML_FOLDER})

ADD_EXECUTABLE(geometry2d "../../../tinyxml2/tinyxml2.cpp" geometry2d.cpp ${lg_SRC} )

FOREACH(item ${INCL_FOLDER})
    MESSAGE(STATUS "  including: " ${item})
ENDFOREACH()
FOREACH(item ${ALL_LIBRARIES})
    MESSAGE(STATUS "  using lib: " ${item})
ENDFOREACH()

TARGET_LINK_LIBRARIES( geometry2d GLEW glfw3 ${ALL_LIBRARIES})
