//
// Copyright(C) 2005--2006 Institute of Computing Tech, Chinese Academy of Sciences. 
// All rights reserved.
// This file is part of FirteX (www.firtex.org)
//
// Use of the FirteX is subject to the terms of the software license set forth in 
// the LICENSE file included with this software, and also available at
// http://www.firtex.org/license.html
//
// Author	: �����(GuoRuijie)
// Email	: ruijieguo@software.ict.ac.cn,ruijieguo@gmail.com
// Created	: 2005/11/20
//
#ifndef _FIELD_H
#define _FIELD_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>
#include "../utility/StdHeader.h"
#include "../utility/Reader.h"
#include "../analyzer/Tokens.h"
#include "../utility/Number.h"
#include "../utility/FXString.h"
#include "../utility/DateTime.h"

using namespace std;
using namespace firtex::utility;
using namespace firtex::analyzer;


//CField bits define (16Byte)
///Index
#define BIT_INDEX_MASK					0x01	///����
#define BIT_ANALYZE_MASK				0x02	///����

///Store
#define BIT_STORE_MASK					0x04	///�洢
#define BIT_COMPRESS_MASK				0x08	///ѹ���洢

///Forward Index
#define BIT_FORWARDINDEX_SEQUENCE_MASK	0x10	///ԭʼ����
#define BIT_FORWARDINDEX_FREQ_MASK		0x20	///����λ��(ֻ���,��Ƶ)
#define BIT_FORWARDINDEX_POSITION_MASK	0x40	///��λ��(���,��Ƶ,λ��)


///----------------------------------------------------------------------//
///				------------------------------------------------------
/// fieldflag_t = |8Byte field type|8Byte token type|16Byte field bits|
///				------------------------------------------------------
///----------------------------------------------------------------------//
#define FIELDTYPE(flag) (field_t)((flag)>>24)
#define TOKENTYPE(flag) (token_t)((flag)>>16)
#define FIELDBITS(flag) (fieldbits_t)(flag)
#define MAKEFIELDFLAG(fieldtype,tokentype,bits) (fieldflag_t)( (((fieldflag_t)(fieldtype))<<24) | (((fieldflag_t)(tokentype)) << 16) | (bits) )

namespace firtex
{
	namespace document
	{		
		typedef unsigned int method_type;
		typedef method_type store_t;
		typedef method_type index_t;
		typedef method_type forwardindex_t;
		#define DEFINE_METHOD(cat,mt,value) namespace cat { const method_type mt = (method_type)(value);}

		DEFINE_METHOD(Index,NO,1)
		DEFINE_METHOD(Index,ANALYZE,2)
		DEFINE_METHOD(Index,UN_ANALYZE,3)

		DEFINE_METHOD(Store,YES,1)
		DEFINE_METHOD(Store,COMPRESS,2)
		DEFINE_METHOD(Store,NO,3)

		DEFINE_METHOD(ForwardIndex,NO,1)		
		DEFINE_METHOD(ForwardIndex,SEQUENCE,2)			///�洢�ĵ�Analyze���ԭʼ����
		DEFINE_METHOD(ForwardIndex,FREQ,3)				///���洢λ����Ϣ�����洢�ʺʹ�Ƶ��Ϣ
		DEFINE_METHOD(ForwardIndex,POSITION,4)			///�洢�ʣ���Ƶ����λ����Ϣ

		
		namespace Index
		{
			static index_t nameToType(const tchar* szName)
			{
				if(!_tcsicmp(szName,_T("no")))
					return Index::NO;
				if(!_tcsicmp(szName,_T("analyze")))
					return Index::ANALYZE;
				if(!_tcsicmp(szName,_T("unanalyze")))
					return Index::UN_ANALYZE;
				
				return Index::NO;
			}
			static tstring typeToName(index_t t)
			{
				if(t == Index::NO)
					return _T("no");
				if(t == Index::ANALYZE)
					return _T("analyze");
				if(t == Index::UN_ANALYZE)
					return _T("unanalyze");
			
				return _T("no");
			}
		}

		namespace Store
		{
			static store_t nameToType(const tchar* szName)
			{
				if(!_tcsicmp(szName,_T("no")))
					return Store::NO;
				if(!_tcsicmp(szName,_T("yes")))
					return Store::YES;
				if(!_tcsicmp(szName,_T("compress")))
					return Store::COMPRESS;
				
				return Store::NO;
			}
			static tstring typeToName(store_t t)
			{
				if(t == Store::NO)
					return _T("no");
				if(t == Store::YES)
					return _T("yes");
				if(t == Store::COMPRESS)
					return _T("compress");
				
				return _T("no");
			}
		}

		namespace ForwardIndex
		{
			static forwardindex_t nameToType(const tchar* szName)
			{
				if(!_tcsicmp(szName,_T("no")))
					return ForwardIndex::NO;
				if(!_tcsicmp(szName,_T("sequence")))
					return ForwardIndex::SEQUENCE;
				if(!_tcsicmp(szName,_T("freq")))
					return ForwardIndex::FREQ;
				if(!_tcsicmp(szName,_T("position")))
					return ForwardIndex::POSITION;
				return ForwardIndex::NO;
			}
			static tstring typeToName(forwardindex_t t)
			{
				if(t == ForwardIndex::NO)
					return _T("no");
				if(t == ForwardIndex::FREQ)
					return _T("freq");
				if(t == ForwardIndex::SEQUENCE)
					return _T("sequence");
				if(t == ForwardIndex::POSITION)
					return _T("position");
				return _T("no");
			}
		}
		
		typedef uint32_t fieldflag_t;		
		typedef uint16_t fieldbits_t;
		typedef uint8_t field_t;
		
		//////////////////////////////////////////////////////////////////////////
		///CField		
		class CField
		{			
		public:
			/** 
			 * ����һ���ַ������͵Ĺؼ����ֶΣ����ֶ��������洢��������,��ǰ������
			 * @param name �ֶ���
			 * @param value �ַ���ֵ
			 * @return �ֶζ���
			 */
			static CField* Keyword(const tchar* name, const char* value,float fBoost = 1.0) 
			{
				return new CField(name, value,Store::YES,Index::UN_ANALYZE,ForwardIndex::NO,fBoost);
			}

