/* -------------------------------------------------------------

   toplevel.h

   (C) 1998-1999 by Matthias Hoelzer-Kluepfel (hoelzer@kde.org)

 ------------------------------------------------------------- */


#ifndef _TOPLEVEL_H_
#define _TOPLEVEL_H_

#include <kapplication.h>
#include <qpopupmenu.h>
#include <qtimer.h>
#include <qlineedit.h>
#include <qstringlist.h>
#include <qlistview.h>
#include <qpushbutton.h>
#include <qgroupbox.h>
#include <knuminput.h>
#include <ksystemtray.h>

class QPixmap;

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

    void listBoxItemSelected();
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
    void actionEnableToggled(bool on);

private:

    QStringList teas, times;		// lists of tea-names and -times

    bool running, ready, firstFrame;
    int current_selected;		// index of currently +selected+ tea in menu
    int seconds;			// variable for counting down seconds
    QString current_name;		// name of currently +running+ tea

    bool beeping, popping;
    QString action;

    QPixmap *mugPixmap, *teaNotReadyPixmap, *teaAnim1Pixmap, *teaAnim2Pixmap;

    QPopupMenu *menu, *steeping_menu;
    QListView *listbox;
    QLineEdit *nameEdit, *actionEdit;
    KIntSpinBox *timeEdit;
    QGroupBox *editgroup;
    QPushButton *btn_new, *btn_del, *btn_up, *btn_down;

    QString lastTip;
};

#endif
