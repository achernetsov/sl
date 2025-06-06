/*========================================
 *    sl.c: SL version 5.06
 *        Copyright 1993,1998,2014-2015,2019,2024
 *                  Toyoda Masashi
 *                  (mtoyoda@acm.org)
 *        Last Modified: 2024/03/16
 *========================================
 */
/* sl version 5.06 : by Artyom Chernetsov and Katya Nevskaya 2024-03-16
 * - Cat version
 * sl version 5.05 : by eyJhb 2019-08-14
 * - Merged pull request from cosmo-ray adding orange TGV (-G)
 * sl version 5.04 : by eyJhb 2019-03-19
 * - Merged pull request from EverNine allowing to specify the number of cars
 * - Merged pull request from ddugovic which adds 
 *      `-e` for `ctrl+c` to work
 *      `-d` to enable disco mode
 *      `-w` making the locomotive go faster
 */
/* sl version 5.03 : Fix some more compiler warnings.                        */
/*                                              by Ryan Jacobs    2015/01/19 */
/* sl version 5.02 : Fix compiler warnings.                                  */
/*                                              by Jeff Schwab    2014/06/03 */
/* sl version 5.01 : removed cursor and handling of IO                       */
/*                                              by Chris Seymour  2014/01/03 */
/* sl version 5.00 : add -c option                                           */
/*                                              by Toyoda Masashi 2013/05/05 */
/* sl version 4.00 : add C51, usleep(40000)                                  */
/*                                              by Toyoda Masashi 2002/12/31 */
/* sl version 3.03 : add usleep(20000)                                       */
/*                                              by Toyoda Masashi 1998/07/22 */
/* sl version 3.02 : D51 flies! Change options.                              */
/*                                              by Toyoda Masashi 1993/01/19 */
/* sl version 3.01 : Wheel turns smoother                                    */
/*                                              by Toyoda Masashi 1992/12/25 */
/* sl version 3.00 : Add d(D51) option                                       */
/*                                              by Toyoda Masashi 1992/12/24 */
/* sl version 2.02 : Bug fixed.(dust remains in screen)                      */
/*                                              by Toyoda Masashi 1992/12/17 */
/* sl version 2.01 : Smoke run and disappear.                                */
/*                   Change '-a' to accident option.                         */
/*                                              by Toyoda Masashi 1992/12/16 */
/* sl version 2.00 : Add a(all),l(long),F(Fly!) options.                     */
/*                                              by Toyoda Masashi 1992/12/15 */
/* sl version 1.02 : Add turning wheel.                                      */
/*                                              by Toyoda Masashi 1992/12/14 */
/* sl version 1.01 : Add more complex smoke.                                 */
/*                                              by Toyoda Masashi 1992/12/14 */
/* sl version 1.00 : SL runs vomiting out smoke.                             */
/*                                              by Toyoda Masashi 1992/12/11 */

#include <ctype.h>
#include <curses.h>
#include <limits.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include "sl.h"

#define VERSION "5.06"

void add_smoke(int y, int x);
void add_man(int y, int x);
int add_C51(int x);
int add_D51(int x);
int add_TGV(int x);
int add_CAT(int x);
int add_sl(int x);
void option(char *str);
int my_mvaddstr(int y, int x, char *str);

int ACCIDENT  = 0;
int C51       = 0;
int NUMBER    = -1;
int DISCO     = 0;
int SIGNAL    = 1;
int FLY       = 0;
int LOGO      = 0;
int WIND      = 0;
int TGV       = 0;
int CAT       = 0;

int my_mvaddstr(int y, int x, char *str)
{
    for ( ; x < 0; ++x, ++str)
        if (*str == '\0')  return ERR;
    for ( ; *str != '\0'; ++str, ++x)
        if (mvaddch(y, x, *str) == ERR)  return ERR;
    return OK;
}

void option(char *str)
{
    extern int ACCIDENT, C51, DISCO, SIGNAL, FLY, LOGO, WIND, TGV;

    while (*str != '\0') {
        switch (*str) {
            case 'a': ACCIDENT = 1; break;
            case 'c': C51      = 1; break;
            case 'G': TGV      = 1; break;
            case 'd': DISCO    = 1; break;
            case 'e': SIGNAL   = 0; break;
            case 'F': FLY      = 1; break;
            case 'l': LOGO     = 1; break;
            case 'w': WIND     = 200; break;
            case 'C': CAT      = 1; break;
            case 'v': 
              printf("Version: %s, last updated: 2019-03-19\n", VERSION);
              exit(0);
              break;
            default:
              if (isdigit(*str))
                  NUMBER = (NUMBER < 0 ? 0 : NUMBER*10) + *str - '0';
              break;
        }
        str++;
    }
}

