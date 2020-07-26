#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#define SIZE_1 3
#define SIZE_2 (SIZE_1*SIZE_1)
#define SIZE_4 (SIZE_2*SIZE_2)
#define LIM_1 (SIZE_1-1)
#define LIM_2 (SIZE_2-1)
#define COMPOSE(NUM1,NUM2) (NUM1*SIZE_1+NUM2)
#define HIGH(NUM) (NUM/SIZE_1)
#define LOW(NUM) (NUM%SIZE_1)
#define MIX_LOW(NUM1,NUM2) COMPOSE(HIGH(NUM1),LOW(NUM2))
#define MIX_HIGH(NUM1,NUM2) COMPOSE(HIGH(NUM1),HIGH(NUM2))
#define LOOP(NAME,LIM) for(char NAME=0; NAME<LIM; NAME++)
#define LOOP_1(NAME) LOOP(NAME,SIZE_1)
#define LOOP_2(NAME) LOOP(NAME,SIZE_2)

#define REPORTING 0
#define REPORT if(REPORTING)

FILE *fp;
#define INIT_FILE 0
#define FILENAME "Relatorio2.txt"
#define PRINT(ARGS...) fprintf(fp,ARGS)
#define NLINE PRINT("\n")

#define OPEN do{
#define CLOSE }while(0)

#define DEFAULT     "\033[0m"
#define RED         "\033[0;31m"
#define YELLOW      "\033[0;33m"
#define BLUE        "\033[0;34m"
#define MAGENTA     "\033[0;35m"
#define CIANO       "\033[0;36m"
#define COLOR(CODE) PRINT(CODE)
#define PRINT_COLOR(CODE,ARGS...) OPEN COLOR(CODE); PRINT(ARGS); COLOR(DEFAULT); CLOSE

#define DEBUGGING 1
#define DEBUG_COND(CODE) if((DEBUGGING == 2) || ((DEBUGGING == 1) && (CODE == 1)))
#define DEBUG(CODE,ARGS...) DEBUG_COND(CODE) PRINT(ARGS)

#define DEBUG_SET 0
#define DEBUG_FOUND 0
#define DEBUG_BACKTRACK 1

typedef char  line[SIZE_2];
typedef line  table[SIZE_2];
typedef table poss[SIZE_2];
typedef table count[4];

#define CNT_ACCESS_0(I,J,K) 0][I][J
#define CNT_ACCESS_1(I,J,K) 1][I][K
#define CNT_ACCESS_2(I,J,K) 2][J][K
#define CNT_ACCESS_3(I,J,K) 3][MIX_HIGH(I,J)][K



typedef struct Sudoku
{
    int   tot;
    table tbl;
    poss  pos;
    count cnt;
} sudoku;

void sudoku_init(sudoku *sdk)
{
    sdk->tot = SIZE_4;
    memset(sdk->tbl,0,sizeof(table));
    memset(sdk->pos,1,sizeof(poss));
    memset(sdk->cnt,SIZE_2,sizeof(count));
}



void error(char *s)
{
    PRINT("\nERROR | %s",s);
    exit(EXIT_FAILURE);
}

void write_template()
{
    FILE *fp;
    fp = fopen("template.txt","w");
    LOOP_1(i)
    {
        fprintf(fp,"\n");
        LOOP_1(k)
        {
            fprintf(fp,"\n");
            LOOP_1(j)
            {
                fprintf(fp," ");
                LOOP_1(l)
                    fprintf(fp," 0");
            }
        }
    }
    fclose(fp);
}

void line_display(line lne)
{
    LOOP_1(j)
    {
        PRINT(" ");
        LOOP_1(l)
        {
            char var = lne[COMPOSE(j,l)];
            if(var >= 0) PRINT(" %d", var);
            else PRINT_COLOR(RED," %d", -var);
        }
    }
}

void table_display(table tab)
{
    LOOP_1(i)
    {
        LOOP_1(k)
        {
            NLINE;
            line_display(tab[COMPOSE(i,k)]);
        }
        NLINE;
    }
}

void table_display_multiple_allign(table T[], int m)
{
    LOOP_1(i)
    {
        LOOP_1(k)
        {
            PRINT("\n");
            LOOP(j,m)
            {
                line_display(T[j][COMPOSE(i,k)]);
                PRINT("\t");
            }
        }
        NLINE;
    }
}

void table_display_multiple(table T[],int n, int m, char c)
{
    LOOP(i,n/m)
    {
        NLINE;
        LOOP(j,m)
            PRINT("%c: %d\t\t\t\t\t",c,j+i*m);

        table_display_multiple_allign(T+i*m,m);
    }
    if(n%m != 0)
    {
        NLINE;
        LOOP(j,n%m)
            PRINT("%c: %d\t\t\t\t\t",c,j+m*(n/m));
        table_display_multiple_allign(T+m*(n/m),n%m);
    }
}

