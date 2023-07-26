#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <time.h>
#define maxN 150
#define maxM 1000
#define inf INT_MAX
#define time_limit 1.092164 /* 課題説明資料 p.27 の通り，制限時間を定義 */
/* 辺データを格納する構造体 */
struct edge_data
{
    int end1;
    int end2;
    int weight;
};
/* ソート関連の関数 */
void quicksort(int *A, int *ednum, int p, int r);
int partition(int *A, int *ednum, int p, int r);
/* ユニオン・ファインド森の関数 */
void make_set(int *p, int *rank, int x);
int find_set(int *p, int x);
void link(int *p, int *rank, int x, int y);
/* f(U) を求める関数 */
int kruskal(int n, int m, struct edge_data *edge, int *ednum, int *IU, int *T);
/* 他の関数も宣言する */
main(void)
{
    int N, M, K; /* N = 頂点数, M = 辺数 , K = ターミナル数（S の頂点数） */
    struct edge_data edge[maxM];
    /* 必要な変数を宣言する */
    char fname[128];
    FILE *fp;
    clock_t start_t, end_t; /* 時間計測用*/
    double utime;
    /* ファイルを開いて入力を読み込む */
    printf("input filename:");
    fgets(fname, sizeof(fname), stdin);
    fname[strlen(fname) - 1] = '\0';
    fflush(stdin);
    fp = fopen(fname, "r");
    fscanf(fp, "%d %d", &N, &M);
    /*（各グループ作成）*/
    fclose(fp);
    start_t = clock(); /*　時間計測開始 */
    /* 辺を重み昇順にソート */
    /* 初期解を発生 */
    end_t = clock();
    utime = (end_t - start_t) / CLOCKS_PER_SEC;
    while (utime <= time_limit)
    {
        /*（主要処理）*/
        end_t = clock();
        utime = (end_t - start_t) / CLOCKS_PER_SEC;
        if (utime > time_limit)
        {
            break;
        } /* 制限時間を過ぎたら終了 */
        else
        { /* そうでなければ必要に応じて解を更新 */
            /* 新たな初期解を発生 */
        }
    }
    /* 結果を出力 */
}
