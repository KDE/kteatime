
INCLUDES= $(all_includes)

bin_PROGRAMS = kteatime

kteatime_SOURCES = main.cpp toplevel.cpp

kteatime_METASOURCES = USE_AUTOMOC
kteatime_LDFLAGS = $(all_libraries) $(KDE_RPATH)

kteatime_LDADD = $(LIB_KDEUI) 

messages:
	$(XGETTEXT) -C -ki18n -x $(includedir)/kde.pot $(kteatime_SOURCES) && mv messages.po ../po/kteatime.pot

appsdir = $(kde_appsdir)/Utilities
apps_DATA = kteatime.desktop

install-data-local: 
	$(mkinstalldirs) $(kde_minidir)
	$(INSTALL_DATA) $(srcdir)/kteatime.xpm $(kde_minidir)

uninstall-local:
	rm -f $(kde_minidir)/kteatime.xpm

