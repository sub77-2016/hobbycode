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

#include "Agent.h"
#include "Observation.h"
#include "PredictiveModel.h"
#include "RLModule.h"

namespace verve
{
	VERVE_EXPORT_FUNCTION Agent* VERVE_CALL createAgent(
		const AgentDescriptor& desc)
	{
		return new Agent(desc);
	}

	//VERVE_EXPORT_FUNCTION Agent* VERVE_CALL loadAgent(
	//	const std::string& filename)
	//{
	//	Agent* a = createAgent(1, 1);
	//	a->internal_load(filename);
	//	return a;
	//}

	Agent::Agent(const AgentDescriptor& desc)
	: mDescriptor(desc)
	{
		mActualPrevObs.init(*this);
		mPredCurrentObs.init(*this);
		mTempPlanningObs.init(*this);

		mRLModule = NULL;
		mPredictiveModel = NULL;

		// We will always need this.
		mRLModule = new RLModule(mPredCurrentObs, 
			desc.isDynamicRBFEnabled(), desc.getNumOutputs());

		switch(desc.getArchitecture())
		{
			case RL:
				// Do nothing extra.
				break;
			case MODEL_RL:
				mPredictiveModel = new PredictiveModel(mPredCurrentObs, 
					desc.isDynamicRBFEnabled(), desc.getNumOutputs());

				// TESTING: trying a predictive model that ignores the 
				// current action; this appears to work fine, but it 
				// has only been tested on a discrete maze task where 
				// the converged policy makes deterministic action 
				// selections.
				//mPredictiveModel = new PredictiveModel(mPredCurrentObs, 
				//	desc.isDynamicRBFEnabled(), 0);
				break;
			case CURIOUS_MODEL_RL:
				mPredictiveModel = new PredictiveModel(mPredCurrentObs, 
					desc.isDynamicRBFEnabled(), desc.getNumOutputs());
				break;
			default:
				assert(false);
				break;
		}

		mFirstStep = true;
		mActionIndex = 0;
		mLearningEnabled = true;
		mStepSize = defaults::stepSize;
		mAgeHours = 0;
		mAgeMinutes = 0;
		mAgeSeconds = 0;
		//mLastPlanningSequenceLength = 0;

		// The following step size-dependent factors must be initialized 
		// in the RLModule and PredictiveModel.
		setETraceTimeConstant(defaults::eTraceTimeConstant);
		setTDDiscountTimeConstant(defaults::TDDiscountTimeConstant);
		setTDLearningRate(defaults::valueFunctionLearningTimeConstant, 
			defaults::policyLearningMultiplier);
		setModelLearningRate(defaults::modelLearningTimeConstant);
	}

	Agent::~Agent()
	{
		delete mRLModule;
		if (mPredictiveModel)
		{
			delete mPredictiveModel;
		}
	}

	void Agent::destroy()
	{
		delete this;
	}

	void Agent::resetShortTermMemory()
	{
		mRLModule->resetShortTermMemory();
		if (mPredictiveModel)
		{
			mPredictiveModel->resetShortTermMemory();
		}

		mFirstStep = true;
		mActionIndex = 0;
		mActualPrevObs.zeroInputData();
		mPredCurrentObs.zeroInputData();
		mTempPlanningObs.zeroInputData();
		//mLastPlanningSequenceLength = 0;
		mDebugData.reset();
	}

