########### install files ###############

#
kdoctools_create_handbook(index.docbook INSTALL_DESTINATION ${KDE_INSTALL_DOCBUNDLEDIR}/en SUBDIR kmousetool)
kdoctools_create_manpage(man-kmousetool.1.docbook 1 INSTALL_DESTINATION ${KDE_INSTALL_MANDIR})
