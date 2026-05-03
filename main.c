#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <limits.h>
#include <time.h>
#include <windows.h>
#define MIN_ROUTES 5              // 路线最少有5条
#define MAX_ROUTES 10             // 路线最多有10条
#define MIN_STATIONS 10           // 站点最少有10个
#define MAX_STATIONS 15           // 站点最多有15个
#define MIN_STATIONS_IN_ROUTE 5   // 每条路线上最少有5个站点
#define MAX_STATIONS_IN_ROUTE 10  // 每天路线上最多有10个站点
#define MAX_ROUTE_NAME_LENGTH 11  // 路线名称的最大长度
#define MAX_STATION_NAME_LENGTH 5 // 站点名称的最大长度
#define TIME 5                    // 同一个站点到另一个相邻一个站点所需时间
#define FILENAME "./routes"         //文件名称
#define CONTINUE 2                //继续程序常量
typedef struct
{
    char route_name[MAX_ROUTE_NAME_LENGTH]; // 路线的名称,比如路线1
    int time[MAX_STATIONS];
    int station_count;                                   // 路线上的站点数
    char station[MAX_STATIONS][MAX_STATION_NAME_LENGTH]; // 路线上站点的名称
} Route;                                                 // 路线
typedef struct
{
    char station[MAX_STATIONS][MAX_STATION_NAME_LENGTH]; // 所有站点
    int arc[MAX_STATIONS][MAX_STATIONS];                 // 站点之间的边
    int station_count;                                   // 站点数量
} Graph;                                                 // 图
int generate_routes(Route *routes, int *stations_count)
{
    if (!routes || !stations_count)
    {
        fprintf(stderr, "错误！generate_routes参数无效！\n");
        return 0;
    }
    // 生成5-10个随机路线,每个路线上随机生成5到10个站点
    const int routes_count = rand() % (MAX_ROUTES - MIN_ROUTES + 1) + MIN_ROUTES;
    *stations_count = rand() % (MAX_STATIONS - MIN_STATIONS + 1) + MIN_STATIONS;
    for (int i = 0; i < routes_count; i++)
    {
        snprintf(routes[i].route_name, MAX_ROUTE_NAME_LENGTH, "路线%d", i + 1);
        routes[i].station_count = rand() % (MAX_STATIONS_IN_ROUTE - MIN_STATIONS_IN_ROUTE + 1) +
                                  MIN_STATIONS_IN_ROUTE;
        bool visited[MAX_STATIONS] = {false};
        int visited_count = 0;
        while (visited_count < routes[i].station_count)
        {
            int id = rand() % (*stations_count);
            if (!visited[id])
            {
                visited[id] = true;
                visited_count++;
            }
        }
        int station_index = 0;
        for (int j = 0; j < *stations_count && station_index < routes[i].station_count; j++)
        {
            if (visited[j])
            {
                snprintf(routes[i].station[station_index], MAX_STATION_NAME_LENGTH, "S%d", j + 1);
                if (station_index != routes[i].station_count - 1)
                {
                    routes[i].time[station_index] = TIME;
                }
                else
                {
                    routes[i].time[station_index] = 0;
                }
                station_index++;
            }
        }
        for (int j = routes[i].station_count; j < MAX_STATIONS; j++)
        {
            routes[i].station[j][0] = '\0';
            routes[i].time[j] = 0;
        }
    }
    return routes_count;
}

bool check_file_available(const char file_name[])
{
    if (!file_name)
    {
        return false;
    }
    FILE *fp = fopen(file_name, "r");
    if (fp)
    {
        fclose(fp);
        return true;
    }
    return false;
}

bool save_to_file(Route *routes, const int routes_count, const int stations_count)
{
    // 将路线保存到文件中
    if (!routes || routes_count < 1 || stations_count < 1)
    {
        fprintf(stderr, "错误！save_to_file参数无效！\n");
        return false;
    }
    FILE *fp = fopen(FILENAME, "w");
    if (!fp)
    {
        fprintf(stderr, "文件%s打开失败！不能进行写入操作！\n", FILENAME);
        return false;
    }
    fprintf(fp, "%d %d\n", routes_count, stations_count);
    for (int i = 0; i < routes_count; i++)
    {
        fprintf(fp, "%s %d :", routes[i].route_name, routes[i].station_count);
        for (int j = 0; j < routes[i].station_count; j++)
        {
            fprintf(fp, " %s", routes[i].station[j]);
            if (j != routes[i].station_count - 1)
            {
                fprintf(fp, " %d", routes[i].time[j]);
            }
        }
        fprintf(fp, "\n");
    }
    fclose(fp);
    return true;
}

