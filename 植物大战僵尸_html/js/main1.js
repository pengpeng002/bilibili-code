

main();

//似乎Promise不支持的话，也可以找其他的sleep实现方式
function sleep(time)//自己实现的sleep
{
	return new Promise((resolve) => setTimeout(resolve, time));
}

function clearCanvas()//清空画布
{
	canvas.width=ctxWidth, canvas.height=ctxHeight;
}

function clearGame()//游戏失败后重新加载游戏
{
	location.reload();//刷新当前页面
}

function setAudio(src, loop, auto=true)//设置背景音乐
{
	audios.src=src;
	audios.loop=loop;
	audios.autoplay=auto;
}

function pauseGame() {//暂停游戏
	document.getElementById("options").style.display="block";
	clearInterval(maintimer);//mainTimer 负责重绘canvas，游戏暂停时只要清掉该定时器就实现了暂停
	maintimer=null;
}

function continueGame() {//继续游戏
	document.getElementById("options").style.display="none";
	maintimer = setInterval(mainTimer, 1000/fps);//继续游戏
}

function inBlock(x, y)//返回鼠标是否在草坪上，如果在，更新鼠标所在草坪块号
{
	for (var i=0; i<5; i++)
		for (var j=0; j<9; j++)
			if (blocks[i][j].inArea(x, y))//如果坐标位于这块小草方块区域内
			{
				blockX=i, blockY=j;
				return true;
			}
	return false;
}

function getCard()//获取在的使用卡片
{
	for (var i=0, len=CSeedBank.cards.length; i<len; i++)
		if (CSeedBank.cards[i].isUsing==true) return CSeedBank.cards[i];
	return null;
}

function getZombieNumber(round)//根据僵尸波数计算本波生成的僵尸数量或者说总战斗力
{
	var num=Math.floor(0.333333*round+0.666666+0.01);
	if (round%10==0) num*=2.5;//每10波是一大波
	return num;
}

function bloodPercent()//计算目前存活僵尸血量占本波总血量的百分比
{
	var now=0;//当前剩余总血量
	var total=0;//本波初始总血量
	for (var i=0, len=tmpZombieList.length; i<len; i++)
	{
		if (tmpZombieList[i]=='pt') total+=10;//僵尸本体10点
		else if (tmpZombieList[i]=='hat') total+=10+18;//路障本体10点+帽子18点
		else if (tmpZombieList[i]=='bucket') total+=10+51;//铁桶本体10点+桶子51点
	}
	for (var i=0, len=zombiesInfo.length; i<len; i++)
	{
		now += zombiesInfo[i].blood;
		if (zombiesInfo[i].name!='pt') now+=zombiesInfo[i].coneBlood;
	}
	return now/total;
}

function checkSetPlant(block, name)//检查能否放置植物
{
	if (name == "gatlingpea")//加特林射手只能放在双枪射手上
	{
		if (block.isUsing==true && block.plant.name=="repeater") return true;
		return false;
	}
	if (block.isUsing==false) return true;
	return false;
}

async function createAnimation() {//创建动画图片序列
	await null;//这句话没太大用，单纯为了让本函数以线程和 main 同步执行，
	for (var i=0, len=plants_name.length; i<len; i++)//创建植物动画
	{
		var name=plants_name[i];
		var plant=all_images.plants[name];
		panimation[name]={idle:[], attack:[]};//简单起见，每种植物都有且只有 idle 和 attack 两种状态
		for (let i=0,len=plant['idle'].len; i<len; i++)//加载 idle 动画
		{
			var idx = i < 10 ? '0' + i : i;
			panimation[name]['idle'].push(createImageFromName(plant['idle'].path.replace(/\*/, idx)));
		}
		for (let i=0,len=plant['attack'].len; i<len; i++)//加载 attack 动画
		{
			var idx = i < 10 ? '0' + i : i;
			panimation[name]['attack'].push(createImageFromName(plant['attack'].path.replace(/\*/, idx)));
		}
	}
	
	var actions=["idle","die","dying","dieboom","attack", "cone", "bucket"]//僵尸的状态比较多，但 run 状态没有用到过，估计没用
	for (var i=0, len=actions.length; i<len; i++)//目前只有一种僵尸
	{
		var action=actions[i];
		var info = all_images.zombies[action];
		if (action=="dying")//僵尸的濒死状态分了头部和身体两个部分
		{
			zanimation[action]={head:[], body:[]};
			for (let i=0,len=info['head'].len; i<len; i++)
			{
				var idx = i < 10 ? '0' + i : i;
				zanimation[action]['head'].push(createImageFromName(info['head'].path.replace(/\*/, idx)));
			}
			for (let i=0,len=info['body'].len; i<len; i++)
			{
				var idx = i < 10 ? '0' + i : i;
				zanimation[action]['body'].push(createImageFromName(info['body'].path.replace(/\*/, idx)));
			}
		}
		else
		{
			zanimation[action]=[];
			for (let i=0,len=info.len; i<len; i++)
			{
				var idx = i < 10 ? '0' + i : i;
				zanimation[action].push(createImageFromName(info.path.replace(/\*/, idx)));
			}
		}
	}
}

