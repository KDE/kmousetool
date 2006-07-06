
add_subdirectory( pics ) 




########### next target ###############

set(kmousetool_SRCS mtstroke.cpp kmousetool.cpp main.cpp )

kde4_automoc(${kmousetool_SRCS})

kde4_add_ui_files(kmousetool_SRCS kmousetoolui.ui )

kde4_add_executable(kmousetool ${kmousetool_SRCS})

target_link_libraries(kmousetool  ${KDE4_KDEUI_LIBS} ${X11_XTest_LIB} ${X11_Xext_LIB}  phononcore)

install(TARGETS kmousetool  DESTINATION ${BIN_INSTALL_DIR} )


########### install files ###############

install( FILES kmousetool.desktop  DESTINATION  ${XDG_APPS_DIR} )
install( FILES mousetool_tap.wav  DESTINATION  ${DATA_INSTALL_DIR}/kmousetool/sounds )




#original Makefile.am contents follow:

#bin_PROGRAMS = kmousetool
#
#kmousetool_SOURCES = kmousetoolui.ui mtstroke.cpp kmousetool.cpp main.cpp 
#kmousetool_LDADD   = -lXtst $(LIB_KDEUI) $(LIB_KDECORE) $(LIB_QT) -lXext -lX11 $(LIBSOCKET)
#
#SUBDIRS = pics 
#
#EXTRA_DIST = main.cpp version.h kmousetool.cpp kmousetool.h kmousetool.desktop mtstroke.cpp mtstroke.h mousetool_tap.wav 
#
#kmousetooldir = $(kde_appsdir)/Applications
#kmousetool_DATA = kmousetool.desktop
#
#sounddatadir = $(kde_datadir)/kmousetool/sounds
#sounddata_DATA = mousetool_tap.wav
#
## set the include path for X, qt and KDE
#INCLUDES= $(all_includes)
#
#METASOURCES = AUTO
#
## the library search path. 
#kmousetool_LDFLAGS = $(all_libraries) $(KDE_RPATH)
#
#messages: rc.cpp
#	LIST=`find . -name \*.h -o -name \*.hh -o -name \*.H -o -name \*.hxx -o -name \*.hpp -o -name \*.cpp -o -name \*.cc -o -name \*.cxx -o -name \*.ecpp -o -name \*.C`; \
#	if test -n "$$LIST"; then \
#	 $(XGETTEXT) $$LIST -o $(podir)/kmousetool.pot; \
#	fi
#
