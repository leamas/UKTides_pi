# ~~~
# Author:      Rasbats aka. Mike Rossiter
# Copyright:
# License:     wxWidgets License
# ~~~

# -------- Cmake setup ---------
#
cmake_minimum_required(VERSION 3.5.1)

cmake_policy(SET CMP0042 NEW)

if (POLICY CMP0077)
  cmake_policy(SET CMP0077 NEW)
endif ()

# Prefer libGL.so to libOpenGL.so, see CMP0072
set(OpenGL_GL_PREFERENCE "LEGACY")

# Locations where cmake looks for cmake modules.
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/build
                      ${CMAKE_SOURCE_DIR}/
                      ${CMAKE_SOURCE_DIR}/cmake
)
if (WIN32)
  set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/buildwin)
endif ()


# -------- Build setup (mostly in PluginCompiler) --------
#
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)
message(STATUS "Building: ${BUILD_TYPE}")

# -------- Options ----------
#

set(OCPN_UNSTABLE_REPO
    "mike-rossiter/uktides-beta"
    CACHE STRING "Default repository for untagged builds"
)
set(OCPN_STABLE_REPO
    "mike-rossiter/uktides-prod"
    CACHE STRING "Default repository for tagged builds"
)
option(PLUGIN_USE_SVG "Use SVG graphics" ON)


# -------  Plugin setup --------
#
project(UKTides_pi VERSION 1.0.50)

set(VERBOSE_NAME UKTides)    # Legacy installers artifact names
set(TITLE_NAME UKTides)      # Legacy installer dialogs
set(PLUGIN_API_NAME UKTides) # As of GetCommonName() in plugin API
set(CPACK_PACKAGE_CONTACT "Mike Rossiter")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "UK Tides from UKHO")
set(CPACK_PACKAGE_DESCRIPTION "Tides for 600+ UK ports")

set(PKG_RELEASE "1")
set(PKG_AUTHOR "Mike Rossiter")

SET(SRC
        src/UKTides_pi.h
        src/UKTides_pi.cpp
        src/icons.h
        src/icons.cpp
	    src/UKTidesgui.h
	    src/UKTidesgui.cpp
	    src/UKTidesgui_impl.cpp
	    src/UKTidesgui_impl.h
	    src/NavFunc.h
	    src/NavFunc.cpp
		src/tidetable.cpp
		src/tidetable.h		
	)


# -------- Setup completed, build the plugin --------
#
add_library(${CMAKE_PROJECT_NAME} SHARED ${SRC})
include_directories(${CMAKE_BINARY_DIR}/include)

add_subdirectory("libs/api-16")
target_link_libraries(${CMAKE_PROJECT_NAME} ocpn::api)

set(VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
set(VERSION_MINOR ${PROJECT_VERSION_MINOR})
set(VERSION_PATCH ${PROJECT_VERSION_PATCH})
set(PACKAGE_VERSION "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}")
set(PACKAGE_NAME ${CMAKE_PROJECT_NAME})

# Create the minimim ocpn version id string like ov50
string(REGEX REPLACE "\([0-9]\)\.\([0-9]\).*" "ov\\1\\2"
       OCPN_MIN_VERSION ${API_OCPN_MIN_VERSION})

if (PLUGIN_USE_SVG)
  add_definitions(-DPLUGIN_USE_SVG)
endif ()


# Set up targets. Targets sets up a recursive call with BUILD_TYPE set to
# 'flatpak', 'pkg' or 'tarball'. The initial call without BUILD_TYPE ends
# here.
#
include(Targets)
create_targets(
  ${PROJECT_SOURCE_DIR}/flatpak/org.opencpn.OpenCPN.Plugin.uktides.yaml
)
if ("${BUILD_TYPE}" STREQUAL "")
  return ()
endif ()

if (NOT ${BUILD_TYPE} STREQUAL "flatpak")
  # Build package as required (flatpak already dealt with).
  #
  include(PluginCompiler)
  include(PluginLibs)
  include(PluginLocalization)
  include(PluginInstall)
  include(PluginPackage)

  add_subdirectory("libs/jsoncpp")
  target_link_libraries(${PACKAGE_NAME} ocpn::jsoncpp)
  
  add_subdirectory("libs/tinyxml")
  target_link_libraries(${PACKAGE_NAME} ocpn::tinyxml)
  
  add_subdirectory("libs/wxJSON")
  target_link_libraries(${PACKAGE_NAME} ocpn::wxjson)
endif ()

configure_file(
  # The cloudsmith upload script
  ${CMAKE_SOURCE_DIR}/ci/upload.sh.in ${CMAKE_BINARY_DIR}/upload.sh
  @ONLY
)

configure_file(
  # The XML metadata file
  ${CMAKE_SOURCE_DIR}/uktides-plugin.xml.in
  ${CMAKE_BINARY_DIR}/${pkg_displayname}.xml
  @ONLY
)
