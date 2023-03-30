//by Radiance
#include <iostream>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <algorithm>
#include <queue>
#include <ctime>
#define ll long long
using namespace std;
const int maxn = 1145141;
const int maxm = 114;

char buf[100005], *p1 = buf, *p2 = buf;
char nc(){
    if(p1 == p2){
        p1 = buf;
        p2 = p1+fread(buf, 1, 100000, stdin);
        if(p1 == p2)return EOF;
    }
    return *p1++;
}

int read(){
    int ret = 0;
    int tag = 1;
    char c = nc();
    while(c < '0' || c > '9'){
        if(c == '-')tag = -1;
        c = nc();
    }
    while('0' <= c && c <= '9'){
        ret = (ret<<1)+(ret<<3)+c-'0';
        c = nc();
    }
    return ret*tag;
}//fast read

struct card{
    int id;//id 用于知道这是那张牌
    int cost;// 费用
    int discard;// 弃牌数
    int charge;// 挑费数
    int draw;// 抽牌数
    int pri_normal;// 出牌优先级
    int pri_awake;// 出牌优先级-7pp后
    int pri_discard;// 弃牌优先级
    int sid; //初始化牌组时用的内置id
    bool act; //防止每回合多次判定同一张牌
    card(){};
    card(int id, int cost, int discard, int charge, int draw, int pri_normal, int pri_awake, int pri_discard){
        this -> id = id;
        this -> cost = cost;
        this -> discard = discard;
        this -> charge = charge;
        this -> draw = draw;
        this -> pri_normal = pri_normal;
        this -> pri_awake = pri_awake;
        this -> pri_discard = pri_discard;
    }
};//card

struct game{
    card init_deck[maxm];//牌组
    queue<card> hand;//手牌
    queue<card> deck;//牌库
    int pp;//最大pp
    int pp_re;//剩余pp
    bool offen;//先攻
    bool awake;//觉醒
    int cnt;//牌组内卡牌数量，init_deck后应该恒为40
}g;//game

int round;
double gold_round;//金龙到手回合
double tenpp_round;//10pp回合
double both_tenpp_gold;//两达成的回合
double sixr_pp;//第6回合pp
bool tenpp;//各种锁
bool gold;
bool both;
bool drop_4;
double num_tenpp;//10pp概率
double num_both;//10pp+金龙的概率
double num_gold;//金龙概率
double num_six_gold;//6回合9pp+龙概率
double pp_6;//6回合9pp概率
double drop_round;//4弃牌概率
int cnt_drop;

void write(card c){
    printf("%d %d %d %d %d %d %d %d\n", c.id,c.cost,c.discard,c.charge,c.draw,c.pri_normal,c.pri_awake,c.pri_discard);
}//调试用

void pri(){
    cout<<round<<endl;
    cout<<g.pp<<" "<<g.offen<<" "<<g.pp_re<<endl;

    card li[maxm];
    int cnt = 0;
    while(!g.hand.empty()){
        li[++cnt] = g.hand.front();
        g.hand.pop();
        write(li[cnt]);
    }
    for(int i = 1;i <= cnt;i++){
        g.hand.push(li[i]);
    }
    cout<<endl;

}//调试用 打印手牌和对局信息

void init_card(int id, int cost, int discard, int charge, int draw, int pri_normal, int pri_awake, int pri_discard, int num){
    card c = card(id, cost, discard, charge, draw, pri_normal, pri_awake, pri_discard);

    for(int i = 1;i <= num;i++){
        g.init_deck[++g.cnt] = c;
    }
}//初始每张牌

void prepare_deck(){
    int n = read();
    int inpu[maxm];
    for(int i = 1;i <= n;i++){
        for(int j = 1;j <= 9;j++){
            inpu[j] = read();
        }
        init_card(inpu[1], inpu[2], inpu[3], inpu[4], inpu[5], inpu[6], inpu[7], inpu[8], inpu[9]);
    }
}//初始化牌组

void draw_card(int num){
    for(int i = 1;i <= num;i++){
        if(g.deck.empty())return;
        card c = g.deck.front();
        g.deck.pop();
        if(c.id == 8 && !gold){
            gold = true;
            gold_round += round;//计数金龙回合
        }
        if(g.hand.size() == 9)continue;
        g.hand.push(c);
    }
}//抽num张牌

