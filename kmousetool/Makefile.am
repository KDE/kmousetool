bin_PROGRAMS = kmousetool

kmousetool_SOURCES = kaboutmousetool.cpp mtstroke.cpp kmousetool.cpp main.cpp 
kmousetool_LDADD   = -lXtst $(LIB_KDEUI) $(LIB_KDECORE) $(LIB_QT) -lXext -lX11 $(LIBSOCKET)

SUBDIRS = pics 

EXTRA_DIST = main.cpp version.h kmousetool.cpp kmousetool.h kmousetool.desktop mtstroke.cpp mtstroke.h kaboutmousetool.cpp kaboutmousetool.h mousetool_tap.wav 

install-data-local:
	$(mkinstalldirs) $(kde_appsdir)/Applications/
	$(INSTALL_DATA) $(srcdir)/kmousetool.desktop $(kde_appsdir)/Applications/kmousetool.desktop
	$(mkinstalldirs) $(kde_sounddir)/
	$(INSTALL_DATA) $(srcdir)/mousetool_tap.wav $(kde_sounddir)/mousetool_tap.wav

uninstall-local:
	-rm -f $(kde_appsdir)/Applications/kmousetool.desktop
	-rm -f $(kde_sounddir)/mousetool_tap.wav

# set the include path for X, qt and KDE
INCLUDES= $(all_includes)

METASOURCES = AUTO

# the library search path. 
kmousetool_LDFLAGS = $(all_libraries) $(KDE_RPATH)

messages: 
	LIST=`find . -name \*.h -o -name \*.hh -o -name \*.H -o -name \*.hxx -o -name \*.hpp -o -name \*.cpp -o -name \*.cc -o -name \*.cxx -o -name \*.ecpp -o -name \*.C`; \
	if test -n "$$LIST"; then \
	 $(XGETTEXT) $$LIST -o $(podir)/kmousetool.pot; \
	fi

