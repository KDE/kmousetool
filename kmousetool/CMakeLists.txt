
add_subdirectory( pics ) 

add_executable(kmousetool)

target_include_directories(kmousetool PRIVATE ${X11_INCLUDE_DIR})

target_sources(kmousetool PRIVATE
    mtstroke.cpp mtstroke.h
    kmousetool.cpp kmousetool.h
    main.cpp
)

ki18n_wrap_ui(kmousetool kmousetoolui.ui)

target_compile_definitions(kmousetool PRIVATE -DKMOUSETOOL_VERSION="${kmousetool_VERSION}")
target_link_libraries(kmousetool
    KF${KF_MAJOR_VERSION}::DBusAddons
    KF${KF_MAJOR_VERSION}::I18n
    KF${KF_MAJOR_VERSION}::IconThemes
    KF${KF_MAJOR_VERSION}::Notifications
    KF${KF_MAJOR_VERSION}::WindowSystem
    KF${KF_MAJOR_VERSION}::XmlGui
    Qt${QT_MAJOR_VERSION}::Widgets
    Qt${QT_MAJOR_VERSION}::Gui
    Qt${QT_MAJOR_VERSION}::Core
    KF${KF_MAJOR_VERSION}::CoreAddons
    Phonon::phonon4qt${QT_MAJOR_VERSION}
    ${X11_XTest_LIB}
    ${X11_Xext_LIB}
    ${X11_LIBRARIES}
)

install(TARGETS kmousetool  ${KDE_INSTALL_TARGETS_DEFAULT_ARGS} )
install( PROGRAMS org.kde.kmousetool.desktop  DESTINATION  ${KDE_INSTALL_APPDIR} )
install( FILES mousetool_tap.wav  DESTINATION  ${KDE_INSTALL_DATADIR}/kmousetool/sounds )
install(FILES org.kde.kmousetool.appdata.xml DESTINATION ${KDE_INSTALL_METAINFODIR})