bool load_from_file(Route *routes, int *routes_count, int *stations_count)
{
    // 从文件中加载路线
    if (!routes || !routes_count || !stations_count)
    {
        fprintf(stderr, "错误！load_from_file参数无效！\n");
        return false;
    }
    FILE *fp = fopen(FILENAME, "r");
    if (!fp)
    {
        fprintf(stderr, "文件%s打开失败！不能进行加载操作！\n", FILENAME);
        return false;
    }
    if (fseek(fp, 0, SEEK_END) != 0)
    {
        fprintf(stderr, "文件%s无法定位\n", FILENAME);
        fclose(fp);
        return false;
    }
    if (ftell(fp) == 0)
    {
        fprintf(stderr, "文件%s为空\n", FILENAME);
        fclose(fp);
        return false;
    }
    if (fseek(fp, 0, SEEK_SET) != 0)
    {
        fprintf(stderr, "文件%s无法重置读取位置\n", FILENAME);
        fclose(fp);
        return false;
    }
    if (fscanf(fp, "%d %d", routes_count, stations_count) != 2)
    {
        fprintf(stderr, "文件%s的第一行格式错误！\n", FILENAME);
        fclose(fp);
        return false;
    }
    if (*routes_count < MIN_ROUTES || *routes_count > MAX_ROUTES)
    {
        fprintf(stderr, "路线数量%d超出范围!范围是%d-%d\n",
                *routes_count, MIN_ROUTES, MAX_ROUTES);
        fclose(fp);
        return false;
    }
    if (*stations_count < MIN_STATIONS || *stations_count > MAX_STATIONS)
    {
        fprintf(stderr, "站点数量%d超出范围!范围是%d-%d\n",
                *stations_count, MIN_STATIONS, MAX_STATIONS);
        fclose(fp);
        return false;
    }
    for (int i = 0; i < *routes_count; i++)
    {
        if (fscanf(fp, "%10s %d :", routes[i].route_name, &routes[i].station_count) != 2)
        {
            fprintf(stderr, "路线%d的基本信息读取失败！\n", i + 1);
            fclose(fp);
            return false;
        }
        if (routes[i].station_count < MIN_STATIONS_IN_ROUTE || routes[i].station_count > MAX_STATIONS_IN_ROUTE)
        {
            fprintf(stderr, "路线%d的站点数量%d超出范围！范围是%d-%d！\n", i + 1, routes[i].station_count,
                    MIN_STATIONS_IN_ROUTE, MAX_STATIONS_IN_ROUTE);
            fclose(fp);
            return false;
        }
        for (int j = 0; j < routes[i].station_count; j++)
        {
            if (fscanf(fp, "%4s", routes[i].station[j]) != 1)
            {
                fprintf(stderr, "路线%d的第%d个站点名称读取失败！", i + 1, j + 1);
                fclose(fp);
                return false;
            }
            if (j != routes[i].station_count - 1)
            {
                if (fscanf(fp, " %d", &routes[i].time[j]) != 1)
                {
                    fprintf(stderr, "路线%d的第%d个站点和第%d个站点之间的时间读取失败！", i + 1, j + 1, j + 2);
                    fclose(fp);
                    return false;
                }
            }
        }
        for (int j = routes[i].station_count; j < MAX_STATIONS; j++)
        {
            routes[i].station[j][0] = '\0';
            routes[i].time[j] = 0;
        }
    }
    fclose(fp);
    printf("成功从文件%s中加载线路信息！\n", FILENAME);
    return true;
}

