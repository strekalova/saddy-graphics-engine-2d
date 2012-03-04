/*! \file   sprite2dobserver.h
	\author HiddenSeeker
	
	An observer, which can work with config for 2d sprites is placed here
 */
#include "../sprite2dcontroller.h"
#include "sprite2dtemplate.h"
#include "../templates/hstring.h"
#pragma once

/* A config for sprites 
 */
class Sprite2DConfig;

/*! A policy for observer config, what it should do, 
	when size of sprite in config is changed
 */
enum Sprite2DConfigObserverPolicy
{
	COP_CHANGESIZE,           //!< Observer should change size of sprite to config size
	COP_CHANGEIFNOTCHANGED,   //!< Observer should change size, only if his size is not changed, otherwise keep
	                          //   it to custom size
    COP_KEEPSIZE              //!< Keep size of sprite anyway
};

/*! \class Sprite2DConfigObserver
	A config observer, which has a sprite, attached to it
 */
class Sprite2DConfigObserver
{
 private:
		 Sprite2DConfig *             m_config;    //!< Attached sprite config
		 Sprite2DConfigObserverPolicy m_policy;    //!< How observer should act
		 hst::string       m_textureid; //!< Sprite Group index in the config
		 int               m_index;     //!< Sprite index in config
		 
		 Sprite2DController * m_sprite;  //!< Sprite, which we are working on 
 public:
		 /*! Constructs a new observer. It does not creates a sprite, so you must create it
			 manually, using createSprite. But observer registers in a config. as observer at
			 creation.
			\param[in] texture name of sprite group
			\param[in] index   index of sprites
			\param[in] config  reference to config
			\param[in] policy  policy, how observer should work with setting size
		  */
		 Sprite2DConfigObserver(const hst::string & texture, int index, 
								Sprite2DConfig * config,
								Sprite2DConfigObserverPolicy policy = COP_CHANGESIZE
							   );

		 /*! Returns current associated sprite
			 \return sprite
		  */ 
		 inline Sprite2DController * sprite() const {return m_sprite; }
		 /*! Destructor. Deletes sprite, if needed
		  */
		 ~Sprite2DConfigObserver();
};

