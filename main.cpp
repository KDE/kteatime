/* -------------------------------------------------------------

   KTeaTime

   (C) 1998-1999 by Matthias Hoelzer-Kluepfel (hoelzer@kde.org)

 ------------------------------------------------------------- */


#include <ktmainwindow.h>
#include <kwm.h>
#include <kapp.h>
#include <klocale.h>
#include <kcmdlineargs.h>

#include "toplevel.h"


static const char *description = 
	I18N_NOOP("KDE utility for making fine cup of tea");

static const char *version = "v0.0.1";

int main(int argc, char *argv[])
{
  KCmdLineArgs::init(argc, argv, "kteatime", description, version);

  KApplication app;

  TopLevel *toplevel=0;

  if (app.isRestored())
      RESTORE(TopLevel)
  else {
      // no session management: just create one window
      toplevel = new TopLevel();
  }

  KWM::setDockWindow(toplevel->winId());
  toplevel->show();

  app.setTopWidget(new QWidget);

  return app.exec();
}
