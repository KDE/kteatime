
INCLUDES= $(all_includes)

bin_PROGRAMS = kteatime

kteatime_SOURCES = main.cpp toplevel.cpp

kteatime_METASOURCES = AUTO
kteatime_LDFLAGS = $(all_libraries) $(KDE_RPATH)

kteatime_LDADD = $(LIB_KDEUI) 

KDE_ICON = kteatime

messages:
	$(XGETTEXT) $(kteatime_SOURCES) -o $(podir)/kteatime.pot

appsdir = $(kde_appsdir)/Toys
apps_DATA = kteatime.desktop

