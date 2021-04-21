#include<bits/stdc++.h>
using namespace std;
const int xa=70, xb=130;
const int error=999;//错误状态
int a[xa][xb];//状态转移矩阵
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
//vector<string> ve(reserve);
string s="~!%^&*(){}+-[];:,?.|>=";
char ch,prech='$';
string name;
void stringTonumber(string num)//字符串转数字，便于计算
{
	if(num[0]=='0')//以0开头，十六进制或八进制数
	{
		long long ans=0;//记录结果
		if(num[1]=='X' || num[1]=='x')//十六进制数
		{
			for(int i=2; i<num.length(); i++)
			{
				ans=ans*16+num[i]-'0';
			}
		}
		else//八进制数
		{
			for(int i=1; i<num.length(); i++)
			{
				ans=ans*8+num[i]-'0';
			}
		}
		cout<<ans<<endl;//输出结果
	}
	else cout<<atof(num.c_str())<<endl;//atof可以自动将数字串转数字，结果为浮点数
}
void init()//初始化状态转移矩阵。0开头小数除外，因为实在是不好搞
{
	//太乱了，慢慢整理
	memset(a,-1,sizeof(a));//end
	for(int i=0; i<xb; i++)
	{
		a[0][i]=error;//初始将0状态的所有后继状态均设为error
		a[2][i]=error;//.结尾，只能接数字，其他状态则为error
		a[7][i]=error;//0x开头，是十六进制数，除0-f以外其他字符均为错误
		a[4][i]=error;//E/e，后面只能是数字，+-号
		a[5][i]=error;//+/-结尾
		a[11][i]=11;// //注释，除换行外，其他均不改变状态
		a[12][i]=12;// “，除”外，其他均不改变状态
		a[22][i]=21;// /** ，除非遇到/，否则说明仍处于注释状态
		a[21][i]=21;// /*注释，在遇到*前，状态不变
		a[13][i]=13;//单引号，在遇到下一个单引号前，状态不变
		a[18][i]=-1;// <后遇到了字符，直接结束
		a[19][i]=-1;// <<后遇到了字符，直接结束
		a[26][i]=26;// #include<xxx，头文件格式
		a[15][i]=error;// #，如果遇到的不是字符，就是错误状态
	}
	for(int i=0; i<10; i++)
	{
		a[0][i+'0']=1;//读到一个数字
		a[1][i+'0']=1;//任意长数字串
		a[2][i+'0']=3;//任意长数字串后读到一个小数点
		a[3][i+'0']=3;//任意长数字串后读到一个小数点后可再读到任意长数字串
		a[4][i+'0']=6;//数字串E后可接数字串
		a[5][i+'0']=6;//数字串E正负号后可接数字串
		a[6][i+'0']=6;//数字串E数字串
		a[7][i+'0']=8;//0x开头，十六进制
		a[8][i+'0']=8;//十六进制0x后可接任意长数字串
		a[9][i+'0']=9;
		a[14][i+'0']=16;//0开头八进制
		a[16][i+'0']=16;//八进制，其实这两个状态应该可合二为一
	}
	for(int i=0; i<26; i++)
	{
		a[0]['a'+i]=9;//cha
		a[0]['A'+i]=9;//cha
		a[9]['a'+i]=9;//cha
		a[9]['A'+i]=9;//cha
		a[15]['A'+i]=17;//#cha
		a[15]['a'+i]=17;
		a[17]['A'+i]=17;//#cha
		a[17]['a'+i]=17;
//		a[18]['A'+i]=19;// <cha
//		a[18]['a'+i]=19;
		a[25]['A'+i]=26;//#include<
		a[25]['a'+i]=26;
	}
	for(int i=0; i<6; i++)//均为处理十六进制
	{
		a[7]['a'+i]=8;
		a[7]['A'+i]=8;
		a[8]['a'+i]=8;
		a[8]['A'+i]=8;
	}
	a[0]['\\']=23;// \ 
	a[18]['<']=20;// <<
	a[26]['>']=-1;//#include<xxx>
	a[18]['=']=-1;// <=
	a[20]['=']=-1;// <<=
	a[14]['8']=a[14]['9']=error;//八进制
	a[16]['8']=a[16]['9']=error;//八进制
	a[0]['0']=14;//0
	a[0][' ']=a[0]['\n']=a[0]['	']=0;//blank
	a[0]['_']=9;//_
	a[1]['.']=2;//dig.
	a[14]['x']=a[14]['X']=7;//0x 0X
	a[3]['e']=a[3]['E']=4;//dig.digE
	a[1]['e']=a[1]['E']=4;//digE
	a[4]['+']=a[4]['-']=5;//digE+
	a[0]['#']=15;//#
	a[0]['<']=18;//<
	a[0]['/']=10;// /
	a[10]['*']=21;// /*
	a[21]['*']=22;// /**
	a[22]['/']=-1;// /**/
	a[10]['/']=11;//注释
	a[11]['\n']=-1;// //
	a[0]['"']=12;// "
	a[12]['"']=-1;// ""
	a[0]['\'']=13;// '
	a[13]['\'']=-1;// ''
	a[24]['<']=25;
	a[24]['"']=12;
	int nn=27;
	for(auto i:s)//如果能构成运算符，长度不会超过2，懒得一个个写状态了，直接往后加状态
	{
		a[0][i]=nn++;
	}
}
void g()//读一个字符
{
	if(!duwanle)//如果没有读到文件尾才往下读 
		if(fscanf(f,"%c",&ch)!=1)
			duwanle=1;//返回值不等于1说明是文件尾（EOF或者-1）
//	if(duwanle) cout<<"ch="<<ch<<endl; 
}
bool re()//转义 本函数识别转义字符并将转义后的字符添加到name中
{
	g();//读取下一个字符
//	cout<<ch<<" ** "<<(int)ch<<endl;
	if(ch=='a') ch=7,name+=ch,g();// \a
	else if(ch=='b') ch=8,name+=ch,g();// \b
	else if(ch=='f') ch=12,name+=ch,g();// \f
	else if(ch=='n') ch=10,name+=ch,g();// \n
	else if(ch=='r') ch=13,name+=ch,g();// \r
	else if(ch=='t') ch=9,name+=ch,g();// \t
	else if(ch=='v') ch=11,name+=ch,g();// \v
	else if(ch=='\\') ch=92,name+=ch,g();// 对转义字符的转义
	else if(ch=='\'') ch=39,name+=ch,g();// \'
	else if(ch=='"') ch=34,name+=ch,g();// \"
	else if(ch=='?') ch=63,name+=ch,g();// \?
	else if(ch=='0') ch=0,name+=ch,g();// \0
	else if(ch=='x')// \x 十六进制
	{
		g();
		int x=0;//需要得到这个十六进制数并转换为对应的字符
		while((ch>='0'&&ch<='9')||(ch>='A'&&ch<='F')||(ch>='a'&&ch<='f'))
		{
			if(ch>='0'&&ch<='9') x=x*16+ch-'0';
			else if(ch>='A'&&ch<='F') x=x*16+ch-'A'+10;
			else x=x*16+ch-'a'+10;
			g();
		}
		name+=x;
	}
	else if(ch>='0'&&ch<='7')//八进制
	{
		int x=0;//得到对应的八进制数
		for(int i=0; i<3 && ch<='7'; i++)
		{
			x=x*8+ch-'0', g();
		}
		name+=x;
	}
	else if(ch=='\n');//接续
	else return 0;//error
	return 1;
}
bool words(int sta)//识别单词
{
	name="";
	if(prech=='$') g();//如果前面有一个未处理的字符，则不需要读取下一个字符
	else ch=prech, prech='$';
	while((!duwanle)&&(ch==' '||ch=='\n'||ch=='	')) g();//跳过换行，空格和\t 
//	cout<<"int:"<<(int)ch<<" ch:"<<ch<<endl;
	while(a[sta][ch] != error)//error状态退出
	{
		if(duwanle) return 1;//读完了就不用继续往下走了 
		if(sta==11 || sta==21)//注释部分，特殊处理 
		{
			if(ch>=0&&ch<128) sta=a[sta][ch];//注释部分直接忽略，不转义
			name+=ch;//可以不加，反正注释对程序运行是没有影响的
			g();
			continue;
		}
		if(a[sta][ch]==-1)//终态，退出
			return 1;
		if(ch=='\\')//单独识别转义字符，对转义字符的处理应该在词法分析阶段就处理完而不是等到后续 
		{
			if(re());//如果读到的转义字符是不合法的，直接退出 例如\9这种的
			else return 0;
		}
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
	if(words(0))//读一个单词出来
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
//	freopen("temp.txt","r",stdin);
	init();//先构造状态转移矩阵
	while(!duwanle)
	{
		node no=cifa();
		cout<<no.name<<" "<<no.value<<endl;
	}
	return 0;
}
