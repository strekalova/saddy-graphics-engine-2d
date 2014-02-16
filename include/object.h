/*! \file ../include/object.h
	\author HiddenSeeker

	Defines a universal object class, used as container for metadataa
 */
#pragma once
#include "classmetadatacontainer.h"

namespace sad
{

class Object
{
 public:
	 /*! A metadata for class, must be implemented in any descendant
		 \return metadata
	  */
	 static sad::ClassMetaData * globalMetaData();
	 /*!  Returns a metadata for class
		  \return metadata
	  */ 
	 virtual sad::ClassMetaData * metaData() const;
	 /*! Returns a name for class
		 \return name of class
	  */
	 virtual const sad::String & name() const;
	 /*! An object data
	  */
	 virtual ~Object();
	 /*! Performs checked casting to object, throws exception on error
		 \return type if it can be casted, otherwise throws an exception
	  */
	 template<typename _Dest> _Dest * as();
	 /*! Performs chained checked casting to object, 
	     throws exception on error. First time casts it to T1, second time 
		 to T2
		 \return type if it can be casted, otherwise throws an exception
	  */
	 template<typename T1, typename T2> T2 * as();
};

/*! An exception, which is thrown, when one object can not be casted to another
 */
class InvalidCastException
{
 private:
	 /*! A name of real type of object
	  */
	 sad::String m_fromname;
	 /*! A name of type, which we were casting to
	  */
	 sad::String m_toname;
	 /*! A message for type
	  */
	 sad::String m_message;
 public:
	 /*! Creates new exception with specified parameters
		 \param[in] fromname real type of object
		 \param[in] toname type, which we were casting to
	  */
	 InvalidCastException(const sad::String & fromname, const sad::String & toname);
	 /*! A real type of object
		 \return real type of object
	  */
	 const sad::String & fromName();
	 /*! A type, which we were casting to
		 \return type, which we were casting to
	  */ 
	 const sad::String & toName();
	 /*! A human-readable message for object
		 \return a message for exception
	  */
	 const sad::String & message();
};


}

namespace sad 
{
/*! Performs a checked cast from one type to another.
	Used to widen domain, or when sad::Object::as does not work
	(multiple inheritance with some uncommon path).

	It is highly unlikely to use it with multiple inheritance, because
	when casting from base sad::Object to second ancestor, or third, due
	to behaviour of upcast we will get invalid pointer, because compiler
	will fail to compute offset of second class in structure.
	
	\param[in] arg argument
	\return result of cast
 */
template<typename _Dest, typename _Src> _Dest * checked_cast(_Src * arg)                
{                                                                
	_Dest * result = NULL;                                       
	const sad::String & destname = _Dest::globalMetaData()->name();      
	if (arg->metaData()->canBeCastedTo(destname) == false)      
	{                                                            
		throw sad::InvalidCastException(arg->name(), destname); 
	}                                                            
	else                                                         
	{       
		result = static_cast<_Dest*>(arg);                      
	}                                                            
	return result;                                               
}  


}

/*! Declares methods as for specified class
 */
#define DECLARE_CASTS_AS_METHODS(CLASSNAME)                      \
template<typename _Dest> _Dest * CLASSNAME ::as()                \
{                                                                \
	return sad::checked_cast<_Dest, CLASSNAME > (this);          \
}                                                                \
                                                                 \
template<typename T1, typename T2> T2 * CLASSNAME ::as()         \
{                                                                \
	T1 * tmp = sad::checked_cast<T1, CLASSNAME >(this);          \
	T2 * result = sad::checked_cast<T2, T1>(tmp);                \
	return result;                                               \
}                                                                \

DECLARE_CASTS_AS_METHODS(sad::Object)

                                                              


/*! Use this macro to define a descendant of sad::Object in your include file
 */
#define SAD_OBJECT												\
protected:                                                      \
	    static sad::ClassMetaData * m_global_metadata;          \
public:															\
		virtual sad::ClassMetaData * metaData() const;			\
		virtual const sad::String & name() const;               \
		static sad::ClassMetaData * globalMetaData();			\

/*! Use this macro to define in source files, 
	that this object is inherited from descendant of sad::Object,
	where NAMEDCLASS should be name of current class and PARENT  - name of his parent class
 */
