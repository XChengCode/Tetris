#include "Tetris.h"

int Tetris::holdcolorNum = 0, Tetris::holdShapeNum = 0;
Vector2i Tetris::holdSquare[4] = { { 0,0 },{ 0,0 },{ 0,0 },{ 0,0 } };



Tetris::Tetris()
{
	dx = 0; //X 方向偏移量
	rotate = false; //是否旋转
	colorNum = 1; //色块的颜色
	timer = 0;
	delay = DALAYVALUE; //下落的速度，
	hold = false;
	b7Int = 0;
	hardDrop = false; //是否硬降
	score = 0;
}

void Tetris::Initial(Texture* tex)
{
	tTiles = tex;
	score = 0;
	holdSquareCornPoint = { HOLD_CORNER_X, HOLD_CORNER_Y };
	holdShapeNum = -1;//游戏初始时候，hold 区的图形设置一个异常值


	animationFlag = true; //动画开启,由游戏是否开始决定
	animationCtrlValue = 1.0;

	if (role == rolePLAYER1)
	{
		mCornPoint = { P1_STAGE_CORNER_X, P1_STAGE_CORNER_Y };
		nextSquareCornPoint = { P1_NXET_CORNER_X, P1_NXET_CORNER_Y };
	}
	if (role == rolePLAYER2)
	{
		mCornPoint = { P2_STAGE_CORNER_X, P2_STAGE_CORNER_Y };
		nextSquareCornPoint = { P2_NXET_CORNER_X, P2_NXET_CORNER_Y };
	}
	sTiles.setTexture(*tTiles);
	//初始化方块图形
	colorNum = 1 + rand() % 7;
	currentShapeNum = rand() % 7;

	//更新下个方块图形
	nextcolorNum = 1 + rand() % 7;
	nextShapeNum = Bag7();

	for (int i = 0; i < STAGE_WIDTH; i++)
	{
		for (int j = 0; j < STAGE_HEIGHT; j++)
			Field[j][i] = 0;
	}

	for (int i = 0; i < 4; i++)
	{
		currentSquare[i].x = Figures[currentShapeNum][i] % 2 + STAGE_WIDTH / 2;
		currentSquare[i].y = Figures[currentShapeNum][i] / 2;
		nextSquare[i].x = Figures[nextShapeNum][i] % 2;
		nextSquare[i].y = Figures[nextShapeNum][i] / 2;

		animationRow[i] = -1;//要播放动画的行数, ‐1 为异常值，表待定的意思
	}

}







void Tetris::traditionLogic()
{ 
	//hold 方块图形
	if (hold)
	{
		holdFunc();
		hold = false;
	}

	//// <‐ 水平Move ‐> ///
	xMove();

	//////Rotate//////
	if (rotate)
	{
		rotateFunc();
		rotate = false;
	}

	slowLoading();


	///////Tick 下落//////
	if (timer > delay)
	{
		yMove();

		timer = 0;
	}

	shadow();

	if (hardDrop)
	{
		hardDropFunc();
		hardDrop = false;
	}

	///////check lines//////////
	checkLine();
}

void Tetris::Logic()
{
	if (!animationFlag)
		traditionLogic();
	//如果有发生新方块生成请求，则先判定是否有动画播放，再new 新的方块
	if (newShapeFlag)
	{
		if (animationFlag == false)
		{
			///////check lines//////////
			checkLine();
			if (animationFlag == false)
				newShapeFunc();//落地应该就要生成新的方块;图形生成函数里面再更新Flag 的状态
			isWin();
		}
		else
		{
			animationCtrlValue -= 0.1;
			if (animationCtrlValue < 0)
			{
				animationFlag = false;
				animationCtrlValue = 1.0;

				clearLine();
				newShapeFunc();
			}
		}
	}
}