function createPlant(name, row, col)//根据名字生成不同的植物类
{
	switch (name) {
		case "sunflower": {
			return new CSunflower(name, blocks[row][col].x, blocks[row][col].y, row, col);
			break;
		}
		case "wallnut": {
			return new CWallnut(name, blocks[row][col].x, blocks[row][col].y, row, col);
			break;
		}
		case "chomper": {
			return new CChomper(name, blocks[row][col].x, blocks[row][col].y, row, col);
			break;
		}
		case "cherrybomb": {
			return new CCherrybomb(name, blocks[row][col].x, blocks[row][col].y, row, col);
			break;
		}
		case "peashooter": {
			return new CPeashoter(name, blocks[row][col].x, blocks[row][col].y, row, col);
			break;
		}
		case "repeater": {
			return new CRepeater(name, blocks[row][col].x, blocks[row][col].y, row, col);
			break;
		}
		case "gatlingpea": {
			return new CGatlingpea(name, blocks[row][col].x, blocks[row][col].y, row, col);
			break;
		}
	}
}

function createCard(name, i)//根据名字生成不同的卡片
{
	var card=null;
	switch (name) {
		case "sunflower": {
			card = new CCard(name, i, 7.5, 50);
			break;
		}
		case "wallnut": {
			card = new CCard(name, i, 30, 50, 20);			
			break;
		}
		case "chomper": {
			card = new CCard(name, i, 7.5, 150);
			break;
		}
		case "cherrybomb": {
			card = new CCard(name, i, 50, 150, 30);
			break;
		}
		case "peashooter": {
			card = new CCard(name, i, 7.5, 100);
			break;
		}
		case "repeater": {
			card = new CCard(name, i, 7.5, 200);
			break;
		}
		case "gatlingpea": {
			card = new CCard(name, i, 50, 250, 50);
			break;
		}
	}
	card.timer = setInterval(cooldownTimer, 100, card);
	return card;
}

function createZombie(name, isShow=false)//随机生成僵尸所在位置并返回僵尸对象
{
	// console.log("create, is show = ", isShow);
	var row=Math.floor(Math.random()*5);
	var col=Math.floor(Math.random()*2)+10;
	if (isShow) col+=2;
	var y=20+row*100;
	var x=110+col*80+Math.floor(Math.random()*20+10);//随机后移一段距离，避免同一列的僵尸重合
	if (name=="pt") return new CZombies(name, x, y, row, col, isShow);
	else if (name=="hat") return new CHatZombie(name, x, y, row, col, isShow);
	else if (name=="bucket") return new CBucketZombie(name, x, y, row, col, isShow);

}

function createZombieList(damage)//根据允许的僵尸战斗力随机生成本波产生僵尸
{
	tmpZombieList=[];
	while (damage>0)
	{
		var num = Math.floor(Math.random()*7);//不同僵尸生成概率不同
		if (num<4)
		{
			damage--;
			zombiesInfo.push(createZombie("pt"));
			tmpZombieList.push("pt");
		}
		else if (num<6 && damage>1)
		{
			damage-=2;
			zombiesInfo.push(createZombie("hat"));
			tmpZombieList.push("hat");
		}
		else if (num == 6 && damage>3 && zombieRound>=10)
		{
			damage-=4;
			zombiesInfo.push(createZombie("bucket"));
			tmpZombieList.push("bucket");
		}
		if (zombiesInfo.length > zombieMax) break;
	}
}

