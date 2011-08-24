project(kmousetool)

find_package(KDE4 REQUIRED)
include (KDE4Defaults)
include (MacroLibrary)

add_definitions (${QT_DEFINITIONS} ${KDE4_DEFINITIONS})
add_definitions( -DQT_NO_CAST_FROM_ASCII )
add_definitions( -DQT_NO_CAST_TO_ASCII )
add_definitions(-DQT_USE_FAST_CONCATENATION -DQT_USE_FAST_OPERATOR_PLUS)

include_directories (${CMAKE_SOURCE_DIR} ${CMAKE_BINARY_DIR} ${KDE4_INCLUDES})

if(Q_WS_X11)
  macro_log_feature(X11_XTest_FOUND "libXtst" "X11 Testing Resource extension library" "http://xorg.freedesktop.org" TRUE "" "Needed to build kmousetool.")

  if(X11_XTest_FOUND)
    add_subdirectory( doc )
    add_subdirectory( kmousetool )
  endif(X11_XTest_FOUND)
else(Q_WS_X11)
  message(FATAL_ERROR "kmousetool needs an X11-based system.")
endif(Q_WS_X11)

if(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_CURRENT_SOURCE_DIR})
    macro_display_feature_log()
endif(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_CURRENT_SOURCE_DIR})
