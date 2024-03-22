cmake_minimum_required(VERSION 3.22)
project(turn_based_game)

set(CMAKE_CXX_STANDARD 17)

if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /fp:strict")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-math-errno")
endif()

set(PORT "44444" CACHE STRING "Set the port")
set(HOST_NAME "localhost" CACHE STRING "Set the host name")

add_definitions("-DPORT=${PORT}")
add_definitions("-DHOST_NAME=\"${HOST_NAME}\"")

find_package(SFML COMPONENTS system graphics window network audio REQUIRED)
find_package(ImGui-SFML CONFIG REQUIRED)
find_package(GTest CONFIG REQUIRED)

# Add a CMake option to enable or disable Tracy Profiler
option(USE_TRACY "Use Tracy Profiler" OFF)

if (USE_TRACY)
    # Enable Tracy profiling by setting the preprocessor directive
    add_compile_definitions(TRACY_ENABLE)
    # Include Tracy's headers for the entire project
    include_directories(externals/tracy_profiler/tracy)

    # Add the Tracy source code as part of the project
    add_library(tracyClient STATIC externals/tracy_profiler/TracyClient.cpp)
endif()

file(GLOB_RECURSE DATA_FILES
            "data/*.json"
            "data/*.png"
            "data/*.jpg"
            "data/*.otf"
            )
    foreach(DATA ${DATA_FILES})
        get_filename_component(FILE_NAME ${DATA} NAME)
        get_filename_component(PATH_NAME ${DATA} DIRECTORY)
        get_filename_component(EXTENSION ${DATA} EXT)
        file(RELATIVE_PATH PATH_NAME "${CMAKE_CURRENT_SOURCE_DIR}" ${PATH_NAME})
        #MESSAGE("Data PATH: ${PATH_NAME} NAME: ${FILE_NAME}")
        set(DATA_OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/${PATH_NAME}/${FILE_NAME}")
        #MESSAGE("Data OUT PATH: ${DATA_OUTPUT}")
        add_custom_command(
                OUTPUT ${DATA_OUTPUT}
                COMMAND ${CMAKE_COMMAND} -E copy
                ${DATA}
                ${DATA_OUTPUT}
                DEPENDS ${DATA})
        list(APPEND Data_OUTPUT_FILES ${DATA_OUTPUT})
    endforeach(DATA)


    add_custom_target(
            data_target
            DEPENDS ${Data_OUTPUT_FILES}
    )

# Create the Math library
file(GLOB_RECURSE MATH_SRC_FILES libs/math/include/*.h libs/math/src/*.cpp)
add_library(math ${MATH_SRC_FILES})
set_target_properties(math PROPERTIES LINKER_LANGUAGE CXX)
target_include_directories(math PUBLIC libs/math/include/)

# Create the Common library with Math as a dependency
file(GLOB_RECURSE COMMON_SRC_FILES common/include/*.h common/src/*.cpp)
add_library(common ${COMMON_SRC_FILES})
set_target_properties(common PROPERTIES LINKER_LANGUAGE CXX)
target_include_directories(common PUBLIC common/include/)
target_link_libraries(common PRIVATE math)

# Create the physics library with math and common as dependencies.
file(GLOB_RECURSE PHYSICS_SRC_FILES physics_engine/include/*.h physics_engine/src/*.cpp)
add_library(physics ${PHYSICS_SRC_FILES})
set_target_properties(physics PROPERTIES LINKER_LANGUAGE CXX)
target_include_directories(physics PUBLIC physics_engine/include/)
target_include_directories(math PUBLIC libs/math/include/)
target_link_libraries(physics PRIVATE math common)
if (USE_TRACY)
    target_compile_definitions(physics PUBLIC TRACY_ENABLE)
    # Link the TracyClient library
    target_link_libraries(physics PRIVATE tracyClient)
endif()

# Create the network library.
file(GLOB_RECURSE NETWORK_SRC_FILES network/include/*.h network/src/*.cpp)
add_library(network ${NETWORK_SRC_FILES})
set_target_properties(network PROPERTIES LINKER_LANGUAGE CXX)
target_include_directories(network PUBLIC network/include/)
target_link_libraries(network PRIVATE sfml-network)

# Create the main library.
file(GLOB_RECURSE MAIN_SRC_FILES main/include/*.h main/src/*.cpp)
add_library(main ${MAIN_SRC_FILES})
set_target_properties(main PROPERTIES LINKER_LANGUAGE CXX)
target_include_directories(main PUBLIC main/include/)
target_link_libraries(main PRIVATE physics common network math sfml-system sfml-graphics sfml-window ImGui-SFML::ImGui-SFML)
add_dependencies(main data_target)

add_executable(server main/server_entry_point.cpp)
target_link_libraries(server PRIVATE main physics common network math sfml-system sfml-graphics sfml-window ImGui-SFML::ImGui-SFML)
if (USE_TRACY)
    target_compile_definitions(server PRIVATE TRACY_ENABLE)
    # Link the TracyClient library
    target_link_libraries(server PRIVATE tracyClient)
endif()

add_executable(client main/client_entry_point.cpp)
target_link_libraries(client PRIVATE main physics common math network sfml-system sfml-graphics sfml-window ImGui-SFML::ImGui-SFML)
if (USE_TRACY)
    target_compile_definitions(client PRIVATE TRACY_ENABLE)
    # Link the TracyClient library
    target_link_libraries(client PRIVATE tracyClient)
endif()

# Tests network library.
file(GLOB_RECURSE NETWORK_TEST_FILES network/tests/*.cpp)
foreach(test_file ${NETWORK_TEST_FILES} )
    get_filename_component(test_name ${test_file} NAME_WE)

    add_executable(${test_name} ${test_file})

    target_link_libraries(${test_name} PRIVATE network GTest::gtest GTest::gtest_main)
endforeach()

## Tests physics library.
#file(GLOB_RECURSE PHYSICS_TEST_FILES physics_engine/tests/*.cpp)
#foreach(test_file ${PHYSICS_TEST_FILES} )
#    get_filename_component(test_name ${test_file} NAME_WE)
#
#    add_executable(${test_name} ${test_file})
#
#    target_link_libraries(${test_name} PRIVATE physics common math)
#    target_link_libraries(${test_name} PRIVATE GTest::gtest GTest::gtest_main)
#endforeach()
#
#
## Tests common library.
#file(GLOB_RECURSE COMMON_TEST_FILES common/tests/*.cpp)
#foreach(test_file ${COMMON_TEST_FILES} )
#    get_filename_component(test_name ${test_file} NAME_WE)
#
#    add_executable(${test_name} ${test_file})
#
#    target_link_libraries(${test_name} PRIVATE common math)
#    target_link_libraries(${test_name} PRIVATE GTest::gtest GTest::gtest_main)
#endforeach()