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
#include "../WinnerTakeAllPopulation.h"
#include "../UltraSparseCodePopulation.h"
#include "../Projection.h"
#include "../Connection.h"
#include "../Neuron.h"

class MockNeuron : public verve::Neuron
{
public:
	MockNeuron(unsigned int id)
	: Neuron(id)
	{
	}

	verve::real getSynapticInputSum()const
	{
		return mSynapticInputSum;
	}
};

class MockWTAPopulation : public verve::WinnerTakeAllPopulation
{
public:
	MockWTAPopulation()
	: WinnerTakeAllPopulation()
	{
	}

	virtual void createNeuron(unsigned int id)
	{
		verve::Neuron* neuron = new MockNeuron(id);
		mNeurons.push_back(neuron);
	}

	unsigned int computeWinner()
	{
		return verve::WinnerTakeAllPopulation::computeWinnerIndex();
	}

protected:
	virtual void clear()
	{
		// VC8 requires this to be implemented for some reason?
	}
};

// This test is disabled for now because we're not using the normal 
// winner-take-all method.
//QT_TEST(testWinnerTakeAllPopulationRunWinnerTakeAll)
//{
//	MockWTAPopulation pop;
//	unsigned int numNeurons = 10;
//	pop.init(numNeurons);
//
//	for (unsigned int i = 0; i < numNeurons; ++i)
//	{
//		verve::real randomRate = 0;
//
//		if (3 == i)
//		{
//			randomRate = verve::globals::randomRealUniform(
//				(verve::real)0.6, 1);
//		}
//		else
//		{
//			randomRate = verve::globals::randomRealUniform(0, 
//				(verve::real)0.4);
//		}
//
//		pop.getNeuron(i)->setFiringRate(randomRate);
//	}
//
//	pop.applyOutputProcessing();
//
//	QT_CHECK_CLOSE(pop.getNeuron(3)->getFiringRate(), 1, (verve::real)0.0001);
//	QT_CHECK_EQUAL(pop.getActiveOutput(), 3);
//
//	unsigned int winnerCount = 0;
//	for (unsigned int i = 0; i < numNeurons; ++i)
//	{
//		if (pop.getNeuron(i)->getFiringRate() > (verve::real)0.99)
//		{
//			++winnerCount;
//		}
//	}
//
//	QT_CHECK_EQUAL(winnerCount, 1);
//}

// This test is temporarily disabled because (in VC8) our MockWTAPopulation 
// can't access the protected computeWinnerIndex() function from within a 
// public function. ???
//QT_TEST(testWinnerTakeAllPopulationRunProbWinnerTakeAll)
//{
//	MockWTAPopulation pop;
//	unsigned int numNeurons = 3;
//	pop.init(numNeurons);
//
//	// Since we are analyzing a probabilistic algorithm, sample it several 
//	// times and average the results.  Note that this test might fail 
//	// every once in a while.
//	verve::real neuron1AvgFiringRate = 0;
//	unsigned int numSamples = 300;
//	for (unsigned int i = 0; i < numSamples; ++i)
//	{
//		pop.getNeuron(0)->setFiringRate((verve::real)0.1);
//		pop.getNeuron(1)->setFiringRate((verve::real)0.8);
//		pop.getNeuron(2)->setFiringRate((verve::real)0.1);
//
//		if (pop.computeWinner() == 1)
//		{
//			neuron1AvgFiringRate += 1;
//		}
//	}
//
//	neuron1AvgFiringRate = neuron1AvgFiringRate / (verve::real)numSamples;
//
//	QT_CHECK_CLOSE(neuron1AvgFiringRate, (verve::real)0.8, (verve::real)0.1);
//
//	// Count the number of "winner" Neurons which should equal 1.
//	unsigned int winnerCount = 0;
//	for (unsigned int i = 0; i < numNeurons; ++i)
//	{
//		if (pop.getNeuron(i)->getFiringRate() > (verve::real)0.99)
//		{
//			++winnerCount;
//		}
//	}
//
//	QT_CHECK_EQUAL(winnerCount, 1);
//}

QT_TEST(testWinnerTakeAllPopulationInitialActiveOutput)
{
	MockWTAPopulation pop;
	pop.init(6);

	QT_CHECK_EQUAL(pop.getActiveOutput(), -1);
}

QT_TEST(testWinnerTakeAllPopulationInitialSynapticInputSum)
{
	MockWTAPopulation pop;
	pop.init(2);

	MockNeuron* n = static_cast<MockNeuron*>(pop.getNeuron(0));
	QT_CHECK_CLOSE(n->getSynapticInputSum(), 0, (verve::real)0.0001);

	n = static_cast<MockNeuron*>(pop.getNeuron(1));
	QT_CHECK_CLOSE(n->getSynapticInputSum(), 0, (verve::real)0.0001);
}

QT_TEST(testWinnerTakeAllPopulationCorrectSynapticInputSumAfterChangingPreSynapticFiringRates)
{
	verve::UltraSparseCodePopulation statePop;
	statePop.init(2);
	MockWTAPopulation wtaPop;
	wtaPop.init(1);
	verve::Projection proj;
	proj.init(&statePop, &wtaPop, verve::IDEAL_NOISE, 1);
	proj.getConnection(0)->setWeight((verve::real)-0.4);
	proj.getConnection(1)->setWeight((verve::real)0.3);

	MockNeuron* n = static_cast<MockNeuron*>(wtaPop.getNeuron(0));

	statePop.setActiveOutput(0);
	// Sum should be 1 * -0.4 + 0 * 0.3 = -0.4
	QT_CHECK_CLOSE(n->getSynapticInputSum(), (verve::real)-0.4, 
		(verve::real)0.0001);

	statePop.setActiveOutput(1);
	// Sum should be 0 * -0.4 + 1 * 0.3 = 0.3
	QT_CHECK_CLOSE(n->getSynapticInputSum(), (verve::real)0.3, 
		(verve::real)0.0001);
}

QT_TEST(testWinnerTakeAllPopulationCorrectSynapticInputSumAfterChangingConnectionWeights)
{
	verve::UltraSparseCodePopulation statePop;
	statePop.init(2);
	MockWTAPopulation wtaPop;
	wtaPop.init(1);
	verve::Projection proj;
	proj.init(&statePop, &wtaPop, verve::IDEAL_NOISE, 1);
	proj.getConnection(0)->setWeight((verve::real)-0.4);
	proj.getConnection(1)->setWeight((verve::real)0.3);
	statePop.setActiveOutput(0);

	MockNeuron* n = static_cast<MockNeuron*>(wtaPop.getNeuron(0));

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
