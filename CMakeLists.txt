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

file(GLOB cry_sources ${PROJECT_SOURCE_DIR}/cry_v1.7/src/*.cc)
set(CRY_LIBRARIES ${PROJECT_SOURCE_DIR}/cry_v1.7/lib/libCRY.a)
include_directories(${PROJECT_SOURCE_DIR}/cry_v1.7/src)

find_package(ROOT)
include_directories(${ROOT_INCLUDE_DIRS})

add_executable(nova nova.cc ${sources} ${headers} ${cry_sources})
target_link_libraries(nova ${Geant4_LIBRARIES} ${ROOT_LIBRARIES} ${CRY_LIBRARIES})

set(NOVA_SCRIPTS
        vis.mac
        gui.mac
        run.mac
        run.png
        general_particle_source.mac
        )

foreach(_script ${NOVA_SCRIPTS})
    configure_file(
            ${PROJECT_SOURCE_DIR}/${_script}
            ${PROJECT_BINARY_DIR}/${_script}
            COPYONLY
    )
endforeach()

add_custom_command(TARGET nova PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${PROJECT_SOURCE_DIR}/input
        ${PROJECT_BINARY_DIR}/input)
