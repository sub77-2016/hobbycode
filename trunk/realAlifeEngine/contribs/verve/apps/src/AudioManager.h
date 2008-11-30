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

#ifndef AUDIO_MANAGER_H
#define AUDIO_MANAGER_H

#include <iostream>
#include <string>
#include <map>

#include <audiere.h>

/// Use this macro to access the singleton.  For example: 
/// AUDIO.loadSound("woosh", "data/sounds/woosh.wav");
#define AUDIO AudioManager::instance()

/// A simple singleton class to manage sound effects and music.  Uses 
/// the Audiere library (http://audiere.sourceforge.net).  Use the 
/// macro above to make it easier to access the singleton.  To use this 
/// class, first call the load* functions to bring the sound files 
/// into memory.  Then use the get* functions to access the sounds 
/// by name.  There are 3 types of sounds: 1) normal sounds, which 
/// have more options but cannot be played multiple times 
/// simultaneously but cannot loop, 2) multi sounds, which are more 
/// limited but can be played multiple times simultaneously, and 
/// 3) music, which is streamed from the disk.
class AudioManager : public audiere::StopCallback
{
public:
	/// Returns a reference to the singleton.  
	static AudioManager& instance()
	{
		static AudioManager mSelf;
		return mSelf;
	}

	/// Loads the given sound into memory and associates it with the 
	/// given name.  Does nothing if the given name is already being 
	/// used.
	void loadSound(const std::string& name, const std::string& filename);

	/// Loads the given multi sound into memory and associates it with 
	/// the given name.  Does nothing if the given name is already being 
	/// used.
	void loadMultiSound(const std::string& name, const std::string& filename);

	/// Loads the given song associates it with the given name.  This 
	/// doesn't actually load the song into memory - music is streamed 
	/// as it plays.  Does nothing if the given name is already being 
	/// used.
	void loadSong(const std::string& name, const std::string& filename);

	/// Returns a pointer to the named sound, set to the default sound 
	/// volume.
	audiere::OutputStreamPtr getSound(const std::string& name);

	/// Returns a pointer to the named multi sound, set to the 
	/// default sound volume.
	audiere::SoundEffectPtr getMultiSound(const std::string& name);

	/// Starts playing music.  Plays the given song unless it 
	/// is an empty string, in which case it will start with a random 
	/// song.  If continuous music is enabled, it will play a new 
	/// random song when the current song finishes.  Note: to get a 
	/// new random playlist every time, you must seed the random number 
	/// generator yourself.
	void startMusic(const std::string& initialSongName, bool continuous);

	/// Returns whether music is currently playing.
	bool isMusicPlaying()const;

	/// Stops the music from playing.
	void stopMusic();

	/// Sets the default initial volume level for normal and multi sounds.  
	/// This must be set before sounds are loaded to have any effect.  
	/// (Individual sounds' volumes can be set manually later.)  This 
	/// must be within [0.0, 1.0].
	void setDefaultInitialSoundVolume(float volume);

	/// Returns the default volume level for normal and multi sounds.  This 
	/// is always within [0.0, 1.0].
	float getDefaultInitialSoundVolume()const;

	/// Sets the music volume, which must be within [0.0, 1.0].
	void setMusicVolume(float volume);

	/// Returns the music volume, which is always within [0.0, 1.0].
	float getMusicVolume()const;

	/// This gets called by when a music stream has stopped.
	ADR_METHOD(void) streamStopped(audiere::StopEvent* event);

	/// Required virtual function implementation from the abstract 
	/// audiere::RefCounted class.
	ADR_METHOD(void) ref();

	/// Required virtual function implementation from the abstract 
	/// audiere::RefCounted class.
	ADR_METHOD(void) unref();

private:
	AudioManager();

	~AudioManager();

	/// Returns a pointer to the named song.
	audiere::OutputStreamPtr getSong(const std::string& name);

	/// Returns a random integer from [min, max] using a uniform probability 
	/// distribution.
	int randomIntUniform(int min, int max);

	/// Chooses a random song name from the current list of songs.
	std::string chooseRandomSongName();

	/// Prints an error message to standard output.
	void printError(const std::string& message);

	/// The audio output device for normal and multi sounds.
	audiere::AudioDevicePtr mSoundDevice;

	/// A map of named sounds.
	std::map<std::string, audiere::OutputStreamPtr> mSoundMap;

	/// A map of named multi sounds.
	std::map<std::string, audiere::SoundEffectPtr> mMultiSoundMap;

	/// A map of named songs.
	std::map<std::string, audiere::OutputStreamPtr> mSongMap;

	/// A pointer to the current song, if music is playing.
	audiere::OutputStreamPtr mCurrentSong;

	/// Determines whether songs will keep playing once the current 
	/// one finishes.
	bool mContinuousMusicEnabled;

	/// Determines the initial volume level for normal and multi sounds.
	float mDefaultInitialSoundVolume;

	/// Determines the music volume.
	float mMusicVolume;
};

#endif
