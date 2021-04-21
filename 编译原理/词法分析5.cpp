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
			for(int i=0; i<maxn; i++)
				a[l][i]==-1 ? a[l][i]=r : 0; //双目运算符，如果是-1，就赋值为r，否则什么都不做
		else if(s=="space")//cin默认是无法读空格的，只能用space代替空格
			a[l][' ']=r;
		else a[l][s[0]]=r;//本来应该就是一个状态识别一个字符的，为了让输入文件短一点，才用了字符串
	}
	a[0]['\n']=0;//老师的那个DFA没有考虑这个，导致输入只能是一行 
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
