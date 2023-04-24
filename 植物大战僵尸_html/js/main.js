/*
	请从main开始阅读
*/

let canvas = document.getElementById("canvas").getContext("2d");//canvsa handle
let context = document.getElementsByClassName("context")[0];//div context handle
let btn_game = document.getElementById('btn-startGame');//game start button handle
var audios = document.getElementById("audios");//background music
var sunValue = 50;//init sun value

function sleep(time)
{
	return new Promise((resolve) => setTimeout(resolve, time));
}

function gameStart()
{
	audios.src="music/MainTheme.mp3";
	audios.loop = false;
}

function main()
{
	//set main theme in start scene
	
	audios.src="music/MainTheme.mp3";
	audios.autoplay = true;
	audios.loop = true;
	
	btn.onclick = gameStart();
}

main();










var mouseX=0, mouseY=0;
var drawPic = 0;
var undraw = 0;
var pic = document.createElement("img");
var pic2 = document.createElement("img");
pic.zIndex="99";
pic2.zIndex="99";
context.appendChild(pic);
context.appendChild(pic2);
var picPath = "";
var cp = []
for (var i=0; i<5; i++)
{
	cp[i] = []
	for (var j=0; j<9; j++)
	{
		cp[i][j] = 0;
	}
}

// function sleep(delay) {
	// var st = (new Date()).getTime();
	// while (1)
	// {
		// var ed = (new Date()).getTime();
		// if (ed-st >= delay) break; 
	// }
// }

function dynamic(i)
{
	context.style.backgroundPosition = "-"+i*5+"px";
}
async function run()
{
	for (var i=0; i<100; i++)
	{
		await sleep(10);
		dynamic(i);
	}
	await sleep(1000);
	for (var i=100; i>30; i--)
	{
		await sleep(10);
		dynamic(i);
	}
}

btn_game.onclick = function() {
	console.log("start game");
	btn_game.style.display = "none";
	context.style.background="url(images/game-bg.jpg)";
	// run();
	dynamic(30);
	var seedBank = document.createElement("img");
	seedBank.src="images/SeedBank.png";
	seedBank.style.left = "10px";
	seedBank.style.position = "relative";
	context.appendChild(seedBank);
	var car = []
	for (var i=0; i<5; i++)
	{	
		car[i] = document.createElement("img");
		car[i].src="images/LawnCleaner.png";
		car[i].style.left = "40px";
		car[i].style.top = 110+100*i+"px";
		car[i].style.position = "absolute";
		context.appendChild(car[i]);
	}
	var plants = []
	var cards  = []
	for (var i=0; i<6; i++)
	{	
		plants[i] = document.createElement("img");
		plants[i].src="images/SeedPacket_Larger.png";
		plants[i].style.left = 95+i*60+"px";
		plants[i].style.top = 10+"px";
		plants[i].style.height = "68px";
		plants[i].style.position = "absolute";
		plants[i].style.cursor = "pointer";
		plants[i].onclick = function() {drawPic=1; picPath="images/sunflower.png"}
		context.appendChild(plants[i]);
		
		var sunflower = document.createElement("img");
		sunflower.src = "images/sunflower.png";
		sunflower.style.height = "37px";
		sunflower.style.left = 103+i*60+"px";
		sunflower.style.top = "20px";
		sunflower.style.position = "absolute";
		sunflower.style.cursor = "pointer";
		sunflower.onclick = function() {drawPic=1; picPath="images/sunflower.png"}
		context.appendChild(sunflower);
		
		var sunText = document.createElement("div");
		sunText.style.left = 110+i*60+"px";
		sunText.style.top = 60+"px";
		// sunText.style.height = "68px";
		sunText.style.position = "absolute";
		sunText.innerHTML = "50"
		sunText.style.fontWeight = "900";
		sunText.style.fontSize = "10px";
		context.appendChild(sunText);
	}
	
	var sunValue = document.createElement("div");
	sunValue.style.left = 40+"px";
	sunValue.style.top = 60+"px";
	// sunText.style.height = "68px";
	sunValue.style.position = "absolute";
	sunValue.innerHTML = "50"
	sunValue.style.fontWeight = "900";
	sunValue.style.fontSize = "17px";
	context.appendChild(sunValue);
}

context.onmousemove = function(event) {
	let e = event || window.event;
	scrollX = document.documentElement.scrollLeft || document.body.scrollLeft;
	scrollY = document.documentElement.scrollTop || document.body.scrollTop;
	mouseX = e.pageX || e.clientX + scrollX;
	mouseY = e.pageY || e.clientY + scrollY;
	if (drawPic == 1)
	{
		pic.src = picPath;
		pic.style.left = e.pageX || e.clientX + scrollX;
		pic.style.top = e.pageY || e.clientY + scrollY;
		pic.style.display = "block";
		pic.style.position = "fixed";
		
		var cx=80, cy=100;
		var pp=4;
		pic2.src = picPath;
		var dx = parseInt((mouseX - 100) / cx) + 1;
		var dy = parseInt((mouseY - 90) / cy) + 1;
		console.log(dx, dy)
		if (dx >= 1 && dx<=9+pp && dy>=1 && dy<=5 && cp[dy][dx]==0)
		{
			pic2.style.left = (dx) * cx + 30;
			pic2.style.top = dy * cy;
			pic2.style.display = "block";
			pic2.style.position = "fixed";
			pic2.style.opacity = "0.5";
		}
		undraw = 1;
	}
}

context.onclick = function(event) {
	if (drawPic == 1 && undraw == 1)
	{
		drawPic = 0;
		undraw = 0;
		// pic2.style.opacity = "1";
		var picc = document.createElement("img");
		
		picc.style.left = pic2.style.left;
		picc.style.top = pic2.style.top;
		picc.src = pic2.src;
		picc.style.display = "block";
		picc.style.position = "fixed";
		context.appendChild(picc);
		pic.style.display = "none";
		pic2.style.display = "none";
		// cp[parseInt((mouseY - 90) / cy)][parseInt((mouseX - 100) / cx) + 1]=1;
	}
}