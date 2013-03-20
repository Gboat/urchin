//
// Copyright(C) 2005--2006 Institute of Computing Tech, Chinese Academy of Sciences. 
// All rights reserved.
// This file is part of FirteX (www.firtex.org)
//
// Use of the FirteX is subject to the terms of the software license set forth in 
// the LICENSE file included with this software, and also available at
// http://www.firtex.org/license.html
//
// Author	: 郭瑞杰(GuoRuijie)
// Email	: ruijieguo@software.ict.ac.cn,ruijieguo@gmail.com
// Created	: 2006/6/3
//
#ifndef _BITVECTOR_H
#define _BITVECTOR_H


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../utility/StdHeader.h"
#include <stddef.h>
#include <limits.h>
#include <string>
#include <ostream>
#include <istream>
#include "../store/Directory.h"

using namespace firtex::store;
using namespace std;


namespace firtex
{
	namespace utility
	{
		const size_t NPOS = (size_t) -1;      // "SIZE_T_MAX"

		class CBitVector
		{
		public:
			CBitVector();
			CBitVector(CDirectory* pDirectory,const char* name);
			CBitVector(CIndexInput* pInput);
			CBitVector(size_t n);
			CBitVector(size_t n,uint32_t val);
			CBitVector(const string& s);
			CBitVector(const CBitVector& b);
			~CBitVector();

			// Conversions
			string toString() const;

			// Assignment
			CBitVector& operator=(const CBitVector& b);

			// Equality
			bool operator==(const CBitVector&) const;
			bool operator!=(const CBitVector&) const;

			// Basic bit operations
			CBitVector& set(size_t, bool = true);
			CBitVector& set();
			CBitVector& reset(size_t);
			CBitVector& reset();
			CBitVector& toggle(size_t);
			CBitVector& toggle();
			bool		test(size_t) const;
			bool		any() const;
			bool		none() const;
			bool		hasSmallThan(size_t val)const;
			bool		hasBiggerThan(size_t val)const;
			size_t		firstNotSet();
			size_t		firstSet();
			CBitVector	operator~() const;

			/**
			 * total set bits
			 */
			size_t		count();

			void		read(CDirectory* pDirectory,const char* name);
			void		write(CDirectory* pDirectory,const char* name);

			void		read(CIndexInput* pInput);
			void		write(CIndexOutput* pOutput);

			// Bitwise operators
			CBitVector& operator&=(const CBitVector& b);
			CBitVector& operator|=(const CBitVector& b);
			CBitVector& operator^=(const CBitVector& b);
			CBitVector& operator>>=(size_t n);
			CBitVector& operator<<=(size_t n);
			CBitVector	operator>>(size_t n) const;
			CBitVector	operator<<(size_t n) const;

			// String operations
			CBitVector& operator+=(const CBitVector&);
			CBitVector& insert(size_t pos, const CBitVector& b);
			CBitVector& remove(size_t pos, size_t n);
			CBitVector& replace(size_t pos, size_t n, const CBitVector& b);
			size_t		find(int val, size_t pos = 0) const;
			size_t		rfind(int val, size_t pos = NPOS) const;
			CBitVector	substr(size_t pos, size_t n) const;
			/**
			 * length of the vector
			 */
			size_t		length() const;

			/**
			 * 设置容量
			 * @param n 位个数，即容量
			 * @param val 初始值
			 */
			size_t		length(size_t n, bool val = 0);

			/**
			 * 裁减右边空白位
			 */
			size_t		trim();

		private:
			typedef uint32_t Block;

			Block*	m_bits;			///基数组
			size_t	m_nBits;		///位数
			size_t	m_nBlks;		///基数组中块数
			size_t	m_count;		///总置位数
			Block	m_cleanmask;	///用来隐藏不常使用的位

			enum {BLKSIZ = CHAR_BIT * sizeof(Block)};