			/** 
			 * ����һ���ַ������͵Ĺؼ����ֶΣ����ֶ��������洢����������ǰ������
			 * @param name �ֶ���
			 * @param value �ַ���ֵ
			 * @return �ֶζ���
			 */
			static CField* Keyword(const tchar* name, const wchar_t* value,float fBoost = 1.0) 
			{
				return new CField(name, value,Store::YES,Index::UN_ANALYZE,ForwardIndex::NO,fBoost);
			}

			/**
			 * ����һ��ʱ���������͵Ĺؼ����ֶΣ����ֶ��������洢����������ǰ������
			 * @param name �ֶ���
			 * @param value ʱ������ֵ
			 * @return �ֶζ���
			 */
			static CField* Keyword(const tchar* name, const CDateTime& value,float fBoost = 1.0)
			{
				numbervar nvar;
				nvar.value.i64val = value.getTime();
				nvar.nt = CNumber::INT64;
				return new CField(name, nvar,Store::YES,Index::UN_ANALYZE,fBoost);
			}

			/**
			 * ����һ��ʱ���������͵Ĺؼ����ֶΣ����ֶ��������洢����������ǰ������
			 * @param name �ֶ���
			 * @param value ʱ������ֵ
			 * @return �ֶζ���
			 */
			static CField* Keyword(const tchar* name, const numbervar& value,float fBoost = 1.0)
			{			
				return new CField(name, value,Store::YES,Index::UN_ANALYZE,fBoost);
			}

			/** 
			 * ����һ���ַ������͵ķ������ֶΣ����ֶδ洢������������������ǰ������
			 * @param name �ֶ���
			 * @param value �ַ���ֵ
			 * @return �ֶζ���
			 */
			static CField* UnIndexed(const tchar* name, const char* value,float fBoost = 1.0) 
			{
				return new CField(name, value, Store::YES,Index::NO,ForwardIndex::NO,fBoost);
			}

			/** 
			 * ����һ���ַ������͵ķ������ֶΣ����ֶδ洢������������������ǰ������
			 * @param name �ֶ���
			 * @param value �ַ���ֵ
			 * @return �ֶζ���
			 */
			static CField* UnIndexed(const tchar* name, const wchar_t* value,float fBoost = 1.0) 
			{
				return new CField(name, value, Store::YES,Index::NO,ForwardIndex::NO,fBoost);
			}

			/** 
			 * ����һ��reader���͵ķ������ֶΣ����ֶδ洢������������������ǰ������
			 * @param name �ֶ���
			 * @param value readerֵ
			 * @param bDelete �Ƿ���Ҫ���ڲ�ɾ��value����
			 * @return �ֶζ���
			 */
			static CField* UnIndexed(const tchar* name, CReader* value,bool bDelete = false,float fBoost = 1.0)
			{
				return new CField(name,value,bDelete,Store::YES,Index::NO,ForwardIndex::NO,fBoost);
			}

			/** 
			 * ����һ���ı����͵ķ������ֶΣ����ֶδ洢������������������ǰ������
			 * @param name �ֶ���
			 * @param text �ı�
			 * @param length �ı�����
			 * @param bNoCopy �Ƿ񿽱��ı�
			 * @return �ֶζ���
			 */
			static CField* UnIndexed(const tchar* name,char* text,size_t length,bool bNoCopy = false,float fBoost = 1.0) 
			{				
				return new CField(name,text,length,bNoCopy,Store::YES,Index::NO,ForwardIndex::NO,fBoost);
			}

			/** 
			 * ����һ���ı����͵ķ������ֶΣ����ֶδ洢������������������ǰ������
			 * @param name �ֶ���
			 * @param text �ı�
			 * @param length �ı�����
			 * @param bNoCopy �Ƿ񿽱��ı�
			 * @return �ֶζ���
			 */
			static CField* UnIndexed(const tchar* name,wchar_t* text,size_t length,bool bNoCopy = false,float fBoost = 1.0) 
			{				
				return new CField(name,text,length,bNoCopy,Store::YES,Index::NO,ForwardIndex::NO,fBoost);
			}


			/** 
			 * ����һ���ı����͵ķ������ֶΣ����ֶδ洢������������������ǰ������
			 * @param name �ֶ���
			 * @param value �ı�			 
			 * @param bNoCopy �Ƿ񿽱��ı�
			 * @return �ֶζ���
			 */
			static CField* UnIndexed(const tchar* name, const str_t& value,bool bNoCopy = true,float fBoost = 1.0) 
			{				
				return new CField(name,value.str,value.length,bNoCopy,Store::YES,Index::NO,ForwardIndex::NO,fBoost);
			}

			/** 
			 * ����һ���ı����͵������ֶΣ����ֶη������������洢��ǰ��������ѡ
			 * @param name �ֶ���
			 * @param value �ı�
			 * @param store �洢��ʽ��Ĭ�ϲ��洢
			 * @param forwardIndex ǰ��������ʽ��Ĭ����ǰ������
			 * @return �ֶζ���
			 */
			static CField* Text(const tchar* name, const char* value,store_t store=Store::NO,forwardindex_t forwardIndex=ForwardIndex::NO,float fBoost = 1.0) 
			{
				return new CField(name, value, store,Index::ANALYZE,forwardIndex,fBoost);
			}

			/** 
			 * ����һ���ı����͵������ֶΣ����ֶη������������洢��ǰ��������ѡ
			 * @param name �ֶ���
			 * @param value �ı�
			 * @param store �洢��ʽ��Ĭ�ϲ��洢
			 * @param forwardIndex ǰ��������ʽ��Ĭ����ǰ������
			 * @return �ֶζ���
			 */
			static CField* Text(const tchar* name, const wchar_t* value,store_t store=Store::NO,forwardindex_t forwardIndex=ForwardIndex::NO,float fBoost = 1.0) 
			{
				return new CField(name, value, store,Index::ANALYZE,forwardIndex,fBoost);
			}

