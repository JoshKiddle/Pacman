
#include "Pacman.h"

#include <sstream>

#include <ctime>

#include <cstdlib>





// Constructor Method
Pacman::Pacman(int argc, char* argv[]) : Game(argc, argv), _cPacmanSpeed(0.1f), _cPacmanFrameTime(250), _cMunchieFrameTime(500)
{
	// Local Variables
	int i;
	
	srand(time(NULL));
	

	// Initialise Munchies
	for (i = 0; i < MUNCHIECOUNT; i++)
	{
		_munchies[i] = new Enemy();
		_munchies[i]->currentFrameTime = 0;
		_munchies[i]->frame = rand() % 1;
		_munchies[i]->frameTime = rand() % 500 + 50;
	}
	
	// Initialise ghost
	_ghosts[0] = new MovingEnemy();
	_ghosts[0]->direction = 0;
	_ghosts[0]->speed = 0.2f;

	// Initialise member variables
	_pacman = new Player();
	_cherry = new Enemy();

	_pacman->dead = false;
	_start = false;
	_paused = false;
	_pacman->currentFrameTime = 0;
	_pacman->direction = 0;
	_pacman->frame = 0;
	_pacman->speedMultiplier = 1.0f;
	_cherry->currentFrameTime = 0;
	_cherry->frame = 0;
	_cherry->frameTime = 0;


	//Initialise important Game aspects
	Graphics::Initialise(argc, argv, this, 1024, 768, false, 25, 25, "Pacman", 60);
	Input::Initialise();

	// Start the Game Loop - This calls Update and Draw in game loop
	Graphics::StartGameLoop();
}

// Destructor Method
Pacman::~Pacman()
{
	// Clean up the texture
	delete _munchies[0]->texture;
	delete _munchies[0]->texture2;

	for (int i = 0; i < MUNCHIECOUNT; i++)
	{
		delete _munchies[i]->sourceRect;
		delete _munchies[i];
	}

	// Clean up the Pacman structure pointer
	delete _pacman;

	// Clean up the Munchie structure pointer

	// Clean up the Cherry structure pointer
	delete _cherry;

	// Clean up the Ghosts structure pointer
	delete _ghosts[0];

}

void Pacman::LoadContent()
{
	// Load Pacman
	_pacman->texture = new Texture2D();
	_pacman->texture->Load("Textures/Pacman.tga", false);
	_pacman->position = new Vector2(350.0f, 350.0f);
	_pacman->sourceRect = new Rect(0.0f, 0.0f, 32, 32);

	// Local Variables
	int i;

	// Initialise Munchies

	Texture2D* munchieTex = new Texture2D();
	munchieTex->Load("Textures/Munchie.tga", true);

	for (i = 0; i < MUNCHIECOUNT; i++)
	{
		_munchies[i]->texture = munchieTex;
		_munchies[i]->texture = new Texture2D();
		_munchies[i]->texture->Load("Textures/Munchie.tga", true);
		_munchies[i]->texture2 = new Texture2D();
		_munchies[i]->texture2->Load("Textures/MunchieInverted.tga", true);
		_munchies[i]->sourceRect = new Rect(100.0f, 450.0f, 12, 12);
		_munchies[i]->position = new Vector2((rand() % Graphics::GetViewportWidth()), (rand() % Graphics::GetViewportHeight()));
	}

	// Initialise ghost
	_ghosts[0]->texture = new Texture2D();
	_ghosts[0]->texture->Load("Textures / GhostBlue.png", false);
	_ghosts[0]->position = new Vector2((rand() % Graphics::GetViewportWidth()), (rand() % Graphics::GetViewportHeight()));
	_ghosts[0]->sourceRect = new Rect(0.0f, 0.0f, 20, 20);

	// Set string position
	_stringPosition = new Vector2(10.0f, 25.0f);

	// Set Menu Parameters
	_menuBackground = new Texture2D();
	_menuBackground->Load("Textures/Transparency.png", false);
	_menuRectangle = new Rect(0.0f, 0.0f, Graphics::GetViewportWidth(), Graphics::GetViewportHeight());
	_menuStringPosition = new Vector2(Graphics::GetViewportWidth() / 2.0f, Graphics::GetViewportHeight() / 2.0f);

}

