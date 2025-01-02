cmake_minimum_required(VERSION 3.19)
project(TimelapseApp LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOUIC ON)  # Ensure automatic UI file processing
set(CMAKE_AUTOMOC ON)  # Enable automatic MOC processing
set(CMAKE_AUTORCC ON)  # Enable automatic RCC processing for resources

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets LinguistTools Gui)
find_package(OpenCV REQUIRED)

qt_standard_project_setup()

qt_add_executable(TimelapseApp
    WIN32 MACOSX_BUNDLE
    main.cpp
    timelapseapp.cpp
    timelapseapp.h
    timelapseapp.ui
)

qt_add_translations(
    TARGETS TimelapseApp
    TS_FILES TimelapseApp_ja_JP.ts
)

target_link_libraries(TimelapseApp
    PRIVATE
        Qt::Core
        Qt::Widgets
        ${OpenCV_LIBS}
)

include(GNUInstallDirs)

install(TARGETS TimelapseApp
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_generate_deploy_app_script(
    TARGET TimelapseApp
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
