cmake_minimum_required(VERSION 3.10)
project(valyria VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

message(STATUS "Project version: ${PROJECT_VERSION}")

find_package(PkgConfig REQUIRED)
find_package(Threads REQUIRED)
pkg_check_modules(OpenGLES2 REQUIRED IMPORTED_TARGET glesv2)
pkg_check_modules(JPEG REQUIRED IMPORTED_TARGET libjpeg)
pkg_check_modules(PNG REQUIRED IMPORTED_TARGET libpng)
pkg_check_modules(ESSOS REQUIRED IMPORTED_TARGET essos>=1.0)
set(ESSOS_LIBRARIES ${ESSOS_STATIC_LIBRARIES} ${ESSOS_SHARED_LIBRARIES})
set(ESSOS_INCLUDE_DIRS ${ESSOS_INCLUDE_DIRS})

message(STATUS "OpenGLES2 library found: ${OpenGLES2_LIBRARIES}")
message(STATUS "JPEG library found: ${JPEG_LIBRARIES}")
message(STATUS "PNG library found: ${PNG_LIBRARIES}")
message(STATUS "Essos library found: ${ESSOS_LIBRARIES}")

option(PLATFORM "Specify the target platform (e.g., realtek, amlogic, broadcom)" "generic")
message(STATUS "Selected platform: ${PLATFORM}")

include(GNUInstallDirs)
set(ASSET_BASE_DIR ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_DATADIR}/valyria/assets CACHE PATH "Base directory for asset files")
message(STATUS "Asset base directory: ${ASSET_BASE_DIR}")

include_directories(
    ${PROJECT_SOURCE_DIR}/include
    ${OpenGLES2_INCLUDE_DIRS}
    ${JPEG_INCLUDE_DIRS}
    ${PNG_INCLUDE_DIRS}
    ${ESSOS_INCLUDE_DIRS}
)

set(SOURCES
    src/BenchmarkEngine.cpp
    src/ConfigurationManager.cpp
    src/GraphicsContext.cpp
    src/HTMLReportGenerator.cpp
    src/ImageLoader.cpp
    src/Logger.cpp
    src/main.cpp
    src/MetricsCollector.cpp
    src/RenderTask.cpp
    src/Shader.cpp
    src/ShaderProgram.cpp
    src/ShaderManager.cpp
    src/tasks/Cellular.cpp
    src/tasks/Clear.cpp
    src/tasks/Cube.cpp
    src/tasks/Triangle.cpp
)

if (${PLATFORM} STREQUAL "amlogic")
    list(APPEND SOURCES src/collectors/AmlogicMetricsCollector.cpp)
    add_definitions(-DPLATFORM_AMLOGIC)
elseif (${PLATFORM} STREQUAL "broadcom")
    list(APPEND SOURCES src/collectors/BroadcomMetricsCollector.cpp)
    add_definitions(-DPLATFORM_BROADCOM)
elseif (${PLATFORM} STREQUAL "realtek")
    list(APPEND SOURCES src/collectors/RealtekMetricsCollector.cpp)
    add_definitions(-DPLATFORM_REALTEK)
else()
    message(WARNING "No platform specific metric collector will be used.")
    add_definitions(-DPLATFORM_GENERIC)
endif()

add_executable(valyria ${SOURCES})

target_compile_definitions(valyria PRIVATE PROJECT_VERSION="${PROJECT_VERSION}")
target_compile_definitions(valyria PRIVATE ASSET_BASE_DIR="${ASSET_BASE_DIR}")

target_link_libraries(valyria PRIVATE
    Threads::Threads
    PkgConfig::OpenGLES2
    PkgConfig::JPEG
    PkgConfig::PNG
    cjson
    ${ESSOS_LIBRARIES}
)

install(TARGETS valyria DESTINATION ${CMAKE_INSTALL_BINDIR})
install(DIRECTORY assets/ DESTINATION ${ASSET_BASE_DIR})
