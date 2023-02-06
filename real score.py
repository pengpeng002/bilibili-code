import requests,json,time
header = {"User-Agent":"Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:64.0) Gecko/20100101 Firefox/64.0"}
methods = ['long', 'short']#长短评
info = {'long':{}, 'short':{}}#长短评分数信息
use_time = 0.0
use = []

def url(methods, id, next):#拼接url
    return 'https://api.bilibili.com/pgc/review/'+methods+'/list?media_id='+id+'&ps=30&sort=0'+("&cursor="+str(next) if next != 0 else "")

def get_data(method, id):#获取评分数据
    next = 0
    lists = []
    global use_time, use
    while True:
        x = time.perf_counter()
        res = session.get(url(method, id, next))#get请求获取评分数据
        use_time = use_time + (time.perf_counter() - x)
        use.append(time.perf_counter() - x)
        data = json.loads(res.text)['data']#string转json
        next = int(data['next'])
        if next == 0:#评分获取完成
            #print(len(lists), data['total'], next)
            return lists, data['total']#评分列表和系统给的评分人数
        lists += data['list']#列表拼接

def get_score(lists,total, method):#计算长评与短评分数
    score = {0:0, 2:0, 4:0, 6:0, 8:0, 10:0}
    sum = 0#总分
    for item in lists:#遍历评分列表
        score[item['score']] += 1
        sum += item['score']
    print(method+':', '\n总分:', sum, '评分人数:', len(lists), '分数:', round(sum/len(lists),2), '\n详情:', score)
    return score#返回评分详情

def get_Rscore(info):#计算总评分，未考虑长评与短评的权值，直接计算
    score = 0
    num = 0#评分人数
    for i in info:
        for s in info[i]:
            score += s*info[i][s]
            num += info[i][s]
    print('总分:', score, '评分人数:', num, '分数:', round(score/num, 2))

if __name__ == '__main__':
    id = input('input id:')
    #use_time = 0.0
    start = time.perf_counter()
    session = requests.session()
    for method in methods:
        lists,total = get_data(method, id)
        score = get_score(lists, total, method)
        info[method] = score
    get_Rscore(info)
    end = time.perf_counter()
    print('总用时:', round(end-start, 2), 'seconds. ', 'get请求获取用时', round(use_time, 2), 'seconds')
    #print(use)
    