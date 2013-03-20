//
// Copyright(C) 2005--2006 Institute of Computing Tech, Chinese Academy of Sciences. 
// All rights reserved.
// This file is part of FirteX (www.firtex.org)
//
// Use of the FirteX is subject to the terms of the software license set forth in 
// the LICENSE file included with this software, and also available at
// http://www.firtex.org/license.html
//
// Author	: ¹ùÈð½Ü(GuoRuijie)
// Email	: ruijieguo@software.ict.ac.cn,ruijieguo@gmail.com
// Created	: 2006/5/20
//
#ifndef _NUMBER_H
#define _NUMBER_H

#include <string>
#include <sstream>
using namespace std;

namespace firtex
{
	namespace utility
	{
		typedef uint8_t number_t;
		typedef struct _tagnumbervar
		{
			number_t nt;				//Number Type
			union
			{
				bool		bval;
				int8_t		i8val;
				uint8_t		ui8val;
				int16_t		i16val;
				uint16_t	ui16val;
				int32_t		i32val;
				uint32_t	ui32val;
				int64_t		i64val;
				uint64_t	ui64val;

				float		fval;
				double		dbval;				
			}value;
		}numbervar;
		
		class CNumber : public _tagnumbervar
		{
		public:			
			const static int NUL	= 0;
			const static int BOOL	= 1;
			const static int INT8	= 2;
			const static int UINT8	= 3;
			const static int INT16	= 4;
			const static int UINT16	= 5;
			const static int INT32	= 6;
			const static int UINT32	= 7;
			const static int INT64	= 8;
			const static int UINT64	= 9;
			
			const static int FLOAT	= 10;
			const static int DOUBLE	= 11;

		public:
			CNumber();	
			CNumber(bool bv);
			CNumber(int8_t i8v);
			CNumber(uint8_t ui8v);
			CNumber(int16_t i16v);
			CNumber(uint16_t ui16v);
			CNumber(int32_t i32v);
			CNumber(uint32_t ui32v);
			CNumber(int64_t i64v);
			CNumber(uint64_t ui64v);
			CNumber(float f);
			CNumber(double d);
			CNumber(const numbervar& src);
			CNumber(const CNumber& clone);
			~CNumber(void);
		public:
			tstring toString();
		public:
			static int32_t toInt32(const tchar* str);
			static int64_t toInt64(const tchar* str);			
		public:
			operator bool() const ;                            // Extracts a bool from a CNumber::BOOL
			operator int8_t() const ;                          // Extracts a int8_t from a CNumber::INT8
			operator uint8_t() const ;                         // Extracts a uint8_t from a CNumber::UINT8
			operator int16_t() const ;                         // Extracts a int16_t from a CNumber::INT16
			operator uint16_t() const ;                        // Extracts a uint16_t from a CNumber::UINT8
			operator int32_t() const ;                         // Extracts a int32_t from a CNumber::INT32
			operator uint32_t() const ;                        // Extracts a uint32_t from a CNumber::UINT32
			operator int64_t() const ;                         // Extracts a int64_t from a CNumber::INT64
			operator uint64_t() const ;                        // Extracts a uint64_t from a CNumber::UINT64
			operator float() const ;                           // Extracts a float from a VT_R4
			operator double() const ;                          // Extracts a double from a VT_R8		

			CNumber& operator=(bool bSrc) ;						// Assign a VT_BOOL
			CNumber& operator=(int8_t i8Src) ;					// Assign a VT_I1
			CNumber& operator=(uint8_t ui8Src) ;				// Assign a VT_UI1
			CNumber& operator=(int16_t i8Src) ;					// Assign a VT_I2
			CNumber& operator=(uint16_t ui16Src) ;				// Assign a VT_UI2
			CNumber& operator=(int32_t i32Src) ;				// Assign a VT_I4
			CNumber& operator=(uint32_t ui32Src) ;				// Assign a VT_UI4
			CNumber& operator=(int64_t i64Src) ;				// Assign a VT_I8
			CNumber& operator=(uint64_t ui64Src) ;				// Assign a VT_UI8
			CNumber& operator=(float fltSrc) ;					// Assign a VT_R4
			CNumber& operator=(double dblSrc) ;					// Assign a VT_R8, or a VT_DATE			

