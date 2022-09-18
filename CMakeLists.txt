cmake_minimum_required(VERSION 3.22)

project(qak)

SET(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}")
SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

if (APPLE)
    set(MACOSX_BUNDLE_ICON_FILE bqterm.icns)
    add_executable(qak MACOSX_BUNDLE
        ./main.cc
        ./util.m
#         ./res/bqterm.icns
        )
#     set_source_files_properties(./res/bqterm.icns PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
    target_link_libraries(qak "-framework Cocoa")
else ()
    add_executable(qak ./main.cc)
endif ()

find_package(Qt5Widgets "5.15.0" REQUIRED)

target_link_libraries(qak Qt5::Widgets)

target_include_directories(qak PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/.opt/include/qtermwidget5)
target_link_directories(qak PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/.opt/lib)
target_link_libraries(qak qtermwidget5)

