# Install script for directory: C:/Users/Dennis van Ee/Downloads/OpenSubdiv-master/opensubdiv/hbr

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "C:/Program Files/OpenSubdiv")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Release")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/opensubdiv/hbr" TYPE FILE PERMISSIONS OWNER_READ GROUP_READ WORLD_READ FILES
    "C:/Users/Dennis van Ee/Downloads/OpenSubdiv-master/opensubdiv/hbr/allocator.h"
    "C:/Users/Dennis van Ee/Downloads/OpenSubdiv-master/opensubdiv/hbr/bilinear.h"
    "C:/Users/Dennis van Ee/Downloads/OpenSubdiv-master/opensubdiv/hbr/catmark.h"
    "C:/Users/Dennis van Ee/Downloads/OpenSubdiv-master/opensubdiv/hbr/cornerEdit.h"
    "C:/Users/Dennis van Ee/Downloads/OpenSubdiv-master/opensubdiv/hbr/creaseEdit.h"
    "C:/Users/Dennis van Ee/Downloads/OpenSubdiv-master/opensubdiv/hbr/faceEdit.h"
    "C:/Users/Dennis van Ee/Downloads/OpenSubdiv-master/opensubdiv/hbr/face.h"
    "C:/Users/Dennis van Ee/Downloads/OpenSubdiv-master/opensubdiv/hbr/fvarData.h"
    "C:/Users/Dennis van Ee/Downloads/OpenSubdiv-master/opensubdiv/hbr/fvarEdit.h"
    "C:/Users/Dennis van Ee/Downloads/OpenSubdiv-master/opensubdiv/hbr/halfedge.h"
    "C:/Users/Dennis van Ee/Downloads/OpenSubdiv-master/opensubdiv/hbr/hierarchicalEdit.h"
    "C:/Users/Dennis van Ee/Downloads/OpenSubdiv-master/opensubdiv/hbr/holeEdit.h"
    "C:/Users/Dennis van Ee/Downloads/OpenSubdiv-master/opensubdiv/hbr/loop.h"
    "C:/Users/Dennis van Ee/Downloads/OpenSubdiv-master/opensubdiv/hbr/mesh.h"
    "C:/Users/Dennis van Ee/Downloads/OpenSubdiv-master/opensubdiv/hbr/subdivision.h"
    "C:/Users/Dennis van Ee/Downloads/OpenSubdiv-master/opensubdiv/hbr/vertexEdit.h"
    "C:/Users/Dennis van Ee/Downloads/OpenSubdiv-master/opensubdiv/hbr/vertex.h"
    )
endif()

