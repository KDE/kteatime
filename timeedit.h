/* -------------------------------------------------------------

   timeedit.h

   (C) 2003 by Daniel Teske (teske@bigfoot.com)

 ------------------------------------------------------------- */
#ifndef TIMEEDIT_H
#define TIMEEDIT_H

#include <qspinbox.h>
#include <qwidget.h>

class QBoxLayout;


/**
 * @short   A spinbox that wraps around after reaching minValue resp. maxValue.
 * @author  Daniel Teske
 */
class WrappingSpinBox : public QSpinBox
{
	Q_OBJECT

public:
	WrappingSpinBox(int minValue, int maxValue, int step = 1, QWidget *parent=0, const char *name=0);
	~WrappingSpinBox();

	void stepUp();
	void stepDown();

signals:
	void wrapUp();
	void wrapDown();
};


/**
 * @short   A widget for entering a timeout in minutes and seconds.
 * @author  Daniel Teske
 */
class TimeEdit : public QWidget
{
	Q_OBJECT

public:
	TimeEdit(QWidget* parent = 0, const char* name = 0);
	~TimeEdit();

	void setValue(int value);
	int value();

public slots:
	void setFocus();

private slots:
	void spinBoxValueChanged(int);
	void wrappedUp();
	void wrappedDown();

signals:
	void valueChanged(int);


protected:
	QSpinBox *minuteBox;
	WrappingSpinBox *secondBox;
	QBoxLayout* layout;
};

#endif