void poss_display(poss pos)
{
    poss pos1;
    LOOP_2(i) LOOP_2(j) LOOP_2(k)
        pos1[k][i][j] = pos[i][j][k];

    table_display_multiple(pos1,SIZE_2,SIZE_1,'k');
}

void count_display(count cnt)
{
    table_display_multiple(cnt,4,2,'c');
}

void sudoku_display(sudoku sdk)
{
    PRINT("\nUnknown: %d\n",sdk.tot);
    PRINT("\nPossibilities:");
    poss_display(sdk.pos);
    PRINT("\nCounts:");
    count_display(sdk.cnt);
    PRINT("\nTable:");
    table_display(sdk.tbl);
}



char search_last_line(char *p, char jump, char i)
{
    if(i<0) return i;
    if(*p == 1) return i;
    return search_last_line(p-jump,jump,i-1);
}

void search_last_square(char *p, char index[2], char i0)
{
    REPORT if(i0<0) error("INVALID SEARCH LAST SQUARE");
    char i1;
    i1 = search_last_line(p,1,LIM_1);
    if(i1<0)
    {
        search_last_square(p-SIZE_2,index,i0-1);
        return;
    }

    index[0] = i0;
    index[1] = i1;
}




int sudoku_position_set(sudoku *sdk, char i, char j, char k);

int sudoku_position_clear(sudoku *sdk, char i, char j, char k)
{
    DEBUG(DEBUG_SET,"\n\t\t\tCLEAR [%d][%d][%d]",i,j,k);
    REPORT if(sdk->pos[i][j][k] == 2) error("ATTEMPT TO CLEAR POSITION ALREADY SET");

    if(sdk->pos[i][j][k] == 0)
    {
        DEBUG(DEBUG_SET,"\t\tCleared");
        return 1;
    }

    if(sdk->cnt[CNT_ACCESS_0(i,j,k)] == 1) return 0;
    if(sdk->cnt[CNT_ACCESS_1(i,j,k)] == 1) return 0;
    if(sdk->cnt[CNT_ACCESS_2(i,j,k)] == 1) return 0;
    if(sdk->cnt[CNT_ACCESS_3(i,j,k)] == 1) return 0;

    sdk->pos[i][j][k] = 0;

    (sdk->cnt[CNT_ACCESS_0(i,j,k)])--;
    (sdk->cnt[CNT_ACCESS_1(i,j,k)])--;
    (sdk->cnt[CNT_ACCESS_2(i,j,k)])--;
    (sdk->cnt[CNT_ACCESS_3(i,j,k)])--;


    if(sdk->cnt[CNT_ACCESS_0(i,j,k)] == 1)
    {
        DEBUG_COND(DEBUG_FOUND)
        {
            PRINT("\nFind 0");
            count_display(sdk->cnt);
        }

        char k1;
        k1 = search_last_line(&(sdk->pos[i][j][LIM_2]),1,LIM_2);
        DEBUG(DEBUG_FOUND,"\nFound k: %d",k1);
        if(!sudoku_position_set(sdk,i,j,k1)) return 0;
    }
    if(sdk->cnt[CNT_ACCESS_1(i,j,k)] == 1)
    {
        DEBUG_COND(DEBUG_FOUND)
        {
            PRINT("\nFind 1");
            count_display(sdk->cnt);
        }

        char j1;
        j1 = search_last_line(&(sdk->pos[i][LIM_2][k]),SIZE_2,LIM_2);
        DEBUG(DEBUG_FOUND,"\nFound j: %d",j1);
        if(!sudoku_position_set(sdk,i,j1,k)) return 0;
    }
    if(sdk->cnt[CNT_ACCESS_2(i,j,k)] == 1)
    {
        DEBUG_COND(DEBUG_FOUND)
        {
            PRINT("\nFind 2");
            count_display(sdk->cnt);
        }

        char i1;
        i1 = search_last_line(&(sdk->pos[LIM_2][j][k]),SIZE_4,LIM_2);
        DEBUG(DEBUG_FOUND,"\nFound i: %d",i1);
        if(!sudoku_position_set(sdk,i1,j,k)) return 0;
    }
    if(sdk->cnt[CNT_ACCESS_3(i,j,k)] == 1)
    {
        DEBUG_COND(DEBUG_FOUND)
        {
            PRINT("\nFind 3");
            count_display(sdk->cnt);
        }
    }

    return 1;
}

