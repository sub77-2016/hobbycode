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

#include "PredictiveModel.h"
#include "RBFPopulation.h"
#include "Neuron.h"

namespace verve
{
	PredictiveModel::PredictiveModel(const Observation& obs, 
		bool isDynamicRBFEnabled, unsigned int numActions)
	{
		// Setup the RBFInputData structure.  We must add an additional 
		// discrete input for the action input.
		unsigned int* discreteNumOptionsData = 
			new unsigned int[obs.getNumDiscreteInputs() + 1];
		unsigned int* discreteInputData = 
			new unsigned int[obs.getNumDiscreteInputs() + 1];
		for (unsigned int i = 0; i < obs.getNumDiscreteInputs(); ++i)
		{
			discreteNumOptionsData[i] = obs.getDiscreteInputNumOptions(i);
			discreteInputData[i] = 0;
		}
		discreteNumOptionsData[obs.getNumDiscreteInputs()] = numActions;
		discreteInputData[obs.getNumDiscreteInputs()] = 0;

		mStateActionInputData.init(obs.getNumDiscreteInputs() + 1, 
			discreteNumOptionsData, 
			discreteInputData, 
			obs.getNumContinuousInputs(), 
			obs.getContinuousResolution(), 
			obs.getContinuousCircularData(), 
			obs.getContinuousInputData());

		delete [] discreteNumOptionsData;
		delete [] discreteInputData;

		mDiscObsTrainingData = new real[obs.getNumDiscreteInputs()];
		for (unsigned int i = 0; i < obs.getNumDiscreteInputs(); ++i)
		{
			mDiscObsTrainingData[i] = 0;
		}

		// Create the state-action representation Population.
		mStateActionRepresentation = new RBFPopulation();
		mStateActionRepresentation->init(mStateActionInputData, 
			isDynamicRBFEnabled);
		mAllPopulations.push_back(mStateActionRepresentation);

		// Create the discrete Observation prediction Population.
		mDiscObsPredPopulation = new Population();
		mDiscObsPredPopulation->init(obs.getNumDiscreteInputs());
		mAllPopulations.push_back(mDiscObsPredPopulation);

		// Create the continuous Observation prediction Population.
		mContObsPredPopulation = new Population();
		mContObsPredPopulation->init(obs.getNumContinuousInputs());
		mAllPopulations.push_back(mContObsPredPopulation);

		// Create the reward prediction Population.
		mRewardPredPopulation = new Population();
		mRewardPredPopulation->init(1);
		mAllPopulations.push_back(mRewardPredPopulation);

		// Create the uncertainty prediction Population.
		mUncertaintyPredPopulation = new Population();
		mUncertaintyPredPopulation->init(1);
		mAllPopulations.push_back(mUncertaintyPredPopulation);

		// Create a Projection from the state-action representation to the 
		// discrete Observation predictor.
		mStateActionRepresentation->project(mDiscObsPredPopulation, 
			IDEAL_NOISE, 
			mStateActionRepresentation->computeMaxActivationSum());

		// Create a Projection from the state-action representation to the 
		// continuous Observation predictor.
		mStateActionRepresentation->project(mContObsPredPopulation, 
			IDEAL_NOISE, 
			mStateActionRepresentation->computeMaxActivationSum());

		// Create a Projection from the state-action representation to the 
		// reward predictor.
		mStateActionRepresentation->project(mRewardPredPopulation, 
			IDEAL_NOISE, 
			mStateActionRepresentation->computeMaxActivationSum());

		// Create a Projection from the state-action representation to the 
		// uncertainty predictor.  Make the new weights initially positive, 
		// forcing the uncertainty predictions to be high at first.
		mStateActionRepresentation->project(mUncertaintyPredPopulation, 
			WEIGHTS_NEAR_1, 
			mStateActionRepresentation->computeMaxActivationSum());

		mLatestPredMSE = 0;
		mDeltaLearningTimeConstant = 0;
		mDeltaLearningFactor = 0;
	}

	PredictiveModel::~PredictiveModel()
	{
		delete [] mDiscObsTrainingData;

		// Destroy Populations, including the Neurons and Projections 
		// contained within them.
		while (!mAllPopulations.empty())
		{
			delete mAllPopulations.back();
			mAllPopulations.pop_back();
		}
	}

	void PredictiveModel::resetShortTermMemory()
	{
		mStateActionInputData.zeroInputData();

		mLatestPredMSE = 0;

		unsigned int size = (unsigned int)mAllPopulations.size();
		for (unsigned int i = 0; i < size; ++i)
		{
			mAllPopulations[i]->resetShortTermMemory();
		}
	}

