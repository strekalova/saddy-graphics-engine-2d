/*! \file   scene.h
    \author HiddenSeeker
    \brief  Here placed a scene files
*/
#pragma once
#include "sadvector.h"
#include "sadhash.h"
#include "temporarilyimmutablecontainer.h"
#include "sadmutex.h"
#include "scenenode.h"

#include <assert.h>


namespace sad
{
class Camera;

/*! Scene is a special container, which renders itself, using a renderer
 */
class Scene: public sad::TemporarilyImmutableContainer<sad::SceneNode>
{
public:
	/*! Creates an empty scene
	 */
	Scene();                   
	/*! You can freely inherit and implement your own scene
	 */
	virtual ~Scene(); 
	/*! Returns current camera for scene
		\return current camera for scene
	 */
	sad::Camera   & camera(); 
	/*! Sets a renderer
		\param[in] renderer renderer part
	 */
	inline void setRenderer(sad::Renderer * renderer)
	{
		m_renderer = renderer;
	}
	/*! Returns a renderer from scene
		\return renderer from scene
	 */
	inline sad::Renderer * renderer() const
	{
		return m_renderer;
	}
	/*! Sets a camera in scene, deleting old camera
		\param[in] camera  new camera
	*/
	void setCamera(sad::Camera * camera);
	/*! Finds a layers for node
		\param[in] node this node
		\return -1 if not found, index otherwise
     */
	int findLayer(sad::SceneNode * node);
	/*! Sets a layer for node. If no node found in scene - nothing happens, if layer not found - it 
		pushes to end
		\param[in] node node data
		\param[in] layer layer number
	 */
	void setLayer(sad::SceneNode * node,unsigned int layer);
	/*! Swaps to layers for nodes
		\param[in] node1 first node
		\param[in] node2 second node
	 */
	void swapLayers(sad::SceneNode * node1, sad::SceneNode * node2);
	/*! Renders a scene, making it visible
	 */
	virtual void render();
	/*! Returns amount of scene objects
		\return objects amount
	 */
	inline unsigned long objectCount()
	{
		return m_layers.count();
	}
	/*! Sets scene activity flag, which determines, whether it should be rendered
		\param[in] active an activity flag
	 */
	inline void setActive(bool active)
	{
		m_active = active;
	}
	/*! Returns scene activity flag, which determines, whether it should be rendered
		\return whether scene is active
	 */
	inline bool active()
	{
		return m_active;
	}
protected:
	/*! Determines, whether scene is active and should be rendered
	 */
	bool                      m_active;
	/*! A layers, which stores a node 
	 */
	sad::Vector<SceneNode *>   m_layers;   
	/*! A camera, to be applied before rendering
	 */
	sad::Camera      *        m_camera;       
	/*! Renderer, which scene belongs to
	 */
	sad::Renderer    *        m_renderer;       
	/*! Adds an object to scene
		\param[in] node 
	 */
	virtual void addNow(sad::SceneNode * node);
	/*! Removes object from scene
		\param[in] node
	 */
	virtual void removeNow(sad::SceneNode * node);
	/*! Clears a scene
	 */
	virtual void clearNow();
};

}