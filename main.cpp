#include <fcntl.h>
#include <locale.h>
#include <termio.h> // Linux 专用头文件，实现getch()、kbhit()
#include <unistd.h>
#define CLRSCR system("clear"); // Linux 命令 (Win与Lin实现方法不同
#define OUTTYPE "%s "
int getch(void); //实现：输入不需要回车键便返回
int kbhit(void); //实现：检测键盘是否有输入      （这两个函数需要自行实现）


#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#define CHESS 5 //宏定义棋子数


#include <gperftools/profiler.h>

#ifdef SIZE

#define SLEN SIZE	

#else

#define SLEN 10 //宏定义棋盘大小

#endif



#define BOARD (SLEN * 2 + 1)
#define FLASH 0.001 //输入的检测频率

void welcome(void);              //欢迎界面
void envir(void);                //环境构建
void print(void);                //棋盘输出
int Icon(void);                  //核心 控制、交互部分
int checkchess(void);            //棋子布局检测
void regret(void);               //悔棋
void gameover(int);              //游戏结束后的判定输出
void delay(double);              //延时函数
void gotoxy(unsigned, unsigned); //将输入光标移动到指定位置

const char *board[BOARD][BOARD] = {0}; //棋盘格子样式
int check[SLEN][SLEN] = {0};           //格子的状态
int reg[SLEN * SLEN][3] = {0};         //悔棋：回合下子记录
int turn = -1;                         // -1==white  1==black  //回合
int row = 0, col = 0, con;             //行列  |  输入记录
int count = 0, undo = 0;               //下子数  |  悔棋数

//const char *BLACK = "☹", *WHITE = "☺", *POS = "🐺"; // UTF-8 棋子、棋盘字符
//const char *BLACK = "✅", *WHITE = "⚠", *POS = "☂"; // UTF-8 棋子、棋盘字符
const char *BLACK = "○", *WHITE = "●", *POS = "¤"; // UTF-8 棋子、棋盘字符
const char *LT = "┌", *TOP = "┬", *RT = "┐";
const char *LEFT = "├", *CENTER = "┼", *RIGHT = "┤";
const char *LF = "└", *FOOT = "┴", *RF = "┘";
const char *SPACE = " ";

int main(void) {
  ProfilerStart("info.prof");
  int s;       //游戏结束的判定状态输出
  welcome();   //欢迎界面
  envir();     //棋盘构建
  s = Icon();  //控制交互部分
  gameover(s); //游戏结束
  ProfilerStop();
  return 0;
}

void welcome(void) {
  printf("|     Welcome To Play       |\n");
  printf("|     %d In Line Game        |\n", CHESS);
  printf("|      Board: %2d X %2d       |\n", SLEN, SLEN);
  printf("|                           |\n");
  printf("|        Copyright:         |\n");
  printf("|       MAO-SONG-YAN        |\n");
  printf("\033[?25l"); //（隐藏光标）Linux下的特殊转义字符
  getch();
}

void envir(void) //棋盘构建
{
  for (int j = 0; j < BOARD; j++) // 先全部赋为空格
    for (int k = 0; k < BOARD; k++)
      board[j][k] = SPACE;

  board[0][0] = LT; //棋盘四个角的搭建
  board[0][BOARD - 1] = RT;
  board[BOARD - 1][0] = LF;
  board[BOARD - 1][BOARD - 1] = RF;

  for (int i = 2; i < BOARD - 2; i += 2) { //四边搭建
    board[0][i] = TOP;
    board[BOARD - 1][i] = FOOT;
    board[i][0] = LEFT;
    board[i][BOARD - 1] = RIGHT;
  }
  for (int j = 2; j < BOARD - 2; j += 2) //非边角搭建
    for (int k = 2; k < BOARD - 2; k += 2) {
      board[j][k] = CENTER;
    }
}

void print(void) {
  CLRSCR; //清屏 清除欢迎界面
  for (int j = 0; j < BOARD; j++) {
    for (int k = 0; k < BOARD; k++) ///  这里比较复杂
    {
      if ((j - 1) % 2 == 0 &&
          (k - 1) % 2 == 0) { //根据格子对应check[][]的值输出
        int sam = check[(j - 1) / 2][(k - 1) / 2];
        if (sam == 0)
          printf("  ");
        else if (sam == -1)
          printf(OUTTYPE, WHITE);
        else if (sam == 1)
          printf(OUTTYPE, BLACK);
        else if (sam == 3 || sam == -2 || sam == 2)
          printf(OUTTYPE, POS);
      } else
        printf(OUTTYPE, board[j][k]);
    }
    putchar('\n');
  }
  if (turn == 1)
    printf("  %s  Black turn", BLACK); //回合输出
  else
    printf("  %s  White turn", WHITE);
  printf("    POS(%2d,%2d)  undo: %d\n", row + 1, col + 1,
         undo); //光标位置与悔棋次数
}
//   超级堆叠宏定义
#define LOCATE check[row][col] //格子对应的值
#define NOTCHESS LOCATE == 0   //格子对应值的判断
#define ISWHITE LOCATE == -1
#define ISBLACK LOCATE == 1
#define GOTOCURR                                                               \
  gotoxy(1 + 2 * (1 + 2 * col), 2 + 2 * row); //将光标移到光标位置
