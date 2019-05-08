# Impoe uma versao minima do CMake
cmake_minimum_required (VERSION 3.7)

# Definicao do nome e da versao do programa
set(PROJECT_NAME GeoMod)
set(VERSION 1.0)

# Registra o nome do projeto no CMake.
project (${PROJECT_NAME})

# Instrui para encontrar arquivos-cabecalho no diretorio de build
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Instrui para criar codigo de classes com Q_OBJECT (MOCs) automaticamente
set(CMAKE_AUTOMOC ON)

# Instrui para criar codigo a partir dos UI do Qt Cretor automaticamente
set(CMAKE_AUTOUIC ON)

# Defines de compilacao para os locais dos fontes onde se usa o nome
# e a versao do aplicativo
add_compile_definitions( APP_VERSION="${VERSION}" )
add_compile_definitions( APP_NAME="${PROJECT_NAME}" )
add_compile_definitions( APP_NAME_VER="${PROJECT_NAME} ${VERSION}" )

# adiciona caminhos para o CMake buscar modulos com find_package() alem 
# dos defaults (PATH, LD_LIBRARY_PATH, etc.)
# set(CMAKE_PREFIX_PATH "C:/Users/ur5m/Desktop/VTK64/install_release/lib/cmake" ${CMAKE_PREFIX_PATH})


# Procura e resolve os caminhos dos SO/DLL dos modulos dos quais o programa depende
find_package(Qt5Core CONFIG REQUIRED)
find_package(Qt5Gui CONFIG REQUIRED)
find_package(Qt5Widgets CONFIG REQUIRED)
#find_package(VTK CONFIG REQUIRED)
include_directories(SYSTEM "C:/Users/ur5m/Desktop/VTK64/install_release/include/vtk-8.1")
link_directories("C:/Users/ur5m/Desktop/VTK64/install_release/lib")
set(VTK_LIBS  
               vtkGUISupportQt-8.1
               vtkCommonCore-8.1
               vtkFiltersSources-8.1
               vtkRenderingCore-8.1
               vtkCommonExecutionModel-8.1
               vtkInteractionStyle-8.1
               vtkRenderingOpenGL2-8.1
               vtkRenderingAnnotation-8.1
               vtkRenderingFreeType-8.1
               vtkInteractionWidgets-8.1
               vtkCommonDataModel-8.1
               vtkFiltersGeneral-8.1
               vtkCommonTransforms-8.1
               vtkImagingSources-8.1
               vtkImagingCore-8.1
		    vtkFiltersCore-8.1
               vtkFiltersExtraction-8.1
		    vtkImagingFourier-8.1
		    vtkCommonMisc-8.1
		    vtkCommonComputationalGeometry-8.1
		    vtkCommonMath-8.1
		    vtksys-8.1
		    vtkFiltersGeometry-8.1
		    vtkCommonColor-8.1
		    vtkCommonSystem-8.1
		    vtkglew-8.1
		    vtkfreetype-8.1
		    vtkzlib-8.1
		    vtkFiltersHybrid-8.1
		    vtkFiltersModeling-8.1
		    vtkImagingGeneral-8.1
		    vtkRenderingVolume-8.1
		    vtkFiltersStatistics-8.1
               vtkalglib-8.1
               vtkImagingStencil-8.1
               vtkImagingHybrid-8.1
)

# Por os includes do VTK no include path (-I<path> no gcc)
#include(${VTK_USE_FILE})

add_compile_definitions( ${VTK_DEFINITIONS} )

# Cria uma variavel CMake com os fontes do aplicativo
set(${PROJECT_NAME}_SRCS
    widgets/focuswatcher.h
    widgets/focuswatcher.cpp
    viewer3d/view3dcolortables.cpp
    viewer3d/view3dcolortables.h
    viewer3d/view3dverticalexaggerationwidget.cpp
    viewer3d/view3dverticalexaggerationwidget.h
    viewer3d/view3dverticalexaggerationwidget.ui
    viewer3d/view3dwidget.cpp
    viewer3d/view3dwidget.h
    viewer3d/view3dwidget.ui
    viewer3d/viewer3dlistwidget.cpp
    viewer3d/viewer3dlistwidget.h
    gui/mainwindow.ui
    gui/mainwindow.cpp
    util.h
    util.cpp
    main.cpp
)

# Instrui a criar um executavel cujo nome estah em PROJECT_NAME
add_executable(${PROJECT_NAME} ${${PROJECT_NAME}_SRCS})

# Instrui a link-editar os SO/DLL dos modulos dos quais o programa depende
target_link_libraries(${PROJECT_NAME} Qt5::Core)
target_link_libraries(${PROJECT_NAME} Qt5::Gui)
target_link_libraries(${PROJECT_NAME} Qt5::Widgets)
target_link_libraries(${PROJECT_NAME} ${VTK_LIBS})

