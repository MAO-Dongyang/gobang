#ifndef MAIN_H_
#define MAIN_H_


#include <locale.h>
#include<unistd.h>
#include<fcntl.h>
#define CLRSCR system("clear");                         // Linux 命令 (Win与Lin实现方法不同
#define OUTTYPE "%s "
#include<cstdio>
#include<cstdlib>
#include<ctime>
#include<cstring>                       /// UPDATE: | AI |
                                        /// option menu
                                        /// optimize check Algorithm
#define CHESS 5                                         //宏定义棋子数
#define SLEN 10                                         //宏定义棋盘大小
#define BOARD (SLEN*2+1)
#define FLASH 0.001                                     //输入的检测频率

#include <gperftools/profiler.h>


#include<termio.h>                                      // Linux 专用头文件，实现getch()、kbhit()
int getch(void);                            //实现：输入不需要回车键便返回
int kbhit(void);                            //实现：检测键盘是否有输入      （这两个函数需要自行实现）



                                //   超级堆叠宏定义
#define LOCATE  check[row][col]                 //格子对应的值
#define NOTCHESS  LOCATE==0                     //格子对应值的判断
#define ISWHITE  LOCATE==-1
#define ISBLACK  LOCATE==1
#define GOTOCURR gotoxy(1+2*(1+2*col),2+2*row);     //将光标移到光标位置
#define printSPACE { GOTOCURR printf(OUTTYPE,SPACE); }      //输出空格
#define printPOS { GOTOCURR printf(OUTTYPE,POS); }          //输出光标
#define printWHITE { GOTOCURR printf(OUTTYPE,WHITE); }      //输出白棋
#define printBLACK { GOTOCURR printf(OUTTYPE,BLACK); }      //输出黑棋
#define INFORM { gotoxy(0,BOARD+1); if(turn>0) printf("  %s  Black turn",BLACK); else printf("  %s  White turn",WHITE); printf("    POS(%2d,%2d)  undo: %d\n",row+1,col+1,undo); }  //棋盘底状态输出
#define MOVE  {if(NOTCHESS) LOCATE=3;else if(ISWHITE) LOCATE=-2;else if(ISBLACK) LOCATE=2;INFORM}   //光标移动
#define RESTORE  {if(LOCATE==3) {LOCATE=0; printSPACE} else if(LOCATE==-2) {LOCATE=-1; printWHITE} else if(LOCATE==2) {LOCATE=1;printBLACK} }   //光标移出格子的回复
#define PUTDOWN {if(turn==-1) { printWHITE LOCATE=turn; } else if(turn==1) { printBLACK LOCATE=turn; } }    //下子
#define MEMORY {reg[count][0]=turn; reg[count][1]=row; reg[count][2]=col;}      //把下棋状态记在悔棋回合记录里
#define PLACE {PUTDOWN; MEMORY; turn=-turn; count++; INFORM; }                  //下子的总过程


void welcome(void);                                     //欢迎界面
void envir(void);                                       //环境构建
void print(void);                                       //棋盘输出
int Icon(void);                                         //核心 控制、交互部分
int checkchess(void);                                   //棋子布局检测
void regret(void);                                      //悔棋
void gameover(int);                                     //游戏结束后的判定输出
void delay(double);                                     //延时函数
void gotoxy(unsigned,unsigned);                         //将输入光标移动到指定位置

const char* board[BOARD][BOARD]={0};                    //棋盘格子样式
int check[SLEN][SLEN]={0};                              //格子的状态
int reg[SLEN*SLEN][3]={0};                              //悔棋：回合下子记录
int turn=-1;                    // -1==white  1==black  //回合
int row=0, col=0,  con;                                 //行列  |  输入记录
int count=0, undo=0;                                    //下子数  |  悔棋数

const char * BLACK="○",  * WHITE ="●",   * POS  ="¤";   // UTF-8 棋子、棋盘字符
const char * LT   ="┌",  * TOP   ="┬",   * RT   ="┐";
const char * LEFT ="├",  * CENTER="┼",   * RIGHT="┤";
const char * LF   ="└",  * FOOT  ="┴",   * RF   ="┘";
const char * SPACE=" ";

#endif // MAIN_H_
