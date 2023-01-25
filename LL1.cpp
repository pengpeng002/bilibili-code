#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <set>
#include <fstream>
#include <cstring>

#define MAX 100
using namespace std;
int flag = 1;

// 产生式的数据结构
struct node
{
	char left; //左部一个非终结符字母
	string right; //右部一个字符串
};

static int T;
static node Pro[MAX]; // 产生式

//set的特性是，所有元素都会根据元素的键值自动排序
//vector 是向量类型，它可以容纳许多类型的数据
static set<char> firstSet[MAX]; // First集
static set<char> followSet[MAX]; // Follow集
static vector<char> terminal; // 终结符
static vector<char> terminalN; // 去空的终结符
static vector<char> nonterminal; // 非终结符
static vector<char> analyticalS; // 分析栈
static vector<char> RemainE; // 剩余输入串
static int forecastAnaly[100][100]; // 预测表

//判断是否为非终结符
bool IsNonterminal(char c)
{
	if (c >= 'A' && c <= 'Z')
		return true;
	return false;
}

// 获得非终结符集合中的元素下标
int GetindexN(char c)
{
	for (int i = 0; i < nonterminal.size(); i++)
	{
		if (c == nonterminal[i])
			return i;
	}
	return -1;
}

// 获得终结符集合中的元素下标
int Getindex(char c)
{
	for (int i = 0; i < terminalN.size(); i++)
	{
		if (c == terminalN[i])
			return i;
	}
	return -1;
}

// 求非终结符X的的First集
void GFirstSet(char X)
{
	int countEmpty = 0; // 用于最后判断右部非终结符的First集中，是不是都含有$
	int hasEmpty = 0;//First(X)集中是否含有$
	int Xindex = GetindexN(X);//非终结符在非终结符表中的位置
	for (int i = 0; i < T; i++)
	{
		// 匹配产生式左部
		if (Pro[i].left == X)
		{
			// 右部第一个符号若为终结符，直接加入first集
			if (!IsNonterminal(Pro[i].right[0]))
			{
				firstSet[Xindex].insert(Pro[i].right[0]);
			}
			else
			{
				// 产生式为X->Y1..Yj..Yk形式
				for (int j = 0; j < Pro[i].right.length(); j++)
				{
					char Yj = Pro[i].right[j];//Yj为产生式右部的符号（可能为终结符也可能为非终结符）

					// 若Yj是终结符(不能产生$),则加入First(X)集后，立刻跳出循环
					if (!IsNonterminal(Yj))
					{
						firstSet[Xindex].insert(Yj);
						break;
					}

					// 若Yj是非终结符，递归求出First(Yj)
					GFirstSet(Yj);
					int YjIndex = GetindexN(Yj);//找到Yj在非终结符表中的下标

					//将Yj的First集中的非空的终结符加入First(X)中
					set<char>::iterator it;//迭代器变量
					for (it = firstSet[YjIndex].begin(); it != firstSet[YjIndex].end(); it++)
					{
						if (*it == 'e') // 遍历查看First(Yj)中是否含有'$'(能产生空)
							hasEmpty = 1;
						else
							firstSet[Xindex].insert(*it);//将FIRST(Yj)中的非$就加入FIRST(X)
					}

					// Yj不能产生空, 迭代结束
					if (hasEmpty == 0)
						break;
					//  Yj能产生空,继续进行迭代运算，并累加到countEmpty中
					else
					{
						countEmpty += hasEmpty;
						hasEmpty = 0;
					}
				}
				//所有右部first(Y)都有$,则将$加入First(X)中
				if (countEmpty == Pro[i].right.length())
					firstSet[GetindexN(X)].insert('e');
			}
		}
	}
}