bool random(int x, int n){
    int p = rand()%n+1;
    if(x >= p)return true;
    return false;
}//判定

void exchange_init_card(){
    card li[5];
    for(int i = 1;i <= 3;i++){
        li[i] = g.hand.front();
        g.hand.pop();
        if(random(li[i].pri_discard, 10)){
            draw_card(1);
            g.deck.push(li[i]);
        }else{
            g.hand.push(li[i]);
        }
    }
}//初始交换手牌

void increase_pp(int x){
    g.pp = min(g.pp+x, 10);
    if(g.pp >= 7)g.awake = true;//觉醒

    if(g.pp >= 10 && !tenpp){
        tenpp = true;//计数10pp回合
        tenpp_round += round;
    }
}//增长最大pp

bool cmp0(card a, card b){
    return a.sid < b.sid;
}

void init_game(){
    while(!g.hand.empty())g.hand.pop();
    while(!g.deck.empty())g.deck.pop();
    for(int i = 1;i <= 40;i++){
        g.init_deck[i].sid = rand();//为每张牌赋随机值，实现每次对局开始时牌库随机排列
    }
    sort(g.init_deck+1, g.init_deck+41, cmp0);
    for(int i = 1;i <= 40;i++){
        g.deck.push(g.init_deck[i]);
    }
    round = 1;
    g.offen = rand()&1;
    draw_card(3);
    exchange_init_card();//初始换牌
    if(!g.offen)draw_card(1);//后手多抽一张
    g.pp = 0;//各种锁和信息的初始化
    g.awake = false;
    both = false;
    gold = false;
    tenpp = false;
    drop_4 = false;
    cnt_drop = 0;
}

void init_round(){
    increase_pp(1);
    draw_card(1);
    g.pp_re = g.pp;
}//初始化回合信息

bool cmp1(card a, card b){
    if(g.awake)return a.pri_awake > b.pri_awake;
    return a.pri_normal > b.pri_normal;
}

bool cmp2(card a, card b){
    return a.pri_discard < b.pri_discard;
}

bool drop_card(int num){
    card li[maxm];
    int n = g.hand.size();
    if(num > n)return false;
    for(int i = 1;i <= n;i++){
        li[i] = g.hand.front();
        g.hand.pop();
    }

    sort(li+1, li+n+1, cmp2);//把手牌拿出来按弃牌优先级排序
    int k = 0;
    for(int i = 1;i <= num;i++){
        k += 10-li[i].pri_discard;
    }//取出前num张的权值和
    if(random(k, 10*num)){//判定是否弃牌
        int p = 0;//判定成功
        for(int i = 1;i <= num;i++){
            if(li[i].id == 6)p++;//如果被弃掉的是怪力抽1
        }
        for(int i = num+1;i <= n;i++){
            g.hand.push(li[i]);//没被弃的塞回手牌
        }
        draw_card(p);
        cnt_drop++;
        if(cnt_drop >= 4 && !drop_4){
            drop_4 = true;//计数弃牌
            drop_round += round;
        }
        return true;
    }else{//判定失败
        for(int i = 1;i <= n;i++){
            g.hand.push(li[i]);//全塞回去
        }
        return false;
    }
}

void play_drakan(){
    card c = card(2, 2, 2, 0, 0, 3, 3, 1);
    if(g.hand.size() < 9)g.hand.push(c);//塞一张token
}//龙之魔女

void play_drakan_token(){
    card c1 = card(0, 0, 0, 0, 0, 1, 1, 0);
    card c2 = card(0, 2, 0, 0, 0, 1, 1, 0);
    if(g.hand.size() < 9)g.hand.push(c1);
    if(g.hand.size() < 9)g.hand.push(c2);//赛两张token
}//龙之魔女token

void play_silong(){
    if(round >= 4+g.offen)increase_pp(1);//进化回合后跳费
    card c = card(0, 1, 0, 0, 0, 4, 4, 0);
    if(g.hand.size() < 9)g.hand.push(c);//赛一张token
}

