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

#include <verve/verve.h>
#include <fstream>
#include <math.h>

void testSineWave();
void testSawtoothWave();
void testSquareWave();
void testSpikeTrain();
verve::real sineWave(verve::real t, verve::real a, verve::real f);
verve::real sawtoothWave(verve::real t, verve::real a, verve::real f);
verve::real squareWave(verve::real t, verve::real a, verve::real f);
verve::real spikeTrain(verve::real t, verve::real a, verve::real f);
verve::real activateNeuron(verve::real input, verve::real dt);
void resetDynamics();

// make sure the input wave frequency is high enough to allow multiple time steps per cycle; 
// e.g. if the wave frequency is 10, the step size should be less than 1/10
const verve::real gDT = verve::defaults::stepSize;
const verve::real gMaxTime = 2;
const verve::real gNeuronTimeConstant = (verve::real)0.1;
const verve::real gWaveAmplitude = 15;
const verve::real gWaveFrequency = 1;

verve::Neuron* gSensorNeuron = NULL;
verve::Neuron* gOutputNeuron = NULL;
verve::Connection* gConnection = NULL;

int _main(int argc, char **argv)
{
	std::cout << "===============================================================" << std::endl;
	std::cout << "Verve leaky_neuron test" << std::endl;
	std::cout << "This will test the input/output response of a neuron " << std::endl;
	std::cout << "using various input signal patterns." << std::endl;
	std::cout << "Output data will be saved to files in the current directory." << std::endl;
	std::cout << "===============================================================" << std::endl;

	// create neurons and a connection between them; we will test
	// one neuron and use the other simply as a sensor neuron
	gSensorNeuron = new verve::Neuron(0, verve::NEURON_TYPE_INPUT, gNeuronTimeConstant, 0);
	gOutputNeuron = new verve::Neuron(1, verve::NEURON_TYPE_OUTPUT, gNeuronTimeConstant, 0);
	gConnection = new verve::Connection(gSensorNeuron, gOutputNeuron, 1);
	verve::real eTraceDecayFactor = 
		1 - verve::globals::calcDecayConstant(verve::defaults::ETraceDecayPercent, 
		verve::defaults::ETraceDecayTime, gDT);
	gConnection->setETraceDecayFactor(eTraceDecayFactor);
	gOutputNeuron->addDendrite(gConnection);

	// run the tests
	testSineWave();
	resetDynamics();

	testSawtoothWave();
	resetDynamics();

	testSquareWave();
	resetDynamics();

	testSpikeTrain();
	resetDynamics();

	gSensorNeuron->destroy();
	gOutputNeuron->destroy();
	gConnection->destroy();
	return 0;
}

void resetDynamics()
{
	gSensorNeuron->resetDynamics();
	gConnection->resetDynamics();
	gOutputNeuron->resetDynamics();
}

void testSineWave()
{
	std::ofstream file("../results/sine_wave.dat", std::ios::out);

	for (verve::real currentTime=0; currentTime < gMaxTime; currentTime += gDT)
	{
		verve::real input = sineWave(currentTime, gWaveAmplitude, gWaveFrequency);
		verve::real output = activateNeuron(input, gDT);
		file << currentTime << " " << input << " " << output << std::endl;
	}

	file.close();
}

void testSawtoothWave()
{
	std::ofstream file("../results/sawtooth_wave.dat", std::ios::out);

	for (verve::real currentTime=0; currentTime < gMaxTime; currentTime += gDT)
	{
		verve::real input = sawtoothWave(currentTime, gWaveAmplitude, gWaveFrequency);
		verve::real output = activateNeuron(input, gDT);
		file << currentTime << " " << input << " " << output << std::endl;
	}

	file.close();
}

void testSquareWave()
{
	std::ofstream file("../results/square_wave.dat", std::ios::out);

	for (verve::real currentTime=0; currentTime < gMaxTime; currentTime += gDT)
	{
		verve::real input = squareWave(currentTime, (verve::real)0.5*gWaveAmplitude, gWaveFrequency);
		verve::real output = activateNeuron(input, gDT);
		file << currentTime << " " << input << " " << output << std::endl;
	}

	file.close();
}

void testSpikeTrain()
{
	std::ofstream file("../results/spike_train.dat", std::ios::out);

	for (verve::real currentTime=0; currentTime < gMaxTime; currentTime += gDT)
	{
		verve::real input = spikeTrain(currentTime, (verve::real)0.5*gWaveAmplitude, gWaveFrequency);
		verve::real output = activateNeuron(input, gDT);
		file << currentTime << " " << input << " " << output << std::endl;
	}

	file.close();
}

verve::real sineWave(verve::real t, verve::real a, verve::real f)
{
	return ((verve::real)0.5 * a * sin(f * 2 * (verve::real)3.14159265358979323846 * t));
}

verve::real sawtoothWave(verve::real t, verve::real a, verve::real f)
{
	int numWaves = int(t * f);
	t -= (verve::real)numWaves * (1/f);

	return (a * t - (verve::real)0.01 * a);
}

verve::real squareWave(verve::real t, verve::real a, verve::real f)
{
	verve::real period = 1/f;
	int numWaves = int(t * f);
	t -= (verve::real)numWaves * period;
	
	if (t < period * (verve::real)0.5)
	{
		//return (verve::real)0.5 * a;
		return a;
	}
	else
	{
		//return -(verve::real)0.5 * a;
		return 0;
	}
}

verve::real spikeTrain(verve::real t, verve::real a, verve::real f)
{
	f *= 2;
	verve::real period = 1/f;
	int numWaves = int(t * f);
	t -= (verve::real)numWaves * (1/f);

	if (t > period - (verve::real)0.05 && t < period + (verve::real)0.05)
	{
		return a;
	}
	else
	{
		return 0;
	}
}

verve::real activateNeuron(verve::real input, verve::real dt)
{
	// normally this all gets handled in the neural network, but here we
	// are testing a single neuron so we have to do it manually

	// load sensor neuron
	gSensorNeuron->setFiringRate(input);

	// update the output neuron's synaptic input
	gOutputNeuron->updateSynapticInput();

	// update the output neuron's firing rate
	gOutputNeuron->updateFiringRate();

	// return the output neuron's membrane potential
	return gOutputNeuron->getMembranePotential();

	// return the output neuron's membrane potential
	//return gOutputNeuron->getFiringRate();

	//gConnection->updateETrace();
	//return gConnection->getETrace();
}