int main(int argc, char *argv[])
{
    int x, i;
    int base_usleep = 40000;

    for (i = 1; i < argc; ++i) {
        if (*argv[i] == '-') {
            option(argv[i] + 1);
        }
    }
    initscr();
    if (DISCO == 1) {
        start_color();
        init_pair(4, COLOR_RED, COLOR_BLACK);
        init_pair(3, COLOR_YELLOW, COLOR_BLACK);
        init_pair(2, COLOR_GREEN, COLOR_BLACK);
        init_pair(1, COLOR_CYAN, COLOR_BLACK);
    }
    if (SIGNAL) signal(SIGINT, SIG_IGN);
    noecho();
    curs_set(0);
    nodelay(stdscr, TRUE);
    leaveok(stdscr, TRUE);
    scrollok(stdscr, FALSE);
/*
 *   first Non prototype TGV was orange, first 1 was red
 */
    if (TGV == 1) {
	if (has_colors()) {
	    start_color();
	    init_pair(1, COLOR_WHITE, COLOR_BLACK);
	    init_pair(2, COLOR_YELLOW, COLOR_BLACK);
	    base_usleep /= 2;
	    if (WIND)
		WIND = 150;
	} else {
	    TGV = 0;
	}
    }

    for (x = COLS - 1; ; --x) {
        if (LOGO == 1) {
            if (add_sl(x) == ERR) break;
        }
        else if (C51 == 1) {
            if (add_C51(x) == ERR) break;
        }
        else if (TGV == 1) {
            if (add_TGV(x) == ERR) break;
        }
        else if (CAT == 1) {
            if (add_CAT(x) == ERR) break;
        }
        else {
            if (add_D51(x) == ERR) break;
        }
        getch();
        refresh();
        usleep(base_usleep - (WIND * 100));
    }
    mvcur(0, COLS - 1, LINES - 1, 0);
    endwin();

    return(EXIT_FAILURE);
}


int add_sl(int x)
{
    static char *sl[LOGOPATTERNS][LOGOHEIGHT + 1]
        = {{LOGO1, LOGO2, LOGO3, LOGO4, LWHL11, LWHL12, DELLN},
           {LOGO1, LOGO2, LOGO3, LOGO4, LWHL21, LWHL22, DELLN},
           {LOGO1, LOGO2, LOGO3, LOGO4, LWHL31, LWHL32, DELLN},
           {LOGO1, LOGO2, LOGO3, LOGO4, LWHL41, LWHL42, DELLN},
           {LOGO1, LOGO2, LOGO3, LOGO4, LWHL51, LWHL52, DELLN},
           {LOGO1, LOGO2, LOGO3, LOGO4, LWHL61, LWHL62, DELLN}};

    static char *coal[LOGOHEIGHT + 1]
        = {LCOAL1, LCOAL2, LCOAL3, LCOAL4, LCOAL5, LCOAL6, DELLN};

    static char *car[LOGOHEIGHT + 1]
        = {LCAR1, LCAR2, LCAR3, LCAR4, LCAR5, LCAR6, DELLN};

    if (NUMBER < 0)
        NUMBER = 2;

    int i, j, y, py = 0;
    int LOGOLENGTH = 42 + 21*NUMBER;

    if (x < - LOGOLENGTH)  return ERR;
    y = LINES / 2 - 3;

    if (FLY == 1) {
        y = (x / 6) + LINES - (COLS / 6) - LOGOHEIGHT;
        py = 2;
    }
    for (i = 0; i <= LOGOHEIGHT; ++i) {
        my_mvaddstr(y + i, x, sl[(LOGOLENGTH + x) / 3 % LOGOPATTERNS][i]);
        my_mvaddstr(y + i + py, x + 21, coal[i]);
        for (j = 2; j <= NUMBER + 1; ++j)
            my_mvaddstr(y + i + py*j, x + 21*j, car[i]);
    }
    if (ACCIDENT == 1) {
        add_man(y + 1, x + 14);
        for (j = 2; j <= NUMBER + 1; ++j) {
            add_man(y + 1 + py*j, x + 3 + 21*j);  add_man(y + 1 + py*j, x + 11 + 21*j);
        }
    }
    add_smoke(y - 1, x + LOGOFUNNEL);
    return OK;
}


