#include <list>

#include "Sound.h"
#include "CRT_LCD_LED.h"

#define OPEN "@open" ///Open file and play
#define PLAY "@play" ///--||--
#define NEXT "@next" ///Play next item
#define END "@end" ///End token
#define VOL "@vol" ///Change volume
#define ENQ "@enq"  ///Enqueue file
#define MKLST "@mklst" ///Make list (erases current)
#define ENQLST "@enqlst" ///Enqueue items
#define MKPLAYLST "@mkplaylst" ///Create playlist file
#define TGPAUSE "@tgpause" ///Toggle pause
#define GPAUSE "@gpause" ///Get pause
#define LIB "@lib" ///Set library folder

string libdir="";

typedef unsigned int Uint32;

list<string> flist;
string current_file;
bool quit=0;

string itos(int n)
{
    string a="";
    while(n>0)
    {
        a=char((n%10)+'0')+a;
        n/=10;
    }
    return a;
}

string time_format(Uint32 u)
{
    char buff[16];
    sprintf(buff,"%d:%d\0",u/60,u%60);
    return string(buff);
}

class Manager
{
public:

    Manager()
    {
        sys_init();
    }

    ~Manager()
    {
        free_sys();
    }

    void toggle_pause()
    {
        FMOD_BOOL p;
        FMOD_Channel_GetPaused(channel,&p);
        FMOD_Channel_SetPaused(channel,!p);
    }

    bool exists(string f)
    {
        ifstream in(f.c_str());
        return !in.eof();
    }

    const char* get_pause()
    {
        int paused=0;
        if(channel)
        {
            result = FMOD_Channel_GetPaused(channel, &paused);
            if ((result != FMOD_OK) && (result != FMOD_ERR_INVALID_HANDLE) && (result != FMOD_ERR_CHANNEL_STOLEN))
            {
                ERRCHK(result);
            }
        }
        return paused?"Yes":"No";
    }

    unsigned int get_length()
    {
        if(!channel)return 0;
        Uint32 lenms=0;
        if(channel)
        {
            FMOD_SOUND* currentsound=NULL;
            FMOD_Channel_GetCurrentSound(channel, &currentsound);
            if (currentsound)
            {
                result = FMOD_Sound_GetLength(currentsound, &lenms, FMOD_TIMEUNIT_MS);
                if ((result != FMOD_OK) && (result != FMOD_ERR_INVALID_HANDLE) && (result != FMOD_ERR_CHANNEL_STOLEN))
                {
                    ERRCHK(result);
                }
            }
        }
        return lenms/=1000;
    }

    unsigned int get_position()
    {
        Uint32 ms=0;
        if(!channel)return 0;
        result = FMOD_Channel_GetPosition(channel, &ms, FMOD_TIMEUNIT_MS);
        if ((result != FMOD_OK) && (result != FMOD_ERR_INVALID_HANDLE) && (result != FMOD_ERR_CHANNEL_STOLEN))
        {
            ERRCHK(result);
        }
        return ms/=1000;
    }

    void play(string f)
    {
        if(!exists(libdir+f)||!exists(f))
        {
            cout<<"\nDoes not exist!\n";
            return;
        }

        if(format(f)==".mpl")
        {
            ifstream inp(f.c_str());
            string buff;
            do
            {
            getline(inp,buff);
            }while(!exists(buff));
            current_file=buff;
            play(buff);
            while(inp.good())
            {
                getline(inp,buff);
                flist.push_front(buff);
            }
            return;
        }
        current_file=libdir+f;
        load(current_file);
        if(channel)play_sound(FMOD_CHANNEL_REUSE);
        else play_sound(FMOD_CHANNEL_FREE);
    }

    bool play_next()
    {

        if(flist.empty())
        {
            current_file="";
            return 0;
        }

        for(;!flist.empty();)
        {
            if(exists(flist.front()))
            {
                play(flist.front());
                flist.pop_front();
                return 1;
            }
            flist.pop_front();
        }

    }

