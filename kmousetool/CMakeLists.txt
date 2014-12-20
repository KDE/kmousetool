
add_subdirectory( pics ) 




########### next target ###############

include_directories(${PHONON_INCLUDE_DIR})

set(kmousetool_SRCS mtstroke.cpp kmousetool.cpp main.cpp )

kde4_add_ui_files(kmousetool_SRCS kmousetoolui.ui )

kde4_add_executable(kmousetool ${kmousetool_SRCS})

target_link_libraries(kmousetool KF5::KDELibs4Support ${PHONON_LIBRARIES} KF5::Notifications Qt5::Core Qt5::Gui Qt5::Widgets ${X11_XTest_LIB} ${X11_Xext_LIB} ${X11_LIBRARIES})

install(TARGETS kmousetool  ${INSTALL_TARGETS_DEFAULT_ARGS} )


########### install files ###############

install( PROGRAMS kmousetool.desktop  DESTINATION  ${XDG_APPS_INSTALL_DIR} )
install( FILES mousetool_tap.wav  DESTINATION  ${DATA_INSTALL_DIR}/kmousetool/sounds )

