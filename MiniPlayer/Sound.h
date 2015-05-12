#ifndef SOUND_H_INCLUDED
#define SOUND_H_INCLUDED

#include <windows.h>
#include <conio.h>
#include <cstdio>
#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>

using namespace std;

#include <fmod.h>
#include <fmod_errors.h>


bool exists(string a)
{
    return ifstream(a.c_str()).good();
}

string format(string nfilename)
{
    return nfilename.substr(nfilename.find_last_of('.'),nfilename.size()-nfilename.find_last_of('.'));
}


using namespace std;

FMOD_SYSTEM            *sys;
FMOD_SOUND      *sound=NULL;
FMOD_CHANNEL  *channel=NULL;
FMOD_RESULT          result;

void ERRCHK(FMOD_RESULT result)
{
    if (result != FMOD_OK)
    {
        cout<<"FMOD error!\n"<<result<<": "<<FMOD_ErrorString(result);
        exit(-1);
    }
}

void sys_init()
{
    unsigned int ver;
    result=FMOD_System_Create(&sys);
    ERRCHK(result);
    result=FMOD_System_GetVersion(sys,&ver);
    ERRCHK(result);
    if(ver<FMOD_VERSION)
    {
        cout<<"Error! You are using an old version of FMOD. This program requires: \n"<<FMOD_VERSION<<"\n";
        exit(-2);
    }
    result = FMOD_System_Init(sys, 2, FMOD_INIT_NORMAL, 0);
    ERRCHK(result);
    cout<<"System initialized successfully;\n";
}

void load(string f)
{
        result = FMOD_System_CreateStream(sys,f.c_str(), FMOD_SOFTWARE, NULL, &sound);
        ERRCHK(result);
        cout<<"File loaded successfully;\n";
}

void free_sys()
{
    result=FMOD_Sound_Release(sound);
    ERRCHK(result);
    result=FMOD_System_Close(sys);
    ERRCHK(result);
    result=FMOD_System_Release(sys);
    ERRCHK(result);
}

void play_sound(FMOD_CHANNELINDEX indx)
{
    result=FMOD_System_PlaySound(sys,indx,sound,0,&channel);
    ERRCHK(result);
    cout<<"Sound playing succesfully!\n";
}

string get_name()
{
    FMOD_TAG buff;
    if(sound)
    {
        result=FMOD_Sound_GetTag(sound,0,0,&buff);
        ERRCHK(result);
        return string((char*)(buff.data));
    }
    return "";
}


#endif // SOUND_H_INCLUDED
