
add_subdirectory( pics ) 




########### next target ###############

include_directories(${X11_INCLUDE_DIR})

set(kmousetool_SRCS mtstroke.cpp kmousetool.cpp main.cpp )

ki18n_wrap_ui(kmousetool_SRCS kmousetoolui.ui )

add_executable(kmousetool ${kmousetool_SRCS})

target_link_libraries(kmousetool
  Qt::Core
  Qt::Gui
  Qt::Widgets
  Phonon::phonon4qt5
  KF5::DBusAddons
  KF5::I18n
  KF5::IconThemes
  KF5::Notifications
  KF5::WindowSystem
  KF5::XmlGui
  ${X11_XTest_LIB}
  ${X11_Xext_LIB}
  ${X11_LIBRARIES})

install(TARGETS kmousetool  ${KDE_INSTALL_TARGETS_DEFAULT_ARGS} )


########### install files ###############

install( PROGRAMS org.kde.kmousetool.desktop  DESTINATION  ${KDE_INSTALL_APPDIR} )
install( FILES mousetool_tap.wav  DESTINATION  ${KDE_INSTALL_DATADIR}/kmousetool/sounds )
install(FILES org.kde.kmousetool.appdata.xml DESTINATION ${KDE_INSTALL_METAINFODIR})

