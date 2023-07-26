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
int set_union(int *p, int *rank, int x, int y);
int kruskal(int n, int m, struct edge_data *edge, int *ednum, int *IU, int *T);
/* 他の関数も宣言する */

int main(void)
{
    int N, M, K; /* N = 頂点数, M = 辺数 , K = ターミナル数（S の頂点数） */
    struct edge_data edge[maxM];
    int i, j;
    int u, v, w, t; // ファイルの読み込みに使用
    int S[maxN];
    int s;             // sの頂点数
    int ednum[maxM];   // 昇順における辺の番号を格納する配列
    int IU[maxN];      // 頂点集合Uを表す．IU[u]=1ならばUは頂点uを含む
    int T[maxM];       // 最小木を表す配列．T[i]=1ならば最小木は辺iを含む
    int fU, fUdash;    // 初期解のf(U), 近傍のf(U')
    int best_fU = inf; // 最良解のf(U)．infに初期化しておく
    int first_fU;      // 初期解のf(U)
    int num_U = 0;     // 発生させた初期解の数．0に初期化しておく
    int in_S;          // 頂点が集合Sに入っているかどうか．in_S=1ならばSに含まれる．
    int U_update;      // 近傍を求めた後f(U)を更新したかどうか(終了条件)
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
    if (N > maxN || M > maxM)
    {
        printf("N or M is too large, redefine maxN, maxM\n");
    }
    for (i = 0; i < M; i++)
    {
        fscanf(fp, " %d %d %d", &u, &v, &w); /* 順番に辺の両端 u, v を読み込む */
        edge[i].end1 = u;                    /* 配列 edge に格納 */
        edge[i].end2 = v;
        edge[i].weight = w;
    }
    fscanf(fp, "%d", &s);
    for (i = 0; i < s; i++)
    {
        fscanf(fp, "%d", &t);
        S[i] = t;
    }
    fclose(fp);

    start_t = clock(); /*　時間計測開始 */
    /* 辺を重み昇順にソート */
    // ソート関数
    void sortEdges(struct edge_data * edge, int m)
    {
        int *W = (int *)malloc(sizeof(int) * m);
        int *ednum = (int *)malloc(sizeof(int) * m);

        for (int i = 0; i < m; ++i)
        {
            W[i] = edge[i].weight;
            ednum[i] = i;
        }

        quicksort(W, ednum, 0, m - 1);

        struct edge_data *sorted_edge = (struct edge_data *)malloc(sizeof(struct edge_data) * m);
        for (int i = 0; i < m; ++i)
        {
            sorted_edge[i] = edge[ednum[i]];
        }

        for (int i = 0; i < m; ++i)
        {
            edge[i] = sorted_edge[i];
        }

        free(W);
        free(ednum);
        free(sorted_edge);
    }

    /* 初期解を発生 */
    void generateInitialSolution(Graph * graph, int S[], int num_S)
    {
        // 最小全域木を求める
        Graph mst = kruskalMST(graph);

        // Tの辺が1本のみ接続するS以外の頂点νがあれば、νに接続するTの(唯一の)辺をTから取り除く操作
        bool removed_edge;
        do
        {
            removed_edge = false;
            for (int i = 0; i < mst.E; ++i)
            {
                int u = mst.edges[i].u;
                int v = mst.edges[i].v;
                if (num_S == 0 || (S[0] != u && S[0] != v))
                {
                    int degree_u = 0, degree_v = 0;
                    for (int j = 0; j < mst.E; ++j)
                    {
                        if (mst.edges[j].u == u || mst.edges[j].v == u)
                        {
                            degree_u++;
                        }
                        if (mst.edges[j].u == v || mst.edges[j].v == v)
                        {
                            degree_v++;
                        }
                    }
                    if (degree_u == 1 || degree_v == 1)
                    {
                        mst.edges[i] = mst.edges[mst.E - 1];
                        mst.E--;
                        removed_edge = true;
                    }
                }
            }
        } while (removed_edge);

        // 残った辺に接続する頂点の集合をUとする
        int U[MAX_VERTICES];
        int num_U = 0;
        for (int i = 0; i < mst.E; ++i)
        {
            U[num_U++] = mst.edges[i].u;
            U[num_U++] = mst.edges[i].v;
        }

        // Uの重複を削除
        int unique_U[MAX_VERTICES];
        int num_unique_U = 0;
        for (int i = 0; i < num_U; ++i)
        {
            bool is_duplicate = false;
            for (int j = 0; j < num_unique_U; ++j)
            {
                if (U[i] == unique_U[j])
                {
                    is_duplicate = true;
                    break;
                }
            }
            if (!is_duplicate)
            {
                unique_U[num_unique_U++] = U[i];
            }
        }

        // 初期解として得られた頂点集合Uを出力
        printf("Initial Solution (U): ");
        for (int i = 0; i < num_unique_U; ++i)
        {
            printf("%d ", unique_U[i]);
        }
        printf("\n");
    }

    num_U++;       // 初期解発生させた数+1
    first_fU = fU; // この時のf(U)を記録
    end_t = clock();
    utime = (end_t - start_t) / CLOCKS_PER_SEC;
    while (utime <= time_limit)
    {
        /*（主要処理）*/
        fU = kruskal(N, M, edge, ednum, IU, T); // f(U)の計算
        U_update = 0;
        while (U_update == 0) // Uが更新される限り近傍を探し続ける
        {
            U_update = 1;
            for (i = 0; i < N; i++) // すべての頂点について
            {
                in_S = 0;               // in_Sの初期化
                for (j = 0; j < s; j++) // Sの配列の参照
                {
                    if (S[j] == i) // Sにiが含まれているなら
                    {
                        in_S = 1; // in_S = 1とする
                    }
                }
                if (in_S == 0) // in_S = 0のときのみ近傍を作る
                {
                    if (IU[i] == 1) // iがUに含まれているなら
                    {
                        IU[i] = 0;                                  // iをUから除く
                        fUdash = kruskal(N, M, edge, ednum, IU, T); // f(U')の計算
                        if (fUdash < fU)                            // f(U')の方がf(U)より小さいなら
                        {
                            fU = fUdash;  // f(U)を更新
                            U_update = 0; // 更新されたことを示す
                            break;        // for文を抜ける
                        }
                        else // 小さくないなら
                        {
                            IU[i] = 1; // 除いたiを戻す
                        }
                    }
                    else // iがUに含まれていないなら
                    {
                        IU[i] = 1;                                  // iをUに加える
                        fUdash = kruskal(N, M, edge, ednum, IU, T); // f(U')の計算
                        if (fUdash < fU)                            // f(U')の方がf(U)より小さいなら
                        {
                            fU = fUdash;  // f(U)を更新
                            U_update = 0; // 更新されたことを示す
                            break;        // for文を抜ける
                        }
                        else // 小さくないなら
                        {
                            IU[i] = 0; // 加えたiを戻す
                        }
                    }
                }
            }
        }

        end_t = clock();
        utime = (end_t - start_t) / CLOCKS_PER_SEC;
        if (utime > time_limit)
        {
            break;
        } /* 制限時間を過ぎたら終了 */
        else
        { /* そうでなければ必要に応じて解を更新 */
            /* 新たな初期解を発生 */
            void generateNewInitialSolution(Graph * graph, int S[], int num_S)
            {
                // グラフの辺を重み昇順にソート
                sortEdges(graph->edges, graph->E);

                // 最小全域木を求める
                Graph mst = kruskalMST(graph);

                // 「Tの辺が1本のみ接続するS以外の頂点νがあれば、νに接続するTの(唯一の)辺をTから取り除く」操作
                bool removed_edge;
                do
                {
                    removed_edge = false;
                    for (int i = 0; i < mst.E; ++i)
                    {
                        int u = mst.edges[i].u;
                        int v = mst.edges[i].v;
                        if (num_S == 0 || (S[0] != u && S[0] != v))
                        {
                            int degree_u = 0, degree_v = 0;
                            for (int j = 0; j < mst.E; ++j)
                            {
                                if (mst.edges[j].u == u || mst.edges[j].v == u)
                                {
                                    degree_u++;
                                }
                                if (mst.edges[j].u == v || mst.edges[j].v == v)
                                {
                                    degree_v++;
                                }
                            }
                            if (degree_u == 1 || degree_v == 1)
                            {
                                mst.edges[i] = mst.edges[mst.E - 1];
                                mst.E--;
                                removed_edge = true;
                            }
                        }
                    }
                } while (removed_edge);

                // 残った辺に接続する頂点の集合をUとする
                int U[MAX_VERTICES];
                int num_U = 0;
                for (int i = 0; i < mst.E; ++i)
                {
                    U[num_U++] = mst.edges[i].u;
                    U[num_U++] = mst.edges[i].v;
                }

                // Uの重複を削除
                int unique_U[MAX_VERTICES];
                int num_unique_U = 0;
                for (int i = 0; i < num_U; ++i)
                {
                    bool is_duplicate = false;
                    for (int j = 0; j < num_unique_U; ++j)
                    {
                        if (U[i] == unique_U[j])
                        {
                            is_duplicate = true;
                            break;
                        }
                    }
                    if (!is_duplicate)
                    {
                        unique_U[num_unique_U++] = U[i];
                    }
                }

                // 新たな初期解として得られた頂点集合Uを出力
                printf("New Initial Solution (U): ");
                for (int i = 0; i < num_unique_U; ++i)
                {
                    printf("%d ", unique_U[i]);
                }
                printf("\n");
            }
            num_U++; // 初期解発生させた数+1
        }
    }

    /* 結果の出力 */
    printf("最良解のコスト: %d\n初期解のコスト: %d\n発生させた初期解の数: %d\n", best_fU, first_fU, num_U);
}

