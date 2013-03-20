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
#include "utility/BitVector.h"
#include "utility/Exception.h"
#include "store/IndexInput.h"
#include "store/IndexOutput.h"

using namespace firtex::utility;

namespace firtex
{
	namespace utility
	{
		// Public Functions:
		CBitVector::CBitVector( size_t n,uint32_t val)
		{
			if(n<0 || n>= NPOS)
				FIRTEX_THROW2(OUTOFRANGE_ERROR,"CBitVector::CBitVector( size_t n,uint32_t val)");		

			// val == 0 is easy...
			if (val == 0)
			{
				init(n);
				return;
			}

			// Find highest significant bit
			uint32_t temp = val;
			size_t high_bit = 0;
			size_t i;
			for (i = 0; temp; ++i)
			{
				if (temp & 1)
					high_bit = i;
				temp >>= 1;
			}

			// Determine m_nBits and construct
			m_nBits = high_bit + 1;
			if (m_nBits < n)
				m_nBits = n;
			init(m_nBits);

			// Store bit pattern
			for (i = 0; i < m_nBits; ++i)
			{
				if (val & 1)
					set_(i);
				val >>= 1;
			}
		}
		CBitVector::CBitVector(CDirectory* pDirectory,const char* name)
		{
			read(pDirectory,name);
		}
		CBitVector::CBitVector(CIndexInput* pInput)
		{
			read(pInput);
		}
		CBitVector::CBitVector(const string& s)
		{
			// Validate that s has only 0's and 1's
			size_t i;
			for (i = 0; i < s.length(); ++i)
			{
				char c = s.at(i);
				if (c != '0' && c != '1')
					break;
			}
			if(i != s.length())
				FIRTEX_THROW2(BAD_PARAMETER_ERROR,"CBitVector::CBitVector(const string& s)");

			fromString(s);
		}

		CBitVector::CBitVector(const CBitVector& b)
		{
			init(b.m_nBits);
			memcpy(m_bits,b.m_bits,m_nBlks * sizeof m_bits[0]);
		}

		string CBitVector::toString() const
		{
			char *s = new char[m_nBits+1];
			for (size_t i = 0; i < m_nBits; ++i)
				s[i] = '0' +test_(i);

			s[m_nBits] = '\0';
			string s2(s);
			delete [] s;
			return s2;
		}

		CBitVector& CBitVector::operator=(const CBitVector& b)
		{
			if (this != &b)
			{
				delete [] m_bits;
				init(b.m_nBits);
				memcpy(m_bits,b.m_bits,m_nBlks * sizeof m_bits[0]);
				m_count = b.m_count;
			}
			return *this;
		}

		bool CBitVector::operator==(const CBitVector& b) const
		{
			return (m_nBits == b.m_nBits) &&
				!memcmp(m_bits,b.m_bits,m_nBlks * sizeof m_bits[0]);
		}

		CBitVector& CBitVector::set()
		{
			memset(m_bits,~0u,m_nBlks* sizeof m_bits[0]);
			m_count = m_nBits;
			cleanup();
			return *this;
		}

		CBitVector& CBitVector::set(size_t pos, bool val)
		{
			if(pos < 0 || pos >= NPOS)
				FIRTEX_THROW2(OUTOFRANGE_ERROR,"CBitVector& CBitVector::set(size_t pos, bool val)");

			if (pos >= m_nBits)
				length(pos + 1); // Append

			set_(pos,val);
			m_count = NPOS;
			return *this;
		}

		CBitVector& CBitVector::reset(size_t pos)
		{
			if(pos < 0 || pos > m_nBits)
				FIRTEX_THROW2(OUTOFRANGE_ERROR,"CBitVector& CBitVector::reset(size_t pos)");

			if (pos == m_nBits)
				length(m_nBits + 1); // Append

			reset_(pos);
			m_count = NPOS;
			return *this;
		}

		CBitVector& CBitVector::reset()
		{
			memset(m_bits,0,m_nBlks * sizeof m_bits[0]);
			m_count = 0;
			return *this;
		}

		CBitVector& CBitVector::toggle()
		{
			size_t nw = m_nBlks;
			while (nw--)
				m_bits[nw] = ~m_bits[nw];
			cleanup();
			m_count = NPOS;
			return *this;
		}

