# Impoe uma versao minima do CMake
cmake_minimum_required(VERSION 3.7)

# Definicao do nome do programa
set(NOME_PROGRAMA HorizontesPickProbe)

# Registra o nome do projeto no CMake.
PROJECT(${NOME_PROGRAMA})

# adiciona caminhos para o CMake buscar modulos com find_package() alem 
# dos defaults (PATH, LD_LIBRARY_PATH, etc.)
# diretorio de deploy ou onde foi instalado o VTK
set(CMAKE_PREFIX_PATH "C:/Users/ur5m/Desktop/VTK64/install_release/lib/cmake" ${CMAKE_PREFIX_PATH})

# Procura e resolve os caminhos dos SO/DLL dos modulos dos quais o programa depende
find_package(VTK REQUIRED)

# Por os includes do VTK no include path (-I<path> no gcc)
include(${VTK_USE_FILE})

# Instrui para criar codigo de classes com Q_OBJECT (MOCs) automaticamente
set(CMAKE_AUTOMOC ON)

#find_package(Qt5Widgets REQUIRED)

# Faz buscar headers tanto nos fontes quanto no diretorio de build (-I<path> no gcc)
include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR})

# Busca recursivamente todos os fontes
file(GLOB_RECURSE UI_FILES *.ui)
file(GLOB_RECURSE QT_WRAP *.h)
file(GLOB_RECURSE CXX_FILES *.cpp)

# Manda gerar os headers derivados dos .UI do Qt
qt5_wrap_ui(UISrcs ${UI_FILES} )

# Manda criar um executavel com os .cpp, .h, etc. encontrados
add_executable(${NOME_PROGRAMA} ${CXX_FILES} ${UISrcs} ${QT_WRAP})

# Informa os modulos do Qt necessarios (similar ao comando QT+= do .pro)
qt5_use_modules(${NOME_PROGRAMA} Core Gui)

# Manda link-editar as SO/DLL passados (no caso as do VTK)
target_link_libraries(${NOME_PROGRAMA} ${VTK_LIBRARIES})