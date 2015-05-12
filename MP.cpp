#include<windows.h>
#include<cmath>
#include<iostream>
#include<fstream>
#include<string>
using namespace std;

long double stof(string n) //Converts string to float, required for extracting tempo values and generating frequencies;
{
    int i=n.size()-1;
    long double r[2]= {0}; //One natural number part and fractional part;
    ///The following lines are required to be compiled this way in order to prevent execution errors*;
    if(n.find('.')!=string::npos)
    {
        for(; n[i]!='.'; i--)r[0]+=n[i]-'0',r[0]/=10; //Gets the part after the floating point, provided that it exists;
        for(i=0; n[i]!='.'; i++)r[1]+=n[i]-'0',r[1]*=10; //Gets the integer part multiplied by 10;
        return r[0]+r[1]/10;
    }
    ///*;
    for(i=0; i<n.size(); i++)r[1]+=n[i]-'0',r[1]*=10;
    return r[0]+r[1]/10;
}

typedef long double frequency; //Makes code more easily readble within the project context;
const frequency freq0=440.00,A4=freq0; //Base frequency( tone), also known as A4 or "Middle A";
const frequency a=pow(2.0,1.0/12.0); //Frequency modulator;

frequency freq(int n) //n - steps away from the base, the result is another frequency, present in the tone table;
{
    return freq0*pow(a,n); //Modulate frequency to a tone;
}

frequency stofr(string note) //Converts string such as 'A#0' or 'A4' to frequencies ( floating points);
//Convertion is based on the half steps between the given tone and the base;
//Used information:
///http://www.phy.mtu.edu/~suits/NoteFreqCalcs.html
///http://www.phy.mtu.edu/~suits/notefreqs.html
{
    if(note.size()==2)
    {
        if(note[0]=='A')return freq((note[1]-'4')*12);
        if(note[0]=='B')return freq((note[1]-'4')*12+2);
        if(note[0]=='C')return freq((note[1]-'4')*12-9);
        if(note[0]=='D')return freq((note[1]-'4')*12-7);
        if(note[0]=='E')return freq((note[1]-'4')*12-5);
        if(note[0]=='F')return freq((note[1]-'4')*12-4);
        if(note[0]=='G')return freq((note[1]-'4')*12-2);
    }
    if(note.size()==3)
    {
        if(note[0]=='A')
        {
            if(note[1]=='b')return freq((note[2]-'4')*12-1);
            if(note[1]=='#')return freq((note[2]-'4')*12+1);
        }
        if(note[0]=='B')return freq((note[2]-'4')*12+1);
        if(note[0]=='C')return freq((note[2]-'4')*12-8);
        if(note[0]=='D')
        {
            if(note[1]=='b')return freq((note[2]-'4')*12-8);
            if(note[1]=='#')return freq((note[2]-'4')*12-6);
        }
        if(note[0]=='E')return freq((note[2]-'4')*12-6);
        if(note[0]=='F')return freq((note[2]-'4')*12-3);
        if(note[0]=='D')
        {
            if(note[1]=='b')return freq((note[2]-'4')*12-4);
            if(note[1]=='#')return freq((note[2]-'4')*12-1);
        }
    }
    return 0;
}

int tempo=120;//Default tempo = 2b/s = 120b/min;
double note_val=4;//Default note value;

class Tone //A separate class for tones;
{
    frequency mfreq;//Frequency of the tone;
    double length;//Actual tone length, default is 4;

public:

    Tone(string note)
    {
        mfreq=stofr(note);
        length=4;
    }

    Tone(frequency fr=0)
    {
        mfreq=fr;
        length=4;
    }

    Tone& operator=(string note)
    {
        mfreq=stofr(note);
    }

    Tone& operator=(frequency fr)
    {
        mfreq=fr;
    }

    const frequency FREQ()const
    {
        return mfreq;
    }

    const int LEN()const
    {
        return length;
    }

    friend istream& operator>>(istream& in,Tone& tone);//"Friendzoned" so we can change Tone::length;
};

ostream& operator<<(ostream& out,Tone tone)//STD output operator;
{
    if(tone.FREQ())//Check if mute;
        Beep(tone.FREQ(),1000*60*double(note_val/tone.LEN())/tempo);//WinAPI Beep(double freq, long msec),
                                                                    // msec is caluclated according to the BPM definitoin of tone length;
    return out;
}

istream& operator>>(istream& in,Tone& tone)//STD input operator;
{
    string buff;
    in>>buff;
    if(buff[0]=='v')//Default ote value;
    {
        note_val=stof(buff.substr(1,buff.size()-1));
        tone.mfreq=0;
        return in;
    }
    if(buff[0]=='x')//Set note current length;
    {
        tone.length=stof(buff.substr(1,buff.size()-1));
        tone.mfreq=0;
        return in;
    }
    if(buff[0]=='t')//Change the tempo;
    {
        tempo=stof(buff.substr(1,buff.size()-1));
        tone.mfreq=0;
        return in;
    }
    if(buff[0]=='p')//Pause with length equal;
    {
        tone.mfreq=1;
        return in;
    }
    if(buff[0]>='0'&&buff[0]<='9')//If we have a floating point frequency;
    {
        tone.mfreq=stof(buff);
    }
    else tone.mfreq=stofr(buff);//If the tone is in such format (A...G)+(b||#)+(0...10);
    return in;
}

bool FileExists(string file)//Check if file exists, using WinAPI;
{
    DWORD value;
    value = GetFileAttributes(file.c_str());
    return !(value == ((DWORD)-1));
}

bool ValidFile(string file)//Validate file;
{
    return (FileExists(file)&&file.substr(file.find_last_of('.'),file.size()-file.find_last_of('.'))==".mp");//Check file format and if file exists;
}

int main(int argc,char **argv)
{
    bool quit=0;
    Tone n;
    string MPfile;
    while(!quit)
    {
        string MPfile;
        cin>>MPfile;
        if(MPfile=="exit")
        {
            quit=1;
            continue; //Nice and simple exit command;
        }
        if(ValidFile(MPfile))
        {
            ifstream in(MPfile.c_str());
            while(in.good())
            {
                in>>n;
                cout<<n;
            }
        }
        else cout<<"Error handling file."<<Tone("G7");
        if(MPfile=="exit")quit=1;
    }
    return 0;
}

