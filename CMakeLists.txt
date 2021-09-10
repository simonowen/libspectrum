cmake_minimum_required(VERSION 3.10)
project(spectrum)

include(GNUInstallDirs)
include(CheckIncludeFiles)
include(CheckFunctionExists)
include(CheckSymbolExists)

set(MAJOR_VERSION 1)
set(MINOR_VERSION 5)
set(PATCH_VERSION 0)
set(VERSION ${MAJOR_VERSION}.${MINOR_VERSION}.${PATCH_VERSION})

if (CMAKE_TOOLCHAIN_FILE)
  message(STATUS "Using toolchain file: ${CMAKE_TOOLCHAIN_FILE}")
  include(${CMAKE_TOOLCHAIN_FILE})
endif()

set(SOURCES buffer.c bzip2.c creator.c csw.c dck.c ide.c libspectrum.c memory.c
    microdrive.c mmc.c plusd.c pzx_read.c rzx.c sna.c snapshot.c snp.c sp.c
    symbol_table.c szx.c tap.c tape.c tape_block.c timings.c tzx_read.c
    tzx_write.c utilities.c warajevo_read.c z80.c z80em.c zip.c zlib.c zxs.c
    snap_accessors.c tape_accessors.c tape_set.c
    myglib/garray.c myglib/ghash.c myglib/gslist.c)
set(HEADERS libspectrum.h internals.h tape_block.h zip.h)

add_library(${PROJECT_NAME} ${SOURCES} ${HEADERS})

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR})
target_compile_definitions(${PROJECT_NAME} PRIVATE HAVE_CONFIG_H=1)

check_include_files("strings.h" HAVE_STRINGS_H)
check_include_files("unistd.h" HAVE_UNISTD_H)
set(HAVE_STRING_H 1)

check_function_exists(strcasecmp HAVE_STRCASECMP)
check_function_exists(strncasecmp HAVE_STRNCASECMP)
check_function_exists(snprintf HAVE_SNPRINTF)
check_function_exists(_snprintf HAVE__SNPRINTF)
check_function_exists(_stricmp HAVE__STRICMP)
check_function_exists(_strnicmp HAVE__STRNICMP)

find_package(ZLIB)
if (ZLIB_FOUND)
  target_link_libraries(${PROJECT_NAME} PRIVATE ZLIB::ZLIB)
  set(HAVE_LIBZ 1)
  set(HAVE_ZLIB_H 1)
endif()

find_package(BZip2)
if (BZIP2_FOUND)
  target_link_libraries(${PROJECT_NAME} PRIVATE BZip2::BZip2)
  set(HAVE_LIBBZ2 1)
endif()

find_library(AUDIOFILE_LIBRARY NAMES audiofile)
find_path(AUDIOFILE_INCLUDE_DIR audiofile.h)
if (AUDIOFILE_LIBRARY AND SPECTRUM_INCLUDE_DIR)
  target_include_directories(${PROJECT_NAME} PRIVATE ${AUDIOFILE_INCLUDE_DIR})
  target_link_libraries(${PROJECT_NAME} PRIVATE ${AUDIOFILE_LIBRARY})
  message(STATUS "Found audiofile: ${AUDIOFILE_LIBRARY}")
  set(HAVE_LIBAUDIOFILE 1)
endif()

configure_file(config.h.in ${CMAKE_CURRENT_BINARY_DIR}/config.h)

install(TARGETS ${PROJECT_NAME}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  FRAMEWORK DESTINATION ${CMAKE_INSTALL_LIBDIR}
  PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