void Tetris::yMove() {
	for (int i = 0; i < 4; i++)
	{
		tempSquare[i] = currentSquare[i];
		currentSquare[i].y += 1;
	}

	
	if (!hitTest())//如果撞上了
	{
		for (int i = 0; i < 4; i++)
			Field[tempSquare[i].y][tempSquare[i].x] = colorNum;
		newShapeFunc();//由于动画管理的需要，此处进行了代码封装，并移到外部，等动画结束后再new 新的图形。
		newShapeFlag = true; //撞上了就要有新的图形生成
	}
}

void Tetris::hardDropFunc()
{
	for (int i = 0; i < 4; i++)
	{
		currentSquare[i] = shadowSquare[i];
	}
}

void Tetris::slowLoading()//底部缓冲
{
	for (int i = 0; i < 4; i++)
	{
		tempSquare[i] = currentSquare[i];
		currentSquare[i].y += 1;
	}

	if (!hitTest())//如果撞上了
	{
		delay = DALAYVALUE * 2;
	}
	for (int i = 0; i < 4; i++)
		currentSquare[i] = tempSquare[i];
}


void  Tetris::rotateFunc() {
	int originalHeight = currentSquare[0].y;
	for (int j = 0; j < 4; j++)//wall kick 中心偏移
	{
		Vector2i p = currentSquare[j]; //设置旋转中心点
		for (int i = 0; i < 4; i++)
		{//顺时针旋转90 度
			int x = currentSquare[i].y - p.y;
			int y = currentSquare[i].x - p.x;
			currentSquare[i].x = p.x - x;//新坐标X=中心点坐标‐新的X 方向差值
			currentSquare[i].y = p.y + y;//新坐标Y=中心点坐标+新的Y 方向差值
		}
		if (hitTest()) //如果没撞上了
		{
			int detaY = 0;
			detaY = currentSquare[0].y - originalHeight;//新重心与原重心高度差
			if (detaY != 0)
				for (int i = 0; i < 4; i++)
					currentSquare[i].y -= detaY;//高度差修正
			if (!hitTest())//如果撞上了
				for (int i = 0; i < 4; i++)
					currentSquare[i] = tempSquare[i];
			else
				break; // 结束循环
		}
		else
		{
			if(!hitTest())//如果撞上了
				for (int i = 0; i < 4; i++)
					currentSquare[i] = tempSquare[i];
		}
	}
}


void Tetris::xMove() {

	for (int i = 0; i < 4; i++)
	{
		tempSquare[i] = currentSquare[i];
		currentSquare[i].x += dx;
	}
	if (!hitTest()) //如果撞上了
		for (int i = 0; i < 4; i++)
			currentSquare[i] = tempSquare[i];//到左右的边界，不能移出边界
}

void Tetris::clearLine()
{
	int k = STAGE_HEIGHT - 1;
	for (int i = STAGE_HEIGHT - 1; i > 0; i--)
	{
		int xCount = 0;
		for (int j = 0; j < STAGE_WIDTH; j++)
		{
			if (Field[i][j])
				xCount++;
			Field[k][j] = Field[i][j];//下降
		}
		if (xCount < STAGE_WIDTH)
			k--;
	}
}


void Tetris::checkLine()
{
	int k = STAGE_HEIGHT - 1;
	int yCount = 0;
	for (int i = STAGE_HEIGHT - 1; i > 0; i--)
	{
		int xCount = 0;
		for (int j = 0; j < STAGE_WIDTH; j++)
		{
			if (Field[i][j])
				xCount++;
			//Field[k][j] = Field[i][j];//下降
		}
		if (xCount < STAGE_WIDTH)
			k--;
		else
		{
			animationRow[yCount] = i;//要播放动画的行数
			yCount++;
			animationFlag = true;
		}
	}
	switch (yCount)
	{
	case 1:
		score += 10;
		break;
	case 2:
		score += 30;
		break;
	case 3:
		score += 60;
		break;
	case 4:
		score += 100;
		break;
	}
	//得分后的动画策略
}

