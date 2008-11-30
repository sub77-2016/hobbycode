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

#include "Logger.h"
#include <fstream>

namespace opal
{
	namespace loggerImpl
	{
		Logger::Logger()
		{
			//static std::ofstream debugOut, warningOut, errorOut;
			//debugOut.open("opal_debug.log");
			//warningOut.open("opal_warning.log");
			//errorOut.open("opal_error.log");

			//setStream("debug", &debugOut, "[OPAL debug] ");
			//setStream("warning", &warningOut, "[OPAL warning] ");
			//setStream("error", &errorOut, "[OPAL error] ");
			//setStream("performance", &std::cout);

			setStream("debug", &std::cout, "[OPAL debug] ");
			setStream("warning", &std::cout, "[OPAL warning] ");
			setStream("error", &std::cout, "[OPAL error] ");
		}

		Logger::~Logger()
		{
		}

		void Logger::setStream(const std::string &name, std::ostream *stream, 
			const std::string& prefix, char mark)
		{
			Logger::Stream s;
			s.mark = mark;
			s.silent = false;
			s.stream = stream;
			s.prefix = prefix;
	//		s.nullStream = new std::ostream(s.nullBuffer);
			mStreams[name] = s;
		}

		//void Logger::silenceStream(const std::string &name)
		//{
		//	std::map<std::string, Logger::Stream>::iterator itr = 
		//		mStreams.find(name);
		//	if(itr != mStreams.end())
		//	{
		//		itr->second.silent = true;
		//	}
		//}

		//void Logger::unsilenceStream(const std::string &name)
		//{
		//	std::map<std::string, Logger::Stream>::iterator itr = 
		//		mStreams.find(name);		
		//	if(itr != mStreams.end())
		//	{
		//		itr->second.silent = false;
		//	}
		//}

		std::ostream &Logger::stream(const std::string &name)
		{
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
	}
}
