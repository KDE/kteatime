
INCLUDES= $(all_includes)

bin_PROGRAMS = kteatime

kteatime_SOURCES = main.cpp toplevel.cpp

kteatime_METASOURCES = USE_AUTOMOC
kteatime_LDFLAGS = $(all_libraries) $(KDE_RPATH)

kteatime_LDADD = $(LIB_KDEUI) 

KDE_ICON = kteatime

messages:
	$(XGETTEXT) -C -ki18n -x $(includedir)/kde.pot $(kteatime_SOURCES) && mv messages.po ../po/kteatime.pot

appsdir = $(kde_appsdir)/Utilities
apps_DATA = kteatime.desktop

