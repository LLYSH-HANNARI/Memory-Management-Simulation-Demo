#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define getjcb(type) ((type *)malloc(sizeof(type)))
#define getsub(type) ((type *)malloc(sizeof(type)))

int num, num2;         // 要调度的作业数和要回收的区域数
int m = 0;             // 已分配作业数
int flag;              // 分配成功标志
int isup, isdown;      // 回收区域存在上部和下部的标志
int is = 0;

struct jcb
{
    char name[10];
    char state;
    int size;          // 所需空间大小
    int addr;          // 所分配分区的首地址
    struct jcb *link;
} *ready = NULL, *p, *q, *as = NULL; // 作业队列ready, 已分配作业队列as

typedef struct jcb JCB;

struct subarea
{
    char name[10];
    int addr;          // 分区首地址
    int size;          // 分区大小
    char state;
    struct subarea *link;
} *sub = NULL, *r, *s, *cur; // 空闲分区队列sub, 当前分区指针cur

typedef struct subarea SUB;

// 函数声明
void NewSort();
void AllocationSort();
void AllocationInput();
void AllocationDisp();
void FreeInput();
void FreeDisp();
void FreeReclaim(JCB *pr);
void print();
void init_sub();
void Assign(JCB *pr);
void Display(int x);
void menu();
void FirstFit();
void BestFit();
void WorstFit();
void BestFreeSort();   // 新增：最佳适应排序函数
void WorstFreeSort();  // 新增：最坏适应排序函数

// 新建作业直接插在作业队列之尾
void NewSort()
{
    JCB *first;
    if (ready == NULL)
        ready = p;
    else
    {
        first = ready;
        while (first->link != NULL)
            first = first->link;
        first->link = p;
        p->link = NULL;
    }
}

// 建立对已分配作业队列的排列函数, 直接插在队列之尾
void AllocationSort()
{
    JCB *fir;
    if (as == NULL)
        as = q;
    else
    {
        fir = as;
        while (fir->link != NULL)
            fir = fir->link;
        fir->link = q;
        q->link = NULL;
    }
    m++;
}

// 建立需要调度的作业控制块函数
void AllocationInput()
{
    int i;
    printf("\n请输入要调度的总作业数:");
    scanf("%d", &num);
    for (i = 0; i < num; i++)
    {
        printf("\n作业号 No.%d:\n", i);
        p = getjcb(JCB);
        printf("\n输入作业名:");
        scanf("%s", &p->name);
        printf("\n输入作业的大小:");
        scanf("%d", &p->size);
        p->link = NULL;
        NewSort(); // 调用NewSort函数
    }
    printf("\n按任一键继续...\n");
    getchar();
}

// 建立要回收区域的函数
void FreeInput()
{
    JCB *k;
    int has, n = 0;
    q = getjcb(JCB);
    printf("\n输入区域名(作业名):");
    scanf("%s", &q->name);
    p = as;
    while (p != NULL)
    {
        if (strcmp(p->name, q->name) == 0) // 在已分配作业队列中寻找
        {
            q->addr = p->addr;
            q->size = p->size;
            has = 1; // 输入作业名存在标志位
            if (p == as)
                as = p->link; // 在已分配作业队列中删除该作业
            else
            {
                k = as;
                while (k->link != p)
                    k = k->link;
                k->link = k->link->link; // 删除
            }
            printf("输出该作业首地址:%d\n", q->addr);
            printf("输出该作业大小:%d\n\n", q->size);
            q->link = NULL;
            break;
        }
        else // 输入作业名不存在标志
        {
            p = p->link;
            has = 0;
        }
    }
    if (has == 0)
    {
        printf("\n输入作业名错误! 请重新输入!\n");
        FreeInput();
    }
}

void print()
{
    printf("\n\n\n\n");
    printf("\t\t************************************************\n");
    printf("\t\t\t实验四:存储管理实验演示\n");
    printf("\t\t************************************************\n\n\n");
    printf("\t\t！！仅供学习交流！！\n\n\n\n");
    printf("\t\t请勿用于商业用途\n\n\n\n");
    printf("\t\t完成时间:2026年5月11日\n\n\n\n");
    printf("\t\t\t请输入任意键进入演示过程\n");
    printf("\n\n\n");
    getchar();
    system("cls");
}

