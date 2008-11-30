#include <quicktest/quicktest.h>
#include "../../../src/ContinuousInputChannel.h"

QT_TEST(testContinuousInputChannelCorrectInitialValues)
{
	verve::ContinuousInputChannel c(6);

	QT_CHECK_EQUAL(c.getOldValue(), -1);
	QT_CHECK_EQUAL(c.getNewValue(), -1);
}

QT_TEST(testContinuousInputChannelMakeNewValueOld)
{
	verve::ContinuousInputChannel c(6);
	c.setNewContinuousInput(1);
	
	QT_CHECK_EQUAL(c.getOldValue(), -1);
	QT_CHECK_EQUAL(c.getNewValue(), 5);

	c.makeNewValueOld();

	QT_CHECK_EQUAL(c.getOldValue(), 5);
	QT_CHECK_EQUAL(c.getNewValue(), 5);
}

QT_TEST(testContinuousInputChannelCorrectDiscretization)
{
	verve::ContinuousInputChannel c(6);

	QT_CHECK_EQUAL(c.getNewValue(), -1);

	unsigned int value = 0;
	unsigned int prevValue = 0;

	for (verve::real input = -1; input < 1; input += (verve::real)0.1)
	{
		c.setNewContinuousInput(input);
		value = c.getNewValue();

		QT_CHECK_GREATER_OR_EQUAL(value, prevValue);

		prevValue = value;
	}
}
