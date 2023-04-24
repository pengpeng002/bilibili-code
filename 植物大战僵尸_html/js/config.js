
const all_audios = {
	mainTheme: "sounds/MainTheme.mp3",
	startGame: "sounds/evillaugh.ogg",
	inGame: "sounds/uraniwani.mp3",
	awooga: "sounds/awooga.ogg",
	gameOver: "sounds/losemusic.ogg",
	readySetPlant: "sounds/readysetplant.ogg",
	aBigWave: "sounds/finalwave.ogg",
}

const all_images = {
	firstBackground: "background.png",
	btn_startImage: "firstScene/button1.png",
	btn_startImage2: "firstScene/button1_light.png",
	mainBackground: "game-bg.jpg",
	seedBank: "SeedBank.png",
	lawnCar: "LawnCleaner.png",
	card: "SeedPacket_Larger.png",
	sunflower: "sunflower.png",
	cherrybomb: "cherrybomb.png",
	chomper: "chomper.png",
	gatlingpea: "gatlingpea.png",
	peashooter: "peashooter.png",
	repeater: "repeater.png",
	wallnut: "wallnut.png",
	shovel0: "ShovelBank.png",
	shovel1: "shovel1.png",
	button: "button.png",
	option: "options.png",
	option1: "options1.png",
	bullet: "bullet.png",
	sun: "sun.png",
	gameOver: "gameOver.png",
	aBigWave: "LargeWave.gif",
	startPlant: "readySetPlant/startPlant.png",
	startReady: "readySetPlant/startReady.png",
	startSet: "readySetPlant/startSet.png",
	plants: {//植物的动画
		sunflower: {  // 向日葵
			idle: {
				path: 'plants/sunflower/idle/idle_*.png',
				len: 18,
			},
			attack: {
				path: 'plants/sunflower/attack/attack_*.png',
				len: 0,
			}
		},
		peashooter: { // 豌豆射手
			idle: {
				path: 'plants/peashooter/idle/idle_*.png',
				len: 8,
			},
			attack: {
				path: 'plants/peashooter/attack/attack_*.png',
				len: 8,
			}
		},
		repeater: { // 双发射手
			idle: {
				path: 'plants/repeater/idle/idle_*.png',
				len: 15,
			},
			attack: {
				path: 'plants/repeater/attack/attack_*.png',
				len: 15,
			}
		},
		gatlingpea: { // 加特林射手
			idle: {
				path: 'plants/gatlingpea/idle/idle_*.png',
				len: 13,
			},
			attack: {
				path: 'plants/gatlingpea/attack/attack_*.png',
				len: 13,
			}
		},
		cherrybomb: { // 樱桃炸弹
			idle: {
				path: 'plants/cherrybomb/idle/idle_*.png',
				len: 7,
			},
			attack: {
				path: 'plants/cherrybomb/attack/attack_*.png',
				len: 5,
			}
		},
		wallnut: { // 坚果墙
			idle: { // 血量高时动画
				path: 'plants/wallnut/idle/idle_*.png',
				len: 42,
			},
			attack: {
				path: 'plants/wallnut/attack/attack_*.png',
				len: 0,
			}
		},
		chomper: { // 食人花
			idle: { // 站立动画
				path: 'plants/chomper/idle/idle_*.png',
				len: 13,
			},
			attack: { // 攻击动画
				path: 'plants/chomper/attack/attack_*.png',
				len: 14,
			}
		}
	},
	zombies: { // 僵尸的动画
		idle: { // 站立动画
			path: 'zombies/idle/idle_*.png',
			len: 31,
		},
		attack: { // 攻击动画
			path: 'zombies/attack/attack_*.png',
			len: 21,
		},
		dieboom: { // 被炸死亡动画
			path: 'zombies/dieboom/dieboom_*.png',
			len: 20,
		},
		dying: { // 濒死动画
			head: {
				path: 'zombies/dying/head/head_*.png',
				len: 12,
			},
			body: {
				path: 'zombies/dying/body/body_*.png',
				len: 18,
			}
		},
		die: { // 死亡动画
			path: 'zombies/die/die_*.png',
			len: 10,
		},
		cone: {
			path: 'zombies/cone/Zombie_cone*.png',
			len: 3,
		},
		bucket: {
			path: 'zombies/bucket/Zombie_bucket*.png',
			len: 3,
		}	
	}
}

var plants_name =["sunflower", "cherrybomb", "chomper", "gatlingpea", "peashooter", "repeater", "wallnut"]
var plants_image={};
var panimation={}//植物动画
var zanimation={}//僵尸动画

let canvas = document.getElementById("canvas");//canvas handle
let ctx = canvas.getContext("2d");
let context = document.getElementsByClassName("context")[0];//div context handle
let btn_game = document.getElementById('btn-startGame');//game start button handle
var audios = document.getElementById("audios");//background music
var btn_menu = document.getElementById("btn-menu");
var sunValue = 50;//init sun value
var sb=null;//seebbank 植物卡槽
var sh=null;//shovel 铲子
var cars=[];//车子
var blocks=[];//草坪方块
var mouseX, mouseY;//鼠标在canvas坐标
var picPath="";//应该没有用到
var tmpImage=null;//放置植物临时存放图片
var tmpType="";//放置植物临时存放名字
var blockX=0, blockY=0;//草坪方块位置
var mainInterval=null;
const fps=13;//每秒刷新次数
var zombiesInfo=[];//僵尸实例化对象
var zombieMax=50;//最大允许出现僵尸数量
var maintimer=null;//主定时器
var sunArray=[];//阳光实例化对象
var bullets=[];//子弹实例化对象
const ctxWidth=900;//canvas宽高
const ctxHeight=600;
var tmpZombies=[];//临时展示用僵尸实例化对象
var gameOverImg=null;
var gameOver=false;
var zombieRound=49;//第几波僵尸
// var roundStart=false;
var preWaveTime=Date.now();//本波僵尸已生成时间
var nextRoundNeedTime=0;//最多多久一定生成下一波
var nextRoundNeedBlood=0;//生成下一波僵尸所需血量比
var paBigWave=null;//一大波僵尸 图片
var bigWaveTimes=0;//一大波僵尸 图片显示帧数
var tmpZombieList=[];//本波所有僵尸