int find_station_in_route(const Route *route, const char station_name[])
{
    if (!route || !station_name)
    {
        fprintf(stderr, "错误！find_station_in_route参数无效！\n");
        return -1;
    }
    for (int i = 0; i < route->station_count; i++)
    {
        if (strcmp(station_name, route->station[i]) == 0)
        {
            return i;
        }
    }
    return -1;
}

void graph_init(Graph *graph)
{
    // 初始化图
    if (!graph)
    {
        fprintf(stderr, "错误！graph_init参数无效！\n");
        return;
    }
    graph->station_count = 0;
    for (int i = 0; i < MAX_STATIONS; i++)
    {
        graph->station[i][0] = '\0';
        for (int j = 0; j < MAX_STATIONS; j++)
        {
            graph->arc[i][j] = INT_MAX;
        }
        graph->arc[i][i] = 0;
    }
}

int find_station_in_graph(const Graph *graph, const char station_name[])
{
    // 找到站点下标
    if (!graph || !station_name)
    {
        fprintf(stderr, "错误！find_station_in_graph参数无效！\n");
        return -1;
    }
    for (int i = 0; i < graph->station_count; i++)
    {
        if (strcmp(graph->station[i], station_name) == 0)
        {
            return i;
        }
    }
    return -1;
}

bool add_station(Graph *graph, const char station_name[])
{
    if (!graph || !station_name)
    {
        fprintf(stderr, "错误！add_station参数无效！\n");
        return false;
    }
    if (graph->station_count > MAX_STATIONS)
    {
        fprintf(stderr, "错误！图的站点数量超过上限%d！\n", MAX_STATIONS);
        return false;
    }
    if (find_station_in_graph(graph, station_name) != -1)
    {
        return true;
    }
    strcpy(graph->station[graph->station_count++], station_name);
    return true;
}

bool add_edge(Graph *graph, const char station1_name[], const char station2_name[], const int time)
{
    if (!graph || !station1_name || !station2_name || time <= 0)
    {
        fprintf(stderr, "错误！add_edge参数无效！\n");
        return false;
    }
    if (graph->station_count > MAX_STATIONS)
    {
        fprintf(stderr, "错误！图的站点数量超过上限%d！\n", MAX_STATIONS);
        return false;
    }
    const int index1 = find_station_in_graph(graph, station1_name);
    const int index2 = find_station_in_graph(graph, station2_name);
    if (index1 == -1)
    {
        fprintf(stderr, "未在图中找到%s！边添加失败！\n", station1_name);
        return false;
    }
    if (index2 == -1)
    {
        fprintf(stderr, "未在图中找到%s！边添加失败！\n", station2_name);
        return false;
    }
    if (time < graph->arc[index1][index2])
    {
        graph->arc[index1][index2] = time;
        graph->arc[index2][index1] = time;
    }
    return true;
}

void build_graph(const Route *routes, Graph *graph, const int routes_count, const int stations_count)
{
    if (!routes || !graph || routes_count < 1)
    {
        fprintf(stderr, "错误！build_graph参数无效！\n");
        return;
    }
    for (int i = 0; i < routes_count; i++)
    {
        for (int j = 0; j < routes[i].station_count; j++)
        {
            add_station(graph, routes[i].station[j]);
            if (graph->station_count >= stations_count)
            {
                i = routes_count;
                break;
            }
        }
    }
    for (int i = 0; i < routes_count; i++)
    {
        for (int j = 0; j < routes[i].station_count - 1; j++)
        {
            add_edge(graph, routes[i].station[j],
                     routes[i].station[j + 1], routes[i].time[j]);
        }
    }
}

bool find_route_have_neighbor(const Route *routes, const int routes_count,
                              const char station1[], const char station2[], int belongs[], int *belongs_count)
{//找到两个相邻节点所在的线路索引
    *belongs_count = 0;
    for (int i = 0; i < routes_count; i++)
    {
        const int index1 = find_station_in_route(&routes[i], station1);
        const int index2 = find_station_in_route(&routes[i], station2);
        if (index1 != -1 && index2 != -1 && abs(index1 - index2) == 1)
        {
            belongs[(*belongs_count)++] = i;
        }
    }
    if (*belongs_count != 0)
    {
        return true;
    }
    return false;
}