void Tetris::newShapeFunc()
{
	//取下个方块图形
	colorNum = nextcolorNum;
	currentShapeNum = nextShapeNum;
	//更新下个方块图形
	nextcolorNum = 1 + rand() % 7;
	nextShapeNum = Bag7();

	for (int i = 0; i < 4; i++)
	{
		currentSquare[i] = nextSquare[i];//当前块更新
		currentSquare[i].x = currentSquare[i].x + STAGE_WIDTH / 2;
		nextSquare[i].x = Figures[nextShapeNum][i] % 2;
		nextSquare[i].y = Figures[nextShapeNum][i] / 2;
	}

	shadow();

	newShapeFlag = false;//这样下次才能再进来

	for (int i = 0; i < 4; i++)
		animationRow[i] = -1;//本来应该动画播放完之后就清零，但容易造成框架结构混乱；这里‐1 为异常值，表待定的意思
}

bool Tetris::hitTest()
{
	for (int i = 0; i < 4; i++)
		if (currentSquare[i].x < 0 || currentSquare[i].x >= STAGE_WIDTH || currentSquare[i].y >= STAGE_HEIGHT)
			return false;
		else if (Field[currentSquare[i].y][currentSquare[i].x])
			return false;

	return true;
}

int Tetris::Bag7()
{
	int num;
	srand(time(NULL));
	num = rand() % 7;
	for (int i = 0; i < b7Int; i++)
	{
		if (b7array[i] == num)
		{
			i = -1;//i++后归零，数组重新遍历
			num = rand() % 7;
		}
	}
	b7array[b7Int] = num;

	b7Int++;
	if (b7Int == 7)
	{
		b7Int = 0;
		for (int i = 0; i < 7; i++)
			b7array[i] = 0;
	}
	return num;
}

void Tetris::holdFunc()
{
	Vector2i backUpSquare[4];
	tempcolorNum = holdcolorNum;
	tempShapeNum = holdShapeNum;

	holdcolorNum = colorNum;
	holdShapeNum = currentShapeNum;

	for (int i = 0; i < 4; i++)
	{
		holdSquare[i].x = Figures[holdShapeNum][i] % 2;
		holdSquare[i].y = Figures[holdShapeNum][i] / 2;
		tempSquare[i].x = Figures[tempShapeNum][i] % 2;
		tempSquare[i].y = Figures[tempShapeNum][i] / 2;
		backUpSquare[i] = currentSquare[i];
	}

	if (tempShapeNum == -1)//hold 区图形的异常值表示hold 区为空的状态，所以要从Next 区取值
	{
		newShapeFunc();
	}
	else//当前图形取原来hold 图形的值，既发生交换
	{
		colorNum = tempcolorNum;
		currentShapeNum = tempShapeNum;

		//从hold 区置换过来的方块图形，在舞台上坐标的计算。
		int minCurrentX = currentSquare[0].x, minCurrentY = currentSquare[0].y, minTempX = tempSquare[0].x, minTempY = tempSquare[0].y;

		int dx, dy;

		for (int i = 1; i < 4; i++)
		{
			if (currentSquare[i].x < minCurrentX)
				minCurrentX = currentSquare[i].x;

			if (currentSquare[i].y < minCurrentY)
				minCurrentY = currentSquare[i].y;

			if (tempSquare[i].x < minTempX)
				minTempX = tempSquare[i].x;

			if (tempSquare[i].y < minTempY)
				minTempY = tempSquare[i].y;
		}
		dx = minCurrentX - minTempX;
		dy = minCurrentY - minTempY;

		for (int i = 0; i < 4; i++)
		{
			currentSquare[i].x = tempSquare[i].x + dx;
			currentSquare[i].y = tempSquare[i].y + dy;
			holdSquare[i].x = Figures[holdShapeNum][i] % 2;
			holdSquare[i].y = Figures[holdShapeNum][i] / 2;
		}
	}
	if (!hitTest()) //如果撞上了
	{
		holdcolorNum = tempcolorNum;
		holdShapeNum = tempShapeNum;

		for (int i = 0; i < 4; i++)
		{
			currentSquare[i] = backUpSquare[i];//
		}
	}
}
void Tetris::animationFunc(int i, sf::RenderWindow* window)
{
	Vector2f p;
	sTiles.scale(animationCtrlValue, animationCtrlValue);
	p = sTiles.getOrigin();
	sTiles.setOrigin(GRIDSIZE / 2, GRIDSIZE / 2);
	sTiles.rotate(360 * animationCtrlValue);
	for (int j = 0; j < STAGE_WIDTH; j++)
	{
		sTiles.setTextureRect(IntRect(Field[i][j] * GRIDSIZE, 0, GRIDSIZE, GRIDSIZE));
		sTiles.setPosition(j * GRIDSIZE, i * GRIDSIZE);
		sTiles.move(mCornPoint.x + GRIDSIZE / 2, mCornPoint.y + GRIDSIZE / 2); //offset
		//让方块旋转并缩小
		window->draw(sTiles);
	}
	sTiles.scale(1.0 / animationCtrlValue, 1.0 / animationCtrlValue);
	sTiles.rotate(-360 * animationCtrlValue);
	sTiles.setOrigin(p);
}

