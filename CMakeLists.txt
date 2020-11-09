cmake_minimum_required(VERSION 3.13)
project(WinNWT5 C CXX)


#set(CMAKE_AUTOMOC ON)
# Find Qt5 libs
find_package(Qt5 COMPONENTS Widgets LinguistTools PrintSupport SerialPort REQUIRED)
# List of sources
set(NWT_SOURCES
    main.cpp
    mainwindow.cpp 
    nwt7mainclass.cpp 
    sweepcurvewidget.cpp 
    optiondlg.cpp 
    profildlg.cpp
    fmarkedlg.cpp 
    configfile.cpp 
    markersdlg.cpp 
    probedlg.cpp 
    firmwaredlg.cpp 
    Attenuator.cpp
    # Select QSerialPort-based modem impl
    modem_qsp.cpp
)
# List of headers (needed for MOC)
set(NWT_HEADERS
    constdef.h
    mainwindow.h
    nwt7mainclass.h
    sweepcurvewidget.h
    optiondlg.h
    profildlg.h
    fmarkedlg.h
    configfile.h
    markersdlg.h
    probedlg.h
    firmwaredlg.h
    BugReport.h
    Attenuator.h
    build.h
    modem.hpp
)
# Resources
qt5_add_resources(NWT_SOURCES icon.qrc)
# Translations
set(TS_FILES
    app_cn.ts 
    app_de.ts 
    app_ea.ts 
    app_es.ts 
    app_hu.ts 
    app_nl.ts 
    app_pl.ts 
    app_ru.ts
)
set_source_files_properties(${TS_FILES} PROPERTIES OUTPUT_LOCATION "l10n")
qt5_add_translation(qmFiles ${TS_FILES})
# A final executable
QT5_WRAP_CPP(MOC_FILES ${NWT_HEADERS})
add_executable(winnwt5 ${NWT_SOURCES} ${MOC_FILES})
target_link_libraries(winnwt5 PRIVATE Qt5::Core Qt5::Widgets Qt5::PrintSupport Qt5::SerialPort)
