/* -------------------------------------------------------------

   timeedit.cpp

   (C) 2003 by Daniel Teske (teske@bigfoot.com)

 ------------------------------------------------------------- */

#include <knuminput.h>
#include <klocale.h>

#include <qwidget.h>
#include <qlabel.h>
#include <qlayout.h>

#include "timeedit.h"


TimeEdit::TimeEdit(QWidget* parent, const char* name)
    : QWidget(parent, name)
{
	layout = new QHBoxLayout(this);
	minuteBox = new KIntSpinBox(0, 60, 1, 0, 10, this);
	QLabel* min = new QLabel(i18n(" min"), this);
	min->setFixedSize(min->sizeHint());
	secondBox = new KIntSpinBox(0, 59, 1, 0, 10, this);
	secondBox->setFixedSize(secondBox->sizeHint());
	QLabel* sec = new QLabel(i18n(" sec"),this);
	layout->addWidget(minuteBox);
	layout->addWidget(min);

	layout->addWidget(secondBox);
	layout->addWidget(sec);

	connect(minuteBox, SIGNAL(valueChanged(int)), SLOT(spinBoxValueChanged(int)) );
	connect(secondBox, SIGNAL(valueChanged(int)), SLOT(spinBoxValueChanged(int)) );
}

TimeEdit::~TimeEdit()
{
}

void TimeEdit::setValue(int value)
{
	if (value < 0)
		return;
	// FIXME: What does KSpinBox do if value is out of range?

	secondBox->setValue(value % 60);    
	minuteBox->setValue(value / 60);
}

int TimeEdit::value()
{
	return minuteBox->value()*60 + secondBox->value();
}

void TimeEdit::spinBoxValueChanged(int v)
{
	if (this->value() == 0) {
		secondBox->stepUp();        // this will give another spinBoxValueChanged() invocation
		return;
	}

	// FIXME: decrement 'minutes' if 'seconds' are 0 and still decremented
	//        (need to set KIntSpinBox's lower limit to -1 then?)

    emit valueChanged(this->value());
}
