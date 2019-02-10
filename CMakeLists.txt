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
SET(VERSION_MINOR "1")

#SET(CMAKE_BUILD_TYPE Debug)
SET( wxWidgets_USE_DEBUG OFF)
SET( wxWidgets_USE_UNICODE ON)
SET( wxWidgets_USE_UNIVERSAL OFF)
SET( wxWidgets_USE_STATIC OFF)

SET(libcurl ${CMAKE_SOURCE_DIR}/src/curl/build/lib/x86)
SET(libcurl "${PARENT}.lib")
SET(curlcpp ${CMAKE_SOURCE_DIR}/src/curlcpp/lib/x86)
SET(curlcpp "${PARENT}.lib")

LINK_DIRECTORIES(${CMAKE_SOURCE_DIR}/src/curl/build/lib/x86)
LINK_DIRECTORIES(${CMAKE_SOURCE_DIR}/src/curlcpp/lib/x86)

OPTION(UKTIDES_USE_SVG "Use SVG graphics" ON)

INCLUDE("cmake/PluginConfigure.cmake")

IF(UKTIDES_USE_SVG)
  ADD_DEFINITIONS(-DUKTIDES_USE_SVG)
ENDIF(UKTIDES_USE_SVG)


INCLUDE("cmake/PluginConfigure.cmake")
INCLUDE("cmake/PluginJSON.cmake")

IF(MSVC)
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++17")	
ENDIF(MSVC)

ADD_COMPILE_DEFINITIONS(_HAS_AUTO_PTR_ETC=1)
ADD_COMPILE_DEFINITIONS(_MBCS)
ADD_COMPILE_DEFINITIONS(_HAS_CXX17)
ADD_COMPILE_DEFINITIONS(CURL_STATICLIB)

INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/src/curl/include)
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/src/curl/include/curl)
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/src/curlcpp/include)




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



FIND_LIBRARY(libcurl ${CMAKE_SOURCE_DIR}/src/curl/build/lib/x86)
FIND_LIBRARY(curlcpp ${CMAKE_SOURCE_DIR}/src/curlcpp/lib/x86)

TARGET_LINK_LIBRARIES(${PACKAGE_NAME} libcurl curlcpp)

TARGET_LINK_LIBRARIES(${PACKAGE_NAME} Crypt32.lib)
TARGET_LINK_LIBRARIES(${PACKAGE_NAME} ws2_32.lib)
TARGET_LINK_LIBRARIES(${PACKAGE_NAME} winmm.lib)
TARGET_LINK_LIBRARIES(${PACKAGE_NAME} wldap32.lib)
TARGET_LINK_LIBRARIES(${PACKAGE_NAME} uuid.lib)
TARGET_LINK_LIBRARIES(${PACKAGE_NAME} odbc32.lib)
TARGET_LINK_LIBRARIES(${PACKAGE_NAME} odbccp32.lib)


ADD_DEFINITIONS(-DTIXML_USE_STL)

INCLUDE("cmake/PluginInstall.cmake")
INCLUDE("cmake/PluginLocalization.cmake")
INCLUDE("cmake/PluginPackage.cmake")