void Tetris::Input(sf::Event event)
{
	if (role == rolePLAYER1)//玩家1 的按键响应
	{
		if (event.type == Event::KeyPressed)
		{
			if (event.key.code == Keyboard::W)
				if (currentShapeNum != shapeO)
					rotate = true;
			if (event.key.code == Keyboard::A)
				dx = -1;
			else if (event.key.code == Keyboard::D)
				dx = 1;
			if (event.key.code == Keyboard::S)
				delay = 0.05;
		}
		if (event.type == Event::KeyReleased)
		{
			if (event.key.code == Keyboard::LControl)
				hold = true;
			if (event.key.code == Keyboard::A || event.key.code == Keyboard::D)
				dx = 0;
			if (event.key.code == Keyboard::S)
				delay = 0.3;
			if (event.key.code == Keyboard::Space)
				hardDrop = true;
		}
	}

	if (role == rolePLAYER2)//玩家2 的按键响应
	{
		if (event.type == Event::KeyPressed)
		{
			if (event.key.code == Keyboard::Up)
				if (currentShapeNum != shapeO)
					rotate = true;
			if (event.key.code == Keyboard::Left)
				dx = -1;
			else if (event.key.code == Keyboard::Right)
				dx = 1;
			if (event.key.code == Keyboard::Down)
				delay = 0.05;
		}
		if (event.type == Event::KeyReleased)
		{
			if (event.key.code == Keyboard::RControl)
				hold = true;
			if (event.key.code == Keyboard::Left || event.key.code == Keyboard::Right)
				dx = 0;
			if (event.key.code == Keyboard::Down)
				delay = 0.3;
			if (event.key.code == Keyboard::Enter)
				hardDrop = true;
		}
	}
}


void Tetris::shadow()
{
	for (int i = 0; i < 4; i++)
	{
		shadowSquare[i] = currentSquare[i];
	}
	for (int j = 0; j < STAGE_HEIGHT; j++) //一直下落
	{
		for (int i = 0; i < 4; i++)
		{
			currentSquare[i].y += 1;
		}
		if (!hitTest()) //如果撞上了
		{
			for (int i = 0; i < 4; i++) //到底了，返回上一次位置
			{
				currentSquare[i].y -= 1;
			}
			break;
		}
	}
	for (int i = 0; i < 4; i++)
	{
		tempSquare[i] = currentSquare[i];
		currentSquare[i] = shadowSquare[i];
		shadowSquare[i] = tempSquare[i];
	}
}