void Pacman::Input(int elapsedTime, Input::KeyboardState *state, Input::MouseState* mouseState)
{
	// Handle Mouse Input - Reposition Cherry
	if (mouseState->LeftButton == Input::ButtonState::PRESSED)
	{
		_cherry->position->X = mouseState->X;
		_cherry->position->Y = mouseState->Y;
	}

	// Handle Keyboard Input - Move Pacman
	// Move Pacman up
	if (state->IsKeyDown(Input::Keys::W))
	{
		_pacman->position->Y -= _cPacmanSpeed * elapsedTime * _pacman->speedMultiplier;
		_pacman->direction = 3;
	}
	// Move Pacman down
	else if (state->IsKeyDown(Input::Keys::S))
	{
		_pacman->position->Y -= _cPacmanSpeed * elapsedTime * _pacman->speedMultiplier;
		_pacman->direction = 1;
	}
	// Move Pacman left
	else if (state->IsKeyDown(Input::Keys::A))
	{
		_pacman->position->X -= _cPacmanSpeed * elapsedTime * _pacman->speedMultiplier;
		_pacman->direction = 2;
	}
	// Move Pacman right
	else if (state->IsKeyDown(Input::Keys::D))
	{
		_pacman->position->X -= _cPacmanSpeed * elapsedTime * _pacman->speedMultiplier;
		_pacman->direction = 0;
	}
	
	// Speed Multiplier 
	if (state->IsKeyDown(Input::Keys::LEFTSHIFT))
	{
		// Apply multiplier
		_pacman->speedMultiplier = 2.0f;
	}
	else
	{
		// Reset multiplier
		_pacman->speedMultiplier = 1.0f;
	}
}

void Pacman::CheckPaused(Input::KeyboardState *state, Input::Keys pauseKey)
{
	if (state->IsKeyDown(Input::Keys::P) && !_pKeyDown)
	{
		_pKeyDown = true;
		_paused = !_paused;
	}

	if (state->IsKeyUp(Input::Keys::P))
		_pKeyDown = false;

	if (!_paused)
	{
	}
}
void Pacman::CheckViewportCollision()
{
	// Checks if Pacman is trying to disappear on the X axis
	if (_pacman->position->X + _pacman->sourceRect->Width > Graphics::GetViewportWidth()) // 1024 is game width
	{
		// Pacman hit right wall - reset his position 
		_pacman->position->X = 32 - _pacman->sourceRect->Width;
	}

	// Checks if Pacman is trying to disappear on the X axis
	if (_pacman->position->X + _pacman->sourceRect->Width < 32)
	{
		// Pacman hit left wall - reset his position
		_pacman->position->X = Graphics::GetViewportWidth() - _pacman->sourceRect->Width;
	}

	// Checks if Pacman is trying to disappear on the Y axis
	if (_pacman->position->Y + _pacman->sourceRect->Height > 768) // 768 is game height
	{
		//Pacman hit bottom wall - reset his position
		_pacman->position->Y = 32 - _pacman->sourceRect->Height;
	}

	// Checks if Pacman is trying to disappear on the Y axis
	if (_pacman->position->Y + _pacman->sourceRect->Height < 32)
	{
		// Pacman hit top wall - reset his position 
		_pacman->position->Y = 768 - _pacman->sourceRect->Height;
	}
}

void Pacman::CheckGhostCollisions()
{
	// Local variables
	int i = 0;
	int bottom1 = _pacman->position->Y + _pacman->sourceRect->Height;
	int bottom2 = 0;
	int left1 = _pacman->position->X;
	int left2 = 0;
	int right1 = _pacman->position->X + _pacman->sourceRect->Width;
	int right2 = 0;
	int top1 = _pacman->position->Y;
	int top2 = 0;

	for (i = 0; i < GHOSTCOUNT; i++)
	{
		// Populate the variables with Ghost data
		bottom2 = _ghosts[i]->position->Y + _ghosts[i]->sourceRect->Height;
		left2 = _ghosts[i]->position->X;
		right2 = _ghosts[i]->position->X + _ghosts[i]->sourceRect->Width;
		top2 = _ghosts[i]->position->Y;

		if ((bottom1 > top2) && (top1 < bottom2) && (right1 > left2) && (left1 < right2))
		{
			_pacman->dead = true;
			i = GHOSTCOUNT;
		}
	}
}

