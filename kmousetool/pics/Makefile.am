EXTRA_DIST = hi16-app-kmousetool.png hi32-app-kmousetool.png lo16-app-kmousetool.png lo32-app-kmousetool.png mousetool.png 

install-data-local:
	$(mkinstalldirs) $(kde_icondir)/hicolor/16x16/apps/
	$(INSTALL_DATA) $(srcdir)/hi16-app-kmousetool.png $(kde_icondir)/hicolor/16x16/apps/kmousetool.png
	$(mkinstalldirs) $(kde_icondir)/hicolor/32x32/apps/
	$(INSTALL_DATA) $(srcdir)/hi32-app-kmousetool.png $(kde_icondir)/hicolor/32x32/apps/kmousetool.png
	$(mkinstalldirs) $(kde_icondir)/locolor/16x16/apps/
	$(INSTALL_DATA) $(srcdir)/lo16-app-kmousetool.png $(kde_icondir)/locolor/16x16/apps/kmousetool.png
	$(mkinstalldirs) $(kde_icondir)/locolor/32x32/apps/
	$(INSTALL_DATA) $(srcdir)/lo32-app-kmousetool.png $(kde_icondir)/locolor/32x32/apps/kmousetool.png

uninstall-local:
	-rm -f $(kde_icondir)/hicolor/16x16/apps/kmousetool.png
	-rm -f $(kde_icondir)/hicolor/32x32/apps/kmousetool.png
	-rm -f $(kde_icondir)/locolor/16x16/apps/kmousetool.png
	-rm -f $(kde_icondir)/locolor/32x32/apps/kmousetool.png
