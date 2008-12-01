#include <quicktest/quicktest.h>
#include "../../../src/IntegratorPopulation.h"
#include "../../../src/SigmoidNeuron.h"
#include "../../../src/UltraSparseCodePopulation.h"
#include "../../../src/TDProjection.h"
#include "../../../src/Connection.h"

class MockIntegratorPopulation : public verve::IntegratorPopulation
{
public:
	MockIntegratorPopulation()
	: IntegratorPopulation()
	{
	}

	void MockIntegratorPopulation::createNeuron(unsigned int id)
	{
		verve::SigmoidNeuron* neuron = new verve::SigmoidNeuron(id);
		mNeurons.push_back(neuron);
	}
};

QT_TEST(testIntegratorPopulationWinnerTakeAll)
{
	MockIntegratorPopulation pop;
	unsigned int numNeurons = 10;
	pop.create(numNeurons);
	pop.setOutputProcessingMethod(verve::WINNER_TAKE_ALL);

	for (unsigned int i = 0; i < numNeurons; ++i)
	{
		verve::real randomRate = 0;

		if (3 == i)
		{
			randomRate = verve::globals::randomRealUniform(
				(verve::real)0.6, 1);
		}
		else
		{
			randomRate = verve::globals::randomRealUniform(0, 
				(verve::real)0.4);
		}

		pop.getNeuron(i)->setFiringRate(randomRate);
	}

	pop.applyOutputProcessing();

	QT_CHECK_CLOSE(pop.getNeuron(3)->getFiringRate(), 1, (verve::real)0.0001);
	QT_CHECK_EQUAL(pop.getActiveOutput(), 3);

	unsigned int winnerCount = 0;
	for (unsigned int i = 0; i < numNeurons; ++i)
	{
		if (pop.getNeuron(i)->getFiringRate() > (verve::real)0.99)
		{
			++winnerCount;
		}
	}

	QT_CHECK_EQUAL(winnerCount, 1);
}

QT_TEST(testIntegratorPopulationProbWinnerTakeAll)
{
	MockIntegratorPopulation pop;
	unsigned int numNeurons = 3;
	pop.create(numNeurons);
	pop.setOutputProcessingMethod(verve::PROB_WINNER_TAKE_ALL);

	// Since we are analyzing a probabilistic algorithm, sample it several 
	// times and average the results.  Note that this test might fail 
	// every once in a while.
	verve::real avgFiringRate = 0;
	unsigned int numSamples = 300;
	for (unsigned int i = 0; i < numSamples; ++i)
	{
		pop.getNeuron(0)->setFiringRate((verve::real)0.1);
		pop.getNeuron(1)->setFiringRate((verve::real)0.8);
		pop.getNeuron(2)->setFiringRate((verve::real)0.1);

		pop.applyOutputProcessing();
		avgFiringRate += pop.getNeuron(1)->getFiringRate();
	}

	avgFiringRate = avgFiringRate / (verve::real)numSamples;

	QT_CHECK_CLOSE(avgFiringRate, (verve::real)0.8, (verve::real)0.1);

	// Count the number of "winner" Neurons which should equal 1.
	unsigned int winnerCount = 0;
	for (unsigned int i = 0; i < numNeurons; ++i)
	{
		if (pop.getNeuron(i)->getFiringRate() > (verve::real)0.99)
		{
			++winnerCount;
		}
	}

	QT_CHECK_EQUAL(winnerCount, 1);
}

QT_TEST(testIntegratorPopulationInitialActiveOutput)
{
	MockIntegratorPopulation pop;
	pop.create(6);

	QT_CHECK_EQUAL(pop.getActiveOutput(), -1);
}

QT_TEST(testIntegratorPopulationInitialSynapticInputSum)
{
	MockIntegratorPopulation pop;
	pop.create(2);

	verve::IntegratorNeuron* n = static_cast<verve::IntegratorNeuron*>(
		pop.getNeuron(0));
	QT_CHECK_CLOSE(n->getSynapticInputSum(), 0, (verve::real)0.0001);

	n = static_cast<verve::IntegratorNeuron*>(pop.getNeuron(1));
	QT_CHECK_CLOSE(n->getSynapticInputSum(), 0, (verve::real)0.0001);
}

QT_TEST(testIntegratorPopulationCorrectSynapticInputSumAfterChangingPreSynapticFiringRates)
{
	verve::UltraSparseCodePopulation statePop;
	statePop.create(2);
	MockIntegratorPopulation integratorPop;
	integratorPop.create(1);
	verve::TDProjection proj;
	proj.create(&statePop, &integratorPop);
	proj.getConnection(0)->setWeight((verve::real)-0.4);
	proj.getConnection(1)->setWeight((verve::real)0.3);

	verve::IntegratorNeuron* n = static_cast<verve::IntegratorNeuron*>(
		integratorPop.getNeuron(0));

	statePop.setActiveOutput(0);
	// Sum should be 1 * -0.4 + 0 * 0.3 = -0.4
	QT_CHECK_CLOSE(n->getSynapticInputSum(), (verve::real)-0.4, 
		(verve::real)0.0001);

	statePop.setActiveOutput(1);
	// Sum should be 0 * -0.4 + 1 * 0.3 = 0.3
	QT_CHECK_CLOSE(n->getSynapticInputSum(), (verve::real)0.3, 
		(verve::real)0.0001);
}

QT_TEST(testIntegratorPopulationCorrectSynapticInputSumAfterChangingConnectionWeights)
{
	verve::UltraSparseCodePopulation statePop;
	statePop.create(2);
	MockIntegratorPopulation integratorPop;
	integratorPop.create(1);
	verve::TDProjection proj;
	proj.create(&statePop, &integratorPop);
	proj.getConnection(0)->setWeight((verve::real)-0.4);
	proj.getConnection(1)->setWeight((verve::real)0.3);
	statePop.setActiveOutput(0);

	verve::IntegratorNeuron* n = static_cast<verve::IntegratorNeuron*>(
		integratorPop.getNeuron(0));

	// Sum should be 1 * -0.4 + 0 * 0.3 = -0.4
	QT_CHECK_CLOSE(n->getSynapticInputSum(), (verve::real)-0.4, 
		(verve::real)0.0001);

	proj.getConnection(0)->addToWeight((verve::real)0.5);
	// Sum should be 1 * 0.1 + 0 * 0.3 = 0.1
	QT_CHECK_CLOSE(n->getSynapticInputSum(), (verve::real)0.1, 
		(verve::real)0.0001);

	proj.getConnection(0)->setWeight((verve::real)-0.7);
	// Sum should be 1 * -0.7 + 0 * 0.3 = -0.7
	QT_CHECK_CLOSE(n->getSynapticInputSum(), (verve::real)-0.7, 
		(verve::real)0.0001);
}
