### 322、零钱兑换

给你一个整数数组 `coins` ，表示不同面额的硬币；以及一个整数 `amount` ，表示总金额。

计算并返回可以凑成总金额所需的 **最少的硬币个数** 。如果没有任何一种硬币组合能组成总金额，返回 `-1` 。

你可以认为每种硬币的数量是无限的。

**示例 1：**

```
输入：coins = [1, 2, 5], amount = 11
输出：3 
解释：11 = 5 + 5 + 1
```

**示例 2：**

```
输入：coins = [2], amount = 3
输出：-1
```

**示例 3：**

```
输入：coins = [1], amount = 0
输出：0
```

> 举个例子理解最优子结构吧
> DP[i]的定义是：凑到i这么多钱，需要最少多少个硬币。
>
> 那么对于[1,3,4,5]，要凑7块钱的计算方法有4种：
> 选一个硬币1: 需要的数量就是1 + DP[7-1] = 1 + DP[6]， 也就是选一个硬币+DP[6]的量
> 选一个硬币3: 需要的数量就是1 + DP[7-3] = 1 + DP[4]
> 选一个硬币4: 需要的数量就是1 + DP[7-4] = 1 + DP[3]
> 选一个硬币5: 需要的数量就是1 + DP[7-5] = 1 + DP[2]
>
> 综合下来选最小：
> DP[7] = min(1 + DP[6], 1 + DP[4], 1 + DP[3], 1 + DP[2])

```c++
// 版本一
class Solution {
public:
    int coinChange(vector<int>& coins, int amount) {
        vector<int> dp(amount + 1, INT_MAX);
        dp[0] = 0;
        for (int i = 0; i < coins.size(); i++) { // 遍历物品
            for (int j = coins[i]; j <= amount; j++) { // 遍历背包
                if (dp[j - coins[i]] != INT_MAX) { // 如果dp[j - coins[i]]是初始值则跳过
                    dp[j] = min(dp[j - coins[i]] + 1, dp[j]);
                }
            }
        }
        if (dp[amount] == INT_MAX) return -1;
        return dp[amount];
    }
};
```

