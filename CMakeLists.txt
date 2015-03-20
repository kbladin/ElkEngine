# Some functions require a certain version of cmake
cmake_minimum_required(VERSION 2.8)

# Create the project
set(PROJECT_NAME ModelViewer)
project(${PROJECT_NAME})

# Add the external module path
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake/Modules)

# Find external packages
find_package(OPENGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(GLFW3 REQUIRED)
find_package(LEAP REQUIRED)
#find_package(ANTTWEAKBAR REQUIRED)
#find_package(CINDER REQUIRED)

if(APPLE)
  # Find external libraries
  find_library(OPENGL_FRAMEWORK OpenGL)
  find_library(COCOA_FRAMEWORK Cocoa)
endif(APPLE)

# Add include directories 
include_directories(${GLEW_INCLUDE_DIR})
include_directories(${GLFW_INCLUDE_DIRS})
include_directories(${LEAP_INCLUDE_DIRS})
#include_directories(${ANT_TWEAK_BAR_INCLUDE_PATH})
include_directories("cinder_0.8.6_mac/include")

# Our own include directory
set(COMMON_INCLUDES ${PROJECT_SOURCE_DIR}/include)
include_directories(${COMMON_INCLUDES})

set(CMAKE_CXX_FLAGS "-g")

# Creating our own library
file(GLOB Model_Viewer_lib_SOURCES ${PROJECT_SOURCE_DIR}/src/*.cpp)
file(GLOB Model_Viewer_lib_HEADERS ${PROJECT_SOURCE_DIR}/include/*.h)
# We do not want main.cpp in the library
file(GLOB to_remove ${PROJECT_SOURCE_DIR}/src/main.cpp)
list(REMOVE_ITEM Model_Viewer_lib_SOURCES ${to_remove})
list(REMOVE_ITEM Model_Viewer_lib_HEADERS ${to_remove})
add_library(Model_Viewer_lib ${Model_Viewer_lib_SOURCES} ${Model_Viewer_lib_HEADERS})

# Creating our executable
add_executable(ModelViewer src/main.cpp)
# Key idea: SEPARATE OUT your main() function into its own file so it can be its
# own executable. Separating out main() means you can add this library to be
# used elsewhere (In our case for testing).

# C++11 compatability
set_target_properties(ModelViewer PROPERTIES COMPILE_FLAGS "-std=c++11")

# Link all the libraries (including our own)
target_link_libraries(ModelViewer Model_Viewer_lib ${LEAP_LIBRARY} ${OPENGL_LIBRARIES} ${OPENGL_glu_LIBRARY} ${GLEW_LIBRARY} ${GLFW_LIBRARIES})

######### OPENGL LIBRARY (ONLY FOR OSX) #############
if(APPLE)
  target_link_libraries(ModelViewer ${OPENGL_FRAMEWORK} ${COCOA_FRAMEWORK})
endif(APPLE)
