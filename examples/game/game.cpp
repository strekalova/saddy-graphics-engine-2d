#include "game.h"

#include "bonus.h"
#include "enemy.h"
#include "shootingenemy.h"
#include "supershootingenemy.h"
#include "startscreenrain.h"
#include "enemyspawn.h"
#include "player.h"
#include "playerbullet.h"
#include "enemybullet.h"

#include <renderer.h>
#include <texturemanager.h>
#include <orthographiccamera.h>
#include <sprite2d.h>
#include <geometry2d.h>
#include <formattedlabel.h>
#include <pipeline/pipeline.h>
#include <input/controls.h>
#include <keymouseconditions.h>

const sad::String GameState::START = "start";
const sad::String GameState::PLAYING = "playing";


Game::Game()
{
	m_highscore = 0;
	m_ispaused = false;

	// Create a new machine
	m_machine = new sad::hfsm::Machine();
	sad::hfsm::Machine * m = m_machine;

	sad::input::Controls * controls = sad::Renderer::ref()->controls();
	controls->add(*sad::input::ET_KeyPress & sad::F, this, &Game::tryToggleFullscreen);
	controls->add(*sad::input::ET_KeyPress & sad::Esc, this, &sad::p2d::app::App::quit);

	// Create a new idle state
	sad::hfsm::State * idleState = new sad::hfsm::State();

	controls->add(
		(*sad::input::ET_KeyPress) & sad::Enter & (m * GameState::START), 
		this, &Game::startPlaying
	);

	idleState->addEnterHandler(this, &Game::enterStartingScreen);
	idleState->addLeaveHandler(this, &Game::leaveStartingScreen);

	// Create a new playing state
	sad::hfsm::State * playState = new sad::hfsm::State();
	controls->add(
		*sad::input::ET_KeyPress & sad::P & (m * GameState::PLAYING), 
		this,  &Game::togglePaused
	);

	// Controls, that respond for player shooting
	controls->add(
		*sad::input::ET_KeyPress & sad::Enter & (m * GameState::PLAYING), 
		this, &Game::player, &Player::startShooting
	);
	controls->add(
		*sad::input::ET_KeyPress & sad::Space & (m * GameState::PLAYING), 
		this, &Game::player, &Player::startShooting
	);
	controls->add(
		*sad::input::ET_MousePress & (m * GameState::PLAYING), 
		this, &Game::player, &Player::startShooting
	);
	controls->add(
		*sad::input::ET_KeyRelease & sad::Enter & (m * GameState::PLAYING), 
		this, &Game::player, &Player::stopShooting
	);
	controls->add(
		*sad::input::ET_KeyRelease & sad::Space & (m * GameState::PLAYING), 
		this, &Game::player, &Player::stopShooting
	);
	controls->add(
		*sad::input::ET_MouseRelease & (m * GameState::PLAYING), 
		this, &Game::player, &Player::stopShooting
	);

	// Controls, that respond for player movement
	controls->add(
		*sad::input::ET_KeyPress & sad::KeyLeft & (m * GameState::PLAYING), 
		this, &Game::player, &Player::tryStartMovingLeft
	);
	controls->add(
		*sad::input::ET_KeyPress & sad::KeyRight & (m * GameState::PLAYING), 
		this, &Game::player, &Player::tryStartMovingRight
	);
	controls->add(
		*sad::input::ET_KeyPress & sad::KeyUp & (m * GameState::PLAYING), 
		this, &Game::player, &Player::tryStartMovingUp
	);
	controls->add(
		*sad::input::ET_KeyPress & sad::KeyDown & (m * GameState::PLAYING), 
		this, &Game::player, &Player::tryStartMovingDown
	);
	controls->add(
		*sad::input::ET_KeyRelease & sad::KeyLeft & (m * GameState::PLAYING), 
		this, &Game::player, &Player::tryStopMovingHorizontally
	);
	controls->add(
		*sad::input::ET_KeyRelease & sad::KeyRight & (m * GameState::PLAYING), 
		this, &Game::player, &Player::tryStopMovingHorizontally
	);
	controls->add(
		*sad::input::ET_KeyRelease & sad::KeyUp & (m * GameState::PLAYING), 
		this, &Game::player, &Player::tryStopMovingVertically
	);
	controls->add(
		*sad::input::ET_KeyRelease & sad::KeyDown & (m * GameState::PLAYING), 
		this, &Game::player, &Player::tryStopMovingVertically
	);

	// Mouse movement
	controls->add(
		*sad::input::ET_MouseMove & (m * GameState::PLAYING), 
		this, &Game::player, &Player::tryLookAt
	);

	playState->addEnterHandler(this, &Game::enterPlayingScreen);
	playState->addLeaveHandler(this, &Game::leavePlayingScreen);


	SL_MESSAGE("Input handlers bound succesfully");

	// Add states - starting screen and playing screen
	m_machine->addState(GameState::START, idleState);
	m_machine->addState(GameState::PLAYING, playState);
	SL_MESSAGE("States bound successfully");

	m_player = NULL;
	
	this->initApp();
	m_spawntask =  new sad::PeriodicalEventPollProcess(NULL);
	sad::Renderer::ref()->pipeline()->append(m_spawntask);

	m_walls = NULL;
	m_registered_supershooting_enemies_count = 0;
}