	unsigned int Agent::update(real reinforcement, const Observation& obs, 
		real dt)
	{
		assert(reinforcement >= -1 && reinforcement <= 1);

		if (dt != mStepSize)
		{
			// We only need to recompute step size-dependent things when 
			// the dt changes.  Thus, as long as the dt is constant between 
			// successive updates, we rarely need to recompute these things.  
			// It also keeps the API simple (instead of having a public 
			// 'setStepSize(real)' and 'update()').
			setStepSize(dt);
		}

		if (mLearningEnabled)
		{
			incrementAge();

			if (mDescriptor.getArchitecture() == RL)
			{
				mActionIndex = mRLModule->update(obs, reinforcement);
			}
			else
			{
				// Do planning.

				// 2 PLANNING METHODS (currently, METHOD 1 is used): 
				//		1. RL component always uses predicted observation 
				//			and reward (i.e. predictive state 
				//			representation).
				//		2. RL component only uses predicted obs and reward 
				//			when in planning mode; otherwise, it uses the 
				//			actual values.

				if (mFirstStep)
				{
					// We must handle the first step differently 
					// because we do not yet have a valid previous-step 
					// Observation.  We do not update the model, and we 
					// simply update the RLModule with the actual 
					// Observation and reward.
					mActionIndex = mRLModule->update(obs, reinforcement);
				}
				else
				{
					// We train the predictive model once per update with 
					// the current Observation and reward, then we train 
					// the RLModule during planning.  Planning sequences 
					// proceed until the prediction uncertainty is too 
					// high or we exceed the max planning sequence length.  
					// These sequences might have zero length at first.  
					// Whether we do planning or not, at the end we need 
					// to have the RLModule's policy choose a new action.

					// Get the predicted current Observation and reward 
					// from the model.  This also trains the model based 
					// on the actual data.  At this point 'mActionIndex' 
					// represents the action from the previous step.  
					// Along with the predicted current Observation and 
					// reward, this returns a prediction uncertainty 
					// estimation.
					real predCurrentReward = 0;
					real predictionUncertaintyEstimation = 0;
					mPredictiveModel->predictAndTrain(mActualPrevObs, 
						mActionIndex, obs, reinforcement, 
						mPredCurrentObs, predCurrentReward, 
						predictionUncertaintyEstimation);

					// Perform a planning sequence to train the RLModule.  
					planningSequence(mPredCurrentObs, predCurrentReward, 
						predictionUncertaintyEstimation);

					// Have the RLModule's policy choose a new action from 
					// the predicted current Observation.
					mActionIndex = mRLModule->updatePolicyOnly(
						mPredCurrentObs);
				}
			}
		}
		else
		{
			if (mDescriptor.getArchitecture() == RL)
			{
				mActionIndex = mRLModule->updatePolicyOnly(obs);
			}
			else
			{
				if (mFirstStep)
				{
					// We must handle the first step differently 
					// because we do not yet have a valid previous-step 
					// Observation.  We do not update the model, and we 
					// simply update the RLModule's policy with the 
					// actual Observation.
					mActionIndex = mRLModule->updatePolicyOnly(obs);
				}
				else
				{
					// At this point 'mActionIndex' represents the 
					// action from the previous step.  Do not allow 
					// dynamic RBF creation here.  Since we're not 
					// learning here, we just ignore the predicted 
					// reward and uncertainty.
					real dummyPredCurrentReward = 0;
					real dummyPredictionUncertainty = 0;
					mPredictiveModel->predict(mActualPrevObs, 
						mActionIndex, mPredCurrentObs, 
						dummyPredCurrentReward, dummyPredictionUncertainty, 
						false);
					mActionIndex = mRLModule->updatePolicyOnly(
						mPredCurrentObs);
				}
			}
		}

		if (mFirstStep)
		{
			mFirstStep = false;
		}

		// Store a copy of the current actual Observation for next time.
		mActualPrevObs.copyInputData(obs);

		mDebugData.reward = reinforcement;
		mDebugData.valueEstimation = mRLModule->getLatestValueEstimation();
		mDebugData.TDError = mRLModule->getTDError();
		if (mPredictiveModel)
		{
			mDebugData.modelPredictionError = mPredictiveModel->
				getPredictionMSE();
		}

		return mActionIndex;
	}