	void PredictiveModel::predictAndTrain(const Observation& actualPrevObs, 
			unsigned int prevAction, const Observation& actualCurrentObs, 
			const real actualCurrentReward, Observation& predCurrentObs, 
			real& predCurrentReward, real& predUncertainty)
	{
		// Update the predictor Populations' outputs to represent 
		// the predicted current values.  Allow dynamic RBF creation 
		// since we're in training.
		predict(actualPrevObs, prevAction, predCurrentObs, 
			predCurrentReward, predUncertainty, true);

		// Now we must train the predictors using the actual current 
		// values.  We need to scale each actual discrete value to 
		// be within [-1, 1] first.
		unsigned numDiscreteValues = 
			actualCurrentObs.getNumDiscreteInputs();
		for (unsigned int i = 0; i < numDiscreteValues; ++i)
		{
			real increment = 2 / (real)
				(actualCurrentObs.getDiscreteInputNumOptions(i) - 1);
			mDiscObsTrainingData[i] = -1 + increment * 
				actualCurrentObs.getDiscreteValue(i);
		}
		real rewardPredMSE = mRewardPredPopulation->
			trainPreDeltaRuleLinear(&actualCurrentReward, 
			mDeltaLearningFactor);
		real discObsPredMSE = mDiscObsPredPopulation->
			trainPreDeltaRuleLinear(mDiscObsTrainingData, 
			mDeltaLearningFactor);
		real contObsPredMSE = mContObsPredPopulation->
			trainPreDeltaRuleLinear(
			actualCurrentObs.getContinuousInputData(), 
			mDeltaLearningFactor);

		// Here we combine the MSE from all predictions into a single MSE 
		// value.
		mLatestPredMSE = 
			(discObsPredMSE * mDiscObsPredPopulation->getNumNeurons() + 
			contObsPredMSE * mContObsPredPopulation->getNumNeurons() + 
			rewardPredMSE) / (mDiscObsPredPopulation->getNumNeurons() + 
			mContObsPredPopulation->getNumNeurons() + 1);

		// Now that we know the actual current uncertainty, we can train 
		// the uncertainty predictor.
		mUncertaintyPredPopulation->trainPreDeltaRuleLinear(
			&mLatestPredMSE, mDeltaLearningFactor);
	}

	void PredictiveModel::predict(const Observation& actualCurrentObs, 
		unsigned int currentAction, Observation& predNextObs, 
		real& predNextReward, real& predUncertainty, 
		bool allowDynamicRBFCreation)
	{
		// Convert the data for the current Observation and action into 
		// a form the state-action Population can use.
		unsigned int numDiscInputs = actualCurrentObs.getNumDiscreteInputs();
		for (unsigned int i = 0; i < numDiscInputs; ++i)
		{
			// Copy the discrete Observation input data.
			mStateActionInputData.discInputData[i] = 
				actualCurrentObs.getDiscreteValue(i);
		}

		// Copy the action.
		mStateActionInputData.discInputData[numDiscInputs] = currentAction;

		unsigned int numContInputs = 
			actualCurrentObs.getNumContinuousInputs();
		for (unsigned int i = 0; i < numContInputs; ++i)
		{
			mStateActionInputData.contInputData[i] = 
				actualCurrentObs.getContinuousValue(i);
		}

		// Update the state representation using the given current 
		// Observation and action.
		mStateActionRepresentation->updateFiringRatesRBF(
			mStateActionInputData, allowDynamicRBFCreation);

		// Update the predictor Populations.
		mDiscObsPredPopulation->updateFiringRatesLinearBoundedNegOneToOne();
		mContObsPredPopulation->updateFiringRatesLinearBoundedNegOneToOne();
		mRewardPredPopulation->updateFiringRatesLinearBoundedNegOneToOne();
		mUncertaintyPredPopulation->updateFiringRatesLinearBoundedZeroToOne();
		//// TESTING
		//mDiscObsPredPopulation->updateFiringRatesLinearBoundedNegOneToOne();
		//mContObsPredPopulation->updateFiringRatesLinear();
		//mRewardPredPopulation->updateFiringRatesLinear();
		//mUncertaintyPredPopulation->updateFiringRatesLinearBoundedZeroToOne();

		// Fill the predicted next Observation, reward, and uncertainty data 
		// to be returned.
		for (unsigned int i = 0; i < numDiscInputs; ++i)
		{
			// We must convert the discrete value from [-1, 1] to its 
			// actual range.  We must use a rounding function here to 
			// ensure correct results.
			unsigned int numOptions = 
				actualCurrentObs.getDiscreteInputNumOptions(i);
			real increment = 2 / (real)(numOptions - 1);
			int discValue = globals::roundToInt(
				(mDiscObsPredPopulation->getNeuron(i)->
				getFiringRate() + 1) / increment);

			if (discValue < 0)
			{
				discValue = 0;
			}
			else if (discValue >= (int)numOptions)
			{
				discValue = numOptions - 1;
			}
			predNextObs.setDiscreteValue(i, (unsigned int)discValue);
		}

		for (unsigned int i = 0; i < numContInputs; ++i)
		{
			predNextObs.setContinuousValue(i, 
				mContObsPredPopulation->getNeuron(i)->getFiringRate());
		}

		predNextReward = 
			mRewardPredPopulation->getNeuron(0)->getFiringRate();
		predUncertainty = 
			mUncertaintyPredPopulation->getNeuron(0)->getFiringRate();
	}

	void PredictiveModel::changeStepSize(real newValue)
	{
		setDeltaLearningRate(mDeltaLearningTimeConstant, newValue);
	}

	void PredictiveModel::setDeltaLearningRate(real timeConstant, 
		real stepSize)
	{
		mDeltaLearningTimeConstant = timeConstant;
		mDeltaLearningFactor = 1 - globals::calcDecayConstant(
			mDeltaLearningTimeConstant, stepSize);

		// The learning factor should be normalized as follows: 
		// 
		// learning factor = learning factor / # of active features
		// 
		// This method allows us to change the number of active features 
		// in the state representation without making learning unstable.  
		// Since we're using an RBF state representation, the number of 
		// active features is equal to the total sum of RBF activation.
		mDeltaLearningFactor = mDeltaLearningFactor / 
			mStateActionRepresentation->computeMaxActivationSum();
	}

	real PredictiveModel::getPredictionMSE()
	{
		return mLatestPredMSE;
	}
}
