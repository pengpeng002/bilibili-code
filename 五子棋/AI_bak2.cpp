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
// ��ؼ��ļ������ֺ���
void calculateScore()
{
    // ͳ����һ��ߵ������ɵ���
    int humanNum = 0; // ��������ӵĸ���
    int robotNum = 0; // AI�����ӵĸ���
    int emptyNum = 0; // ������հ�λ�ĸ���

    // �Ʒ֣��˴�����ȫ��������ʵ������bfs����dfs�Ӽ�֦���͸��Ӷȣ�ͨ������Ȩ��ֵ������AI���̶ܳ��Լ����ط��
    for (int row = 0; row < LINES; row++)
        for (int col = 0; col < LINES; col++)
        {
            // �հ׵����
            if (row >= 0 && col >= 0 &&
                maps[row][col] == 0)
            {
                // ������Χ�˸�����
                for (int y = -1; y <= 1; y++)
                    for (int x = -1; x <= 1; x++)
                    {
                        // ����
                        humanNum = 0;
                        robotNum = 0;
                        emptyNum = 0;

                        // ԭ���겻��
                        if (!(y == 0 && x == 0))
                        {
                            // ÿ����������4����

                            // ����Ұ������֣�������������
                            for (int i = 1; i <= 4; i++)
                            {
                                if (row + i * y >= 0 && row + i * y < LINES &&
                                    col + i * x >= 0 && col + i * x < LINES &&
                                    maps[row + i * y][col + i * x] == HUMAN) // ��ҵ���
                                {
                                    humanNum++;
                                }
                                else if (row + i * y >= 0 && row + i * y < LINES &&
                                         col + i * x >= 0 && col + i * x < LINES &&
                                         maps[row + i * y][col + i * x] == 0) // �հ�λ
                                {
                                    emptyNum++;
                                    break;
                                }
                                else            // ���߽�
                                    break;
                            }

                            for (int i = 1; i <= 4; i++)
                            {
                                if (row - i * y >= 0 && row - i * y < LINES &&
                                    col - i * x >= 0 && col - i * x < LINES &&
                                    maps[row - i * y][col - i * x] == HUMAN) // ��ҵ���
                                {
                                    humanNum++;
                                }
                                else if (row - i * y >= 0 && row - i * y < LINES &&
                                         col - i * x >= 0 && col - i * x < LINES &&
                                         maps[row - i * y][col - i * x] == 0) // �հ�λ
                                {
                                    emptyNum++;
                                    break;
                                }
                                else            // ���߽�
                                    break;
                            }

                            if (humanNum == 1)                      // ɱ��
                                score[row][col] += 10;
                            else if (humanNum == 2)                 // ɱ��
                            {
                                if (emptyNum == 1)
                                    score[row][col] += 30;
                                else if (emptyNum == 2)
                                    score[row][col] += 40;
                            }
                            else if (humanNum == 3)                 // ɱ��
                            {
                                // �����λ��һ�������ȼ���һ��
                                if (emptyNum == 1)
                                    score[row][col] += 60;
                                else if (emptyNum == 2)
                                    score[row][col] += 110;
                            }
                            else if (humanNum == 4)                 // ɱ��
                                score[row][col] += 10100;

                            // ����һ�����
                            emptyNum = 0;

                            // ��AI��������
                            for (int i = 1; i <= 4; i++)
                            {
                                if (row + i * y >= 0 && row + i * y < LINES &&
                                    col + i * x >= 0 && col + i * x < LINES &&
                                    maps[row + i * y][col + i * x] == HUMAN) // ��ҵ���
                                {
                                    robotNum++;
                                }
                                else if (row + i * y >= 0 && row + i * y < LINES &&
                                         col + i * x >= 0 && col + i * x < LINES &&
                                         maps[row +i * y][col + i * x] == 0) // �հ�λ
                                {
                                    emptyNum++;
                                    break;
                                }
                                else            // ���߽�
                                    break;
                            }

                            for (int i = 1; i <= 4; i++)
                            {
                                if (row - i * y >= 0 && row - i * y < LINES &&
                                    col - i * x >= 0 && col - i * x < LINES &&
                                    maps[row - i * y][col - i * x] == AI) // AI����
                                {
                                    robotNum++;
                                }
                                else if (row - i * y > 0 && row - i * y < LINES &&
                                         col - i * x > 0 && col - i * x < LINES &&
                                         maps[row - i * y][col - i * x] == 0) // �հ�λ
                                {
                                    emptyNum++;
                                    break;
                                }
                                else            // ���߽�
                                    break;
                            }

                            if (robotNum == 0)                      // ��ͨ����
                                score[row][col] += 5;
                            else if (robotNum == 1)                 // ���
                                score[row][col] += 10;
                            else if (robotNum == 2)
                            {
                                if (emptyNum == 1)                // ����
                                    score[row][col] += 25;
                                else if (emptyNum == 2)
                                    score[row][col] += 50;  // ����
                            }
                            else if (robotNum == 3)
                            {
                                if (emptyNum == 1)                // ����
                                    score[row][col] += 55;
                                else if (emptyNum == 2)
                                    score[row][col] += 100; // ����
                            }
                            else if (robotNum >= 4)
                                score[row][col] += 10000;   // ����

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