		bool CBitVector::any() const
		{
			for (size_t i = 0; i < m_nBlks; ++i)
				if (m_bits[i])
					return true;
			return false;
		}
		bool CBitVector::hasSmallThan(size_t val)const
		{
			if(val >= m_nBits)
				return any();
			size_t nTestBlk = nblks(val);
			for (size_t i = 0; i <= nTestBlk; ++i)
				if (m_bits[i])
					return true;
			return false;
		}
		bool CBitVector::hasBiggerThan(size_t val)const
		{
			if(val >= m_nBits)
				return false;
			size_t nTestBlk = nblks(val);
			for (size_t i = nTestBlk; i < m_nBlks; ++i)
				if (m_bits[i])
					return true;
			return false;
		}
		size_t CBitVector::firstNotSet()
		{
			size_t i;	
			for (i = 0; i < m_nBits; ++i)
			{
				if (!test_(i))
					return i;
			}
			return i;
		}
		size_t CBitVector::firstSet()
		{
			size_t i ;
			for (i = 0; i < m_nBits; ++i)
			{
				if (test_(i))
					return i;
			}
			return i;
		}

		size_t CBitVector::count()
		{
			if(m_count == NPOS)
			{
				size_t sum = 0;
				for (size_t i = 0; i < m_nBits; ++i)
					if (test_(i))
						++sum;
				m_count = sum;
			}			
			return m_count;
		}

		CBitVector& CBitVector::operator&=(const CBitVector& b)
		{
			CBitVector rhs(b);

			equalize(rhs);
			for (size_t i = 0; i < m_nBlks;++i)
				m_bits[i] &= rhs.m_bits[i];
			m_count = NPOS;
			return *this;
		}

		CBitVector& CBitVector::operator|=(const CBitVector& b)
		{
			CBitVector rhs(b);

			equalize(rhs);
			for (size_t i = 0; i < m_nBlks; ++i)
				m_bits[i] |= rhs.m_bits[i];
			cleanup();
			m_count = NPOS;
			return *this;
		}

		CBitVector& CBitVector::operator^=(const CBitVector& b)
		{
			CBitVector rhs(b);

			equalize(rhs);
			for (size_t i = 0; i < m_nBlks; ++i)
				m_bits[i] ^= rhs.m_bits[i];
			cleanup();
			m_count = NPOS;
			return *this;
		}

		CBitVector& CBitVector::operator>>=(size_t n)
		{
			if (n >= m_nBits)
				reset();
			else
			{
				size_t i;
				for (i = m_nBits - 1; i >= n; --i)
					(void) set_(i,test_(i-n));
				for (i = 0; i < n;++i)
					reset_(i);
			}
			m_count = NPOS;
			return *this;
		}

		CBitVector& CBitVector::operator<<=(size_t n)
		{
			if (n >= m_nBits)
				reset();
			else
			{
				size_t i;
				for (i = 0; i < m_nBits - n; ++i)
					(void) set_(i,test_(i+n));
				for (i = m_nBits - n; i < m_nBits; ++i)
					reset_(i);
			}
			m_count = NPOS;
			return *this;
		}

		CBitVector& CBitVector::operator+=(const CBitVector& b)
		{
			if(m_nBits + b.m_nBits >= NPOS)
				FIRTEX_THROW2(OUTOFRANGE_ERROR,"CBitVector& CBitVector::operator+=(const CBitVector& b)");

			size_t oldlen = m_nBits;

			length(m_nBits + b.m_nBits);
			for (size_t i = 0; i < b.m_nBits; ++i)
				(void) set_(oldlen+i,b.test_(i));

			m_count = NPOS;
			return *this;
		}

		CBitVector& CBitVector::insert(size_t pos, const CBitVector& b)
		{
			if(pos < 0 || pos > m_nBits)
				FIRTEX_THROW2(OUTOFRANGE_ERROR,"CBitVector& CBitVector::insert(size_t pos, const CBitVector& b)");

			size_t oldlen = m_nBits;
			size_t newlen = m_nBits + b.m_nBits;

			// Grow to accommodate insertion
			length(newlen);

			// Move tail to right
			size_t i ;
			for (i = 0; i < oldlen - pos;++i)
				set_(newlen-i-1,test_(oldlen-i-1));

			// Replicate b in between

			for (i = 0; i < b.m_nBits;++i)
				set_(pos+i,b.test_(i));

			m_count = NPOS;
			return *this;
		}

