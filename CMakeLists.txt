cmake_minimum_required(VERSION 3.6)
project(nova)

set(CMAKE_CXX_STANDARD 11)

option(WITH_GEANT4_UIVIS "Build example with Geant4 UI and Vis drivers" ON)
if(WITH_GEANT4_UIVIS)
    find_package(Geant4 REQUIRED ui_all vis_all)
else()
    find_package(Geant4 REQUIRED)
endif()

include(${Geant4_USE_FILE})
include_directories(${PROJECT_SOURCE_DIR}/include)

file(GLOB sources ${PROJECT_SOURCE_DIR}/src/*.cc)
file(GLOB headers ${PROJECT_SOURCE_DIR}/include/*.hh)

set(CRY_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/cry_v1.7/src)
set(CRY_LIBRARIES ${PROJECT_SOURCE_DIR}/cry_v1.7/lib/libCRY.a)
include_directories(${CRY_INCLUDE_DIRS})

find_package(ROOT)
include_directories(${ROOT_INCLUDE_DIRS})

add_executable(LXe LXe.cc ${sources} ${headers})
target_link_libraries(LXe ${Geant4_LIBRARIES} ${ROOT_LIBRARIES} ${CRY_LIBRARIES})