// 初始化空闲分区表
void init_sub()
{
    r = getsub(SUB);
    strcpy(r->name, "0");
    r->addr = 0;
    r->size = 512;
    r->state = 'n';
    sub = r;
    r->link = NULL;
}

// 显示空闲分区表的函数
void FreeDisp()
{
    printf("\n\n");
    printf("\t\t分区\t首地址\t长度\t状态\n");
    r = sub;
    while (r != NULL)
    {
        printf("\t\t%s\t%d\t%d\t%c\n", r->name, r->addr, r->size, r->state);
        r = r->link;
    }
    printf("\n\n请输入任意键继续\n");
    getchar();
    printf("\n");
}

// 显示已分配内存的作业表函数
void AllocationDisp()
{
    printf("\n\n");
    printf("\t\t作业名\t首地址\t长度\t状态\n");
    p = as;
    while (p != NULL)
    {
        printf("\t\t%s\t%d\t%d\t%c\n", p->name, p->addr, p->size, p->state);
        p = p->link;
    }
    printf("\n\n");
}

// 作业分配
void Assign(JCB *pr)
{
    r = sub; // 从空闲表头开始寻找
    while (r != NULL)
    {
        if (((r->size) > (pr->size)) && (r->state == 'n')) // 有空闲分区大于作业大小的情况
        {
            pr->addr = r->addr;
            r->size -= pr->size;
            r->addr += pr->size;
            flag = 1; // 分配成功标志位置1
            q = pr;
            q->state = 'r';
            AllocationSort(); // 插入已分配作业队列
            printf("作业%s的首地址为%d.\n", pr->name, pr->addr);
            break;
        }
        else if (((r->size) == (pr->size)) && (r->state == 'n')) // 有空闲分区等于作业大小的情况
        {
            pr->addr = r->addr;
            flag = 1; // 分配成功标志位置1
            q = pr;
            AllocationSort(); // 插入已分配作业队列
            if (r == sub) // 刚好是空闲队列中的第一个空闲区
                sub = sub->link;
            else // 不是空闲队列中的第一个空闲区
            {
                s = sub;
                while (s->link != r)
                    s = s->link;
                s->link = s->link->link; // 删除空闲分区
            }
            printf("作业%s的首地址为%d.\n", pr->name, pr->addr);
            break;
        }
        else
        {
            r = r->link;
            flag = 0;
        }
    }
    if (flag == 0) // 作业过大的情况
    {
        printf("作业%s长度过大,内存不足,分区分配出错!\n", pr->name);
        is = 1;
    }
}

// 作业回收
void FreeReclaim(JCB *pr)
{
    SUB *k;
    r = sub;
    while (r != NULL)
    {
        if (r->addr == ((pr->addr) + (pr->size))) // 回收区域有下邻
        {
            pr->size += r->size;
            s = sub;
            isdown = 1; // 下邻标志位置1
            while (s != NULL)
            {
                if (((s->addr) + (s->size)) == (pr->addr)) // 有下邻又有上邻
                {
                    s->size += pr->size;
                    k = sub;
                    while (k->link != r)
                        k = k->link;
                    k->link = k->link->link;
                    isup = 1; // 上邻标志位置1
                    break;
                }
                else // 上邻标志位置0
                {
                    s = s->link;
                    isup = 0;
                }
            }
            if (isup == 0) // 有下邻无上邻
            {
                r->addr = pr->addr;
                r->size = pr->size;
            }
            break;
        }
        else
        {
            r = r->link;
            isdown = 0; // 下邻标志位置0
        }
    }
    if (isdown == 0) // 区域无下邻
    {
        s = sub;
        while (s != NULL)
        {
            if (((s->addr) + (s->size)) == (pr->addr)) // 无下邻但有上邻
            {
                s->size += pr->size;
                isup = 1; // 上邻标志位置1
                break;
            }
            else // 上邻标志位置0
            {
                s = s->link;
                isup = 0;
            }
        }
        if (isup == 0) // 无下邻且无上邻
        {
            k = getsub(SUB); // 重新生成一个新的分区节点
            strcpy(k->name, pr->name);
            k->addr = pr->addr;
            k->size = pr->size;
            k->state = 'n';
            r = sub;
            while (r != NULL)
            {
                if ((r->addr) > (k->addr)) // 按分区首地址排列,回收区域插在合适的位置
                {
                    if (r == sub) // 第一个空闲分区首址大于回收区域的情况
                    {
                        k->link = r;
                        sub = k;
                    }
                    else
                    {
                        s = sub;
                        while (s->link != r)
                            s = s->link;
                        k->link = r;
                        s->link = k;
                    }
                    break;
                }
                else
                    r = r->link;
            }
            if (r == NULL) // 所有空闲分区的首址都大于回收区域首址的情况
            {
                s = sub;
                while (s->link != NULL)
                    s = s->link;
                s->link = k;
                k->link = NULL;
            }
        }
    }
    printf("\n区域%s已回收.", pr->name);
}

