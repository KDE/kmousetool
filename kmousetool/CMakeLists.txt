
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
    KF6::DBusAddons
    KF6::I18n
    KF6::IconThemes
    KF6::Notifications
    KF6::WindowSystem
    KF6::XmlGui
    Qt6::Widgets
    Qt6::Gui
    Qt6::Core
    KF6::CoreAddons
    KF6::StatusNotifierItem
    Phonon::phonon4qt6
    ${X11_XTest_LIB}
    ${X11_Xext_LIB}
    ${X11_LIBRARIES}
)

install(TARGETS kmousetool  ${KDE_INSTALL_TARGETS_DEFAULT_ARGS} )
install( PROGRAMS org.kde.kmousetool.desktop  DESTINATION  ${KDE_INSTALL_APPDIR} )
install( FILES mousetool_tap.wav  DESTINATION  ${KDE_INSTALL_DATADIR}/kmousetool/sounds )
install(FILES org.kde.kmousetool.appdata.xml DESTINATION ${KDE_INSTALL_METAINFODIR})
