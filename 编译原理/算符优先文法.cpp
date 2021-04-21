#include <bits/stdc++.h>
#include "词法分析4.cpp"
using namespace std;
//语法树的两层或两层以上子树称为短语，直接短语是两层子树，素短语是至少包含一个非终结符，且不包含其他更小素短语的子树
//显然素短语都是可规约串，最左侧的素短语就是当前要规约的串
//在规约的过程中，如果某一个子串最左侧终结符比它前面的都要高，最右侧的终结符比后面的也都要高，可以证明这个子串是最左素短语
//算符优先分析法在规约时，会跳过单个非终结符，所以得到的结果不一定时语法树，也会导致可能会识别到非法句子
//但合法的句子一定可以识别出 
vector<string> inputString;//输入的语法规则
set<string> vn,vt;//终结符与非终结符，vn非终结符，vt终结符，试下允许终结符和非终结符为字符串 
map<string, vector<vector<string> > >yufa;//替换规则，每个非终结符可以有多个候选产生式
map<string, map<string, int> >F, L, table;//FIRSTVT = {a | F[P][a]=TRUE} ，LASTVT同理。table为优先关系表 
stack<pair<string, string> >sta;
string start="";//文法的开始符号 
string S[123];
string Next()
{
	string ans="";
	char ch;
	scanf("%c",&ch);
	ans+=ch;
	if(ch=='\n') ans="#";
	return ans;
	node no=cifa();//获取词法分析程序的输出作为语法分析程序的输入 
//	cout<<no.name<<" "<<no.value<<endl;
	return no.name;//暂时先这样吧，只取第一个字符做非终结符，以后应该会改成非终结符为单词的 
}
void getVnVt()
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
		for(int j=pos+2; j<i.length(); j++)
		{
			s+=i[j];
			if(vn.find(s) != vn.end())
			{
				ve.push_back(s);
				s="";
				continue;
			}
			if(binary_search(reserve.begin(), reserve.end(), s))//二分，应该比find会稍微快一点。找到了返回1，没找到返回0 
			{
				ve.push_back(s);
				vt.insert(s); 
				s="";
			}
			else if(s=="defined" || s=="number" || s=="$")
			//如果在reserve中没找到，有可能是defined(自定义标识符),number(数字),$(空字) 
			{
				ve.push_back(s);
				vt.insert(s); 
				s="";
			}
			else
			{
				ve.push_back(s);
				vt.insert(s); 
				s="";
			}
		}
		yufa[i.substr(0,pos)].push_back(ve);//yufa是string映射二维数组 
	}
