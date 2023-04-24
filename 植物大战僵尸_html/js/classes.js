


//根据图片名字返回image对象
const createImageFromName = function(src) {
	// console.log("src="+src);
	let img = new Image();
	img.src = 'images/' + src;
	return img;
}
//临时音效
async function createAudio(src, time=1000)
{
	let audio = document.createElement("audio");
	audio.src = src;
	audio.autoplay = true;
	document.body.appendChild(audio);
	await sleep(time);
	document.body.removeChild(audio);
}

//Chorme无法使用，不允许用本地文件 不过我也没用
function toGray(ctx, x, y, w, h)
{
	var imageData = ctx.getImageData(x, y, w, h);
	var data=imageData.data;
	for (var i=0, len=data.length; i<len; i+=4)
	{
		var r=data[i], g=data[i+1], b=data[i+2], a=data[i+3];
		var ave=(r+g+b)/3;
		data[i]=data[i+1]=data[i+2]=ave;
	}
	ctx.putImageData(imageData, x, y);
}

//判断 (x,y) 是否在obj的区域里
function inArea(x, y, obj)
{
	return x>=obj.x && x<=obj.x+obj.width && y>=obj.y && y<=obj.y+obj.height;
}

//阳光
class CSun {
	constructor(x, y, row, col, canMove=false) {//自然生成的会下落，即canMove=true。向日葵生成的静止不动
		this.x = x;
		this.y = y;
		this.row=row;//row 和 col 没有意义，可以忽略
		this.col = col;
		this.width = CSun.img.width;
		this.height = CSun.img.height;
		this.canMove=canMove;//是否运动
	}
	draw(ctx)//绘制并计算下一帧所应该在的位置
	{
		ctx.drawImage(CSun.img, this.x, this.y);
		this.nextPosition();
	}
	inArea(x, y)
	{
		return inArea(x, y, this);
	}
	nextPosition()
	{
		if (this.canMove==false) return ;
		if (this.y > 350) this.canMove=false;//避免阳光无止境的下落
		this.y+=4;
	}
}
CSun.img = createImageFromName(all_images.sun);
//子弹 即 豌豆
class CBullet {
	constructor(x, y, row) {
		this.x=x+50;
		this.row=row;
		this.y=y;
		this.width=28;
		this.height=28;
		this.isEnd=false;//是否离开草坪或遇到了僵尸
		// console.log(x, y, row);
	}
	draw(ctx)
	{
		// ctx.clearRect(this.x-10, this.y, this.width, this.height);
		if (this.isEnd) return ;
		ctx.drawImage(CBullet.img, this.x, this.y);
		this.nextPosition();
	}
	nextPosition()
	{
		if (this.x > 900) this.isEnd=true;
		for (var i=0; i<zombiesInfo.length; i++)//遍历僵尸对象，查找是否碰到僵尸
		{
			if (this.row == zombiesInfo[i].row)//如果子弹和僵尸处于同一行
			{
				// console.log("bullet: ", this.x, zombiesInfo[i].x, this.row);
				var dis=this.x-(zombiesInfo[i].x+30);//离僵尸的距离，在特定范围内则视为碰到了僵尸
				if (dis>=2 && dis<=12)
				{
					zombiesInfo[i].hurt();//碰到了僵尸，僵尸受伤，一次一点血
					this.isEnd=true;//子弹消失
					return ;
				}
			}
		}
		this.x+=10;
	}
	
}
CBullet.img = createImageFromName(all_images.bullet);


