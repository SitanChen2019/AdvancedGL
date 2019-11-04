CMAKE_MINIMUM_REQUIRED(VERSION 3.12)

project(AdvancedGL)
set(CMAKE_CXX_STANDARD  14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#Debug, Release, RelWithDebInfo, MinSizeRel
set(CMAKE_BUILD_TYPE Debug)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

# ${CMAKE_SYSTEM_NAME} can be Windows/Darwin/Linux
if( ${CMAKE_SYSTEM_NAME} STREQUAL Linux )
    set (OpenGL_GL_PREFERENCE GLVND)
endif()

set(THIRD_PARTY_ROOT ${CMAKE_SOURCE_DIR}/thirdparty)



add_subdirectory(./thirdparty/glfw-3.3)
add_subdirectory(./thirdparty/assimp)
add_subdirectory(./PBD)

file(GLOB IMGUI_SRC  ${THIRD_PARTY_ROOT}/imgui/src/*.cpp)
set(THIRD_PARTY_SOURCE
        ${THIRD_PARTY_ROOT}/glad/src/glad.c
        ${IMGUI_SRC} )

file(GLOB FRAMEWORK_SRC Framework/src/*.cpp Framework/src/*.h)
file(GLOB DEMO_MODELDISPLAY_SRC
        Demos/ModelDisplay/*.h
        Demos/ModelDisplay/*.cpp
        Demos/CharacterAnimation/*.h
        Demos/CharacterAnimation/*.cpp
        Demos/TessellationShader/*.h
        Demos/TessellationShader/*.cpp
        Demos/Untangle/*.h
        Demos/Untangle/*.cpp
        Demos/Triangulation/*.h
        Demos/Triangulation/*.cpp
        )
add_executable(DEMO main.cpp
        ${THIRD_PARTY_SOURCE}
        ${FRAMEWORK_SRC}
        ${DEMO_MODELDISPLAY_SRC}
        )

target_include_directories(DEMO PRIVATE
        ${THIRD_PARTY_ROOT}/glfw-3.3/include
        ${THIRD_PARTY_ROOT}/glad/include
        ${THIRD_PARTY_ROOT}/imgui/include
        ${THIRD_PARTY_ROOT}/assimp/include
        ${THIRD_PARTY_ROOT}/boost-1.71/include
        ${THIRD_PARTY_ROOT}/glm/include
        ${THIRD_PARTY_ROOT}/cst/include
        Framework/include)




target_compile_definitions( DEMO PRIVATE -DIMGUI_IMPL_OPENGL_LOADER_GLAD)
target_link_libraries(DEMO PRIVATE glfw assimp PBDSim)



