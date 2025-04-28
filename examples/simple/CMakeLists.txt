cmake_minimum_required(VERSION 3.16)

project(ed_example_simple VERSION ${VERSION_SHORT}) 

find_package(QT NAMES Qt6 Qt5 COMPONENTS Core REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} 5.5 COMPONENTS Core Gui Widgets REQUIRED)


qt_standard_project_setup()

set(SOURCES
    main.cpp
    MainWindow.cpp
)

qt_add_resources(SOURCES resources.qrc)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
add_executable(SimpleExample WIN32 
    ${SOURCES}
)


find_package(qt${QT_VERSION_MAJOR}-eddockmenu CONFIG QUIET)
if(qt${QT_VERSION_MAJOR}-eddockmenu_FOUND)
    get_target_property(QDOCKMENU_INCLUDES  ed::qt${QT_VERSION_MAJOR}-eddockmenu INTERFACE_INCLUDE_DIRECTORIES)
    get_target_property(QDOCKMENU_LIBRARIES ed::qt${QT_VERSION_MAJOR}-eddockmenu INTERFACE_LINK_LIBRARIES)
    message(STATUS "EDDockMenu Found: ${QDOCKMENU_LIBRARIES}, ${QDOCKMENU_INCLUDES}")
    
    target_link_libraries(SimpleExample PUBLIC ed::qt${QT_VERSION_MAJOR}-eddockmenu)

    target_include_directories(SimpleExample PUBLIC
        ${QDOCKMENU_INCLUDES}
    )
else()
    target_link_libraries(SimpleExample PRIVATE ed::qt${QT_VERSION_MAJOR}-eddockmenu)
    message(STATUS "EDDockMenu Not Found: Use private instead")
endif()

target_link_libraries(SimpleExample PUBLIC 
    Qt${QT_VERSION_MAJOR}::Core 
    Qt${QT_VERSION_MAJOR}::Gui 
    Qt${QT_VERSION_MAJOR}::Widgets
)

set_target_properties(SimpleExample PROPERTIES 
    AUTOMOC ON
    AUTORCC ON
    AUTOUIC ON
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
    VERSION ${VERSION_SHORT}
    EXPORT_NAME "Qt Dock Menu Simple Example"
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)
