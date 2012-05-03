#ifndef __THE_STRING_H__
#define __THE_STRING_H__
//! From: STRING: VC98
#include <istream>
#include "basic_string.h"

// basic_string TEMPLATE OPERATORS
template<class _E, class _Tr, class _A> inline
basic_string<_E, _Tr, _A> __cdecl operator+(
											const basic_string<_E, _Tr, _A>& _L,
											const basic_string<_E, _Tr, _A>& _R)
{
	return (basic_string<_E, _Tr, _A>(_L) += _R); 
}
template<class _E, class _Tr, class _A> inline
basic_string<_E, _Tr, _A> __cdecl operator+(const _E *_L,
											const basic_string<_E, _Tr, _A>& _R)
{
	return (basic_string<_E, _Tr, _A>(_L) += _R); 
}
template<class _E, class _Tr, class _A> inline
basic_string<_E, _Tr, _A> __cdecl operator+(
											const _E _L, const basic_string<_E, _Tr, _A>& _R)
{return (basic_string<_E, _Tr, _A>(1, _L) += _R); }
template<class _E, class _Tr, class _A> inline
basic_string<_E, _Tr, _A> __cdecl operator+(
											const basic_string<_E, _Tr, _A>& _L,
											const _E *_R)
{return (basic_string<_E, _Tr, _A>(_L) += _R); }
template<class _E, class _Tr, class _A> inline
basic_string<_E, _Tr, _A> __cdecl operator+(
											const basic_string<_E, _Tr, _A>& _L, const _E _R)
{return (basic_string<_E, _Tr, _A>(_L) += _R); }
template<class _E, class _Tr, class _A> inline
bool __cdecl operator==(const basic_string<_E, _Tr, _A>& _L,
						const basic_string<_E, _Tr, _A>& _R)
{return (_L.compare(_R) == 0); }
template<class _E, class _Tr, class _A> inline
bool __cdecl operator==(const _E * _L,
						const basic_string<_E, _Tr, _A>& _R)
{return (_R.compare(_L) == 0); }
template<class _E, class _Tr, class _A> inline
bool __cdecl operator==(const basic_string<_E, _Tr, _A>& _L,
						const _E *_R)
{return (_L.compare(_R) == 0); }
template<class _E, class _Tr, class _A> inline
bool __cdecl operator!=(const basic_string<_E, _Tr, _A>& _L,
						const basic_string<_E, _Tr, _A>& _R)
{return (!(_L == _R)); }
template<class _E, class _Tr, class _A> inline
bool __cdecl operator!=(const _E *_L,
						const basic_string<_E, _Tr, _A>& _R)
{return (!(_L == _R)); }
template<class _E, class _Tr, class _A> inline
bool __cdecl operator!=(const basic_string<_E, _Tr, _A>& _L,
						const _E *_R)
{return (!(_L == _R)); }
template<class _E, class _Tr, class _A> inline
bool __cdecl operator<(const basic_string<_E, _Tr, _A>& _L,
					   const basic_string<_E, _Tr, _A>& _R)
{return (_L.compare(_R) < 0); }
template<class _E, class _Tr, class _A> inline
bool __cdecl operator<(const _E * _L,
					   const basic_string<_E, _Tr, _A>& _R)
{return (_R.compare(_L) > 0); }
template<class _E, class _Tr, class _A> inline
bool __cdecl operator<(const basic_string<_E, _Tr, _A>& _L,
					   const _E *_R)
{return (_L.compare(_R) < 0); }
template<class _E, class _Tr, class _A> inline
bool __cdecl operator>(const basic_string<_E, _Tr, _A>& _L,
					   const basic_string<_E, _Tr, _A>& _R)
{return (_R < _L); }
template<class _E, class _Tr, class _A> inline
bool __cdecl operator>(const _E * _L,
					   const basic_string<_E, _Tr, _A>& _R)
{return (_R < _L); }
template<class _E, class _Tr, class _A> inline
bool __cdecl operator>(const basic_string<_E, _Tr, _A>& _L,
					   const _E *_R)
{return (_R < _L); }
template<class _E, class _Tr, class _A> inline
bool __cdecl operator<=(const basic_string<_E, _Tr, _A>& _L,
						const basic_string<_E, _Tr, _A>& _R)
{return (!(_R < _L)); }
template<class _E, class _Tr, class _A> inline
bool __cdecl operator<=(const _E * _L,
						const basic_string<_E, _Tr, _A>& _R)
{return (!(_R < _L)); }
template<class _E, class _Tr, class _A> inline
bool __cdecl operator<=(const basic_string<_E, _Tr, _A>& _L,
						const _E *_R)
{return (!(_R < _L)); }
template<class _E, class _Tr, class _A> inline
bool __cdecl operator>=(const basic_string<_E, _Tr, _A>& _L,
						const basic_string<_E, _Tr, _A>& _R)
{return (!(_L < _R)); }
template<class _E, class _Tr, class _A> inline
bool __cdecl operator>=(const _E * _L,
						const basic_string<_E, _Tr, _A>& _R)
{return (!(_L < _R)); }
template<class _E, class _Tr, class _A> inline
bool __cdecl operator>=(const basic_string<_E, _Tr, _A>& _L,
						const _E *_R)
{return (!(_L < _R)); }
// basic_string INSERTERS AND EXTRACTORS
template<class _E, class _Tr, class _A> inline
basic_istream<_E, _Tr>& __cdecl operator>>(
										   basic_istream<_E, _Tr>& _I,
										   basic_string<_E, _Tr, _A>& _X)
{
	typedef ctype<_E> _Ctype;
	typedef basic_istream<_E, _Tr> _Myis;
	ios_base::iostate _St = ios_base::goodbit;
	bool _Chg = false;
	_X.erase();
	const _Myis::sentry _Ok(_I);
	
	if (_Ok)
	{
		const _Ctype& _Fac = _USE(_I.getloc(), _Ctype);
		_TRY_IO_BEGIN
			_A::size_type _N = 0 < _I.width() && _I.width() < _X.max_size() ? _I.width() : _X.max_size();
		_Tr::int_type _C = _I.rdbuf()->sgetc();
		for (; 0 < --_N; _C = _I.rdbuf()->snextc())
			if(_Tr::eq_int_type(_Tr::eof(), _C))
			{
				_St |= ios_base::eofbit;
				break; 
			}
			else if (_Fac.is(_Ctype::space, _Tr::to_char_type(_C)))
				break;
			else
			{
				_X.append(1, _Tr::to_char_type(_C));
				_Chg = true; 
			}
			_CATCH_IO_(_I); 
	}
	
	_I.width(0);
	if (!_Chg)
		_St |= ios_base::failbit;
	_I.setstate(_St);
	return (_I); 
}

