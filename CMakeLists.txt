cmake_minimum_required(VERSION 3.16)

project(EasyMusik VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Encuentra las bibliotecas necesarias de Qt
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Qml Quick Sql Multimedia)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Qml Quick Sql Multimedia)

set(PROJECT_SOURCES
        main.cpp
        MainWindow.cpp
        MainWindow.h
        MainWindow.ui
        Tecla.h Tecla.cpp
        Teclado.h Teclado.cpp
        recursos.qrc
        PausaDialog.h PausaDialog.cpp PausaDialog.ui
        FinDialog.h FinDialog.cpp
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(EasyMusik
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        MenuPrincipal.h MenuPrincipal.cpp
        Cancion.h Cancion.cpp
        FinDialog.h FinDialog.cpp FinDialog.ui
        VentanaCarga.h
        VentanaCarga.cpp




    )
else()
    if(ANDROID)
        add_library(EasyMusik SHARED
            ${PROJECT_SOURCES}
        )
    else()
        add_executable(EasyMusik
            ${PROJECT_SOURCES}
        )
    endif()
endif()

# 🔗 Enlaza todos los módulos necesarios
target_link_libraries(EasyMusik PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Quick
    Qt${QT_VERSION_MAJOR}::Qml
    Qt${QT_VERSION_MAJOR}::Sql
    Qt${QT_VERSION_MAJOR}::Multimedia
)

# iOS/macOS bundle config
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.EasyMusik)
endif()

set_target_properties(EasyMusik PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS EasyMusik
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(EasyMusik)
endif()