int sudoku_position_set(sudoku *sdk, char i, char j, char k)
{
    DEBUG(DEBUG_SET,"\n\t\tSET [%d][%d][%d]",i,j,k);

    REPORT if(sdk->tbl[i][j] != 0) error("ATTEMPT TO SET POSITION ALREADY SET");

    sdk->cnt[CNT_ACCESS_0(i,j,k)] = -1;
    sdk->cnt[CNT_ACCESS_1(i,j,k)] = -1;
    sdk->cnt[CNT_ACCESS_2(i,j,k)] = -1;
    sdk->cnt[CNT_ACCESS_3(i,j,k)] = -1;

    LOOP_2(l)
    {
        if(!sudoku_position_clear(sdk,i,j,l)) return 0;
        if(!sudoku_position_clear(sdk,i,l,k)) return 0;
        if(!sudoku_position_clear(sdk,l,j,k)) return 0;
        if(!sudoku_position_clear(sdk,MIX_HIGH(i,l),MIX_LOW(j,l),k)) return 0;
        DEBUG(DEBUG_SET,"\n\t\t\t---------");
    }

    sdk->pos[i][j][k] = 2;

    sdk->cnt[CNT_ACCESS_0(i,j,k)] = 0;
    sdk->cnt[CNT_ACCESS_1(i,j,k)] = 0;
    sdk->cnt[CNT_ACCESS_2(i,j,k)] = 0;
    sdk->cnt[CNT_ACCESS_3(i,j,k)] = 0;

    sdk->tbl[i][j] = k+1;

    (sdk->tot)--;

    DEBUG(DEBUG_SET,"\n\t\tEND [%d][%d][%d]",i,j,k);
    return 1;
}




#define ADVANCE(I,J) (I+(J+1)/SIZE_2),((J+1)%SIZE_2)

int sudoku_backtrack(sudoku *sdk, char i, char j);

int sudoku_bactrack_assert(sudoku *sdk, char i, char j, char k)
{
    DEBUG(DEBUG_BACKTRACK,"\n\tAssert [%d][%d][%d]",i,j,k);
    if(sdk->pos[i][j][k] == 0)
    {
        DEBUG(DEBUG_BACKTRACK,"\tNEXT");
        return sudoku_bactrack_assert(sdk,i,j,k+1);
    }

    sudoku sdktmp;
    memcpy(&sdktmp,sdk,sizeof(sudoku));
    if(!sudoku_position_set(&sdktmp,i,j,k))
    {
        DEBUG(DEBUG_BACKTRACK,"\tUNSETABLE");
        if(!sudoku_position_clear(sdk,i,j,k))
        {
            DEBUG(DEBUG_BACKTRACK,"\tUNCLEARABLE");
            return 0;
        }
        if(sdk->tot == 0) return 1;
        if(sdk->tbl[i][j] != 0) return sudoku_backtrack(sdk,ADVANCE(i,j));
        return sudoku_bactrack_assert(sdk,i,j,k+1);
    }

    DEBUG(DEBUG_BACKTRACK,"\tPOSSIBLE");
    if(sdktmp.tot != 0)
    if(!sudoku_backtrack(&sdktmp,ADVANCE(i,j)))
        return sudoku_bactrack_assert(sdk,i,j,k+1);

    memcpy(sdk,&sdktmp,sizeof(sudoku));
    return 1;
}

int sudoku_backtrack(sudoku *sdk, char i, char j)
{
    DEBUG(DEBUG_BACKTRACK,"\nBacktrack [%d][%d]",i,j);
    if(sdk->tbl[i][j] != 0)  return sudoku_backtrack(sdk,ADVANCE(i,j));

    return sudoku_bactrack_assert(sdk,i,j,0);
}



