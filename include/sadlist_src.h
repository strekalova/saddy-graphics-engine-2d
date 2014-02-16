#pragma once
#include <malloc.h>
#include <stdio.h>


namespace sad
{

template<class T> void List<T>::makeClear()
{
	if (_p!=NULL)
	{
      long i;
	  for (i=0;i<sz;i++)
		  delete _p[i];
	  free(_p);
	  _p=NULL;
	  sz=0;
	}
}
template<class T> void List<T>::clear () {makeClear();}
template<class T> bool List<T>::operator!=(const List<T> & h)
{
	if (sz!=h.sz)
		return true;
    long i;
	for (i=0;i<sz;i++)
		if ((*this)[i]!=h[i])
			return true;
	return false;
}
template<class T> bool List<T>::operator==(const List<T> & h)
{
	return !(*this!=h);
}
template<class T> void List<T>::copy(const List<T> & h)
{
  if (h.sz!=0)
  {
	  long i;
	  sz=h.sz;
	  _p=(T**)malloc(h.sz*sizeof(T*));
	  for (i=0;i<sz;i++)
		  _p[i]=new T(h[i]);
  }
  else
  {
	_p=0;sz=0;
  }
}

template<class T> List<T>::List()
{
	_p=0;
	sz=0;
}
template<class T> List<T>::~List()
{
	makeClear();
}
template<class T> List<T>::List(const List<T> & h)
{
	copy(h);
}
template<class T> List<T> & List<T>::operator=(const List<T> & h)
{
 if (h._p!=_p)
 {
    makeClear();
	copy(h);
 }
 return *this;
}
template<class T> bool List<T>::empty() const {return sz==0;}
template<class T> long List<T>::count() const {return sz;}
template<class T> const T** List<T>::data()  const {return _p;}


template<class T> T&   List<T>::operator[](long i)
{
	if (i<0 || i>=sz) return *(new T());
	return *(_p[i]);
}
template<class T> const T&   List<T>::operator[](long i) const
{
	if (i<0 || i>=sz) return *(new T());
	return *(_p[i]);
}
template<class T> List<T> & List<T>::operator<<(const T & obj)
{
	_p=(T**)realloc(_p,(++sz)*sizeof(T*));
	_p[sz-1]=new T(obj);
	return *this;
}
template<class T> List<T> & List<T>::operator<<(const List<T> & obj)
{
    sz+=obj.sz;
	_p=(T**)realloc(_p,sz*sizeof(T*));
    long li=sz-1;
	for (long i=obj.sz-1;i>-1;i--)
	{
		_p[li]=new T(obj[i]);
        --li;
	}
	return *this;
}
template<class T> List<T>  & List<T>::add(const List<T> & o)
{
	return (*this)<<o;
}
template<class T> List<T>  & List<T>::add(const T & obj)
{
	return (*this)<<obj;
}
template<class T> List<T> & List<T>::insert(const T &obj,long pos)
{
   if (pos>=sz) return (*this)<<obj;
   if (pos<0) pos=0;
   //puts("Inner insertion in List");
   _p=(T**)realloc(_p,(++sz)*sizeof(T*));
   //printf("%p,%ld",_p,sz);
   for (long i=sz-1;i>pos;i--)
   {
       //printf("Working with %ld and %ld",i,i-1);
       _p[i]=_p[i-1];
   }
   _p[pos]=new T(obj);
   return *this;
}
template<class T> List<T> & List<T>::addFront(const T & obj)
{
	return insert(obj,0);
}
template<class T> List<T> & List<T>::removeAt(long i)
{
	if (i<0 || i>=sz) return *this;
    if (sz!=1)
	{
	  delete _p[i];
      for (long j=i;j<sz-1;j++)
		  _p[j]=_p[j+1];
	  _p=(T**)realloc(_p,(--sz)*sizeof(T*));
	}
	else
	{
		delete _p[0];
		free(_p);
        _p=NULL;
		sz=0;
	}
  return *this;
}
template<class T> List<T> & List<T>::remove(const T & obj)
{
	for (long i=0;i<count();i++)
	{
		if ( (*this)[i]==obj )
		{
			removeAt(i);
			--i;
		}
	}
	return *this;
}
template<class T> List<T> & List<T>::removeFirst(const T & obj)
{
	for (long i=0;i<count();i++)
	{
		if ( (*this)[i]==obj )
		{
			removeAt(i);
			return *this;
		}
	}
	return *this;
}
template<class T> List<T> & List<T>::removeLast(const T & obj)
{
  long pos=-1;
  for (long i=0;i<count();i++)
  {
		if ( (*this)[i]==obj )
		{
			pos=i;
		}
  }
  if (pos!=-1)
	  removeAt(pos);
  return *this;
}
template<class T> List<T> & List<T>::operator>>(T & obj)
{
	if (count()!=0)
	{
		obj=(*this)[sz-1];
		removeAt(sz-1);
	}
	return *this;
}
template<class T> List<T> & List<T>::removeRange(long imin,long imax)
{
  long xmin,xmax;
  xmin=(imin<imax)?( xmax=imax,imin ):( xmin=imin,imax );
  long i=xmin;
  do
  {
	  removeAt(xmin);
	  ++i;
  }
  while (i!=xmax);
}

}