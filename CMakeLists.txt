cmake_minimum_required(VERSION 2.8.7)
project(nativeService C)

# set link directory
#link_directories(${CMAKE_SOURCE_DIR}/pkg_$ENV{ARCH}/lib)

# ---
# add include files
include_directories(${CMAKE_SOURCE_DIR})
include_directories(${CMAKE_SOURCE_DIR}/src)
include_directories(${CMAKE_SOURCE_DIR}/include)

# ---
# find required packages
include(FindPkgConfig)

pkg_check_modules(GTHREAD2 REQUIRED gthread-2.0)
include_directories(${GTHREAD2_INCLUDE_DIRS})

pkg_check_modules(PBNJSON REQUIRED pbnjson_c)
include_directories(${PBNJSON_INCLUDE_DIRS})

# -- check for glib 2.0
pkg_check_modules(GLIB2 REQUIRED glib-2.0)
include_directories(${GLIB2_INCLUDE_DIRS})

pkg_check_modules(LS2 REQUIRED luna-service2)
include_directories(${LS2_INCLUDE_DIRS})

pkg_check_modules(PMLOG REQUIRED PmLogLib)
include_directories(${PMLOG_INCLUDE_DIRS})

# ---
# create executable file
set(BIN_NAME biometrics_service)

set(SRC_LIST
    ${CMAKE_SOURCE_DIR}/src/gpio_handle.c
    ${CMAKE_SOURCE_DIR}/src/main.c
)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/pkg_$ENV{ARCH}/")
add_executable(${BIN_NAME} ${SRC_LIST})

# Locate libraries and headers
find_package(WiringPi REQUIRED)
find_package(Threads REQUIRED) 

# Include headers
include_directories(${WIRINGPI_INCLUDE_DIRS})
MESSAGE(${WIRINGPI_INCLUDE_DIRS})

# Link against libraries
target_link_libraries(${BIN_NAME} ${WIRINGPI_LIBRARIES}) 
target_link_libraries(${BIN_NAME} ${CMAKE_THREAD_LIBS_INIT}) 

# ignore shared library
set(CMAKE_EXE_LINKER_FLAGS "-Wl,--allow-shlib-undefined")
set_target_properties(${BIN_NAME} PROPERTIES LINKER_LANGUAGE C)

target_link_libraries (${BIN_NAME}
    ${GTHREAD2_LDFLAGS}
    ${PBNJSON_LDFLAGS}
    ${LS2_LDFLAGS}
    ${GLIB2_LDFLAGS}
    ${PMLOG_LDFLAGS}
)

file(COPY ${CMAKE_SOURCE_DIR}/services.json DESTINATION "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")