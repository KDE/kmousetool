project(kmousetool)

if(Q_WS_X11)
  macro_log_feature(X11_XTest_FOUND "libXtst" "X11 Testing Resource extension library" "http://xorg.freedesktop.org" TRUE "" "Needed to build kmousetool.")

  if(X11_XTest_FOUND)
    add_subdirectory( kmousetool )
  endif(X11_XTest_FOUND)
else(Q_WS_X11)
  message(FATAL_ERROR "kmousetool needs an X11-based system.")
endif(Q_WS_X11)


