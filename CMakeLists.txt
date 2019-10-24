cmake_minimum_required(VERSION 3.0)
project(synthtext)

set(CMAKE_CXX_STANDARD 11)
find_package( PkgConfig )
find_package( Boost COMPONENTS system filesystem REQUIRED )
#find_package(ICU COMPONENTS io i18n uc )

#if(NOT ICU_FOUND)
# set(ENV{PKG_CONFIG_PATH} "$ENV{PKG_CONFIG_PATH}:/home/zhengwu/opt/icu/lib/pkgconfig")
 pkg_check_modules(ICU REQUIRED icu-io icu-i18n icu-uc)
#endif()

pkg_check_modules(Pango REQUIRED pango)
pkg_check_modules(Cairo REQUIRED cairo)
pkg_check_modules(PangoFt2 REQUIRED pangoft2)
pkg_check_modules(PangoCairo REQUIRED pangocairo)
pkg_check_modules(FontConfig REQUIRED fontconfig)
pkg_check_modules(Leptonica REQUIRED lept )
pkg_check_modules(FREETYPE freetype2)
pkg_check_modules(FontConfig REQUIRED fontconfig)


include_directories( ${PROJECT_SOURCE_DIR}/src )
include_directories( ${Boost_INCLUDE_DIR} )
include_directories( ${Pango_INCLUDE_DIRS})
include_directories( ${Cairo_INCLUDE_DIRS})
include_directories(${ICU_INCLUDE_DIRS})
include_directories(${Leptonica_INCLUDE_DIRS})
include_directories(${FREETYPE_INCLUDE_DIRS})



file( GLOB synth_src
        src/font_info.cpp
        src/pango_font_info.cpp
        src/fileio.cpp
        src/normstrngs.cpp
        src/word_render.cpp
        )

file( GLOB synth_hdr
        src/font_info.h
        src/pango_font_info.h
        src/fileio.h
        src/normstrngs.h
        src/word_render.h
        )

add_library( synthlib ${synth_src} ${synth_hdr} )
target_link_libraries(synthlib
    ${Pango_LIBRARIES}
    ${Cairo_LIBRARIES}
    ${PangoCairo_LIBRARIES}
    ${PangoFt2_LIBRARIES}
    ${FontConfig_LIBRARIES}
    ${ICU_LIBRARIES}
    ${Boost_LIBRARIES}
    ${FontConfig_LIBRARIES}
    ${Leptonica_LIBRARIES}
)
target_compile_definitions  (synthlib PRIVATE -DPANGO_ENABLE_ENGINE)

add_executable(synthword synth_word.cpp)
target_link_libraries(synthword synthlib)

add_executable(synthl synth_line.cpp)
target_link_libraries(synthl synthlib)