	void Agent::planningSequence(const Observation& predCurrentObs, 
		real predCurrentReward, real initialUncertainty)
	{
		mDebugData.planLength = 0;
		mDebugData.predictionUncertaintyEstimation = initialUncertainty;

		// Continue planning as long as uncertainty is low enough.  This 
		// uses a predictive state representation: all inputs to the 
		// RLModule are predicted.
		const real uncertaintyThreshold = 
			mDescriptor.getPlanningUncertaintyThreshold();
		if (mDebugData.predictionUncertaintyEstimation < uncertaintyThreshold)
		{
			// Make sure the RLModule is fresh; it should not have any STM 
			// left over from previous updates at this point.
			mRLModule->resetShortTermMemory();

			// Setup temporary data to be used during planning.
			mTempPlanningObs.copyInputData(predCurrentObs);
			unsigned int tempActionIndex = 0;

			// We continue the planning sequence until either: 1) the 
			// prediction uncertainty is too high, or 2) we exceed the 
			// max plan length.  Note that the RLModule will only learn 
			// here if the number of planning steps is at least two.  This 
			// is because the RLModule must have data from two subsequent 
			// steps.
			while (mDebugData.planLength < 
				mDescriptor.getMaxNumPlanningSteps())
			{
				real totalReward = 0;

				if (mDescriptor.getArchitecture() == CURIOUS_MODEL_RL)
				{
					// Add in curiosity rewards.
					mDebugData.curiosityReward = computeCuriosityReward(
						mDebugData.predictionUncertaintyEstimation);

					// The current reward equals the predicted reward plus 
					// an extra curiosity reward proportional to 
					// uncertainty.
					totalReward = predCurrentReward + 
						mDebugData.curiosityReward;

					// Clamp the total reward to +1.
					if (totalReward > 1)
					{
						totalReward = 1;
					}
				}
				else
				{
					totalReward = predCurrentReward;
				}

				// Give the RLModule the current predicted Observation and 
				// reward.
				tempActionIndex = mRLModule->update(mTempPlanningObs, 
					totalReward);

				if (mDebugData.predictionUncertaintyEstimation > 
					uncertaintyThreshold)
				{
					// Predicted uncertainty is too high, so we'll stop the 
					// planning sequence.
					break;
				}

				// If this is not the last step, get new predicted data 
				// from the model for the next step.
				if (mDebugData.planLength < 
					mDescriptor.getMaxNumPlanningSteps())
				{
					// We are allowing RBF creation here; this is probably 
					// necessary for planning trajectories that enter new 
					// territory.
					real tempPredictionUncertainty = 0;
					mPredictiveModel->predict(mTempPlanningObs, 
						tempActionIndex, mTempPlanningObs, 
						predCurrentReward, tempPredictionUncertainty, true);

					//// Make the current uncertainty equal to the latest 
					//// prediction's uncertainty estimation.  
					//mDebugData.predictionUncertaintyEstimation = 
					//	tempPredictionUncertainty;

					// TESTING: Instead of the previous line, try accumulating 
					// uncertainty here.  This hasn't been tested very much 
					// yet, but it seems more realistic.
					mDebugData.predictionUncertaintyEstimation += 
						tempPredictionUncertainty;
				}

				++mDebugData.planLength;
			}
		}
	}

	real Agent::computeCuriosityReward(real uncertaintyEstimation)
	{
		// We're using a very simple model for now...
		return 3 * uncertaintyEstimation;
	}

	//void Agent::internal_load(const std::string& filename)
	//{
	//	TiXmlDocument file;
	//	bool success = file.LoadFile(filename.c_str());
	//	if (!success)
	//	{
	//		VERVE_LOGGER("warning") << 
	//			"verve::Agent::load: Failed to load XML file " 
	//			<< filename << "." << std::endl;
	//		return;
	//	}

	//	// Find the root element (i.e. the 'VerveAgent' element).
	//	TiXmlElement* rootElement = file.RootElement();
	//	if (NULL == rootElement)
	//	{
	//		VERVE_LOGGER("warning") << 
	//			"verve::Agent::load: Missing root element in " 
	//			<< filename << ". Ignoring file." << std::endl;
	//		return;
	//	}

	//	// Load the Agent's age attributes.
	//	mAgeHours = globals::getAttributeInt(rootElement, "ageHours");
	//	mAgeMinutes = globals::getAttributeInt(rootElement, "ageMinutes");
	//	mAgeSeconds = globals::getAttributeReal(rootElement, "ageSeconds");

	//	if (!mBrain->load(rootElement))
	//	{
	//		VERVE_LOGGER("warning") << 
	//			"verve::Agent::load: Could not load file " 
	//			<< filename << std::endl;
	//	}

	//	// Update: there are no longer different types of Agents.
	//	//// Make sure this is the right type of Agent.
	//	//std::string type = globals::getAttributeString(rootElement, "type");
	//	//if ("RL" != type)
	//	//{
	//	//	VERVE_LOGGER("warning") << 
	//	//		"verve::Agent::load: Wrong Agent type found.  Expected " 
	//	//		<< "type RL but found " << type << ".  Ignoring file " 
	//	//		<< filename << "." << std::endl;
	//	//	return;
	//	//}

	//	//success = true;

	//	//// Check if the actor element exists.
	//	//TiXmlNode* actorNode = rootElement->FirstChild("Actor");
	//	//if (!actorNode)
	//	//{
	//	//	VERVE_LOGGER("warning") << 
	//	//		"verve::Agent::load: Actor element not found in " 
	//	//		<< filename << ".  Ignoring file." << std::endl;
	//	//	success = false;
	//	//}

