/*
 * 权重轮询调度算法: 可以用于实现简单的负载均衡
 * 例如: 4台机器, 权重比为3:1:1:1,最后流量比为3:1:1:1
 */

#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define SRV_NUM 10
#define MAX_LEN 20
#define WEIGHT_RANGE 10

typedef struct srv_info {
    char* ip;
    int weight;
} ServerInfo;

ServerInfo servers[SRV_NUM];

int getGcd();
int getArrayGcd();
int getMaxWeight();
int getServer();


int main(int argc, char const *argv[]) {
    srand(time(NULL));

    // 配置服务器的信息
    int i;
    const char* ip_st = "192.168.1.10";
    for (i = 0; i < SRV_NUM; i++) {
        ServerInfo temp;
        temp.ip = (char*)malloc(sizeof(char) * MAX_LEN);
        sprintf(temp.ip, "%s%d", ip_st, i);
        temp.weight = rand() % WEIGHT_RANGE + 1;
        servers[i] = temp;
    }
    // 输出配置信息
    for (i = 0; i < SRV_NUM; i++) {
        printf("%s   %d\n", servers[i].ip, servers[i].weight);
        //free(servers[i].ip);
    }
    printf("====================================================\n");

    // 每个连接选择的服务器
    int res;
    int request_statistics[SRV_NUM];  // 流量统计
    memset(request_statistics, 0, sizeof(request_statistics));
    for (i = 1; i <= 100; i++) {
        res = getServer();
        assert(res >= -1);
        request_statistics[res] += 1;
        printf("Request %2d is sent to %s with weight %d\n", i,
                servers[res].ip, servers[res].weight);
    }
    printf("====================================================\n");

    printf("All request statistics:\n");
    for (i = 0; i < SRV_NUM; i++) {
        printf("%s with weight %d has got %d requests\n",
                servers[i].ip, servers[i].weight, request_statistics[i]);
    }
    return 0;
}


int getGcd(int fir, int sec) {
    int temp = fir % sec;

    if (temp == 0) {
        return sec;
    } else {
        return getGcd(sec, temp);
    }
}


// 求权重的最大公约数
int getArrayGcd() {
    int i, res;

    res = getGcd(servers[0].weight, servers[1].weight);
    for (i = 2; i < SRV_NUM; i++) {
        res = getGcd(res, servers[i].weight);
    }
    return res;
}


// 求最大权重值
int getMaxWeight() {
    int i, max_wgt = -1;

    for (i = 0; i < SRV_NUM; i++) {
        if (max_wgt < servers[i].weight) {
            max_wgt = servers[i].weight;
        }
    }
    return max_wgt;
}


// 获取新连接选择的服务器编号
int getServer() {
    static int index = -1;  // 上一个选取的服务器,初始化为-1
    static int cur_wgt = 0;  // 当前权重值
    static int gcd;
    gcd = getArrayGcd();
    static int max_wgt;
    max_wgt = getMaxWeight();

    while (1) {  // cur_wgt初始设置为max_wgt,此后循环递减,保证每个服务器都能获得连接
        index = (index + 1) % SRV_NUM;
        if (index == 0) {
            cur_wgt -= gcd;
            if (cur_wgt <= 0) {
                cur_wgt = max_wgt;
                if (cur_wgt == 0) {
                    return -1;
                }
            }
        }

        if (servers[index].weight >= cur_wgt) {
            return index;
        }
    }
    return -1;
}
