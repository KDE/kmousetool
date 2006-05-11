kde4_header()




########### install files ###############

install_files( ${DATA_INSTALL_DIR}/kmousetool/icons FILES  )

kde4_install_icons( ${ICON_INSTALL_DIR}  hicolor )

kde4_footer()



#original Makefile.am contents follow:

#kmousetooliconsdir = $(kde_datadir)/kmousetool/icons
#kmousetoolicons_ICON = AUTO
#KDE_ICON = kmousetool