template<class _E, class _Tr, class _A> inline
basic_istream<_E, _Tr>& __cdecl getline(basic_istream<_E, _Tr>& _I,
										basic_string<_E, _Tr, _A>& _X)
{
	return (getline(_I, _X, _I.widen('\n'))); 
}

template<class _E, class _Tr, class _A> inline
basic_istream<_E, _Tr>& __cdecl getline(basic_istream<_E, _Tr>& _I,
										basic_string<_E, _Tr, _A>& _X, const _E _D)
{
	typedef basic_istream<_E, _Tr> _Myis;
	ios_base::iostate _St = ios_base::goodbit;
	bool _Chg = false;
	_X.erase();
	const _Myis::sentry _Ok(_I, true);
	if (_Ok)
	{
		_TRY_IO_BEGIN
			_Tr::int_type _C = _I.rdbuf()->sgetc();
		for (; ; _C = _I.rdbuf()->snextc())
			if (_Tr::eq_int_type(_Tr::eof(), _C))
			{
				_St |= ios_base::eofbit;
				break; 
			}
			else if (_Tr::eq((_E)_C, _D))
			{
				_Chg = true;
				_I.rdbuf()->snextc();
				break; 
			}
			else if (_X.max_size() <= _X.size())
			{
				_St |= ios_base::failbit;
				break; 
			}
			else
				_X += _Tr::to_char_type(_C), _Chg = true;
			_CATCH_IO_(_I); 
	}
	if (!_Chg)
		_St |= ios_base::failbit;
	_I.setstate(_St);
	return (_I); 
}


template<class _E, class _Tr, class _A> inline
basic_ostream<_E, _Tr>& __cdecl operator<<(
										   basic_ostream<_E, _Tr>& _O,
										   const basic_string<_E, _Tr, _A>& _X)
{
	_O << _X.c_str();
	return (_O); }

#endif /* _STRING */

	/*
	* Copyright (c) 1994 by P.J. Plauger.  ALL RIGHTS RESERVED. 
	* Consult your license regarding permissions and restrictions.
*/