			/** 
			 * ����һ��reader���͵������ֶΣ����ֶη������������洢��ǰ��������ѡ
			 * @param name �ֶ���
			 * @param value reader
			 * @param bDelete �Ƿ����ڲ�ɾ��
			 * @param store �洢��ʽ��Ĭ�ϲ��洢
			 * @param forwardIndex ǰ��������ʽ��Ĭ����ǰ������
			 * @return �ֶζ���
			 */
			static CField* Text(const tchar* name, CReader* value,bool bDelete = false,store_t store = Store::NO,forwardindex_t forwardIndex = ForwardIndex::NO,float fBoost = 1.0)
			{
				return new CField(name,value,bDelete,store,Index::ANALYZE,forwardIndex,fBoost);
			}

			/** 
			 * ����һ���ı����͵������ֶΣ����ֶη������������洢��ǰ��������ѡ
			 * @param name �ֶ���
			 * @param value �ı�
			 * @param store �洢��ʽ��Ĭ�ϲ��洢
			 * @param forwardIndex ǰ��������ʽ��Ĭ����ǰ������
			 * @return �ֶζ���
			 */
			static CField* Text(const tchar* name, char* text,size_t length,bool bNoCopy = true,store_t store = Store::NO,forwardindex_t forwardIndex = ForwardIndex::NO,float fBoost = 1.0) 
			{				
				return new CField(name,text,length,bNoCopy,store,Index::ANALYZE,forwardIndex,fBoost);
			}

			/** 
			 * ����һ���ı����͵������ֶΣ����ֶη������������洢��ǰ��������ѡ
			 * @param name �ֶ���
			 * @param value �ı�
			 * @param store �洢��ʽ��Ĭ�ϲ��洢
			 * @param forwardIndex ǰ��������ʽ��Ĭ����ǰ������
			 * @return �ֶζ���
			 */
			static CField* Text(const tchar* name, wchar_t* text,size_t length,bool bNoCopy = true,store_t store = Store::NO,forwardindex_t forwardIndex = ForwardIndex::NO,float fBoost = 1.0) 
			{				
				return new CField(name,text,length,bNoCopy,store,Index::ANALYZE,forwardIndex,fBoost);
			}

			/** 
			 * ����һ���ı����͵������ֶΣ����ֶη������������洢��ǰ��������ѡ
			 * @param name �ֶ���
			 * @param value �ı�
			 * @param bNoCopy �Ƿ񿽱��ı�
			 * @param store �洢��ʽ��Ĭ�ϲ��洢
			 * @param forwardIndex ǰ��������ʽ��Ĭ����ǰ������
			 * @return �ֶζ���
			 */
			static CField* Text(const tchar* name, const str_t& value,bool bNoCopy = true,store_t store = Store::NO,forwardindex_t forwardIndex = ForwardIndex::NO,float fBoost = 1.0) 
			{				
				return new CField(name,value.str,value.length,bNoCopy,store,Index::ANALYZE,forwardIndex,fBoost);
			}			

			/** 
			 * ����һ���ı����͵������ֶΣ����ֶη��������������洢��ǰ��������ѡ
			 * @param name �ֶ���
			 * @param value �ı�			 
			 * @param forwardIndex ǰ��������ʽ��Ĭ����ǰ������
			 * @return �ֶζ���
			 */
			static CField* UnStored(const tchar* name, const char* value,forwardindex_t forwardIndex=ForwardIndex::NO,float fBoost = 1.0) 
			{
				return new CField(name,value,Store::NO,Index::ANALYZE,forwardIndex,fBoost);
			}

			/** 
			 * ����һ���ı����͵������ֶΣ����ֶη��������������洢��ǰ��������ѡ
			 * @param name �ֶ���
			 * @param value �ı�			 
			 * @param forwardIndex ǰ��������ʽ��Ĭ����ǰ������
			 * @return �ֶζ���
			 */
			static CField* UnStored(const tchar* name, const wchar_t* value,forwardindex_t forwardIndex=ForwardIndex::NO,float fBoost = 1.0) 
			{
				return new CField(name,value,Store::NO,Index::ANALYZE,forwardIndex,fBoost);
			}

			/** 
			 * ����һ��reader���͵������ֶΣ����ֶη��������������洢��ǰ��������ѡ
			 * @param name �ֶ���
			 * @param value �ı���reader			 
			 * @param bDelete �Ƿ����ڲ�ɾ��reader����
			 * @param forwardIndex ǰ��������ʽ��Ĭ����ǰ������
			 * @return �ֶζ���
			 */
			static CField* UnStored(const tchar* name, CReader* value,bool bDelete = false,forwardindex_t forwardIndex=ForwardIndex::NO,float fBoost = 1.0)
			{
				return new CField(name,value,bDelete,Store::NO,Index::ANALYZE,forwardIndex,fBoost);
			}

			/** 
			 * ����һ���ı����͵������ֶΣ����ֶη��������������洢��ǰ��������ѡ
			 * @param name �ֶ���
			 * @param text �ı�			 
			 * @param length �ı�����
			 * @param bNoCopy �Ƿ񿽱�
			 * @param forwardIndex ǰ��������ʽ��Ĭ����ǰ������
			 * @return �ֶζ���
			 */
			static CField* UnStored(const tchar* name, char* text,size_t length,bool bNoCopy = true,forwardindex_t forwardIndex=ForwardIndex::NO,float fBoost = 1.0)
			{				
				return new CField(name,text,length,bNoCopy,Store::NO,Index::ANALYZE,forwardIndex,fBoost);
			}