void Tetris::Draw(sf::RenderWindow* window)
{


	if (animationFlag == false)//动画管控
	{
		//绘制Shadow 的方块
		for (int i = 0; i < 4; i++)
		{
			sTiles.setTextureRect(IntRect(colorNum * GRIDSIZE, 0, GRIDSIZE, GRIDSIZE));
			sTiles.setPosition(shadowSquare[i].x * GRIDSIZE, shadowSquare[i].y * GRIDSIZE);
			sTiles.setColor(Color(50, 50, 50, 255)); //设置阴影的颜色
			sTiles.move(mCornPoint.x, mCornPoint.y); //offset
			window->draw(sTiles);
			sTiles.setColor(Color(255, 255, 255, 255));//颜色改回原色
		}
		//绘制活动的方块
		for (int i = 0; i < 4; i++)
		{
			sTiles.setTextureRect(IntRect(colorNum * GRIDSIZE, 0, GRIDSIZE, GRIDSIZE));
			sTiles.setPosition(currentSquare[i].x * GRIDSIZE, currentSquare[i].y * GRIDSIZE);
			sTiles.move(mCornPoint.x, mCornPoint.y); //offset
			window->draw(sTiles);
		}

	}

	//绘制固定的方块
	for (int i = 0; i < STAGE_HEIGHT; i++)
		for (int j = 0; j < STAGE_WIDTH; j++)
		{
			if (Field[i][j] == 0)
				continue;
			sTiles.setTextureRect(IntRect(Field[i][j] * GRIDSIZE, 0, GRIDSIZE, GRIDSIZE));
			sTiles.setPosition(j * GRIDSIZE, i * GRIDSIZE);
			sTiles.move(mCornPoint.x, mCornPoint.y); //offset
			window->draw(sTiles);
		}


	for (int i = 0; i < STAGE_HEIGHT; i++)
		if (i == animationRow[0] || i == animationRow[1] || i == animationRow[2] || i == animationRow[3]) {
		
			animationFunc(i, window);//遇到动画行的时候，执行特效动画
		}
			
		else
			for (int j = 0; j < STAGE_WIDTH; j++)
			{
				if (Field[i][j] == 0)
					continue;
				sTiles.setTextureRect(IntRect(Field[i][j] * GRIDSIZE, 0, GRIDSIZE, GRIDSIZE));
				sTiles.setPosition(j * GRIDSIZE, i * GRIDSIZE);
				sTiles.move(mCornPoint.x, mCornPoint.y); //offset
				//animationFlag = false;
				window->draw(sTiles);
			}


	//绘制Next 区的方块
	for (int i = 0; i < 4; i++)
	{
		sTiles.setTextureRect(IntRect(nextcolorNum * GRIDSIZE, 0, GRIDSIZE, GRIDSIZE));
		sTiles.setPosition(nextSquare[i].x * GRIDSIZE, nextSquare[i].y * GRIDSIZE);
		sTiles.move(nextSquareCornPoint.x, nextSquareCornPoint.y); //offset
		window->draw(sTiles);
	}
	//绘制Hold 区的方块
	if (holdShapeNum > -1)//hold 区图形正常时候进行绘制
		for (int i = 0; i < 4; i++)
		{
			sTiles.setTextureRect(IntRect(holdcolorNum * GRIDSIZE, 0, GRIDSIZE, GRIDSIZE));
			sTiles.setPosition(holdSquare[i].x * GRIDSIZE, holdSquare[i].y * GRIDSIZE);
			sTiles.move(holdSquareCornPoint.x, holdSquareCornPoint.y); //offset
			window->draw(sTiles);
		}
}

void Tetris::isWin() 
{  //原本应该用碰撞检测来做结束判断，但此处，两方块位置有一不为零，游戏即结束 
   //if(!hitTest()) 
	if (Field[2][5] || Field[2][6]) 
		gameOver = true; 
}

