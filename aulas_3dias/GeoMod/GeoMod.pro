#-------------------------------------------------
#
# Project created by hand
#
#-------------------------------------------------

QT       += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = GeoMod
TEMPLATE = app

CONFIG( release, debug|release ) {
	DESTDIR = ../GeoMod_release/dist
	OBJECTS_DIR = ../GeoMod_release/obj
	MOC_DIR = ../GeoMod_release/moc
	RCC_DIR = ../GeoMod_release/rcc
	UI_DIR = ../GeoMod_release/ui
} else {
	DESTDIR = ../GeoMod_debug/dist
	OBJECTS_DIR = ../GeoMod_debug/obj
	MOC_DIR = ../GeoMod_debug/moc
	RCC_DIR = ../GeoMod_debug/rcc
	UI_DIR = ../GeoMod_debug/ui
}
CONFIG += c++11


win32 {
	#-Wa,-mbig-obj not currently supported (or possibly not necessary) by GCC 4.8 (Linus)
	#necessary for compiling svd.cpp in debug mode.
	QMAKE_CXXFLAGS_DEBUG += -Wa,-mbig-obj
	#Don't know why -Wa,-mbig-obj sticks... removing it for release mode.
	QMAKE_CXXFLAGS_RELEASE -= -Wa,-mbig-obj
}

SOURCES +=  main.cpp \
    gui/mainwindow.cpp \
    util.cpp \
    viewer3d/viewer3dlistwidget.cpp \
    viewer3d/view3dcolortables.cpp \
    viewer3d/view3dwidget.cpp \
    viewer3d/view3dverticalexaggerationwidget.cpp \
    widgets/focuswatcher.cpp

HEADERS  += gui/mainwindow.h \
    util.h \
    viewer3d/viewer3dlistwidget.h \
    viewer3d/view3dcolortables.h \
    viewer3d/view3dwidget.h \
    viewer3d/view3dverticalexaggerationwidget.h \
    widgets/focuswatcher.h

FORMS    += gui/mainwindow.ui \
    viewer3d/view3dwidget.ui \
    viewer3d/view3dverticalexaggerationwidget.ui


#========== The VTK include and lib paths and libraries==================
_VTK_INCLUDE = $$(VTK_INCLUDE)
isEmpty(_VTK_INCLUDE){
    error(VTK_INCLUDE environment variable not defined.)
}
_VTK_LIB = $$(VTK_LIB)
isEmpty(_VTK_LIB){
    error(VTK_LIB environment variable not defined.)
}
_VTK_VERSION_SUFFIX = $$(VTK_VERSION_SUFFIX)
isEmpty(_VTK_VERSION_SUFFIX){
    warning(VTK_VERSION_SUFFIX environment variable not defined or empty.)
}
INCLUDEPATH += $$_VTK_INCLUDE
LIBPATH     += $$_VTK_LIB
LIBS        += -lvtkGUISupportQt$$_VTK_VERSION_SUFFIX \
               -lvtkCommonCore$$_VTK_VERSION_SUFFIX \
               -lvtkFiltersSources$$_VTK_VERSION_SUFFIX \
               -lvtkRenderingCore$$_VTK_VERSION_SUFFIX \
               -lvtkCommonExecutionModel$$_VTK_VERSION_SUFFIX \
               -lvtkInteractionStyle$$_VTK_VERSION_SUFFIX \
               -lvtkRenderingOpenGL2$$_VTK_VERSION_SUFFIX \
               -lvtkRenderingAnnotation$$_VTK_VERSION_SUFFIX \
               -lvtkRenderingFreeType$$_VTK_VERSION_SUFFIX \
               -lvtkInteractionWidgets$$_VTK_VERSION_SUFFIX
#               -lvtkCommonDataModel$$_VTK_VERSION_SUFFIX 
#               -lvtkFiltersGeneral$$_VTK_VERSION_SUFFIX 
#               -lvtkCommonTransforms$$_VTK_VERSION_SUFFIX 
#               -lvtkImagingSources$$_VTK_VERSION_SUFFIX 
#               -lvtkImagingCore$$_VTK_VERSION_SUFFIX 
#			   -lvtkFiltersCore$$_VTK_VERSION_SUFFIX 
#                           -lvtkFiltersExtraction$$_VTK_VERSION_SUFFIX  
#			   -lvtkImagingFourier$$_VTK_VERSION_SUFFIX 
#			   -lvtkCommonMisc$$_VTK_VERSION_SUFFIX 
#			   -lvtkCommonComputationalGeometry$$_VTK_VERSION_SUFFIX 
#			   -lvtkCommonMath$$_VTK_VERSION_SUFFIX 
#			   -lvtksys$$_VTK_VERSION_SUFFIX 
#			   -lvtkFiltersGeometry$$_VTK_VERSION_SUFFIX 
#			   -lvtkCommonColor$$_VTK_VERSION_SUFFIX 
#			   -lvtkCommonSystem$$_VTK_VERSION_SUFFIX 
#			   -lvtkglew$$_VTK_VERSION_SUFFIX 
#			   -lvtkfreetype$$_VTK_VERSION_SUFFIX 
#			   -lvtkzlib$$_VTK_VERSION_SUFFIX 
#			   -lvtkFiltersHybrid$$_VTK_VERSION_SUFFIX 
#			   -lvtkFiltersModeling$$_VTK_VERSION_SUFFIX 
#			   -lvtkImagingGeneral$$_VTK_VERSION_SUFFIX 
#			   -lvtkRenderingVolume$$_VTK_VERSION_SUFFIX 
#			   -lvtkFiltersStatistics$$_VTK_VERSION_SUFFIX 
#                           -lvtkalglib$$_VTK_VERSION_SUFFIX \
#                -lvtkImagingStencil$$_VTK_VERSION_SUFFIX \
#                -lvtkImagingHybrid$$_VTK_VERSION_SUFFIX

#=============================================================================

#Library used in Util::getPhysicalRAMusage()
win32 {
    LIBS += -lPsapi
}

# The application version
VERSION = 1.0

# Define a preprocessor macro so we can get the application version in application code.
DEFINES += APP_VERSION=\\\"$$VERSION\\\"

# Define application name macro
DEFINES += APP_NAME=\\\"$$TARGET\\\"

# Define application name and version macro
# \040 means a whitespace.  Don't replace it with an explicit space because
# it doesn't compile.
DEFINES += APP_NAME_VER=\\\"$$TARGET\\\040$$VERSION\\\"

RESOURCES += \
    resources.qrc

#set the Windows executable icon
#win32:RC_ICONS += art/exeicon.ico