#define printSPACE                                                             \
  { GOTOCURR printf(OUTTYPE, SPACE); } //输出空格
#define printPOS                                                               \
  { GOTOCURR printf(OUTTYPE, POS); } //输出光标
#define printWHITE                                                             \
  { GOTOCURR printf(OUTTYPE, WHITE); } //输出白棋
#define printBLACK                                                             \
  { GOTOCURR printf(OUTTYPE, BLACK); } //输出黑棋
#define INFORM                                                                 \
  {                                                                            \
    gotoxy(0, BOARD + 1);                                                      \
    if (turn > 0)                                                              \
      printf("  %s  Black turn", BLACK);                                       \
    else                                                                       \
      printf("  %s  White turn", WHITE);                                       \
    printf("    POS(%2d,%2d)  undo: %d\n", row + 1, col + 1, undo);            \
  } //棋盘底状态输出
#define MOVE                                                                   \
  {                                                                            \
    if (NOTCHESS)                                                              \
      LOCATE = 3;                                                              \
    else if (ISWHITE)                                                          \
      LOCATE = -2;                                                             \
    else if (ISBLACK)                                                          \
      LOCATE = 2;                                                              \
    INFORM                                                                     \
  } //光标移动
#define RESTORE                                                                \
  {                                                                            \
    if (LOCATE == 3) {                                                         \
      LOCATE = 0;                                                              \
      printSPACE                                                               \
    } else if (LOCATE == -2) {                                                 \
      LOCATE = -1;                                                             \
      printWHITE                                                               \
    } else if (LOCATE == 2) {                                                  \
      LOCATE = 1;                                                              \
      printBLACK                                                               \
    }                                                                          \
  } //光标移出格子的回复
#define PUTDOWN                                                                \
  {                                                                            \
    if (turn == -1) {                                                          \
      printWHITE LOCATE = turn;                                                \
    } else if (turn == 1) {                                                    \
      printBLACK LOCATE = turn;                                                \
    }                                                                          \
  } //下子
#define MEMORY                                                                 \
  {                                                                            \
    reg[count][0] = turn;                                                      \
    reg[count][1] = row;                                                       \
    reg[count][2] = col;                                                       \
  } //把下棋状态记在悔棋回合记录里
#define PLACE                                                                  \
  {                                                                            \
    PUTDOWN;                                                                   \
    MEMORY;                                                                    \
    turn = -turn;                                                              \
    count++;                                                                   \
    INFORM;                                                                    \
  } //下子的总过程

int Icon(void) //核心 控制、交互部分
{
  unsigned int s = 0; //输入检测的总次数
  LOCATE = 3;         //初始光标位置
  print();            //棋盘整体输出    （其实0.8版只用了一次）
  system("stty -echo"); // Linux命令   命令不被显示
  while (1) {
    if (kbhit()) { //检测键盘是否有输入   有输入情况下
      while ((con = getch()) != '`') //退出按键
      {
        if (con == 29)
          if (getch() == 91)
            con = getch(); // Linux键盘控制的特殊情况
        if (con == 'w' || con == 's' || con == 'a' || con == 'd' ||
            con == ' ' || con == 65 || con == 66 || con == 67 || con == 68 ||
            con == '/' || con == 'r') {
          if (con != ' ' && con != '/')
            RESTORE;     //移动光标前，先把原格子复位
          switch (con) { //光标移动、下子
          case 'w':
            if (row > 0)
              row--;
            else
              row = SLEN - 1;
            MOVE;
            printPOS break;
          case 's':
            if (row < SLEN - 1)
              row++;
            else
              row = 0;
            MOVE;
            printPOS break;
          case 'a':
            if (col > 0)
              col--;
            else
              col = SLEN - 1;
            MOVE;
            printPOS break;
          case 'd':
            if (col < SLEN - 1)
              col++;
            else
              col = 0;
            MOVE;
            printPOS break;
          case 65:
            if (row > 0)
              row--;
            else
              row = SLEN - 1;
            MOVE;
            printPOS break;
          case 66:
            if (row < SLEN - 1)
              row++;
            else
              row = 0;
            MOVE;
            printPOS break;
          case 68:
            if (col > 0)
              col--;
            else
              col = SLEN - 1;
            MOVE;
            printPOS break;
          case 67:
            if (col < SLEN - 1)
              col++;
            else
              col = 0;
            MOVE;
            printPOS break;
          case ' ':
            if (LOCATE == 3)
              PLACE;
            break;
          case '/':
            if (LOCATE == 3)
              PLACE;
            break;
          case 'r':
            if (count >= 0)
              regret();
            break; //悔棋按钮
          default:
            break;
          }
        }
        if (count == SLEN * SLEN)
          return 0; //如果棋盘满 返回平局
        if (checkchess())
          return 1; //如果一方赢了 返回分出胜负
        break;
      }
      if (con == '`')
        return 2; //强制退出
    } else {
      if (s % 500 == 0) { //每检测输入500次闪烁一次
        GOTOCURR          //去光标所在格子替换原字符
            if (LOCATE == -1) {
          LOCATE = -2;
          printf(OUTTYPE, POS);
        }
        else if (LOCATE == -2) {
          LOCATE = -1;
          printf(OUTTYPE, WHITE);
        }
        else if (LOCATE == 1) {
          LOCATE = 2;
          printf(OUTTYPE, POS);
        }
        else if (LOCATE == 2) {
          LOCATE = 1;
          printf(OUTTYPE, BLACK);
        }
        // print();
      }
      s++;
      delay(FLASH); //延时
    }
  }
}

