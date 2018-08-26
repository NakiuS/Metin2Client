#pragma once

template <typename T> 
class TAbstractSingleton
{ 
	static T * ms_singleton;
	
public: 
	TAbstractSingleton()
	{ 
		assert(!ms_singleton);
		int offset = (int) (T*) 1 - (int) (CSingleton <T>*) (T*) 1; 
		ms_singleton = (T*) ((int) this + offset);
	} 

	virtual ~TAbstractSingleton()
	{ 
		assert(ms_singleton);
		ms_singleton = 0; 
	}

	__forceinline static T & GetSingleton()
	{
		assert(ms_singleton!=NULL);
		return (*ms_singleton);
	}	
};

template <typename T> T * TAbstractSingleton <T>::ms_singleton = 0;