int add_D51(int x)
{
    static char *d51[D51PATTERNS][D51HEIGHT + 1]
        = {{D51STR1, D51STR2, D51STR3, D51STR4, D51STR5, D51STR6, D51STR7,
            D51WHL11, D51WHL12, D51WHL13, D51DEL},
           {D51STR1, D51STR2, D51STR3, D51STR4, D51STR5, D51STR6, D51STR7,
            D51WHL21, D51WHL22, D51WHL23, D51DEL},
           {D51STR1, D51STR2, D51STR3, D51STR4, D51STR5, D51STR6, D51STR7,
            D51WHL31, D51WHL32, D51WHL33, D51DEL},
           {D51STR1, D51STR2, D51STR3, D51STR4, D51STR5, D51STR6, D51STR7,
            D51WHL41, D51WHL42, D51WHL43, D51DEL},
           {D51STR1, D51STR2, D51STR3, D51STR4, D51STR5, D51STR6, D51STR7,
            D51WHL51, D51WHL52, D51WHL53, D51DEL},
           {D51STR1, D51STR2, D51STR3, D51STR4, D51STR5, D51STR6, D51STR7,
            D51WHL61, D51WHL62, D51WHL63, D51DEL}};
    static char *coal[D51HEIGHT + 1]
        = {COAL01, COAL02, COAL03, COAL04, COAL05,
           COAL06, COAL07, COAL08, COAL09, COAL10, COALDEL};

    if (NUMBER < 0)
        NUMBER = 1;

    int y, i, j, dy = 0;
    int D51LENGTH = 54 + 29*NUMBER;

    if (x < - D51LENGTH)  return ERR;
    y = LINES / 2 - 5;

    if (FLY == 1) {
        y = (x / 7) + LINES - (COLS / 7) - D51HEIGHT;
        dy = 1;
    }
    for (i = 0; i <= D51HEIGHT; ++i) {
        my_mvaddstr(y + i, x, d51[(D51LENGTH + x) % D51PATTERNS][i]);
        for (j = 1; j <= NUMBER; ++j)
            my_mvaddstr(y + i + dy*j, x + 24 + 29*j, coal[i]);
    }
    if (ACCIDENT == 1) {
        add_man(y + 2, x + 43);
        add_man(y + 2, x + 47);
    }
    add_smoke(y - 1, x + D51FUNNEL);
    return OK;
}

int add_TGV(int x)
{
    static char *tgv[TGVPATTERNS][TGVHEIGHT + 1]
      = {{TGVSTR0, TGVSTR1, TGVSTR2, TGVSTR3, TGVSTR4, TGVSTR5, TGVSTR6,
            TGVWHL1, TGVDEL},
           {TGVSTR0, TGVSTR1, TGVSTR2, TGVSTR3, TGVSTR4, TGVSTR5, TGVSTR6,
            TGVWHL2, TGVDEL}};
    static char *vagoon[TGVHEIGHT + 1]
        = {TGVVAG0, TGVVAG1, TGVVAG2, TGVVAG3, TGVVAG4, TGVVAG5, TGVVAG6, TGVVAG7, TGVDEL};

    int y, i, dy = 0;

    if (x < - TGVLENGTH)  return ERR;
    y = LINES / 2 - 5;

    if (FLY == 1) {
        y = (x / 7) + LINES - (COLS / 7) - TGVHEIGHT;
        dy = 1;
    }
    attron(COLOR_PAIR(2));
    for (i = 0; i <= TGVHEIGHT; ++i) {
        my_mvaddstr(y + i, x, tgv[((TGVLENGTH + x) / 2) % TGVPATTERNS][i]);
        my_mvaddstr(y + i + dy, x + 55, vagoon[i]);
    }
    attroff(COLOR_PAIR(2));

    if (ACCIDENT == 1) {
        add_man(y + 2, x + 14);

        add_man(y + dy + 3, x + 85);
        add_man(y + dy + 3, x + 90);
        add_man(y + dy + 3, x + 95);
        add_man(y + dy + 3, x + 100);
    }
    return OK;
}

