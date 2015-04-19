# Author: Alexander Böhn / http://github.com/fish2000 / © 2015.04
# FindLibArchive.cmake

FIND_PATH(LibArchive_ROOT_DIR
    NAMES include/archive.h include/archive_entry.h
    PATH_SUFFIXES libarchive/include)

FIND_PATH(LibArchive_LIBRARIES
    NAMES libarchive
    HINTS ${LibArchive_ROOT_DIR}/lib
    PATH_SUFFIXES libarchive/lib)

FIND_PATH(LibArchive_INCLUDE_DIR
    NAMES archive.h archive_entry.h
    HINTS ${LibArchive_ROOT_DIR}/include
    PATH_SUFFIXES libarchive/include)

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(
    LibArchive DEFAULT_MSG
    LibArchive_LIBRARIES
    LibArchive_INCLUDE_DIR)

SET(LibArchive_LIBRARY      LibArchive_LIBRARIES            PARENT_SCOPE)
SET(LibArchive_INCLUDE_DIRS LibArchive_INCLUDE_DIR          PARENT_SCOPE)

MARK_AS_ADVANCED(
    LibArchive_ROOT_DIR
    LibArchive_LIBRARY
    LibArchive_LIBRARIES
    LibArchive_INCLUDE_DIR
    LibArchive_INCLUDE_DIRS)
