/* -------------------------------------------------------------

   timeedit.cpp

   (C) 2003 by Daniel Teske (teske@bigfoot.com)

 ------------------------------------------------------------- */

#include <klocale.h>
#include <qlabel.h>
#include <qlayout.h>

#include "timeedit.h"
#include "timeedit.moc"

WrappingSpinBox::WrappingSpinBox(int minValue, int maxValue, int step, QWidget *parent, const char *name)
	: QSpinBox(minValue, maxValue, step, parent, name)
{
}

WrappingSpinBox::~WrappingSpinBox()
{
}


/** Overloaded QSpinBox method */
void WrappingSpinBox::stepUp()
{
	bool wrap = false;
	if (value() == 59)
		wrap = true;
	if (wrap)
		emit wrapUp();              // must wrap first (to avoid double-step-up)
	QSpinBox::stepUp();
}

/** Overloaded QSpinBox method */
void WrappingSpinBox::stepDown()
{
	bool wrap = false;
	if (value() == 0)
		wrap = true;
	QSpinBox::stepDown();
	if (wrap)
		emit wrapDown();
}


// -------------------------------------------------------------------------


TimeEdit::TimeEdit(QWidget* parent, const char* name)
    : QWidget(parent, name)
{
	layout = new QHBoxLayout(this);
	minuteBox = new QSpinBox(0, 300, 1, this);
//	minuteBox->setFixedSize(minuteBox->sizeHint());

	QLabel* min = new QLabel(i18n(" min"), this);
	min->setFixedSize(min->sizeHint());
	secondBox = new WrappingSpinBox(0, 59, 1, this);
	secondBox->setWrapping(true);
//	secondBox->setFixedSize(secondBox->sizeHint());

	QLabel* sec = new QLabel(i18n(" sec"),this);
	sec->setFixedSize(sec->sizeHint());

	layout->addWidget(minuteBox);
	layout->addWidget(min);

	layout->addWidget(secondBox);
	layout->addWidget(sec);

	connect(minuteBox, SIGNAL(valueChanged(int)), SLOT(spinBoxValueChanged(int)) );
	connect(secondBox, SIGNAL(valueChanged(int)), SLOT(spinBoxValueChanged(int)) );
	connect(secondBox, SIGNAL(wrapUp()), SLOT(wrappedUp()));
	connect(secondBox, SIGNAL(wrapDown()), SLOT(wrappedDown()));
}

TimeEdit::~TimeEdit()
{
}

/** Set to specified number of seconds. */
void TimeEdit::setValue(int val)
{
	if (val < 0)
		return;

	// block signals to avoid receiption of valueChanged()
	// between changing of minutes and seconds
	secondBox->blockSignals(true);
	minuteBox->blockSignals(true);

	secondBox->setValue(val % 60);
	minuteBox->setValue(val / 60);

	secondBox->blockSignals(false);
	minuteBox->blockSignals(false);

	emit valueChanged(value());
}

/** Return current value in seconds. */
int TimeEdit::value()
{
	return minuteBox->value()*60 + secondBox->value();
}

/** SLOT: Handle wrap-up of seconds-box */
void TimeEdit::wrappedUp()
{
	if (minuteBox->value() != minuteBox->maxValue()) {
		minuteBox->stepUp();
	} else {
		secondBox->setValue(58);    // ugly: must cater for wrapping-first
	}
}

/** SLOT: Handle wrap-down of seconds-box */
void TimeEdit::wrappedDown()
{
	// well, the "if" should always be true
	if (minuteBox->value() != minuteBox->minValue()) {
		minuteBox->stepDown();
	} else {
		secondBox->setValue(0);
	}
}

/** SLOT: Handle any change in minutes of seconds */
void TimeEdit::spinBoxValueChanged(int)
{
	if (value() == 0) {
		secondBox->stepUp();        // this will give another spinBoxValueChanged() invocation
		return;
	}

	emit valueChanged(value());
}

/** SLOT (overloading QSpinBox): set focus */
void TimeEdit::setFocus()
{
	minuteBox->setFocus();
}
