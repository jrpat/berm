cmake_minimum_required(VERSION 3.22)

project(berm)

SET(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}")
SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

if (APPLE)
    set(MACOSX_BUNDLE_ICON_FILE berm.icns)
    add_executable(Berm MACOSX_BUNDLE
        ./main.cc
        ./berm.icns
        )
    set_source_files_properties(./berm.icns PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
    target_link_libraries(Berm "-framework Cocoa")
else ()
    add_executable(Berm ./main.cc)
endif ()

find_package(Qt5Widgets "5.15.0" REQUIRED)

target_link_libraries(Berm Qt5::Widgets)

target_include_directories(Berm PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/.opt/include/qtermwidget5)
target_link_directories(Berm PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/.opt/lib)
target_link_libraries(Berm qtermwidget5)

set(RESOURCE_FILES
  file(GLOB "${CMAKE_CURRENT_SOURCE_DIR}/.opt/share/kb")
)

set_target_properties(Berm
  PROPERTIES
    MACOSX_BUNDLE_BUNDLE_NAME     "Berm"
    MACOSX_BUNDLE_BUNDLE_VERSION  "0.0.1"
    MACOSX_BUNDLE_COPYRIGHT       "© 2022 Shed Software LLC"
    MACOSX_BUNDLE_GUI_IDENTIFIER  "com.shed-software.berm"
)