void Pacman::UpdatePacman(int elapsedTime)
{
	if (_pacman->currentFrameTime > _cPacmanFrameTime)
	{
		_pacman->frame++;

		if (_pacman->frame >= 2)
		{
			_pacman->frame = 0;
		}

		_pacman->currentFrameTime = 0;
	}
}

void Pacman::UpdateMunchie(Enemy* munchie, int elapsedTime)
{
	if (munchie->currentFrameTime > _cMunchieFrameTime)
	{
		munchie->frame++;

		if (munchie->frame >= 2)
		{
			munchie->frame = 0;
		}

		munchie->currentFrameTime = 0;
	}
}

void Pacman::UpdateGhost(MovingEnemy* ghost, int elapsedTime)
{
	if (ghost->direction == 0) // Moves right
	{
		ghost->position->X += ghost->speed * elapsedTime;
	}
	else if (ghost->direction == 1) // Moves Left
	{
		ghost->position->X -= ghost->speed * elapsedTime;
	}
	if (ghost->position->X + ghost->sourceRect->Width >= Graphics::GetViewportWidth()) // Hits right edge
	{
		ghost->direction = 1; // Change direction
	}
	else if (ghost->position->X <= 0) // Hits left edge
	{
		ghost->direction = 0; // Change direction
	}
}

void Pacman::Update(int elapsedTime)
{	
	// Gets the current state of the mouse
	Input::MouseState* mouseState = Input::Mouse::GetState();

	// Gets the current state of the keyboard
	Input::KeyboardState* keyboardState = Input::Keyboard::GetState();

	// Check if the game is paused
	CheckPaused(keyboardState, Input::Keys::P);

	// Update tasks
	if (!_paused)
	{
		Input(elapsedTime, keyboardState, mouseState);
		UpdatePacman(elapsedTime);
		UpdateGhost(_ghosts[0], elapsedTime);
		CheckGhostCollisions();
		CheckViewportCollision();

		for (int i = 0; i < MUNCHIECOUNT; i++)
		{
			UpdateMunchie(_munchies[i], elapsedTime);
		}

	}
	_pacman->currentFrameTime += elapsedTime;
	//_munchies[MUNCHIECOUNT]->currentFrameTime += elapsedTime;

	_pacman->sourceRect->Y = _pacman->sourceRect->Height * _pacman->direction;
	_pacman->sourceRect->X = _pacman->sourceRect->Width * _pacman->frame;



	/* Checks if the Space bar is pressed
	if (keyboardState->IsKeyDown(Input::Keys::SPACE) && !_spaceKeyDown)
	{
		_spaceKeyDown = true;
		_start = true;
	}

	if (_start == true)
	{

		// Checks if the P key is pressed
		if (keyboardState->IsKeyDown(Input::Keys::P) && !_pKeyDown)
		{
			_pKeyDown = true;
			_paused = !_paused;
		}

		if (keyboardState->IsKeyUp(Input::Keys::P))
		{
			_pKeyDown = false;
		}
		if (!_paused)
		{

			_munchieFrameCount++;
			

			 // Checks if D key is pressed
			if (keyboardState->IsKeyDown(Input::Keys::D))
			{
				_pacmanPosition->X += _cPacmanSpeed * elapsedTime; //Moves Pacman across X axis
				_pacmanDirection = 0;
			}

			// Checks if A key is pressed
			else if (keyboardState->IsKeyDown(Input::Keys::A))
			{
				_pacmanPosition->X -= _cPacmanSpeed * elapsedTime; //Moves Pacman across X axis
				_pacmanDirection = 2;
			}

			// Checks if W key is pressed
			else if (keyboardState->IsKeyDown(Input::Keys::W))
			{
				_pacmanPosition->Y -= _cPacmanSpeed * elapsedTime; //Moves Pacman across X axis
				_pacmanDirection = 3;
			}

			// Checks if S key is pressed
			else if (keyboardState->IsKeyDown(Input::Keys::S))
			{
				_pacmanPosition->Y += _cPacmanSpeed * elapsedTime; //Moves Pacman across X axis
				_pacmanDirection = 1;
			}
			

			// Checks if Pacman is trying to disappear on the X axis
			if (_pacmanPosition->X + _pacmanSourceRect->Width > Graphics::GetViewportWidth()) // 1024 is game width
			{
				// Pacman hit right wall - reset his position 
				_pacmanPosition->X = 32 - _pacmanSourceRect->Width;
			}

			// Checks if Pacman is trying to disappear on the X axis
			if (_pacmanPosition->X + _pacmanSourceRect->Width < 32)
			{
				// Pacman hit left wall - reset his position
				_pacmanPosition->X = Graphics::GetViewportWidth() - _pacmanSourceRect->Width;
			}

			// Checks if Pacman is trying to disappear on the Y axis
			if (_pacmanPosition->Y + _pacmanSourceRect->Height > 768) // 768 is game height
			{
				//Pacman hit bottom wall - reset his position
				_pacmanPosition->Y = 32 - _pacmanSourceRect->Height;
			}

			// Checks if Pacman is trying to disappear on the Y axis
			if (_pacmanPosition->Y + _pacmanSourceRect->Height < 32)
			{
				// Pacman hit top wall - reset his position 
				_pacmanPosition->Y = 768 - _pacmanSourceRect->Height;
			}
		}
	}
	if (_pacmanCurrentFrameTime > _cPacmanFrameTime)
	{
		_pacmanFrame++;

		if (_pacmanFrame >= 2)
		{
			_pacmanFrame = 0;
		}

		_pacmanCurrentFrameTime = 0;
	}
	if (_munchieCurrentFrameTime > _cMunchieFrameTime)
	{
		_munchieFrameCount++;

		if (_munchieFrameCount >= 2)
		{
			_munchieFrameCount = 0;
		}
	
		_munchieCurrentFrameTime = 0;
	}*/
}

