/** \file   fonttemplatesdatabase.h
	\author HiddenSeeker

	A database for holding all of fonts and templates, loaded at time.
	You can with
 */
#include <QHash>
#include <QString>
#include <QDomDocument>
#include <log/log.h>
#include "fontdatabase.h"
#pragma once

namespace db
{
typedef QHash<QString, QString> NameFileMap;
}

class EditorLog;

/** A maps, to be read from XML file data
 */
class FontTemplatesMaps
{
 private:
		db::NameFileMap m_fonts;
		db::NameFileMap m_configs;
		/** Loads a found entry
			\param[in] entry element
			\param[in] parent parent directory
			\param[in] log log data
		 */
		void loadFont(QDomElement & entry, const sad::String & parent, sad::log::Log * log);
		/** Loads a found config
			\param[in] entry element
			\param[in] parent parent directory
			\param[in] log log data
		 */
		void loadConfig(QDomElement & entry, const sad::String & parent, sad::log::Log * log);

 public:
		/** Constructs new empty maps
		 */
		FontTemplatesMaps();
		/** Loads a configs from file, specified by path and name
			Also preprocesses a local paths from config data
		 */
		bool load(const QString & name, sad::log::Log * log);
		/** Read fonts
			\return entities of fonts
		 */
	    inline const db::NameFileMap & fonts() { return m_fonts; }
		/** Read config
			\return entities  of configs
		 */
		inline const db::NameFileMap & configs() { return m_configs;}
};


class SpriteDatabase;
/** A database, which stores a font database and other in depth
 */
class FontTemplateDatabase
{
 private:
		 /** Counter, that makes every loading of textures mutable
		  */
		 int * m_counter;
	     /** Fonts database data 
		  */
		 IFaceEditorFontList * m_fonts;
		 /** Used maps data
		  */
		 FontTemplatesMaps m_maps;
		 /** A sprite database 
		  */
		 SpriteDatabase * m_sprites;
 public:
		/** Creates an empty database	
			\param[in] counter counter data
		 */
		FontTemplateDatabase(int * counter);
        /** Loads new font template maps
			NOTE: That function is not reentrant, please do not use it second time
			\param[in] maps maps data
			\param[in] log log to write to
		 */
		bool load(FontTemplatesMaps & maps, sad::log::Log * log);
		/** Fonts, represented in database
			\return fonts
		 */
		inline IFaceEditorFontList & fonts() { return *m_fonts; }
		/** Returns used sprites in data
			\return used sprites
		 */
		inline SpriteDatabase & sprites() { return *m_sprites; }
		/** Maps, which fonts and configs came from
			\return maps, where configs came from
		 */
		inline FontTemplatesMaps & maps() { return m_maps; }
		/** Frees memory of database
		 */
		~FontTemplateDatabase();
};