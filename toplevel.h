/* -------------------------------------------------------------

   toplevel.h

   (C) 1998-1999 by Matthias Hoelzer-Kluepfel (hoelzer@kde.org)

 ------------------------------------------------------------- */


#ifndef _TOPLEVEL_H_
#define _TOPLEVEL_H_

#include <kapp.h>
#include <qpopmenu.h>
#include <qtimer.h>
#include <qpixmap.h>
#include <qstringlist.h>

class TopLevel : public QWidget
{
  Q_OBJECT

public:

  TopLevel();
  ~TopLevel();

protected:

  void paintEvent(QPaintEvent *);
  void mousePressEvent(QMouseEvent *);
  void timerEvent(QTimerEvent *);

private slots:

  void teaSelected(int index);
  void start();
  void config();

private:

  bool running, ready, frame1;

  QStringList teas, times;
  int seconds, teatime, current_tea;

  QPopupMenu *menu;
  QPixmap *mugPixmap, *bagPixmap, *tea1Pixmap, *tea2Pixmap;

  bool beeping, popping;
  QString action;
};


#endif

