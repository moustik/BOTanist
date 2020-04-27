project(botanist)
cmake_minimum_required(VERSION 3.1.2)

# TgBot n'importe pas les compile def boost
cmake_policy(SET CMP0079 NEW)

# Download automatically, you can also just copy the conan.cmake file
if(NOT EXISTS "${CMAKE_BINARY_DIR}/conan.cmake")
   message(STATUS "Downloading conan.cmake from https://github.com/conan-io/cmake-conan")
   file(DOWNLOAD "https://raw.githubusercontent.com/conan-io/cmake-conan/v0.15/conan.cmake"
                 "${CMAKE_BINARY_DIR}/conan.cmake")
endif()
include(${CMAKE_BINARY_DIR}/conan.cmake)

set(CMAKE_INSTALL_RPATH "$ORIGIN/../lib")
set(CMAKE_BUILD_WITH_INSTALL_RPATH ON)

set(Boost_USE_MULTITHREADED ON)
conan_cmake_run(CONANFILE conanfile.txt
                BASIC_SETUP CMAKE_TARGETS
                BUILD missing)

add_subdirectory(lib/tgbot-cpp)
# ajoute les compile def de boost a TgBot
target_link_libraries(TgBot CONAN_PKG::boost)

set(BOTANIST_SRC_DIR "src")
list(APPEND BOTANIST_SOURCES
    ${BOTANIST_SRC_DIR}/journal.cpp
    ${BOTANIST_SRC_DIR}/simple_commands.cpp
    ${BOTANIST_SRC_DIR}/storage.cpp
    ${BOTANIST_SRC_DIR}/telegram_helpers.cpp
    ${BOTANIST_SRC_DIR}/web.cpp
    )

add_executable(botanist src/main.cpp ${BOTANIST_SOURCES})

#target_compile_options(botanist PUBLIC "-Wall")
#target_compile_definitions(TgBot PUBLIC -DBOOST_ALL_DYN_LINK)
set_property(TARGET botanist PROPERTY CXX_STANDARD 17)
set_property(TARGET botanist PROPERTY CXX_STANDARD_REQUIRED ON)

target_link_libraries(botanist TgBot ${CONAN_TARGETS})

install(TARGETS botanist
    RUNTIME DESTINATION bin
    )

# Bring the friend dynamic libraries
INSTALL(
  DIRECTORY "${CMAKE_BINARY_DIR}/lib"
  DESTINATION "."
)

if(UNIX)  # we do package on linux only [for deployment purposes]
    set(CPACK_GENERATOR "TGZ")
    include(CPack)
endif()
