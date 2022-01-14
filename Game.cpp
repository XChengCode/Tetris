#include "Game.h"

Game::Game()
{
	Window_width = 1350;
	Window_height = 1000;
	imgBGno = 1;
	imgSkinNo = 1;
	imgSetNo = 1;
	ButtonState_Start = Start_Dark;
	ButtonState_Hold = Hold_Dark;
	
	Uint8 WindowStyle = sf::Style::Close | sf::Style::Titlebar;
	window.create(sf::VideoMode(Window_width, Window_height), L"Tetris by �̲�Դ", WindowStyle);
	sf::VideoMode mode = sf::VideoMode::getDesktopMode();//��ѯ��������� 
	Vector2i p = { 0,0 };
	p.x = (mode.width - Window_width) / 2;
	window.setPosition(p);
}

void Game::LoadMediaData()
{
	std::stringstream ss;
	ss << "data/images/bg" << imgSetNo << ".jpg";
	if (!tBackground.loadFromFile(ss.str()))//��������ͼƬ
	{
		std::cout << "BK image û���ҵ�" << std::endl;
	}
	ss.str("");//����ַ���
	ss << "data/images/tiles" << imgSkinNo << ".jpg";
	if (!tTiles.loadFromFile(ss.str()))
	{
		std::cout << "tiles.png û���ҵ�" << std::endl;
	}
	if (!tFrame.loadFromFile("data/images/frame.png"))
	{
		std::cout << "frame.png û���ҵ�" << std::endl;
	}
	if (!tCover.loadFromFile("data/images/cover.png"))
	{
		std::cout << "cover.png û���ҵ�" << std::endl;
	}
	if (!tGameOver.loadFromFile("data/images/end.png"))
	{
		std::cout << "end.png û���ҵ�" << std::endl;
	}

	if (!tButtons.loadFromFile("data/images/button.png"))
		std::cout << "button.png û���ҵ�" << std::endl;
	if (!tSwitcher.loadFromFile("data/images/bgSwitch.png"))
		std::cout << "bgSwap.png û���ҵ�" << std::endl;


	sBackground.setTexture(tBackground); //���þ�����������
	 //sTiles.setTexture(tTiles);//��Tetris ������Ʒ��飬������Ҹ��Ի��Լ��ķ���
	sFrame.setTexture(tFrame);
	sCover.setTexture(tCover);
	sGameOver.setTexture(tGameOver);
	sButtons.setTexture(tButtons);
	sSwitcher.setTexture(tSwitcher);
	sSwitcher.setOrigin(sSwitcher.getLocalBounds().width / 2.0, sSwitcher.getLocalBounds().height / 2.0);
	if (!font.loadFromFile("data/Fonts/�����ֺ��μ���.ttf"))
		std::cout << "����û���ҵ�" << std::endl;
	text.setFont(font);
}


void Game::gameInitial()
{
	window.setFramerateLimit(15); //ÿ������Ŀ��֡��
	LoadMediaData(); //�ȼ����ز�
	isGameBegin = false;
	isGameHold = false;
	player1.gameOver = false;
	player2.gameOver = false;

	player1.role = rolePLAYER1; //����Tetris ����Ϊplayer1
	player2.role = rolePLAYER2; //����Tetris ����Ϊplayer1
	player1.Initial(&tTiles); //��������زĴ���Tetris ������Tetris ������Ʒ���
	player2.Initial(&tTiles); //��������زĴ���Tetris ������Tetris ������Ʒ���
}





