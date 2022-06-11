
add_subdirectory( pics ) 

add_executable(kmousetool)

target_include_directories(kmousetool PRIVATE ${X11_INCLUDE_DIR})

target_sources(kmousetool PRIVATE
    mtstroke.cpp
    kmousetool.cpp
    main.cpp
)

ki18n_wrap_ui(kmousetool kmousetoolui.ui)

target_compile_definitions(kmousetool PRIVATE -DKMOUSETOOL_VERSION="${kmousetool_VERSION}")
target_link_libraries(kmousetool
    KF5::DBusAddons
    KF5::I18n
    KF5::IconThemes
    KF5::Notifications
    KF5::WindowSystem
    KF5::XmlGui
    Qt${QT_MAJOR_VERSION}::Widgets
    Qt${QT_MAJOR_VERSION}::Gui
    Qt${QT_MAJOR_VERSION}::Core
    Phonon::phonon4qt${QT_MAJOR_VERSION}
    ${X11_XTest_LIB}
    ${X11_Xext_LIB}
    ${X11_LIBRARIES}
)

install(TARGETS kmousetool  ${KDE_INSTALL_TARGETS_DEFAULT_ARGS} )
install( PROGRAMS org.kde.kmousetool.desktop  DESTINATION  ${KDE_INSTALL_APPDIR} )
install( FILES mousetool_tap.wav  DESTINATION  ${KDE_INSTALL_DATADIR}/kmousetool/sounds )
install(FILES org.kde.kmousetool.appdata.xml DESTINATION ${KDE_INSTALL_METAINFODIR})
