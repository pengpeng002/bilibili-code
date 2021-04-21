#include <bits/stdc++.h>
#include "词法分析6.cpp"
using namespace std;
vector<string> inputString;//输入的语法规则
set<string> vn,vt,se;//终结符与非终结符，vn非终结符，vt终结符，试下允许终结符和非终结符为字符串
//se是临时存储任意字符串的first集合
map<string, vector<vector<string> > >yufa;//替换规则，每个非终结符可以有多个候选产生式
string start="";//文法的开始符号
vector<vector<string> >input;//记录产生式，并给予一个编号，即在数组中的位置 ，可通过编号求得产生式
map<vector<string>, int> rInput; //通过产生式得到编号，与input相反
typedef pair<vector<string>, int> Obj;//产生式及 ·的位置 ，定义为一个项目
typedef pair<Obj, string> OObj;//LR(1)项目 
map<pair<int, string>, int>GO;//pair<项目编号，终结符/非终结符>，项目编号  I状态识别一个X之后到达J状态
vector<set<OObj> >DFA;//项目集规范族
stack<pair<int, string> > sta;//分析栈
map<int, map<string, pair<string, int> > > action;//action表，action[项目集编号][终结符]=pair<移进/规约， 项目集/产生式编号>
map<int, map<string, int> >GOTO; //GOTO表，goto[项目集编号][非终结符]=项目集编号
map<string, set<string> >first, follow;
string Next()//调用词法分析获取下一个输入单词 
{
	node no=cifa();
	cout<<no.name<<" "<<no.value<<endl;
	return no.name;//因为有return，后面的不会执行
	char ch;
	if(scanf("%c",&ch)!=1) return "#";//返回值不等于1说明读完了
	string ans="";
	ans+=ch;
	if(ch=='\n') ans="#";
	return ans;
}
void getVnVt()//计算终结符与非终结符 
{
	string s;
	while(cin>>s) inputString.push_back(s);
	sort(reserve.begin(), reserve.end());//方便二分查找，因为reserve里面的都是终结符，可以直接在reserve里找
	for(auto i:inputString)
	{
		int pos=i.find("->");//pos是找到的位置
		vn.insert(i.substr(0,pos));//从0开始，到pos-1停止，不包括第pos个
		if(start=="") start=i.substr(0,pos);//文法开始符号即第一条产生式的第一个非终结符
	}
	for(auto i:inputString)
	{
		int pos=i.find("->");
		string s="";//s存储当前得到的字符串，寻找是否为终结符或非终结符，如果是就清空，找下一个
		vector<string> ve;//将终结符与非终结符分开，存储产生式的右部
		vector<string> ne;//记录一条产生式，包括其左部
		ne.push_back(i.substr(0,pos)); //将产生式的左部放在第一个位置
		for(int j=pos+2; j<i.length(); j++)
		{
			s+=i[j];
			if(vn.find(s) != vn.end())
			{
				ve.push_back(s);
				ne.push_back(s);
				s="";
				continue;
			}
			if(binary_search(reserve.begin(), reserve.end(), s))//二分，应该比find会稍微快一点。找到了返回1，没找到返回0
			{
				ve.push_back(s);
				ne.push_back(s);
				vt.insert(s);
				s="";
			}
			else if(s=="defined" || s=="number" || s=="$")
				//如果在reserve中没找到，有可能是defined(自定义标识符),number(数字),$(空字)
			{
				ve.push_back(s);
				ne.push_back(s);
				vt.insert(s);
				s="";
			}
			else if(s=="a" || s=="b")
			{
				ve.push_back(s);
				ne.push_back(s);
				vt.insert(s);
				s="";
			}
		}
		yufa[i.substr(0,pos)].push_back(ve);//yufa是string映射二维数组
		input.push_back(ne);
	}
	vt.insert("#");//将结束符#放入终结符集中
	for(int i=0; i<input.size(); i++) //遍历所有记录的产生式，并将其与编号对应起来存放入rInput中
	{
		rInput[input[i]]=i;
	}
	printf("\n非终结符：");
	for(auto i:vn) cout<<i<<" ";
	printf("\n终结符：");
	for(auto i:vt) cout<<i<<" ";
	cout<<endl;
	for(int i=0; i<input.size(); i++)
	{
		cout<<"number "<<i<<endl;
		cout<<input[i][0]<<"->";
		for(int j=1; j<input[i].size(); j++)
		{
			cout<<input[i][j]<<" ";
		}
		cout<<endl;
	}
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
				vector<string> s=j;
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
							if(k!="$")//不插入空元素
							{
								if(first[i].insert(k).second) change=1;//插入成功，first集合发生了变化
							}
							else flag=1;//有空元素，标记，进行下一次
						}
					}
					if(flag==0) break;//不含空元素，不进行下一次
				}
				if(flag==1 && first[i].insert("$").second) change=1;
				//每个非终结符的first集合都有空元素，可以将空元素放入。放入成功就标记发生了变化
			}
		}
		for(auto i:vt) first[i].insert(i);//终结符的first集合是它本身
	}
	for(auto i:first)
	{
		string ch=i.first;
		cout<<"char="<<ch<<": ";
		for(auto j:first[ch])
		{
			cout<<j<<" ";
		}
		cout<<endl;
	}
}
void getFirst(vector<string> s)//计算任意串的非终结符
{
	se.clear();//临时使用，每次清空
	bool flag=1;//标记空元素，初始为1可以不用考虑s长度为0的清空
	for(int i=0; i<s.size(); i++) //遍历
	{
		flag=0;//初始默认没有
		for(auto j:first[s[i]])//将每个字符的first集合加入到s的first集合中，除空字符外
		{
			if(j!="$") se.insert(j);
			else flag=1;
		}
		if(flag==0) break;//如果没有空字符，不进行下一次
	}
	if(flag==1) se.insert("$");//每个字符的first集合都包含空字符，显然s可以若干步推导出空字
}
void getFollow()//计算非终结符的follow集合
{
	bool change=1;
	follow[start].insert("#");//文法开始符号的follow集合中放入一个#号
	while(change)
	{
		change=0;
		for(auto i:vn)//遍历非终结符
		{
			for(auto j:yufa[i])//遍历每个产生式
			{
				for(int pos=0; pos<j.size(); pos++) //遍历产生式
				{
					if(vn.find(j[pos]) != vn.end())//非终结符
					{
						vector<string> s;
						s.assign(j.begin()+pos+1, j.end());//形如A->aBb，取得子串b
//						string s=j.substr(pos+1);
						getFirst(s);//计算b的first集合，保存在se中
						if(pos+1 != j.size())//形如A->aBb，将b的first集合中所有非空元素加入到B的follow集合中
						{
							for(auto k:se)//遍历se，将非空元素加入
							{
								if(k!="$")
								{
									if(follow[j[pos]].insert(k).second) change=1;//加入成功，标记有改变
								}
							}
						}
						if(se.find("$") != se.end())
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
		string ch=i.first;
		cout<<"char="<<ch<<": ";
		for(auto j:follow[ch])
		{
			cout<<j<<" ";
		}
		cout<<endl;
	}
}

set<OObj> closure(OObj x)//计算项目闭包 
{
	set<OObj> ans;//需要返回的闭包
	ans.insert(x);
	bool f=1;//记里集合是否有变化
	while(f)//直到集合不再增大为止
	{
		f=0;//如果集合有增大，f=1
		for(auto x1:ans)
		{
			auto x=x1.first;//为了减少代码改动 
			if(x.second == x.first.size())//如果点已经到了最后一个为止，显然不可能用它继续产生闭包
			{
				f=0;
				break;
			}
			string s=x.first[x.second];//得到点后面的符号
			if(vn.find(s) != vn.end())//如果点后面式非终结符，形如A->X ·BY,则所有形如 B->X的产生式都应该属于这个闭包
			{
				for(auto i:yufa[s])
				{
					vector<string> temp=i;
					temp.insert(temp.begin(), s);
					//因为yufa所保存的产生式是不包括左部符号的，所以需要把左部符号插入到最开头
					vector<string> xx;
					//若项目[A->X·BY,a]属于闭包ans，B->R是一个产生式，对first(Ra)中的每个终结符b，若[B->R,b]不在ans中，加入 
					xx.assign(x.first.begin()+x.second+1, x.first.end());
					xx.push_back(x1.second);
					getFirst(xx);
					for(auto b:se)//getfirst计算得到的结果临时存放在se中 
					if(ans.insert(make_pair(make_pair(temp,1),b)).second)
					{
						f=1;//如果插入成功，f=1，集合增大了
					}
				}
			}
		}
	}
	return ans;
}
void go(int pos, string xx)//计算项目转换函数GO 
{
	set<OObj> ans;
	for(auto x1:DFA[pos])
	{
		auto x=x1.first;
		if(x.first.size() == x.second) continue;//如果已经到了最后一个位置了，说明是一个规约项目，无需往后移
		if(x.first[x.second] == xx)//只有在当前状态看到的下一个符号是xx，才能后移点进行计算闭包
		{
			set<OObj> temp=closure(make_pair(make_pair(x.first, x.second+1),x1.second));//计算点向后移一位的闭包
			ans.insert(temp.begin(), temp.end());//set可以自动去重
		}
	}
	if(ans.empty()) return ;//如果得到的是空的，那么直接返回
	int loc=find(DFA.begin(), DFA.end(), ans) - DFA.begin();//find返回在数组中的位置，减去起始位置得到编号
	if(loc != DFA.size())
	{
		GO[make_pair(pos, xx)]=loc; 
	}
	else
	{
		DFA.push_back(ans);
		GO[make_pair(pos, xx)] = DFA.size()-1;
	}
}
void itemSets()//计算项目集规范族或者说形成DFA 
{
	set<OObj> statu=closure(make_pair(make_pair(input[0],1),"#"));//初始项目的闭包，作为一个项目集
	DFA.push_back(statu);//存入DFA中，得到一个编号为0，因为位置为0号位置
	for(int i=0; i<DFA.size(); i++)
		//遍历每一个项目，知道DFA不再增大为止，因为push_back会自动放在数组的最后一个为止，所以可以直接遍历
	{
		for(auto t:vt) go(i,t);//尝试每一个项目集和每一个文法符号，看是否有可能得到一个新的项目集，\
		如果可以，则将新的项目集加入到项目集规范族中
		for(auto n:vn) go(i,n);
	}
	for(int i=0; i<DFA.size(); i++)
	{
		cout<<i<<endl;
		for(auto j1:DFA[i])
		{
			auto j=j1.first;
			vector<string> temp=j.first;
			cout<<temp[0]<<"->";
			for(int k=1; k<temp.size(); k++)
			{
				if(k==j.second) cout<<" ·";
				cout<<temp[k]<<" ";
			}
			if(j.second == temp.size()) cout<<" ·";
			cout<<", "<<j1.second<<"\n";
		}
		cout<<endl;
	}
	for(auto i:GO)
	{
		cout<<i.first.first<<" "<<i.first.second<<" "<<i.second<<endl;
	}
}
void getActionAndGoto()//计算action表和goto表 
{
	for(int i=0; i<DFA.size(); i++) //将action表和goto表初始化
	{
		for(auto j:vt) action[i][j]=make_pair("error",0);
		for(auto j:vn) GOTO[i][j]=-1;
	}
	for(auto i:GO)//map<pair<int, string>, int>
	{
		if(vt.find(i.first.second) != vt.end())//形如 A->Xa·Y,属于项目集k，GO[k][a]=j,则action[k][a]=<s,j>
		{
			if(action[i.first.first][i.first.second].first != "error")//如果之前的值不为error，说明有重复项
			{
				cout<<"have an error\n";
			}
			action[i.first.first][i.first.second]=make_pair("s",i.second);
		}
		else//GO[k][A]=j,GOTO[k][A]=j
		{
			if(GOTO[i.first.first][i.first.second] != -1)
			{
				cout<<"have an error\n";
			}
			GOTO[i.first.first][i.first.second]=i.second;
		}
	}
	for(int i=0; i<DFA.size(); i++)
	{
		set<OObj> x=DFA[i];
		for(auto j1:x)
		{
			auto j=j1.first;
			if(j.first.size() == j.second)//如果点在最后一位，说明是一个规约项目
			{
				if(j.first[0] == start)//如果是文法开始符号，说明分析完成
				{
					if(action[i]["#"].first != "error")
					{
						cout<<"have an error\n";
					}
					action[i]["#"]=make_pair("acc",0);
					continue;
				}
				string a=j1.second;//LR(1)分析，只有当接下来的词是 
//				for(auto a:follow[j.first[0]])//SLR(1)时，需要往后看一个字符，将follow集合的置r 
//				for(auto a:vt)//LR(0)分析在遇到规约项目时，不需要看下一个字符，直接进行规约，所以在所有的终结符都填上r
//				{
					if(action[i][a].first != "error")
					{
						cout<<"have an error\n";
					}
					action[i][a]=make_pair("r",rInput[j.first]);//规约项目，用的产生式编号通过rinput找
//				}
			}
		}
	}
	cout<<"	";
	for(auto i:vt) cout<<i<<"	";
	cout<<endl;
	for(int i=0; i<DFA.size(); i++)
	{
		cout<<i<<"	";
		for(auto j:vt)
		{
			cout<<action[i][j].first<<action[i][j].second<<"	";
		}
		cout<<endl;
	}
	cout<<"	";
	for(auto i:vn) cout<<i<<"	";
	cout<<endl;
	for(int i=0; i<DFA.size(); i++)
	{
		cout<<i<<"	";
		for(auto j:vn)
		{
			cout<<GOTO[i][j]<<"	";
		}
		cout<<endl;
	}
}
string guiyue(int pos)//进行规约 
{
	string s=input[pos][0];//s就是左部符号
	cout<<"采用产生式"<<pos<<"进行规约\n";
	cout<<s<<"->";
	for(int i=1; i<input[pos].size(); i++)cout<<input[pos][i]<<" ";
	cout<<endl;
	for(int i=0; i<input[pos].size()-1; i++) //-1是因为第一个是获取的式子多了一个左部符号，所以要-1，避免多移除
	{
		sta.pop();
	}
	return s;
}
void analyze()//语法分析 
{
	sta.push(make_pair(0,"#"));//分析栈先放一个状态0，符号为#
	string a=Next();
	while(1)
	{
		pair<int, string> x=sta.top();//获取栈顶元素
		pair<string, int> op=action[x.first][a];//始终拿当前栈顶的状态与面临的终结符去查action表，获取接下来的操作
		if(op.first=="s")//如果是s，表示是移进项目
		{
			sta.push(make_pair(op.second, a));//action的第二个元素就是下一个状态
			cout<<"当前符号为"<<a<<"，移进操作\n";
			a=Next();//获取下一个输入
		}
		else if(op.first=="r")//如果是r，说明是规约项目
		{
			cout<<"当前符号是"<<a<<endl;
			string N=guiyue(op.second);//获取规约产生式的左部，加入到栈顶
			int st=GOTO[sta.top().first][N];//获取下一个状态
			sta.push(make_pair(st,N));//规约操作没有用到当前面临的终结符，所以不用获取Next
		}
		else if(op.first=="acc") break;//如果是acc，接受状态，说明分析完了
		else
		{
			if(action[x.first]["$"].first != "error")//如果能用空字扩展，选择用空字扩展
			{
				cout<<"移进一个空字\n";
				sta.push(make_pair(action[x.first]["$"].second,"$"));
//				a=Next();
			}
			else cout<<"error!\n";//其他状态，都是error
		}
	}
}
int main()
{
	freopen("DFA.txt","r",stdin);
	init();
	cin.clear();//cin的问题，重载输入流之后，如果不clear就会导致后面的cin输入出现问题 
	freopen("prodown.txt","r",stdin);
	getVnVt();
	getFirst();
	getFollow();
	itemSets();
	getActionAndGoto();
	freopen("temp.txt","r",stdin);
	analyze();
}
