/*! \file animations/setstate/setcamerarotation.h
    

    Defines a comand, which rotates scene's camea for angle around a pivot
 */
#pragma  once
#include "typedcommand.h"

#include "../../camera.h"
#include "../../scene.h"
#include "../../sadpoint.h"

namespace sad
{

namespace animations
{

namespace setstate
{

class SetCameraRotation: public sad::animations::setstate::TypedCommmand<double>
{
public:
    /*! Sets a scene and pivot for translation
        \param[in] s scene
        \param[in] p a pivot offset for rotation
     */
    SetCameraRotation(sad::Scene* s, const sad::Point3D& p);
    /*! Clones command
        \return command
     */
    virtual sad::animations::setstate::AbstractSetStateCommand* clone() const;
    /*! Invokes a command with specified command
        \param[in] angle argument
     */
    virtual void call(const double& angle);
    /*! Must be inherited
     */
    virtual ~SetCameraRotation();
protected:
    /*! A scene, which should be used
     */
    sad::Scene* m_scene;
    /*! A pivot for camera rotation
     */
    sad::Point3D m_pivot;
};

}

}

}
