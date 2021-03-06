#ifndef WC3LIB_EDITOR_ENVIRONMENT_HPP
#define WC3LIB_EDITOR_ENVIRONMENT_HPP

#include <OGRE/Terrain/OgreTerrain.h>
#include <OGRE/Terrain/OgreTerrainGroup.h>

#include "resource.hpp"

#include "../map/environment.hpp"

namespace wc3lib
{

namespace editor
{

class Environment : public Resource
{
	public:
		static Ogre::TerrainGroup* convert(Ogre::SceneManager *sceneManager, const map::Environment &environment, const QString &fileName, const QString &fileExtension);
		static Ogre::ManualObject* convert(Ogre::SceneManager *sceneManager, const map::Environment &environment, const Ogre::String &name);

		Environment(const QUrl &url);

		virtual void load();
	private:
		Ogre::TerrainGroup *m_group;
};

}

}

#endif
