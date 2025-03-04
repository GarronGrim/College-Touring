cmake_minimum_required(VERSION 3.16)

project(College VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Quick)

qt_standard_project_setup(REQUIRES 6.5)

# Add SQLite source files
add_library(sqlite3 STATIC third_party/sqlite/sqlite3.c)
target_include_directories(sqlite3 PUBLIC third_party/sqlite)

qt_add_executable(appCollege
    main.cpp
)

qt_add_qml_module(appCollege
    URI College
    VERSION 1.0
    QML_FILES
        Main.qml
    SOURCES databasemanager.h databasemanager.cpp
    RESOURCES
    SOURCES souvenirdatabasemanager.h souvenirdatabasemanager.cpp
)

set_target_properties(appCollege PROPERTIES
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

find_package(Qt6 REQUIRED COMPONENTS Quick Sql)

target_link_libraries(appCollege
    PRIVATE Qt6::Quick Qt6::Sql sqlite3
)

include(GNUInstallDirs)
install(TARGETS appCollege
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
