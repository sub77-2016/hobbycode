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

#ifndef VERVE_LOGGER_H
#define VERVE_LOGGER_H

#include <iomanip>
#include <stdlib.h>
#include <string>
#include <ostream>
#include <iostream>
#include <map>
#include <iosfwd>
#include <stdexcept>

/// Use this macro to access the Logger singleton.  For example, 
/// VERVE_LOGGER("debug") << errorMsg << std::endl;  When called 
/// on an new named stream, this automatically sets it up to 
/// use the standard output.
#define VERVE_LOGGER verve::Logger::instance().getStream

/// Use this macro to setup a specific named stream to use a 
/// particular output method.  For example, 
/// VERVE_LOGGER_SETUP_STREAM("warning", &myFileStream); would 
/// set the "warning" stream to use the given file for output.  
#define VERVE_LOGGER_SETUP_STREAM verve::Logger::instance().createStream

namespace verve
{
	///	This class is used to log any events, errors, warnings, etc. 
	/// that may come up.  Calls to this class should replace all 
	/// output statements.  To use this class, see the VERVE_LOGGER 
	/// macros.  We use a singleton here so we never have to initialize 
	/// the Logger explicitly.
	class Logger
	{
	public:
		/// Returns a reference to the singleton.  
		static Logger& VERVE_CALL instance()
		{
			static Logger mSelf;
			return mSelf;
		}

		/// Returns the named stream.  Use the VERVE_LOGGER macro instead 
		/// of calling this directly.
		std::ostream& VERVE_CALL getStream(const std::string& name)
		{
			if (mStreams.find(name) == mStreams.end())
			{
				// If the named stream does not yet exist, create it.
				createStream(name, &std::cout);
			}

			Logger::Stream s = mStreams[name];

			if(s.mark != '\0')
			{
				std::cout << s.mark;
				std::cout.flush();
			}

			if (!s.prefix.empty())
			{
				*s.stream << s.prefix;
			}

			return *(s.stream);
		}

		/// Register a new stream for later use.  Use the 
		/// VERVE_LOGGER_CREATE_STREAM macro instead of calling this 
		/// directly.
		void createStream(const std::string& name, std::ostream *stream, 
			char mark='\0')
		{
			Logger::Stream s;
			s.mark = mark;
			s.stream = stream;
			s.prefix = "[Verve " + name + "] ";
			mStreams[name] = s;
		}

	private:
		Logger()
		{
		}

		~Logger()
		{
			mStreams.clear();
		}

		/// A simple output stream structure.
		struct Stream
		{
			std::ostream *nullStream;
			std::ostream *stream;
			char mark;
			std::string prefix;
		};

		/// Internal map of named output streams.
		std::map<std::string, Stream> mStreams;
	};
}

#endif