void Game::gameInput()
{
	sf::Event event;
	window.setKeyRepeatEnabled(false);
	while (window.pollEvent(event))
	{
		if (event.type == sf::Event::Closed)
		{
			window.close();
			gameQuit = true;
		}
		if (event.type == sf::Event::EventType::KeyReleased && event.key.code == sf::Keyboard::Escape)
		{
			window.close();
			gameQuit = true;
		}
		if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left)
		{
			if (ButtonRectStart.contains(event.mouseButton.x, event.mouseButton.y))
				if (isGameBegin == false)
				{
					isGameBegin = true;
					player1.animationFlag = false;//��ʼ��ʱ�򣬶�����״̬
					player2.animationFlag = false;//��ʼ��ʱ�򣬶�����״̬
					ButtonState_Start = Close_Light;
				}
				else
				{
					isGameBegin = false;
					gameInitial();//���³�ʼ��
					ButtonState_Start = Start_Light;
				}
			if (ButtonRectHold.contains(event.mouseButton.x, event.mouseButton.y))
				if (isGameHold == false)
				{
					isGameHold = true;
					ButtonState_Hold = Continue_Light;
				}
				else
				{
					isGameHold = false;
					ButtonState_Hold = Hold_Light;
				}

			if (ButtonRectLeft.contains(event.mouseButton.x, event.mouseButton.y))
			{
				imgSetNo--;
				if (imgSetNo < 1)//С��1 ��ʱ���Ƥ��4 ��ʼ��ѭ��
					imgSetNo = 4;//�����ֻ�Ƥ��ͼ
				std:: cout << imgSetNo;
				LoadMediaData();
			}
			if (ButtonRectRight.contains(event.mouseButton.x, event.mouseButton.y))
			{
				imgSetNo++;
				if (imgSetNo > 4)//����Ƥ��ͼ������ʱ��
					imgSetNo = 1;//�����ֻ�Ƥ��ͼ
				std::cout << imgSetNo;
				LoadMediaData();
			}

		}
		if (event.type == sf::Event::MouseMoved)
		{
			if (ButtonRectStart.contains(event.mouseMove.x, event.mouseMove.y))
				if (isGameBegin == false)
					ButtonState_Start = Start_Light;
				else
					ButtonState_Start = Close_Light;
			else
				if (isGameBegin == false)
					ButtonState_Start = Start_Dark;
				else
					ButtonState_Start = Close_Dark;
			if (ButtonRectHold.contains(event.mouseMove.x, event.mouseMove.y))
				if (isGameHold == false)
					ButtonState_Hold = Hold_Light;
				else
					ButtonState_Hold = Continue_Light;
			else
				if (isGameHold == false)
					ButtonState_Hold = Hold_Dark;
				else
					ButtonState_Hold = Continue_Dark;
		}
		player1.Input(event);
		player2.Input(event);

	}

}


void Game::gameLogic()
{
	if (isGameHold == true)return;
	float time = clock.getElapsedTime().asSeconds();
	clock.restart();
	player1.timer += time;
	player2.timer += time;
	player1.Logic();
	player2.Logic();
}

void Game::DrawButton()
{
	int ButtonWidth, ButtonHeight;

	ButtonWidth = 110;
	ButtonHeight = sButtons.getLocalBounds().height;
	//ButtonRectStart
	sButtons.setTextureRect(IntRect(ButtonState_Start * ButtonWidth, 0, ButtonWidth, ButtonHeight));//��ȡ��ť����������
	sButtons.setPosition(B_START_CORNER_X, B_START_CORNER_Y);
	ButtonRectStart.left = B_START_CORNER_X;
	ButtonRectStart.top = B_START_CORNER_Y;
	ButtonRectStart.width = ButtonWidth;
	ButtonRectStart.height = ButtonHeight;
	window.draw(sButtons);
	//ButtonRectHold
	sButtons.setTextureRect(IntRect(ButtonState_Hold * ButtonWidth, 0, ButtonWidth, ButtonHeight));//��ȡ��ť����������
	sButtons.setPosition(B_HOLD_CORNER_X, B_HOLD_CORNER_Y);
	ButtonRectHold.left = B_HOLD_CORNER_X;
	ButtonRectHold.top = B_HOLD_CORNER_Y;
	ButtonRectHold.width = ButtonWidth;
	ButtonRectHold.height = ButtonHeight;
	window.draw(sButtons);
	//�����ز��л�
	ButtonWidth = sSwitcher.getLocalBounds().width;
	ButtonHeight = sSwitcher.getLocalBounds().height;
	//ButtonRectLeft
	sSwitcher.setPosition(B_LEFT_CORNER_X, B_LEFT_CORNER_Y);
	ButtonRectLeft.left = B_LEFT_CORNER_X - ButtonWidth / 2;
	ButtonRectLeft.top = B_LEFT_CORNER_Y - ButtonHeight / 2;
	ButtonRectLeft.width = ButtonWidth;
	ButtonRectLeft.height = ButtonHeight;
	window.draw(sSwitcher);
	//ButtonRectRight
	sSwitcher.setPosition(B_RIGHT_CORNER_X, B_RIGHT_CORNER_Y);
	ButtonRectRight.left = B_RIGHT_CORNER_X - ButtonWidth/2;
	ButtonRectRight.top = B_RIGHT_CORNER_Y - ButtonHeight / 2;
	ButtonRectRight.width = ButtonWidth;
	ButtonRectRight.height = ButtonHeight;
	sSwitcher.rotate(180);//ֻ��һ����ͷ�زģ���ȻҪ���Լ�����������һ��^_^
	window.draw(sSwitcher);
	sSwitcher.rotate(180);//��ԭ�Ƕ�
}