int calculate_transfer(const Route *routes, const int routes_count,
                       const Graph *graph, const int x_route, const int x, const int y, int *new_route)
{
    //计算换乘次数
    int belongs[MAX_ROUTES];
    int belongs_count = 0;
    find_route_have_neighbor(routes, routes_count,
        graph->station[x], graph->station[y], belongs, &belongs_count);
    if (x_route == -1)//还未乘车，无需换乘
    {
        if (belongs_count > 0)
        {
            *new_route = belongs[0];
        }
        else
        {
            *new_route = -1;//理论上不会发生
        }
        return 0;
    }
    for (int i = 0; i < belongs_count; i++)
    {
        if (belongs[i] == x_route)
        {
            *new_route = belongs[i];
            return 0;
        }
    }
    if (belongs_count == 0)
    {
        fprintf(stderr, "内部错误！相邻站点无共享路线\n");
        *new_route = -1;
        return 0;
    }
    *new_route = belongs[0];
    return 1;
}

void Dijkstra(const Graph *graph, const Route *routes, const int routes_count,
              const char start[], const char end[], int path[], int *path_length, int *total_time)
{
    // 求最短且换乘次数最少的路径
    if (!graph || !routes || !start || !end || !path || !path_length || !total_time)
    {
        fprintf(stderr, "错误！Dijkstra参数无效！\n");
        return;
    }
    const int start_index = find_station_in_graph(graph, start);
    const int end_index = find_station_in_graph(graph, end);
    if (start_index == -1)
    {
        fprintf(stderr, "起点%s未找到！\n", start);
        return;
    }
    if (end_index == -1)
    {
        fprintf(stderr, "终点%s未找到！\n", end);
        return;
    }
    if (start_index == end_index)
    {
        fprintf(stderr, "起点和终点相同！\n");
        path[0] = start_index;
        *path_length = 1;
        *total_time = 0;
        return;
    }
    int Dist[MAX_STATIONS], Pre[MAX_STATIONS]; // 最短距离和前驱节点
    bool visited[MAX_STATIONS];
    int transfer_count[MAX_STATIONS];//到某个站点的最短换乘次数
    int arrive_route[MAX_STATIONS];//到达某个站点的当前路线
    for (int i = 0; i < graph->station_count; i++)
    {//初始化
        Dist[i] = graph->arc[start_index][i];
        visited[i] = false;
        Pre[i] = -1;
        transfer_count[i] = (i == start_index ? 0 : INT_MAX);
        arrive_route[i] = -1;
    }
    Dist[start_index] = 0;
    for (int i = 0; i < graph->station_count - 1; i++)
    {
        int MIN = INT_MAX;
        int u = -1;
        for (int j = 0; j < graph->station_count; j++)
        {
            if(!visited[j] && Dist[j] < MIN){
                MIN = Dist[j];
                u = j;
            }
        }
        if (u == -1)
        {
            break;
        }
        visited[u] = true;
        for (int j = 0; j < graph->station_count; j++)
        {
            if (visited[j] || graph->arc[u][j] == INT_MAX)
            {
                continue;
            }
            const int new_dist = graph->arc[u][j] + Dist[u];
            int new_route = 0;
            const int add = calculate_transfer(routes, routes_count, graph, arrive_route[u], u, j, &new_route);
            if (transfer_count[u] == INT_MAX)
            {
                continue;
            }
            const int new_transfer = transfer_count[u] + add;
            if (new_dist < Dist[j])
            {//路径长度更短
                Dist[j] = new_dist;
                Pre[j] = u;
                transfer_count[j] = new_transfer;
                arrive_route[j] = new_route;
            }
            else if (new_dist == Dist[j] && new_transfer < transfer_count[j])
            {//路径长度相同的前提下，换乘次数更少
                Pre[j] = u;
                transfer_count[j] = new_transfer;
                arrive_route[j] = new_route;
            }
        }
    }
    if (Dist[end_index] == INT_MAX)
    {
        *path_length = 0;
        *total_time = 0;
        fprintf(stderr, "未找到从%s到%s的路径\n", start, end);
        return;
    }
    int count = 0;
    int current = end_index;
    while (current != -1)
    {
        path[count++] = current;
        current = Pre[current];
    }
    *path_length = count;
    *total_time = Dist[end_index];
    printf("\n");
}