// 求非终结符X的的Follow集
void GFollowSet(char X)
{
	int Xindex = GetindexN(X);//非终结符在非终结符表中的位置
	for (int i = 0; i < T; i++)
	{
		int index = -1;//index为非终结符X在产生式Pro[i]中右部的位置，初始化为-1

		int len = Pro[i].right.length();//产生式Pro[i]中右部的长度

		// 寻找X在产生式中的位置index
		for (int j = 0; j < len; j++)
		{
			if (Pro[i].right[j] == X)
			{
				index = j;
				break;
			}
		}
		// 找到X在产生式中的位置index
		// 存在A->aBb, 将FIRST(b)中除了空$之外的所有放入FOLLOW(B)中
		// 这里B对应X, b对应next0
		if (index != -1 && index < len - 1)
		{
			char next0 = Pro[i].right[index + 1];//X右部的第一个符号

			// 若b是终结符 则FIRST(b)=b，直接插入B的Follow集中
			if (!IsNonterminal(next0))
			{
				followSet[Xindex].insert(next0);
			}
			// 若b是非终结符
			else
			{
				int hasEmpty = 0;//First(b)集中是否含有$
				set<char>::iterator it;
				int next0Index = GetindexN(next0);

				// 插入FIRST(b)中除了$之外的所有的所有符号
				for (it = firstSet[next0Index].begin(); it != firstSet[next0Index].end(); it++)
				{
					if (*it == 'e')
						hasEmpty = 1;
					else
						followSet[Xindex].insert(*it);
				}

				// 存在A->aBb且FIRST(b)->$
				// FOLLOW(A)放在FOLLOW(B)中
				if (hasEmpty && Pro[i].left != X)
				{
					GFollowSet(Pro[i].left);//递归求得A的Follow集

					//把A的Follow集中的元素全部加到B的Follow集中
					char A = Pro[i].left;
					int AIndex = GetindexN(A);

					set<char>::iterator it;
					for (it = followSet[AIndex].begin(); it != followSet[AIndex].end(); it++)
						followSet[Xindex].insert(*it);
				}
			}
		}
		// 存在A->aB ,FOLLOW(A)放在FOLLOW(B)中,方法与上部类似
		else if (index != -1 && index == len - 1 && X != Pro[i].left)
		{
			GFollowSet(Pro[i].left);//递归求得A的Follow集

			//把A的Follow集中的元素全部加到B的Follow集中
			char A = Pro[i].left;
			int AIndex = GetindexN(A);

			set<char>::iterator it;
			for (it = followSet[AIndex].begin(); it != followSet[AIndex].end(); it++)
				followSet[Xindex].insert(*it);
		}
	}
}