		CBitVector& CBitVector::remove(size_t pos, size_t n)
		{			
			if(pos < 0 || pos >= m_nBits)
				FIRTEX_THROW2(OUTOFRANGE_ERROR,"CBitVector& CBitVector::remove(size_t pos, size_t n)");

			if (n > m_nBits - pos)
				n = m_nBits - pos;

			// Slide tail down to cover gap
			for (size_t i = 0; i < m_nBits - pos - n; ++i)
				set_(pos+i,test_(pos+n+i));

			// Shrink
			length(m_nBits - n);
			m_count = NPOS;
			return *this;
		}

		CBitVector& CBitVector::replace(size_t pos, size_t n,const CBitVector& b)
		{		

			if(pos < 0 || pos > m_nBits)
				FIRTEX_THROW2(OUTOFRANGE_ERROR,"CBitVector& CBitVector::replace(size_t pos, size_t n,const CBitVector& b)");

			if (n > m_nBits - pos)
				n = m_nBits - pos;

			size_t oldlen= m_nBits;
			size_t newlen = oldlen - n + b.m_nBits;
			ptrdiff_t diff = newlen - oldlen;

			// Adjust length and move tail as needed
			if (diff > 0)
			{
				// Grow
				length(newlen);
				for (size_t i = oldlen - 1; i >= pos + n; --i)
					(void) set_(i+diff,test_(i));
			}
			else if (diff < 0)
			{
				// Shrink
				for (size_t i = pos + n; i < oldlen; ++i)
					(void) set_(i+diff,test_(i));
				length(newlen);
			}

			// Copy b into place
			for (size_t i = 0; i < b.m_nBits;++i)
				(void) set_(pos+i,b.test_(i));

			m_count = NPOS;

			return *this;
		}

		size_t CBitVector::find(int val, size_t pos) const
		{			
			if(pos < 0 || pos >= m_nBits)
				FIRTEX_THROW2(OUTOFRANGE_ERROR,"size_t CBitVector::find(int val, size_t pos) const");

			for (size_t i = pos; i < m_nBits; ++i)
			{
				int t = test_(i);
				if (val && t || !val && !t)
					return i;
			}
			return NPOS;
		}

		size_t CBitVector::rfind(int val, size_t pos) const
		{			
			if(pos < 0 || pos >= m_nBits)
				FIRTEX_THROW2(OUTOFRANGE_ERROR,"size_t CBitVector::rfind(int val, size_t pos) const");

			for (size_t i = pos; i >= 0; --i)
			{
				int t = test_(i);
				if (val && t || !val && !t)
					return i;
			}
			return NPOS;
		}

		CBitVector CBitVector::substr(size_t pos, size_t n) const
		{			
			if(pos < 0 || pos > m_nBits)
				FIRTEX_THROW2(OUTOFRANGE_ERROR,"CBitVector CBitVector::substr(size_t pos, size_t n) const");

			if (n > m_nBits - pos)
				n = m_nBits - pos;

			CBitVector b(n,0);
			for (size_t i = 0; i < n; ++i)
				b.set_(i,test_(pos + i));
			return b;
		}

		size_t CBitVector::length(size_t n, bool val)
		{
			size_t oldlen = m_nBits;
			size_t nw1 = m_nBlks;
			size_t nw2 = nblks(n);

			// Alter the size of a CBitVector
			if (nw1 != nw2)
			{
				Block *newbits = new Block[nw2];
				size_t i ;
				for (i = 0; i < nw1 && i < nw2; ++i)
					newbits[i] = m_bits[i];

				delete [] m_bits;
				m_bits = newbits;

				for (i = nw1; i < nw2;++i)
					m_bits[i] = val ? ~Block(0) : Block(0);
				m_nBlks = nw2;
			}
			m_nBits = n;
			make_cleanmask();
			cleanup();
			m_count = NPOS;
			return oldlen;
		}

		size_t CBitVector::trim()
		{
			size_t i;
			for (i = m_nBits - 1; i >= 0; --i)
				if (test_(i))
					break;
			size_t newlen = i + 1;
			length(newlen);
			return newlen;
		}

