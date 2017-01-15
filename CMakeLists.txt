# Some functions require a certain version of cmake (for example file GLOB )
cmake_minimum_required(VERSION 2.8)

# Create the project
set(PROJECT_NAME SGE)
set(CMAKE_MACOSX_RPATH 1)
project(${PROJECT_NAME})

# Add the external module path
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR}/cmake/Modules)

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
file(GLOB ${PROJECT_NAME}_HEADERS ${PROJECT_SOURCE_DIR}/include/SGE/*.h)
add_library(${PROJECT_NAME} SHARED ${SGE_SOURCES} ${SGE_HEADERS})

# Link other libraries
target_link_libraries(
	${PROJECT_NAME}
	${OPENGL_LIBRARIES}
	${OPENGL_glu_LIBRARY}
	${ASSIMP_LIBRARIES}
	${GLEW_LIBRARIES})

# Required on Unix OS family to be able to be linked into shared libraries.
set_target_properties(${PROJECT_NAME}
                      PROPERTIES POSITION_INDEPENDENT_CODE ON)
set_target_properties(${PROJECT_NAME} PROPERTIES COMPILE_FLAGS "-std=c++14")

############
# Examples #
############
option(BUILD_EXAMPLES "Build all examples." OFF)
if (BUILD_EXAMPLES)
  find_package(GLFW3 REQUIRED)
  include_directories(${GLFW_INCLUDE_DIRS})
  file(GLOB EXAMPLE1_SOURCE ${PROJECT_SOURCE_DIR}/examples/glfw_example.cpp)
  add_executable(glfw_example ${EXAMPLE1_SOURCE})
  target_link_libraries(
    glfw_example
    ${PROJECT_NAME}
    ${GLFW_LIBRARIES}
  )
  set_target_properties(glfw_example PROPERTIES COMPILE_FLAGS "-std=c++14")
endif()