/* ソート関連の関数 */
void quicksort(int *W, int *ednum, int p, int r)
{
    if (p < r)
    {
        int q = partition(W, ednum, p, r);
        quicksort(W, ednum, p, q);
        quicksort(W, ednum, q + 1, r);
    }
}

int partition(int *W, int *ednum, int p, int r)
{
    int x = W[p];
    int i = p;
    int j = r;

    while (1)
    {
        while (W[j] > x)
        {
            j--;
        }

        while (W[i] < x)
        {
            i++;
        }

        if (i < j)
        {
            int tmp1 = W[i];
            W[i] = W[j];
            W[j] = tmp1;

            int tmp2 = ednum[i];
            ednum[i] = ednum[j];
            ednum[j] = tmp2;

            i++;
            j--;
        }
        else
        {
            break;
        }
    }

    return j;
}

/* ユニオン・ファインド森の関数 */
void make_set(int *p, int *rank, int x)
{
    p[x] = x;
    rank[x] = 0;
}

int find_set(int *p, int x)
{
    int y = x;
    while (1)
    {
        y = p[y];
        if (p[y] == y)
        {
            break;
        }
    }
    int r = y;
    y = x;

    while (1)
    {
        int z = p[y];
        p[y] = r;
        y = z;
        if (p[y] == y)
        {
            break;
        }
    }

    return r;
}

