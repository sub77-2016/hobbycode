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

#include "Globals.h"
#include "external/tinyxml/tinyxml.h"

namespace verve
{
	namespace globals
	{
		real VERVE_CALL getAttributeReal(const TiXmlNode* nodePtr, 
			const std::string& name)
		{
			TiXmlElement* elementPtr = 
				const_cast<TiXmlNode*>(nodePtr)->ToElement();
			double temp = 0;

			if (NULL == elementPtr->Attribute(name.c_str(), &temp))
			{
				VERVE_LOGGER("warning") << 
					"verve::globals::getAttributeReal: Element " 
					<< nodePtr->Value() << " is missing attribute '" 
					<< name << "'.  Parameter will be set to '0.0'." 
					<< std::endl;
				return 0;
			}
			else
			{
				return (real)temp;
			}
		}

		int VERVE_CALL getAttributeInt(const TiXmlNode* nodePtr, 
			const std::string& name)
		{
			TiXmlElement* elementPtr = 
				const_cast<TiXmlNode*>(nodePtr)->ToElement();
			int temp = 0;

			if (NULL == elementPtr->Attribute(name.c_str(), &temp))
			{
				VERVE_LOGGER("warning") << 
					"verve::globals::getAttributeInt: Element " 
					<< nodePtr->Value() << " is missing attribute '" 
					<< name << "'.  Parameter will be set to '0'." 
					<< std::endl;
				return 0;
			}
			else
			{
				return temp;
			}
		}

		bool VERVE_CALL getAttributeBool(const TiXmlNode* nodePtr, 
			const std::string& name)
		{
			TiXmlElement* elementPtr = 
				const_cast<TiXmlNode*>(nodePtr)->ToElement();
			const char* temp = elementPtr->Attribute(name.c_str());

			if (NULL == temp)
			{
				VERVE_LOGGER("warning") << 
					"verve::globals::getAttributeBool: Element " 
					<< nodePtr->Value() << " is missing attribute '" 
					<< name << "'.  Parameter will be set to 'false'." 
					<< std::endl;
				return false;
			}
			else
			{
				std::string value = temp;
				if ("true" == value || "1" == value)
				{
					return true;
				}
				else if ("false" == value || "0" == value)
				{
					return false;
				}
				else
				{
					VERVE_LOGGER("warning") << 
						"verve::globals::getAttributeBool: Element " 
						<< nodePtr->Value() << " attribute '" << name 
						<< "' has an invalid value.  Possible " 
						<< "values are 'true', 'false', '1', and '0'.  "
						<< "Parameter will be set to 'false'." << std::endl;
					return false;
				}
			}
		}

		std::string VERVE_CALL getAttributeString(const TiXmlNode* nodePtr, 
			const std::string& name)
		{
			TiXmlElement* elementPtr = 
				const_cast<TiXmlNode*>(nodePtr)->ToElement();
			const char* temp = elementPtr->Attribute(name.c_str());

			if (NULL == temp)
			{
				VERVE_LOGGER("warning") << 
					"verve::globals::getAttributeString: Element " 
					<< nodePtr->Value() << " is missing attribute '" 
					<< name << "'.  Parameter will be set to \"\"." 
					<< std::endl;
				return "";
			}
			else
			{
				return temp;
			}
		}
	}
}
