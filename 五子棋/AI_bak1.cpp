#define SPACE 0
#define HUMAN (nowBlack ? 1 : -1)
#define AI (nowBlack ? -1 : 1)
const int LINES = 15;
int maps[LINES+2][LINES+2];
int score[LINES+2][LINES+2];

void AIAction(int *p,int *q);     /* 电脑下子 p q返回下子坐标 */
int value(int p,int q);     /* 计算空点p q的价值 */
int qixing(int n,int p,int q);    /* 返回空点p q在n方向上的棋型 n为1-8方向 从右顺时针开始数 */
void yiwei(int n,int *i,int *j);   /* 在n方向上对坐标 i j 移位 n为1-8方向 从右顺时针开始数  */

void AIAction(int *p,int *q)           /* 电脑下子 *p *q返回下子坐标 */
{
	int i,j,k,max=0,I,J;        /* I J为下点坐标 */
	for(j=0; j<15; j++)
		for(i=0; i<15; i++)
			if(maps[j][i]==SPACE)       /* 历遍棋盘，遇到空点则计算价值，取最大价值点下子。 */
			{
				k=value(i,j);
				if(k>=max)
				{
					I=i;
					J=j;
					max=k;
				}
			}
	*p=I;
	*q=J;
}
int value(int p,int q) /* 计算空点p q的价值 以k返回 */
{
	int n=1,k=0,k1,k2,K1,K2,X1,Y1,Z1,X2,Y2,Z2,temp;
	int a[2][4][4]= {40,400,3000,10000,6,10,600,10000,20,120,200,0,6,10,500,0,30,300,2500,5000,2,8,300,8000,26,160,0,0,4,20,300,0};
	/* 数组a中储存己方和对方共32种棋型的值  己方0对方1    活0冲1空活2空冲3    子数0-3（0表示1个子，3表示4个子） */
	while(n!=5)
	{
		k1=qixing(n,p,q);
		n+=4;                        /* k1,k2为2个反方向的棋型编号 */
		k2=qixing(n,p,q);
		n-=3;
		if(k1>k2)
		{
			temp=k1;    /* 使编号小的为k1,大的为k2 */
			k1=k2;
			k2=temp;
		}
		K1=k1;
		K2=k2;       /* K1 K2储存k1 k2的编号 */
		Z1=k1%10;
		Z2=k2%10;
		k1/=10;
		k2/=10;
		Y1=k1%10;
		Y2=k2%10;
		k1/=10;
		k2/=10;
		X1=k1%10;
		X2=k2%10;
		/* X Y Z分别表示 己方0对方1    活0冲1空活2空冲3    子数0-3（0表示1个子，3表示4个子） */
		if(K1==-1)
		{
			if(K2<0)
			{
				k+=0;
				continue;
			}
			else k+=a[X2][Y2][Z2]+5;
			continue;
		};    /* 空棋型and其他 */
		if(K1==-2)
		{
			if(K2<0)
			{
				k+=0;
				continue;
			}
			else k+=a[X2][Y2][Z2]/2;
			continue;
		};       /* 边界冲棋型and其他 */
		if(K1==-3)
		{
			if(K2<0)
			{
				k+=0;
				continue;
			}
			else k+=a[X2][Y2][Z2]/3;
			continue;
		};    /* 边界空冲棋型and其他 */
		if(((K1>-1&&K1<4)&&((K2>-1&&K2<4)||(K2>9&&K2<14)))||((K1>99&&K1<104)&&((K2>99&&K2<104)||(K2>109&&K2<114))))
		{
			/* 己活己活 己活己冲 对活对活 对活对冲 的棋型赋值*/
			if(Z1+Z2>=2)
			{
				k+=a[X2][Y2][3];
				continue;
			}
			else
			{
				k+=a[X2][Y2][Z1+Z2+1];
				continue;
			}
		}
		if(((K1>9&&K1<14)&&(K2>9&&K2<14))||((K1>109&&K1<114)&&(K2>109&&K2<114)))
		{
			/* 己冲己冲 对冲对冲 的棋型赋值*/
			if(Z1+Z2>=2)
			{
				k+=10000;
				continue;
			}
			else
			{
				k+=0;
				continue;
			}
		}
		if(((K1>-1&&K1<4)&&((K2>99&&K2<104)||(K2>109&&K2<114)))||((K1>9&&K1<14)&&((K2>99&&K2<104)||(K2>109&&K2<114))))
		{
			/* 己活对活 己活对冲 己冲对活 己冲对冲 的棋型赋值*/
			if(Z1==3||Z2==3)
			{
				k+=10000;
				continue;
			}
			else
			{
				k+=a[X2][Y2][Z2]+a[X1][Y1][Z1]/4;
				continue;
			}
		}
		else
		{
			k+=a[X1][Y1][Z1]+a[X2][Y2][Z2];    /* 其他棋型的赋值 */
			continue;
		}
	}
	return k;
}
int qixing(int n,int p,int q)            /* 返回空点p q在n方向上的棋型号 n为1-8方向 从右顺时针开始数 */
{
	int k,m=0;  /* 棋型号注解:  己活000-003 己冲010-013 对活100-103 对冲110-113 己空活020-023 己空冲030-033 对空活120-123 对空冲130-133 空-1 边界冲-2 边界空冲-3*/
	yiwei(n,&p,&q);
	if(p<0||p>14||q<0||q>14) k=-2;                                      /* 边界冲棋型 */
	switch(maps[q][p])
	{
		case AI:
		{
			m++;
			yiwei(n,&p,&q);
			if(p<0||p>14||q<0||q>14)
			{
				k=m+9;
				return k;
			}
			while(maps[q][p]==AI)
			{
				m++;
				yiwei(n,&p,&q);
				if(p<0||p>14||q<0||q>14)
				{
					k=m+9;
					return k;
				}
			}
			if(maps[q][p]==SPACE) k=m-1;                         /* 己方活棋型 */
			else k=m+9;                                         /* 己方冲棋型 */
		}
		break;
		case HUMAN:
		{
			m++;
			yiwei(n,&p,&q);
			if(p<0||p>14||q<0||q>14)
			{
				k=m+109;
				return k;
			}
			while(maps[q][p]==HUMAN)
			{
				m++;
				yiwei(n,&p,&q);
				if(p<0||p>14||q<0||q>14)
				{
					k=m+109;
					return k;
				}
			}
			if(maps[q][p]==SPACE) k=m+99;                         /* 对方活棋型 */
			else k=m+109;                                        /* 对方冲棋型 */
		}
		break;
		case SPACE:
		{
			yiwei(n,&p,&q);
			if(p<0||p>14||q<0||q>14)
			{
				k=-3;    /* 边界空冲棋型 */
				return k;
			}
			switch(maps[q][p])
			{
				case AI:
				{
					m++;
					yiwei(n,&p,&q);
					if(p<0||p>14||q<0||q>14)
					{
						k=m+29;
						return k;
					}
					while(maps[q][p]==AI)
					{
						m++;
						yiwei(n,&p,&q);
						if(p<0||p>14||q<0||q>14)
						{
							k=m+29;
							return k;
						}
					}
					if(maps[q][p]==SPACE) k=m+19;                      /* 己方空活棋型 */
					else k=m+29;                                      /* 己方空冲棋型 */
				}
				break;
				case HUMAN:
				{
					m++;
					yiwei(n,&p,&q);
					if(p<0||p>14||q<0||q>14)
					{
						k=m+129;
						return k;
					}
					while(maps[q][p]==HUMAN)
					{
						m++;
						yiwei(n,&p,&q);
						if(p<0||p>14||q<0||q>14)
						{
							k=m+129;
							return k;
						}
					}
					if(maps[q][p]==SPACE) k=m+119;                     /* 对方空活棋型 */
					else k=m+129;                                     /* 对方空冲棋型 */
				}
				break;
				case SPACE:
					k=-1;
					break;                                         /* 空棋型 */
			}
		}
		break;
	}
	return k;
}
void yiwei(int n,int *i,int *j)        /* 在n方向上对坐标 i j 移位 n为1-8方向 从右顺时针开始数 */
{
	switch(n)
	{
		case 1:
			*i+=1;
			break;
		case 2:
			*i+=1;
			*j+=1;
			break;
		case 3:
			*j+=1;
			break;
		case 4:
			*i-=1;
			*j+=1;
			break;
		case 5:
			*i-=1;
			break;
		case 6:
			*i-=1;
			*j-=1;
			break;
		case 7:
			*j-=1;
			break;
		case 8:
			*i+=1;
			*j-=1;
			break;
	}
}
