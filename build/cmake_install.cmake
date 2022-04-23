# Install script for directory: C:/Users/Arnav/Downloads/Open EDA 2-2.0/src

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "C:/Program Files/OpenEDA-2.0")
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

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  include("C:/Users/Arnav/Downloads/Open EDA 2-2.0/build/parser/cmake_install.cmake")
  include("C:/Users/Arnav/Downloads/Open EDA 2-2.0/build/atpg/cmake_install.cmake")
  include("C:/Users/Arnav/Downloads/Open EDA 2-2.0/build/cop/cmake_install.cmake")
  include("C:/Users/Arnav/Downloads/Open EDA 2-2.0/build/expand/cmake_install.cmake")
  include("C:/Users/Arnav/Downloads/Open EDA 2-2.0/build/faults/cmake_install.cmake")
  include("C:/Users/Arnav/Downloads/Open EDA 2-2.0/build/prpg/cmake_install.cmake")
  include("C:/Users/Arnav/Downloads/Open EDA 2-2.0/build/sat/cmake_install.cmake")
  include("C:/Users/Arnav/Downloads/Open EDA 2-2.0/build/simulation/cmake_install.cmake")
  include("C:/Users/Arnav/Downloads/Open EDA 2-2.0/build/structures/cmake_install.cmake")
  include("C:/Users/Arnav/Downloads/Open EDA 2-2.0/build/tpi/cmake_install.cmake")
  include("C:/Users/Arnav/Downloads/Open EDA 2-2.0/build/trace/cmake_install.cmake")
  include("C:/Users/Arnav/Downloads/Open EDA 2-2.0/build/window/cmake_install.cmake")

endif()

if(CMAKE_INSTALL_COMPONENT)
  set(CMAKE_INSTALL_MANIFEST "install_manifest_${CMAKE_INSTALL_COMPONENT}.txt")
else()
  set(CMAKE_INSTALL_MANIFEST "install_manifest.txt")
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
file(WRITE "C:/Users/Arnav/Downloads/Open EDA 2-2.0/build/${CMAKE_INSTALL_MANIFEST}"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
