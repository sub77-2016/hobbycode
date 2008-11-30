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
#include "../Projection.h"
#include "../Neuron.h"
#include "../WinnerTakeAllPopulation.h"

class MockProjection : public verve::Projection
{
public:
	MockProjection()
	: Projection()
	{
		mNumConnections = 0;
	}

	virtual unsigned int VERVE_CALL getNumConnections()const
	{
		return mNumConnections;
	}

	virtual void connectNeurons(verve::Neuron* neuron1, 
		verve::Neuron* neuron2)
	{
		++mNumConnections;
	}

private:
	unsigned int mNumConnections;
};

QT_TEST(testProjectionCorrectNumConnections)
{
	verve::Neuron n1(0);
	verve::WinnerTakeAllPopulation pop1;
	pop1.init(3);
	MockProjection proj1;
	proj1.init(&n1, &pop1, verve::IDEAL_NOISE, 1);
	QT_CHECK_EQUAL(proj1.getNumConnections(), 3);

	verve::Neuron n2(0);
	verve::WinnerTakeAllPopulation pop2;
	pop2.init(5);
	MockProjection proj2;
	proj2.init(&pop2, &n2, verve::IDEAL_NOISE, 1);
	QT_CHECK_EQUAL(proj2.getNumConnections(), 5);

	verve::WinnerTakeAllPopulation pop3;
	verve::WinnerTakeAllPopulation pop4;
	pop3.init(4);
	pop4.init(6);
	MockProjection proj3;
	proj3.init(&pop3, &pop4, verve::IDEAL_NOISE, 1);
	QT_CHECK_EQUAL(proj3.getNumConnections(), 24);
}
