
#ifndef _Singleton_h_
#define _Singleton_h_

#include <iostream>

template <typename T> class Singleton {

    protected:
	/** The static member object */
        static T* ms_Singleton;

    	/**
     	* Constructor - this is protected as this class uses
     	* the singleton design pattern.
     	*/


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
		//assert( ms_Singleton );  
		ms_Singleton = 0;  
	}
        
	/**
	* This method just returns the internal member by 
	* reference
	* @return T& - reference to internal abstract Type
	*/
	static T& getInstance() {	
    		if (!ms_Singleton) {
			ms_Singleton = new T();
    		}

		return *ms_Singleton; 
	}
        
	/**
	* This method just returns the internal member by 
	* a pointer
	* @return T* - pointer to the internal abstract Type
	*/
	static T* getInstancePtr() { 
    		if (!ms_Singleton) {
			ms_Singleton = new T();
    		}

		return ms_Singleton; 
	}
};
#endif
