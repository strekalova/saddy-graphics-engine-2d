#include "db/dbtypename.h"
#include "db/save.h"
#include "db/load.h"

#include "sadstring.h"
#include "sadpoint.h"
#include "sadrect.h"
#include "sadcolor.h"
#include "sadsize.h"
#include "object.h"
#include "sadcolor.h"
#include "sadrect.h"
#include "sadvector.h"
#include "sadptrvector.h"

#include "font.h"
#include "texturemappedfont.h"
#include "scenenode.h"
#include "label.h"
#include "formattedlabel.h"
#include "sprite2d.h"
#include "sprite3d.h"

#include "dialogue/phrase.h"

DECLARE_COMMON_TYPE(bool)
DECLARE_COMMON_TYPE(char)
DECLARE_COMMON_TYPE(signed char)
DECLARE_COMMON_TYPE(unsigned char)
DECLARE_COMMON_TYPE(short)
DECLARE_COMMON_TYPE(unsigned short)
DECLARE_COMMON_TYPE(int)
DECLARE_COMMON_TYPE(unsigned int)
DECLARE_COMMON_TYPE(long)
DECLARE_COMMON_TYPE(unsigned long)
DECLARE_COMMON_TYPE(long long)
DECLARE_COMMON_TYPE(unsigned long long)
DECLARE_COMMON_TYPE(float)
DECLARE_COMMON_TYPE(double)
DECLARE_COMMON_TYPE(std::string)
DECLARE_COMMON_TYPE(sad::String)
DECLARE_COMMON_TYPE(sad::Point2D)
DECLARE_COMMON_TYPE(sad::Point2I)
DECLARE_COMMON_TYPE(sad::Point3D)
DECLARE_COMMON_TYPE(sad::Point3I)
DECLARE_COMMON_TYPE(sad::Rect<sad::Point3D>)
DECLARE_COMMON_TYPE(sad::Rect2D)
DECLARE_COMMON_TYPE(sad::Rect2I)
DECLARE_COMMON_TYPE(sad::Color)
DECLARE_COMMON_TYPE(sad::AColor)
DECLARE_COMMON_TYPE(sad::Size2D)
DECLARE_COMMON_TYPE(sad::Size2I)
DECLARE_COMMON_TYPE(sad::Vector<sad::Vector<sad::AColor> >)
DECLARE_COMMON_TYPE(sad::Vector<sad::Point2D>)
DECLARE_COMMON_TYPE(sad::dialogue::Phrase)
DECLARE_COMMON_TYPE(sad::Vector<sad::dialogue::Phrase*>)

DECLARE_TYPE_AS_SAD_OBJECT(sad::Object);