void add_meow(int y, int x)
#define SMOKEPTNS        16
{
    static struct smokes {
        int y, x;
        int ptrn, kind;
    } S[1000];
    static int sum = 0;
    static char *Smoke[2][SMOKEPTNS]
        = {{"(   )", "(    )", "(    )", "(   )", "(  )",
            "(  )" , "( )"   , "( )"   , "()"   , "()"  ,
            "r"    , "r"     , "r"     , "r"    , "r"   ,
            " "                                          },
           {"(MEW)", "(MEOW)", "(MRRR)", "(MR!)", "(MR)",
            "(mr)" , "(r)"   , "(r)"   , "mr"   , "rr"  ,
            "r"    , "!"     , "."     , "!"    , "."   ,
            " "                                          }};
    static char *Eraser[SMOKEPTNS]
        =  {"     ", "      ", "      ", "     ", "    ",
            "    " , "   "   , "   "   , "  "   , "  "  ,
            " "    , " "     , " "     , " "    , " "   ,
            " "                                          };
    static int dy[SMOKEPTNS] = { 2,  1, 1, 1, 0, 0, 0, 0, 0, 0,
                                 0,  0, 0, 0, 0, 0             };
    static int dx[SMOKEPTNS] = {-2, -1, 0, 1, 1, 1, 1, 1, 2, 2,
                                 2,  2, 2, 3, 3, 3             };
    int i;

    if (DISCO && (x + INT_MAX/2) % 4 == 2)
        attron(COLOR_PAIR((x + INT_MAX/2) / 16 % 4 + 1));
    if (x % 4 == 0) {
        for (i = 0; i < sum; ++i) {
            my_mvaddstr(S[i].y, S[i].x, Eraser[S[i].ptrn]);
            S[i].y    -= dy[S[i].ptrn];
            S[i].x    += dx[S[i].ptrn];
            S[i].ptrn += (S[i].ptrn < SMOKEPTNS - 1) ? 1 : 0;
            my_mvaddstr(S[i].y, S[i].x, Smoke[S[i].kind][S[i].ptrn]);
        }
        my_mvaddstr(y, x, Smoke[sum % 2][0]);
        S[sum].y = y;    S[sum].x = x;
        S[sum].ptrn = 0; S[sum].kind = sum % 2;
        sum ++;
    }
}

int add_CAT(int x)
{
    static char *sl[CATPATTERNS][CATHEIGHT + 1]
        = {{CAT001, CAT0, CAT1, CAT2, CAT3, CAT4, CATLWHL11, CATLWHL12, CATDEL},
           {CAT002, CAT02, CAT12, CAT2, CAT3, CAT4, CATLWHL21, CATLWHL22, CATDEL},
           {CAT001, CAT0, CAT1, CAT2, CAT3, CAT4, CATLWHL11, CATLWHL12, CATDEL},
           {CAT002, CAT02, CAT12, CAT2, CAT3, CAT4, CATLWHL21, CATLWHL22, CATDEL},
           {CAT001, CAT0, CAT1, CAT2, CAT3, CAT4, CATLWHL11, CATLWHL12, CATDEL},
           {CAT002, CAT02, CAT12, CAT2, CAT3, CAT4, CATLWHL21, CATLWHL22, CATDEL}};

    if (NUMBER < 0)
        NUMBER = 2;

    int i, j, y, py = 0;
    int CATLENGTH = 42 + 21*NUMBER + 100;

    if (x < - CATLENGTH)  return ERR;
    y = LINES / 2 - 3;

    if (FLY == 1) {
        y = (x / 6) + LINES - (COLS / 6) - CATHEIGHT;
        py = 2;
    }
    for (i = 0; i <= CATHEIGHT; ++i) {
        my_mvaddstr(y + i, x, sl[(CATLENGTH + x) / 3 % CATPATTERNS][i]);
        my_mvaddstr(y + i + py, x + 24, CATDEL);
    }

    add_meow(y - 1, x + CATFUNNEL);
    return OK;
}

