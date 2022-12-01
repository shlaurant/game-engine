project(engine VERSION 0.0.1)

file(GLOB_RECURSE SRC_FILES src/*.cpp CMAKE_CONFIGURE_DEPENDS)
add_library(${PROJECT_NAME} ${SRC_FILES})

add_definitions(-DFUSE_EXPORT)
add_definitions(-D_DEBUG)

if (WIN32)
    target_link_libraries(${PROJECT_NAME} SDL2 SDL2main SDL2_image SDL2_ttf SDL2_mixer yaml-cppd)
    target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/windows/includes)
    target_link_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/windows/libs)
endif (WIN32)

target_precompile_headers(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/pch.h)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)

#directx includes and links
target_include_directories(${PROJECT_NAME} PUBLIC C:/Program\ Files\ \(x86\)/Windows\ Kits/10/Include)
target_link_libraries(${PROJECT_NAME} d3dcompiler d3d12 dxgi dxguid DirectXTex_d)
target_link_directories(${PROJECT_NAME} PUBLIC C:/Program\ Files\ \(x86\)/Windows\ Kits/10/Lib/10.0.19041.0/um/x64)