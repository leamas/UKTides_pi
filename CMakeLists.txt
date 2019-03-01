##---------------------------------------------------------------------------
## Author:      Rasbats aka. Mike Rossiter
## Copyright:
## License:     wxWidgets License
##---------------------------------------------------------------------------

# define minimum cmake version
CMAKE_MINIMUM_REQUIRED(VERSION 2.6.2)

PROJECT(UKTides_pi)

SET(PACKAGE_NAME UKTides_pi)
SET(VERBOSE_NAME UKTides)
SET(TITLE_NAME UKTides)
SET(CPACK_PACKAGE_CONTACT "Mike Rossiter")

SET(VERSION_MAJOR "0")
SET(VERSION_MINOR "2.1")

#SET(CMAKE_BUILD_TYPE Debug)
SET( wxWidgets_USE_DEBUG OFF)
SET( wxWidgets_USE_UNICODE ON)
SET( wxWidgets_USE_UNIVERSAL OFF)
SET( wxWidgets_USE_STATIC OFF)

OPTION(UKTIDES_USE_SVG "Use SVG graphics" ON)

INCLUDE("cmake/PluginConfigure.cmake")

IF(UKTIDES_USE_SVG)
  ADD_DEFINITIONS(-DUKTIDES_USE_SVG)
ENDIF(UKTIDES_USE_SVG)

INCLUDE("cmake/PluginConfigure.cmake")
INCLUDE("cmake/PluginJSON.cmake")

ADD_COMPILE_DEFINITIONS(_HAS_AUTO_PTR_ETC=1)

SET(SRC_UKTides
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
		src/ocpn_plugin.h
		src/tinyxml.cpp
        src/tinyxmlerror.cpp
        src/tinyxmlparser.cpp
        src/tinyxml.h
	)


ADD_LIBRARY(${PACKAGE_NAME} SHARED ${SRC_UKTides} ${SRC_JSON})

ADD_DEFINITIONS(-DTIXML_USE_STL)

INCLUDE("cmake/PluginInstall.cmake")
INCLUDE("cmake/PluginLocalization.cmake")
INCLUDE("cmake/PluginPackage.cmake")
