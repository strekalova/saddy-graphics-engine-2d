/** \file editorbehaviourstate.h
	\author HiddenSeeker

	Describes an editor behaviour state data
 */
#include <input/controls.h>
#include <log/log.h>
#pragma once

class EditorBehaviour;


/** Describes an editor behaviour state, used to handle some events
 */
class EditorBehaviourState
{
 protected:
	 /** Describes a behaviour for data
	  */
	 EditorBehaviour * m_behaviour;
 public:
	 EditorBehaviourState();
	 /** Sets a behaviour for state
	  */
	 inline void setBehaviour(EditorBehaviour * behaviour)
	 {
		 m_behaviour = behaviour;
	 }
	 /** Returns a behaviour
	  */
	 inline EditorBehaviour * behaviour()
	 {
		 return m_behaviour;
	 }
	 /** Action, performed when entering state
	  */
	 virtual void enter();
	 /** Action, performed when leaving state
	  */
	 virtual void leave();
	 /** Handles mouse movement
		 \param[in] ev event data
	  */
	 virtual void onMouseMove(const sad::input::MouseMoveEvent & ev);
	 /** Handles mouse down event
		 \param[in] ev  even  data
	  */
	 virtual void onMouseDown(const sad::input::MousePressEvent & ev);
	 /** Handles mouse down event
		 \param[in] ev  even  data
	  */
	 virtual void onMouseUp(const sad::input::MouseReleaseEvent & ev);
	 /** Handles wheel  movement
		 \param[in] ev event data
	  */
	 virtual void onWheel(const sad::input::MouseWheelEvent & ev);
	

	 /** Handles key down event
		 \param[in] ev event data
	  */
	 virtual void onKeyDown(const sad::input::KeyPressEvent & ev);
	 /** Handles key down event
		 \param[in] ev event data
	  */
	 virtual void onKeyUp(const sad::input::KeyReleaseEvent & ev);
	 /** Does nothing
	  */
	 virtual ~EditorBehaviourState();

	
};