#include "groupsaddinstance.h"

#include "../../core/editor.h"

#include "../../closuremethodcall.h"

#include "../../gui/uiblocks/uiblocks.h"
#include "../../gui/uiblocks/uianimationsgroupblock.h"

#include "../../gui/actions/actions.h"
#include "../../gui/actions/animationinstanceactions.h"


Q_DECLARE_METATYPE(sad::animations::Group*) //-V566
Q_DECLARE_METATYPE(sad::animations::Instance*) //-V566

history::groups::AddInstance::AddInstance( sad::animations::Group* d,
         sad::animations::Instance* i,
         int pos
) : m_node(d), m_instance(i), m_position(pos)
{
    m_node->addRef();
    m_instance->addRef();
}

history::groups::AddInstance::~AddInstance()
{
    m_node->delRef();
    m_instance->delRef();
}

void history::groups::AddInstance::commit(core::Editor * ob)
{
    m_node->addAsLink(m_instance);

    if (ob)
    {
        if (ob->shared()->selectedGroup() == m_node)
        {
            ob->emitClosure(bind(
                this,
                &history::groups::AddInstance::updateUIWhileCommitting,
                ob
            ));
        }
    }
}

void history::groups::AddInstance::rollback(core::Editor * ob)
{
    m_node->removeAsLink(m_instance);

    if (ob)
    {
        if (ob->shared()->selectedGroup() == m_node)
        {
            ob->emitClosure(bind(
                this,
                &history::groups::AddInstance::updateUIWhileRollingBack,
                ob
            ));
        }
    }
}

void history::groups::AddInstance::updateUIWhileCommitting(core::Editor* e)
{
    QVariant v;
    v.setValue(m_instance);

    QListWidgetItem* item = new QListWidgetItem(e->actions()->instanceActions()->nameForInstance(m_instance));
    item->setData(Qt::UserRole, v);
    
    e->uiBlocks()->uiAnimationsGroupBlock()->lstAnimationsGroupInGroup->addItem(item);

    delete e->uiBlocks()->uiAnimationsGroupBlock()->lstAnimationsGroupAllAnimations->takeItem(m_position);
}

void history::groups::AddInstance::updateUIWhileRollingBack(core::Editor* e)
{
    QVariant v;
    v.setValue(m_instance);

    QListWidgetItem* item = new QListWidgetItem(e->actions()->instanceActions()->nameForInstance(m_instance));
    item->setData(Qt::UserRole, v);
    
    e->uiBlocks()->uiAnimationsGroupBlock()->lstAnimationsGroupAllAnimations->insertItem(m_position, item);

    QListWidget* w = e->uiBlocks()->uiAnimationsGroupBlock()->lstAnimationsGroupInGroup;
    delete w->takeItem(w->count() - 1);
}
