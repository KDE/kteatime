/* -------------------------------------------------------------

   KTeaTime

   (C) 1998-1999 by Matthias Hoelzer-Kluepfel (hoelzer@kde.org)

 ------------------------------------------------------------- */


#include <ktmainwindow.h>
#include <kwm.h>
#include <kapp.h>
#include <klocale.h>
#include <kcmdlineargs.h>
#include <kaboutdata.h>

#include "toplevel.h"


static const char *description = 
	I18N_NOOP("KDE utility for making fine cup of tea");

static const char *version = "v0.0.1";

int main(int argc, char *argv[])
{
  KAboutData aboutData( "kteatime", I18N_NOOP("KTeaTime"),
    version, description, KAboutData::License_GPL,
    "(c) 1998-1999, Matthias Hoelzer-Kluepfel");
  aboutData.addAuthor("Matthias Hoelzer-Kluepfel",0, "hoelzer@kde.org");
  KCmdLineArgs::init( argc, argv, &aboutData );
  
  KApplication app;

  TopLevel toplevel;
  KWM::setDockWindow(toplevel.winId());
  toplevel.show();

  app.setTopWidget(&toplevel);

  return app.exec();
}