// 最佳适应排序：空闲区按大小升序排列
void BestFreeSort()
{
    SUB *s1, *s2, *s3 = NULL, *s4, *s5;
    int insert = 0;
    s1 = sub;
    while (s1 != NULL)
    {
        s2 = s1;
        s1 = s2->link;
        s2->link = NULL;
        insert = 0;
        if (s3 == NULL || (s2->size < s3->size))
        {
            s2->link = s3;
            s3 = s2;
        }
        else
        {
            s4 = s3;
            s5 = s4->link;
            while (s5 != NULL)
            {
                if (s2->size < s5->size)
                {
                    s2->link = s5;
                    s4->link = s2;
                    insert = 1;
                    break;
                }
                else
                {
                    s4 = s4->link;
                    s5 = s5->link;
                }
            }
            if (insert == 0)
                s4->link = s2;
        }
    }
    sub = s3;
}

// 最坏适应排序：空闲区按大小降序排列
void WorstFreeSort()
{
    SUB *s1, *s2, *s3 = NULL, *s4, *s5;
    int insert = 0;
    s1 = sub;
    while (s1 != NULL)
    {
        s2 = s1;
        s1 = s2->link;
        s2->link = NULL;
        insert = 0;
        if (s3 == NULL || (s2->size > s3->size))
        {
            s2->link = s3;
            s3 = s2;
        }
        else
        {
            s4 = s3;
            s5 = s4->link;
            while (s5 != NULL)
            {
                if (s2->size > s5->size)
                {
                    s2->link = s5;
                    s4->link = s2;
                    insert = 1;
                    break;
                }
                else
                {
                    s4 = s4->link;
                    s5 = s5->link;
                }
            }
            if (insert == 0)
                s4->link = s2;
        }
    }
    sub = s3;
}

void menu()
{
    int m;
    system("cls");
    printf("\n\n\t\t************************************************\t\t\n");
    printf("\t\t\t作业调度演示\n");
    printf("\t\t************************************************\t\t\n");
    printf("\n\n\t\t1. 首次适应分配算法.");
    printf("\n\t\t2. 最佳适应分配算法.");
    printf("\n\t\t3. 最坏适应分配算法");
    printf("\n\t\t0. 退出程序.");
    printf("\n\n\t\t选择所要操作:");
    scanf("%d", &m);
    switch (m)
    {
        case 1:
            FirstFit();
            getchar();
            system("cls");
            menu();
            break;
        case 2:
            BestFit();
            getchar();
            system("cls");
            menu();
            break;
        case 3:
            WorstFit();
            getchar();
            system("cls");
            menu();
            break;
        case 0:
            system("cls");
            break;
        default:
            printf("选择错误,重新选择.");
            getchar();
            system("cls");
            menu();
    }
}

// 显示已分配表和空闲表的情况
void Display(int x)
{
    printf("\n显示空闲分区表和已分配作业表...");
    getchar();
    printf("\n\t\t\t完成分配后的空闲分区表\n");
    FreeDisp();
    printf("\n\t\t\t已分配作业表\n");
    AllocationDisp();
    if (x == 2)
    {
        if (is == 0)
            printf("\n全部作业已经被分配内存.");
        else
            printf("\n作业没有全部被分配内存.\n");
    }
    printf("\n\n请输入任意键继续...");
    getchar();
}

