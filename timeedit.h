/* -------------------------------------------------------------

   timeedit.h

   (C) 2003 by Daniel Teske (teske@bigfoot.com)

 ------------------------------------------------------------- */

#include <qspinbox.h>
#include <qwidget.h>

class QBoxLayout;


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


class TimeEdit : public QWidget
{
	Q_OBJECT

public:
	TimeEdit(QWidget* parent = 0, const char* name = 0);
	~TimeEdit();

	void setValue(int value);
	int value();


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
