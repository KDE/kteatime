/* -------------------------------------------------------------

   KTeaTime

   (C) 1998-1999 by Matthias Hoelzer-Kluepfel (hoelzer@kde.org)

 ------------------------------------------------------------- */


#include <ktmainwindow.h>
#include <kwm.h>
#include <kapp.h>

#include "toplevel.h"


int main(int argc, char *argv[])
{
  KApplication app(argc, argv, "kteatime");

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
