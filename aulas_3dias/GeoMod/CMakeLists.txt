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

# Procura e resolve os caminhos dos SO/DLL do modulo QtWidgets
find_package(Qt5Widgets CONFIG REQUIRED)

# Cria uma variavel CMake com os fontes do aplicativo
set(${PROJECT_NAME}_SRCS
    gui/mainwindow.ui
    gui/mainwindow.cpp
    main.cpp
)

# Instrui a criar um executavel cujo nome estah em PROJECT_NAME
add_executable(${PROJECT_NAME} ${${PROJECT_NAME}_SRCS})

# Instrui a link-editar os SO/DLL do modulo QtWidgets
target_link_libraries(${PROJECT_NAME} Qt5::Widgets)
