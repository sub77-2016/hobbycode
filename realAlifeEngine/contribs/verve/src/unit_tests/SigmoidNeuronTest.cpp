#include <quicktest/quicktest.h>
#include "../../../src/SigmoidNeuron.h"
#include "../../../src/Connection.h"
//#include "../../../src/InputNeuron.h"
//#include "../../../src/RBFNeuron.h"

QT_TEST(testSigmoidNeuronBaseFiringRate)
{
	verve::SigmoidNeuron n(0);
	QT_CHECK_CLOSE(n.getFiringRate(), (verve::real)0.5, (verve::real)0.0001);
	n.updateFiringRate();
	QT_CHECK_CLOSE(n.getFiringRate(), (verve::real)0.5, (verve::real)0.0001);
}

struct SigmoidNeuronFixture
{
	SigmoidNeuronFixture()
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

	verve::SigmoidNeuron outputNeuron;
	verve::Neuron preNeuron1;
	verve::Neuron preNeuron2;
	verve::Connection dendrite1;
	verve::Connection dendrite2;
};

QT_TEST(testSigmoidNeuronCorrectActivation)
{
	SigmoidNeuronFixture f;
	f.dendrite1.setWeight((verve::real)0.4);
	f.dendrite2.setWeight((verve::real)-0.15);
	f.preNeuron1.setFiringRate((verve::real)0.91);
	f.preNeuron2.setFiringRate((verve::real)0.26);
	f.outputNeuron.updateFiringRate();
	QT_CHECK_CLOSE(f.outputNeuron.getFiringRate(), (verve::real)0.5805423, 
		(verve::real)0.0001);
}
