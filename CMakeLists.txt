# Some functions require a certain version of cmake (for example file GLOB )
cmake_minimum_required(VERSION 2.8)

# Create the project
set(PROJECT_NAME SGE)
project(${PROJECT_NAME})

# Add the external module path
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake/Modules)

# Find external packages
find_package(OPENGL REQUIRED)
find_package(GLEW 	REQUIRED)
find_package(ASSIMP REQUIRED)
find_package(GLM 	REQUIRED)
if(APPLE)
  find_library(OPENGL_FRAMEWORK OpenGL)
  find_library(COCOA_FRAMEWORK Cocoa)
endif(APPLE)

# Add include directories
include_directories(${GLEW_INCLUDE_DIR})
include_directories(${ASSIMP_INCLUDE_DIRS})
include_directories(${GLM_INCLUDE_DIR})

# Our own include directory
set(${PROJECT_NAME}_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/include
                                 ${GLEW_INCLUDE_DIR}
                                 ${ASSIMP_INCLUDE_DIRS}
                                 ${GLM_INCLUDE_DIR}
    CACHE INTERNAL "${PROJECT_NAME}: Include Directories" FORCE)
include_directories(${${PROJECT_NAME}_INCLUDE_DIRS})

set(CMAKE_CXX_FLAGS "-g")

# Creating our own SGE library
file(GLOB ${PROJECT_NAME}_SOURCES ${PROJECT_SOURCE_DIR}/src/*.cpp)
file(GLOB ${PROJECT_NAME}_HEADERS ${PROJECT_SOURCE_DIR}/include/*.h)
add_library(${PROJECT_NAME} SHARED ${SGE_SOURCES} ${SGE_HEADERS})

# Link other libraries
target_link_libraries(
	${PROJECT_NAME}
	${OPENGL_LIBRARIES}
	${OPENGL_glu_LIBRARY}
	${ASSIMP_LIBRARIES}
	${GLEW_LIBRARY})

# Required on Unix OS family to be able to be linked into shared libraries.
set_target_properties(${PROJECT_NAME}
                      PROPERTIES POSITION_INDEPENDENT_CODE ON)