bool check_direct_arrival(const Route *route, const char path_name[][MAX_STATION_NAME_LENGTH], const int path_length, int *direction)
{
    if (!route || !path_name || path_length < 2 || !direction)
    {
        fprintf(stderr, "错误！check_direct_arrival参数无效！\n");
        return false;
    }
    const int start_index = find_station_in_route(route, path_name[0]);
    if (start_index == -1)
    {
        return false;
    }
    const int end_index = find_station_in_route(route, path_name[path_length - 1]);
    if (end_index == -1)
    {
        return false;
    }
    *direction = start_index < end_index ? 1 : -1;
    int index = start_index;
    for (int i = 0; i < path_length; i++)
    {
        if (index < 0 || index >= route->station_count)
        {
            return false;
        }
        if (strcmp(route->station[index], path_name[i]) != 0)
        {
            return false;
        }
        index += *direction;
    }
    return true;
}

void transfer(const Route *routes, const int routes_count, const Graph *graph, const int path[],
              const int path_length, const int total_time)
{
    if (!routes || !graph || !path || path_length < 2 || total_time < 1)
    {
        fprintf(stderr, "路径信息不完整！\n");
        return;
    }
    printf("乘车方案:\n");
    printf("起点：%s\n", graph->station[path[path_length - 1]]);
    printf("终点：%s\n", graph->station[path[0]]);
    printf("用时：%d分钟\n", total_time);
    printf("路径：");
    char path_name[path_length][MAX_STATION_NAME_LENGTH];
    for (int i = 0; i < path_length; i++)
    {
        strcpy(path_name[i], graph->station[path[path_length - i - 1]]);
        printf("%s", path_name[i]);
        if (i < path_length - 1)
        {
            printf(" -> ");
        }
    }
    printf("\n\n");
    for (int i = 0; i < routes_count; i++)
    {
        int direction = 0;
        if (check_direct_arrival(&routes[i], path_name, path_length, &direction))
        {
            printf("乘坐%s(%s) : ", routes[i].route_name, direction > 0 ? "正向" : "反向");
            for (int j = 0; j < path_length; j++)
            {
                printf("%s", path_name[j]);
                if (j != path_length - 1)
                {
                    printf(" -> ");
                }
            }
            int direct_time = 0;
            for (int j = 0; j < path_length - 1; j++)
            {
                direct_time += graph->arc[find_station_in_graph(graph, path_name[j])][find_station_in_graph(graph, path_name[j + 1])];
            }
            printf(" 用时%d分钟\n", direct_time);
            printf("本次乘车方案无需换乘，可直达目的地\n");
            return ;
        }
    }
    int current_pos = 0;
    int transfer_count = 0;
    while (current_pos < path_length - 1)//找到换乘次数最少的乘车方案，找到最长路段
    {
        int best_route = -1;//最佳路线
        int best_direction = 0;//最佳方向
        int best_length = 0;//最佳长度
        int best_end_index = current_pos;//最佳结束位置
        for (int i = 0; i < routes_count; i++)
        {
            const int start_index = find_station_in_route(&routes[i], path_name[current_pos]);
            if (start_index == -1)
            {
                continue;
            }
            int forward_length = 1;//尝试寻找正向最长路段
            int forward_pos = current_pos + 1;
            int index1 = start_index;
            while (forward_pos < path_length)
            {
                if (index1 + 1 < routes[i].station_count && strcmp(routes[i].station[index1 + 1], path_name[forward_pos]) == 0)
                {
                    forward_length++;
                    forward_pos++;
                    index1++;
                }
                else
                {
                    break;
                }
            }
            int back_length = 1;//尝试寻找正向最长路段
            int back_pos = current_pos + 1;
            int index2 = start_index;
            while (back_pos < path_length)
            {
                if (index2 - 1 > -1 && strcmp(routes[i].station[index2 - 1], path_name[back_pos]) == 0)
                {
                    back_length++;
                    back_pos++;
                    index2--;
                }
                else
                {
                    break;
                }
            }
            const int better_length = forward_length > back_length ? forward_length : back_length;
            const int better_direction = forward_length > back_length ? 1 : -1;
            if (better_length > best_length)
            {
                best_length = better_length;
                best_direction = better_direction;
                best_route = i;
                best_end_index = current_pos + better_length - 1;
            }
        }
        if (best_route == -1)
        {
            fprintf(stderr, "没有找到从%s到%s的路径！\n", path_name[current_pos], path_name[current_pos + 1]);
            return ;
        }
        if (current_pos == 0)
        {
            printf("乘坐");
        }
        else
        {
            printf("换乘到");
            transfer_count++;
        }
        printf("%s(%s) : ", routes[best_route].route_name, best_direction > 0 ? "正向" : "反向");
        int route_time = 0;
        for (int i = current_pos; i <= best_end_index; i++)
        {
            printf("%s", path_name[i]);
            if (i != best_end_index)
            {
                route_time += graph->arc[find_station_in_graph(graph, path_name[i])][find_station_in_graph(graph, path_name[i + 1])];
                printf(" -> ");
            }
        }
        printf(" 用时%d分钟\n", route_time);
        current_pos = best_end_index;
    }
    if (transfer_count == 0)
    {
        printf("本次乘车方案无需换乘，可直达目的地\n");
    }
    else
    {
        printf("本次乘车方案共换乘%d次！\n", transfer_count);
    }
}