#define DECLARE_SOBJ_INHERITANCE(NAMEDCLASS, PARENT)			 \
sad::ClassMetaData * NAMEDCLASS ::m_global_metadata=NULL;	     \
sad::ClassMetaData * NAMEDCLASS ::globalMetaData()	  		 	 \
{																 \
	if (m_global_metadata != NULL) return m_global_metadata;     \
    bool created = false;																		\
	m_global_metadata = sad::ClassMetaDataContainer::ref()->get(#NAMEDCLASS, created);          \
	if (created)																				\
	{																							\
		m_global_metadata->addAncestor(#PARENT);											    \
	}																							\
	return m_global_metadata;																	\
}																								\
sad::ClassMetaData * NAMEDCLASS ::metaData() const												\
{                                                                                               \
	return NAMEDCLASS ::globalMetaData();                                                       \
}                                                \
const sad::String &  NAMEDCLASS ::name() const  \
{                                                \
	return this-> PARENT :: name();              \
}

/*! Use this macro to define, that this class is direct descendant of sad::Object in your source 
	file. NAMEDCLASS is name of your class
 */ 
#define DECLARE_SOBJ(NAMEDCLASS) DECLARE_SOBJ_INHERITANCE(NAMEDCLASS, sad::Object)

/*! Use this macro to define in source files, 
	that this object is inherited from descendant of sad::Object other classes,
	where NAMEDCLASS should be name of current class and PARENT1, PARENT2 
	- name of his parent classes
 */
#define DECLARE_SOBJ_INHERITANCE2(NAMEDCLASS, PARENT1, PARENT2)			 \
sad::ClassMetaData * NAMEDCLASS ::m_global_metadata=NULL;	             \
sad::ClassMetaData * NAMEDCLASS ::globalMetaData()	  		 			 \
{																		 \
	if (m_global_metadata != NULL) return m_global_metadata;             \
	bool created = false;																		\
	m_global_metadata = sad::ClassMetaDataContainer::ref()->get(#NAMEDCLASS, created);          \
	if (created)																				\
	{																							\
		m_global_metadata->addAncestor(#PARENT1);												\
		m_global_metadata->addAncestor(#PARENT2);												\
	}																							\
	return m_global_metadata;																	\
}																								\
sad::ClassMetaData * NAMEDCLASS ::metaData() const												\
{                                                                                               \
	return NAMEDCLASS ::globalMetaData();                                                       \
}    \
const sad::String &  NAMEDCLASS :: name() const  \
{                                                \
	return this-> PARENT1 :: name();              \
}

/*! Use this macro to define in source files, 
	that this object is inherited from descendant of sad::Object other classes,
	where NAMEDCLASS should be name of current class and PARENT1, PARENT2, PARENT3 
	- name of his parent classes
 */
#define DECLARE_SOBJ_INHERITANCE3(NAMEDCLASS, PARENT1, PARENT2, PARENT3) \
sad::ClassMetaData * NAMEDCLASS ::m_global_metadata=NULL;	             \
sad::ClassMetaData * NAMEDCLASS ::globalMetaData()	  		 			 \
{																		 \
	if (m_global_metadata != NULL) return m_global_metadata;             \
	bool created = false;																		\
	m_global_metadata = sad::ClassMetaDataContainer::ref()->get(#NAMEDCLASS, created);          \
	if (created)																				\
	{																							\
		m_global_metadata->addAncestor(#PARENT1);												\
		m_global_metadata->addAncestor(#PARENT2);												\
		m_global_metadata->addAncestor(#PARENT3);												\
	}																							\
	return m_global_metadata;																	\
}																								\
sad::ClassMetaData * NAMEDCLASS ::metaData() const												\
{                                                                                               \
	return NAMEDCLASS ::globalMetaData();                                                       \
}   \
const sad::String &  NAMEDCLASS :: name() const  \
{                                                \
	return this-> PARENT1 :: name();              \
}

/*! Use this macro to define in source files, 
	that this object is inherited from descendant of sad::Object other classes,
	where NAMEDCLASS should be name of current class and PARENT1, PARENT2, PARENT3, PARENT4 
	- name of his parent classes
 */
#define DECLARE_SOBJ_INHERITANCE4(NAMEDCLASS, PARENT1, PARENT2, PARENT3, PARENT4) \
sad::ClassMetaData * NAMEDCLASS ::m_global_metadata=NULL;	                      \
sad::ClassMetaData * NAMEDCLASS ::globalMetaData()	  		 					  \
{																				  \
	if (m_global_metadata != NULL) return m_global_metadata;                      \
	bool created = false;																		\
	m_global_metadata = sad::ClassMetaDataContainer::ref()->get(#NAMEDCLASS, created);          \
	if (created)																				\
	{																							\
		m_global_metadata->addAncestor(#PARENT1);												\
		m_global_metadata->addAncestor(#PARENT2);												\
		m_global_metadata->addAncestor(#PARENT3);												\
		m_global_metadata->addAncestor(#PARENT4);												\
	}																							\
	return m_global_metadata;																	\
}																								\
sad::ClassMetaData * NAMEDCLASS ::metaData() const												\
{                                                                                               \
	return NAMEDCLASS ::globalMetaData();                                                       \
}   \
const sad::String &  NAMEDCLASS :: name() const  \
{                                                \
	return this-> PARENT1 :: name();              \
}



/*! Use this macro to define in source files, 
	that this object is inherited from descendant of sad::Object,
	where NAMEDCLASS should be name of current class and PARENT  - name of his parent class
	INDEX should be used as private Index in tables
 */
#define DECLARE_SOBJ_INHERITANCE_WITH_INDEX(NAMEDCLASS, PARENT, INDEX)	 \
sad::ClassMetaData * NAMEDCLASS ::m_global_metadata=NULL;	     \
sad::ClassMetaData * NAMEDCLASS ::globalMetaData()	  		 	 \
{																 \
	if (m_global_metadata != NULL) return m_global_metadata;     \
    bool created = false;																		\
	m_global_metadata = sad::ClassMetaDataContainer::ref()->get(#NAMEDCLASS, created);          \
	if (created)																				\
	{																							\
		m_global_metadata->addAncestor(#PARENT);											    \
		m_global_metadata->setPrivateIndex(INDEX);											    \
	}																							\
	return m_global_metadata;																	\
}																								\
sad::ClassMetaData * NAMEDCLASS ::metaData() const												\
{                                                                                               \
	return NAMEDCLASS ::globalMetaData();                                                       \
}                                                \
const sad::String &  NAMEDCLASS ::name() const   \
{                                                \
	return this-> PARENT :: name();              \
}