			/** 
			 * ����һ���ı����͵������ֶΣ����ֶη��������������洢��ǰ��������ѡ
			 * @param name �ֶ���
			 * @param text �ı�			 
			 * @param length �ı�����
			 * @param bNoCopy �Ƿ񿽱�
			 * @param forwardIndex ǰ��������ʽ��Ĭ����ǰ������
			 * @return �ֶζ���
			 */
			static CField* UnStored(const tchar* name, wchar_t* text,size_t length,bool bNoCopy = true,forwardindex_t forwardIndex=ForwardIndex::NO,float fBoost = 1.0)
			{				
				return new CField(name,text,length,bNoCopy,Store::NO,Index::ANALYZE,forwardIndex,fBoost);
			}

			/** 
			 * ����һ���ı����͵������ֶΣ����ֶη��������������洢��ǰ��������ѡ
			 * @param name �ֶ���
			 * @param value �ı�			 
			 * @param forwardIndex ǰ��������ʽ��Ĭ����ǰ������
			 * @return �ֶζ���
			 */
			static CField* UnStored(const tchar* name, const str_t& value,bool bNoCopy = true,forwardindex_t forwardIndex=ForwardIndex::NO,float fBoost = 1.0)
			{				
				return new CField(name,value.str,value.length,bNoCopy,Store::NO,Index::ANALYZE,forwardIndex,fBoost);
			}

			/** 
			 * ����һ�����������͵������ֶΣ����ֶδ洢����������������ǰ������
			 * @param name �ֶ���
			 * @param value ������ֵ
			 * @param length value �ĳ���
			 * @param bNoCopy �Ƿ񿽱�������ֵ
			 * @param bCompress �Ƿ�ѹ��
			 * @return �ֶζ���
			 */
			static CField* Binary(const tchar* name, byte* value,size_t length,bool bNoCopy,bool bCompress,float fBoost = 1.0) 
			{				
				return new CField(name,value,length,bNoCopy,bCompress?Store::COMPRESS:Store::YES,fBoost);
			}

			/** 
			 * ����һ�����������͵������ֶΣ����ֶδ洢����������������ǰ������
			 * @param name �ֶ���
			 * @param dr ������ֵ			 
			 * @param bNoCopy �Ƿ񿽱�������ֵ
			 * @param bCompress �Ƿ�ѹ��
			 * @return �ֶζ���
			 */
			static CField* Binary(const tchar* name,const datarecord& dr,bool bNoCopy,bool bCompress,float fBoost = 1.0) 
			{				
				return new CField(name,dr.data,dr.length,bNoCopy,bCompress?Store::COMPRESS:Store::YES,fBoost);
			}

			/** 
			 * ����һ��Token���͵������ֶΣ����ֶβ��洢��������������ǰ��������ѡ
			 * @param name �ֶ���
			 * @param value Tokenֵ			 
			 * @param bDelete �Ƿ����ڲ�ɾ��Tokenֵ
			 * @param forwardIndex ǰ��������ʽ��Ĭ��Ϊ��ǰ������
			 * @return �ֶζ���
			 */
			static CField* Tokens(const tchar* name,CTokens* value, bool bDelete = true,forwardindex_t forwardIndex = ForwardIndex::NO,float fBoost = 1.0)
			{
				return new CField(name,value,bDelete,forwardIndex,fBoost);
			}

		protected:
			struct fielddata
			{
				field_t	vt;			
				union
				{
					CFXStringA*	strval;	///ANSI
					CFXStringW*	wstrval;///UNICODE					
					CReader*	reader;
					datarecord	binary;				
					numbervar	number;
				}data;
				bool	owner;			///is it the owner of the data?
			};

			class Data : public fielddata
			{
			public:
				Data();
				Data(const string& str);							///will copy the string
				Data(const wstring& wstr);						///will copy the string
				Data(const char* str);							///will copy the string
				Data(const wchar_t* str);							///will copy the string
				Data(char* str,size_t len);						///will NOT copy the string
				Data(wchar_t* str,size_t len);					///will NOT copy the string
				Data(const str_t& str,bool bNoCopy = true);		///will or will NOT copy the string,depends on bNoCopy
				Data(const wstr_t& wstr,bool bNoCopy = true);		///will or will NOT copy the string,depends on bNoCopy
				Data(CFXString* str,bool bOwner = false);			///will or will NOT delete str,depends on bOwner
				Data(CFXStringW* wstr,bool bOwner = false);		///will or will NOT delete str,depends on bOwner
				Data(CReader* r,bool bOwner = false);				///will or will NOT delete data.reader depends on bOwner
				Data(const datarecord& bin,bool bNoCopy = false);	///will or will NOT copy the data,depends on bNoCopy
				Data(const numbervar& num);						///will copy the data			
				Data(const Data& src);						
				~Data();
			public:
				operator string()const;
				operator wstring()const;
				operator char*()const;
				operator wchar_t*()const;
				operator str_t()const;			
				operator wstr_t()const;
				operator CFXStringA*()const;
				operator CFXStringW*()const;
				operator CReader*()const;
				operator datarecord()const;
				operator numbervar&();			

				Data& operator=(const string& str);				///will copy the string
				Data& operator=(const wstring& str);				///will copy the string
				Data& operator=(const char* str);					///will copy the string
				Data& operator=(const wchar_t* wstr);				///will copy the string
				Data& operator=(const str_t& str);				///will NOT copy the string
				Data& operator=(const wstr_t& wstr);				///will NOT copy the string
				Data& operator=(CFXStringA* str);					///will NOT delete str
				Data& operator=(CFXStringW* wstr);				///will NOT delete wstr
				Data& operator=(CReader* r);						///will NOT delete data.reader
				Data& operator=(const datarecord& bin);			///will NOT copy the data
				Data& operator=(const numbervar& num);			///will copy the data			
				Data& operator=(const Data& src);

				void	assignBinary(byte* bin,size_t len,bool bNoCopy);
				void	assignText(char* text,size_t len,bool bNoCopy);
				void	assignText(wchar_t* text,size_t len,bool bNoCopy);
				void	assign(const Data& src,bool bNoCopy);
			public:
				void	clear();
				friend class CField;
			};		
		
