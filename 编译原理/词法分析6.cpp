//DFA属于一种特殊的NFA
//为了让程序的输入能够接上DFA.txt，本程序不负责解决初态唯一性，交给人工了
//测试的NFA所对应的正规式是 (a|b)*(aa|bb)(a|b)*
#include<bits/stdc++.h>
using namespace std;
struct node
{
	string name;//单词种类
	string value;//单词自身值
};
bool duwanle=0;
FILE *f=fopen("temp.txt","r");//只读打开文件temp.txt
vector<string> reserve= {"short","int","bool","long","float","double","char","static","for","while","do","continue","break",\
                         "return","void","switch","case","auto","goto","const","if","else","+","++","+=","-","-=","--","*",\
                         "*=","/","/=","%","%=","<","<<","<=",">",">>",">=","&","|","^","&&","||","^=","!=","!","~","#","=",\
                         "(",")",",",";","?",":","[","]","{","}","==","<<=",">>=","&=","|="
                        };
char ch,prech='$';
string name;
const int maxn=130;//128个ASCII字符
vector<int> statu, acc;//所有状态编号，结束状态编号
map<int, int[maxn]> a;//二维数组，可以选择用a[xx][130]
map<pair<int, char>, int>GO1;//pair<项目编号，任意字符>，项目编号  I状态识别一个X之后到达J状态
vector<set<int> >DFA1;//项目集规范族
int Node=2147483647;//int最大值，再程序中是新建节点的编号 
set<int> closure(int x)//计算项目闭包
{
	set<int> ans;//需要返回的闭包运算结果
	ans.insert(x);//当前状态做闭包得到的结果勘定包括它本身
	bool f=1;//标记集合是否发生变化
	while(f)//如果有变化就继续算，知道不再发生变化为止
	{
		f=0;//如果有变化就记为1
		for(auto x:ans)//遍历集合里的每一个元素
		{
			if(a[x]['$'] != -1)//状态x出发经过任意多条空弧所达到的状态都属于closure(x)
			{
				if(ans.insert(a[x]['$']).second) f=1;//insert的返回值是pair，first是插入的元素的位置，second是是否插入成功
			}
		}
	}
	return ans;
}
void go(int pos, char xx)//计算项目转换函数GO ,计算后得到的GO1内容就是确定化后的DFA
{
	set<int> ans;
	for(auto x:DFA1[pos])//遍历闭包中的每一个元素x
	{
		if(a[x][xx] != -1)//如果x射出弧xx到达的状态不是error
		{
			set<int> temp=closure(a[x][xx]);//则计算闭包并加入到ans中
			ans.insert(temp.begin(), temp.end());//set可以自动去重
		}
	}
	if(ans.empty()) return ;//如果得到的是空的，那么直接返回
	int loc=find(DFA1.begin(), DFA1.end(), ans) - DFA1.begin();//find返回在数组中的位置，减去起始位置得到编号
	if(loc != DFA1.size())
	{
		GO1[make_pair(pos, xx)]=loc;
	}
	else
	{
		DFA1.push_back(ans);
		GO1[make_pair(pos, xx)] = DFA1.size()-1;
	}
}
void item()
{
	set<int> se=closure(0);//先计算初态的闭包
	for(auto i:se)cout<<i<<" ";cout<<endl;
	DFA1.push_back(se);//加入到项目集规范族中
	for(int i=0; i<DFA1.size(); i++) //对每一个closure(i)
	{
		for(int x=0; x<maxn-2; x++) //尝试计算Ia,Ib,Ic。。。
		{
			if(x!='$') go(i,x);//$表示空字，所以本程序识别的DFA是不能有$字符的，再这里，I$相当于对闭包再求一次闭包
		}
	}
//	for(int i=0;i<DFA1.size();i++)
//	{
//		cout<<i<<endl;
//		for(auto j:DFA1[i])
//		{
//			cout<<j<<" ";
//		}
//		cout<<"\n";
//	}
//	for(auto i:GO1)
//	{
//		cout<<i.first.first<<" "<<i.first.second<<" "<<i.second<<endl;
//	}
}
void init()
{
	int x;//状态编号
	set<int> se;//存储所有读入的状态，set会自动去重，可以避免输入元素重复的错误
	//本人比较懒，状态的输入在一行，没有数状态的个数了
	while(scanf("%d%c",&x,&ch)==2)//本循环用来读取所有接受状态
	{
		se.insert(x);
		if(ch=='\n') break;
	}
	acc.assign(se.begin(), se.end());//将se中的所有元素全部放到acc中，se不清空
	while(scanf("%d%c",&x,&ch)==2)//本循环用来读取所有接受状态
	{
		se.insert(x);
		if(ch=='\n') break;
	}
	statu.assign(se.begin(), se.end());//将se中的所有元素都放到statu中
	//此时statu中的内容是DFA的所有状态编号，包括结束编号
	for(auto i:statu)
		for(int j=0; j<maxn; j++)
			a[i][j]=-1; //将a数组全部赋值为-1，表示错误状态
	int l, r;
	string s;//从 l 状态到 r 状态是识别一个 s 得到的
	while(cin>>l>>s>>r)
	{
		if(r>=96 && r<=100) r=-1;//DFA是直接抄的我们编译原理实验的DFA，其中96到100号状态均为error状态
		if(s=="dig")//如果是数字，不想写10行，就用dig代替了1到10
			for(int i=0; i<10; i++)
				a[l]['0'+i]=r; //让 l 状态识别所有数字转到 r 状态
		else if(s=="cha")//如果是cha，我用cha代表26个字母
			for(int i=0; i<26; i++)
				a[l]['a'+i]=r, a[l]['A'+i]=r;
		else if(s=="other")//用来表示所有没有进入过的字符
			for(int i=0; i<maxn-2; i++)
			{
				if(i=='$')continue;
				a[l][i]==-1 ? a[l][i]=r : 0; //双目运算符，如果是-1，就赋值为r，否则什么都不做
			}

		else if(s=="space")//cin默认是无法读空格的，只能用space代替空格
			a[l][' ']=r;
		else//本程序暂时不考虑弧上是正规式的情况 
		{
			int len=s.length(), pre=l;
			if(len>1)//如果长度大于1，说明弧上不是单个字符 
			{
				for(int i=0;i<len-1;i++)//那就一个个拆，对拆出来的每个字符都新建一个节点 
				{
					a[pre][s[i]]=Node;//新建一个节点 
					pre=Node--; //节点的编号-1，为了避免和已有编号冲突，编号节点从大到小递减，这句代码先赋值，后Node--； 
				}
			}
			a[pre][s[len-1]]=r;//本来应该就是一个状态识别一个字符的，为了让输入文件短一点，才用了字符串
		}
	}
	a[0]['\n']=0;//老师的那个DFA没有考虑这个，导致输入只能是一行
	item();
	for(auto i:GO1)//GO得到的是确定化后的DFA，全部放入a中，后面的代码就可以不改了 
	{
		a[i.first.first][i.first.second]=i.second;
	}
	vector<int> temp;//temp临时计算得到的新的终态集合 
	for(int i=0;i<DFA1.size();i++)//对项目集里的每个元素进行遍历，若包含原来的终态，则该状态作为新的终态 
	{
		set<int> s=DFA1[i];
		for(auto x:s)
		{
			if(find(acc.begin(), acc.end(), x) - acc.begin() != acc.size())//包含原来的终态 
			{
				temp.push_back(i);
				break;//继续下一个项目集的查找，该项目集已经不需要找了 
			}
		}
	}
	acc=temp;
}
void g()//读一个字符
{
	if(!duwanle)//如果没有读到文件尾才往下读
		if(fscanf(f,"%c",&ch)!=1)
			duwanle=1;//返回值不等于1说明是文件尾（EOF或者-1）
//	if(!duwanle) cout<<"ch="<<ch<<endl;
}
bool words()//识别单词
{
	name="";
	if(prech=='$') g();//如果前面有一个未处理的字符，则不需要读取下一个字符
	else ch=prech, prech='$';
	int sta=0;
	while(a[sta][ch] != -1)//error状态退出
	{
		if(duwanle) return 1;//读完了就不用继续往下走了
		for(auto i:acc)//遍历所有终态编号
			if(a[sta][ch]==i)//终态，退出
				return 1;
		if(ch>=0&&ch<128) sta=a[sta][ch];//进入下一个状态
		else name+=ch;//非英文字符，然而还是无法成功处理中文字符，会乱码。。。
		if(sta) name+=ch;//非0状态，继续读下一个，并将当前的加入到name中，否则就不加入
		g();
	}
	return 0;
}
bool find(string name)//查找一个字符串是否在保留字段内，即判断是否为预定义标识符或已知运算符
{
	for(auto i:reserve)
	{
		if(name==i)
		{
			return 1;
		}
	}
	return 0;
}
node cifa()
{
	if(words())//读一个单词出来
	{
		if(duwanle) return {"#","-"};//读完了，返回一个#，用来与语法分析程序的预测分析程序中栈内初始的#对应
		if(find(name+ch)==0)//如果name+ch不是保留字段
		{
			if(find(name))//判断name是不是保留字段
			{
				prech=ch;
				return {name,"-"};
			}
			//如果也不是，就什么都不做，往后执行判断name的种别
		}
		else//无论是if还是else部分，都说明返回的是一个预定义标识符或运算符，界符
		{
			return {name+ch,"-"};
		}
		if(name[0]>='0'&&name[0]<='9')//说明是数字
		{
			prech=ch;
			return {"number",name};
		}
		else if((name[0]>='a'&&name[0]<='z')||(name[0]>='A'&&name[0]<='Z')||name[0]=='_')//用户自定义标识符
		{
			prech=ch;
			return {"defined",name};
		}
		//虽然应该还有不少情况，但暂时放弃了。词法分析程序可以做，但语法分析程序。。。我想不出完全能匹配的文法
		//以后有兴趣了再加吧
	}
	else
	{
		prech=ch;
		return {"error","-"};//错误状态
	}
}
int mainn()
{
	freopen("DFA.txt","r",stdin);//重载输入流
	init();
	while(!duwanle)
	{
		node no=cifa();
		cout<<no.name<<" "<<no.value<<endl;
	}
	return 0;
}
