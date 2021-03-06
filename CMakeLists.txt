cmake_minimum_required(VERSION 3.0.0)
project(VolumetricRenderer)
set(PROJECT_BRIEF "VolumetricRenderer")

set(SoftwareVersion 0.1)
add_definitions(-DSOFTWAREVERSION=${SoftwareVersion})

set(CMAKE_AUTOMOC ON)

set(CMAKE_BUILD_TYPE RELEASE)
set(GCC_COMPILE_FLAGS "-std=c++11 -Wall -O3 -g -static")
set(GCC_LINK_FLAGS "")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${GCC_COMPILE_FLAGS}")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${GCC_LINK_FLAGS}")


#qt
find_package(Qt5 COMPONENTS Core Widgets Gui OpenGL)
include_directories(${Qt5Widgets_INCLUDE_DIRS})

#glm
set(GLM_INC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/ext/glm CACHE PATH "Path to GLM")
include_directories(${GLM_INC_DIR})

#DCMTK
set(DCMTK_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/ext/dcmtk-3.6.4/build/install CACHE PATH "Path to DCMTK")
set(DCMTK_INC_DIR ${DCMTK_ROOT_DIR}/include)
set(DCMTK_LIB_DIR ${DCMTK_ROOT_DIR}/lib)
set(DCMTK_BIN_DIR ${DCMTK_ROOT_DIR}/bin)
include_directories(${DCMTK_INC_DIR})
link_directories(${DCMTK_LIB_DIR})

#devil
set(DEVIL_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/ext/devil CACHE PATH "Path to Devil")
set(DEVIL_INC_DIR ${DEVIL_ROOT_DIR}/include)
set(DEVIL_LIB_DIR ${DEVIL_ROOT_DIR}/lib/x64/Release)
set(DEVIL_BIN_DIR ${DEVIL_ROOT_DIR}/bin)
include_directories(${DEVIL_INC_DIR})
link_directories(${DEVIL_LIB_DIR})

#teem
set(TEEM_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/ext/teem-1.11.0-src/build/install CACHE PATH "Path to Teem")
set(TEEM_INC_DIR ${TEEM_ROOT_DIR}/include)
set(TEEM_LIB_DIR ${TEEM_ROOT_DIR}/lib)
set(TEEM_BIN_DIR ${TEEM_ROOT_DIR}/bin)
include_directories(${TEEM_INC_DIR})
link_directories(${TEEM_LIB_DIR})

#opencv
set(OPENCV_ROOT_DIR C:/opencv/build CACHE PATH "Path to opencv")
set(OPENCV_INC_DIR ${OPENCV_ROOT_DIR}/include)
set(OPENCV_LIB_DIR ${OPENCV_ROOT_DIR}/x64/vc15/lib)
set(OPENCV_BIN_DIR ${OPENCV_ROOT_DIR}/x64/vc15/bin)
include_directories(${OPENCV_INC_DIR})
link_directories(${OPENCV_LIB_DIR})


include_directories(src)
add_subdirectory(src)



#installer stuff
set(QT_BIN_DIR "C:/Qt/5.12.0/msvc2017_64/bin" CACHE PATH "Path to QT binary files ")

install(FILES ${QT_BIN_DIR}/Qt5Core.dll
			  ${QT_BIN_DIR}/Qt5Gui.dll
			  ${QT_BIN_DIR}/Qt5Widgets.dll
			  ${QT_BIN_DIR}/Qt5Charts.dll
			  ${QT_BIN_DIR}/Qt5OpenGL.dll
			  ${QT_BIN_DIR}/Qt5Concurrent.dll
			  ${QT_BIN_DIR}/../plugins/platforms/qwindows.dll
			  ${DCMTK_BIN_DIR}/dcmdata.dll
			  ${DCMTK_BIN_DIR}/dcmdsig.dll
			  ${DCMTK_BIN_DIR}/dcmfg.dll
			  ${DCMTK_BIN_DIR}/dcmimage.dll
			  ${DCMTK_BIN_DIR}/dcmimgle.dll
			  ${DCMTK_BIN_DIR}/dcmiod.dll
			  ${DCMTK_BIN_DIR}/dcmjpeg.dll
			  ${DCMTK_BIN_DIR}/dcmjpls.dll
			  ${DCMTK_BIN_DIR}/dcmnet.dll
			  ${DCMTK_BIN_DIR}/dcmpmap.dll
			  ${DCMTK_BIN_DIR}/dcmpstat.dll
			  ${DCMTK_BIN_DIR}/dcmqrdb.dll
			  ${DCMTK_BIN_DIR}/dcmrt.dll
			  ${DCMTK_BIN_DIR}/dcmseg.dll
			  ${DCMTK_BIN_DIR}/dcmsr.dll
			  ${DCMTK_BIN_DIR}/dcmtls.dll
			  ${DCMTK_BIN_DIR}/dcmtract.dll
			  ${DCMTK_BIN_DIR}/dcmwlm.dll
			  ${DCMTK_BIN_DIR}/i2d.dll
			  ${DCMTK_BIN_DIR}/ijg8.dll
			  ${DCMTK_BIN_DIR}/ijg12.dll
			  ${DCMTK_BIN_DIR}/ijg16.dll
			  ${DCMTK_BIN_DIR}/oflog.dll
			  ${DCMTK_BIN_DIR}/ofstd.dll
			  ${DEVIL_BIN_DIR}/DevIL.dll
			  ${TEEM_BIN_DIR}/teem.dll
			  ${OPENCV_BIN_DIR}/opencv_world400.dll
			  ${OPENCV_BIN_DIR}/opencv_world400d.dll
			  ${OPENCV_BIN_DIR}/opencv_ffmpeg400_64.dll
		DESTINATION bin
		COMPONENT MainApplication
)

install(DIRECTORY ${QT_BIN_DIR}/../plugins/platforms
DESTINATION bin
COMPONENT MainApplication
)

set(CPACK_PACKAGE_NAME "VolumetricRenderer")
set(CPACK_PACKAGE_VENDOR "CMake.org")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "VolumetricRenderer - Install For Maximum Fun")
set(CPACK_PACKAGE_VERSION ${SoftwareVersion})
set(CPACK_PACKAGE_INSTALL_DIRECTORY "VolumetricRenderer")
#set(CPACK_RESOURCE_FILE_LICENSE ${CMAKE_CURRENT_SOURCE_DIR}/licence.txt)

set(CPACK_COMPONENTS_ALL MainApplication)

set(CPACK_PACKAGE_EXECUTABLES "VolumetricRenderer;VolumetricRenderer")

set(CPACK_CREATE_DESKTOP_LINKS "VolumetricRenderer")


include(CPack)