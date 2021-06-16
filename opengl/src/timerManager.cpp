//
//  TimerManager.cpp
//  OpenGL
//
//  Created by xt on 15/8/10.
//  Copyright (c) 2015年 xt. All rights reserved.
//

#include "sys.h"

#if defined EN_OS_DARWIN
	#include <unistd.h>
#elif EN_OS_WIN32
	#include <windows.h>
#endif
#include <chrono>
#include<thread>
#include "timerManager.h"


// This calculates our current scene's frames per second and displays it in the console
double TimeManager::calculateFrameRate(bool writeToConsole = false)
{
	// Below we create a bunch of static variables to track the deltas of the time.
	static double framesPerSecond = 0.0f;								// This will store our fps
	static double startTime = getTime();								// This will hold the time per second to test against
	static double lastTime = getTime();									// This will hold the time from the last frame
	static char   strFrameRate[50] = { 0 };								// We will store the string here for the window title
	static double currentFPS = 0.0f;									// This stores the current frames per second

	// Grab the current time in seconds from our environment's time function wrapped in our WindowManager
	currentTime_ = getTime();

	// Calculate our delta time, which is the time that has elapsed since the last time we checked the time
	deltaTime_ = currentTime_ - lastTime;

	// Now store the lastTime as the currentTime to then get the time passed since the last frame
	lastTime = currentTime_;

	// Increase the frame counter
	++framesPerSecond;

	// if a second has passed we can get the current frame rate
	if (currentTime_ - startTime > 1.0f)
	{
		// Here we set the startTime to the currentTime.  This will be used as the starting point for the next second.
		// This is because GetTime() counts up, so we need to create a delta that subtract the current time from.
		startTime = currentTime_;

		// Show the frames per second in the console window if desired
		framesPerSecond_ = int(framesPerSecond);


		// Store the current FPS since we reset it down below and need to store it to return it
		currentFPS = framesPerSecond;

		// Reset the frames per second
		framesPerSecond = 0;
	}

	// Return the most recent FPS
	return currentFPS;
}


double TimeManager::getMicroTime()
{
	// Grab the current system time since 1/1/1970, otherwise know as the Unix Timestamp or Epoch
	auto beginningOfTime = std::chrono::system_clock::now().time_since_epoch();

	// Convert the system time to milliseconds
	auto ms = std::chrono::duration_cast<std::chrono::microseconds>(beginningOfTime).count();

	return ms ;
}

// This returns the current time in seconds (uses C++ 11 system_clock)
double TimeManager::getTime()
{
	// Grab the current system time since 1/1/1970, otherwise know as the Unix Timestamp or Epoch
	auto beginningOfTime = std::chrono::system_clock::now().time_since_epoch();

	// Convert the system time to milliseconds
	auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(beginningOfTime).count();

	// Return the time in seconds and give us a fractional value (important!)
	return ms * 0.001;
 
}


// This pauses the current thread for an amount of time in milliseconds
void TimeManager::sleepSec(int ms)
{
	// Use the C++ 11 sleep_for() function to pause the current thread
   // Sleep(ms);
	std::this_thread::sleep_for(std::chrono::milliseconds(1000)  * ms);
}


void TimeManager::initTime()
{
	timeOfStartApplicaton_ = getTime();
}

double TimeManager::getApplicationDeltaTime()
{
	return getTime() - timeOfStartApplicaton_;
}

int TimeManager::getFrameRate() const
{
	return framesPerSecond_;
}

TimeManager::TimeManager()
{
	deltaTime_ = 0.0;
	currentTime_ = 0.0;
}