		ostream& operator<<(ostream& os, const CBitVector& b)
		{
			os << b.toString();
			return os;
		}

		istream& operator>>(istream& is, CBitVector& b)
		{
			const size_t N = 256;
			char *buf = new char[N];
			char c;

			// Consume bit characters
			is >> ws;
			int i;
			for (i = 0; i < N && is.get(c); ++i)
			{
				if (c == '0' || c == '1')
					buf[i] = c;
				else
				{
					is.putback(c);
					break;
				}
			}
			buf[i] = '\0';

			if (i == 0)
				is.clear(ios::failbit);
			else
			{
				delete [] b.m_bits;
				b.fromString(string(buf));
			}

			delete buf;
			return is;
		}

		// Private Functions:
		void CBitVector::set_(size_t n, bool val)
		{
			if (val)
				set_(n);
			else
				reset_(n);
			m_count = NPOS;			
		}

		void CBitVector::fromString(const string& s)
		{
			// Assumes string contains only 0's and 1's
			init(s.length());
			for (size_t i = 0; i < s.length(); ++i)
				if (s.at(i) == '1')
					set_(i);
		}

		void CBitVector::init(size_t n)
		{
			m_nBits = n;			
			if (n > 0)
			{
				m_nBlks = nblks(n);
				m_bits = new Block[m_nBlks];
				memset(m_bits,0,m_nBlks * sizeof m_bits[0]);
				make_cleanmask();		
			}
			else
			{
				m_bits = 0;
				m_nBlks = 0;
				m_cleanmask = 0;
			}				
			m_count = NPOS;
		}

		void CBitVector::equalize(CBitVector& b)
		{
			// Make the smaller the size of the larger
			if (b.m_nBits < m_nBits)
				b.length(m_nBits);
			else if (m_nBits < b.m_nBits)
				length(b.m_nBits);
		}
		void CBitVector::read(CDirectory* pDirectory,const char* name)
		{
			CIndexInput* pInput = pDirectory->openInput(name);
			try
			{				
				m_nBits = (size_t)pInput->readInt();	//位数				
				init(m_nBits);
				m_count = (size_t)pInput->readInt();	//置位数
				pInput->read((char*)m_bits,m_nBlks*sizeof(Block));
			}
			catch (std::bad_alloc& ) 
			{
				delete pInput;
				FIRTEX_THROW2(OUTOFMEM_ERROR,"CBitVector::CBitVector(CDirectory* pDirectory,const char* name)");
			}
			catch (...) 
			{
				delete pInput;
				FIRTEX_THROW2(UNKNOWN_ERROR,"CBitVector::CBitVector(CDirectory* pDirectory,const char* name)");
			}		
			delete pInput;
		}
		void CBitVector::write(CDirectory* pDirectory,const char* name)
		{
			CIndexOutput* pOutput = pDirectory->createOutput(name);
			try
			{							
				pOutput->writeInt((int32_t)m_nBits);
				count();
				pOutput->writeInt((int32_t)m_count);
				pOutput->write((const char*)m_bits,m_nBlks*sizeof(Block));
			}			
			catch (const CFirteXException& e) 
			{
				delete pOutput;
				FIRTEX_RETHROW(e);				
			}		
			delete pOutput;
		}
		void CBitVector::read(CIndexInput* pInput)
		{
			try
			{				
				m_nBits = (size_t)pInput->readInt();	//位数				
				init(m_nBits);
				m_count = (size_t)pInput->readInt();	//置位数
				if(m_count > 0)
					pInput->read((char*)m_bits,m_nBlks*sizeof(Block));
			}
			catch (std::bad_alloc& ) 
			{				
				FIRTEX_THROW2(OUTOFMEM_ERROR,"CBitVector::CBitVector(CDirectory* pDirectory,const char* name)");
			}
			catch (...) 
			{				
				FIRTEX_THROW2(UNKNOWN_ERROR,"CBitVector::CBitVector(CDirectory* pDirectory,const char* name)");
			}		
		}
		void CBitVector::write(CIndexOutput* pOutput)
		{
			trim();
			pOutput->writeInt((int32_t)m_nBits);
			count();
			pOutput->writeInt((int32_t)m_count);
			pOutput->write((const char*)m_bits,m_nBlks*sizeof(Block));			
		}
	}
}