void Game::DrawResults()
{
	int ButtonWidth, ButtonHeight;
	ButtonWidth = 250;// sGameOver.getLocalBounds().width / 2; 
	ButtonHeight = sGameOver.getLocalBounds().height;

	if (player1.gameOver || player2.gameOver)
	{
		sGameOver.setTextureRect(IntRect(player1.gameOver * ButtonWidth, 0, ButtonWidth, ButtonHeight));//��ȡ��ť���������� 
		sGameOver.setPosition(P1_STAGE_CORNER_X + GRIDSIZE * 1.5, 0); //���ð�ť��λ������ 
		window.draw(sGameOver);

		sGameOver.setTextureRect(IntRect(player2.gameOver * ButtonWidth, 0, ButtonWidth, ButtonHeight));//��ȡ��ť���������� 
		sGameOver.setPosition(P2_STAGE_CORNER_X + GRIDSIZE * 1.5, 0); //���ð�ť��λ������ 
		window.draw(sGameOver);
	}
}


void Game::TextOut()
{
	int initialX, initialY;
	int CharacterSize = 48;
	text.setCharacterSize(CharacterSize);
	text.setFillColor(Color(255, 0, 0, 255));//��ɫ���� 
	text.setStyle(Text::Bold); 
	text.setPosition(P1_SCORE_CORNER_X, P1_SCORE_CORNER_Y);
	std::stringstream ss;

	ss << player1.score;
	text.setString(ss.str()); window.draw(text);

	text.setPosition(P2_SCORE_CORNER_X - CharacterSize * 3, P2_SCORE_CORNER_Y);

	ss.str("");
	ss << player2.score;
	text.setString(ss.str()); window.draw(text);
	CharacterSize = 30;
	text.setCharacterSize(CharacterSize);
	text.setFillColor(Color(255, 255, 255, 255));
	text.setStyle(Text::Regular || Text::Italic);
	initialY = P1_STAGE_CORNER_Y + STAGE_HEIGHT * GRIDSIZE;
	text.setPosition(P1_STAGE_CORNER_X, initialY);
	text.setString(L"�� ���ͣ���������������"); window.draw(text);
	initialY += CharacterSize;
	text.setPosition(P1_STAGE_CORNER_X, initialY);

	CharacterSize = 24;
	text.setCharacterSize(CharacterSize);
	text.setFillColor(Color(255, 255, 255, 255));
	text.setStyle(Text::Bold); // |Text::Underlined 
	initialY = INFO_CORNER_Y;
	text.setPosition(INFO_CORNER_X, initialY);
	text.setString(L"���1:"); window.draw(text);
	initialY += CharacterSize;
	text.setPosition(INFO_CORNER_X, initialY);
	text.setString(L" �������WASD"); window.draw(text);
	initialY += CharacterSize;
	text.setPosition(INFO_CORNER_X, initialY);
	text.setString(L" �ٽ��� Space��"); window.draw(text);
	initialY += CharacterSize;
	text.setPosition(INFO_CORNER_X, initialY);
	text.setString(L" ������ ��Ctrl��"); window.draw(text);
	initialY += CharacterSize * 1.5;
	text.setPosition(INFO_CORNER_X, initialY);
	text.setString(L"���2:"); window.draw(text);
	initialY += CharacterSize;
	text.setPosition(INFO_CORNER_X, initialY);
	text.setString(L" ���������������"); window.draw(text);
	initialY += CharacterSize;
	text.setPosition(INFO_CORNER_X, initialY);
	text.setString(L" �ٽ��� Enter��"); window.draw(text);
	initialY += CharacterSize;
	text.setPosition(INFO_CORNER_X, initialY);
	text.setString(L" ������ ��Ctrl��"); window.draw(text);
	initialY += 2 * CharacterSize;
	text.setPosition(INFO_CORNER_X, initialY);
	text.setString(L"�Դ��ײ����幦��"); window.draw(text);
	initialY += CharacterSize;
	text.setPosition(INFO_CORNER_X, initialY);
	text.setString(L"�˳��� Esc��"); window.draw(text);
}



void Game::gameDraw()
{
	window.clear(); //����
	//���Ʊ���
	sBackground.setPosition(0, 0);
	window.draw(sBackground);
	window.draw(sFrame);
	if (isGameBegin == true) {
		player1.Draw(&window);
		player2.Draw(&window);
	}
	

	sCover.setPosition(P1_STAGE_CORNER_X, P1_STAGE_CORNER_Y);
	window.draw(sCover);
	sCover.setPosition(P2_STAGE_CORNER_X, P2_STAGE_CORNER_Y);
	window.draw(sCover);

	DrawButton();
	TextOut(); 
	DrawResults();
	window.display(); //����ʾ�����������ݣ���ʾ����Ļ�ϡ�SFML ���õ���˫�������
}





void Game::gameRun()
{
	do {
		gameInitial();

		while (window.isOpen() && gameOver == false)
		{
			gameInput();

			if (isGameBegin == true)
				gameLogic();
			else
				gameInitial();
			gameDraw();
		}
	} while (!gameQuit);
}