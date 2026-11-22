include(FetchContent)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
set(USE_MSVC_RUNTIME_LIBRARY_DLL OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG 3.5.1
    GIT_SHALLOW TRUE
    SYSTEM
)

FetchContent_Declare(
    imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG v1.93.0
    GIT_SHALLOW TRUE
    SYSTEM
)

FetchContent_MakeAvailable(glfw imgui)

find_package(OpenGL REQUIRED)

add_library(imgui STATIC
    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
)

target_include_directories(imgui SYSTEM PUBLIC
    ${imgui_SOURCE_DIR}
    ${imgui_SOURCE_DIR}/backends
)

target_link_libraries(imgui PUBLIC glfw OpenGL::GL)

set(LIARS_DICE_GENERATED_DIR ${CMAKE_CURRENT_BINARY_DIR}/generated)

add_custom_command(
    OUTPUT
        ${LIARS_DICE_GENERATED_DIR}/embedded_droid_sans.cpp
        ${LIARS_DICE_GENERATED_DIR}/embedded_droid_sans.hpp
    COMMAND ${CMAKE_COMMAND}
        -DINPUT=${CMAKE_CURRENT_SOURCE_DIR}/assets/fonts/DroidSans.ttf
        -DOUTPUT_CPP=${LIARS_DICE_GENERATED_DIR}/embedded_droid_sans.cpp
        -DOUTPUT_HPP=${LIARS_DICE_GENERATED_DIR}/embedded_droid_sans.hpp
        -DSYMBOL=kDroidSansTtf
        -P ${PROJECT_SOURCE_DIR}/cmake/embed_file.cmake
    DEPENDS
        ${CMAKE_CURRENT_SOURCE_DIR}/assets/fonts/DroidSans.ttf
        ${PROJECT_SOURCE_DIR}/cmake/embed_file.cmake
    COMMENT "Embedding DroidSans.ttf"
    VERBATIM
)

add_library(liars_dice_ui STATIC
    src/app.cpp
    src/display_fit.cpp
    src/theme.cpp
    ${LIARS_DICE_GENERATED_DIR}/embedded_droid_sans.cpp
)

target_include_directories(liars_dice_ui PUBLIC src ${LIARS_DICE_GENERATED_DIR})

target_link_libraries(liars_dice_ui PUBLIC
    imgui
    liars_dice::probability
)

liars_dice_enable_warnings(liars_dice_ui)

add_executable(liars_dice src/main.cpp)

target_link_libraries(liars_dice PRIVATE liars_dice_ui)

liars_dice_enable_warnings(liars_dice)

if(MSVC)
    set_target_properties(liars_dice PROPERTIES WIN32_EXECUTABLE ON)
    target_link_options(liars_dice PRIVATE /ENTRY:mainCRTStartup)
endif()

install(TARGETS liars_dice RUNTIME DESTINATION .)
install(FILES assets/fonts/LICENSE.txt DESTINATION licenses RENAME DroidSans-LICENSE.txt)
install(FILES ${imgui_SOURCE_DIR}/LICENSE.txt DESTINATION licenses RENAME imgui-LICENSE.txt)

if(BUILD_TESTING)
    add_executable(liars_dice_ui_tests
        tests/format_test.cpp
        tests/app_state_test.cpp
        tests/display_fit_test.cpp
        tests/app_layout_test.cpp
        tests/embedded_assets_test.cpp
    )

    target_link_libraries(liars_dice_ui_tests PRIVATE
        liars_dice_ui
        GTest::gtest_main
    )

    target_compile_definitions(liars_dice_ui_tests PRIVATE
        LIARS_DICE_DROID_SANS_SOURCE="${CMAKE_CURRENT_SOURCE_DIR}/assets/fonts/DroidSans.ttf"
    )

    liars_dice_enable_warnings(liars_dice_ui_tests)

    include(GoogleTest)
    gtest_discover_tests(liars_dice_ui_tests)
endif()