void print_routes(Route *routes, const int routes_count)
{
    // 打印路线
    if (!routes || routes_count <= 0)
    {
        fprintf(stderr, "没有路线数据可以显示！\n");
        return;
    }
    printf("所有公交路线如下:\n");
    for (int i = 0; i < routes_count; i++)
    {
        printf("%s（共%d站）: ", routes[i].route_name, routes[i].station_count);
        for (int j = 0; j < routes[i].station_count; j++)
        {
            printf("%s", routes[i].station[j]);
            if (j != routes[i].station_count - 1)
            {
                printf(" -> ");
            }
        }
        printf("\n");
    }
    printf("注意!在所有公交路线中任意两个相邻站点之间的耗时都是5分钟！\n\n");
}


void clear_input_buffer(){
    int c;
    while((c = getchar()) != '\n' && c != EOF);
}

int get_valid_input(const int start, const int end, const char prompt[])
{
    int option;
    while (1)
    {
        if (prompt)
        {
            printf("%s\n", prompt);
        }
        if (scanf("%d", &option) != 1)
        {
            printf("错误输入！请输入%d到%d之间的整数！\n", start, end);
            clear_input_buffer();
            continue;
        }
        if (option >= start && option <= end)
        {
            return option;
        }
        printf("输入不在范围内！请输入%d到%d之间的整数\n", start, end);
        clear_input_buffer();
    }
}

bool get_station_name(Graph *graph, char station_name[], const char prompt[])
{
    if (!graph)
    {
        fprintf(stderr, "错误！get_station_name参数无效！\n");
        return false;
    }
    char name[MAX_STATION_NAME_LENGTH * 2];
    while (1)
    {
        if (prompt)
        {
            printf("%s\n", prompt);
        }
        if (fgets(name, sizeof(name), stdin) == NULL)
        {
            printf("输入读取失败， 请重新输入！\n");
            continue;
        }
        size_t len = strlen(name);
        bool has_newline = (len > 0 && name[len - 1] == '\n');
        if(has_newline){
            name[--len] = '\0';
        }
        if(len == 0){
            printf("输入不能为空，请重新输入！\n");
            continue;
        }
        if(len >= MAX_STATION_NAME_LENGTH){
            printf("站点名称过长（最多%d个字符），请重新输入！\n", MAX_STATION_NAME_LENGTH - 1);
            if(!has_newline){//如果fgets没读完一行，清空缓冲区
                clear_input_buffer();
            }
            continue;
        }
        if (strcmp(name, "exit") == 0)
        {
            printf("感谢使用，再见！");
            return false;
        }
        if (find_station_in_graph(graph, name) != -1)
        {
            strcpy(station_name, name);
            return true;
        }
        fprintf(stderr, "未找到站点%s!请输入有效站点名称！\n", name);
    }
}


