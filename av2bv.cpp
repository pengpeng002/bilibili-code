#include <bits/stdc++.h>
using namespace std;
const string s="fZodR9XQDSUm21yCkr6zBqiveYah8bt4xsWpHnJE7jL5VG3guMTKNPAwcF";
const long long xo=177451812;//xor
const long long add=298840120778677120;//2 1xx411c7mD
map<char,int>ma;
void bv2av(string bv)//只是av转bv的逆过程而已 
{
	string st=bv;//将已经移位的bv号替换成移位前的结果 
	st[6]=bv[0];
	st[2]=bv[1];
	st[4]=bv[2];
	st[7]=bv[3];
	st[5]=bv[4];
	st[8]=bv[5];
	st[3]=bv[6];
	st[9]=bv[7];
	st[1]=bv[8];
	st[0]=bv[9];
	__int128 r=ma[st[0]];
	__int128 x=1;//58**i
	for(int i=1;i<10;i++)
	{
		x*=58;
		r+=ma[st[i]]*x;
	}
	long long av=(r-add)^xo;
	cout<<"av"<<av<<endl;
}
void av2bv(long long av)
{
	string tmpans="";
	av^=xo;//先异或 
	av+=add;//再加数 
	__int128 x=1;//58**i 
	tmpans+=s[(av/x)%58];//循环计算结果 
	for(int i=0;i<9;i++)
	{
		x*=58;
		tmpans+=s[(av/x)%58];
	}
	string ans=tmpans; //移位 
	ans[0]=tmpans[6];
	ans[1]=tmpans[2];
	ans[2]=tmpans[4];
	ans[3]=tmpans[7];
	ans[4]=tmpans[5];
	ans[5]=tmpans[8];
	ans[6]=tmpans[3];
	ans[7]=tmpans[9];
	ans[8]=tmpans[1];
	ans[9]=tmpans[0];
	cout<<"bv"<<ans<<endl;//输出结果 
}
int main()
{
	for(int i=0;i<58;i++) ma[s[i]]=i;
	cout<<"0\t bvToav \n1\t avTobv\n";
	int op;
	while(scanf("%d",&op)==1)
	{
		if(op)
		{
			long long x;
			cin>>x;
			av2bv(x);
		}
		else
		{
			string s;
			cin>>s;
			bv2av(s);
		}
	}
}
