# Install script for directory: C:/Users/Arnav/Downloads/Open EDA 2-2.0/src

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "C:/Users/Arnav/Downloads/Open EDA 2-2.0/src/out/install/x64-Debug")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Debug")
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
  include("C:/Users/Arnav/Downloads/Open EDA 2-2.0/src/out/build/x64-Debug/parser/cmake_install.cmake")
  include("C:/Users/Arnav/Downloads/Open EDA 2-2.0/src/out/build/x64-Debug/atpg/cmake_install.cmake")
  include("C:/Users/Arnav/Downloads/Open EDA 2-2.0/src/out/build/x64-Debug/cop/cmake_install.cmake")
  include("C:/Users/Arnav/Downloads/Open EDA 2-2.0/src/out/build/x64-Debug/expand/cmake_install.cmake")
  include("C:/Users/Arnav/Downloads/Open EDA 2-2.0/src/out/build/x64-Debug/faults/cmake_install.cmake")
  include("C:/Users/Arnav/Downloads/Open EDA 2-2.0/src/out/build/x64-Debug/prpg/cmake_install.cmake")
  include("C:/Users/Arnav/Downloads/Open EDA 2-2.0/src/out/build/x64-Debug/sat/cmake_install.cmake")
  include("C:/Users/Arnav/Downloads/Open EDA 2-2.0/src/out/build/x64-Debug/simulation/cmake_install.cmake")
  include("C:/Users/Arnav/Downloads/Open EDA 2-2.0/src/out/build/x64-Debug/structures/cmake_install.cmake")
  include("C:/Users/Arnav/Downloads/Open EDA 2-2.0/src/out/build/x64-Debug/tpi/cmake_install.cmake")
  include("C:/Users/Arnav/Downloads/Open EDA 2-2.0/src/out/build/x64-Debug/trace/cmake_install.cmake")
  include("C:/Users/Arnav/Downloads/Open EDA 2-2.0/src/out/build/x64-Debug/window/cmake_install.cmake")

endif()

if(CMAKE_INSTALL_COMPONENT)
  set(CMAKE_INSTALL_MANIFEST "install_manifest_${CMAKE_INSTALL_COMPONENT}.txt")
else()
  set(CMAKE_INSTALL_MANIFEST "install_manifest.txt")
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
file(WRITE "C:/Users/Arnav/Downloads/Open EDA 2-2.0/src/out/build/x64-Debug/${CMAKE_INSTALL_MANIFEST}"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
