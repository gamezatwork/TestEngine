#pragma once

namespace Misc
{
	// Helps with removing the qualifiers from a type
	//====== RemQual =======//

	template <class T>
	struct RemQual
	{
		typedef T ActualType;
	};
	template <class T>
	struct RemQual<const T>
	{
		typedef typename RemQual<T>::ActualType ActualType;
	};
	template <class T>
	struct RemQual<T&>
	{
		typedef typename RemQual<T>::ActualType ActualType;
	};
	template <class T>
	struct RemQual<const T&>
	{
		typedef typename RemQual<T>::ActualType ActualType;
	};
	template <class T>
	struct RemQual<T&&>
	{
		typedef typename RemQual<T>::ActualType ActualType;
	};
	template <class T>
	struct RemQual<const T&&>
	{
		typedef typename RemQual<T>::ActualType ActualType;
	};
	template <class T>
	struct RemQual<const T*>
	{
		typedef typename RemQual<T*>::ActualType ActualType;
	};


	//====== RemQual =======//
}