void Pacman::Draw(int elapsedTime)
{
	// Allows us to easily create a string
	std::stringstream stream;
	stream << "Pacman X: " << _pacman->position->X << " Y: " << _pacman->position->Y;

	SpriteBatch::BeginDraw(); // Starts Drawing
	if (!_pacman->dead)
	{
		SpriteBatch::Draw(_pacman->texture, _pacman->position, _pacman->sourceRect); // Draws Pacman
	}
	if (_munchies[MUNCHIECOUNT]->frame == 0)
	{
		// Draws Red Munchie
		SpriteBatch::Draw(_munchies[MUNCHIECOUNT]->texture2, _munchies[MUNCHIECOUNT]->sourceRect, nullptr, Vector2::Zero, 1.0f, 0.0f, Color::White, SpriteEffect::NONE);
	}
	else
	{
		// Draw Blue Munchie
		SpriteBatch::Draw(_munchies[MUNCHIECOUNT]->texture, _munchies[MUNCHIECOUNT]->sourceRect, nullptr, Vector2::Zero, 1.0f, 0.0f, Color::White, SpriteEffect::NONE);
		
		_munchies[MUNCHIECOUNT]->frame++;

		if (_munchies[MUNCHIECOUNT]->frame >= 60)
		{
			_munchies[MUNCHIECOUNT]->frame = 0;
		}
		
	}

	// Draw Ghosts
	SpriteBatch::Draw(_ghosts[0]->texture, _ghosts[0]->position, _ghosts[0]->sourceRect);
	
	// Draws String
	SpriteBatch::DrawString(stream.str().c_str(), _stringPosition, Color::Green);
	if (_start == false)
	{
		std::stringstream menuStream;
		menuStream << "PRESS SPACE TO START!";

		SpriteBatch::Draw(_menuBackground, _menuRectangle, nullptr);
		SpriteBatch::DrawString(menuStream.str().c_str(), _menuStringPosition, Color::Red);
	}
	if (_paused)
	{
		std::stringstream menuStream;
		menuStream << "PAUSED!";

		SpriteBatch::Draw(_menuBackground, _menuRectangle, nullptr);
		SpriteBatch::DrawString(menuStream.str().c_str(), _menuStringPosition, Color::Red);
	}
	SpriteBatch::EndDraw(); // Ends Drawing
}