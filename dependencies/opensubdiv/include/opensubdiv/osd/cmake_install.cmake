# Install script for directory: C:/Users/Dennis van Ee/Downloads/OpenSubdiv-master/opensubdiv/osd

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
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/opensubdiv/osd" TYPE FILE PERMISSIONS OWNER_READ GROUP_READ WORLD_READ FILES
    "C:/Users/Dennis van Ee/Downloads/OpenSubdiv-master/opensubdiv/osd/bufferDescriptor.h"
    "C:/Users/Dennis van Ee/Downloads/OpenSubdiv-master/opensubdiv/osd/cpuEvaluator.h"
    "C:/Users/Dennis van Ee/Downloads/OpenSubdiv-master/opensubdiv/osd/cpuPatchTable.h"
    "C:/Users/Dennis van Ee/Downloads/OpenSubdiv-master/opensubdiv/osd/cpuVertexBuffer.h"
    "C:/Users/Dennis van Ee/Downloads/OpenSubdiv-master/opensubdiv/osd/mesh.h"
    "C:/Users/Dennis van Ee/Downloads/OpenSubdiv-master/opensubdiv/osd/nonCopyable.h"
    "C:/Users/Dennis van Ee/Downloads/OpenSubdiv-master/opensubdiv/osd/opengl.h"
    "C:/Users/Dennis van Ee/Downloads/OpenSubdiv-master/opensubdiv/osd/types.h"
    "C:/Users/Dennis van Ee/Downloads/OpenSubdiv-master/opensubdiv/osd/ompEvaluator.h"
    "C:/Users/Dennis van Ee/Downloads/OpenSubdiv-master/opensubdiv/osd/ompKernel.h"
    "C:/Users/Dennis van Ee/Downloads/OpenSubdiv-master/opensubdiv/osd/cpuGLVertexBuffer.h"
    "C:/Users/Dennis van Ee/Downloads/OpenSubdiv-master/opensubdiv/osd/glLegacyGregoryPatchTable.h"
    "C:/Users/Dennis van Ee/Downloads/OpenSubdiv-master/opensubdiv/osd/glPatchTable.h"
    "C:/Users/Dennis van Ee/Downloads/OpenSubdiv-master/opensubdiv/osd/glVertexBuffer.h"
    "C:/Users/Dennis van Ee/Downloads/OpenSubdiv-master/opensubdiv/osd/glMesh.h"
    "C:/Users/Dennis van Ee/Downloads/OpenSubdiv-master/opensubdiv/osd/glslPatchShaderSource.h"
    "C:/Users/Dennis van Ee/Downloads/OpenSubdiv-master/opensubdiv/osd/glXFBEvaluator.h"
    "C:/Users/Dennis van Ee/Downloads/OpenSubdiv-master/opensubdiv/osd/glComputeEvaluator.h"
    "C:/Users/Dennis van Ee/Downloads/OpenSubdiv-master/opensubdiv/osd/cpuD3D11VertexBuffer.h"
    "C:/Users/Dennis van Ee/Downloads/OpenSubdiv-master/opensubdiv/osd/d3d11ComputeEvaluator.h"
    "C:/Users/Dennis van Ee/Downloads/OpenSubdiv-master/opensubdiv/osd/d3d11LegacyGregoryPatchTable.h"
    "C:/Users/Dennis van Ee/Downloads/OpenSubdiv-master/opensubdiv/osd/d3d11PatchTable.h"
    "C:/Users/Dennis van Ee/Downloads/OpenSubdiv-master/opensubdiv/osd/d3d11VertexBuffer.h"
    "C:/Users/Dennis van Ee/Downloads/OpenSubdiv-master/opensubdiv/osd/d3d11Mesh.h"
    "C:/Users/Dennis van Ee/Downloads/OpenSubdiv-master/opensubdiv/osd/hlslPatchShaderSource.h"
    )
endif()

