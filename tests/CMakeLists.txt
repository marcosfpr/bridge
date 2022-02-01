# set env variables
# set(OHSUMED "${CMAKE_CURRENT_SOURCE_DIR}/benchmarks/OHSUMED")

find_package(Threads REQUIRED)

add_executable(
  test_bridge
  main.cpp
  unit/analyzer_test.cpp
  unit/schema_options_test.cpp
  unit/field_test.cpp
  unit/term_test.cpp
)

# add_executable(
#         func_test_bridge
#         functional/something.cpp
# )

target_compile_features(test_bridge PUBLIC cxx_std_20)

if (BUILD_CONAN)
        target_link_libraries(test_bridge bridge ${CONAN_LIBS}  Threads::Threads)
else()
        # GTest library
        CPMAddPackage(
        NAME googletest
        GITHUB_REPOSITORY google/googletest
        GIT_TAG release-1.10.0
        VERSION 1.10.0
        OPTIONS "INSTALL_GTEST OFF" "gtest_force_shared_crt"
        )
        target_link_libraries(test_bridge bridge gtest gtest_main)
endif()

include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

target_link_libraries(test_bridge bridge)

# Create testing target and redirect its output to `Testing` folder
add_test(NAME test_bridge COMMAND test_bridge WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/Testing)
# add_test(NAME func_ltrpp COMMAND func_ltrpp WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/Testing)

set_tests_properties(test_bridge
        PROPERTIES
        TIMEOUT 30
#        ENVIRONMENT MOVIES_BENCHMARK=${MOVIES_BENCHMARK}
)
# set_tests_properties(func_ltrpp
#         PROPERTIES
#         TIMEOUT 30
#         ENVIRONMENT MOVIES_BENCHMARK=${MOVIES_BENCHMARK}
#)

include(FindValgrind)

if(VALGRIND_FOUND)

        add_custom_target(valgrind_test_bridge
                COMMAND ${VALGRIND_PROGRAM} --leak-check=full $<TARGET_FILE:test_bridge>
                DEPENDS test_bridge
                )
        # add_custom_target(valgrind_func_ltrpp
        #         COMMAND ${VALGRIND_PROGRAM} --leak-check=full $<TARGET_FILE:func_ltrpp>
        #         DEPENDS func_ltrpp
        #         )
                
endif()