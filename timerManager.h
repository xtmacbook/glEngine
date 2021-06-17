//
//  TimerManager.h
//  OpenGL
//
//  Created by xt on 15/8/10.
//  Copyright (c) 2015年 xt. All rights reserved.
//

#ifndef __OpenGL__TimerManager__
#define __OpenGL__TimerManager__

#include "decl.h"
#include <stdio.h>

class LIBENIGHT_EXPORT TimeManager
{
public:

	// The function to get the instance of the manager, or initialize and return the instance.
	// By creating a static variable of the instance we ensure this only gets created once.
	// This is also thread-safe in C++ 11 according to the specifications.
	static TimeManager& instance()
	{
		static TimeManager instance;

		return instance;
	}

	// This calculates our current scene's frames per second and displays it in the console
	double					calculateFrameRate(bool writeToConsole);

	// This returns the current time in seconds (since 1/1/1970, call "epoch")
	double					getTime();

	double					getMicroTime();
	// This pauses the current thread for an amount of time in milliseconds
	void					sleepSec(int milliseconds);

	inline double			getDeltaTime() { return deltaTime_; }

	void					initTime();

	double					getApplicationDeltaTime();
	int					   getFrameRate()const;

private:

	// This is the time slice that stores the total time in seconds that has elapsed since the last frame
	double					deltaTime_;

	// This is the current time in seconds
	double					currentTime_;

	double					timeOfStartApplicaton_;

	int						framesPerSecond_;

private:
	TimeManager();								// Private so that it can  not be called
	TimeManager(TimeManager const&);				// copy constructor is private
	TimeManager& operator=(TimeManager const&);		// assignment operator is private
};

#define CURRENT_TIME  return TimeManager::instance().getTime();

#endif /* defined(__OpenGL__TimerManager__) */
