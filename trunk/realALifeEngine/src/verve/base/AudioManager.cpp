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

#include "AudioManager.h"
#include <assert.h>

AudioManager::AudioManager()
{
#if defined(WIN32) || defined(_WIN32)
	const char* deviceName = NULL;
#else
	const char* deviceName = NULL;
#endif

	mSoundDevice = audiere::OpenDevice(deviceName);
	if (!mSoundDevice)
	{
		printError("Failed to open Audiere device");
	}
	else
	{
		std::cout << "[AudioManager] Opened Audiere " 
			<< "device '" << mSoundDevice->getName() 
			<< "'" << std::endl;
	}

	// Setup the callback for when streams finish.
	mSoundDevice->registerCallback(this);

	mCurrentSong = NULL;
	mContinuousMusicEnabled = true;
	mDefaultInitialSoundVolume = 0.5f;
	mMusicVolume = 0.5f;
}

AudioManager::~AudioManager()
{
	mSoundMap.clear();
	mMultiSoundMap.clear();
	mSongMap.clear();
}

void AudioManager::loadSound(const std::string& name, 
	const std::string& filename)
{
	audiere::OutputStreamPtr s;
	s = audiere::OpenSound(mSoundDevice, filename.c_str(), false);

	if (!s)
	{
		printError("Failed to load sound file '" + filename + "'");
		return;
	}

	if (mSoundMap.find(name) != mSoundMap.end())
	{
		//printError("The sound '" + name + "' has already been loaded");
		return;
	}

	s->setVolume(mDefaultInitialSoundVolume);
	mSoundMap[name] = s;
}

void AudioManager::loadMultiSound(const std::string& name, 
	const std::string& filename)
{
	audiere::SoundEffectPtr s;
	s = audiere::OpenSoundEffect(mSoundDevice, filename.c_str(), 
		audiere::MULTIPLE);

	if (!s)
	{
		printError("Failed to load sound file '" + filename + "'");
		return;
	}

	if (mMultiSoundMap.find(name) != mMultiSoundMap.end())
	{
		//printError("The sound '" + name + "' has already been loaded");
		return;
	}

	s->setVolume(mDefaultInitialSoundVolume);
	mMultiSoundMap[name] = s;
}

void AudioManager::loadSong(const std::string& name, 
	const std::string& filename)
{
	audiere::OutputStreamPtr s;
	s = audiere::OpenSound(mSoundDevice, filename.c_str(), true);

	if (!s)
	{
		printError("Failed to load song file '" + filename + "'");
		return;
	}

	if (mSongMap.find(name) != mSongMap.end())
	{
		//printError("The song '" + name + "' has already been loaded");
		return;
	}

	mSongMap[name] = s;
}

audiere::OutputStreamPtr AudioManager::getSound(const std::string& name)
{
	std::map<std::string, audiere::OutputStreamPtr>::iterator iter = 
		mSoundMap.find(name);

	if (iter == mSoundMap.end())
	{
		printError("The sound file '" + name + "' has not been loaded");
		return NULL;
	}

	return (*iter).second;
}

audiere::SoundEffectPtr AudioManager::getMultiSound(const std::string& name)
{
	std::map<std::string, audiere::SoundEffectPtr>::iterator iter = 
		mMultiSoundMap.find(name);

	if (iter == mMultiSoundMap.end())
	{
		printError("The sound file '" + name + "' has not been loaded");
		return NULL;
	}

	return (*iter).second;
}

void AudioManager::startMusic(const std::string& songName, 
	bool continuous)
{
	mContinuousMusicEnabled = continuous;

	// Stop playing any other music first.
	stopMusic();

	if (!songName.empty())
	{
		mCurrentSong = getSong(songName);
	}
	else
	{
		mCurrentSong = getSong(chooseRandomSongName());
	}

	if (!mCurrentSong)
	{
		return;
	}

	mCurrentSong->setVolume(mMusicVolume);
	mCurrentSong->play();
}

bool AudioManager::isMusicPlaying()const
{
	if (mCurrentSong && mCurrentSong->isPlaying())
	{
		return true;
	}
	else
	{
		return false;
	}
}

void AudioManager::stopMusic()
{
	if (isMusicPlaying())
	{
		mCurrentSong->stop();
		mCurrentSong->setPosition(0);
	}

	mCurrentSong = NULL;
}

void AudioManager::setDefaultInitialSoundVolume(float volume)
{
	mDefaultInitialSoundVolume = volume;
}

float AudioManager::getDefaultInitialSoundVolume()const
{
	return mDefaultInitialSoundVolume;
}

void AudioManager::setMusicVolume(float volume)
{
	mMusicVolume = volume;

	if (mCurrentSong)
	{
		mCurrentSong->setVolume(volume);
	}
}

float AudioManager::getMusicVolume()const
{
	return mMusicVolume;
}

void AudioManager::streamStopped(audiere::StopEvent* event)
{
	// This is setup to handle stop events for music streams.
	if (mCurrentSong && !mCurrentSong->isPlaying() && mContinuousMusicEnabled 
		&& event->getReason() == audiere::StopEvent::STREAM_ENDED)
	{
		mCurrentSong = getSong(chooseRandomSongName());

		if (!mCurrentSong)
		{
			return;
		}

		mCurrentSong->setVolume(mMusicVolume);
		mCurrentSong->play();
	}
}

void AudioManager::ref()
{
}

void AudioManager::unref()
{
}

audiere::OutputStreamPtr AudioManager::getSong(const std::string& name)
{
	std::map<std::string, audiere::OutputStreamPtr>::iterator iter = 
		mSongMap.find(name);

	if (iter == mSongMap.end())
	{
		printError("The song '" + name + "' has not been loaded");
		return NULL;
	}

	return (*iter).second;
}

int AudioManager::randomIntUniform(int min, int max)
{
	// Note: rand() isn't a very good generator, but it's good enough for 
	// simple tasks that only require a few thousand values.

	int range = max - min;
	int randValue = rand();
	int randMaxValue = RAND_MAX;
	while (randMaxValue < range)
	{
		// Increase the random value range until we reach the desired 
		// range.
		randValue = randValue * RAND_MAX;
		randMaxValue = randMaxValue * RAND_MAX;
		if (randValue < randMaxValue)
		{
			randValue += rand();
		}
	}

	// Get a uniform random real value from [0, 1).
	double zeroToOneHalfOpen = randValue / (randMaxValue + 1.0);

	// The [0, 1) value times (max - min + 1) gives each integer within 
	// [0, max - min] an equal chance of being chosen.
	return min + int((range + 1) * zeroToOneHalfOpen);
}

std::string AudioManager::chooseRandomSongName()
{
	std::map<std::string, audiere::OutputStreamPtr>::iterator iter = 
		mSongMap.begin();
	int randInt = randomIntUniform(0, (int)(mSongMap.size() - 1));

	for (int i = 0; i < randInt; ++i)
	{
		++iter;

		if (mSongMap.end() == iter)
		{
			iter = mSongMap.begin();
		}
	}

	return (*iter).first;
}

void AudioManager::printError(const std::string& message)
{
	std::cout << "[AudioManager Error] " << message << std::endl;
}