	//	//// Check if the positive critic element exists.
	//	//TiXmlNode* posCriticNode = rootElement->FirstChild("PositiveCritic");
	//	//if (!posCriticNode)
	//	//{
	//	//	VERVE_LOGGER("warning") << 
	//	//		"verve::Agent::load: PositiveCritic element not found in " 
	//	//		<< filename << ".  Ignoring file." << std::endl;
	//	//	success = false;
	//	//}

	//	//// Check if the negative critic element exists.
	//	//TiXmlNode* negCriticNode = rootElement->FirstChild("NegativeCritic");
	//	//if (!negCriticNode)
	//	//{
	//	//	VERVE_LOGGER("warning") << 
	//	//		"verve::Agent::load: NegativeCritic element not found in " 
	//	//		<< filename << ".  Ignoring file." << std::endl;
	//	//	success = false;
	//	//}

	//	//if (success)
	//	//{
	//	//	// Load the actor and critic NeuralArchitectures.

	//	//	success = mActor->load(actorNode, 
	//	//		defaults::actorMembraneTimeConstant, 
	//	//		defaults::actorLearningRatePercent, 
	//	//		defaults::actorETraceDecayPercent, 
	//	//		defaults::actorMaxExplorationNoise);

	//	//	if (!success)
	//	//	{
	//	//		VERVE_LOGGER("warning") << 
	//	//			"verve::Agent::load: Could not load file " 
	//	//			<< filename << std::endl;
	//	//	}

	//	//	success = mPositiveCritic->load(posCriticNode, 
	//	//		defaults::criticMembraneTimeConstant, 
	//	//		defaults::criticLearningRatePercent, 
	//	//		defaults::criticETraceDecayPercent, 0);
	//	//	if (!success)
	//	//	{
	//	//		VERVE_LOGGER("warning") << 
	//	//			"verve::Agent::load: Could not load file " 
	//	//			<< filename << std::endl;
	//	//	}

	//	//	success = mNegativeCritic->load(negCriticNode, 
	//	//		defaults::criticMembraneTimeConstant, 
	//	//		defaults::criticLearningRatePercent, 
	//	//		defaults::criticETraceDecayPercent, 0);
	//	//	if (!success)
	//	//	{
	//	//		VERVE_LOGGER("warning") << 
	//	//			"verve::Agent::load: Could not load file " 
	//	//			<< filename << std::endl;
	//	//	}
	//	//}
	//}

	//void Agent::save(const std::string& filename)
	//{
	//	// Check if we need to auto generate a unique filename.
	//	std::string nameStr = filename;
	//	if (nameStr.empty())
	//	{
	//		static unsigned int count = 0;
	//		char newName[64];
	//		sprintf(newName, "agent%d_age-%dh.xml", count, (int)mAgeHours);
	//		nameStr = newName;
	//		++count;
	//	}

	//	TiXmlDocument file(nameStr.c_str());
	//
	//	// Add the XML declaration.
	//	TiXmlDeclaration declaration("1.0", "", "yes");
	//	file.InsertEndChild(declaration);
	//
	//	// Create the root element.
	//	TiXmlElement rootElement("VerveAgent");

	//	// Set the Agent's age attributes.
	//	rootElement.SetAttribute("ageHours", (int)mAgeHours);
	//	rootElement.SetAttribute("ageMinutes", (int)mAgeMinutes);
	//	rootElement.SetDoubleAttribute("ageSeconds", mAgeSeconds);

	//	// Update: there are no longer different types of Agents.
	//	//// Set the Agent's type attribute.
	//	//rootElement.SetAttribute("type", "RL");

	//	//// Create the actor element and add it to the root element.
	//	//TiXmlElement actorElement("Actor");
	//	//if (mActor->save(&actorElement))
	//	//{
	//	//	rootElement.InsertEndChild(actorElement);
	//	//}
	//	//else
	//	//{
	//	//	VERVE_LOGGER("warning") << 
	//	//		"verve::Agent::save: Could not save file " 
	//	//		<< nameStr << std::endl;
	//	//	return;
	//	//}

	//	//// Create the positive critic element and add it to the root element.
	//	//TiXmlElement posCriticElement("PositiveCritic");
	//	//if (mPositiveCritic->save(&posCriticElement))
	//	//{
	//	//	rootElement.InsertEndChild(posCriticElement);
	//	//}
	//	//else
	//	//{
	//	//	VERVE_LOGGER("warning") << 
	//	//		"verve::Agent::save: Could not save file " 
	//	//		<< nameStr << std::endl;
	//	//	return;
	//	//}

