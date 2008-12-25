/*
 * Created by Waipot Ngamsaad (ngamsaad.waipot@gmail.com)
 * Dated 20-12-2008

 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _Singleton_h_
#define _Singleton_h_

#include <iostream>

namespace OPAL_OSG { 
	
	
	template <typename T> class Singleton 
	{
	public:
		/**
		* Constructor
		*/
		Singleton() {
			ms_Singleton = static_cast< T* >( this );
		}
	
		/**
		* Destructor
		*/
     	~Singleton() {  
			assert( ms_Singleton );  
			ms_Singleton = 0;
	 	}
        
		/**
		* This method just returns the internal member by 
		* reference
		* @return T& - reference to internal abstract Type
		*/
		static T& getInstance() {	
    		if (!ms_Singleton)
				ms_Singleton = new T();
		
			return *ms_Singleton; 
		}
        
		/**
		* This method just returns the internal member by 
		* a pointer
		* @return T* - pointer to the internal abstract Type
		*/
		static T* getInstancePtr() { 
    		if (!ms_Singleton) 
				ms_Singleton = new T();

			return ms_Singleton; 
		}
	
	protected:
		/** The static member object */
		static T* ms_Singleton;
	};
	
}
#endif