void Game::startPlaying()
{
	m_machine->enterState(GameState::PLAYING);
}

Game::~Game()
{
	delete m_walls;
	delete m_machine;
}

bool Game::isPaused() const
{
	return m_ispaused;
}

bool Game::isPlaying() const
{
	return !isPaused();
}

void Game::togglePaused()
{
	m_ispaused = !m_ispaused;
	if (isPaused())
	{
		// Disable stepping the world
		this->m_steptask->disable();
	}
	else
	{
		// Enable stepping the world
		this->m_steptask->enable();
	}
}

int Game::highscore() const
{
	return m_highscore;
}

void Game::trySetHighscore(int score)
{
	if (score > m_highscore)
	{
		m_highscore = score;
	}
}

void Game::tryToggleFullscreen()
{
	if (isPlaying())
		sad::Renderer::ref()->toggleFullscreen();
}

void Game::run()
{
	sad::Renderer::ref()->setWindowTitle("sad::Game");
	
	// Set window size to be fixed
	sad::Renderer::ref()->makeFixedSize();

	m_machine->enterState(GameState::START);
	// Run an engine, starting a main loop
	SL_MESSAGE("Will start now");	

	sad::Renderer::ref()->run();
}

Player * Game::player()
{
	return m_player;
}

void Game::increasePlayerScore(int delta)
{
	this->trySetHighscore(this->player()->increaseScore(delta));
}

void Game::increasePlayerHealth(int by)
{
	this->player()->incrementHP(by);
}

void Game::decreasePlayerHealth(int by)
{
	this->player()->decrementHP(by);
}

void Game::leaveStartingScreen()
{
	this->scene()->clear();
	
	delete m_world;
	createWorld();
	m_steptask->setWorld(m_world);
	m_registered_supershooting_enemies_count = 0;
}

void Game::leavePlayingScreen()
{
	// We don't leave screen here, because it can be called from p2d::World::step()
	// so we can break execution chain, if we kill world here
}


void Game::enterStartingScreen()
{
	// See Game::moveToPlayingScreen. We should perform change from here,
	// otherwise, execution chain will break
	SL_SCOPE("Game::enterStartingScreen()");
	sad::Renderer::ref()->pipeline()->appendTask(this, &Game::moveToStartingScreen);
} 


void Game::enterPlayingScreen()
{
	m_ispaused = false;
	sad::Scene * sc = this->scene();

	sad::Texture * tex = sad::Renderer::ref()->textures()->get("background");
	sad::Sprite2D * background = new sad::Sprite2D(
		tex, 
		sad::Rect2D(0, 0, 512, 512),
		sad::Rect2D(0, 0, 640, 480)
	);
	sc->add(background);

	sad::FormattedLabel * label = new sad::FormattedLabel();
	label->setFont("font");
	label->setPoint(0, 480);
	label->setUpdateInterval(200);
	label->setSize(30);

	label->setFormatString("Health: {0} Score: {1} Highscore: {2}\nFPS: {3}");
	label->arg(this, &Game::player, &GameObject::hitPoints);
	label->arg(this, &Game::player, &Player::score);
	label->arg(this, &Game::highscore);
	label->argFPS();

	sc->add(label);
	m_spawntask->setEvent(new EnemySpawn(this) );

	// Handlers also register types in world, so they MUST BE added before
	// any object ia added to scene

	m_world->addHandler(this, &Game::onWallCollision);
	m_world->addHandler(this, &Game::onBonusCollision);
	m_world->addHandler(this, &Game::onPlayerBulletEnemy);
	m_world->addHandler(this, &Game::onPlayerBulletSEnemy);
	m_world->addHandler(this, &Game::onPlayerBulletSuperEnemy);
	m_world->addHandler(this, &Game::onPlayerEnemyBullet);
	m_world->addHandler(this, &Game::onPlayerEnemy);
	m_world->addHandler(this, &Game::onPlayerShootingEnemy);
	m_world->addHandler(this, &Game::onPlayerSuperShootingEnemy);

	// We add background, emitter and new player's alter-ego at 320,240 - center of screen
	Player * p  = new  Player();
	p->setPosition(sad::p2d::Point(320.0,240.0));
	addObject(p);

	m_player = p;	

	createWalls();
}

void Game::removeObject(sad::p2d::app::Object *o)
{
	// If player is dead, no reason to continue playing, 
	// return to start screen
	if (o->metaData()->name() == "Player")
	{
		m_machine->enterState(GameState::START);
	}
	if (o->metaData()->name() == "SuperShootingEnemy")
	{
		--m_registered_supershooting_enemies_count;
	}
	this->sad::p2d::app::App::removeObject(o);
}



