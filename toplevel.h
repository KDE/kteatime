/* -------------------------------------------------------------

   toplevel.h

   (C) 1998-1999 by Matthias Hoelzer-Kluepfel (hoelzer@kde.org)

 ------------------------------------------------------------- */


#ifndef _TOPLEVEL_H_
#define _TOPLEVEL_H_

#include <kapplication.h>
#include <qpopupmenu.h>
#include <qtimer.h>
#include <qpixmap.h>
#include <qlineedit.h>
#include <qstringlist.h>
#include <qlistbox.h>
#include <qpushbutton.h>
#include <qgroupbox.h>
#include <knuminput.h>
#include <ksystemtray.h>

class TopLevel : public KSystemTray
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
    void stop();
    void config();
    void help();
    void setToolTip(const QString &text);
    void rebuildTeaMenu();

    void listBoxItemSelected(int id);
    void nameEditTextChanged(const QString& newText);
    void spinBoxValueChanged(const QString& newText);
    void newButtonClicked();
    void delButtonClicked();
    void upButtonClicked();
    void downButtonClicked();
    void enable_menuEntries();
    void disable_properties();
    void enable_properties();
    void enable_controls();

private:

    bool running, ready, frame1;

    QStringList teas, times;
    QStringList ntimes;
    int seconds, teatime, current_tea;

    bool beeping, popping;
    QString action;

    QPopupMenu *menu;
    QPixmap *mugPixmap, *bagPixmap, *tea1Pixmap, *tea2Pixmap;

    QListBox *listbox;
    QLineEdit *nameEdit;
    KIntSpinBox *timeEdit;
    QGroupBox *editgroup;
    QPushButton *btn_new, *btn_del, *btn_up, *btn_down;

    QString lasttip;
};

#endif
