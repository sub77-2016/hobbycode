#include <quicktest/quicktest.h>
#include "../../../src/LinearNeuron.h"
#include "../../../src/Connection.h"
//#include "../../../src/InputNeuron.h"
//#include "../../../src/RBFNeuron.h"

QT_TEST(testLinearNeuronBaseFiringRate)
{
	verve::LinearNeuron n(0);
	QT_CHECK_CLOSE(n.getFiringRate(), 0, (verve::real)0.0001);
	n.updateFiringRate();
	QT_CHECK_CLOSE(n.getFiringRate(), 0, (verve::real)0.0001);
}

struct LinearNeuronFixture
{
	LinearNeuronFixture()
	: outputNeuron(0), 
	preNeuron1(0), 
	preNeuron2(0), 
	dendrite1(&preNeuron1, &outputNeuron), 
	dendrite2(&preNeuron2, &outputNeuron)
	{
		outputNeuron.addDendrite(&dendrite1);
		outputNeuron.addDendrite(&dendrite2);
		preNeuron1.addAxon(&dendrite1);
		preNeuron2.addAxon(&dendrite2);
	}

	verve::LinearNeuron outputNeuron;
	verve::Neuron preNeuron1;
	verve::Neuron preNeuron2;
	verve::Connection dendrite1;
	verve::Connection dendrite2;
};

QT_TEST(testLinearNeuronCorrectActivation)
{
	LinearNeuronFixture f;
	f.dendrite1.setWeight((verve::real)0.4);
	f.dendrite2.setWeight((verve::real)-0.15);
	f.preNeuron1.setFiringRate((verve::real)0.91);
	f.preNeuron2.setFiringRate((verve::real)0.26);
	f.outputNeuron.updateFiringRate();

	// Firing rate should equal 0.4 * 0.91 + -0.15 * 0.26 = 0.325
	QT_CHECK_CLOSE(f.outputNeuron.getFiringRate(), (verve::real)0.325, 
		(verve::real)0.0001);
}

// UPDATE: This is used for bounded linear activation only.
//QT_TEST(testLinearNeuronActivationLowBoundary)
//{
//	LinearNeuronFixture f;
//	f.dendrite1.setWeight((verve::real)-0.25);
//	f.dendrite2.setWeight((verve::real)-0.26);
//	f.preNeuron1.setFiringRate((verve::real)1);
//	f.preNeuron2.setFiringRate((verve::real)1);
//	f.outputNeuron.sumInput();
//	f.outputNeuron.updateFiringRate();
//	verve::real initialFiringRate = f.outputNeuron.getFiringRate();
//	QT_CHECK_CLOSE(initialFiringRate, 0, (verve::real)0.0001);
//
//	f.dendrite2.setWeight((verve::real)-0.25);
//	f.outputNeuron.sumInput();
//	f.outputNeuron.updateFiringRate();
//	QT_CHECK_EQUAL(f.outputNeuron.getFiringRate(), initialFiringRate);
//
//	f.dendrite2.setWeight((verve::real)-0.24);
//	f.outputNeuron.sumInput();
//	f.outputNeuron.updateFiringRate();
//	QT_CHECK_GREATER(f.outputNeuron.getFiringRate(), initialFiringRate);
//}
//
// UPDATE: This is used for bounded linear activation only.
//QT_TEST(testLinearNeuronActivationHighBoundary)
//{
//	LinearNeuronFixture f;
//	f.dendrite1.setWeight((verve::real)0.25);
//	f.dendrite2.setWeight((verve::real)0.26);
//	f.preNeuron1.setFiringRate((verve::real)1);
//	f.preNeuron2.setFiringRate((verve::real)1);
//	f.outputNeuron.sumInput();
//	f.outputNeuron.updateFiringRate();
//	verve::real initialFiringRate = f.outputNeuron.getFiringRate();
//	QT_CHECK_CLOSE(initialFiringRate, 1, (verve::real)0.0001);
//
//	f.dendrite2.setWeight((verve::real)0.25);
//	f.outputNeuron.sumInput();
//	f.outputNeuron.updateFiringRate();
//	QT_CHECK_EQUAL(f.outputNeuron.getFiringRate(), initialFiringRate);
//
//	f.dendrite2.setWeight((verve::real)0.24);
//	f.outputNeuron.sumInput();
//	f.outputNeuron.updateFiringRate();
//	QT_CHECK_LESS(f.outputNeuron.getFiringRate(), initialFiringRate);
//}
