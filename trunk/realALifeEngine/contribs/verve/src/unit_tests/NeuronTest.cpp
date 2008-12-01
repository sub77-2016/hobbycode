/************************************************************************
* Verve                                                                 *
* http://verve-agents.sourceforge.net                                   *
* Copyright (C) 2004-2008                                               *
* Tyler Streeter (http://www.tylerstreeter.net)                         *
*                                                                       *
* This library is free software; you can redistribute it and/or         *
* modify it under the terms of EITHER:                                  *
*   (1) The GNU Lesser General Public License as published by the Free  *
*       Software Foundation; either version 2.1 of the License, or (at  *
*       your option) any later version. The text of the GNU Lesser      *
*       General Public License is included with this library in the     *
*       file license-LGPL.txt.                                          *
*   (2) The BSD-style license that is included with this library in     *
*       the file license-BSD.txt.                                       *
*                                                                       *
* This library is distributed in the hope that it will be useful,       *
* but WITHOUT ANY WARRANTY; without even the implied warranty of        *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the files    *
* license-LGPL.txt and license-BSD.txt for more details.                *
************************************************************************/

#include "../external/quicktest/quicktest.h"
#include "../Neuron.h"
#include "../Connection.h"

QT_TEST(testNeuronAddDendrite)
{
	verve::Neuron preNeuron(0);
	verve::Neuron postNeuron(0);
	verve::Connection c(&preNeuron, &postNeuron);
	int numDendrites = postNeuron.getNumDendrites();
	postNeuron.addDendrite(&c);
	QT_CHECK_EQUAL(postNeuron.getNumDendrites(), numDendrites + 1);
}

QT_TEST(testNeuronAddAxon)
{
	verve::Neuron preNeuron(0);
	verve::Neuron postNeuron(0);
	verve::Connection c(&preNeuron, &postNeuron);
	int numAxons = preNeuron.getNumAxons();
	preNeuron.addAxon(&c);
	QT_CHECK_EQUAL(preNeuron.getNumAxons(), numAxons + 1);
}

QT_TEST(testNeuronResetShortTermMemory)
{
	verve::Neuron n(0);
	n.setFiringRate(1);
	n.resetShortTermMemory();
	QT_CHECK_CLOSE(n.getFiringRate(), 0, (verve::real)0.0001);
}

struct NeuronFixture
{
	NeuronFixture()
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

	verve::Neuron outputNeuron;
	verve::Neuron preNeuron1;
	verve::Neuron preNeuron2;
	verve::Connection dendrite1;
	verve::Connection dendrite2;
};

QT_TEST(testNeuronCorrectInitialFiringRate)
{
	verve::Neuron n(0);
	QT_CHECK_CLOSE(n.getFiringRate(), 0, (verve::real)0.0001);
}

QT_TEST(testNeuronCorrectBaseLinearFiringRate)
{
	verve::Neuron n(0);
	n.updateFiringRateLinear();
	QT_CHECK_CLOSE(n.getFiringRate(), 0, (verve::real)0.0001);
}

QT_TEST(testNeuronCorrectBaseLinearBoundedNegOneToOneFiringRate)
{
	verve::Neuron n(0);
	n.updateFiringRateLinearBoundedNegOneToOne();
	QT_CHECK_CLOSE(n.getFiringRate(), 0, (verve::real)0.0001);
}

QT_TEST(testNeuronCorrectBaseSigmoidFiringRate)
{
	verve::Neuron n(0);
	n.updateFiringRateSigmoid();
	QT_CHECK_CLOSE(n.getFiringRate(), (verve::real)0.5, (verve::real)0.0001);
}

QT_TEST(testNeuronCorrectLinearActivation)
{
	NeuronFixture f;
	f.dendrite1.setWeight((verve::real)0.4);
	f.dendrite2.setWeight((verve::real)-0.15);
	f.preNeuron1.setFiringRate((verve::real)0.91);
	f.preNeuron2.setFiringRate((verve::real)0.26);
	f.outputNeuron.updateFiringRateLinear();

	// Firing rate should equal 0.4 * 0.91 + -0.15 * 0.26 = 0.325
	QT_CHECK_CLOSE(f.outputNeuron.getFiringRate(), (verve::real)0.325, 
		(verve::real)0.0001);
}

QT_TEST(testNeuronCorrectLinearBoundedNegOneToOneActivation)
{
	NeuronFixture f;
	f.dendrite1.setWeight((verve::real)0.4);
	f.dendrite2.setWeight((verve::real)-0.15);
	f.preNeuron1.setFiringRate((verve::real)0.91);
	f.preNeuron2.setFiringRate((verve::real)0.26);
	f.outputNeuron.updateFiringRateLinearBoundedNegOneToOne();

	// Firing rate should equal 0.4 * 0.91 + -0.15 * 0.26 = 0.325
	QT_CHECK_CLOSE(f.outputNeuron.getFiringRate(), (verve::real)0.325, 
		(verve::real)0.0001);
}

QT_TEST(testNeuronCorrectSigmoidActivation)
{
	NeuronFixture f;
	f.dendrite1.setWeight((verve::real)0.4);
	f.dendrite2.setWeight((verve::real)-0.15);
	f.preNeuron1.setFiringRate((verve::real)0.91);
	f.preNeuron2.setFiringRate((verve::real)0.26);
	f.outputNeuron.updateFiringRateSigmoid();
	QT_CHECK_CLOSE(f.outputNeuron.getFiringRate(), (verve::real)0.5805423, 
		(verve::real)0.0001);
}