		public:
			//field type define
			const static uint8_t EMPTY	= 0;		///Empty
			const static uint8_t TEXT	= 1;		///text
			const static uint8_t TEXTW	= 2;		///Unicode text
			const static uint8_t READER	= 3;		///reader
			const static uint8_t BINARY = 4;		///binary 
			const static uint8_t NUMBER = 5;		///number

		public:								
			/**			 
			 * @param name �ֶ���
			 * @param value �ַ���ֵ
			 * @param store �洢��ʽ
			 * @param index ������ʽ			 
			 * @param forwardIndex ǰ��������ʽ
			 */ 
			CField(const tchar* name, const char* value,store_t store,index_t index,forwardindex_t forwardIndex,float fBoost = 1.0);

			/**			 
			 * @param name �ֶ���
			 * @param value ���ַ��ַ���ֵ
			 * @param store �洢��ʽ
			 * @param index ������ʽ			 
			 * @param forwardIndex ǰ��������ʽ
			 */ 
			CField(const tchar* name, const wchar_t* value,store_t store,index_t index,forwardindex_t forwardIndex,float fBoost = 1.0);

			/**			 
			 * @param name �ֶ���
			 * @param value �ı�reader
			 * @param bDelete �Ƿ����ڲ�ɾ��
			 * @param store �洢��ʽ
			 * @param index ������ʽ			 
			 * @param forwardIndex ǰ��������ʽ
			 */ 
			CField(const tchar* name, CReader* reader,bool bDelete, store_t store,index_t index,forwardindex_t forwardIndex,float fBoost = 1.0);

			/**			 
			 * @param name �ֶ���
			 * @param value �ı�
			 * @param length �ı�����
			 * @param bNoCopy �Ƿ񿽱��ı�
			 * @param store �洢��ʽ
			 * @param index ������ʽ			 
			 * @param forwardIndex ǰ��������ʽ
			 */ 
			CField(const tchar* name, char* text,size_t length,bool bNoCopy, store_t store,index_t index,forwardindex_t forwardIndex,float fBoost = 1.0);

			/**			 
			 * @param name �ֶ���
			 * @param value �ı�
			 * @param length �ı�����
			 * @param bNoCopy �Ƿ񿽱��ı�
			 * @param store �洢��ʽ
			 * @param index ������ʽ			 
			 * @param forwardIndex ǰ��������ʽ
			 */ 
			CField(const tchar* name, wchar_t* text,size_t length,bool bNoCopy,store_t store,index_t index,forwardindex_t forwardIndex,float fBoost = 1.0);

			/**			 
			 * @param name �ֶ���
			 * @param value ������ֵ
			 * @param length �����Ƴ���
			 * @param bNoCopy �Ƿ񿽱�������
			 * @param store �洢��ʽ			 
			 */ 
			CField(const tchar* name, byte* value,size_t length,bool bNoCopy,store_t store,float fBoost = 1.0);
			
			/**			 
			 * @param name �ֶ���
			 * @param value Tokenֵ
			 * @param bDelete �Ƿ����ڲ�ɾ��Token����			 
			 * @param forwardIndex ǰ��������ʽ
			 */ 
			CField(const tchar* name, CTokens* value,bool bDelete,forwardindex_t forwardIndex,float fBoost = 1.0);

			/**			 
			 * @param name �ֶ���
			 * @param nvar ����ֵ
			 * @param store �洢��ʽ
			 * @param index ������ʽ			 			 
			 */ 
			CField(const tchar* name, const numbervar& nvar,store_t store,index_t index,float fBoost = 1.0);

			/**
			 * �������캯��
			 */
			CField(const CField& clone);

			/**
			 * ����һ��ֵΪ�յ��ֶ�
			 * @param id �ֶ�id
			 * @param name �ֶ���
			 * @param store �洢��ʽ
			 * @param index ������ʽ			 
			 * @param forwardIndex ǰ��������ʽ
			 */
			CField(fieldid_t id,const tchar* name,store_t store,index_t index,forwardindex_t fi,float fBoost = 1.0);

			virtual ~CField();
		public:
			fieldid_t			getID(){return m_id;}
			void				setID(fieldid_t fid){m_id = fid;}

			CFXString&			getName(){return m_name;}

			field_t				getType()const{return m_data.vt;}

			token_t				getTokenType()const{return m_tokenType;}

			index_t				getIndex(){return m_index;}
			store_t				getStore(){return m_store;}
			forwardindex_t		getForwardIndex(){return m_forwardIndex;}

			void				fromFlag(fieldflag_t flag);
			fieldflag_t			toFlag();	

			
			inline datarecord&	binaryValue();
			inline CFXStringA*	textAValue();
			inline CFXStringW*	textWValue();
			inline numbervar&	numberValue();
			inline CReader*		readerValue();

			inline CTokens*		tokensValue();
			
			void				textValue(char* text,size_t len,bool bNoCopy = true);
			void				textValue(wchar_t* text,size_t len,bool bNoCopy = true);
			void				textValue(const str_t& str,bool bNoCopy = true);
			void				textValue(const wstr_t& wstr,bool bNoCopy = true);

			void				binaryValue(byte* data,size_t len,bool bNoCopy = true);
			void				binaryValue(const datarecord& dr,bool bNoCopy = true);
			void				numberValue(const numbervar& num);
			void				readerValue(CReader* reader,bool bDelete = false);

			void				tokensValue(CTokens* tokens,bool bDelete = true);
			
			inline void			setBoost(float fBoost){m_fBoost=fBoost;}
			inline float		getBoost(){return m_fBoost;}

			inline bool			isIndexed(){return (m_index != Index::NO);}
			inline bool			isAnalyzed(){return (m_index == Index::ANALYZE);}

			inline bool			isStored(){return (m_store != Store::NO);}
			inline bool			isCompressed(){return (m_store == Store::COMPRESS);}

			inline bool			isStoreForwardIndex(){return (m_forwardIndex != ForwardIndex::NO);}

