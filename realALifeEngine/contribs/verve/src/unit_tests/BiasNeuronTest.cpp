#include <quicktest/quicktest.h>
#include "../../../src/BiasNeuron.h"
#include "../../../src/Connection.h"

QT_TEST(testBiasNeuronResetShortTermMemory)
{
	verve::BiasNeuron n;
	n.setFiringRate(1);
	n.resetShortTermMemory();
	QT_CHECK_CLOSE(n.getFiringRate(), 1, (verve::real)0.0001);
}