			static Block word(size_t b)
			{return (Block)(b / BLKSIZ);}
			static Block offset(size_t b)
			{return (Block)(BLKSIZ - b%BLKSIZ - 1);}
			static Block mask1(size_t b)
			{return Block(1) << offset(b);}
			static Block mask0(size_t b)
			{return ~(Block(1) << offset(b));}
			static size_t nblks(size_t nb)
			{return (nb+BLKSIZ-1) / BLKSIZ;}

			void	make_cleanmask();
			void	cleanup();
			void	set_(size_t n);
			void	set_(size_t n, bool val);
			void	reset_(size_t b);
			bool	test_(size_t b) const;
			void	fromString(const string& s);
			void	init(size_t n);
			void	equalize(CBitVector& b);

			friend istream& operator>>(istream& is, CBitVector& b);
		};

		// Global Functions:
		ostream& operator<<(ostream& os, const CBitVector& b);
		istream& operator>>(istream& is, CBitVector& b);
		CBitVector operator& (const CBitVector& x, const CBitVector& y);
		CBitVector operator|(const CBitVector& x, const CBitVector& y);
		CBitVector operator^ (const CBitVector& x, const CBitVector& y);
		CBitVector operator+ (const CBitVector& x, const CBitVector& y);

		//////////////////////////////////////////////////////////////////////////
		// Inline publics:
		inline CBitVector::CBitVector()
		{
			init(0);
		}		
		inline CBitVector::CBitVector(size_t n)
		{
			init(n);
		}

		inline CBitVector::~CBitVector()
		{
			delete [] m_bits;
		}

		inline CBitVector& CBitVector::toggle(size_t pos)
		{
			if(pos < 0 || pos >= m_nBits)
				FIRTEX_THROW2(OUTOFRANGE_ERROR,"bool CBitVector::test(size_t pos) const");

			m_bits[word(pos)] ^= mask1(pos);
			m_count = NPOS;
			return *this;
		}

		inline bool CBitVector::test(size_t pos) const
		{
			if(pos < 0)
				FIRTEX_THROW2(OUTOFRANGE_ERROR,"bool CBitVector::test(size_t pos) const");
			if(pos >= m_nBits)
				return false;

			return test_(pos);
		}

		inline CBitVector CBitVector::operator~() const
		{
			CBitVector b(*this);
			b.toggle();
			return b;
		}

		inline bool CBitVector::operator!=(const CBitVector& b) const
		{
			return !operator==(b);
		}

		inline bool CBitVector::none() const
		{
			return !any();
		}

		inline size_t CBitVector::length() const
		{
			return m_nBits;
		}

		inline CBitVector
			operator&(const CBitVector& x, const CBitVector& y)
		{
			CBitVector b(x);
			return b &= y;
		}

		inline CBitVector
			operator|(const CBitVector& x, const CBitVector& y)
		{
			CBitVector b(x);
			return b |= y;
		}

		inline CBitVector
			operator^(const CBitVector& x, const CBitVector& y)
		{
			CBitVector b(y);
			return b ^= x;
		}

		inline CBitVector CBitVector::operator<<(size_t n) const
		{
			CBitVector r(*this);
			return r <<= n;
		}

		inline CBitVector CBitVector::operator>>(size_t n) const
		{
			CBitVector r(*this);
			return r >>= n;
		}

		inline CBitVector
			operator+(const CBitVector& b1, const CBitVector& b2)
		{
			CBitVector b(b1);
			return b.operator+=(b2);
		}

		// Inline privates:
		inline void CBitVector::make_cleanmask()
		{
			m_cleanmask = ~Block(0) << (m_nBlks * BLKSIZ - m_nBits);
		}

		inline void CBitVector::cleanup()
		{
			// Make sure unused bits don't get set
			if (m_nBits % BLKSIZ)
				m_bits[m_nBlks - 1] &= m_cleanmask;
		}
		inline void CBitVector::set_(size_t b)
		{
			m_bits[word(b)] |= mask1(b);
		}

		inline void CBitVector::reset_(size_t b)
		{
			m_bits[word(b)] &= mask0(b);
		}

		inline bool CBitVector::test_(size_t b) const
		{
			return !!(m_bits[word(b)] & mask1(b));
		}

	}
}

#endif