	//	//// Create the negative critic element and add it to the root element.
	//	//TiXmlElement negCriticElement("NegativeCritic");
	//	//if (mNegativeCritic->save(&negCriticElement))
	//	//{
	//	//	rootElement.InsertEndChild(negCriticElement);
	//	//}
	//	//else
	//	//{
	//	//	VERVE_LOGGER("warning") << 
	//	//		"verve::Agent::save: Could not save file " 
	//	//		<< nameStr << std::endl;
	//	//	return;
	//	//}

	//	// Fill the root element.
	//	if (!mBrain->save(&rootElement))
	//	{
	//		VERVE_LOGGER("warning") << 
	//			"verve::Agent::save: Could not save file " 
	//			<< nameStr << std::endl;
	//		return;
	//	}

	//	// Add the root element to the file.
	//	file.InsertEndChild(rootElement);
	//
	//	// Now save the document to a file.
	//	if (false == file.SaveFile())
	//	{
	//		VERVE_LOGGER("warning") << 
	//			"verve::Agent::save: Failed to save XML file " 
	//			<< nameStr << "." << std::endl;
	//	}
	//}

	unsigned int Agent::getNumDiscreteSensors()const
	{
		return mDescriptor.getNumDiscreteSensors();
	}

	unsigned int Agent::getNumContinuousSensors()const
	{
		return mDescriptor.getNumContinuousSensors();
	}

	void Agent::setStepSize(real value)
	{
		mStepSize = value;

		// These components must be notified of the step size change.
		mRLModule->changeStepSize(value);
		if (mPredictiveModel)
		{
			mPredictiveModel->changeStepSize(value);
		}
	}

	void Agent::setETraceTimeConstant(real timeConstant)
	{
		mRLModule->setETraceTimeConstant(timeConstant, mStepSize);
	}

	void Agent::setTDDiscountTimeConstant(real timeConstant)
	{
		mRLModule->setTDDiscountTimeConstant(timeConstant, mStepSize);
	}

	void Agent::setTDLearningRate(real valueFunctionTimeConstant, 
		real policyLearningMultiplier)
	{
		mRLModule->setTDLearningRate(valueFunctionTimeConstant, 
			policyLearningMultiplier, mStepSize);
	}

	void Agent::setModelLearningRate(real timeConstant)
	{
		if (mPredictiveModel)
		{
			mPredictiveModel->setDeltaLearningRate(timeConstant, mStepSize);
		}
	}

	void Agent::setLearningEnabled(bool enabled)
	{
		mLearningEnabled = enabled;
	}

	real Agent::computeValueEstimation(const Observation& obs)
	{
		return mRLModule->computeValueEstimation(obs);
	}

	const AgentDescriptor* Agent::getDescriptor()const
	{
		return &mDescriptor;
	}

	long unsigned int Agent::getAge()const
	{
		return (3600 * mAgeHours + 60 * mAgeMinutes + 
			(long unsigned int)mAgeSeconds);
	}

	std::string Agent::getAgeString()const
	{
		char temp[32];
		sprintf(temp, "%dh, %dm, %.2fs", mAgeHours, mAgeMinutes, mAgeSeconds);
		return std::string(temp);
	}

	//real Agent::getTDError()const
	//{
	//	return mRLModule->getTDError();
	//}

	//real Agent::getModelMSE()const
	//{
	//	if (mPredictiveModel)
	//	{
	//		return mPredictiveModel->getPredictionMSE();
	//	}
	//	else
	//	{
	//		return 0;
	//	}
	//}

	//unsigned int Agent::getLastPlanLength()const
	//{
	//	return mLastPlanningSequenceLength;
	//}

	void Agent::saveValueData(unsigned int continuousResolution, 
		const std::string& filename)
	{
		mRLModule->saveValueData(continuousResolution, filename);
	}

	void Agent::saveStateRBFData(const std::string& filename)
	{
		mRLModule->saveStateRBFData(filename);
	}

	void Agent::incrementAge()
	{
		mAgeSeconds += mStepSize;
		if (mAgeSeconds >= 60)
		{
			mAgeMinutes += 1;
			mAgeSeconds -= 60;

			if (60 == mAgeMinutes)
			{
				mAgeHours += 1;
				mAgeMinutes = 0;
			}
		}
	}

	const AgentDebugData& Agent::getDebugData()const
	{
		return mDebugData;
	}
}
