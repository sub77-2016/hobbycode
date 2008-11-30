#include <quicktest/quicktest.h>
#include "../../../src/ContinuousRBFPopulation.h"
#include "../../../src/RBFNeuron.h"

QT_TEST(testContinuousRBFPopulationSetRBFCenters)
{
	verve::ContinuousRBFPopulation pop;
	pop.create(10);

	QT_CHECK_CLOSE(static_cast<verve::RBFNeuron*>(pop.getNeuron(0))->
		getCenter(), (verve::real)-0.9, (verve::real)0.0001);
	QT_CHECK_CLOSE(static_cast<verve::RBFNeuron*>(pop.getNeuron(2))->
		getCenter(), (verve::real)-0.5, (verve::real)0.0001);
	QT_CHECK_CLOSE(static_cast<verve::RBFNeuron*>(pop.getNeuron(6))->
		getCenter(), (verve::real)0.3, (verve::real)0.0001);
	QT_CHECK_CLOSE(static_cast<verve::RBFNeuron*>(pop.getNeuron(9))->
		getCenter(), (verve::real)0.9, (verve::real)0.0001);
}

QT_TEST(testContinuousRBFPopulationCorrectActivation)
{
	verve::ContinuousRBFPopulation pop;
	pop.create(11);
	verve::Neuron* centerNeuron = pop.getNeuron(5);

	// Check when input is far from the RBF center.
	pop.setNextInput(-1);
	pop.switchToNextInput();
	pop.updateFiringRates();
	verve::real farOutput1 = centerNeuron->getFiringRate();
	pop.setNextInput(1);
	pop.switchToNextInput();
	pop.updateFiringRates();
	verve::real farOutput2 = centerNeuron->getFiringRate();
	QT_CHECK_CLOSE(farOutput1, farOutput2, (verve::real)0.0001);
	QT_CHECK_CLOSE(farOutput1, 0, (verve::real)0.0001);

	// Check when input is close to the RBF center.
	pop.setNextInput((verve::real)-0.1);
	pop.switchToNextInput();
	pop.updateFiringRates();
	verve::real nearOutput1 = centerNeuron->getFiringRate();
	pop.setNextInput((verve::real)0.1);
	pop.switchToNextInput();
	pop.updateFiringRates();
	verve::real nearOutput2 = centerNeuron->getFiringRate();
	QT_CHECK_CLOSE(nearOutput1, nearOutput2, (verve::real)0.0001);
	QT_CHECK_GREATER(nearOutput1, farOutput1);

	// Check when input is right on the RBF center.
	pop.setNextInput(0);
	pop.switchToNextInput();
	pop.updateFiringRates();
	QT_CHECK_CLOSE(centerNeuron->getFiringRate(), 1, (verve::real)0.0001);
	QT_CHECK_GREATER(centerNeuron->getFiringRate(), nearOutput1);
}

QT_TEST(testContinuousRBFPopulationCircularInputChannelLimits)
{
	verve::ContinuousRBFPopulation pop;
	pop.setCircular(true);
	pop.create(5);
	verve::Neuron* lowerLimitNeuron = pop.getNeuron(0);
	verve::Neuron* upperLimitNeuron = pop.getNeuron(4);

	// Test one extreme.
	pop.setNextInput(-1);
	pop.switchToNextInput();
	pop.updateFiringRates();
	QT_CHECK_GREATER(lowerLimitNeuron->getFiringRate(), (verve::real)0.2);
	QT_CHECK_GREATER(upperLimitNeuron->getFiringRate(), (verve::real)0.2);
	QT_CHECK_CLOSE(lowerLimitNeuron->getFiringRate(), 
		upperLimitNeuron->getFiringRate(), (verve::real)0.2);

	// Test the other extreme.
	pop.setNextInput(1);
	pop.switchToNextInput();
	pop.updateFiringRates();
	QT_CHECK_GREATER(lowerLimitNeuron->getFiringRate(), (verve::real)0.2);
	QT_CHECK_GREATER(upperLimitNeuron->getFiringRate(), (verve::real)0.2);
	QT_CHECK_CLOSE(lowerLimitNeuron->getFiringRate(), 
		upperLimitNeuron->getFiringRate(), (verve::real)0.2);
}
