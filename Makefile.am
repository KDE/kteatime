
INCLUDES= $(all_includes)

bin_PROGRAMS = kteatime

kteatime_SOURCES = main.cpp toplevel.cpp tealist.cpp timeedit.cpp

kteatime_METASOURCES = AUTO
kteatime_LDFLAGS = $(all_libraries) $(KDE_RPATH)

# LIB_KIO needed for KNotifyDialog
kteatime_LDADD = $(LIB_KDEUI) $(LIB_KIO)

KDE_ICON = kteatime

data_DATA = eventsrc
datadir = $(kde_datadir)/kteatime

pics_DATA = mug.png tea_not_ready.png tea_anim1.png tea_anim2.png
picsdir = $(kde_datadir)/kteatime/pics

messages:
	$(XGETTEXT) $(kteatime_SOURCES) -o $(podir)/kteatime.pot

appsdir = $(kde_appsdir)/Toys
apps_DATA = kteatime.desktop

EXTRA_DIST = $(pics_DATA)