    bool enqueue(string f)
    {
        if(!exists(f))
        {
            return 0;
        }
        if(format(f)!=".mpl")
        {
            flist.push_back(libdir+f);
            return 1;
        }
        ifstream inp(f.c_str());
        while(inp.good())
        {
            string buff;
            getline(inp,buff);
            flist.push_back(buff);
        }
        return 1;
    }

    void make_list()
    {
        while(!flist.empty())flist.pop_front();
        for(; true;)
        {
            string inp;
            cin>>inp;
            if(inp[0]=='@')
            {
                if(inp=="@end")return;
            }
            else enqueue(inp);
        }
    }

    void enqueue_list()
    {
        for(;true;)
        {
            string inp;
            cin>>inp;
            if(inp[0]=='@')
            {
                if(inp=="@end")return;
            }
            else enqueue(inp);
        }
    }

    void make_playlist()
    {
        string nfilename;
        cin>>nfilename;
        if(format(nfilename)!=string(".mpl"))return;
        if(exists(nfilename))
        {
            cout<<"\nDo you want to overwrite this file? <y/n>\n";
            char yn;
            cin>>yn;
            if(yn=='n')return;
        }
        ofstream outp(nfilename.c_str());
        for(; true;)
        {
            string inp;
            cin>>inp;

            if(inp[0]=='@')
            {
                if(inp=="@end")return;
                if(inp=="@flst")
                {
                    cout<<"\nEmpty current queue list? <y/n>\n";
                    char yn;
                    cin>>yn;
                    if(yn=='y')
                        while(!flist.empty())
                        {
                            outp<<flist.front()<<endl;
                            flist.pop_front();
                        }
                    else
                    {
                        list<string>::iterator it=flist.begin();
                        while(it!=flist.end())
                        {
                            outp<<(*it)<<endl;
                            it++;
                        }
                    }
                }
            }

            else outp<<libdir+inp<<endl;
        }
    }

    void empty_list()
    {
        while(!flist.empty())flist.pop_front();
    }

    void handle_input()
    {
        if(!_kbhit())return;
        char base=_getch();
        if(base==27)
        {
            quit=1;
            return;
        }
        if(base!='@')return;
        cout<<base;
        string cmd;
        cin>>cmd;
        cmd=base+cmd;
        if(cmd=="@play"||cmd=="@open")
        {
            string arg;
            cin>>arg;
            play(arg);
        }
        else if(cmd=="@enq")
        {
            string arg;
            cin>>arg;
            enqueue(arg);
        }
        else if(cmd=="@empty")empty_list();
        else if(cmd=="@mklst")make_list();
        else if(cmd=="@mkplaylst")make_playlist();
        else if(cmd=="@enqlst")enqueue_list();
        else if(cmd=="@next")play_next();
        else if(cmd=="@tgpause")toggle_pause();
        else if(cmd=="@gpause")cout<<get_pause();
        else if(cmd=="@vol")
        {
            double volume;
            cin>>volume;
            result=FMOD_Channel_SetVolume(channel,volume/100);
            ERRCHK(result);
        }
        else if(cmd=="@lib")
        {
            cin>>libdir;
            if(libdir[libdir.size()-1]!='\\')libdir+='\\';
        }
        else cout<<"\nUnkown command;\n";
    }

    void main_loop()
    {
        while(!quit)
        {
            gotoxy(Pnt(1,1));
            cout.flush();
            if(!(get_length()-get_position()))play_next();
            cout<<"<"<<time_format(get_position())<<"/"<<time_format(get_length());
            cout<<">\nPaused: "<<get_pause()<<endl<<"\""<<get_name()<<"\""<<endl;
            handle_input();
            FMOD_System_Update(sys);
            clear();
        }
    }

    //void

};

int main()
{
    Manager AlphaSys;
    AlphaSys.main_loop();
    return 0;
}
