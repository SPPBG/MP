#ifndef CRT_LCD_LED_H_INCLUDED
#define CRT_LCD_LED_H_INCLUDED

#include <windows.h>
#include <process.h>
#include <cstring>
#include <time.h>
struct Pnt{
double x,y;
Pnt(double a=0,double b=0){
x=a,y=b;
}
};

void delay(const long& ms){
clock_t time_c=clock();
clock_t stop_time_c=clock()+ms;
while(time_c<stop_time_c)time_c=clock();
}

void clear(){
system("cls");
}

char getchar(Pnt P){
HANDLE hCon=GetStdHandle(STD_OUTPUT_HANDLE);
COORD pos;
char buff[4];
DWORD r;
pos.X=P.x-1,pos.Y=P.y-1;
ReadConsoleOutputCharacter(hCon,buff,1,pos,&r);
return *buff;
}

void putchar(char c,Pnt P){
HANDLE hCon=GetStdHandle(STD_OUTPUT_HANDLE);
COORD pos;
DWORD r;
pos.X=P.x-1,pos.Y=P.y-1;
WriteConsoleOutputCharacter(hCon,&c,1,pos,&r);
}

void gotoxy(Pnt P){
HANDLE hCon=GetStdHandle(STD_OUTPUT_HANDLE);
COORD dwCursorPosition;
dwCursorPosition.X=(P.x)-1,dwCursorPosition.Y=(P.y)-1;
SetConsoleCursorPosition(hCon,dwCursorPosition);
}

///The original library ends here
///where I've added some utility functions useful for animation
/// or just for easier access to the upper ones.
///Enjoy.
#include<cstring>
void insert(const char* str,Pnt P){//Warning: this WILL DELETE previous output in the begining of the row if the input string is longer;
for(unsigned int i=0;i<strlen(str);i++)putchar(str[i],Pnt(int(P.x+i)%81,P.y));
}

//A more standart relative of insert;
void print(const char* str,int col,int r){//Warning: this WILL DELETE previous output in the begining of the next row if the input string is longer;
for(unsigned int i=0;i<strlen(str);i++)putchar(str[i],Pnt(int(col+i)%81,r));
}

//A highlight function;
void flash(const char* msg,Pnt P,int dur){
for(int i=0;i<dur;i++){
insert(msg,P);
delay(200);
for(unsigned int i=0;i<strlen(msg);i++)putchar(' ',P);
delay(200);
}
insert(msg,P);
}

void ClearRow(int row){
for(int i=0;i<81;i++)putchar(' ',Pnt(i,row));
};

void Float(const char* s,int row,long dur,int speed){
clock_t stop_time_c=clock()+dur;
for(int i=1;clock()<=stop_time_c;i++){insert(s,Pnt(i,row));delay(100/speed);ClearRow(row);}
ClearRow(row);
}


Pnt operator*(double m,Pnt mp){
return Pnt(m*mp.x,m*mp.y);
}
Pnt operator+(Pnt A,Pnt B){
return Pnt(A.x+B.x,A.y+B.y);
}
Pnt operator-(Pnt A,Pnt B){
return Pnt(A.x-B.x,A.y-B.y);
}
Pnt BLP(Pnt P0,Pnt P1,double t){
return (1-t)*P0+t*P1;
}
void Draw_Line(Pnt P0,Pnt P1){
for(double t=0.0;t<=1.0;t+=0.01)putchar('o',BLP(P0,P1,t));
}
void Draw_Triangle(Pnt P0,Pnt P1,Pnt P2){
Draw_Line(P0,P1),Draw_Line(P1,P2),Draw_Line(P0,P2);
}

void Draw_Rect(Pnt P0,Pnt P1){
Pnt P2(P1.x,P0.y),P3(P0.x,P1.y);
Draw_Line(P0,P2),Draw_Line(P2,P1),Draw_Line(P1,P3),Draw_Line(P3,P0);
}

#endif // CRT_LCD_LED_H_INCLUDED