//僵尸类
class CZombies {
	constructor(name, x, y, row, col, isShow=false) {
		this.blood=10;//僵尸的血量
		this.haveBlood=this.blood;
		this.name = name;
		this.x=x;
		this.y=y;
		this.head_x=x;
		this.head_y=y;//头的坐标，实际上也没有用到
		this.row=row;//僵尸在草坪的第几行
		this.col = col;//僵尸在草坪的第几列
		this.type='idle';//僵尸状态，初始为idle，即正常行走
		this.framei=0;//播放到了动画的第几帧
		this.animation=zanimation;//动画
		this.isDied=false;//是否已经完全死亡
		this.isShow=isShow;//是否是单纯展示给玩家看的
		this.hurting=false;
		this.start = Date.now();
		// console.log(this);
	}
	draw(ctx)
	{
		if (this.isDied) return ;//如果僵尸已经完全死亡，则不会绘制		
		if (this.x <= 850 || this.isShow==true)//如果僵尸不在草坪上，则不进行绘制，反正玩家看不到
		{
			if (this.type == 'dying')//如果是濒死状态，头部与身体需要分开绘制
			{
				if (this.animation[this.type]['head'].length > this.framei)
				{
					ctx.drawImage(this.animation[this.type]['head'][this.framei], this.x+30, this.y);//绘制头部
				}
				ctx.drawImage(this.animation[this.type]['body'][this.framei], this.x, this.y);//绘制身体
			}
			else
			{
				var img=this.animation[this.type][this.framei];
				if (this.hurting && (this.type=='idle' || this.type=='attack'))
				{
					ctx.globalAlpha = 0.5;
					ctx.beginPath();
					ctx.drawImage(img, this.x, this.y);
					ctx.closePath();
					ctx.save();
					ctx.globalAlpha = 1;
					this.hurting=false;
				}
				else ctx.drawImage(img, this.x, this.y);
			}
		}
		
		this.nextPosition();//下一帧自己应该在的位置
		if (this.isShow==false)//仅展示用僵尸只要循环idle动画即可
		{
			this.switchType();//切换状态
			this.getHurt();//判断周围环境，自己是否受伤
		}
		// console.log(this.row, this.col, this.x);
	}
	getCol()//根据x坐标计算僵尸现在在草坪的第几列
	{
		return Math.floor((this.x)/80);
	}
	nextPosition()//下一帧所在位置
	{
		// if (this.col==7) return ;
		this.framei++;
		if (this.isShow==true && this.framei==this.animation[this.type].length)
		{
			this.framei=0;
			return ;
		}
		if (this.type=='idle' && this.isShow==false)
		{
			if (Date.now()-this.start >= 130)
			{
				this.x-=2;
				this.start=Date.now();
				this.col = this.getCol();//只有idle状态才会移动
				if (this.x <= -90)
				{
					console.log("you have died!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
					gameOver=true;
				}
			}
		}
	}
	switchType()//判断并进行状态切换,以及对植物进行攻击
	{
		switch (this.type)
		{
			case "idle": {
				if (this.framei==this.animation[this.type].length) this.framei=0;
				if (this.col<9 && this.col>=0 && blocks[this.row][this.col].isUsing == true) this.type='attack', this.framei=0;//所在位置有植物，则切换为attack状态
				break;
			}
			case "attack": {
				if (blocks[this.row][this.col].isUsing == false) this.type='idle', this.framei=0;//所在位置无植物，则切换为idle状态
				else
				{
					if (this.framei==this.animation[this.type].length)//if framei==21 then framei=0
					{
						this.framei=0;
						
					}
					if (this.framei%10==1)//1 11
					{
						blocks[this.row][this.col].plant.hurt();
						createAudio('sounds/bigchomp.ogg');
					}
				}
				break;
			}
			case "dying": {
				if (this.framei==this.animation[this.type]['body'].length) this.framei=0, this.type='die';//在dying状态并且dying动画已经播放结束，则播放die动画
				break;
			}
			case "die":
			case "dieboom": {
				if (this.framei==this.animation[this.type].length)//die状态且动画播放结束，僵尸彻底死亡
				{
					this.isDied=true;
				}
			}
		}
		
	}
	startDie(isBoom=false)//开始死亡
	{
		this.blood=0;
		this.framei=0;
		this.type = isBoom ? "dieboom" : "dying";//
	}
	getHurt()//判断是否应该受伤，即对周围的环境监测
	{
		if (this.blood<=0) return ;//只有活着才考虑是否受到伤害
		if (cars[this.row].died==false && cars[this.row].x - this.x>2)//僵尸碰到了车子
		{
			this.startDie();
			cars[this.row].start();
			return ;
		}
	}
	hurt(x=1, isBoom=false)//僵尸受伤，扣血
	{
		if (this.blood>0)//只有血量大于0才会扣血
		{
			this.blood-=x;
			if (this.blood<=0) this.startDie(isBoom);//扣完血后血量归零，进入死亡状态
			else this.hurting=true;
		}
	}
}
//cone 370  self 200(=10) 路障僵尸
class CHatZombie extends CZombies {
	constructor(name, x, y, row, col, isShow=false)
	{
		super(name, x, y, row, col, isShow);
		this.isShow=isShow;
		this.coneblood=18;//370 
		this.conex=this.x+70-16;
		this.coney=this.y-12;
		this.conew=59;
		this.coneh=57;
		// console.log(this);
	}
	draw(ctx)
	{
		super.draw(ctx);
		const dx=73;
		let dy=0;
		if (this.type=='idle')
		{
			if (this.framei>=5 && this.framei<=7) dy=-2;
			else if (this.framei>=18&&this.framei<=26) dy=-3;
		}
		if (this.coneblood>0)
		{
			if (this.coneblood>=12)
			{
				ctx.drawImage(this.animation['cone'][0], this.x+dx, this.coney+dy, this.conew,this.coneh);
			}
			else if (this.coneblood>=6)
			{
				ctx.drawImage(this.animation['cone'][1], this.x+dx, this.coney+dy, this.conew,this.coneh);
			}
			else
			{
				ctx.drawImage(this.animation['cone'][2], this.x+dx, this.coney+dy, this.conew,this.coneh);
			}
		}
	}
	startDie(isBoom=false)
	{
		super.startDie(isBoom);
		this.coneblood=0;
	}
	hurt(x=1, isBoom=false)
	{
		if (this.coneblood>0)
		{
			this.coneblood-=x;
			if (isBoom)
			{
				this.startDie(true);
			}
		}
		else
		{
			super.hurt(x, isBoom);
		}
	}
}
//铁桶僵尸
class CBucketZombie extends CZombies {
	constructor(name, x, y, row, col, isShow=false)
	{
		super(name, x, y, row, col, isShow);
		this.isShow=isShow;
		this.coneblood=51;//1100 
		this.conex=this.x+51;
		this.coney=this.y-4;
		this.conew=59;
		this.coneh=57;
	}
	draw(ctx)
	{
		super.draw(ctx);
		const dx=73;
		if (this.coneblood>0)
		{
			if (this.coneblood>=34)
			{
				ctx.drawImage(this.animation['bucket'][0], this.x+dx, this.coney, this.conew,this.coneh);
			}
			else if (this.coneblood>=17)
			{
				ctx.drawImage(this.animation['bucket'][1], this.x+dx, this.coney, this.conew,this.coneh);
			}
			else
			{
				ctx.drawImage(this.animation['bucket'][2], this.x+dx, this.coney, this.conew,this.coneh);
			}
		}
	}
	startDie(isBoom=false)
	{
		super.startDie(isBoom);
		this.coneblood=0;
	}
	hurt(x=1, isBoom=false)
	{
		if (this.coneblood>0)
		{
			this.coneblood-=x;
			if (isBoom)
			{
				this.startDie(true);
			}
		}
		else
		{
			super.hurt(x, isBoom);
		}
	}
}
//植物类
class CPlant {
	constructor(name, x, y, row, col, blood) {
		this.animation=[];	//动画图片
		this.animation.push(panimation[name]['idle']);
		this.animation.push(panimation[name]['attack']);
		this.name = name;//植物名字
		this.type = 0;//当前状态，0为idle， 1位攻击
		this.framei = 0;//播放到第几帧
		this.blood=blood;//植物血量
		this.row=row;
		this.col=col;
		this.x=x;
		this.y=y;
		this.died=false;//是否已经死亡
		this.hurting=false;//是否在被啃食
		this.width=plants_image[this.name].width;
		this.height=plants_image[this.name].height;
	}
	draw(ctx) {
		if (this.hurting)//被啃食绘制半透明图片
		{
			// console.log("draw half parent");
			ctx.globalAlpha = 0.5;
			ctx.beginPath();
			ctx.drawImage(this.animation[this.type][this.framei], this.x, this.y, this.width, this.height);
			ctx.closePath();
			ctx.save();
			ctx.globalAlpha = 1;
			this.hurting=false;
		}
		else ctx.drawImage(this.animation[this.type][this.framei], this.x, this.y, this.width, this.height);
		
		this.nextFrame();
	}
	zombieInArea() //僵尸在植物攻击区域
	{
		// return true;
		for (var i=0, len=zombiesInfo.length; i<len; i++)
		{
			if (zombiesInfo[i].row == this.row && zombiesInfo[i].x > this.x && zombiesInfo[i].x<820) return true;
		}
		return false;
	}
	changeType()//是否需要切换状态
	{
		if (this.type==0 && this.zombieInArea()) return true;
		if (this.type==1 && !this.zombieInArea()) return true;
	}
	nextFrame() {
		this.framei++;
		if (this.framei==this.animation[this.type].length) this.framei=0;
	}
	hurt(x=1)
	{
		if (this.blood>0)
		{
			this.blood-=x;
			this.hurting=true;
			if (this.blood<=0) this.died=true;
		}	
	}
}
//向日葵可坚持6次啃食，  或僵尸秒伤100，血量300
class CSunflower extends CPlant {
	constructor(name, x, y, row, col) {
		super(name, x+10, y+10, row, col, 6);
		this.round=Math.floor(Math.random()*5+4);
		var scale =70 / plants_image[this.name].height;
		this.height= scale * plants_image[this.name].height;
		this.width = scale * plants_image[this.name].width;
		this.start=Date.now();
		this.initTime=(Math.random()*5+17)*1000;
	}
	nextFrame() {
		super.nextFrame();
		if (Date.now()-this.start>=24000-this.initTime)//每24秒生成一颗阳光
		{
			sunArray.push(new CSun(this.x-30, this.y+20, this.row, this.col));
			this.start=Date.now();
			this.initTime=0;
		}
	}
}
//僵尸秒伤100，血量4000
class CWallnut extends CPlant {
	constructor(name, x, y, row, col) {
		super(name, x+10, y+10, row, col, 80);
		var scale =70 / plants_image[this.name].height;
		this.height= scale * plants_image[this.name].height;
		this.width = scale * plants_image[this.name].width;
	}
	nextFrame() {
		this.framei++;//高中低血量都是idle状态
		if (this.framei==16 && this.blood>55)
		{
			this.framei=0;
		}
		else if (this.framei==27 && this.blood>30)
		{
			this.framei=16;
		}
		else if (this.framei==42 && this.blood>0)
		{
			this.framei=27;
		}
	}
	hurt(x=1)
	{
		super.hurt(x);
		if (this.blood==55) this.framei=16;
		else if (this.blood==30) this.framei=27;
	}
}
//豌豆射手
class CPeashoter extends CPlant {
	constructor(name, x, y, row, col) {
		super(name, x+10, y+10, row, col, 6);
		this.start=Date.now();
	}
	nextFrame()
	{
		super.nextFrame();
		if (this.changeType())
		{
			if (this.type==0) this.type=1, this.shoot();
			else this.type=0;
			this.framei=0;
			return ;
		}
		this.canShoot();
	}
	canShoot()
	{
		if (this.type==1 && Date.now()-this.start>=1400)
		{
			this.shoot();
			this.start=Date.now();
		}
	}
	shoot()
	{
		var cb=new CBullet(this.x, this.y, this.row);
		bullets.push(cb);
	}
}
//樱桃炸弹
class CCherrybomb extends CPlant {
	constructor(name, x, y, row, col) {
		super(name, x-10, y+10, row, col, 1);
		this.width=plants_image[this.name].width;
		this.height=plants_image[this.name].height;
		this.start=Date.now();
	}
	nextFrame() {
		if (Date.now()-this.start>=100)//0.1秒一张图片，使爆炸准备时间为0.7秒.不过原版似乎是3.5秒。这里属于设计有问题
		{
			this.start=Date.now();
			this.framei++;
			if (this.framei==this.animation[this.type].length)
			{
				this.framei=0;
				if (this.type==0) this.type=1, this.hurtZombie();
				else this.died=true;
			}
		}
	}
	hurtZombie() //炸死僵尸
	{
		for (var i=0, len=zombiesInfo.length; i<len; i++)
		{
			if (Math.abs(zombiesInfo[i].row-this.row)<=1 && Math.abs(zombiesInfo[i].col-this.col)<=1) zombiesInfo[i].hurt(50, true);
		}
	}
	hurt()//樱桃炸弹免疫僵尸啃食
	{
		return ;
	}
}
//双枪豌豆射手
class CRepeater extends CPeashoter {
	constructor(name, x, y, row, col) {
		super(name, x, y, row, col, 6);
	}
	shoot()
	{
		var cb=new CBullet(this.x, this.y, this.row);//双枪一次两颗豌豆
		bullets.push(cb);
		setTimeout(()=> {bullets.push(new CBullet(this.x, this.y, this.row));}, 250);
	}
}
//大嘴花
class CChomper extends CPlant {
	constructor(name, x, y, row, col) {
		super(name, x-10, y-20, row, col, 6);
		this.width=plants_image[this.name].width;
		this.height=plants_image[this.name].height;
		this.tmpZombie=null;//可攻击的僵尸
		this.start=Date.now();
	}
	nextFrame()
	{
		this.framei++;
		if (this.changeType())
		{
			if (this.type==0) this.type=1;
			else this.type=0;
			this.framei=0;
			return ;
		}
		if (this.framei==this.animation[this.type].length) 
		{
			if (this.type==0) this.framei=0;
			else this.framei=9;//消化状态图片从9开始
		}
		
		if (this.type==1)//消化状态放在了攻击状态后面
		{
			if (this.framei==6)
			{
				this.tmpZombie.blood=0;
				this.tmpZombie.isDied=true;//吃掉僵尸，没考虑吃空的情况了，虽然确实存在
			}
			else if (this.framei==8) this.start=Date.now();//开始消化
		}
	}
	zombieInArea() //僵尸在攻击区域
	{
		for (var i=0, len=zombiesInfo.length; i<len; i++)
		{
			if (zombiesInfo[i].row == this.row && Math.abs(zombiesInfo[i].col-this.col)<=1)//可以吃同一排前后一格
			{
				this.tmpZombie=zombiesInfo[i];
				return true;
			}
		}
		return false;
	}
	changeType()//植物状态是否需改变
	{
		if (this.type==0 && this.zombieInArea()) return true;
		if (this.type==1)
		{
			if (this.framei>=9 && Date.now()-this.start >= 42000) return true;//42秒消化时间
		}
	}
	
}
//加特林射手
class CGatlingpea extends CPeashoter {
	constructor(name, x, y, row, col) {
		super(name, x, y, row, col, 6);
	}
	shoot()
	{
		var dy=this.y+12;//加特林一次4颗豌豆
		var cb=new CBullet(this.x, dy, this.row);
		bullets.push(cb);
		setTimeout(()=> {bullets.push(new CBullet(this.x, dy, this.row));}, 250);
		setTimeout(()=> {bullets.push(new CBullet(this.x, dy, this.row));}, 500);
		setTimeout(()=> {bullets.push(new CBullet(this.x, dy, this.row));}, 750);
	}
}

//CSeedBank只会被实例化一次
class CSeedBank {
	constructor() {
		this.sunValue=9900;//初始阳光值
		this.x=10;
		this.y=0;//图片左上角所在canvas中的坐标
		this.width = CSeedBank.img.width;//宽
		this.height = CSeedBank.img.height;//高
	}
	draw(ctx) {
		ctx.drawImage(CSeedBank.img, this.x, this.y);//绘图
		ctx.fillStyle = 'black';
		ctx.textAlign = 'center'
		ctx.font = 'bolder 17px Microsoft YaHei';
		ctx.fillText(this.sunValue, this.x + 40, this.y + 80);//绘阳光值
		for (var i=0, len=CSeedBank.cards.length; i<len; i++)//绘植物卡片
		{
			CSeedBank.cards[i].draw(ctx);//在改成清空画布之后，redraw与draw就没有区别了
		}
	}
	inArea(x, y) {
		return inArea(x, y, this);
	}
	updateValue(value)//更新重绘阳光值
	{
		// console.log(this.sunValue, value);
		if (this.sunValue+value > 9999) return ;//参考原版，保证阳光值不超过 1w
		this.sunValue+=value;
	}
}
CSeedBank.img=createImageFromName(all_images.seedBank);//类的静态成员，但在js里似乎它实际上是和类无关的变量
CSeedBank.cards=[];//也是类的静态成员

//CCar只会有5个实例化对象
class CCar {
	constructor(x, y, row) {
		this.row=row;
		this.x=x;
		this.y=y;
		this.moving=false;
		this.died=false;
	}
	draw(ctx) {//绘制车子并计算下一帧应该在的位置
		if (this.died) return ;//如果车辆已经启动过并且离开了草坪，就不再绘制
		ctx.drawImage(CCar.img, this.x, this.y);
		this.nextPosition();
	}
	nextPosition()
	{
		if (this.moving==false) return ;
		if (this.x >= 900) this.died = true;
		this.x+=10;
	}
	inArea(x, y)
	{
		return inArea(x, y, this);
	}
	start()//由僵尸触碰启动
	{
		if (this.moving==false)
		{
			this.moving=true;
			createAudio("sounds/lawnmower.ogg");
		}
	}
}
CCar.img = createImageFromName(all_images.lawnCar);//CCar会被实例化多次，但共用同一个img

class CCard {
	constructor(name, row, cooldown, sun_consume, cooltime=0) {//参数： 植物名字 所在行 冷却时间 阳光消耗 初始冷却时间
		this.name = name;//卡片对应的植物名字
		this.cooldown=cooldown;//卡片冷却时间
		this.sun_consume=sun_consume;//所需阳光值
		this.row=row;//第几个卡片槽
		this.x=88+row*51;
		this.y=10;//卡片绘制坐标
		this.height=68;
		this.width=this.height/CCard.img.height*CCard.img.width;//卡片宽高，需要进行缩放
		this.cooltime=cooltime;//剩余冷却时间
		this.timer=null;//冷却定时器
		this.isUsing=false;//是否卡片植物正在被使用，即准备放置
		this.plantImage=plants_image[name];//卡片上的植物的图片
	}
	isCooling()//植物正在冷却中
	{
		return this.cooltime!=0;
	}
	draw(ctx) {
		ctx.drawImage(CCard.img, this.x, this.y, this.width, this.height);//绘制卡片
		if (!this.isUsing)//卡片植物未使用时才在卡片内绘制植物
		{
			var width = this.plantImage.width * 37 / this.plantImage.height;
			ctx.drawImage(this.plantImage, this.x+8, this.y+15, width-5, 30);
		}
		ctx.font = '500 10px New Roman';
		ctx.textAlign = 'right';
		if (this.sun_consume > sb.sunValue) ctx.fillStyle="red";
		ctx.fillText(this.sun_consume, this.x+30, this.y+63);//绘制卡片的阳光值
		if (this.isCooling())//卡片正在冷却
		{
			ctx.font = '900 30px New Roman';
			// toGray(ctx, this.x, this.y, this.width, this.height);	//无法绘制灰度本地图片
			ctx.fillText(Math.ceil(this.cooltime), this.x+35, this.y+43);//在卡片上直接写剩余时间
			var height = this.cooltime/this.cooldown*this.height;
			
			ctx.globalAlpha = 0.4;
			ctx.fillStyle="black";
			ctx.fillRect(this.x, this.y, this.width, height);//在图片上加上半透明黑色，实现冷却进度条
			ctx.globalAlpha = 1.0;
		}
		if (this.sun_consume > sb.sunValue)//阳光值不够，则图片为黑色，表示不可用
		{
			ctx.globalAlpha = 0.7;
			ctx.fillStyle="black";
			ctx.fillRect(this.x, this.y, this.width, this.height);
			ctx.globalAlpha = 1.0;
		}
		ctx.fillStyle="black";
	}
	inArea(x, y) {
		return inArea(x, y, this);
	}
	startCooldown()//开始冷却
	{
		this.cooltime=this.cooldown;
		this.isUsing=false;
	}
}
CCard.img = createImageFromName(all_images.card);

class CShovel {
	constructor(x, y) {
		this.x=x;
		this.y=y;
		this.width=CShovel.img0.width;
		this.height=CShovel.img0.width;
		this.isUsing=false;//铲子是否正在被使用
	}
	draw(ctx) {
		// ctx.clearRect(this.x-5, this.y-5, this.width, this.height);
		ctx.drawImage(CShovel.img0, this.x, this.y, this.width, this.height);
		if (!this.isUsing)
		ctx.drawImage(CShovel.img1, this.x-5, this.y-5);
	}
	inArea(x, y) {
		return inArea(x, y, this);
	}
}
CShovel.img0 = createImageFromName(all_images.shovel0);
CShovel.img1 = createImageFromName(all_images.shovel1);
//草坪方块
class CBlock {
	constructor(row, col) {
		this.row = row;
		this.col = col;
		this.width = 80;
		this.height = 100;
		this.x = 110+this.col*this.width;
		this.y = 80+this.row*this.height;
		this.plant=null;//放置的植物
		this.isUsing=false;//是否已经放置植物
	}
	inArea(x, y) {
		return inArea(x, y, this);
	}
	draw(ctx)
	{	
		if (this.isUsing == true)
		{
			if (this.plant.died==true)//如果植物死亡，则不绘制，并回收该区域
			{
				this.isUsing = false;
				this.plant=null;
			}
			else this.plant.draw(ctx);
		}
	}
	removePlant()//移除植物
	{
		this.isUsing = false;
		this.plant=null;
	}
	setPlant(plant)//放置植物
	{
		this.isUsing=true;
		this.plant=plant;
	}
}





//这个类已经没用了，可以忽略掉
class CPlants {
	constructor(name, x, y, row, col, blood) {
		//this.idle=[];
		//this.attack=[];
		this.animation=[];
		this.name = name;
		this.animation.push(panimation[name]['idle']);
		this.animation.push(panimation[name]['attack']);
		this.type = 0;
		this.framei = 0;
		this.blood=blood;
		this.row=row;
		this.col=col;
		// if (this.name=="wallnut") this.blood=this.animation[this.type].length;
		// console.log(row, col);
		this.x=x;
		// if (this.name != "wallnut") this.x+=10;
		this.y=y;
		this.died=false;
		var scale = 60/plants_image[this.name].width;
		this.width= plants_image[this.name].width*scale;
		this.height=plants_image[this.name].height*scale;
		this.round=0;
		this.hurting=false;
		// this.bullet=[]
		//console.log(all_images.plants[name]['idle'].len, all_images.plants[name]['idle'].path);
	}
	draw(ctx)
	{
		// console.log(this.animation);
		// console.log(this.x, this.y, this.width, this.height, this.type, this.framei);
		// ctx.clearRect(this.x, this.y, this.width, this.height);
		// console.log("framei: ", this.framei);
		// if (this.framei==this.animation[this.type].length-1) alert("00");
		if (this.hurting)
		{
			// console.log("draw half parent");
			ctx.globalAlpha = 0.5;
			ctx.beginPath();
			ctx.drawImage(this.animation[this.type][this.framei], this.x, this.y, this.width, this.height);
			ctx.closePath();
			ctx.save();
			ctx.globalAlpha = 1;
			this.hurting=false;
		}
		else ctx.drawImage(this.animation[this.type][this.framei], this.x, this.y, this.width, this.height);
		this.nextFrame(ctx);
		
	}
	nextFrame(ctx)
	{
		// console.log("have error");
		if (this.name != "wallnut") this.framei++;
		if (this.changeType())
		{
			// this.type=!this.type;
			if (this.type==0) this.type=1;
			else this.type=0;
			this.framei=0;
			return ;
		}
		if (this.framei==this.animation[this.type].length)
		{
			this.round++;
			if (this.name == "cherrybomb")
			{
				if (this.type == 1)
				{
					this.died=true;
					for (var i=0, len=zombiesInfo.length; i<len; i++)
					{
						if (Math.abs(zombiesInfo[i].row-this.row)<=1 && Math.abs(zombiesInfo[i].col-this.col)<=1)
						{
							zombiesInfo[i].hurt(100);
						}
					}
				}
				else this.type=1, this.framei=0, createAudio('sounds/cherrybomb.ogg');
			}
			else if (this.name == "chomper")
			{
				// this.type=!this.type;
				// if (this.type==0) this.type=1;
				// else this.type=0;
				this.type=0;
				this.framei=0;
				for (var i=0, len=zombiesInfo.length; i<len; i++)
				{
					if (zombiesInfo[i].row == this.row && Math.abs(zombiesInfo[i].col-this.col)<=1)
					{
						zombiesInfo[i].blood=0;
						zombiesInfo[i].isDied=true;
						// zombiesInfo[i].clear(ctx);
						// zombiesInfo.splice(i, 1);
						break;
					}
				}
			}
			else this.framei=0, this.shoot();
		}
		
	}
	zombieInArea()
	{
		if (this.name=="peashooter" || this.name=="repeater" || this.name=="gatlingpea")
		{
			for (var i=0, len=zombiesInfo.length; i<len; i++)
			{
				if (zombiesInfo[i].row == this.row && zombiesInfo[i].x > this.x) return true;
			}
		}
		else if (this.name=="chomper")
		{
			for (var i=0, len=zombiesInfo.length; i<len; i++)
			{
				if (zombiesInfo[i].row == this.row && Math.abs(zombiesInfo[i].col-this.col)<=1) return true;
			}
		}
		return false;
	}
	changeType()
	{
		if (this.name=='chomper')
		{
			if (this.type==0 && this.zombieInArea()) return true;
			if (this.type==1 && !this.zombieInArea() && this.framei==this.animation[this.type].length) return true;
			return false;
		}
		if (this.name == "wallnut" || this.name=="sunflower" || this.name == "cherrybomb") return false;
		// if ( && this.framei==this.animation[this.type].length) return true;
		if (this.type==0 && this.zombieInArea()) return true;
		if (this.type==1 && !this.zombieInArea()) return true;
		return false;
	}
	needAttack()
	{
		// if (this.type == 1) return false;
		if (this.name == "wallnut" || this.name=="sunflower" || this.name == "cherrybomb") return false;		
		else return true;
	}
	shoot()
	{
		// return ;
		if (this.type == 1)
		{
			var cb=new CBullet(this.x, this.y, this.row);
			bullets.push(cb);
			if (this.name == "repeater") setTimeout(()=> {bullets.push(new CBullet(this.x, this.y, this.row));}, 850);
			// cb.draw(ctx);
		}
	}
	hurt(x=1)
	{
		this.blood-=x;
		this.hurting=true;
		// if (this.name=="wallnut") this.framei+=x;
		console.log("i am in attack, my blood: ", this.blood);
		if (this.blood<=0) this.died=true;
	}
}