			/** 
			 * ǳ��¡һ��CField���󣬲��������ݲ���
			 * @return �����µ��ֶζ���,���ⲿɾ��
			 */
			CField*				clone(){return new CField(*this);}	
		public:
			static fieldflag_t	bitsToFlag(index_t index,store_t store,forwardindex_t forwardIndex);
			static void			flagToBits(fieldflag_t flag,index_t& index,store_t& store,forwardindex_t& forwardIndex);
		private:			
			fieldid_t		m_id;				///identifier number 
			CFXString		m_name;				///field name
			index_t			m_index;			///������־
			store_t			m_store;			///�洢��־
			forwardindex_t	m_forwardIndex;		///ǰ��������־

			float			m_fBoost;			///�ֶ�����ֵ
			CField::Data	m_data;				///�ֶ�����
			CTokens*		m_pTokens;			///analyze ���tokens
			bool			m_bDeleteTokens;	///�Ƿ���Ҫɾ��m_pTokens
			token_t			m_tokenType;
		};

		//////////////////////////////////////////////////////////////////////////
		//Inline functions
		//////////////////////////////////////////////////////////////////////////
		//CField::Data
		inline CField::Data::Data()
		{
			memset(this,0,sizeof(fielddata));
			vt = CField::EMPTY;
		}
		inline CField::Data::Data(const string& str)
		{
			memset(this,0,sizeof(fielddata));
			vt = CField::TEXT;
			data.strval = new CFXStringA(str);			
			owner = true;			
		}
		inline CField::Data::Data(const wstring& str)
		{
			memset(this,0,sizeof(fielddata));
			vt = CField::TEXT;
			data.wstrval = new CFXStringW(str);			
			owner = true;
		}
		inline CField::Data::Data(const char* str)
		{
			memset(this,0,sizeof(fielddata));
			vt = CField::TEXT;
			data.strval = new CFXStringA(str);			
			owner = true;			
		}
		inline CField::Data::Data(const wchar_t* str)
		{
			memset(this,0,sizeof(fielddata));
			vt = CField::TEXT;
			data.wstrval = new CFXStringW(str);			
			owner = true;			
		}

		inline CField::Data::Data(char* str,size_t len)
		{
			memset(this,0,sizeof(fielddata));
			vt = CField::TEXT;
			data.strval = new CFXStringA(str,len,false);		//NO copy string
			owner = true;			
		}
		inline CField::Data::Data(wchar_t* str,size_t len)
		{
			memset(this,0,sizeof(fielddata));
			vt = CField::TEXTW;
			data.wstrval = new CFXStringW(str,len,false);		//NO copy string			
			owner = true;			
		}

		inline CField::Data::Data(const str_t& str,bool bNoCopy)
		{
			memset(this,0,sizeof(fielddata));			
			vt = CField::TEXT;			
			data.strval = new CFXStringA(str,bNoCopy);					
			owner = true;			
		}
		inline CField::Data::Data(const wstr_t& wstr,bool bNoCopy)
		{
			memset(this,0,sizeof(fielddata));
			vt = CField::TEXTW;			
			data.wstrval = new CFXStringW(wstr,bNoCopy);					
			owner = true;			
		}
		inline CField::Data::Data(CFXStringA* str,bool bOwner)
		{
			memset(this,0,sizeof(fielddata));
			vt = CField::TEXT;			
			data.strval = str;
			owner = bOwner;			
		}
		inline CField::Data::Data(CFXStringW* wstr,bool bOwner)
		{
			memset(this,0,sizeof(fielddata));
			vt = CField::TEXTW;			
			data.wstrval = wstr;
			owner = bOwner;			
		}

		inline CField::Data::Data(CReader* r,bool bOwner)
		{
			memset(this,0,sizeof(fielddata));
			vt = CField::READER;
			data.reader = r;		
			owner = bOwner;
		}
		inline CField::Data::Data(const datarecord& bin,bool bNoCopy)
		{
			memset(this,0,sizeof(fielddata));
			vt = CField::BINARY;
			data.binary.length = bin.length;
			if(!bNoCopy)
			{
				data.binary.data = new byte[bin.length];
				memcpy(data.binary.data,bin.data,bin.length);
				owner = true;
			}
			else
			{
				data.binary.data = bin.data;
				owner = false;
			}
		}
		inline CField::Data::Data(const numbervar& num)
		{
			memset(this,0,sizeof(fielddata));
			vt = CField::NUMBER;
			data.number = num;
		}		
		inline CField::Data::Data(const CField::Data& src)
		{
			if(vt != CField::EMPTY)
				clear();
			vt = src.vt;	
			switch(src.vt)
			{
			case CField::EMPTY:
				break;
			case CField::TEXT:
				data.strval = new CFXStringA(*(src.data.strval));
				owner = true;
				break;
			case CField::TEXTW:
				data.wstrval = new CFXStringW(*(src.data.wstrval));
				owner = true;
				break;
			case CField::BINARY:
				data.binary.length = src.data.binary.length;
				if(src.owner)
				{					
					data.binary.data = new byte[data.binary.length];
					memcpy(data.binary.data,src.data.binary.data,data.binary.length);
				}
				else 
				{
					data.binary.data = src.data.binary.data;
				}
				owner = src.owner;
				break;
			case CField::READER:
				if(src.owner)
				{
					data.reader = src.data.reader->clone();
					owner = true;
				}
				else 
				{
					data.reader = src.data.reader;
					owner = false;
				}
				break;			
			}
		}
		inline CField::Data::~Data()
		{
			clear();
		}

		inline CField::Data::operator string()const
		{
			if(vt != CField::TEXT)
				FIRTEX_THROW2(CAST_ERROR,_T("CField::Data::operator string()const"));
			return (*(data.strval));
		}
		inline CField::Data::operator wstring()const
		{
			if(vt != CField::TEXTW)
				FIRTEX_THROW2(CAST_ERROR,_T("CField::Data::operator wstring()const"));
			return (*(data.wstrval));
		}
		inline CField::Data::operator char*()const
		{
			if(vt != CField::TEXT)
				FIRTEX_THROW2(CAST_ERROR,_T("CField::Data::operator char*()const"));
			return (*(data.strval));
		}
		inline CField::Data::operator wchar_t*()const
		{
			if(vt != CField::TEXTW)
				FIRTEX_THROW2(CAST_ERROR,_T("CField::Data::operator wchar_t*()const"));
			return (*(data.wstrval));
		}
		
