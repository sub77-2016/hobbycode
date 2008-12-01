#include <quicktest/quicktest.h>
#include "../../../src/DiscreteRBFPopulation.h"
#include "../../../src/RBFNeuron.h"

struct DiscreteRBFPopulationFixture
{
	DiscreteRBFPopulationFixture(unsigned int numOptions)
	{
		pop.create(numOptions);
	}

	/// Returns the number of active Neurons (i.e. maximal firing rate).
	unsigned int getActiveCount()
	{
		unsigned int activeCount = 0;
		unsigned int size = (unsigned int)pop.getNumNeurons();
		for (unsigned int i = 0; i < size; ++i)
		{
			if (pop.getNeuron(i)->getFiringRate() > (verve::real)0.99)
			{
				++activeCount;
			}
		}

		return activeCount;
	}

	/// Returns the number of active Neurons (i.e. minimal firing rate).
	unsigned int getInactiveCount()
	{
		unsigned int inactiveCount = 0;
		unsigned int size = (unsigned int)pop.getNumNeurons();
		for (unsigned int i = 0; i < size; ++i)
		{
			if (pop.getNeuron(i)->getFiringRate() < (verve::real)0.01)
			{
				++inactiveCount;
			}
		}

		return inactiveCount;
	}

	/// Returns the index of the single active Neuron.
	unsigned int getActiveIndex()
	{
		unsigned int size = (unsigned int)pop.getNumNeurons();
		for (unsigned int i = 0; i < size; ++i)
		{
			if (pop.getNeuron(i)->getFiringRate() > (verve::real)0.99)
			{
				return i;
			}
		}

		assert(false);
		return 0;
	}

	verve::DiscreteRBFPopulation pop;
};

QT_TEST(testDiscreteRBFPopulationAlwaysOneOutputActive)
{
	DiscreteRBFPopulationFixture f(10);

	// Test before setting any inputs.
	QT_CHECK_EQUAL(f.getActiveCount(), 1);
	QT_CHECK_EQUAL(f.getInactiveCount(), 9);

	// Test after changing the input value.
	f.pop.setNextInput(4);
	f.pop.switchToNextInput();
	f.pop.updateFiringRates();
	QT_CHECK_EQUAL(f.getActiveCount(), 1);
	QT_CHECK_EQUAL(f.getInactiveCount(), 9);

	// Test after changing the input value once more.
	f.pop.setNextInput(9);
	f.pop.switchToNextInput();
	f.pop.updateFiringRates();
	QT_CHECK_EQUAL(f.getActiveCount(), 1);
	QT_CHECK_EQUAL(f.getInactiveCount(), 9);
}

QT_TEST(testDiscreteRBFPopulationCorrectActivation)
{
	// Make sure the right Neuron is active after setting the input.
	DiscreteRBFPopulationFixture f(10);

	// Check the initial value.
	QT_CHECK_EQUAL(f.getActiveIndex(), 0);

	// Check the value after setting it a few times.
	f.pop.setNextInput(3);
	f.pop.switchToNextInput();
	f.pop.updateFiringRates();
	QT_CHECK_EQUAL(f.getActiveIndex(), 3);
	f.pop.setNextInput(9);
	f.pop.switchToNextInput();
	f.pop.updateFiringRates();
	QT_CHECK_EQUAL(f.getActiveIndex(), 9);
	f.pop.setNextInput(2);
	f.pop.switchToNextInput();
	f.pop.updateFiringRates();
	QT_CHECK_EQUAL(f.getActiveIndex(), 2);
}
