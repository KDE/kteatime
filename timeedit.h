/* -------------------------------------------------------------

   timeedit.h

   (C) 2003 by Daniel Teske (teske@bigfoot.com)

 ------------------------------------------------------------- */

#include <qwidget.h>

class QBoxLayout;
class KIntSpinBox;


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

signals:
	void valueChanged(int); 


protected:
	KIntSpinBox *minuteBox, *secondBox;
	QBoxLayout* layout;
};
