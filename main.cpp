/* -------------------------------------------------------------

   KTeaTime

   (C) 1998-1999 by Matthias Hoelzer-Kluepfel (hoelzer@kde.org)

 ------------------------------------------------------------- */

#include <kwin.h>
#include <klocale.h>
#include <kcmdlineargs.h>
#include <kaboutdata.h>

#include "toplevel.h"


static const char *description =
	I18N_NOOP("KDE utility for making fine cup of tea");

static const char *version = "v1.0.0";

int main(int argc, char *argv[])
{
  KAboutData aboutData( "kteatime", I18N_NOOP("KTeaTime"),
    version, description, KAboutData::License_GPL,
    "(c) 1998-1999, Matthias Hoelzer-Kluepfel");
  aboutData.addAuthor("Matthias Hoelzer-Kluepfel",0, "hoelzer@kde.org");
  aboutData.addAuthor("Martin Willers", 0, "willers@xm-arts.de");
  KCmdLineArgs::init( argc, argv, &aboutData );

  KApplication app;

  TopLevel toplevel;
  KWin::setSystemTrayWindowFor(toplevel.winId(),0);
  toplevel.show();

  app.setTopWidget(&toplevel);
  KWin::appStarted();

  return app.exec();
}