		inline CField::Data::operator str_t()const
		{
			if(vt != CField::TEXT)
				FIRTEX_THROW2(CAST_ERROR,_T("CField::Data::operator str_t()const"));
			return (*(data.strval));
		}
		inline CField::Data::operator wstr_t()const
		{
			if(vt != CField::TEXTW)
				FIRTEX_THROW2(CAST_ERROR,_T("CField::Data::operator wstr_t()const"));
			return (*(data.wstrval));
		}		
	
		inline CField::Data::operator CFXStringA*()const
		{
			if(vt != CField::TEXT)
				FIRTEX_THROW2(CAST_ERROR,_T("CField::Data::operator CFXStringA*()const"));
			return data.strval;
		}
		inline CField::Data::operator CFXStringW*()const
		{
			if(vt != CField::TEXTW)
				FIRTEX_THROW2(CAST_ERROR,_T("CField::Data::operator CFXStringW*()const"));
			return data.wstrval;
		}

		inline CField::Data::operator CReader*()const
		{
			if(vt != CField::READER)
				FIRTEX_THROW2(CAST_ERROR,_T("CField::Data::operator CReader*()const"));
			return data.reader;
		}
		inline CField::Data::operator datarecord()const
		{
			if(vt != CField::BINARY)
				FIRTEX_THROW2(CAST_ERROR,_T("CField::Data::operator datarecord()const"));
			return data.binary;
		}
		inline CField::Data::operator numbervar&()
		{
			if(vt != CField::NUMBER)
				FIRTEX_THROW2(CAST_ERROR,_T("CField::Data::operator numbervar&()const"));
			return data.number;
		}		
		inline CField::Data& CField::Data::operator=(const string& str)
		{
			if(vt != CField::TEXT && vt != CField::EMPTY)
				clear();
			vt = CField::TEXT;
			if(!data.strval)
			{
				data.strval = new CFXStringA(str);
				owner = true;
			}
			else 
				*(data.strval) = str;
			return (*this);
		}
		inline CField::Data& CField::Data::operator=(const wstring& wstr)
		{
			if(vt != CField::TEXTW && vt != CField::EMPTY)
				clear();
			vt = CField::TEXTW;

			if(!data.strval)
			{
				data.wstrval = new CFXStringW(wstr);
				owner = true;
			}
			else 
				*(data.wstrval) = wstr;			
			return (*this);
		}
		inline CField::Data& CField::Data::operator=(const char* str)
		{
			if(vt != CField::TEXT && vt != CField::EMPTY)
				clear();
			vt = CField::TEXT;
			if(!owner || !data.strval)
				data.strval = new CFXStringA(str);
			else (*(data.strval)) = str;
			return (*this);
		}
		inline CField::Data& CField::Data::operator=(const wchar_t* wstr)
		{
			if(vt != CField::TEXT && vt != CField::EMPTY)
				clear();
			vt = CField::TEXTW;
			if(!owner)
				data.wstrval = new CFXStringW(wstr);
			else (*(data.wstrval)) = wstr;			
			return (*this);
		}

		inline CField::Data& CField::Data::operator=(const str_t& str)
		{
			if(vt != CField::TEXT && vt != CField::EMPTY)
				clear();
			vt = CField::TEXT;
			if(!owner || !data.strval)
			{
				data.strval = new CFXStringA(str);
				owner = true;
			}
			else (*(data.strval)) = str;
			return (*this);			
		}
		inline CField::Data& CField::Data::operator=(const wstr_t& wstr)
		{
			if(vt != CField::TEXTW && vt != CField::EMPTY)
				clear();
			vt = CField::TEXTW;
			if(!owner || !data.wstrval)
			{
				data.wstrval = new CFXStringW(wstr);
				owner = true;
			}
			else (*(data.wstrval)) = wstr;
			
			return (*this);
		}
		inline CField::Data& CField::Data::operator=(CFXStringA* str)
		{
			if(vt != CField::TEXT && vt != CField::EMPTY)
				clear();
			vt = CField::TEXT;
			if(owner && data.strval)
				delete data.strval;
			data.strval = str;
			owner = false;
			return (*this);
		}
		inline CField::Data& CField::Data::operator=(CFXStringW* wstr)
		{
			if(vt != CField::TEXTW && vt != CField::EMPTY)
				clear();
			vt = CField::TEXTW;
			if(owner && data.wstrval)
				delete data.wstrval;
			data.wstrval = wstr;
			owner = false;
			return (*this);
		}
		inline CField::Data& CField::Data::operator=(CReader* r)
		{
			if(vt != CField::READER && vt != CField::EMPTY)
				clear();
			vt = CField::READER;
			if(owner && data.reader)
				delete data.reader;
			data.reader = r;
			owner = false;			
			return (*this);
		}
		inline CField::Data& CField::Data::operator=(const datarecord& bin)
		{
			if(vt != CField::BINARY && vt != CField::EMPTY)
				clear();
			vt = CField::BINARY;
			if(owner && data.binary.data)
				delete[] data.binary.data;
			data.binary.data = bin.data;
			data.binary.length = bin.length;
			owner = false;			
			return (*this);
		}
		inline CField::Data& CField::Data::operator=(const numbervar& num)
		{
			if(vt != CField::NUMBER && vt != CField::EMPTY)
				clear();
			vt = CField::NUMBER;
			data.number = num;			
			owner = true;						
			return (*this);
		}		
		inline CField::Data& CField::Data::operator=(const CField::Data& src)
		{
			if(vt != CField::EMPTY)
				clear();
			vt = src.vt;	
			switch(src.vt)
			{
			case CField::EMPTY:
				break;
			case CField::TEXT:
				data.strval = new CFXStringA(*(src.data.strval));
				owner = true;
				break;
			case CField::TEXTW:
				data.wstrval = new CFXStringW(*(src.data.wstrval));
				owner = true;
				break;
			case CField::BINARY:
				data.binary.length = src.data.binary.length;
				if(src.owner)
				{					
					data.binary.data = new byte[data.binary.length];
					memcpy(data.binary.data,src.data.binary.data,data.binary.length);
				}
				else 
				{
					data.binary.data = src.data.binary.data;
				}
				owner = src.owner;
				break;
			case CField::READER:
				if(src.owner)
				{
					data.reader = src.data.reader->clone();
					owner = true;
				}
				else 
				{
					data.reader = src.data.reader;
					owner = false;
				}
				break;			
			}

			return (*this);
		}
		inline void CField::Data::assignBinary(byte* bin,size_t len,bool bNoCopy)
		{
			if(vt != CField::BINARY)
				clear();
			vt = CField::BINARY;
			if(owner)
				delete[] data.binary.data;
			data.binary.length = len;
			if(bNoCopy)
				data.binary.data = bin;	
			else 
			{
				data.binary.data = new byte[len];
				memcpy(data.binary.data,bin,len);
			}
			owner = !bNoCopy;
		}
		inline void CField::Data::assignText(char* text,size_t len,bool bNoCopy)
		{
			if(vt != CField::TEXT)
				clear();
			vt = CField::TEXT;
			if(!owner || !data.strval)
			{
				data.strval = new CFXString(text,len,bNoCopy);
				owner = true;
			}	
			else 
			{
				data.strval->assign(text,len,bNoCopy);
			}			
		}