void play_qishi(){
    if(g.awake)draw_card(1);//龙之启示， 觉醒后抽1
}

void play_chaomuo(){
    if(g.awake){
        g.pp_re += 2;
        draw_card(2);//恩泽（太tm超模了！）觉醒后抽2回2pp
    }
}

void play_silver(){
    if(cnt_drop >= 2)draw_card(3);//银龙抽3
}

void play_suishilong(){
    if(g.awake)draw_card(1);//觉醒额外抽1
}

bool play_card(card c){
    //出牌
    // cout<<"Play a card"<<endl;
    // write(c);
    if(g.pp_re < c.cost)return false;
    if(c.discard > 0){
        if(!drop_card(c.discard))return false;
    }
    increase_pp(c.charge);//跳费
    if(c.id == 1)play_drakan();
    if(c.id == 2)play_drakan_token();
    if(c.id == 3)play_silong();
    if(c.id == 4)play_qishi();
    if(c.id == 5)play_chaomuo();
    if(c.id == 7)play_silver();
    if(c.id == 9)play_suishilong();
    //监狱的从牌组把自己给抽出来懒得写了（
    draw_card(c.draw);//抽卡
    g.pp_re -= c.cost;//扣除费用
    // cout<<"success and now "<<endl;
    // pri();
    
    return true;
}

void play_round(){
    for(int i = 1;i <= 20;i++){
        if(g.pp_re <= 0)break;
        int n = g.hand.size();
        card li[maxm];
        for(int j = 1;j <= n;j++){
            li[j] = g.hand.front();
            li[j].act = true;
            g.hand.pop();
        }//把手牌都从队列拿出来
        sort(li+1, li+n+1, cmp1);//按出牌优先级排序
        int p = 1;
        bool tag = false;
        for(;p <= n;p++){
            if(li[p].cost <= g.pp_re && li[p].act){
                tag = true;
                for(int j = 1;j <= n;j++){
                    if(j != p){
                        g.hand.push(li[j]);
                    }
                }//把其他手牌塞回去，出卡
                if(!play_card(li[p])){
                    li[p].act = false;//失败了这张也要塞回手牌，同时锁定这张牌
                    g.hand.push(li[p]);
                }
                break;
            }
        }
        if(!tag){
            for(int j = 1;j <= n;j++){
                        g.hand.push(li[j]);//1张没出，全塞回去
            }
        }
    }
}


void work(){
    init_game();//初始化游戏
    round = 1;
    for(int i = 1;i <= 10;i++){
        round = i;
        init_round();//初始化回合信息
        if(gold && tenpp && (!both)){//各种判定
            both = true;
            both_tenpp_gold += round;
        }
        if(i == 7 && tenpp == true){
            num_tenpp += 1;
        }
        if(i == 7 && both == true){
            num_both += 1;
        }
        if(i == 7 && gold == true){
            num_gold += 1;
        }
        if(i == 6){
            sixr_pp += g.pp;
        }
        if(i == 6 && gold && g.pp >= 9){
            num_six_gold += 1;
        }
        play_round();//出牌
    }
}

int main(){
    #ifndef ONLINE_JUDGE
    freopen("data.in", "r", stdin);
    freopen("test.out", "w", stdout);
    #endif

    srand(time(0));
    prepare_deck();
   

    int repeat = 1000000;
    double num = repeat;
    while(repeat--){
        work();
    }

    printf("平均第6回合开始时有%lf点pp\n", sixr_pp/num);
    printf("平均第%lf回合弃牌4次\n", drop_round/num);
    printf("平均第%lf回合抽到金龙\n", gold_round/num);
    printf("平均第%lf回合达到10pp\n", tenpp_round/num);
    printf("平均第%lf回合在回合开始时同时达成两者\n", both_tenpp_gold/num);
    printf("有%lf的几率在第6回合开始时有9pp和一张金龙\n", num_six_gold/num);
    printf("有%lf的几率在第7回合开始时有至少一张金龙\n", num_gold/num);
    printf("有%lf的几率在第7回合开始时有10pp\n", num_tenpp/num);
    printf("有%lf的几率在第7回合开始时有10pp和一张金龙\n", num_both/num);
    return 0;
}