void sudoku_set_all_0(sudoku *sdk)
{
    sudoku_init(sdk);
    if(!sudoku_position_set(sdk, 0, 0, 3)) DEBUG(DEBUG_SET, "\nOPORRA"); DEBUG_COND(DEBUG_SET) sudoku_display(*sdk);
    if(!sudoku_position_set(sdk, 1, 5, 8)) DEBUG(DEBUG_SET, "\nOPORRA"); DEBUG_COND(DEBUG_SET) sudoku_display(*sdk);
    if(!sudoku_position_set(sdk, 2, 6, 6)) DEBUG(DEBUG_SET, "\nOPORRA"); DEBUG_COND(DEBUG_SET) sudoku_display(*sdk);
    if(!sudoku_position_set(sdk, 2, 7, 7)) DEBUG(DEBUG_SET, "\nOPORRA"); DEBUG_COND(DEBUG_SET) sudoku_display(*sdk);
    if(!sudoku_position_set(sdk, 2, 8, 4)) DEBUG(DEBUG_SET, "\nOPORRA"); DEBUG_COND(DEBUG_SET) sudoku_display(*sdk);
    if(!sudoku_position_set(sdk, 3, 2, 6)) DEBUG(DEBUG_SET, "\nOPORRA"); DEBUG_COND(DEBUG_SET) sudoku_display(*sdk);
    if(!sudoku_position_set(sdk, 3, 4, 3)) DEBUG(DEBUG_SET, "\nOPORRA"); DEBUG_COND(DEBUG_SET) sudoku_display(*sdk);
    if(!sudoku_position_set(sdk, 3, 5, 7)) DEBUG(DEBUG_SET, "\nOPORRA"); DEBUG_COND(DEBUG_SET) sudoku_display(*sdk);
    if(!sudoku_position_set(sdk, 3, 7, 4)) DEBUG(DEBUG_SET, "\nOPORRA"); DEBUG_COND(DEBUG_SET) sudoku_display(*sdk);
    if(!sudoku_position_set(sdk, 4, 2, 0)) DEBUG(DEBUG_SET, "\nOPORRA"); DEBUG_COND(DEBUG_SET) sudoku_display(*sdk);
    if(!sudoku_position_set(sdk, 4, 3, 2)) DEBUG(DEBUG_SET, "\nOPORRA"); DEBUG_COND(DEBUG_SET) sudoku_display(*sdk);
    if(!sudoku_position_set(sdk, 5, 2, 5)) DEBUG(DEBUG_SET, "\nOPORRA"); DEBUG_COND(DEBUG_SET) sudoku_display(*sdk);
    if(!sudoku_position_set(sdk, 5, 4, 6)) DEBUG(DEBUG_SET, "\nOPORRA"); DEBUG_COND(DEBUG_SET) sudoku_display(*sdk);
    if(!sudoku_position_set(sdk, 6, 0, 7)) DEBUG(DEBUG_SET, "\nOPORRA"); DEBUG_COND(DEBUG_SET) sudoku_display(*sdk);
    if(!sudoku_position_set(sdk, 6, 1, 5)) DEBUG(DEBUG_SET, "\nOPORRA"); DEBUG_COND(DEBUG_SET) sudoku_display(*sdk);
    if(!sudoku_position_set(sdk, 6, 6, 8)) DEBUG(DEBUG_SET, "\nOPORRA"); DEBUG_COND(DEBUG_SET) sudoku_display(*sdk);
    if(!sudoku_position_set(sdk, 6, 8, 2)) DEBUG(DEBUG_SET, "\nOPORRA"); DEBUG_COND(DEBUG_SET) sudoku_display(*sdk);
    if(!sudoku_position_set(sdk, 7, 0, 6)) DEBUG(DEBUG_SET, "\nOPORRA"); DEBUG_COND(DEBUG_SET) sudoku_display(*sdk);
    if(!sudoku_position_set(sdk, 7, 5, 4)) DEBUG(DEBUG_SET, "\nOPORRA"); DEBUG_COND(DEBUG_SET) sudoku_display(*sdk);
    if(!sudoku_position_set(sdk, 7, 7, 5)) DEBUG(DEBUG_SET, "\nOPORRA"); DEBUG_COND(DEBUG_SET) sudoku_display(*sdk);
    if(!sudoku_position_set(sdk, 7, 8, 1)) DEBUG(DEBUG_SET, "\nOPORRA"); DEBUG_COND(DEBUG_SET) sudoku_display(*sdk);
    if(!sudoku_position_set(sdk, 8, 2, 2)) DEBUG(DEBUG_SET, "\nOPORRA"); DEBUG_COND(DEBUG_SET) sudoku_display(*sdk);
    if(!sudoku_position_set(sdk, 8, 3, 6)) DEBUG(DEBUG_SET, "\nOPORRA"); DEBUG_COND(DEBUG_SET) sudoku_display(*sdk);
    /**/
}