int checkchess(void) //棋子布局检测
{
  bool judge;                    //胜利判断
  int pres;                      //当前格子对应值
  for (int j = 0; j < SLEN; j++) /// check rows || columns
    for (int k = 0; k < SLEN; k++)
      if (check[j][k] != 0) {
        pres = check[j][k];
        judge = true;
        if (j <= SLEN - CHESS && k <= SLEN - CHESS) {
          for (int l = 1; l < CHESS; l++)
            if (check[j + l][k + l] != pres)
              judge = false; /// oblique right
          if (judge == true)
            return 1; //右斜
        }
        judge = true;
        if (j <= SLEN - CHESS && k >= CHESS - 1) {
          for (int l = 1; l < CHESS; l++)
            if (check[j + l][k - l] != pres)
              judge = false; /// oblique left
          if (judge == true)
            return 1; //左斜
        }
        judge = true;
        if (k <= SLEN - CHESS) {
          for (int l = 1; l < CHESS; l++)
            if (check[j][k + l] != pres)
              judge = false; /// horizontal
          if (judge == true)
            return 1; //水平
        }
        judge = true;
        if (j <= SLEN - CHESS) {
          for (int l = 1; l < CHESS; l++)
            if (check[j + l][k] != pres)
              judge = false; /// vertical
          if (judge == true)
            return 1; //竖直
        }
      }
  return 0;
}

void gameover(int s) //游戏结束输出
{
  gotoxy(0, BOARD + 2); //把输入光标移去底部
  if (s) {
    if (s == 2)
      printf("Force exit\n");
    else if (turn == 1)
      printf("\t-----%s White Won-----\n", WHITE);
    else
      printf("\t-----%s Black Won-----\n", BLACK);
  } else
    printf("\t-----Tied-----\n");
  system("stty echo"); //终端 恢复默认状态
  printf("\033[?25h");
}

void gotoxy(unsigned x, unsigned y) //将输入光标移动到指定位置
{
  printf("%c[%d;%df", 0x1b, y, x); //同样是特殊的转义字符（感谢圣哥的技术狗窝）
}

void delay(double z) //延时函数
{
  double start = (double)clock() / CLOCKS_PER_SEC; //利用CPU时钟定时
  while (((double)clock() / CLOCKS_PER_SEC) - start < z)
    ;
}

void regret(void) //悔棋实现函数
{
  if (count) {
    count--;
    turn = -turn;
    undo++;
    row = reg[count][1]; //根据数组记录内容悔棋
    col = reg[count][2];
    for (int j = 0; j < 3; j++)
      reg[count][j] = 0;
  }
  INFORM;
  printPOS;
  LOCATE = 3;
}

int getch(void) {
  struct termios tm, tm_old;
  int fd = 0, ch;
  if (tcgetattr(fd, &tm) < 0)
    return -1;
  tm_old = tm;
  cfmakeraw(&tm);
  if (tcsetattr(fd, TCSANOW, &tm) < 0)
    return -1;
  ch = getchar();
  if (tcsetattr(fd, TCSANOW, &tm_old) < 0)
    return -1;
  return ch;
}

int kbhit(void) {
  struct termios oldt, newt;
  int ch;
  int oldf;
  tcgetattr(STDIN_FILENO, &oldt);
  newt = oldt;
  newt.c_lflag &= ~(ICANON | ECHO);
  tcsetattr(STDIN_FILENO, TCSANOW, &newt);
  oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
  fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);
  ch = getchar();
  tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
  fcntl(STDIN_FILENO, F_SETFL, oldf);
  if (ch != EOF) {
    ungetc(ch, stdin);
    return 1;
  }
  return 0;
}
