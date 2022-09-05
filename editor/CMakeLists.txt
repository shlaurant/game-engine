file(GLOB_RECURSE SRC_FILES src/*.cpp CMAKE_CONFIGURE_DEPENDS)
file(GLOB IMGUI_BASE ${CMAKE_SOURCE_DIR}/imgui/imgui*.cpp CMAKE_CONFIGURE_DEPENDS)


set(IMGUI_BE ${CMAKE_SOURCE_DIR}/imgui/backends/imgui_impl_sdl.cpp
        ${CMAKE_SOURCE_DIR}/imgui/backends/imgui_impl_sdlrenderer.cpp)

add_executable(editor ${SRC_FILES} ${IMGUI_BASE} ${IMGUI_BE})
target_include_directories(editor PUBLIC
        ${CMAKE_SOURCE_DIR}/imgui
        ${CMAKE_SOURCE_DIR}/imgui/backends
        ${CMAKE_SOURCE_DIR}/windows/includes
        ${CMAKE_SOURCE_DIR}/windows/includes/SDL2)

target_link_libraries(editor SDL2 SDL2main yaml-cppd)
target_link_directories(editor PUBLIC ${CMAKE_SOURCE_DIR}/windows/libs)