const sad::String & Game::state()
{
	return m_machine->currentState();
}

GameObject *  Game::produce(Objects type)
{
	GameObject * result = NULL;
	switch(type)
	{
		case O_BONUS: result = new Bonus(); break;
        case O_ENEMY: result = new Enemy(); break;
        case O_SHOOTINGENEMY: result = new ShootingEnemy(); break;
		case O_SUPERSHOOTINGENEMY: 
		{
			if (m_registered_supershooting_enemies_count < 3)
			{
				result = new SuperShootingEnemy(); 
				++m_registered_supershooting_enemies_count;
			}
			break;
		}
	}
	if (result)
	{
		this->addObject(result);
	}
	return result;
}




void Game::createWalls()
{
	delete m_walls;
	m_walls = new sad::p2d::Walls(14);
	const sad::Vector<sad::p2d::Body *> & bodies = m_walls->bodies();
	for(size_t i = 0; i < bodies.count(); i++)
	{
		m_world->add(bodies[i]);
	}
}


void Game::onWallCollision(const sad::p2d::CollisionEvent<sad::p2d::Wall, GameObject> & ev)
{
	if (ev.object2().metaData()->name() == "Player")
	{
		ev.object1().tryTeleport(ev.object2().body());
	}
	else
	{
		removeObject(&(ev.object2()));
	}
}



void Game::onBonusCollision(const sad::p2d::CollisionEvent<Player, Bonus> & ev)
{
	ev.object2().decrementHP(1);
	ev.object1().incrementHP(1);
	ev.object1().increaseScore(50);
}

void Game::onPlayerBulletEnemy(const sad::p2d::CollisionEvent<PlayerBullet, Enemy> & ev)
{
	ev.object2().decrementHP(1);
	ev.object1().decrementHP(1);
	this->player()->increaseScore(100);
}

void Game::onPlayerBulletSEnemy(const sad::p2d::CollisionEvent<PlayerBullet, ShootingEnemy> & ev)
{
	ev.object2().decrementHP(1);
	ev.object1().decrementHP(1);
	this->player()->increaseScore(200);
}

void Game::onPlayerBulletSuperEnemy(const sad::p2d::CollisionEvent<PlayerBullet, SuperShootingEnemy> & ev)
{
	ev.object2().decrementHP(1);
	ev.object1().decrementHP(1);
	this->player()->increaseScore(400);
}

void Game::onPlayerEnemyBullet(const sad::p2d::CollisionEvent<Player, EnemyBullet> & ev)
{
	ev.object2().decrementHP(ev.object2().hitPoints());
	ev.object1().decrementHP(1);
}

void Game::onPlayerEnemy(const sad::p2d::CollisionEvent<Player, Enemy> & ev)
{
	ev.object2().decrementHP(ev.object2().hitPoints());
	ev.object1().decrementHP(1);
}

void Game::onPlayerShootingEnemy(const sad::p2d::CollisionEvent<Player, ShootingEnemy> & ev)
{
	ev.object2().decrementHP(ev.object2().hitPoints());
	ev.object1().decrementHP(1);
}

void Game::onPlayerSuperShootingEnemy(const sad::p2d::CollisionEvent<Player, SuperShootingEnemy> & ev)
{
	ev.object2().decrementHP(ev.object2().hitPoints());
	ev.object1().decrementHP(1);
}


void Game::moveToStartingScreen()
{
	SL_SCOPE("Game::moveToStartingScreen()");
	sad::Renderer::ref()->setScene(new sad::Scene());
	delete m_world;
	createWorld();
	m_steptask->setWorld(m_world);
	
	m_player = NULL;
	m_ispaused = false;
	sad::Scene * sc = this->scene();

	// Fill screne with background, label and rain of element (the last object does that).
	sad::Texture * tex = sad::Renderer::ref()->textures()->get("title");
	sad::Sprite2D * background = new sad::Sprite2D(
		tex, 
		sad::Rect2D(0, 0, 512, 512),
		sad::Rect2D(0, 0, 640, 480)
	);
	sc->add(background);
	
	sad::FormattedLabel  * label = new sad::FormattedLabel();
	label->setFont("font");
	label->setSize(30);
	label->setPoint(265, 240);
	label->setUpdateInterval(1000);
	// Here we build a format string, where {0} will be replaced with call of this->highscore()
	label->setFormatString("Highscore: {0}");
	label->arg(this, &Game::highscore);
	sc->add(label);
	

	m_spawntask->setEvent(new StartScreenRain(this) );
	// Handlers also register types in world, so they MUST BE added before
	// any object ia added to scene
	m_world->addHandler(this, &Game::onWallCollision);
	this->createWalls();
	
	m_registered_supershooting_enemies_count = 0;
}
