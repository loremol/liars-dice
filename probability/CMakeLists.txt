add_library(liars_dice_probability STATIC
    src/rules.cpp
    src/odds.cpp
)
add_library(liars_dice::probability ALIAS liars_dice_probability)

target_include_directories(liars_dice_probability PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

liars_dice_enable_warnings(liars_dice_probability)

if(BUILD_TESTING)
    add_executable(liars_dice_probability_tests
        tests/odds_test.cpp
        tests/rules_test.cpp
        tests/types_test.cpp
    )

    target_link_libraries(liars_dice_probability_tests PRIVATE
        liars_dice::probability
        GTest::gtest_main
    )

    liars_dice_enable_warnings(liars_dice_probability_tests)

    include(GoogleTest)
    gtest_discover_tests(liars_dice_probability_tests)
endif()