			bool operator==(const numbervar& varSrc) const throw();
			bool operator==(const numbervar* pSrc) const throw();

			bool operator!=(const numbervar& varSrc) const throw();
			bool operator!=(const numbervar* pSrc) const throw();

			bool operator!=(const CNumber& pSrc) const throw();
		};
		//////////////////////////////////////////////////////////////////////////
		//inline functions	
		inline CNumber::CNumber()
		{
			nt = CNumber::NUL;
		}
		inline CNumber::CNumber(bool bv)
		{
			nt = CNumber::BOOL;
			value.bval = bv;
		}
		inline CNumber::CNumber(int8_t i8v)
		{
			nt = CNumber::INT8;
			value.i8val = i8v;
		}
		inline CNumber::CNumber(uint8_t ui8v)
		{
			nt = CNumber::UINT8;
			value.ui8val = ui8v;
		}
		inline CNumber::CNumber(int16_t i16v)
		{
			nt = CNumber::INT16;
			value.i16val = i16v;
		}
		inline CNumber::CNumber(uint16_t ui16v)
		{
			nt = CNumber::UINT16;
			value.ui16val = ui16v;
		}
		inline CNumber::CNumber(int32_t i32v)
		{
			nt = CNumber::INT32;
			value.i32val = i32v;
		}
		inline CNumber::CNumber(uint32_t ui32v)
		{
			nt = CNumber::UINT32;
			value.ui32val = ui32v;
		}
		inline CNumber::CNumber(int64_t i64v)
		{
			nt = CNumber::INT64;
			value.i64val = i64v;
		}
		inline CNumber::CNumber(uint64_t ui64v)
		{
			nt = CNumber::UINT64;
			value.ui64val = ui64v;
		}
		inline CNumber::CNumber(float f)
		{
			nt = CNumber::FLOAT;
			value.fval = f;
		}
		inline CNumber::CNumber(double d)
		{
			nt = CNumber::DOUBLE;
			value.dbval = d;
		}
		inline CNumber::CNumber(const numbervar& src)
		{
			nt = src.nt;
			value = src.value;
		}
		inline CNumber::CNumber(const CNumber& clone)
		{
			nt = clone.nt;
			value = clone.value;
		}
		inline CNumber::~CNumber(void){}
	
		inline tstring CNumber::toString()
		{			
			stringstream ss;
			switch(nt)
			{
			case CNumber::BOOL:
				return (value.bval?_T("true"):_T("false"));
				break;
			case CNumber::INT8:
				ss << value.i8val;
				break;
			case CNumber::UINT8:
				ss << value.ui8val;				
				break;
			case CNumber::INT16:
				ss << value.i16val;				
				break;
			case CNumber::UINT16:
				ss << value.ui16val;				
				break;
			case CNumber::INT32:
				ss << value.i32val;				
				break;
			case CNumber::UINT32:
				ss << value.ui32val;				
				break;
			case CNumber::INT64:
				ss << value.i64val;				
				break;
			case CNumber::UINT64:
				ss << value.ui64val;				
				break;
			case CNumber::FLOAT:
				ss << value.fval;				
				break;
			case CNumber::DOUBLE:
				ss << value.dbval;				
				break;
			}			
			return ss.str();
		}	

		inline CNumber::operator bool() const 
		{
			return value.bval;
		}
		inline CNumber::operator int8_t() const 
		{
			return value.i8val;
		}
		inline CNumber::operator uint8_t() const 
		{
			return value.ui8val;
		}
		inline CNumber::operator int16_t() const
		{
			return value.i16val;
		}
		inline CNumber::operator uint16_t() const
		{
			return value.ui16val;
		}
		inline CNumber::operator int32_t() const
		{
			return value.i32val;
		}
		inline CNumber::operator uint32_t() const
		{
			return value.ui32val;
		}
		inline CNumber::operator int64_t() const
		{
			return value.i64val;
		}
		inline CNumber::operator uint64_t() const
		{
			return value.ui64val;
		}
		inline CNumber::operator float() const
		{
			return value.fval;
		}
		inline CNumber::operator double() const
		{
			return value.dbval;
		}
		
		//////////////////////////////////////////////////////////////////////////

