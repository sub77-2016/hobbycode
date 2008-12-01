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
#include "../RBFNeuron.h"
#include "../RBFInputData.h"

QT_TEST(testRBFNeuronBaseFiringRate)
{
	verve::RBFNeuron n(0);
	QT_CHECK_CLOSE(n.getFiringRate(), 0, (verve::real)0.0001);
}

QT_TEST(testRBFNeuronDiscreteOnlyCorrectActivation)
{
	verve::RBFNeuron n(0);
	verve::real width = 1; // Width doesn't really matter here.
	verve::RBFInputData inputData;
	inputData.numDiscInputs = 2;
	inputData.discInputData = new unsigned int[2];
	inputData.discInputData[0] = 1;
	inputData.discInputData[1] = 1;
	n.init(width, width, inputData);

	inputData.discInputData[0] = 0;
	inputData.discInputData[1] = 0;
	n.updateFiringRate(inputData);
	QT_CHECK_CLOSE(n.getFiringRate(), 0, (verve::real)0.0001);

	inputData.discInputData[0] = 0;
	inputData.discInputData[1] = 1;
	n.updateFiringRate(inputData);
	QT_CHECK_CLOSE(n.getFiringRate(), 0, (verve::real)0.0001);

	inputData.discInputData[0] = 1;
	inputData.discInputData[1] = 0;
	n.updateFiringRate(inputData);
	QT_CHECK_CLOSE(n.getFiringRate(), 0, (verve::real)0.0001);

	inputData.discInputData[0] = 1;
	inputData.discInputData[1] = 1;
	n.updateFiringRate(inputData);
	QT_CHECK_CLOSE(n.getFiringRate(), 1, (verve::real)0.0001);
}

QT_TEST(testRBFNeuronContinuousOnlyCorrectActivation)
{
	// Position the RBF at (0.5, 0.5).
	verve::RBFNeuron n(0);
	verve::real width = (verve::real)0.1;
	verve::RBFInputData inputData;
	inputData.numContInputs = 2;
	inputData.contInputData = new verve::real[2];
	inputData.contInputData[0] = (verve::real)0.5;
	inputData.contInputData[1] = (verve::real)0.5;
	inputData.contCircularData = new bool[2];
	inputData.contCircularData[0] = false;
	inputData.contCircularData[1] = false;
	n.init(width, width, inputData);

	// Make sure the activation is the same when the input value is at 
	// (0, 1) and at (1, 0).
	inputData.contInputData[0] = 0;
	inputData.contInputData[1] = 1;
	n.updateFiringRate(inputData);
	verve::real farFiringRate = n.getFiringRate();

	inputData.contInputData[0] = 1;
	inputData.contInputData[1] = 0;
	n.updateFiringRate(inputData);
	QT_CHECK_CLOSE(n.getFiringRate(), farFiringRate, (verve::real)0.0001);

	// Make sure the activation is the same when the input value is at 
	// (0.45, 0.45) and at (0.55, 0.55).
	inputData.contInputData[0] = (verve::real)0.45;
	inputData.contInputData[1] = (verve::real)0.45;
	n.updateFiringRate(inputData);
	verve::real closeFiringRate = n.getFiringRate();

	inputData.contInputData[0] = (verve::real)0.55;
	inputData.contInputData[1] = (verve::real)0.55;
	n.updateFiringRate(inputData);
	QT_CHECK_CLOSE(n.getFiringRate(), closeFiringRate, (verve::real)0.0001);
	QT_CHECK_GREATER(closeFiringRate, farFiringRate);

	// Check when input is right on the RBF center.
	inputData.contInputData[0] = (verve::real)0.5;
	inputData.contInputData[1] = (verve::real)0.5;
	n.updateFiringRate(inputData);
	QT_CHECK_CLOSE(n.getFiringRate(), 1, (verve::real)0.0001);
	QT_CHECK_GREATER(n.getFiringRate(), closeFiringRate);
}

QT_TEST(testRBFNeuronDiscreteAndContinuousCorrectActivation)
{
	// Use 2 continuous dimensions and 1 discrete dimension.  Set the 
	// continuous position to (0.5, 0.5) and the discrete position to 
	// 1 (within the range [0, 2]).
	verve::RBFNeuron n(0);
	verve::real width = (verve::real)0.1;
	verve::RBFInputData inputData;
	inputData.numDiscInputs = 1;
	inputData.discInputData = new unsigned int[1];
	inputData.discInputData[0] = 1;
	inputData.numContInputs = 2;
	inputData.contInputData = new verve::real[2];
	inputData.contInputData[0] = (verve::real)0.5;
	inputData.contInputData[1] = (verve::real)0.5;
	inputData.contCircularData = new bool[2];
	inputData.contCircularData[0] = false;
	inputData.contCircularData[1] = false;
	n.init(width, width, inputData);

	// Make sure the activation is 0 when the discrete dimension does not 
	// match.
	inputData.discInputData[0] = 0;
	inputData.contInputData[0] = (verve::real)0.45;
	inputData.contInputData[1] = (verve::real)0.45;
	n.updateFiringRate(inputData);
	QT_CHECK_CLOSE(n.getFiringRate(), 0, (verve::real)0.0001);

	inputData.discInputData[0] = 2;
	inputData.contInputData[0] = (verve::real)0.45;
	inputData.contInputData[1] = (verve::real)0.45;
	n.updateFiringRate(inputData);
	QT_CHECK_CLOSE(n.getFiringRate(), 0, (verve::real)0.0001);

	// Make sure the activation is not 0 or 1 when the discrete dimension 
	// matches and the continuous dimensions are moderately close.
	inputData.discInputData[0] = 1;
	inputData.contInputData[0] = (verve::real)0.45;
	inputData.contInputData[1] = (verve::real)0.45;
	n.updateFiringRate(inputData);
	QT_CHECK_GREATER(n.getFiringRate(), (verve::real)0.1);
	QT_CHECK_LESS(n.getFiringRate(), (verve::real)0.9);

	// Make sure the activation is 1 when all dimensions match.
	inputData.discInputData[0] = 1;
	inputData.contInputData[0] = (verve::real)0.5;
	inputData.contInputData[1] = (verve::real)0.5;
	n.updateFiringRate(inputData);
	QT_CHECK_CLOSE(n.getFiringRate(), 1, (verve::real)0.0001);
}