Route *allocate_routes()
{
    Route *routes = (Route *)malloc(sizeof(Route) * MAX_ROUTES);
    if (!routes)
    {
        fprintf(stderr, "routes内存分配失败！\n");
        return NULL;
    }
    return routes;
}

Graph *allocate_graph()
{
    Graph *graph = (Graph *)malloc(sizeof(Graph));
    if (!graph)
    {
        fprintf(stderr, "graph内存分配失败！\n");
        return NULL;
    }
    return graph;
}

void clean_resources(Route *routes, Graph *graph)
{
    if (routes)
    {
        free(routes);
    }
    if (graph)
    {
        free(graph);
    }
}

void print_available_stations(const Graph *graph, const int stations_count)
{
    if (!graph || graph->station_count == 0)
    {
        fprintf(stderr, "暂无可用站点！\n");
        return;
    }
    printf("可用站点：");
    int count = 0;
    int station_count = 0;
    while (count < graph->station_count && station_count < stations_count)
    {
        char station_name[MAX_STATION_NAME_LENGTH];
        snprintf(station_name, MAX_STATION_NAME_LENGTH, "S%d", station_count + 1);
        if (find_station_in_graph(graph, station_name) != -1)
        {
            printf("%s", station_name);
            if (count != graph->station_count - 1)
            {
                printf(", ");
            }
            count++;

        }
        station_count++;
    }
    printf("\n");
}

int select_data(Route *routes, Graph *graph, int *_routes_count, int *_station_count, int *_option, int *_option1)
{
    if (!routes || !graph || !_routes_count || !_station_count || !_option || !_option1)
    {
        fprintf(stderr, "错误！select_data参数无效！\n");
        return EXIT_FAILURE;
    }
    int routes_count = 0, stations_count = 0;
    int option = 0, option1 = 0;
    do
    {
        system("cls");
        printf("请选择数据来源\n");
        printf("1.从文件中读取\n");
        printf("2.随机生成数据\n");
        printf("3.退出\n");
        option = get_valid_input(1, 3, "请输入选择（1-3）：");
        switch (option)
        {
            case 1:
                if (check_file_available(FILENAME))
                {

                    if (load_from_file(routes, &routes_count, &stations_count))
                    {
                        printf("读取成功！\n");
                        break;
                    }
                    else
                    {
                        printf("文件读取失败！\n");
                    }
                }
                else
                {
                    printf("文件%s不存在\n", FILENAME);
                }
                return EXIT_FAILURE;
            case 2:
                printf("正在生成随机数据...\n");
                do
                {
                    system("cls");
                    routes_count = generate_routes(routes, &stations_count);
                    printf("成功生成随机数据！\n");
                    print_routes(routes, routes_count);
                    printf("是否要将生成的随机数据存入routes文件中?\n");
                    printf("0.退出    1.是    2.否    3.重新生成数据\n");
                    option1 = get_valid_input(0, 3, "请输入选择（0-3）：");
                    if (option1 == 0)
                    {
                        printf("感谢使用，再见！\n");
                        *_routes_count = routes_count;
                        *_station_count = stations_count;
                        *_option = option;
                        *_option1 = option1;
                        return EXIT_SUCCESS;
                    }
                } while (option1 == 3);
                if (option1 == 1)
                {
                    if (check_file_available(FILENAME))
                    {
                        if (save_to_file(routes, routes_count, stations_count))
                        {
                            printf("生成的随机数据已存入routes文件\n");
                        }
                        else
                        {
                            fprintf(stderr, "数据保存失败！\n");
                        }
                    }
                    else
                    {
                        fprintf(stderr, "文件%s不存在\n", FILENAME);
                    }
                }
                break;
            case 3:
                printf("感谢使用，再见！\n");
                *_routes_count = routes_count;
                *_station_count = stations_count;
                *_option = option;
                *_option1 = option1;
                return EXIT_SUCCESS;
            default:
                printf("未知输入，请重新输入！\n");
                break;
        }
    } while (option != 1 && option != 2);
    *_routes_count = routes_count;
    *_station_count = stations_count;
    *_option = option;
    *_option1 = option1;
    printf("\n");
    return CONTINUE;
}