function mainTimer()//游戏界面刷新定时器
{
	if (gameOver==true)
	{
		ctx.drawImage(gameOverImg, 190, 60);
		clearInterval(maintimer);
		setAudio(all_audios.gameOver, false);
		return ;
	}
	
	clearCanvas();//清空画布
	
	sh.draw(ctx);//绘制铲子
	sb.draw(ctx);//绘制阳光植物卡槽
	for (var i=0; i<5; i++) {//绘制车子
		cars[i].draw(ctx);
	}
	
	for (var i=0; i<5; i++)//绘制方块上的植物
		for (var j=0; j<9; j++)
			if (blocks[i][j].isUsing == true)
			{
				blocks[i][j].draw(ctx);
			}
			
	for (var i=0, len=sunArray.length; i<len; i++)//绘制阳光
	{
		sunArray[i].draw(ctx);
	}
	
	for (var i=0, len=zombiesInfo.length; i<len; i++)//绘制僵尸
	{
		if (zombiesInfo[i].isDied==true)//如果僵尸已经死亡，则不绘制，并在数组内清除该对象
		{
			zombiesInfo.splice(i, 1);
			i--;
			len--;
		}
		else zombiesInfo[i].draw(ctx);
	}
	
	for (var i=0, len=bullets.length; i<len; i++)//绘制豌豆射手的子弹
	{
		if (bullets[i].isEnd==true)//如果子弹离开草坪或遇到了僵尸，则不绘制，并在数组内清除该对象
		{
			bullets.splice(i, 1);
			i--;
			len--;
		}
		else
		{
			bullets[i].draw(ctx);
		}
	}
	
	var rnd=parseInt(Math.random()*fps*10);//随机经过一段时间自然生成阳光
	if (rnd==5)
	{
		let x=parseInt(Math.random()*700)+100;
		sunArray.push(new CSun(x, 50, 0, 0, true));//随机生成阳光所在位置
	}
	
	if (tmpType!="")//如果玩家准备放置植物或铲除植物，随鼠标移动绘制图片
	{
		if (inBlock(mouseX, mouseY))
		{
			if (tmpType!="shovel" && checkSetPlant(blocks[blockX][blockY], tmpType))//如果此位置可以放置植物
			{
				// var tmp = new CPlant(tmpType, blocks[blockX][blockY].x, blocks[blockX][blockY].y, blockX, blockY);//生成临时植物对象
				var tmp = createPlant(tmpType, blockX, blockY);//根据名字生成不同的植物类
				ctx.globalAlpha = 0.5;
				ctx.beginPath();
				ctx.drawImage(plants_image[tmpType], tmp.x, tmp.y, tmp.width, tmp.height);//绘制半透明图片
				ctx.closePath();
				ctx.save();
				ctx.globalAlpha = 1;
			}
		}
		ctx.drawImage(tmpImage, mouseX-20, mouseY-10);//随鼠标移动绘制图片
	}
	
	if (bigWaveTimes>0)
	{
		bigWaveTimes--;
		ctx.drawImage(paBigWave, 260, 260);
	}
}

function zombieTimer()//僵尸生成定时器，定时生成僵尸
{
	if (zombiesInfo.length >= zombieMax || maintimer==null) return ;//如果游戏暂停或僵尸存在数量过多，则不再生成僵尸
	var dis = Date.now()-preWaveTime;
	// console.log("dis=", dis);
	if (dis<=6000) return ;//两波间隔至少6秒
	if (dis >= nextRoundNeedTime || bloodPercent()<=nextRoundNeedBlood)//
	{
		var damage=getZombieNumber(zombieRound);
		console.log("round:", zombieRound);
		if (zombieRound%10==0)//一大波僵尸
		{
			bigWaveTimes=fps*2;
			createAudio(all_audios.aBigWave, 2500);
		}
		createZombieList(damage);
		nextRoundNeedTime=((Math.random()*7)+25)*1000;//25~32
		nextRoundNeedBlood=((Math.random()*18)+50)/100;//血量百分比 0.50~0.68
		zombieRound++;
		if (zombieRound%10==0)
		{
			nextRoundNeedTime=45*1000;
			nextRoundNeedBlood=0;
		}
		// console.log("next:  ", nextRoundNeedBlood, nextRoundNeedTime);
		preWaveTime=Date.now();
	}
	// zombiesInfo.push(createZombie());
}

function cooldownTimer(card)//植物卡片冷却定时器
{
	if (maintimer==null) return ;//如果游戏暂停，同时暂停冷却
	if (card.cooltime<=0)//冷却时间归零，清除该定时器
	{
		card.cooltime=0;
		clearInterval(card.timer);
		return ;
	}
	card.cooltime-=0.1;
}

function tmpTimer()//临时定时器，用于展示僵尸
{
	clearCanvas();//清空画布
	for (var i=0,len=tmpZombies.length; i<len; i++)//绘制僵尸
	{
		tmpZombies[i].draw(ctx);
	}
}

function dynamic(i)//背景图片左右移动
{
	context.style.backgroundPosition = "-"+i*5+"px";
}

