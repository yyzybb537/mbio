cmake_minimum_required(VERSION 2.8)

###################################################################################
project(mbio)

if (CMAKE_BUILD_TYPE)
else()
    set(CMAKE_BUILD_TYPE RELEASE)
    #set(CMAKE_BUILD_TYPE DEBUG)
endif()

message("------------ Options -------------")
message("  CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}")
message("  CMAKE_COMMAND: ${CMAKE_COMMAND}")

if (BUILD_DYNAMIC)
    message ("  build dynamic lib: yes")
else()
    message ("  build dynamic lib: no")
endif()

message("-------------- Env ---------------")
message("  CMAKE_SOURCE_DIR: ${CMAKE_SOURCE_DIR}")
message("  CMAKE_BINARY_DIR: ${CMAKE_BINARY_DIR}")
message("----------------------------------")

configure_file(${PROJECT_SOURCE_DIR}/mbio/common/cmake_config.h.in ${PROJECT_SOURCE_DIR}/mbio/common/cmake_config.h)
message("----------------------------------")

if (UNIX)
    set(CMAKE_CXX_FLAGS "-std=c++11 -fPIC -Wall -m64 ${CMAKE_CXX_FLAGS}")
    set(CMAKE_CXX_FLAGS_DEBUG "-g")
    set(CMAKE_CXX_FLAGS_RELEASE "-g -O3 -DNDEBUG")
elseif (WIN32)
    # windows platform
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MDd")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MD")
endif()

message("------------ Cxx flags -------------")
message("  CMAKE_CXX_FLAGS_${CMAKE_BUILD_TYPE}: ${CMAKE_CXX_FLAGS_${CMAKE_BUILD_TYPE}}")
message("------------------------------------")

include_directories(${PROJECT_SOURCE_DIR})
aux_source_directory(${PROJECT_SOURCE_DIR}/mbio CO_SRC_LIST)
aux_source_directory(${PROJECT_SOURCE_DIR}/mbio/common CO_SRC_LIST)

set(TARGET "mbio")
set(STATIC_T "mbio_static")

list(APPEND CO_SRC_LIST ${jump_asm_file})
list(APPEND CO_SRC_LIST ${make_asm_file})
add_library("${STATIC_T}" STATIC ${CO_SRC_LIST})
set_target_properties("${STATIC_T}" PROPERTIES OUTPUT_NAME "${TARGET}")

if (UNIX)
    install(TARGETS ${STATIC_T} LIBRARY DESTINATION "lib" ARCHIVE DESTINATION "lib")
    install(DIRECTORY ${PROJECT_SOURCE_DIR}/mbio/ DESTINATION "include/mbio" FILES_MATCHING PATTERN "*.h")

    if (BUILD_DYNAMIC)
        set(SHARED_T "mbio_dynamic")
        add_library("${SHARED_T}" SHARED ${CO_SRC_LIST})
        set_target_properties("${SHARED_T}" PROPERTIES OUTPUT_NAME "${TARGET}")
        target_link_libraries("${SHARED_T}" ${LINK_LIBS} -ldl)
        install(TARGETS ${SHARED_T} LIBRARY DESTINATION "lib" ARCHIVE DESTINATION "lib")
    endif()

    add_custom_target(debug
        COMMAND ${CMAKE_COMMAND} -DCMAKE_BUILD_TYPE=DEBUG ${CMAKE_SOURCE_DIR}
        COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target all
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        COMMENT "Switch CMAKE_BUILD_TYPE to Debug"
    )

    add_custom_target(release
        COMMAND ${CMAKE_COMMAND} -DCMAKE_BUILD_TYPE=RELEASE ${CMAKE_SOURCE_DIR}
        COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target all
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        COMMENT "Switch CMAKE_BUILD_TYPE to Release"
    )

    set(PROFILE_FLAGS "-pg ${CMAKE_CXX_FLAGS_${CMAKE_BUILD_TYPE}}")

    #message("PROFILE_FLAGS: ${PROFILE_FLAGS}")
    add_custom_target(profile
        COMMAND ${CMAKE_COMMAND} -DCMAKE_BUILD_TYPE=PROFILE -DCMAKE_CXX_FLAGS_PROFILE=\\'${PROFILE_FLAGS}\\' ${CMAKE_SOURCE_DIR}
        COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target all
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        COMMENT "Switch CMAKE_BUILD_TYPE to PROFILE"
    )

    add_custom_target(uninstall
        COMMAND rm ${CMAKE_INSTALL_PREFIX}/lib/libmbio.a ${CMAKE_INSTALL_PREFIX}/lib/libmbio.so -f
        COMMAND rm ${CMAKE_INSTALL_PREFIX}/include/mbio -rf
    )

elseif (WIN32)
    set_target_properties("${STATIC_T}" PROPERTIES COMPILE_FLAGS "/wd4819 /wd4267")
endif()