int shortest_path(Route *routes, Graph *graph, const int routes_count, const int stations_count)
{
    if (!routes || !graph || routes_count < 1 || stations_count < 1)
    {
        fprintf(stderr, "错误！shortest_path参数无效！\n");
        return EXIT_FAILURE;
    }
    system("cls");
    print_routes(routes, routes_count);
    printf("最短路径查询:\n");
    print_available_stations(graph, stations_count);
    printf("\n");
    printf("输入'exit'退出程序\n\n");
    clear_input_buffer();
    char start[MAX_STATION_NAME_LENGTH];
    if (!get_station_name(graph, start, "请输入起点站点："))
    {
        return EXIT_SUCCESS;
    }
    char end[MAX_STATION_NAME_LENGTH];
    if (!get_station_name(graph, end, "请输入终点站点："))
    {
        return EXIT_SUCCESS;
    }
    int path[MAX_STATIONS], path_length = 0, total_time = 0;
    system("cls");
    print_routes(routes, routes_count);
    Dijkstra(graph, routes, routes_count, start, end, path, &path_length, &total_time); // 计算最短路径
    if (path_length > 1)
    {
        transfer(routes, routes_count, graph, path, path_length, total_time); // 生成换乘方案
    }
    else if (path_length == 0)
    {
        printf("未找到可行路径\n");
    }
    fflush(stdout);
    printf("\n");
    return CONTINUE;
}

int continue_search(Route *routes, const int routes_count, const int stations_count, const int option, const int option1)
{
    if (!routes || routes_count < 1 || stations_count < 1)
    {
        fprintf(stderr, "错误！continue_search参数无效！\n");
        return EXIT_FAILURE;
    }
    printf("是否继续查询最短路径?\n");
    printf("1.是     2.否\n");
    const int option2 = get_valid_input(1, 2, "请输入选择（1-2）："); // 询问是否继续
    if (option2 == 2)
    {
        if (option1 != 1 && option != 1)
        {
            printf("是否保存数据到routes文件中?\n");
            printf("1.是     2.否\n");
            const int option3 = get_valid_input(1, 2, "请输入选择（1-2）：");
            if (option3 == 1)
            {
                if (check_file_available(FILENAME))
                {
                    if (save_to_file(routes, routes_count, stations_count))
                    {
                        printf("数据已存入routes文件\n");
                    }
                    else
                    {
                        fprintf(stderr, "数据保存失败！\n");
                    }
                }
                else
                {
                    fprintf(stderr, "文件%s不存在\n", FILENAME);
                }
            }
        }
        return EXIT_SUCCESS; // 不继续查询
    }
    return CONTINUE; // 继续查询
}

int main()
{
    srand((unsigned int)time(NULL));
    SetConsoleCP(CP_UTF8);
    SetConsoleOutputCP(CP_UTF8);//系统初始化
    Route *routes = allocate_routes();
    Graph *graph = allocate_graph();
    if (!routes || !graph)
    {
        clean_resources(routes, graph);
        return EXIT_FAILURE;
    }//内存分配
    int stations_count = 0, routes_count = 0;
    int option = 0, option1 = 0;
    const int function1 = select_data(routes, graph, &routes_count, &stations_count, &option, &option1);
    if (function1 != CONTINUE)//#define CONTINUE 2
    {
        clean_resources(routes, graph);
        return function1;
    }//选择数据来源
    graph_init(graph);
    build_graph(routes, graph, routes_count, stations_count);//构建图
    while (1) // 查询最短路径
    {
        const int function2 = shortest_path(routes, graph, routes_count, stations_count);
        if (function2 != CONTINUE)
        {
            clean_resources(routes, graph);
            return function2;
        }
        const int function3 = continue_search(routes, routes_count, stations_count, option, option1);
        if (function3 == EXIT_FAILURE)
        {
            clean_resources(routes, graph);
            return function3;
        }
        if (function3 == EXIT_SUCCESS)
        {
            break;
        }
        system("cls");
    }
    clean_resources(routes, graph);
    printf("感谢使用，再见！");
    return EXIT_SUCCESS;
}