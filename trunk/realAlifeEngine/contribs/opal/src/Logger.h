/************************************************************************
* Open Physics Abstraction Layer                                        *
* http://opal.sourceforge.net                                           *
* Copyright (C) 2004-2008                                               *
* Alan Fischer (http://www.lightningtoads.com)                          *
* Andres Reinot (http://www.reinot.com)                                 *
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

#ifndef OPAL_LOGGER_H
#define OPAL_LOGGER_H

// Put this here for now.  They seem to nail all the really annoying and
// useless VC6 warning: 4786, Debug info too long
#if defined(WIN32) || defined(_WIN32)
	#pragma warning(disable:4786)
#endif

#include <iomanip>
#include <stdlib.h>
#include <string>
#include <ostream>
#include <iostream>
#include <map>
#include <iosfwd>
#include <stdexcept>

#include "Defines.h"
#include "Singleton.h"

#define OPAL_LOGGER Logger::instance().stream

namespace opal
{
	/// A namespace for the Logger singleton implementation.
	namespace loggerImpl
	{
		///	This class is used to log any events, errors, or warnings that may 
		/// come up.  Calls to this class should replace all cout << statements 
		/// in the form: Logger::stream("myStreamName") << ...
		///	where myStreamName was registered as a cout stream or file stream 
		/// using the setStream function.
		class Logger
		{
		public:
			Logger();

			~Logger();

			/// Register a new stream for later use.
			void OPAL_CALL setStream(const std::string& name, 
				std::ostream *stream, const std::string& prefix="", 
				char mark='\0');

			//static void silenceStream(const std::string& name);

			//static void unsilenceStream(const std::string& name);

			/// Returns the named stream.
			std::ostream& OPAL_CALL stream(const std::string& name);

		private:
			struct Stream
			{
				std::ostream *nullStream;
				std::ostream *stream;
				bool silent;
				char mark;
				std::string prefix;
			};

			void init();
			std::map<std::string, Stream> mStreams;
		};
	}
	typedef Singleton<loggerImpl::Logger> Logger;
}

#endif
