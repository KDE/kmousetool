if(Q_WS_X11)
  if(NOT X11_XTest_FOUND)
    macro_log_feature(X11_XTest_FOUND "libXtst" "X11 Testing Resource extension library" "http://xorg.freedesktop.org" FALSE "" "needed to build kmousetool.")
  ELSE(NOT X11_XTest_FOUND)
    project(kmousetool)
    add_subdirectory( kmousetool )
  endif(NOT X11_XTest_FOUND)
endif(Q_WS_X11)


