APPSDIR = $(kde_appsdir)/Utilities
INCLUDES= $(all_includes)

bin_PROGRAMS = kteatime

kteatime_SOURCES = main.cpp toplevel.cpp

kteatime_METASOURCES = USE_AUTOMOC
kteatime_LDFLAGS = $(all_libraries) $(KDE_RPATH)

kteatime_LDADD = $(LIB_KDEUI) 

DISTCLEANFILES = $()

messages:
	$(XGETTEXT) -C -ki18n -x $(includedir)/kde.pot $(kteatime_SOURCES) && mv messages.po ../po/kteatime.pot

install-data-local: 
	$(mkinstalldirs) $(APPSDIR)
	$(INSTALL_DATA) kteatime.desktop $(APPSDIR)
	$(mkinstalldirs) $(kde_minidir)
	$(INSTALL_DATA) kteatime.xpm $(kde_minidir)

uninstall-local:
	rm -f $(APPSDIR)/kteatime.desktop
	rm -f $(kde_minidir)/kteatime.xpm
