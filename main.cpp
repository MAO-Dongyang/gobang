#include "main.h"

int main(void)
{
    int s;                                              //游戏结束的判定状态输出
    welcome();                                          //欢迎界面
    envir();                                            //棋盘构建
    s=Icon();                                           //控制交互部分
    gameover(s);                                        //游戏结束
    return 0;
 }

