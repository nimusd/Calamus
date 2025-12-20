cmake_minimum_required(VERSION 3.19)
project(Calamus LANGUAGES CXX)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets)
find_package(Qt6 REQUIRED COMPONENTS Widgets)
find_package(Qt6 REQUIRED COMPONENTS Widgets)

qt_standard_project_setup()

qt_add_executable(Calamus
    WIN32 MACOSX_BUNDLE
    main.cpp
    calamusmain.cpp
    calamusmain.h
    calamusmain.ui
    sounitbuilder.ui
    container.ui
    sounitbuilder.h sounitbuilder.cpp
    container.h container.cpp
    canvas.h canvas.cpp
)

target_link_libraries(Calamus
    PRIVATE
        Qt::Core
        Qt::Widgets
)
target_link_libraries(Calamus PRIVATE Qt6::Widgets)
target_link_libraries(Calamus PRIVATE Qt6::Widgets)

include(GNUInstallDirs)

install(TARGETS Calamus
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_generate_deploy_app_script(
    TARGET Calamus
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
