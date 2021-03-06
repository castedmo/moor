SET (include_HEADERS
  archive_writer.hpp
  archive_reader.hpp
  moor_build_config.hpp
  types.hpp
  )
SET (libmoor_SOURCES
  archive_writer.cpp
  archive_reader.cpp
  memory_writer_callback.cpp
)

IF(MSVC)
    set(CMAKE_DEBUG_POSTFIX d)
    add_definitions(-D_CRT_SECURE_NO_DEPRECATE)
    add_definitions(-D_CRT_NONSTDC_NO_DEPRECATE)
ENDIF()


SET(ADDITIONAL_LIBS "")

FIND_PACKAGE(LibArchive)

INCLUDE_DIRECTORIES(${LibArchive_INCLUDE_DIR})
LIST(APPEND ADDITIONAL_LIBS ${LibArchive_LIBRARIES})

IF (NOT MSVC)
  ADD_DEFINITIONS (-std=c++0x -fPIC)
ENDIF()

ADD_LIBRARY (moor SHARED ${libmoor_SOURCES} ${INCLUDE_DIRECTORIES})
TARGET_LINK_LIBRARIES (moor ${ADDITIONAL_LIBS})

ADD_LIBRARY (moor_static STATIC ${libmoor_SOURCES} ${INCLUDE_DIRECTORIES})
SET_TARGET_PROPERTIES(moor_static PROPERTIES COMPILE_DEFINITIONS
  MOOR_STATIC)
TARGET_LINK_LIBRARIES (moor_static ${ADDITIONAL_LIBS})

IF(NOT WIN32 OR CYGWIN)
  SET_TARGET_PROPERTIES(moor_static PROPERTIES OUTPUT_NAME moor)
ENDIF()

INSTALL(TARGETS moor moor_static
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib)

INSTALL(FILES ${include_HEADERS} DESTINATION include/moor)
