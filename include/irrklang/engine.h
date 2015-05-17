/*! \file engine.h
    \author HiddenSeeker

    Defines a global device, which can be accessed from multiple entry points
 */
#pragma once
#include <sadhash.h>
#include <sadmutex.h>

#include <irrKlang.h>


namespace sad
{

namespace irrklang
{
    
class Engine
{
public:
	/*! Returns global reference to an engine
	 */
	static sad::irrklang::Engine* ref();
    /*! Tries to load file from source
        \param[in] source a source
        \return source data
     */
    ::irrklang::ISoundSource* tryLoad(const sad::String& source);
    /*! Adds new source for alias
        \param[in] source 
        \param[in] name a name for alias
        \return copy source
     */
    ::irrklang::ISoundSource* addAsAlias(
        ::irrklang::ISoundSource* source,
        const sad::String& name
    );
	/*! Frees engine instance
     */
    ~Engine();
	/*! Returns engine for global wrapper	
		\return global engine
	 */
	::irrklang::ISoundEngine* get() const;
	/*! Equiualent for calling ref()->get()
		\return global engine
	 */ 
	static ::irrklang::ISoundEngine* eref();
protected:
    /*! A sound engine, which is being used
     */
    ::irrklang::ISoundEngine* m_engine;
    /*! A global instance of engine
     */
    static sad::irrklang::Engine* m_instance;
private:
    /*! A default constructor for engine
     */
    Engine();
    /*! This type is not copyable
        \param[in] o other engine
     */
    Engine(const sad::irrklang::Engine& o);
    /*! This type is not copyable
        \param[in] o other engine
        \return self-reference
     */
    sad::irrklang::Engine& operator=(const sad::irrklang::Engine& o);
};

}

}