async function run()//游戏开始前背景界面展示，背景图片移动，并显示关卡会出现的僵尸
{
	var tmptimer;
	tmpZombies.push(createZombie("hat", true));
	tmpZombies.push(createZombie("bucket", true));
	for (var i=0; i<8; i++)//随机生成10个仅展示用僵尸
	{
		var num = Math.floor(Math.random()*7);
		var name = num<4?"pt":(num<6?"hat":"bucket");
		tmpZombies.push(createZombie(name, true));
	}
	// console.log(tmpZombies);
	tmptimer=setInterval(tmpTimer, 1000/30);//临时定时器，用于展示僵尸
	
	await sleep(800);
	for (var i=0; i<100; i++)//背景图片左移
	{
		await sleep(10);
		dynamic(i);
		for (let i=0,len=tmpZombies.length; i<len; i++)//因为canvas的位置是固定不动的，背景图片移动时僵尸坐标也一起移动，才能看起来像僵尸始终没动
		{
			tmpZombies[i].x-=5;
		}
	}
	await sleep(2000);//2秒僵尸摇摆时间
	
	clearCanvas();//清空画布
	clearInterval(tmptimer);//删除定时器
	while (tmpZombies.length > 0) tmpZombies.pop();//删除僵尸
	for (var i=100; i>30; i--)//背景图片右移至显示草坪
	{
		await sleep(10);
		dynamic(i);
	}
}

async function ss(totalTime, showTime, src, src2)//闪烁效果
{
	var now=0;
	while (now < totalTime)
	{
		btn_game.style.background=src;
		await sleep(showTime/2);
		
		btn_game.style.background=src2;
		await sleep(showTime/2);
		now+=showTime;
	}
}

async function preStart()//播放恶魔笑声并按键闪烁
{
	setAudio(all_audios.startGame, false);//恶魔笑声音频，不循环
	await ss(3000, 200, "url(images/"+all_images.btn_startImage+")", "url(images/"+all_images.btn_startImage2+")");//闪烁3秒
}

async function readySetPlant()
{
	var pready=createImageFromName(all_images.startReady);
	var pset = createImageFromName(all_images.startSet);
	var pplant = createImageFromName(all_images.startPlant);
	await sleep(30);//保证图片加载完了
	ctx.drawImage(pready, 300, 235);//准备
	await sleep(500);
	ctx.clearRect(300, 235, 300, 133);
	ctx.drawImage(pset, 300, 235);//放置
	await sleep(500);
	ctx.clearRect(300, 235, 300, 133);
	ctx.drawImage(pplant, 300, 235);//植物
	await sleep(1500);
	ctx.clearRect(300, 235, 300, 133);
}

function canvasClick()//画布点击事件函数，游戏与玩家交互
{
	if (gameOver==true) clearGame();
	if (maintimer==null) return ;//游戏暂停状态下不响应交互
	if (tmpType!="")//如果玩家准备放置植物或铲除植物
	{
		if (sh.inArea(mouseX, mouseY) || sb.inArea(mouseX, mouseY))//如果点击在卡片槽或铲子区域，删除临时记录
		{
			if (sh.isUsing==true) sh.isUsing=false;
			else getCard().isUsing=false;
			tmpType="";
			tmpImage=null;
		}
		else if (inBlock(mouseX, mouseY))//如果鼠标是否在草坪上
		{
			if (tmpType=="shovel")//之前点击了铲子
			{
				blocks[blockX][blockY].removePlant();//移除植物
				sh.isUsing=false;//铲子放回
			}
			else
			{
				var card=getCard();//获取在的使用卡片
				if ( checkSetPlant(blocks[blockX][blockY], card.name) )
				{
					var pl = new CPlants(tmpType, blocks[blockX][blockY].x, blocks[blockX][blockY].y, blockX, blockY);
					blocks[blockX][blockY].setPlant(createPlant(tmpType, blockX, blockY));
					card.startCooldown();
					sb.updateValue(-card.sun_consume);
					card.timer = setInterval(cooldownTimer, 100, card);
					createAudio("sounds/plant.ogg");
				}
				else
				{
					card.isUsing=false;
				}
			}
			tmpType="";
			tmpImage=null;
		}
	}
	else
	{
		for (var i=0, len=sunArray.length; i<len; i++)//遍历阳光数组，判断是否在收集阳光
		{
			if (sunArray[i].inArea(mouseX, mouseY))//鼠标点击位置在阳光区域
			{
				sb.updateValue(25);//更新阳光值
				sunArray.splice(i, 1);//该阳光已经被收集，在数组中移除该对象
				return ;
			}
		}
		if (sh.inArea(mouseX, mouseY))//如果点击了铲子
		{
			sh.isUsing=true;//设置铲子已使用
			tmpType="shovel";//记录临时名字与图片
			tmpImage=CShovel.img1;
		}
		else if (sb.inArea(mouseX, mouseY))//如果点击在卡片槽区域
		{
			for (var i=0, len=CSeedBank.cards.length; i<len; i++)//遍历卡片槽卡片
			{
				var card = CSeedBank.cards[i];
				if (card.inArea(mouseX, mouseY) && !card.isCooling() && card.sun_consume<=sb.sunValue)//如果点击了某张卡片且该卡片不在冷却中且阳光值足够
				{
					card.isUsing=true;//设置卡片已使用
					tmpType=card.name;//记录临时名字与图片
					tmpImage=card.plantImage;
				}
			}
			
		}
	}
}

