#include "groupschangename.h"

// ReSharper disable once CppUnusedIncludeDirective
#include <db/save.h>

// ReSharper disable once CppUnusedIncludeDirective
#include <QLineEdit>

#include "../../core/editor.h"

#include "../../qstdstring.h"

#include "../../blockedclosuremethodcall.h"
#include "../../closuremethodcall.h"

#include "../../gui/uiblocks/uiblocks.h"
#include "../../gui/uiblocks/uianimationsgroupblock.h"

#include "../../gui/actions/actions.h"
#include "../../gui/actions/animationgroupactions.h"


history::groups::ChangeName::ChangeName(
    sad::animations::Group* d,
    int position, 
    const sad::String& oldvalue,
    const sad::String& newvalue

)
: history::groups::ChangeProperty<sad::String>(
      d,
      "name",
      oldvalue,
      newvalue
), m_position(position), m_should_update_ui(true)
{
}

void history::groups::ChangeName::commitWithoutUpdatingUI(core::Editor* ob)
{
    m_should_update_ui = false;
    this->commit(ob);
    m_should_update_ui = true;
}

history::groups::ChangeName::~ChangeName()
{

}

void history::groups::ChangeName::updateItem(core::Editor* e, const sad::String&) const
{
    e->uiBlocks()->uiAnimationsGroupBlock()->lstAnimationsGroup->item(m_position)->setText(
        e->actions()->groupActions()->nameForGroup(m_animation)
    );
}

void history::groups::ChangeName::tryUpdateUI(core::Editor* e, const sad::String& value)
{
    this->history::groups::ChangeProperty<sad::String>::tryUpdateUI(e, value);
    e->emitClosure(::bind(this, &history::groups::ChangeName::updateItem, e, value));	
}

void history::groups::ChangeName::updateUI(core::Editor* e, const sad::String& value)
{
    if (m_should_update_ui)
    {
        e->emitClosure( blocked_bind(
                e->uiBlocks()->uiAnimationsGroupBlock()->txtAnimationsGroupName,
                &QLineEdit::setText,
                STD2QSTRING(value)
            )
        );
    }
}