		inline void CField::Data::assignText(wchar_t* text,size_t len,bool bNoCopy)
		{
			if(vt != CField::TEXTW)
				clear();
			vt = CField::TEXTW;
			if(!owner || !data.wstrval)
			{
				data.wstrval = new CFXStringW(text,len,bNoCopy);
				owner = true;
			}	
			else 
			{
				data.wstrval->assign(text,len,bNoCopy);
			}			
		}
		inline void CField::Data::assign(const CField::Data& src,bool bNoCopy)
		{
			if(!bNoCopy)
			{
				*this = src;
				return;
			}				
			if(vt != CField::EMPTY)
				clear();			
			memcpy(this,&src,sizeof(fielddata));
			owner = false;
		}

		inline void CField::Data::clear()
		{
			if(owner)
			{
				switch(vt)
				{
				case CField::TEXT:
					delete data.strval;
					break;
				case CField::TEXTW:
					delete data.wstrval;
					break;
				case CField::BINARY:
					delete[] data.binary.data;
					break;
				case CField::READER:
					delete data.reader;
					break;				
				default:
					break;
				}
			}			
			memset(this,0,sizeof(fielddata));
		}


		//////////////////////////////////////////////////////////////////////////
		//CField
		inline datarecord&	CField::binaryValue()
		{
			if(getType() != CField::BINARY)
				FIRTEX_THROW2(CAST_ERROR,_T("CField.binaryValue:the data in this field is not a binary value"));

			return (datarecord&)m_data;
		}
		
		inline CFXStringA* CField::textAValue()
		{
			if(m_data.vt != CField::TEXT)
				FIRTEX_THROW2(CAST_ERROR,_T("CField.CField::textValue():the data in this field is not a text value"));

			return m_data.data.strval;
		}
		inline CFXStringW* CField::textWValue()
		{
			if(m_data.vt != CField::TEXTW)
				FIRTEX_THROW2(CAST_ERROR,_T("CField.CField::textWValue():the data in this field is not a unicode text value"));

			return m_data.data.wstrval;
		}		
		inline numbervar& CField::numberValue()
		{
			if(m_data.vt != CField::NUMBER)
				FIRTEX_THROW2(CAST_ERROR,_T("CField.CField::numberValue():the data in this field is not a number value"));

			return m_data.data.number;
		}
		inline CReader* CField::readerValue()
		{
			if(m_data.vt == CField::EMPTY)
				return NULL;
			if(m_data.vt != CField::READER)
				FIRTEX_THROW2(CAST_ERROR,_T("CField.CField::readerValue():the data in this field is not a reader value"));

			return m_data.data.reader;
		}
		inline CTokens* CField::tokensValue()
		{
			return m_pTokens;
		}		

		inline void CField::textValue(char* text,size_t len,bool bNoCopy)
		{				
			m_data.assignText(text,len,bNoCopy);
		}	
		inline void CField::textValue(wchar_t* text,size_t len,bool bNoCopy)
		{					
			m_data.assignText(text,len,bNoCopy);
		}	
		inline void CField::textValue(const str_t& str,bool bNoCopy)
		{		
			m_data.assignText(str.str,str.length,bNoCopy);
		}
		inline void CField::textValue(const wstr_t& wstr,bool bNoCopy)
		{		
			m_data.assignText(wstr.str,wstr.length,bNoCopy);
		}
		inline void CField::readerValue(CReader* reader,bool bDelete)
		{			
			m_data = reader;
			m_data.owner = bDelete;			
		}

		inline void CField::tokensValue(CTokens* tokens,bool bDelete)
		{
			if(m_bDeleteTokens && m_pTokens)
				delete m_pTokens;
			m_pTokens = tokens;
			m_bDeleteTokens = bDelete;
			m_tokenType = tokens->getType();
		}
		inline void CField::binaryValue(byte* data,size_t len,bool bNoCopy)
		{
			if (data == NULL)
				throw CIllegalArgumentException(_T("value cannot be null."));
			m_data.assignBinary(data,len,bNoCopy);
		}
		inline void CField::binaryValue(const datarecord& dr,bool bNoCopy)
		{
			if (dr.data == NULL)
				throw CIllegalArgumentException(_T("CField::binaryValue():value cannot be null."));
			m_data.assignBinary(dr.data,dr.length,bNoCopy);
		}
		inline void CField::numberValue(const numbervar& num)
		{					
			m_data = num;
		}	
	}
}


#endif
