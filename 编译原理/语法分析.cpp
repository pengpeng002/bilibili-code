#include <bits/stdc++.h>
#include "词法分析4.cpp"
using namespace std;
vector<string>inputString;//输入的语法规则
set<char> vn,vt;//终结符与非终结符，vn非终结符，vt终结符
map<char, vector<string> >yufa;//替换规则，每个非终结符可以有多个候选产生式
map<char, set<char> >first, follow;//first集合与follow集合
set<char> se;//临时存储任意字符串的first集合
map<char, map<char, string> > table;//可看作二维数组，值是string
stack<char> sta;//符号栈 / 输入栈
char Next()
{
	node no=cifa();//获取词法分析程序的输出作为语法分析程序的输入 
//	cout<<no.name<<" "<<no.value<<endl;
	return no.name[0];//暂时先这样吧，只取第一个字符做非终结符，以后应该会改成非终结符为单词的 
}
void getVnVt()//获取终结符与非终结符，并将每个语法规则都保存到yufa中
{
	string s;
	while(cin>>s) inputString.push_back(s);//输入所有语法规则，因为要遍历两次，先存储
	for(auto i:inputString)
	{
		vn.insert(i[0]);//每条规则的第一个字符一定是非终结符，暂时假定终结符和非终结符都为单个字符
		yufa[i[0]].push_back(i.substr(3)); //非终结符与产生式对应
	}
	for(auto i:inputString)
	{
		for(int j=3; j<i.length(); j++) //遍历每一条产生式的每个字符
		{
			if(vn.find(i[j]) == vn.end()) vt.insert(i[j]);//如果在非终结符找不到那就一定是终结符
		}
	}
	printf("\n非终结符：");
	for(auto i:vn) cout<<i<<" ";
	printf("\n终结符：");
	for(auto i:vt) cout<<i<<" ";
}
void getFirst()//计算每个非终结符的first集合
{
	bool change=1;
	while(change)//如果first集合有发生变化就继续，知道first集合不再变化才停止
	{
		change=0;
		for(auto i:vn)//遍历非终结符
		{
			for(auto j:yufa[i])//遍历非终结符的每条产生式
			{
				string s=j;
				bool flag=0;//标记是否有空元素
				for(auto ch:s)//对产生式进行遍历
				{
					if(vn.find(ch) == vn.end())//如果是终结符，就直接加入到first集合中
					{
						if(first[i].insert(ch).second) change=1;
						//如果insert的返回值为真，说明插入成功，标记本次first集合有发生变化
						flag=0;//没有空元素
					}
					else
					{
						flag=0;
						for(auto k:first[ch])//对该非终结符的first集合进行遍历
						{
							if(k!='$')//不插入空元素
							{
								if(first[i].insert(k).second) change=1;//插入成功，first集合发生了变化
							}
							else flag=1;//有空元素，标记，进行下一次
						}
					}
					if(flag==0) break;//不含空元素，不进行下一次
				}
				if(flag==1 && first[i].insert('$').second) change=1;
				//每个非终结符的first集合都有空元素，可以将空元素放入。放入成功就标记发生了变化
			}
		}
		for(auto i:vt) first[i].insert(i);//终结符的first集合是它本身
	}
	for(auto i:first)
	{
		char ch=i.first;
		cout<<"char="<<ch<<": ";
		for(auto j:first[ch])
		{
			cout<<j<<" ";
		}
		cout<<endl;
	}
}
void getFirst(string s)//计算任意串的非终结符
{
	se.clear();//临时使用，每次清空
	bool flag=1;//标记空元素，初始为1可以不用考虑s长度为0的清空
	for(int i=0; i<s.length(); i++) //遍历
	{
		flag=0;//初始默认没有
		for(auto j:first[s[i]])//将每个字符的first集合加入到s的first集合中，除空字符外
		{
			if(j!='$') se.insert(j);
			else flag=1;
		}
		if(flag==0) break;//如果没有空字符，不进行下一次
	}
	if(flag==1) se.insert('$');//每个字符的first集合都包含空字符，显然s可以若干步推导出空字
}
void getFollow()//计算非终结符的follow集合
{
	bool change=1;
	follow[inputString[0][0]].insert('#');//文法开始符号的follow集合中放入一个#号
	while(change)
	{
		change=0;
		for(auto i:vn)//遍历非终结符
		{
			for(auto j:yufa[i])//遍历每个产生式
			{
				for(int pos=0; pos<j.length(); pos++) //遍历产生式
				{
					if(vn.find(j[pos]) != vn.end())//非终结符
					{
						string s=j.substr(pos+1);//形如A->aBb，取得子串b
						getFirst(s);//计算b的first集合，保存在se中
						if(pos+1 != j.length())//形如A->aBb，将b的first集合中所有非空元素加入到B的follow集合中
						{
							for(auto k:se)//遍历se，将非空元素加入
							{
								if(k!='$')
								{
									if(follow[j[pos]].insert(k).second) change=1;//加入成功，标记有改变
								}
							}
						}
						if(se.find('$') != se.end())
							//形如A->aB，将A的follow集合加入到B的follow集合中。若pos已经是最后一位，s的长度为0，se只包含空字
							//将形如A->aB和A->aBb但b星号推出空字两种情况都计算了
						{
							for(auto k:follow[i])
							{
								if(follow[j[pos]].insert(k).second) change=1;
							}
						}
					}
				}
			}
		}
	}
	for(auto i:follow)
	{
		char ch=i.first;
		cout<<"char="<<ch<<": ";W
		for(auto j:follow[ch])
		{
			cout<<j<<" ";
		}
		cout<<endl;
	}
	exit(0); 
}
void getTable()
{
	for(auto i:vn)
	{
		for(auto j:vt)
		{
			table[i][j]="error";//将所有 M[A,a] 初始化为error
		}
	}
	for(auto A:vn)//遍历所有非终结符集合
	{
		for(auto j:yufa[A])//对每个 A->c 执行以下两个步骤
		{
			getFirst(j);//获取first(c)
			for(auto a:se)//对每个终结符a属于first(c)
			{
				if(table[A][a] != "error") cout<<"this is a error!\n"; 
				table[A][a]=j;//把 A->c 加入到 M[A,a];
				if(se.find('$') != se.end())//若空字属于 first(c)
				{
					for(auto b:follow[A])//则对任何 b 属于 follow(A)
					{
						if(table[A][b] != "error") cout<<"this is a error!\n"; 
						table[A][b]=j;//把 A->c 加入到 M[A,b];
					}
					
				}
			}
		}
	}
	exit(0);
}
void analyze()
{
	sta.push('#');//先把句末符压入栈内
	sta.push(inputString[0][0]);//将文法开始符号压入栈
	char in;
	in=Next();//获取下一个单词 
	while(in!='#' || sta.top()!='#')//若X=a='#'，分析成功，停止分析
	{
		if(sta.top() == in)//如果能够匹配，将栈顶弹出，然后读下一个字符
		{
			cout<<"delete "<<in<<endl;
			sta.pop();//栈顶元素弹出
			in=Next();
			if(in=='\n') in='#';//句末符
			continue;
		}
		string s=table[sta.top()][in];//获取预测分析表存放的下一步动作
		if(s!="error")//如果不是错误，
		{
			cout<<sta.top()<<"->"<<s<<endl;//输出执行的动作
			sta.pop();//栈顶元素弹出
			for(int i=s.length()-1; i>=0; i--)//将产生式右部符号串反序压入栈内
			{
				if(s[i]!='$') sta.push(s[i]);
			}
		}
		else cout<<"error!\n";//如果是出错标志就报错
	}
	cout<<"分析成功!\n";
}
int main()
{
	init();//初始化词法分析程序的状态转移矩阵 
	freopen("prodown.txt","r",stdin);
	getVnVt();
	getFirst();
	getFollow();
	getTable();
//	freopen("CON","r",stdin);//重定向到标准输入流，从键盘获取输入
	analyze();
}
