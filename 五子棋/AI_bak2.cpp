#define SPACE 0
#define HUMAN (nowBlack ? 1 : -1)
#define AI (nowBlack ? -1 : 1)
const int LINES = 15;
int maps[LINES+2][LINES+2];
int score[LINES+2][LINES+2];
#define SPACE 0
#define HUMAN (nowBlack ? 1 : -1)
#define AI (nowBlack ? -1 : 1)
void AIAction(int *p, int *q)
{
	memset(score, 0, sizeof(score));
	calculateScore();
	int maxScore=0, x=0, y=0;
	for (int i=0; i<LINES; i++)
	{
		for (int j=0; j<LINES; j++)
		{
			if (maps[i][j] == 0)
			{
				if (score[i][j] > maxScore)
				{
					maxScore = score[i][j];
					x=i, y=j; 
				}
			}
		}
	}
	*q = x;
	*p = y;
}
// 最关键的计算评分函数
void calculateScore()
{
    // 统计玩家或者电脑连成的子
    int humanNum = 0; // 玩家连成子的个数
    int robotNum = 0; // AI连成子的个数
    int emptyNum = 0; // 各方向空白位的个数

    // 计分（此处是完全遍历，其实可以用bfs或者dfs加减枝降低复杂度，通过调整权重值，调整AI智能程度以及攻守风格）
    for (int row = 0; row < LINES; row++)
        for (int col = 0; col < LINES; col++)
        {
            // 空白点就算
            if (row >= 0 && col >= 0 &&
                maps[row][col] == 0)
            {
                // 遍历周围八个方向
                for (int y = -1; y <= 1; y++)
                    for (int x = -1; x <= 1; x++)
                    {
                        // 重置
                        humanNum = 0;
                        robotNum = 0;
                        emptyNum = 0;

                        // 原坐标不算
                        if (!(y == 0 && x == 0))
                        {
                            // 每个方向延伸4个子

                            // 对玩家白子评分（正反两个方向）
                            for (int i = 1; i <= 4; i++)
                            {
                                if (row + i * y >= 0 && row + i * y < LINES &&
                                    col + i * x >= 0 && col + i * x < LINES &&
                                    maps[row + i * y][col + i * x] == HUMAN) // 玩家的子
                                {
                                    humanNum++;
                                }
                                else if (row + i * y >= 0 && row + i * y < LINES &&
                                         col + i * x >= 0 && col + i * x < LINES &&
                                         maps[row + i * y][col + i * x] == 0) // 空白位
                                {
                                    emptyNum++;
                                    break;
                                }
                                else            // 出边界
                                    break;
                            }

                            for (int i = 1; i <= 4; i++)
                            {
                                if (row - i * y >= 0 && row - i * y < LINES &&
                                    col - i * x >= 0 && col - i * x < LINES &&
                                    maps[row - i * y][col - i * x] == HUMAN) // 玩家的子
                                {
                                    humanNum++;
                                }
                                else if (row - i * y >= 0 && row - i * y < LINES &&
                                         col - i * x >= 0 && col - i * x < LINES &&
                                         maps[row - i * y][col - i * x] == 0) // 空白位
                                {
                                    emptyNum++;
                                    break;
                                }
                                else            // 出边界
                                    break;
                            }

                            if (humanNum == 1)                      // 杀二
                                score[row][col] += 10;
                            else if (humanNum == 2)                 // 杀三
                            {
                                if (emptyNum == 1)
                                    score[row][col] += 30;
                                else if (emptyNum == 2)
                                    score[row][col] += 40;
                            }
                            else if (humanNum == 3)                 // 杀四
                            {
                                // 量变空位不一样，优先级不一样
                                if (emptyNum == 1)
                                    score[row][col] += 60;
                                else if (emptyNum == 2)
                                    score[row][col] += 110;
                            }
                            else if (humanNum == 4)                 // 杀五
                                score[row][col] += 10100;

                            // 进行一次清空
                            emptyNum = 0;

                            // 对AI黑子评分
                            for (int i = 1; i <= 4; i++)
                            {
                                if (row + i * y >= 0 && row + i * y < LINES &&
                                    col + i * x >= 0 && col + i * x < LINES &&
                                    maps[row + i * y][col + i * x] == HUMAN) // 玩家的子
                                {
                                    robotNum++;
                                }
                                else if (row + i * y >= 0 && row + i * y < LINES &&
                                         col + i * x >= 0 && col + i * x < LINES &&
                                         maps[row +i * y][col + i * x] == 0) // 空白位
                                {
                                    emptyNum++;
                                    break;
                                }
                                else            // 出边界
                                    break;
                            }

                            for (int i = 1; i <= 4; i++)
                            {
                                if (row - i * y >= 0 && row - i * y < LINES &&
                                    col - i * x >= 0 && col - i * x < LINES &&
                                    maps[row - i * y][col - i * x] == AI) // AI的子
                                {
                                    robotNum++;
                                }
                                else if (row - i * y > 0 && row - i * y < LINES &&
                                         col - i * x > 0 && col - i * x < LINES &&
                                         maps[row - i * y][col - i * x] == 0) // 空白位
                                {
                                    emptyNum++;
                                    break;
                                }
                                else            // 出边界
                                    break;
                            }

                            if (robotNum == 0)                      // 普通下子
                                score[row][col] += 5;
                            else if (robotNum == 1)                 // 活二
                                score[row][col] += 10;
                            else if (robotNum == 2)
                            {
                                if (emptyNum == 1)                // 死三
                                    score[row][col] += 25;
                                else if (emptyNum == 2)
                                    score[row][col] += 50;  // 活三
                            }
                            else if (robotNum == 3)
                            {
                                if (emptyNum == 1)                // 死四
                                    score[row][col] += 55;
                                else if (emptyNum == 2)
                                    score[row][col] += 100; // 活四
                            }
                            else if (robotNum >= 4)
                                score[row][col] += 10000;   // 活五

                        }
                    }

            }
        }
    
//    for (int i=0; i<LINES; i++)
//    {
//    	for (int j=0; j<LINES; j++)
//    	{
//    		if (score[i][j] != 0)
//    		{
//    			printf("i=%d, j=%d, score[i][j]=%d\n", i, j, score[i][j]);
//			}
//		}
//	}
}
