/*! \file signedchardelegate.h
    

    A delegate for property of signed char
 */
#pragma once
#include "delegate.h"
#include <QObject>

namespace gui
{
    
namespace table
{
/*! A delegate for editing values for signed char values
 */
class SignedCharDelegate: public gui::table::Delegate
{
Q_OBJECT
public:
    /*! Constructs new delegate
     */
    SignedCharDelegate();
    /*! Destroys delegate
     */
    virtual ~SignedCharDelegate();
    /*! Sets a value for a delegate. Used by commands to reset
        \param[in] v a value
     */
    virtual void set(const sad::db::Variant& v);
public slots:
    /*! When widget changed
        \param[in] i a widget value
     */
    void widgetChanged(int i);
protected:
    /*! At this point delegate must create own editor, set it to table and connect delegate slots
        to it.
     */
    virtual void makeEditor();
    /*! Disconnects slots  for own editor here
     */ 
    virtual void disconnect();
};

/*! A char delegate
 */
typedef gui::table::SignedCharDelegate CharDelegate;

}

}
