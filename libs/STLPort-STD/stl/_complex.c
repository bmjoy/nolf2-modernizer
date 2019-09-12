/*
 * Copyright (c) 1999
 * Silicon Graphics Computer Systems, Inc.
 *
 * Copyright (c) 1999 
 * Boris Fomitchev
 *
 * This material is provided "as is", with absolutely no warranty expressed
 * or implied. Any use is at your own risk.
 *
 * Permission to use or copy this software for any purpose is hereby granted 
 * without fee, provided the above notices are retained on all copies.
 * Permission to modify the code and to distribute modified code is granted,
 * provided the above notices are retained, and a notice that the code was
 * modified is included with the above copyright notice.
 *
 */ 
#ifndef __STL_COMPLEX_C
#define __STL_COMPLEX_C

__STL_BEGIN_NAMESPACE

// Non-inline member functions.

template <class _Tp>
void complex<_Tp>::_div(const _Tp& __z1_r, const _Tp& __z1_i,
                        const _Tp& __z2_r, const _Tp& __z2_i,
                        _Tp& __res_r, _Tp& __res_i) {
  _Tp __ar = __z2_r >= 0 ? __z2_r : -__z2_r;
  _Tp __ai = __z2_i >= 0 ? __z2_i : -__z2_i;

  if (__ar <= __ai) {
    _Tp __ratio = __z2_r / __z2_i;
    _Tp __denom = __z2_i * (1 + __ratio * __ratio);
    __res_r = (__z1_r * __ratio + __z1_i) / __denom;
    __res_i = (__z1_i * __ratio - __z1_r) / __denom;
  }
  else {
    _Tp __ratio = __z2_i / __z2_r;
    _Tp __denom = __z2_r * (1 + __ratio * __ratio);
    __res_r = (__z1_r + __z1_i * __ratio) / __denom;
    __res_i = (__z1_i - __z1_r * __ratio) / __denom;
  }
}

template <class _Tp>
void complex<_Tp>::_div(const _Tp& __z1_r,
                        const _Tp& __z2_r, const _Tp& __z2_i,
                        _Tp& __res_r, _Tp& __res_i) {
  _Tp __ar = __z2_r >= 0 ? __z2_r : -__z2_r;
  _Tp __ai = __z2_i >= 0 ? __z2_i : -__z2_i;

  if (__ar <= __ai) {
    _Tp __ratio = __z2_r / __z2_i;
    _Tp __denom = __z2_i * (1 + __ratio * __ratio);
    __res_r = (__z1_r * __ratio) / __denom;
    __res_i = - __z1_r / __denom;
  }
  else {
    _Tp __ratio = __z2_i / __z2_r;
    _Tp __denom = __z2_r * (1 + __ratio * __ratio);
    __res_r = __z1_r / __denom;
    __res_i = - (__z1_r * __ratio) / __denom;
  }
}

// I/O.

#ifdef __STL_USE_NEW_IOSTREAMS

// Complex output, in the form (re,im).  We use a two-step process 
// involving stringstream so that we get the padding right.  
template <class _Tp, class _CharT, class _Traits>
basic_ostream<_CharT, _Traits>& __STL_CALL
operator<<(basic_ostream<_CharT, _Traits>& __os, const complex<_Tp>& __z)
{
  basic_ostringstream<_CharT, _Traits, allocator<_CharT> > __tmp;
  __tmp.flags(__os.flags());
  __tmp.imbue(__os.getloc());
  __tmp.precision(__os.precision());
  __tmp << '(' << __z.real() << ',' << __z.imag() << ')';
  return __os << __tmp.str();
}

// Complex input from arbitrary streams.  Note that results in some
// locales may be confusing, since the decimal character varies with
// locale and the separator between real and imaginary parts does not.

template <class _Tp, class _CharT, class _Traits>
basic_istream<_CharT, _Traits>& __STL_CALL
operator>>(basic_istream<_CharT, _Traits>& __is, complex<_Tp>& __z)
{
  _Tp  __re = 0;
  _Tp  __im = 0;

  typedef ctype<_CharT> _Ctype;
  locale __loc = __is.getloc();
  const _Ctype&  __c_type  = use_facet<_Ctype>(__loc);

  char   __punct[4] = "(,)";
  _CharT __wpunct[3];
  __c_type.widen(__punct, __punct + 3, __wpunct);

  _CharT __c;

  __is >> __c;
  if (_Traits::eq(__c, __wpunct[0])) {  // Left paren
    __is >> __re >> __c;
    if (_Traits::eq(__c, __wpunct[1]))  // Comma
      __is >> __im >> __c;
    if (!_Traits::eq(__c, __wpunct[2])) // Right paren
      __is.setstate(ios_base::failbit);
  }
  else {
    __is.putback(__c);
    __is >> __re;
  }

  if (__is)
    __z = complex<_Tp>(__re, __im);
  return __is;
}


#else /* __STL_USE_NEW_IOSTREAMS */

template <class _Tp>
ostream& __STL_CALL operator<<(ostream& s, const complex<_Tp>& __z)
{
  return s << "( " << __z._M_re <<", " << __z._M_im <<")";
}

template <class _Tp>
istream& __STL_CALL operator>>(istream& s, complex<_Tp>& a)
{
  _Tp re = 0, im = 0;
  char 	c = 0;

  s >> c;
  if (c == '(') {
    s >> re >> c;
    if (c == ',') s >> im >> c;
    if (c != ')') s.clear(ios::badbit);
  }
  else {
    s.putback(c);
    s >> re;
  }

  if (s) a = complex<_Tp>(re, im);
  return s;
}

#endif /* __STL_USE_NEW_IOSTREAMS */

__STL_END_NAMESPACE

#endif /* __STL_COMPLEX_C */