// 初始化输入，并求出各个非终结符的First和Follow集
void FFInitialization()
{
	string s = "E->TA";//作为临时字符串，存储产生式
	int Ipro = 0;//记录产生式的数目
	//string filename = "resource.txt";//设置文件名

	//ifstream in_file;
	//in_file.open(filename);//打开文件
	//getline(in_file, s);  //从文件中获取以'\n'为结尾的字符串，也就是读出每一个产生式，赋予s
	int line = 5;
	// 分别处理每一个产生式
	while (line--)
	{
		//cout << s<<endl;
		string temp = ""; // 存储去掉空格的产生式
		// 去掉产生式中的' '
		for (int i = 0; i < s.length(); i++)
		{
			if (s[i] != ' ')
				temp += s[i];
		}

		Pro[Ipro].left = temp[0]; // 产生式的左部

		// 产生式的右部
		//处理‘|’的问题
		//去掉X->,从第四个字符开始
		for (int i = 3; i < temp.length(); i++)
		{
			//如果遇到‘|’，则开始生成一个新产生式，此产生式的左部与旧产生式相同
			if (temp[i] == '|')
			{
				Ipro++;
				Pro[Ipro].left = temp[0];
			}
			else
			{
				Pro[Ipro].right += temp[i];
			}
		}
		Ipro++;//记得递增一个数字

		// 存储所有终结符和非终结符
		for (int i = 0; i < temp.length(); i++)
		{
			// 跳过产生符号->和|
			if (i == 1 || i == 2 || temp[i] == '|')
				continue;
			//插入一个非终结符
			if (IsNonterminal(temp[i]))
			{
				int flag = 0;
				//检查非终结符栈中有无存储此非终结符，若没有，则将其入栈
				for (int j = 0; j < nonterminal.size(); j++)
				{
					if (nonterminal[j] == temp[i])
					{
						flag = 1;
						break;
					}
				}
				if (!flag)
					nonterminal.push_back(temp[i]);
			}
			//插入一个终结符，同上
			else
			{
				int flag = 0;
				for (int j = 0; j < terminal.size(); j++)
				{
					if (terminal[j] == temp[i])
					{
						flag = 1;
						break;
					}
				}
				if (!flag)
					terminal.push_back(temp[i]);
			}
		}
		//读出下一行
		if (line == 4) s = "A->+TA|e";
		else if (line == 3) s = "T->FB";
		else if (line == 2) s = "B->*FB|e";
		else if (line == 1) s = "F->(E)|a";
		else break;
		//getline(in_file, s);
	}
	//in_file.close();             //输出完毕 关闭文件

	terminal.push_back('#'); //将终止符入栈

	T = Ipro;//将产生式的总数目（包括新生成的）赋值给T

	// 存储没有$符号的终结符
	for (int i = 0; i < terminal.size(); i++)
	{
		if (terminal[i] != 'e')
			terminalN.push_back(terminal[i]);
	}
	// 获得first集
	for (int i = 0; i < nonterminal.size(); i++)
	{
		GFirstSet(nonterminal[i]);
	}
	// 获得follow集
	for (int i = 0; i < nonterminal.size(); i++)
	{
		if (i == 0) // 开始符号, 先加入结束符号
			followSet[0].insert('#');
		GFollowSet(nonterminal[i]);
	}
}
//得到分析预测表并输出
void GforecastAnaly()
{
	//cout << "表达式文法预测分析表：" << endl;

	// 对于每个产生式(编号index):A->α
	for (int index = 0; index < T; index++)
	{

		int row = GetindexN(Pro[index].left);//根据产生式左侧非终结符确定分析表行号
		int emptyCount = 0;

		// 对First(α)中的每个终结符号a,将index加入(A, a)中
		for (int i = 0; i < Pro[index].right.size(); i++)
		{
			char tmp = Pro[index].right[i];//临时变量，储存产生式index的第i符号
			// 若tmp是终结符
			if (!IsNonterminal(tmp))
			{
				//若tmp不为空，则将其产生式序号放入分析表的此行对应的非终结符的列中，即将index加入(A,tmp)中
				if (tmp != 'e')
					forecastAnaly[row][Getindex(tmp)] = index;
				//若tmp为空，则将其记录下来
				if (tmp == 'e')
				{
					emptyCount++;
				}
				break;
			}
			// 若tmp是非终结符
			else
			{
				int tmpIndex = GetindexN(tmp); //临时变量，储存此非终结符的标号

				// 对First(tmp)中的每个终结符号a,将index加入(A, a)中
				set<char>::iterator it;
				for (it = firstSet[tmpIndex].begin(); it != firstSet[tmpIndex].end(); it++)
				{
					forecastAnaly[row][Getindex(*it)] = index;
				}
				// 如果空在First(tmp)中,则将其记录下来，并继续看α中的下一个符号
				if (firstSet[tmpIndex].count('e') != 0)
				{
					emptyCount++;
				}
				else
				{
					break;
				}
			}
		}

		//如果空在First(α)中,对Follow(A)中的每个终结符或结束符b,将index加入(A,b)中
		if (emptyCount == Pro[index].right.size())
		{
			set<char>::iterator it;
			for (it = followSet[row].begin(); it != followSet[row].end(); it++)
			{
				forecastAnaly[row][Getindex(*it)] = index;
			}
		}
	}
	//输出分析表

	// 表头，终结符序列
	/*for (int i = 0; i < terminalN.size(); i++)
	{
		//setw(x);//，使输出整齐
		cout << setw(15) << terminalN[i];
	}
	cout << endl;*/

	//表项，非终结符加上预测动作项
	/*for (int i = 0; i < nonterminal.size(); i++)
	{
		cout << nonterminal[i];
		for (int j = 0; j < terminalN.size(); j++)
		{
			if (forecastAnaly[i][j] == -1)
				cout << setw(15)<<" ";
			else
				cout << setw(15)<<"->" << Pro[forecastAnaly[i][j]].right;
		}
		cout << endl;
	}
	cout << endl;*/
}
//对输入串进行分析
void AnalyticalProcess()
{
	int m = 1;//记录步骤数目

	string s;
	//cout << "请输入待分析符号串：" << endl;
	cin >> s;
	if(s=="E")
	{
		flag=0;
		return ;
	}
	//cout << "分析全过程：" << endl;
	//输出表头
	//cout << "步骤"<<setw(30) << "分析栈" << setw(30) << "剩余输入串" << setw(30) << "推导所用产生式或匹配" << endl;
	//剩余输入串栈，先添加符号串，再添加结束符，为了输出美观
	for (int i = 0; i <s.size() ; i++)
		RemainE.push_back(s[i]);
	RemainE.push_back('#');
	//分析栈，先添加结束符，再添加开始符号
	analyticalS.push_back('#');
	analyticalS.push_back(nonterminal[0]); // 加入开始符号
	while (analyticalS.size() > 0)
	{
		//cout << m;
		m++;
		//逐步输出分析栈的当前状况
		string outs = "";
		for (int i = 0; i < analyticalS.size(); i++)
			outs += analyticalS[i];
		//cout << setw(30) << outs;
		//逐步输出剩余符号栈的当前状况
		outs = "";
		for (int i = 0; i < RemainE.size(); i++)
			outs += RemainE[i];
		//cout << setw(30) << outs;
		// 逐步输出推导过程，并对分析栈和剩余符号栈进行处理
		char char1 = analyticalS.back();
		char char2 = RemainE.front();
		if (char1 == char2 && char1 == '#')
		{
			//cout << setw(30) << "接受" << endl;
			return;
		}
		//若分析栈和剩余符号栈开始的终结符相等，则分别出栈
		if (char1 == char2)
		{
			analyticalS.pop_back();
			RemainE.erase(RemainE.begin());
			//cout << "Syntax analysis is right" << endl;
			//cout << setw(30) <<"\""<<char1<<"\"匹配"<<endl;
		}
		// 预测表中有推倒项，可进行推导
		else if (forecastAnaly[GetindexN(char1)][Getindex(char2)] != -1)
		{
			int tg = forecastAnaly[GetindexN(char1)][Getindex(char2)];
			analyticalS.pop_back();
			//若产生式不为空，则将其进行推导，并反向入栈
			if (Pro[tg].right != "e")
			{
				for (int i = Pro[tg].right.length() - 1; i >= 0; i--)
					analyticalS.push_back(Pro[tg].right[i]);
			}
			//cout << setw(30) << Pro[tg].left << "->" << Pro[tg].right << endl;
		}
		// 若分析表对应项为空，则出现错误
		else
		{
			flag = 0;
			//cout << "Error on syntax analysis" << endl;
			return;
		}
	}
}
int main()
{
	memset(forecastAnaly, -1, sizeof(forecastAnaly));//初始化分析预测表，并初始化每个值为-1
	FFInitialization();
	//DFirstAFollow();
	GforecastAnaly();
	while(1)
	{
		AnalyticalProcess();
		if(flag==0) cout << "Error on syntax analysis" << endl;
		else cout << "Syntax analysis is right" << endl;
		flag=1;
		RemainE.clear();
		analyticalS.clear();
	}
	system("pause");
	return 0;
}