// 首次分配算法
void FirstFit()
{
    int i;
    printf("\n\n\n\t\t************************************************\n");
    printf("\t\t\t可变式存储管理实验演示\n");
    printf("\t\t\t(首次适应分配算法)\n");
    printf("\t\t************************************************\n\n\n");
    printf("\t\t1. 显示空闲分区\n");
    printf("\t\t2. 分配作业\n");
    printf("\t\t3. 回收作业\n");
    printf("\t\t0. 退出\n");
    printf("\t\t请选择你要的操作:");
    scanf("%d", &i);
    switch (i)
    {
        case 1:
            system("cls");
            FreeDisp(); // 显示空闲区
            getchar();
            FirstFit();
            break;
        case 2:
            printf("\n首次适应分配算法分配:");
            AllocationInput(); // 新建需要调度的作业控制块
            printf("\n");
            while (num != 0)
            {
                p = ready;
                ready = p->link;
                p->link = NULL;
                Assign(p); // 将新建的调度作业都分配到内存空间中
                num--;
            }
            Display(i);
            FirstFit();
            break;
        case 3:
            printf("\n首次适应分配算法回收:");
            FreeInput(); // 建立要回收的作业
            printf("按任意键继续...");
            getchar();
            printf("\n");
            FreeReclaim(q); // 回收作业
            printf("\n");
            Display(i);
            FirstFit();
            break;
        case 0:
            system("cls");
            break;
        default:
            system("cls");
            FirstFit();
    }
}

// 最佳分配算法
void BestFit()
{
    int i;
    printf("\n\n\n\t\t************************************************\n");
    printf("\t\t\t可变式存储管理实验演示\n");
    printf("\t\t\t(最佳适应分配算法)\n");
    printf("\t\t************************************************\n\n\n");
    printf("\t\t1. 显示空闲分区\n");
    printf("\t\t2. 分配作业\n");
    printf("\t\t3. 回收作业\n");
    printf("\t\t0. 退出\n");
    printf("\t\t请选择你要的操作:");
    scanf("%d", &i);
    switch (i)
    {
        case 1:
            system("cls");
            FreeDisp(); // 显示空闲区
            getchar();
            BestFit();
            break;
        case 2:
            printf("\n最佳适应分配算法分配:");
            AllocationInput(); // 新建需要调度的作业控制块
            printf("\n");
            BestFreeSort();    // 按最佳适应排序空闲区
            while (num != 0)
            {
                p = ready;
                ready = p->link;
                p->link = NULL;
                Assign(p); // 将新建的调度作业都分配到内存空间中
                num--;
            }
            Display(i);
            BestFit();
            break;
        case 3:
            printf("\n最佳适应分配算法回收:");
            FreeInput(); // 建立要回收的作业
            printf("按任意键继续...");
            getchar();
            printf("\n");
            FreeReclaim(q); // 回收作业
            BestFreeSort(); // 回收后重新排序
            printf("\n");
            Display(i);
            BestFit();
            break;
        case 0:
            system("cls");
            break;
        default:
            system("cls");
            BestFit();
    }
}

// 最坏分配算法
void WorstFit()
{
    int i;
    printf("\n\n\n\t\t************************************************\n");
    printf("\t\t\t可变式存储管理实验演示\n");
    printf("\t\t\t(最坏适应分配算法)\n");
    printf("\t\t************************************************\n\n\n");
    printf("\t\t1. 显示空闲分区\n");
    printf("\t\t2. 分配作业\n");
    printf("\t\t3. 回收作业\n");
    printf("\t\t0. 退出\n");
    printf("\t\t请选择你要的操作:");
    scanf("%d", &i);
    switch (i)
    {
        case 1:
            system("cls");
            FreeDisp(); // 显示空闲区
            getchar();
            WorstFit();
            break;
        case 2:
            printf("\n最坏适应分配算法分配:");
            AllocationInput(); // 新建需要调度的作业控制块
            printf("\n");
            WorstFreeSort();   // 按最坏适应排序空闲区
            while (num != 0)
            {
                p = ready;
                ready = p->link;
                p->link = NULL;
                Assign(p); // 将新建的调度作业都分配到内存空间中
                num--;
            }
            Display(i);
            WorstFit();
            break;
        case 3:
            printf("\n最坏适应分配算法回收:");
            FreeInput(); // 建立要回收的作业
            printf("按任意键继续...");
            getchar();
            printf("\n");
            FreeReclaim(q); // 回收作业
            WorstFreeSort(); // 回收后重新排序
            printf("\n");
            Display(i);
            WorstFit();
            break;
        case 0:
            system("cls");
            break;
        default:
            system("cls");
            WorstFit();
    }
}

// 主函数
void main()
{
    init_sub();
    print();
    menu();
}