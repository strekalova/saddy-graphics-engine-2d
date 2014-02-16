/*! \mainpage Strees Test For Physics
	
	Describes a small test for physics engine in a game
 */
/*! \file main.cpp
	\author HiddenSeeker

	Describes a main file for game
 */
#include <renderer.h>
#include <fontmanager.h>
#include <texturemappedfont.h>
#include <texturemanager.h>
#include "world.h"

#include <math.h>
#include <time.h>

/*! Loads a texture from file and registers it in Texture Manager
	\param[in] filename name of loaded file 
	\param[in] texturename name of registered texture
	\return whether loading of texture was successfull
 */
bool load_texture(const char * filename,const char * texturename)
{
	sad::Texture * texture=new sad::Texture();
	bool result=texture->load(sad::String(filename));	
	if (result) { 
		sad::TextureManager::ref()->add(texturename, texture);
	} else	{
		SL_FATAL( fmt::Format("Loading \"{0}\" failed") << filename);
		delete texture;
	}
	return result;
}
/*! Loads a texture from file and registers it in Texture Manager.
	
	This is a special case, when texture has a white background, which must be converted 
	to tansparent before work.
	\param[in] filename name of loaded file 
	\param[in] texturename name of registered texture
	\return whether loading of texture was successfull
 */
bool load_texture_with_alphachannel(const char * filename,const char * texturename)
{
	bool result = load_texture(filename, texturename);
	if (result)
	{
		// With first component we set alpha channel to fully transparent (255) of white color
		// (255,255,255)
		sad::TextureManager::ref()->get(texturename)->setAlpha(255,sad::Color(255,255,255));
	}
	return result;
} 
/*! Returns a font from file and registers it in Font Manager 
 */
bool load_font(const sad::String & fontfolder, const sad::String & fontname)
{
	sad::String fontextendedname = fontfolder + fontname;
	sad::TextureMappedFont * fnt=new sad::TextureMappedFont();
	bool result = fnt->load(fontextendedname);
	if (result) { 
		sad::FontManager::ref()->add(fnt, fontname);
	} else	{
		SL_FATAL( fmt::Format("Loading \"{0}\" failed") << fontextendedname);
		delete fnt;
	}
	return result;
}

/*! A main function 
	\param[in] argc count of arguments
	\param[in] argv arguments
 */
int main(int argc, char** argv)
{
	// Init logs with target streams to file  and console
	sad::log::FileTarget * filetarget = new sad::log::FileTarget();
	filetarget->open("log.txt");

	// A format for console is described as {date}: [file and line] [subsystem] message
	sad::log::ConsoleTarget * consoletarget = new sad::log::ConsoleTarget(
		"{0}: [{1}] {3}{2}{4}", 0, true
	);
	sad::log::Log::ref()->addTarget(filetarget).addTarget(consoletarget);

	SL_MESSAGE ("This is stress-test for physics engine.\nYou will see what capabilities does it have");

	// Inits a renderer as non-fullscreen 800x600 window
	sad::Renderer::ref()->init(sad::Settings(800,600,false));
	SL_MESSAGE("Renderer successfully initialized!");	
	// Inits generator for spawns and random raings
	srand((unsigned int)time(NULL));

	//Loading resources. We re-use textures from game example. 
	bool res=true; 
	sad::String fontfolder = "examples/game/";
	res=res && load_font(fontfolder, "font"); 
	res=res && load_texture("examples/game/ingame.tga","background");
    res=res && load_texture_with_alphachannel("examples/game/objects.bmp","objects"); 
    if (!res)
	{
		SL_FATAL(sad::String("Resource loading failed!"));
		return 1;
	}
	SL_MESSAGE(sad::String("Resources successfully loaded"));
	
	// Create and run game
	World * world = new World();
	world->run();
	delete world;


	// If we are here, this means, that window is closed and we can exit the main loop
	return 0;
}
