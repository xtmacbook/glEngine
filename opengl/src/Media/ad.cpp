//
//  audo.cpp
//  openGLTest
//
//  Created by xt on 17/8/26.
//  Copyright © 2017年 xt. All rights reserved.
//

#include "ad.h"
#ifdef FMOD_HAVE
#include "fmod.hpp"
#endif

#include "resource.h"

Audo::Audo()
{
}

bool Audo::initAudo()
{
    return false;
}

bool Audo::update()
{
    return true;
}

bool Audo::shutDown()
{
    return true;
}

bool Audo::play()
{
    return  true;
}

bool Audo::stop()
{
    return true;
}

class FMAudo :public Audo
{
public:
    FMAudo(){}

    virtual bool initAudo()
    {
#ifdef FMOD_HAVE
        FMOD_RESULT result;
        result = FMOD::System_Create(&system_);
        result = system_->getVersion(&version);
        if(version < FMOD_VERSION)
        {
            printf("FMOD version error\n");
            return  false;
        }

    result = system_->init(32, FMOD_INIT_NORMAL, NULL);

     if(result != FMOD_OK)
     {
        printf("FMOD init error\n");

        return false;
     }
    result = system_->createSound(std::string( get_media_BasePath() + "music/wave.mp3").c_str(), FMOD_DEFAULT, 0, &sound_);

     if(result != FMOD_OK)
     {
        printf("FMOD createSound error\n");

        return false;
     }

    result = sound_->setMode(FMOD_LOOP_OFF);    /* drumloop.wav has embedded loop points which automatically makes looping turn on, */


        #endif
        return  true;
    }

    virtual bool shutDown()
    {
#ifdef FMOD_HAVE
        system_->release();
#endif
        return  true;
    }

    virtual bool update()
    {
#ifdef FMOD_HAVE
        system_->update();
#endif
		return true;
    }

    virtual bool play()
    {
#ifdef FMOD_HAVE
        if(!channel_)
        {
            FMOD_RESULT result ;
            result = system_->playSound(sound_, 0, false, &channel_);
            if(result != FMOD_OK)
            {
                printf("Sound play error!\n");
            }
        }
        else
        {

        }
#endif
		return true;

    }

    virtual bool stop()
    {
#ifdef FMOD_HAVE
        bool         playing = 0;
        FMOD_RESULT result ;
        result = channel_->isPlaying(&playing);
        if(playing)
        {
            result = system_->playSound(sound_, 0, true, &channel_);
        }
#endif
		return true;

    }

private:
#ifdef FMOD_HAVE
    FMOD::System* system_;
    FMOD::Sound * sound_;
    FMOD::Channel * channel_;

    unsigned int version;
#endif
};

Audo *   g_Audo;

extern bool initAudo()
{
    g_Audo = new FMAudo();
#ifdef FMOD_HAVE
    return  g_Audo->initAudo();
#endif
    return  false;
}
extern void updateAudo()
{
#ifdef FMOD_HAVE
    if(g_Audo)
        g_Audo->update();
#endif
}

extern void playMusic()
{
#ifdef FMOD_HAVE
    if(g_Audo)
        g_Audo->play();
#endif
}

extern void stop()
{
#ifdef FMOD_HAVE
    if(g_Audo)
        g_Audo->stop();
#endif
}