//	for(auto i:yufa)
//	{
//		cout<<i.first<<"	";
//		for(auto j:i.second)
//		{
//			cout<<"	";
//			for(auto k:j) cout<<k<<" ";
//			cout<<endl;
//		}
//		cout<<endl;
//	}
	printf("\n非终结符：");
	for(auto i:vn) cout<<i<<" ";
	printf("\n终结符：");
	for(auto i:vt) cout<<i<<" ";cout<<endl;
}
void push(map<string, map<string, int> >&F, string P, string a)//FIRSTVT = {a | F[P][a]=TRUE} F[P][a]=1, 符号对(P,a)压入栈 
{
	if(F[P][a]==0)
	{
		F[P][a]=1;
		sta.push(make_pair(P,a));
	}
}
void getFirstVt()
{
	for(auto i:vn)//对每个非终结符P和终结符a，初始化F[P][a]为假 
	{
		for(auto j:vt) F[i][j]=0;
	}
	for(auto i:yufa)//遍历所有产生式，对形如P->a...或P->Qa...的产生式，F[P][a]=1, 符号对(P,a)压入栈 
	{
		vector<vector<string> >ve=i.second;
		for(auto j:ve)
		{
			for(auto k:j)
			{
				if(vt.find(k)!= vt.end())//形如P->a...或P->Qa...。a属于firstvt(p)； 
				{
					push(F,i.first,k);
					break;
				}
			}
		}
	}
	while(!sta.empty())
	{
		pair<string,string> P=sta.top();//弹出栈顶，记为(Q,a)； 
		sta.pop();
		for(auto i:yufa)//遍历每个产生式 
		{
			for(auto j:i.second)//j为某一条产生式 
			{
				if(j[0]==P.first)//形如P->Q...的产生式，若F[P][a]为假，置真，并压入栈 
				{
					push(F,i.first,P.second);
				}
			}
		 } 
	}
	cout<<"	";
	for(auto i:vt) cout<<i<<"	";cout<<endl;
	for(auto i:F)
	{
		cout<<i.first<<"	";
		for(auto j:i.second)
		{
			cout<<j.second<<"	";
		}
		cout<<endl;
	}
}
void getLastVt()
{
	for(auto i:vn)//对每个非终结符P和终结符a，初始化L[P][a]为假 
	{
		for(auto j:vt) L[i][j]=0;
	}
	for(auto i:yufa)//遍历所有产生式，对形如P->a...或P->Qa...的产生式，L[P][a]=1, 符号对(P,a)压入栈 
	{
		vector<vector<string> >ve=i.second;
		for(auto j:ve)
		{
			for(int k=j.size()-1; k>=0;k--)
			{
				if(vt.find(j[k])!= vt.end())//形如P->...a或P->...aQ。a属于lastvt(p)； 
				{
					push(L,i.first,j[k]);
					break;
				}
			}
		}
	}
	while(!sta.empty())
	{
		pair<string,string> P=sta.top();//弹出栈顶，记为(Q,a)； 
		sta.pop();
		for(auto i:yufa)//遍历每个产生式 
		{
			for(auto j:i.second)//j为某一条产生式 
			{
				if(j[j.size()-1]==P.first)//形如P->...Q的产生式，若L[P][a]为假，置真，并压入栈 
				{
					push(L,i.first,P.second);
				}
			}
		 } 
	}
	cout<<"	";
	for(auto i:vt) cout<<i<<"	";cout<<endl;
	for(auto i:L)
	{
		cout<<i.first<<"	";
		for(auto j:i.second)
		{
			cout<<j.second<<"	";
		}
		cout<<endl;
	}
}
void getTable()
{
	for(auto i:vt)//table存储运算符之间的优先关系，大于，等于，小于，若不能比较，置0，表示出错 
	{
		for(auto j:vt)
		{
			table[i][j]='0';
			table["#"][j]='0';//特别判断# 
		}
		table[i]["#"]='0';
	}
	table["#"]["#"]='=';
	cout<<"	";cout<<"#	";
	for(auto i:vt) cout<<i<<"	";cout<<endl;
	for(auto i:table)
	{
		cout<<i.first<<"	";
		for(auto j:i.second)
		{
			cout<<(char)j.second<<"	";
		}
		cout<<endl;
	}
	for(auto i:yufa)
	{
		vector<vector<string> >ve=i.second;
		for(int j=0;j<ve.size();j++)
		{
			for(int k=0;(k+2)<=ve[j].size();k++)//size的返回类型是unsigned long long，减法可能会溢出 
			{
				string xi=yufa[i.first][j][k], xi1=yufa[i.first][j][k+1];
				if(vt.find(xi) != vt.end() && vt.find(xi1) != vt.end())//形如...ab... ,a和b优先级相同 
				{
					table[xi][xi1]='=';
				}
				if((k+3)<=ve[j].size())
				{
					string xi2=yufa[i.first][j][k+2];
					if(vt.find(xi) != vt.end() && vt.find(xi1) == vt.end() && vt.find(xi2) != vt.end())
					//形如...aQb...，a和b的优先级相同 
					{
						table[xi][xi2]='='; 
					}
				}
				if(vt.find(xi) != vt.end() && vt.find(xi1) == vt.end())
				//形如...aQ...，对任何b属于firstvt(Q)，均有a < b 
				{
					for(auto a:F[xi1])
					{
						if(a.second==1) table[xi][a.first]='<';
					}
				}
				if(vt.find(xi) == vt.end() && vt.find(xi1) != vt.end())
				//形如...Qb...，对任何a属于lastvt(Q)，均有a > b 
				{
					for(auto a:L[xi])
					{
						if(a.second==1) table[a.first][xi1]='>';
					}
				}
			}
		}
	}
	//考虑#E#的情况，左边的#低于firstvt(E)，右边的#高于lastvt(E) 
	for(auto i:F[start]) if(i.second==1) table["#"][i.first]='<';
	for(auto i:L[start]) if(i.second==1) table[i.first]["#"]='>';
	cout<<"	";cout<<"#	";
	for(auto i:vt) cout<<i<<"	";cout<<endl;
	for(auto i:table)
	{
		cout<<i.first<<"	";
		for(auto j:i.second)
		{
			cout<<(char)j.second<<"	";
		}
		cout<<endl;
	}
}
void analyze()
{
	int k=0;//K表示栈的深度 
	S[k]="#";
	string a;
	do
	{
		a=Next();
		int j;//j始终指向栈内最上面的那个终结符
		if(vt.find(S[k]) != vt.end() || S[k]=="#") j=k;//如果栈顶是终结符，j指向栈顶 
		else j=k-1;//否则指向栈顶的下一个，因为算符优先文法不会有两个连续出现的非终结符，故此时的次栈顶一定是终结符
		cout<<a<<" "<<S[j]<<(char)table[S[j]][a]<<endl; 
		while(table[S[j]][a]=='>')//如果栈顶的优先级比外面的要高，栈顶元素先规约。此时已经形成了最左素短语，可以进行规约 
		{
//			printf("this j=%d\n",j);
			string Q=S[j];
			while(table[S[j]][Q]!='<')//往前找到最左素短语的头，停止 
			{
//				cout<<Q<<" "<<S[j]<<" j="<<j<<" "<<(char)table[S[j]][Q]<<endl;
				Q=S[j];
				if(vt.find(S[j-1]) != vt.end() || S[j-1]=="#")//保证j一定指向终结符 
				{
					j=j-1;
				}
				else j=j-2;
			}
//			printf("j=%d\n",j);
			vector<string> ve;
			string N;
			bool f=0;
			for(int i=j+1;i<=k;i++) ve.push_back(S[i]);//把最左素短语放入ve中
			for(auto i:yufa)//这个循环用来找用哪个产生式进行规约 
			{
				for(auto j:i.second)
				{
					if(ve.size() == j.size())
					{
						f=0;
						for(int k=0;k<ve.size();k++)//算符优先分析法规约时只要终结符对应相等，非终结符对非终结符就可以了 
						{
							if(ve[k] != j[k])
							{
								if(vt.find(ve[k]) != vt.end() || ve[k]=="#")//如果终结符不对应相等，不能规约 
								{
									f=1;
									break;
								}
							}
						}
						if(f==0)//说明当前的产生式可以规约 
						{
							N=i.first;
							break;
						 } 
					}
					else f=1;
					if(!f) break;
				}
				if(!f) break;
			}
			k=j+1;
			S[k]=N;
			cout<<"规约：";
			for(auto i:ve) cout<<i;
			cout<<"->"<<N<<endl;
		}
		if(table[S[j]][a]=='<' || table[S[j]][a]=='=')//如果栈顶终结符比外面的低或相等，说明外面的先规约 
		{
			k++;
			S[k]=a;
		}
		else cout<<"error!\n"; //不是大于，等于，小于，说明出错了 
	}
	while(a!="#");
}
int main()
{	
	init();
	freopen("prodown.txt","r",stdin);
	getVnVt();
	getFirstVt();
	getLastVt();
//	exit(0);
	getTable();
	freopen("CON","r",stdin);
	analyze();
}
