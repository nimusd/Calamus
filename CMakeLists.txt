cmake_minimum_required(VERSION 3.19)
project(Calamus LANGUAGES CXX)

# Fetch RTAudio from GitHub
include(FetchContent)
FetchContent_Declare(
    rtaudio
    GIT_REPOSITORY https://github.com/thestk/rtaudio.git
    GIT_TAG 6.0.1
)
set(RTAUDIO_API_WASAPI ON CACHE BOOL "Build WASAPI API" FORCE)
set(RTAUDIO_API_ASIO OFF CACHE BOOL "Build ASIO API" FORCE)
FetchContent_MakeAvailable(rtaudio)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets)
find_package(Qt6 REQUIRED COMPONENTS Widgets)
find_package(Qt6 REQUIRED COMPONENTS Widgets)
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
    scorecanvas.ui
    scorecanvaswindow.h scorecanvaswindow.cpp
    trackselect.h trackselect.cpp
    scorecanvas.h scorecanvas.cpp
    scorecanvascommands.h scorecanvascommands.cpp
    timeline.h timeline.cpp
    frequencylabels.h frequencylabels.cpp
    note.h note.cpp
    phrase.h phrase.cpp
    curve.h curve.cpp
    harmonicgenerator.h harmonicgenerator.cpp
    audioengine.h audioengine.cpp
    spectrum.h spectrum.cpp
    spectrumtosignal.h spectrumtosignal.cpp
    rolloffprocessor.h rolloffprocessor.cpp
    formantbody.h formantbody.cpp
    breathturbulence.h breathturbulence.cpp
    noisecolorfilter.h noisecolorfilter.cpp
    physicssystem.h physicssystem.cpp
    envelopeengine.h envelopeengine.cpp
    driftengine.h driftengine.cpp
    gateprocessor.h gateprocessor.cpp
    easingapplicator.h easingapplicator.cpp
    sounitgraph.h sounitgraph.cpp
    spectrumvisualizer.h spectrumvisualizer.cpp
    envelopevisualizer.h envelopevisualizer.cpp
    dnaeditordialog.h dnaeditordialog.cpp
    envelopelibraryDialog.h envelopelibraryDialog.cpp
    envelopecurvecanvas.h envelopecurvecanvas.cpp
    dialogNewProject.ui

)

target_link_libraries(Calamus
    PRIVATE
        Qt::Core
        Qt::Widgets
        rtaudio
)
target_link_libraries(Calamus PRIVATE Qt6::Widgets)
target_link_libraries(Calamus PRIVATE Qt6::Widgets)
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
