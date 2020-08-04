#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

#define REPORTING 1
#define REPORT if(REPORTING)

#define DEBUGGING 1
#define DEBUG_COND(CODE) if((DEBUGGING == 2) || ((DEBUGGING == 1) && (CODE == 1)))
#define DEBUG(CODE,ARGS...) DEBUG_COND(CODE) PRINT(ARGS)

#define DEBUG_SEARCH    0
#define DEBUG_SET       0
#define DEBUG_BACKTRACK 1

FILE *fp;
#define INIT_FILE 0
#define FILENAME "Relatorio2.txt"
#define PRINT(ARGS...) fprintf(fp,ARGS)
#define NLINE PRINT("\n")

void init_otput()
{
    fp = stdout;
    if(INIT_FILE) fp = fopen(FILENAME,"w");
    setbuf(fp,NULL);
}

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
    poss  psb;
    count cnt;
} sudoku;

void sudoku_init(sudoku *sdk)
{
    sdk->tot = SIZE_4;
    memset(sdk->tbl,0,sizeof(table));
    memset(sdk->psb,1,sizeof(poss));
    memset(sdk->cnt,SIZE_2,sizeof(count));
}



#define DEBUG_POSITION(I,J,K) if(i == I) if(j == J) if(k == K)

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

void table_display_two_contrast(table tbl1, table tbl2, char *color)
{
    LOOP_1(i)
    {
        NLINE;
        LOOP_1(k)
        {
            NLINE;
            line_display(tbl1[COMPOSE(i,k)]);
            PRINT("\t");
            LOOP_1(j)
            {
                PRINT(" ");
                LOOP_1(l)
                {
                    char var = tbl2[COMPOSE(i,k)][COMPOSE(j,l)];
                    if(var == tbl1[COMPOSE(i,k)][COMPOSE(j,l)]) PRINT(" %d", var);
                    else PRINT_COLOR(color," %d", var);
                }
            }
        }
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
    NLINE;
    PRINT("\nPossibilities:");
    poss_display(sdk.psb);
    PRINT("\nCounts:");
    count_display(sdk.cnt);
    PRINT("\nTable:");
    table_display(sdk.tbl);
    PRINT("\nUnknown: %d\n",sdk.tot);
}



char search_line(char *p, char jump, char i)
{
    if(i<0) return i;
    if(*p == 1) return i;
    return search_line(p - jump, jump, i - 1);
}

void search_square(char *p, char index[2], char i0)
{
    DEBUG(DEBUG_SEARCH,"\nSearch square %d",i0);
    REPORT if(i0<0) error("INVALID SEARCH LAST SQUARE");

    char i1;
    i1 = search_line(p,SIZE_2,LIM_1);
    DEBUG(DEBUG_SEARCH,"\tfound: % d",i1);
    if(i1<0)
    {
        search_square(p-SIZE_4,index,i0-1);
        return;
    }

    index[0] = i0;
    index[1] = i1;
}




int sudoku_position_set(sudoku *sdk, char i, char j, char k);

int sudoku_position_clear(sudoku *sdk, char i, char j, char k)
{
    DEBUG(DEBUG_SET,"\n\t\t\tclear [%d][%d][%d]",i,j,k);

    if(sdk->psb[i][j][k] == 2) return 0;

    if(sdk->psb[i][j][k] == 0)
    {
        DEBUG(DEBUG_SET,"\tnext");
        return 1;
    }

    if(sdk->cnt[CNT_ACCESS_0(i,j,k)] == 1) return 0;
    if(sdk->cnt[CNT_ACCESS_1(i,j,k)] == 1) return 0;
    if(sdk->cnt[CNT_ACCESS_2(i,j,k)] == 1) return 0;
    if(sdk->cnt[CNT_ACCESS_3(i,j,k)] == 1) return 0;

    sdk->psb[i][j][k] = 0;

    (sdk->cnt[CNT_ACCESS_0(i,j,k)])--;
    (sdk->cnt[CNT_ACCESS_1(i,j,k)])--;
    (sdk->cnt[CNT_ACCESS_2(i,j,k)])--;
    (sdk->cnt[CNT_ACCESS_3(i,j,k)])--;

    if(sdk->cnt[CNT_ACCESS_0(i,j,k)] == 1)
    {
        DEBUG(DEBUG_SET,"\tfound 0");

        int k1 = search_line(&sdk->psb[i][j][LIM_2],1,LIM_2);

        REPORT if(sdk->psb[i][j][k1] != 1) error("INVALID FINDING 0");

        if(!sudoku_position_set(sdk,i,j,k1)) return 0;
    }

    if(sdk->cnt[CNT_ACCESS_1(i,j,k)] == 1)
    {
        DEBUG(DEBUG_SET,"\tfound 1");

        int j1 = search_line(&sdk->psb[i][LIM_2][k],SIZE_2,LIM_2);

        REPORT if(sdk->psb[i][j1][k] != 1) error("INVALID FINDING 1");

        if(!sudoku_position_set(sdk,i,j1,k)) return 0;
    }

    if(sdk->cnt[CNT_ACCESS_2(i,j,k)] == 1)
    {
        DEBUG(DEBUG_SET,"\tfound 2");

        int i1 = search_line(&sdk->psb[LIM_2][j][k],SIZE_4,LIM_2);

        REPORT if(sdk->psb[i1][j][k] != 1) error("INVALID FINDING 2");

        if(!sudoku_position_set(sdk,i1,j,k)) return 0;
    }

    if(sdk->cnt[CNT_ACCESS_3(i,j,k)] == 1)
    {
        DEBUG(DEBUG_SET,"\tfound 3");

        char index[2];
        search_square(&sdk->psb[MIX_LOW(i,LIM_1)][MIX_LOW(j,LIM_1)][k],index,LIM_1);

        REPORT if(sdk->psb[MIX_LOW(i,index[0])][MIX_LOW(j,index[1])][k] != 1) error("INVALID FINDING 3");

        if(!sudoku_position_set(sdk,MIX_LOW(i,index[0]),MIX_LOW(j,index[1]),k)) return 0;
    }

    return 1;
}

int sudoku_position_set(sudoku *sdk, char i, char j, char k)
{
    DEBUG(DEBUG_SET,"\n\t\tSET [%d][%d][%d]",i,j,k);

    if(sdk->tbl[i][j] != 0) return 0;

    sdk->cnt[CNT_ACCESS_0(i,j,k)] = -1;
    sdk->cnt[CNT_ACCESS_1(i,j,k)] = -1;
    sdk->cnt[CNT_ACCESS_2(i,j,k)] = -1;
    sdk->cnt[CNT_ACCESS_3(i,j,k)] = -1;

    REPORT if(sdk->tot == 0) error("INVALID TOTAL");

    (sdk->tot)--;
    sdk->tbl[i][j] = k+1;

    LOOP_2(l)
    {
        if(!sudoku_position_clear(sdk,i,j,l)) return 0;
        if(!sudoku_position_clear(sdk,i,l,k)) return 0;
        if(!sudoku_position_clear(sdk,l,j,k)) return 0;
        if(!sudoku_position_clear(sdk,MIX_HIGH(i,l),MIX_LOW(j,l),k)) return 0;
        DEBUG(DEBUG_SET,"\n\t\t\t------------");
    }
    DEBUG(DEBUG_SET,"\n\t\tEND [%d][%d][%d]",i,j,k);
    DEBUG(DEBUG_SET,"\n\t\t\t------------");

    sdk->cnt[CNT_ACCESS_0(i,j,k)] = 0;
    sdk->cnt[CNT_ACCESS_1(i,j,k)] = 0;
    sdk->cnt[CNT_ACCESS_2(i,j,k)] = 0;
    sdk->cnt[CNT_ACCESS_3(i,j,k)] = 0;

    sdk->psb[i][j][k] = 2;

    return 1;
}



typedef unsigned long long Long;


#define ADVANCE(I,J) (I+(J+1)/SIZE_2),((J+1)%SIZE_2)

int sudoku_backtrack(sudoku *sdk, char i, char j);

int sudoku_backtrack_assert(sudoku *sdk, char i, char j, char k)
{
    DEBUG(DEBUG_BACKTRACK,"\n\tASSERT [%d][%d][%d]",i,j,k);

    if(sdk->psb[i][j][k] == 0)
    {
        DEBUG(DEBUG_BACKTRACK,"\tNEXT");
        return sudoku_backtrack_assert(sdk,i,j,k+1);
    }

    sudoku sdktmp;
    memcpy(&sdktmp,sdk,sizeof(sudoku));
    Long resset = 0;
    if(sudoku_position_set(&sdktmp,i,j,k))
    {
        DEBUG(DEBUG_BACKTRACK,"\tASSERTED");
        if(sdktmp.tot == 0)
            resset = 1;
        else
            resset = sudoku_backtrack(&sdktmp, ADVANCE(i, j));
    }
    else
        DEBUG(DEBUG_BACKTRACK,"\tIMPOSSIBLE");

    Long resclear = 0;
    DEBUG(DEBUG_BACKTRACK,"\n\tREJECT [%d][%d][%d]",i,j,k);
    memcpy(&sdktmp,sdk,sizeof(sudoku));
    if(sudoku_position_clear(&sdktmp,i,j,k))
    {
        DEBUG(DEBUG_BACKTRACK,"\tREJECTED");
        if(sdktmp.tot == 0)
            resclear = 1;
        else if (sdktmp.tbl[i][j] != 0)
            resclear = sudoku_backtrack(&sdktmp, ADVANCE(i, j));
        else
            resclear = sudoku_backtrack_assert(&sdktmp,i,j,k+1);
    }
    else
        DEBUG(DEBUG_BACKTRACK,"\tIMPOSSIBLE");

    DEBUG(DEBUG_BACKTRACK,"\n\tOUT [%d][%d][%d]",i,j,k);
    return resset + resclear;
}

int sudoku_backtrack(sudoku *sdk, char i, char j)
{
    DEBUG(DEBUG_BACKTRACK,"\nBACKTRACK [%d][%d]",i,j);

    if(sdk->tbl[i][j] != 0)
        return sudoku_backtrack(sdk,ADVANCE(i,j));

    return sudoku_backtrack_assert(sdk,i,j,0);
}



void sudoku_init_0(sudoku *sdk, int lim)
{
    int cnt = 0;
    sudoku_init(sdk);
    if(cnt++ < lim) if(!sudoku_position_set(sdk,0,0,3)) DEBUG(DEBUG_SET,"\nOPORRA"); DEBUG_COND(DEBUG_SET) sudoku_display(*sdk);
    if(cnt++ < lim) if(!sudoku_position_set(sdk,1,5,8)) DEBUG(DEBUG_SET,"\nOPORRA"); DEBUG_COND(DEBUG_SET) sudoku_display(*sdk);
    if(cnt++ < lim) if(!sudoku_position_set(sdk,2,6,6)) DEBUG(DEBUG_SET,"\nOPORRA"); DEBUG_COND(DEBUG_SET) sudoku_display(*sdk);
    if(cnt++ < lim) if(!sudoku_position_set(sdk,2,7,7)) DEBUG(DEBUG_SET,"\nOPORRA"); DEBUG_COND(DEBUG_SET) sudoku_display(*sdk);
    if(cnt++ < lim) if(!sudoku_position_set(sdk,2,8,4)) DEBUG(DEBUG_SET,"\nOPORRA"); DEBUG_COND(DEBUG_SET) sudoku_display(*sdk);
    if(cnt++ < lim) if(!sudoku_position_set(sdk,3,2,6)) DEBUG(DEBUG_SET,"\nOPORRA"); DEBUG_COND(DEBUG_SET) sudoku_display(*sdk);
    if(cnt++ < lim) if(!sudoku_position_set(sdk,3,4,3)) DEBUG(DEBUG_SET,"\nOPORRA"); DEBUG_COND(DEBUG_SET) sudoku_display(*sdk);
    if(cnt++ < lim) if(!sudoku_position_set(sdk,3,5,7)) DEBUG(DEBUG_SET,"\nOPORRA"); DEBUG_COND(DEBUG_SET) sudoku_display(*sdk);
    if(cnt++ < lim) if(!sudoku_position_set(sdk,3,7,4)) DEBUG(DEBUG_SET,"\nOPORRA"); DEBUG_COND(DEBUG_SET) sudoku_display(*sdk);
    if(cnt++ < lim) if(!sudoku_position_set(sdk,4,2,0)) DEBUG(DEBUG_SET,"\nOPORRA"); DEBUG_COND(DEBUG_SET) sudoku_display(*sdk);
    if(cnt++ < lim) if(!sudoku_position_set(sdk,4,3,2)) DEBUG(DEBUG_SET,"\nOPORRA"); DEBUG_COND(DEBUG_SET) sudoku_display(*sdk);
    if(cnt++ < lim) if(!sudoku_position_set(sdk,5,2,5)) DEBUG(DEBUG_SET,"\nOPORRA"); DEBUG_COND(DEBUG_SET) sudoku_display(*sdk);
    if(cnt++ < lim) if(!sudoku_position_set(sdk,5,4,6)) DEBUG(DEBUG_SET,"\nOPORRA"); DEBUG_COND(DEBUG_SET) sudoku_display(*sdk);
    if(cnt++ < lim) if(!sudoku_position_set(sdk,6,0,7)) DEBUG(DEBUG_SET,"\nOPORRA"); DEBUG_COND(DEBUG_SET) sudoku_display(*sdk);
    if(cnt++ < lim) if(!sudoku_position_set(sdk,6,1,5)) DEBUG(DEBUG_SET,"\nOPORRA"); DEBUG_COND(DEBUG_SET) sudoku_display(*sdk);
    if(cnt++ < lim) if(!sudoku_position_set(sdk,6,6,8)) DEBUG(DEBUG_SET,"\nOPORRA"); DEBUG_COND(DEBUG_SET) sudoku_display(*sdk);
    if(cnt++ < lim) if(!sudoku_position_set(sdk,6,8,2)) DEBUG(DEBUG_SET,"\nOPORRA"); DEBUG_COND(DEBUG_SET) sudoku_display(*sdk);
    if(cnt++ < lim) if(!sudoku_position_set(sdk,7,0,6)) DEBUG(DEBUG_SET,"\nOPORRA"); DEBUG_COND(DEBUG_SET) sudoku_display(*sdk);
    if(cnt++ < lim) if(!sudoku_position_set(sdk,7,5,4)) DEBUG(DEBUG_SET,"\nOPORRA"); DEBUG_COND(DEBUG_SET) sudoku_display(*sdk);
    if(cnt++ < lim) if(!sudoku_position_set(sdk,7,7,5)) DEBUG(DEBUG_SET,"\nOPORRA"); DEBUG_COND(DEBUG_SET) sudoku_display(*sdk);
    if(cnt++ < lim) if(!sudoku_position_set(sdk,7,8,1)) DEBUG(DEBUG_SET,"\nOPORRA"); DEBUG_COND(DEBUG_SET) sudoku_display(*sdk);
    if(cnt++ < lim) if(!sudoku_position_set(sdk,8,2,2)) DEBUG(DEBUG_SET,"\nOPORRA"); DEBUG_COND(DEBUG_SET) sudoku_display(*sdk);
    if(cnt++ < lim) if(!sudoku_position_set(sdk,8,3,6)) DEBUG(DEBUG_SET,"\nOPORRA"); DEBUG_COND(DEBUG_SET) sudoku_display(*sdk);
}

void sudoku_init_1(sudoku *sdk)
{
    sudoku_init(sdk);
    if(!sudoku_position_set(sdk,0,2,0)) DEBUG(DEBUG_SET,"\nOPORRA");DEBUG_COND(DEBUG_SET)sudoku_display(*sdk);
    if(!sudoku_position_set(sdk,0,3,3)) DEBUG(DEBUG_SET,"\nOPORRA");DEBUG_COND(DEBUG_SET)sudoku_display(*sdk);
    if(!sudoku_position_set(sdk,0,4,6)) DEBUG(DEBUG_SET,"\nOPORRA");DEBUG_COND(DEBUG_SET)sudoku_display(*sdk);
    if(!sudoku_position_set(sdk,0,5,7)) DEBUG(DEBUG_SET,"\nOPORRA");DEBUG_COND(DEBUG_SET)sudoku_display(*sdk);
    if(!sudoku_position_set(sdk,0,8,8)) DEBUG(DEBUG_SET,"\nOPORRA");DEBUG_COND(DEBUG_SET)sudoku_display(*sdk);
    if(!sudoku_position_set(sdk,1,7,5)) DEBUG(DEBUG_SET,"\nOPORRA");DEBUG_COND(DEBUG_SET)sudoku_display(*sdk);
    if(!sudoku_position_set(sdk,1,8,2)) DEBUG(DEBUG_SET,"\nOPORRA");DEBUG_COND(DEBUG_SET)sudoku_display(*sdk);
    if(!sudoku_position_set(sdk,2,6,3)) DEBUG(DEBUG_SET,"\nOPORRA");DEBUG_COND(DEBUG_SET)sudoku_display(*sdk);
    if(!sudoku_position_set(sdk,3,0,6)) DEBUG(DEBUG_SET,"\nOPORRA");DEBUG_COND(DEBUG_SET)sudoku_display(*sdk);
    if(!sudoku_position_set(sdk,3,1,7)) DEBUG(DEBUG_SET,"\nOPORRA");DEBUG_COND(DEBUG_SET)sudoku_display(*sdk);
    if(!sudoku_position_set(sdk,3,4,4)) DEBUG(DEBUG_SET,"\nOPORRA");DEBUG_COND(DEBUG_SET)sudoku_display(*sdk);
    if(!sudoku_position_set(sdk,3,5,8)) DEBUG(DEBUG_SET,"\nOPORRA");DEBUG_COND(DEBUG_SET)sudoku_display(*sdk);
    if(!sudoku_position_set(sdk,4,4,7)) DEBUG(DEBUG_SET,"\nOPORRA");DEBUG_COND(DEBUG_SET)sudoku_display(*sdk);
    if(!sudoku_position_set(sdk,4,6,2)) DEBUG(DEBUG_SET,"\nOPORRA");DEBUG_COND(DEBUG_SET)sudoku_display(*sdk);
    if(!sudoku_position_set(sdk,5,1,0)) DEBUG(DEBUG_SET,"\nOPORRA");DEBUG_COND(DEBUG_SET)sudoku_display(*sdk);
    if(!sudoku_position_set(sdk,5,2,2)) DEBUG(DEBUG_SET,"\nOPORRA");DEBUG_COND(DEBUG_SET)sudoku_display(*sdk);
    if(!sudoku_position_set(sdk,6,5,2)) DEBUG(DEBUG_SET,"\nOPORRA");DEBUG_COND(DEBUG_SET)sudoku_display(*sdk);
    if(!sudoku_position_set(sdk,6,6,1)) DEBUG(DEBUG_SET,"\nOPORRA");DEBUG_COND(DEBUG_SET)sudoku_display(*sdk);
    if(!sudoku_position_set(sdk,6,8,6)) DEBUG(DEBUG_SET,"\nOPORRA");DEBUG_COND(DEBUG_SET)sudoku_display(*sdk);
    if(!sudoku_position_set(sdk,7,0,4)) DEBUG(DEBUG_SET,"\nOPORRA");DEBUG_COND(DEBUG_SET)sudoku_display(*sdk);
    if(!sudoku_position_set(sdk,7,1,5)) DEBUG(DEBUG_SET,"\nOPORRA");DEBUG_COND(DEBUG_SET)sudoku_display(*sdk);
    if(!sudoku_position_set(sdk,8,1,8)) DEBUG(DEBUG_SET,"\nOPORRA");DEBUG_COND(DEBUG_SET)sudoku_display(*sdk);
    if(!sudoku_position_set(sdk,8,3,0)) DEBUG(DEBUG_SET,"\nOPORRA");DEBUG_COND(DEBUG_SET)sudoku_display(*sdk);
}



void test_search()
{
    table tbl =    {{0,0,0,0,0,0,0,0,0},
                    {0,0,0,0,0,0,0,0,0},
                    {1,0,0,0,0,0,0,0,0},
                    {0,0,0,0,0,0,0,0,0},
                    {0,0,0,0,0,0,0,0,0},
                    {0,0,0,0,0,0,0,0,0},
                    {0,0,0,0,0,0,0,0,0},
                    {0,0,0,0,0,0,0,0,0},
                    {0,0,0,0,0,0,0,0,0}};
    int i = search_line(&tbl[2][LIM_2],1,LIM_2);
    if(i != 0) error("TESTE SEARCH LINE 1 FAILLED");

    i = search_line(&tbl[LIM_2][0],SIZE_2,LIM_2);
    if(i != 2) error("TESTE SEARCH LINE 2 FAILLED");

    char index[2];
    search_square(&tbl[MIX_LOW(0,LIM_1)][MIX_LOW(0,LIM_1)],index,LIM_1);
    if(index[0] != 2)
    if(index[1] != 0)
        error("TESTE SEARCH LINE 3 FAILLED");

    PRINT("\nTEST SEARCH SUSSECFUL");
}

void test()
{
    PRINT("\nHERE");
    test_search();
}



int main()
{
    init_otput();

    sudoku sdk;
    for(int i=23; i>=22; i--)
    {
        PRINT("\nSet %d: ",i);
        sudoku_init_0(&sdk,i);
        Long tot = sudoku_backtrack(&sdk,0,0);
        PRINT("\t%llu",tot);
    }

    return 0;
}
