#include <bits/stdc++.h>
#include "winsock2.h"//����������� -lwsock32
using namespace std;
SOCKET m_socket;//�׽���socket 
string methods[] = {"long", "short"};//��������� 
string host, object;//����url�����ó���������������Դ 
string head, body;//��Ӧͷ����Ӧ�� 
string all_list="";//json.data.list�ļ��� 
string all_head="";
char buf[1026];//recv������ 
map<string , string> headers;//����ͷ 
int score_list[2][12];//����+���� ���������� 
int slist[12];//��ʱ�洢�������� 
int score_sum=0, num=0;//�ܷ��������� 
string s="score";//�������ַ��� score 
string pre="";//֮ǰδ�����Ĵ� 
bool con_break = true; 
int req_times = 0;
int recv_time[10240], data_time[10240];
int get_times(string a, string b)//����b��a�г��ִ�����δ���� 
{
	int idx=0, times=0;
	while((idx=a.find(b, idx)+1)) times++;
	return times;
} 
string get_url(string method, int id, long long next)//����ƴ��url 
{
	return "https://api.bilibili.com/pgc/review/"+method+"/list?media_id="+to_string(id)+\
	"&ps=30&sort=0"+(next ? "&cursor="+to_string(next) : "");
}
void init()//��ʼ���������׽��� 
{
	WSADATA a;
	if (WSAStartup(MAKEWORD(2, 2), &a))//��ʼ���׽��ֿ� 
	{
		printf("init fail\n");
		exit(0);
	}

	m_socket = socket(AF_INET, SOCK_STREAM, 0);//TCPͨ��
	if (m_socket == SOCKET_ERROR)
	{
		WSACleanup();
		printf("����socketʧ��\n");
		exit(0);
	}

}
void getDate()//��ȡϵͳ��ǰ����ʱ�䣬δ���� 
{
	time_t t = time(0); 
	char tmp[32];
	strftime(tmp, sizeof(tmp), "%Y-%m-%d %H:%M:%S",localtime(&t));
}
void set_headers(string url)//��������ͷ 
{
	headers[":authority"]=host;
	headers[":method"]="GET";
	headers[":path"]=object;
//	headers[":scheme"]="https";
//	headers["User-Agent"]="Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:88.0) Gecko/20100101 Firefox/88.0";
	headers["Accept"]="text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,*/*;q=0.8";
//	headers["Accept-Language"]="zh-CN,zh;q=0.8,zh-TW;q=0.7,zh-HK;q=0.5,en-US;q=0.3,en;q=0.2";
//	headers["Cookie"]="-1";
//	headers["Accept"]="text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3;q=0.9";
	headers["accept-encoding"]="gzip, deflate, br";
	headers["accept-language"]="zh-CN,zh;q=0.9,en;q=0.8";
	headers["cache-control"]="no-cache";
	headers["cookie"]="-1";
	headers["pragma"]="no-cache";
	headers["user-agent"]="Mozilla/5.0 (Windows NT 6.3; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/109.0.0.0 Safari/537.36";
//	headers["URL"]="https://default.bilibili.com/pgc/review/long/list?media_id=4315402&ps=30&sort=0";
//	headers["Server"]="cn-jsnt-dx-w-v6-13";
//	time_t t = time(0); 
//	char tmp[32]={NULL};
//	strftime(tmp, sizeof(tmp), "%Y/%m/%d %H:%M:%S",localtime(&t));
//	headers["Date"]=tmp;
}
string analyse_url(string url)//����url, ��ȡhost��object������host 
{
	// http:// abcd.efg /abc/def/ghi
	// object = /abc/def/ghi        host = abcd.efg
	object = "/";
	int pos = url.find("://");//check if url start with https:// or http://
	pos += (pos==url.npos ? 1 : 3); //string::npos == -1 
	int pos2 = url.find('/', pos);//����https:// ����� / 
	if (pos2 == string::npos) return host = url.substr(pos2);
	object = url.substr(pos2);
	return host = url.substr(pos, pos2-pos);
///�����壬δʹ��	
	bool use_https;
	if (url.rfind("https://", 0) == 0)//url start with https://
	{
		int pos = url.find('/', 8);
		use_https = true;
		if (pos == string::npos) return url.substr(8);
		object = url.substr(pos);
		return url.substr(8, pos-8);
	}
	else if (url.rfind("http://", 0) == 0)//url start with http://
	{
		int pos = url.find('/', 7);
		use_https = false;
		if (pos == string::npos) return url.substr(7);
		object = url.substr(pos);
		return url.substr(7, pos-7);
	}
	else
	{
		int pos = url.find('/');
		use_https = false;
		if (pos == string::npos) return url;
		object = url.substr(pos);
		return url.substr(0, pos);
	}
}
void Connect(string url)//����url����host�������� 
{
//	bool use_https = false;
	host = analyse_url(url);
	//cout << host << endl;
	hostent *p = gethostbyname(host.c_str());//get ipv4 address
	if (p == NULL)
	{
		printf("get host fail\n");
		exit(0);
	}
	sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(80);
	memcpy(&addr.sin_addr, p->h_addr, 4);
	//addr.sin_addr.S_un.S_addr = inet_addr("192.168.1.252");
	
	if (connect(m_socket, (sockaddr*)&addr, sizeof(addr)))//������� api.bilibili.com �������� 
	{
		printf("connect error, error code: %d\n", WSAGetLastError());
		exit(0);
	}
	con_break = false;
}
void calc(string str)//����score�����з������� 
{
//	str=pre+str;
	int idx=0;
	int las = 0;
	while ((idx = str.find(s, idx)+1))
	{
		if (idx+6 >= str.length()) break;
		int pos=str.find(',', idx+6);
		int score = atoi(str.substr(idx+6, pos).c_str());
		slist[score]++;
		score_sum += score, num++;
//		las = pos;
	}
//	pre = str.substr(las);
}
void session_get(string url)//�������󲢱�����Ӧ������head��body 
{
	host = analyse_url(url);
	string req = "GET "+object+" HTTP/1.1\r\nHost: "+host+"\r\n";
	for(auto i:headers)
	{
		if(i.second != "-1")
		req+=i.first+": "+i.second+"\r\n";
	}
	req+="Connect:Close\r\n\r\n";
//	cout << "request: \n" << req << endl;
	
	if (send(m_socket, req.c_str(), req.size(), 0) == SOCKET_ERROR)
	{
		printf("send error, error code: %d\n", WSAGetLastError());
		closesocket(m_socket);
		exit(0);
	}
	
	int nNetTimeout=200*5;
	setsockopt(m_socket,SOL_SOCKET,SO_RCVTIMEO, (const char*)&nNetTimeout,sizeof(int));//����recv�ȴ���ʱʱ�� 
	time_t a=clock();
	char ch;
	bool f = false;
	head = "";
	time_t aa=clock();
	while(recv(m_socket, &ch, 1, 0))//����\n\r��ʾ��Ӧͷ��ȡ��� ����ѭ�� 
	{
//		if (clock()-aa > 50) printf("�����˵ȴ�%d ms\n", clock()-aa);
		head += ch;
		if(f==true)
		{
			if(ch=='\r') break;
			else f=false;
		}
		else if(ch=='\n') f=true;
	}
	recv_time[req_times]=clock()-a;
//	cout << "got head " << head;
	if(head.find("close") != head.npos) con_break = true;
	body = "";
//	memset(buf, 0, sizeof(buf));
	
	int tt=0; 
	int got_len = 0;
	f = false;
	while(got_len = recv(m_socket, &ch, 1, 0) > 0)//����\r\n\r\n��ʾ��Ӧͷ��ȡ��� ����ѭ�� 
	{
		body += ch;
		if(f==true)
		{
			if(ch=='\r')
			{
//				cout << "read ok\n";
				break;
			}
			else f=false;
		}
		else if(ch=='\n') f=true;
//		if (got_len == -1) tt++;
//		else tt=0;
//		if (tt >= 2)
//		{
//			printf("����5��δ�յ����ݣ��˳�\n");
//			//cout << body;
//			break;
//			freopen("body_error.txt", "w", stdout);
//			cout << body;
//			freopen("CON", "w", stdout);
//			printf("����������������˳�\n");
//			exit(0);
//			tt=0;
//		}
	}
//	time_t b = clock();
	
//	cout << "got body " << body;
//	while(got_len = recv(m_socket, buf, 1024, 0) )
//	{
//		time_t b = clock();
//		if (got_len == -1) tt++;
//		else tt=0;
////		printf("this recv use time: %lf ms, got_len=%d\n", (b-a)*1.0, got_len);
////		calc(buf);
//		body += buf;
//		memset(buf, 0, sizeof(buf));
//		if (body.find("success") != body.npos) break;
//		if (tt >= 20)
//		{
//			freopen("body_error.txt", "w", stdout);
//			cout << body;
//			freopen("CON", "w", stdout);
//			printf("����������������˳�\n");
//			exit(0);
//			tt=0;
//		}
//		a=clock();
//	}//body+=buf;
//	cout << "body length:" << body.length() /*<< ", len=" << got_len*/ << endl;
	if (con_break == true) closesocket(m_socket);
}
void wash_data()
{
	string s="\r\n";
	int pre=-55;
	int pos=0;
	while (pos = body.find(s, pos))
	{
		if (pos == -1) break;
		if (pos-pre < 10)
		{
			body.erase(pre, pos-pre+2);
		}
		pre=pos;
	}
}
void get_data(string method, int id)//��ȡ�������� ���浽all_list 
{
	long long next = 0;
//	all_list = "";
//	all_head = "";
//	init();
//	string url = get_url(method, id, next);
	
	while(true)
	{
//		time_t st = clock();
		string url = get_url(method, id, next);
		if (con_break == true)
		{
			init();
			Connect(url);
		}
		set_headers(url);
		
		time_t a=clock();
		session_get(url);
		data_time[req_times]=clock()-a;
		req_times++;
		wash_data();
//		time_t ed = clock();
//		printf("use time: %.2lf seconds\n", (ed-st)/1000.0);	
		
		//get next with body
		int pos = body.find("next\":") + 6;
		int pos2 = body.find(',', pos);
		if(pos == 5 || pos2 == -1)//find����ֵΪnpos���Ҳ���next �� , ��ʾ����δ��� 
		{
			printf("body is error\n");
			freopen("body_error.txt", "w", stdout);
			printf("head=%sbody=%s\n", head.c_str(), body.c_str());
			freopen("CON", "w", stdout);
			freopen("out_list.txt", "w", stdout);
			printf("%s", all_list.c_str());
			freopen("CON", "w", stdout);
			freopen("out_head.txt", "w", stdout);
			printf("%s", all_head.c_str());
			freopen("CON", "w", stdout);
			exit(0);
		}
		string snext = body.substr(pos, pos2-pos);
		sscanf(snext.c_str(), "%lld", &next);
//		printf("pos:%d, pos2:%d, snext:%s, next: %lld\n", pos, pos2, snext.c_str(), next);
		
		if (next == 0) break;//����������ȫ����ȡ 
		calc(body);
		//json = {"code":0, "data":{..., "list":[{l1}, {l2}...]}, "next":xxx, ...}, "message":"success"}
//		int pos4 = body.find("\"list\":[")+8;//json.data.list 
//		string List = body.substr(pos4, pos-9-pos4);
////		printf("%d\n", get_times(List, "article_id"));
////		all_list += body.substr(pos4, pos-9-pos4)+",";
//		all_list += body+"\n\n\n\n\n";
//		all_head += head+"\n\n\n\n\n";
	}
	printf("body is all ok\n");
//	freopen("body_ok.txt", "w", stdout);
//	printf("%s\n", all_list.c_str());
//	freopen("CON", "w", stdout);
//	closesocket(m_socket);
//	return ;
//	freopen("out.txt", "w", stdout);
//	cout << all_list;
//	freopen("CON", "w", stdout);
}
void get_score(string method)//����������������� 
{
//	printf("calc method: %s\n������: %d ������: %d �÷�: %.2lf\n", method=="short"?"����":"����", score_sum, num, score_sum*1.0/num);
//	memset(slist, 0, sizeof(slist));
//	score_sum=0, num=0;
//	string s="score";
//	int idx=0;
//	while ((idx=all_list.find(s, idx)+1))
//	{
////		num++;
//		int pos=all_list.find(',', idx+6);
//		slist[atoi(all_list.substr(idx+6, pos).c_str())]++;
//	}
//	for (int i=2; i<12; i+=2)
//	{
//		score_sum += i*slist[i];
//		num += slist[i];
//	}
	printf("method: %s\n������: %d ������: %d �÷�: %.2lf\n����: ", method=="short"?"����":"����", score_sum, num, score_sum*1.0/num);
	for (int i=2; i<12; i+=2)
	{
		printf("%s%d: %d ",(i==2?"":","), i, slist[i]);
	}
	printf("\n");
//	memset(slist, 0, sizeof(slist));
//	score_sum=0, num=0;
}
void get_Rscore()
{
	int score_sum=0, num=0, idx=0;
	for (int j=0; j<2; j++)
	for (int i=2; i<12; i+=2)
	{
		score_sum += i*score_list[j][i];
		num += score_list[j][i];
	}
	printf("������: %d ������: %d �÷�: %.2lf\n", score_sum, num, score_sum*1.0/num);
}
int main()
{
	int id;
	printf("input id: ");
	scanf("%d", &id);
	time_t st = clock();
	for (int i=0; i<2; i++)
	{
		memset(slist, 0, sizeof(slist));
		score_sum=0, num=0;//, idx=0;
//		pre = "";
		get_data(methods[i], id);
//		time_t a=clock();
		get_score(methods[i]);
		if(i==0) printf("������ʱ%.2lfs\n", (clock()-st)*1.0/1000);
//		time_t b=clock();
//		printf("get score use time %d ms\n", b-a);
		for(int j=2; j<12; j+=2) score_list[i][j] = slist[j];
//		break;
	}
	get_Rscore();
	time_t ed = clock();
	printf("����: %.2lf seconds\n", (ed-st)*1.0/1000);
	int rt=0, dt=0;//recv_time, data_time
	for (int i=0; i<req_times; i++) rt+=recv_time[i], dt+=data_time[i];
	printf("���� %.2lf seconds,  recv time %.2lf seconds\n", dt*1.0/1000, rt*1.0/1000);
}
