#include <quicktest/quicktest.h>
#include "../../../src/InputNeuron.h"
#include "../../../src/Connection.h"

QT_TEST(testInputNeuronIsCircularDefault)
{
	verve::InputNeuron n(0);
	QT_CHECK_EQUAL(n.isCircular(), false);
}