void link(int *p, int *rank, int x, int y)
{
    if (rank[x] > rank[y])
    {
        p[y] = x;
    }
    else
    {
        p[x] = y;
        if (rank[x] == rank[y])
        {
            rank[y] = rank[y] + 1;
        }
    }
}

int set_union(int *p, int *rank, int x, int y)
{
    int u = find_set(p, x);
    int v = find_set(p, y);
    int t = 0;
    if (u != v)
    {
        link(p, rank, u, v);
        t = 1;
    }
    return t;
}

/* f(U) を求める関数 */
int kruskal(int n, int m, struct edge_data *edge, int *ednum, int *IU, int *T)
{
    int p[maxN], rank[maxN]; /* p[x], rank[x] を格納する変数 */
    int W[maxM];             /* 辺の重みを格納する配列 */
    int i;
    int k;
    int u, v;      /* 頂点を表す変数 */
    int num_U = 0; /* Uの頂点数 */
    int num_M = 0; /* 選ばれた辺の数 */
    int fU = 0;    /* f(U) */

    for (i = 0; i < n; i++)
    {
        W[i] = edge[i].weight;
        ednum[i] = i; /* 配列 W と ednum にデータを格納 */
    }

    quicksort(W, ednum, 0, m - 1);
    for (i = 0; i < m; i++)
    {
        T[i] = 0;
    }
    for (u = 0; u < n; u++)
    {
        if (IU[u] == 1)
        { /* Uに含まれるuについて */
            make_set(p, rank, u);
        }
    }

    for (i = 0; i < m; i++)
    {
        k = ednum[i];
        u = edge[k].end1;
        v = edge[k].end2;
        if (IU[u] == 1 && IU[v] == 1)
        { /* u,vがどちらもUに含まれるなら */
            if (set_union(p, rank, u, v) == 1)
            { /* 統合したなら */
                T[k] = 1;
            }
        }
    }

    for (i = 0; i < n; i++)
    { /* Uの頂点数を計算 */
        if (IU[i] == 1)
        {            /* すべての頂点についてIU[i]==0なら */
            num_U++; /* 頂点数を+1 */
        }
    }
    for (i = 0; i < m; i++)
    { /* 選ばれた辺の数を計算 */
        if (T[i] == 1)
        {            /* すべての辺についてIU[i]==0なら */
            num_M++; /* 選ばれた辺の数を+1 */
        }
    }
    if (num_M == num_U - 1)
    { /* 選ばれた辺の数 = Uの頂点数-1 ならば */
        for (i = 0; i < m; i++)
        { /* すべての辺について */
            if (T[i] == 1)
            {                   /* その辺が最小木に含まれるなら */
                fU = fU + W[i]; /* その重みをf(U)に加算 */
            }
        }
        return fU; /* f(U)を返す */
    }
    else
    {
        return inf; /* 辺が少ないならinfを返す */
    }
}
/* 他の関数も宣言する */