		inline CNumber& CNumber::operator=(bool bSrc)
		{
			nt = CNumber::BOOL;
			value.bval = bSrc;
			return *this;
		}
		inline CNumber& CNumber::operator=(int8_t i8Src)
		{
			nt = CNumber::INT8;
			value.i8val = i8Src;
			return *this;
		}
		inline CNumber& CNumber::operator=(uint8_t ui8Src)
		{
			nt = CNumber::UINT8;
			value.ui8val;
			return *this;
		}
		inline CNumber& CNumber::operator=(int16_t i8Src)
		{
			nt = CNumber::INT16;
			value.i16val;
			return *this;
		}
		inline CNumber& CNumber::operator=(uint16_t ui16Src)
		{
			nt = CNumber::UINT16;
			value.ui16val = ui16Src;
			return *this;
		}
		inline CNumber& CNumber::operator=(int32_t i32Src)
		{
			nt = CNumber::INT32;
			value.i32val = i32Src;
			return *this;
		}
		inline CNumber& CNumber::operator=(uint32_t ui32Src)
		{
			nt = CNumber::UINT32;
			value.ui32val = ui32Src;
			return *this;
		}
		inline CNumber& CNumber::operator=(int64_t i64Src)
		{
			nt = CNumber::INT64;
			value.i64val = i64Src;
			return *this;
		}
		inline CNumber& CNumber::operator=(uint64_t ui64Src)
		{
			nt = CNumber::UINT64;
			value.ui64val = ui64Src;
			return *this;
		}
		inline CNumber& CNumber::operator=(float fltSrc)
		{
			nt = CNumber::FLOAT;
			value.fval = fltSrc;
			return *this;
		}
		inline CNumber& CNumber::operator=(double dblSrc)
		{
			nt = CNumber::DOUBLE;
			value.dbval = dblSrc;
			return *this;
		}

		//////////////////////////////////////////////////////////////////////////

		inline bool CNumber::operator==(const numbervar& varSrc) const throw()
		{
			 return *this == &varSrc;
		}
		inline bool CNumber::operator==(const numbervar* pSrc) const throw()
		{
			if(!pSrc)
				return false;
			if(this == pSrc)
				return true;
			if(nt != pSrc->nt)
				return false;
			switch(nt)
			{
			case CNumber::BOOL:
				return (value.bval == pSrc->value.bval);
				break;
			case CNumber::INT8:
				return (value.i8val == pSrc->value.i8val);
				break;
			case CNumber::UINT8:
				return (value.ui8val == pSrc->value.ui8val);
				break;
			case CNumber::INT16:
				return (value.i16val == pSrc->value.i16val);
				break;
			case CNumber::UINT16:
				return (value.ui16val == pSrc->value.ui16val);
				break;
			case CNumber::INT32:
				return (value.i32val == pSrc->value.i32val);
				break;
			case CNumber::UINT32:
				return (value.ui32val == pSrc->value.ui32val);
				break;
			case CNumber::INT64:
				return (value.i64val == pSrc->value.i64val);
				break;
			case CNumber::UINT64:
				return (value.ui64val == pSrc->value.ui64val);
				break;
			case CNumber::FLOAT:
				return (value.fval == pSrc->value.fval);
				break;
			case CNumber::DOUBLE:
				return (value.dbval == pSrc->value.dbval);
				break;
			}
			return true;
		}

		inline bool CNumber::operator!=(const numbervar& varSrc) const throw()
		{
			return !(this == &varSrc);
		}
		inline bool CNumber::operator!=(const numbervar* pSrc) const throw()
		{
			return !(*this == pSrc);
		}		

		inline int32_t CNumber::toInt32(const tchar* str)
		{
			try
			{
				return (int32_t)_ttoi(str);
			}
			catch (...)
			{		
				FIRTEX_THROW3(NUMBER_FORMAT_ERROR,_T("CNumber::toInt32"));
			}
		}
		inline int64_t CNumber::toInt64(const tchar* str)
		{
			try
			{
				return (int64_t)_ttoi64(str);
			}
			catch (...)
			{		
				FIRTEX_THROW3(NUMBER_FORMAT_ERROR,_T("CNumber::toInt64"));
			}
		}
	}
}

#endif