int add_C51(int x)
{
    static char *c51[C51PATTERNS][C51HEIGHT + 1]
        = {{C51STR1, C51STR2, C51STR3, C51STR4, C51STR5, C51STR6, C51STR7,
            C51WH11, C51WH12, C51WH13, C51WH14, C51DEL},
           {C51STR1, C51STR2, C51STR3, C51STR4, C51STR5, C51STR6, C51STR7,
            C51WH21, C51WH22, C51WH23, C51WH24, C51DEL},
           {C51STR1, C51STR2, C51STR3, C51STR4, C51STR5, C51STR6, C51STR7,
            C51WH31, C51WH32, C51WH33, C51WH34, C51DEL},
           {C51STR1, C51STR2, C51STR3, C51STR4, C51STR5, C51STR6, C51STR7,
            C51WH41, C51WH42, C51WH43, C51WH44, C51DEL},
           {C51STR1, C51STR2, C51STR3, C51STR4, C51STR5, C51STR6, C51STR7,
            C51WH51, C51WH52, C51WH53, C51WH54, C51DEL},
           {C51STR1, C51STR2, C51STR3, C51STR4, C51STR5, C51STR6, C51STR7,
            C51WH61, C51WH62, C51WH63, C51WH64, C51DEL}};
    static char *coal[C51HEIGHT + 1]
        = {COALDEL, COAL01, COAL02, COAL03, COAL04, COAL05,
           COAL06, COAL07, COAL08, COAL09, COAL10, COALDEL};

    if (NUMBER < 0)
        NUMBER = 1;

    int y, i, j, dy = 0;
    int C51LENGTH = 58 + 29*NUMBER;

    if (x < - C51LENGTH)  return ERR;
    y = LINES / 2 - 5;

    if (FLY == 1) {
        y = (x / 7) + LINES - (COLS / 7) - C51HEIGHT;
        dy = 1;
    }
    for (i = 0; i <= C51HEIGHT; ++i) {
        my_mvaddstr(y + i, x, c51[(C51LENGTH + x) % C51PATTERNS][i]);
        for (j = 1; j <= NUMBER; j++)
            my_mvaddstr(y + i + dy*j, x + 26 + 29*j, coal[i]);
    }
    if (ACCIDENT == 1) {
        add_man(y + 3, x + 45);
        add_man(y + 3, x + 49);
    }
    add_smoke(y - 1, x + C51FUNNEL);
    return OK;
}


void add_man(int y, int x)
{
    static char *man[2][2] = {{"", "(O)"}, {"Help!", "\\O/"}};
    int i;

    for (i = 0; i < 2; ++i) {
        my_mvaddstr(y + i, x, man[(42 + 21*NUMBER + x) / 12 % 2][i]);
    }
}


void add_smoke(int y, int x)
#define SMOKEPTNS        16
{
    static struct smokes {
        int y, x;
        int ptrn, kind;
    } S[1000];
    static int sum = 0;
    static char *Smoke[2][SMOKEPTNS]
        = {{"(   )", "(    )", "(    )", "(   )", "(  )",
            "(  )" , "( )"   , "( )"   , "()"   , "()"  ,
            "O"    , "O"     , "O"     , "O"    , "O"   ,
            " "                                          },
           {"(@@@)", "(@@@@)", "(@@@@)", "(@@@)", "(@@)",
            "(@@)" , "(@)"   , "(@)"   , "@@"   , "@@"  ,
            "@"    , "@"     , "@"     , "@"    , "@"   ,
            " "                                          }};
    static char *Eraser[SMOKEPTNS]
        =  {"     ", "      ", "      ", "     ", "    ",
            "    " , "   "   , "   "   , "  "   , "  "  ,
            " "    , " "     , " "     , " "    , " "   ,
            " "                                          };
    static int dy[SMOKEPTNS] = { 2,  1, 1, 1, 0, 0, 0, 0, 0, 0,
                                 0,  0, 0, 0, 0, 0             };
    static int dx[SMOKEPTNS] = {-2, -1, 0, 1, 1, 1, 1, 1, 2, 2,
                                 2,  2, 2, 3, 3, 3             };
    int i;

    if (DISCO && (x + INT_MAX/2) % 4 == 2)
        attron(COLOR_PAIR((x + INT_MAX/2) / 16 % 4 + 1));
    if (x % 4 == 0) {
        for (i = 0; i < sum; ++i) {
            my_mvaddstr(S[i].y, S[i].x, Eraser[S[i].ptrn]);
            S[i].y    -= dy[S[i].ptrn];
            S[i].x    += dx[S[i].ptrn];
            S[i].ptrn += (S[i].ptrn < SMOKEPTNS - 1) ? 1 : 0;
            my_mvaddstr(S[i].y, S[i].x, Smoke[S[i].kind][S[i].ptrn]);
        }
        my_mvaddstr(y, x, Smoke[sum % 2][0]);
        S[sum].y = y;    S[sum].x = x;
        S[sum].ptrn = 0; S[sum].kind = sum % 2;
        sum ++;
    }
}
