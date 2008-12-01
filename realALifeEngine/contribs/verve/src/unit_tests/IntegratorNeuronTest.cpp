#include <quicktest/quicktest.h>
#include "../../../src/IntegratorNeuron.h"
#include "../../../src/Connection.h"
#include "../../../src/TDProjection.h"
#include "../../../src/UltraSparseCodePopulation.h"

class MockIntegratorNeuron : public verve::IntegratorNeuron
{
public:
	MockIntegratorNeuron(unsigned int id)
	: IntegratorNeuron(id)
	{
	}

protected:
	virtual verve::real computeNewFiringRate()
	{
		// Just use a linear activation function.
		return mSynapticInputSum;
	}
};

QT_TEST(testIntegratorNeuronResetShortTermMemory)
{
	MockIntegratorNeuron n(0);
	n.setFiringRate(1);
	n.resetShortTermMemory();
	QT_CHECK_CLOSE(n.getFiringRate(), 0, (verve::real)0.0001);
}

QT_TEST(testIntegratorNeuronNormalizeInputWeights)
{
	verve::UltraSparseCodePopulation pop;
	pop.create(20);
	MockIntegratorNeuron n(0);
	verve::TDProjection proj;
	proj.create(&pop, &n);
	proj.applyUniformNoise(-1, 1);
	n.normalizeInputWeights();

	// After normalization, the sum of squared weights should equal 1.
	verve::real sumOfSquaredWeights = 0;
	unsigned int size = proj.getNumConnections();
	for (unsigned int i = 0; i < size; ++i)
	{
		verve::real weight = proj.getConnection(i)->getWeight();
		sumOfSquaredWeights += (weight * weight);
	}

	QT_CHECK_CLOSE(sumOfSquaredWeights, 1, (verve::real)0.0001);
}