void sudoku_set_all_1(sudoku *sdk)
{
    sudoku_init(sdk);
    if(!sudoku_position_set(sdk,0,2,0)) DEBUG(DEBUG_SET, "\nOPORRA"); DEBUG_COND(DEBUG_SET) sudoku_display(*sdk);
    if(!sudoku_position_set(sdk,0,3,3)) DEBUG(DEBUG_SET, "\nOPORRA"); DEBUG_COND(DEBUG_SET) sudoku_display(*sdk);
    if(!sudoku_position_set(sdk,0,4,6)) DEBUG(DEBUG_SET, "\nOPORRA"); DEBUG_COND(DEBUG_SET) sudoku_display(*sdk);
    if(!sudoku_position_set(sdk,0,5,7)) DEBUG(DEBUG_SET, "\nOPORRA"); DEBUG_COND(DEBUG_SET) sudoku_display(*sdk);
    if(!sudoku_position_set(sdk,0,8,8)) DEBUG(DEBUG_SET, "\nOPORRA"); DEBUG_COND(DEBUG_SET) sudoku_display(*sdk);
    if(!sudoku_position_set(sdk,1,7,5)) DEBUG(DEBUG_SET, "\nOPORRA"); DEBUG_COND(DEBUG_SET) sudoku_display(*sdk);
    if(!sudoku_position_set(sdk,1,8,2)) DEBUG(DEBUG_SET, "\nOPORRA"); DEBUG_COND(DEBUG_SET) sudoku_display(*sdk);
    if(!sudoku_position_set(sdk,2,6,3)) DEBUG(DEBUG_SET, "\nOPORRA"); DEBUG_COND(DEBUG_SET) sudoku_display(*sdk);
    if(!sudoku_position_set(sdk,3,0,6)) DEBUG(DEBUG_SET, "\nOPORRA"); DEBUG_COND(DEBUG_SET) sudoku_display(*sdk);
    if(!sudoku_position_set(sdk,3,1,7)) DEBUG(DEBUG_SET, "\nOPORRA"); DEBUG_COND(DEBUG_SET) sudoku_display(*sdk);
    if(!sudoku_position_set(sdk,3,4,4)) DEBUG(DEBUG_SET, "\nOPORRA"); DEBUG_COND(DEBUG_SET) sudoku_display(*sdk);
    if(!sudoku_position_set(sdk,3,5,8)) DEBUG(DEBUG_SET, "\nOPORRA"); DEBUG_COND(DEBUG_SET) sudoku_display(*sdk);
    if(!sudoku_position_set(sdk,4,4,7)) DEBUG(DEBUG_SET, "\nOPORRA"); DEBUG_COND(DEBUG_SET) sudoku_display(*sdk);
    if(!sudoku_position_set(sdk,4,6,2)) DEBUG(DEBUG_SET, "\nOPORRA"); DEBUG_COND(DEBUG_SET) sudoku_display(*sdk);
    if(!sudoku_position_set(sdk,5,1,0)) DEBUG(DEBUG_SET, "\nOPORRA"); DEBUG_COND(DEBUG_SET) sudoku_display(*sdk);
    if(!sudoku_position_set(sdk,5,2,2)) DEBUG(DEBUG_SET, "\nOPORRA"); DEBUG_COND(DEBUG_SET) sudoku_display(*sdk);
    if(!sudoku_position_set(sdk,6,5,2)) DEBUG(DEBUG_SET, "\nOPORRA"); DEBUG_COND(DEBUG_SET) sudoku_display(*sdk);
    if(!sudoku_position_set(sdk,6,6,1)) DEBUG(DEBUG_SET, "\nOPORRA"); DEBUG_COND(DEBUG_SET) sudoku_display(*sdk);
    if(!sudoku_position_set(sdk,6,8,6)) DEBUG(DEBUG_SET, "\nOPORRA"); DEBUG_COND(DEBUG_SET) sudoku_display(*sdk);
    if(!sudoku_position_set(sdk,7,0,4)) DEBUG(DEBUG_SET, "\nOPORRA"); DEBUG_COND(DEBUG_SET) sudoku_display(*sdk);
    if(!sudoku_position_set(sdk,7,1,5)) DEBUG(DEBUG_SET, "\nOPORRA"); DEBUG_COND(DEBUG_SET) sudoku_display(*sdk);
    if(!sudoku_position_set(sdk,8,1,8)) DEBUG(DEBUG_SET, "\nOPORRA"); DEBUG_COND(DEBUG_SET) sudoku_display(*sdk);
    if(!sudoku_position_set(sdk,8,3,0)) DEBUG(DEBUG_SET, "\nOPORRA"); DEBUG_COND(DEBUG_SET) sudoku_display(*sdk);
    /**/
}



int main()
{
    fp = stdout;
    if(INIT_FILE) fp = fopen(FILENAME,"w");

    sudoku sdk;
    sudoku_set_all_1(&sdk);

    NLINE;NLINE;NLINE;
    table_display(sdk.tbl);

    NLINE;NLINE;NLINE;

    PRINT("\nBacktrack");
    NLINE;
    sudoku_backtrack(&sdk,0,0);
    COLOR(YELLOW);
    table_display(sdk.tbl);
    COLOR(DEFAULT);
    return 0;
}