async function startGame()//开始游戏
{
	await preStart();//开始游戏前的按键闪烁以及恶魔笑声的播放
	context.style.background="url(images/"+all_images.mainBackground+")";//设置背景图片
	btn_game.style.display = "none";//隐藏按钮
	setAudio(all_audios.readySetPlant, true);
	await run();//背景图片的左右移动
	
	// dynamic(30);//仅调试用
	await readySetPlant();
	setAudio(all_audios.inGame, true);
	for (var i=0, len = plants_name.length; i<len; i++)//生成卡槽内应放置的植物卡片
	{
		CSeedBank.cards[i]=createCard(plants_name[i], i);
	}
	sb = new CSeedBank();
	sb.draw(ctx);//绘制阳光植物卡槽
	
	for (var i=0; i<5; i++)//绘制车子
	{
		var car=new CCar(40, 110+i*100, i+1);
		car.draw(ctx);
		cars.push(car);
	}
	// preWaveTime=Date.now();
	sh = new CShovel(CSeedBank.img.width+10, 0);
	sh.draw(ctx);//绘制铲子
	
	btn_menu.style.display="block";//显示菜单按钮，实际上是暂停游戏
	maintimer = setInterval(mainTimer, 1000/fps);//主定时器，定时刷新重绘整个游戏界面
	
	setTimeout(()=>{
		let audio = document.createElement("audio");
		audio.src = all_audios.awooga;
		audio.autoplay = true;
		document.body.appendChild(audio);
		
	},10000);
	setInterval(zombieTimer, 1000);//僵尸生成定时器，定时生成僵尸
	canvas.onmousemove = function (event) {//监听canvas内鼠标移动
        let e = event || window.event;
		scrollX = document.documentElement.scrollLeft || document.body.scrollLeft;
		scrollY = document.documentElement.scrollTop || document.body.scrollTop;
		mouseX = e.pageX ||e.clientX + scrollX;
		mouseX -= context.offsetLeft;//将浏览器内坐标转化为canvas内坐标
		mouseY = e.pageY || e.clientY + scrollY;
		mouseY -= context.offsetTop;
    }	
	canvas.onclick = canvasClick;//监听canvas内鼠标点击
}

//main函数只做了游戏开始前的一些配置，游戏开始的代码在startGame
function main()
{
	//浏览器不允许用户和网页交互前就自动播放音乐
	// setAudio(all_audios.mainTheme, true);
	// if (audios.pause) console.log("pause");
	// audios.play();
	createAnimation();//以线程的方式执行createAnimation，节省一点点主界面加载时间
	gameOverImg=createImageFromName(all_images.gameOver);
	for (var i=0; i<5; i++)//草坪，分成5*9块
	{
		blocks[i]=[];
		for (var j=0; j<9; j++)
		{
			blocks[i][j]=new CBlock(i, j);
		}
	}
	
	plants_image['sunflower'] = createImageFromName(all_images.sunflower);//加载植物图片
	plants_image['cherrybomb'] = createImageFromName(all_images.cherrybomb);
	plants_image['chomper'] = createImageFromName(all_images.chomper);
	plants_image['gatlingpea'] = createImageFromName(all_images.gatlingpea);
	plants_image['peashooter'] = createImageFromName(all_images.peashooter);
	plants_image['repeater'] = createImageFromName(all_images.repeater);
	plants_image['wallnut'] = createImageFromName(all_images.wallnut);
	paBigWave=createImageFromName(all_images.aBigWave);
	
	document.getElementById('btn-startGame').onclick=startGame;//启动游戏按钮的监听，游戏主体代码都写在这个函数
	document.getElementById("btn-menu").onclick=pauseGame;//菜单按钮，实际上是暂停游戏
	document.getElementById("btn-continue").onclick=continueGame;//继续游戏按钮
	
	canvas.width = 900;//设置画布的宽和高，似乎只能在js设置 在css或html写就会有奇怪的现象
	canvas.height = 600;
	
}

