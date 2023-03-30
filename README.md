# 牌组分析

## 基本架构

#### 每张卡牌信息

1. id -- 卡牌编号，用于了解这是那张牌
2. cost -- 费用
3. discard -- 弃牌张数，如果为0则不弃牌
4. charge -- 跳费
5. draw -- 抽牌
6. pri_normal -- 出牌权重
7. pri_awake -- 出牌权重**觉醒后**
8. pri_discard -- 弃牌权重

#### 每场对局

1. 给40张牌随机排序
2. 50%概率先手
3. 抽3张牌
4. 初始更换手牌（和弃牌机制一样是概率更换）
5. 后手多抽一张
6. 初始化pp

#### 每个回合

1. pp+1，剩余pp = 最大pp
2. 抽牌
3. 按照出牌权重排序
4. 尝试play权重最大且cost够的卡
5. 若有弃牌判定弃牌（详见弃牌机制
6. 根据卡牌信息执行卡牌
7. 返回
8. 每回合最多尝试出牌20次

#### 弃牌机制

1. 主要是为了模拟实际对局中，因为场面压力过大或者卡手不愿意弃牌的场景
2. 并不是很成熟，有很大改进空间
3. 每张卡牌有自己的弃牌权重，权重越大说明这张卡牌越不希望被丢弃
4. 弃牌成功概率为 $\sum (10-被弃牌权重)/10*弃牌数量$，通过生成随机数大小进行判定





## 一些不足

1. 有一些卡牌，比如监狱，和实际对局（监狱被我改成当场抽一
2. 没有场面压力，实际对局快攻的时候需要放弃一些节奏
3. 牌序很垃圾，比如你2费龙启，4费可以先碎石龙再银龙，但是这个程序会先银龙再碎石龙
4. 牌序很垃圾2，比如回合开始你有6pp,一张碎石龙和一张恩泽，可以先抽卡看看有没有其他挑费，然后再使用恩泽，但是这个程序一定会先使用恩泽
5. 运营很垃圾，比如你2费手上有一张启示，又有银龙和一张龙之魔女，你可以先龙女弃牌然后下回合银龙，但这这个程序一定会用费启示
6. 有理想于实际情况的地方，也有劣于实际情况的地方，综合来看我觉得还是会比实际情况劣一些，实际的各种概率应该会程序模拟出的概率高一点
7. 懒得算置信区间啥的了，谁数理统计学的好帮我算一下（bushi

### 数据格式

#### data.in

1. 第一行是卡牌种类数
2. 之后每一行前8个值是卡牌基础信息（见“每张卡牌信息”），最后一个值是在卡组内编入几张



### 为什么是弃牌龙而不是其他卡组

1. 由于弃牌龙拥有大量的回血和优质返场，所以前期相对自闭，决策**相较于其他卡组较为**简单，比较适合贪心模型
2. 弃牌龙的机制比较简单，我一想如果要写融合，回手，顺念啥的就头大
3. 我本人被弃牌龙打急了(=>) 