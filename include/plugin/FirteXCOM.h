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
// Created	: 2006/5/8
//
#ifndef _FIRTEXCOM_H
#define _FIRTEXCOM_H

#include "../com/Com.h"

#ifdef __cplusplus

namespace firtex
{
	namespace plugin
	{
		//ANSI字符串定义
        typedef struct _tagstr 
        {
			char*	str;
			size_t	length;
        }STR;

		//宽字符串定义
		typedef struct _tagwstr 
		{
			wchar_t*	str;
			size_t		length;
		}WSTR;

		typedef int32_t fieldid_t;	

		typedef unsigned short NUMBERTYPE;

		//数字类型定义
		enum NUMENUM
		{			
			NT_NULL		= 0,
			NT_BOOL		= 1,
			NT_I1		= 2,
			NT_UI1		= 3,
			NT_I2		= 4,
			NT_UI2		= 5,
			NT_I4		= 6,
			NT_UI4		= 7,
			NT_I8		= 8,
			NT_UI8		= 9,

			NT_R4		= 10,
			NT_R8		= 11,
		};

		//数字定义
		typedef struct _tagNUMBERVAR
		{
			NUMBERTYPE	nt;				//Number Type
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
		}NUMBERVAR;

		//二进制数值定义
		typedef struct _tagDATARECORD
		{
			byte*	data;			//data 
			size_t	length;			//length of data
			bool	nocopy;			//copy data or not
 		}DATARECORD;

		///Charset Types
		enum CHARSET
		{
			CHARSET_UNKNOWN      = 0,
			CHARSET_ISO_8859_1   = 1,
			CHARSET_UTF_8        = 2,
			CHARSET_EUC_KR       = 3,
			CHARSET_ISO_8859_15  = 4,
			CHARSET_WINDOWS_1252 = 5,
			CHARSET_UTF_16       = 6,
			CHARSET_EUC_JP       = 7,
			CHARSET_KOI8_R       = 8,
			CHARSET_KOI8_U       = 9,
			CHARSET_ISO_8859_2   = 10,
			CHARSET_ISO_8859_3   = 11,
			CHARSET_ISO_8859_4   = 12,
			CHARSET_ISO_8859_5   = 13,
			CHARSET_ISO_8859_6   = 14,
			CHARSET_ISO_8859_6_I = 15,
			CHARSET_ISO_8859_6_E = 16,
			CHARSET_ISO_8859_7   = 17,
			CHARSET_ISO_8859_8   = 18,
			CHARSET_ISO_8859_8_I = 19,
			CHARSET_ISO_8859_8_E = 20,
			CHARSET_ISO_8859_9   = 21,
			CHARSET_ISO_8859_10  = 22,
			CHARSET_ISO_8859_11  = 23,
			CHARSET_ISO_8859_13  = 24,
			CHARSET_ISO_8859_14  = 25,
			CHARSET_ISO_8859_16  = 26,
			CHARSET_ISO_IR_111   = 27,
			CHARSET_ISO_2022_CN  = 29,
			CHARSET_ISO_2022_KR  = 30,
			CHARSET_ISO_2022_JP  = 31,
			CHARSET_US_ASCII     = 32,
			CHARSET_UTF_32BE     = 33,
			CHARSET_UTF_32LE     = 34,
			CHARSET_UTF_16BE     = 35,
			CHARSET_UTF_16LE     = 36,
			CHARSET_WINDOWS_1250 = 37,
			CHARSET_WINDOWS_1251 = 38,
			CHARSET_WINDOWS_1253 = 39,
			CHARSET_WINDOWS_1254 = 40,
			CHARSET_WINDOWS_1255 = 41,
			CHARSET_WINDOWS_1256 = 42,
			CHARSET_WINDOWS_1257 = 43,
			CHARSET_WINDOWS_1258 = 44,
			CHARSET_IBM866       = 45,
			CHARSET_IBM850       = 46,
			CHARSET_IBM852       = 47,
			CHARSET_IBM855       = 48,
			CHARSET_IBM857       = 49,
			CHARSET_IBM862       = 50,
			CHARSET_IBM864       = 51,
			CHARSET_IBM864I      = 52,
			CHARSET_UTF_7        = 53,
			CHARSET_SHIFT_JIS    = 54,
			CHARSET_BIG5         = 55,
			CHARSET_GB2312       = 56,
			CHARSET_GB18030      = 57,
			CHARSET_VISCII       = 58,
			CHARSET_TIS_620      = 59,
			CHARSET_HZ_GB_2312   = 61,
			CHARSET_BIG5_HKSCS   = 62,
			CHARSET_X_GBK        = 63,
			CHARSET_X_EUC_TW     = 64
		};


		typedef unsigned int method_type;
		typedef method_type store_t;
		typedef method_type index_t;
		typedef method_type forwardindex_t;
		
		enum Index
		{
			INDEX_NO			= 1,
			INDEX_ANALYZER		= 2,
			INDEX_UN_ANALYZER	= 3,
		};
		enum Store
		{
			STORE_YES		= 1,
			STORE_COMPRESS	= 2,
			STORE_NO		= 3
		};
		enum ForwardIndex
		{
			FORWARDINDEX_NO				= 1,
			FORWARDINDEX_SEQUENCE		= 2,	//存储文档Analyze后的原始序列
			FORWARDINDEX_FREQ			= 3,	//不存储位置信息，仅存储词和词频信息
			FORWARDINDEX_WITH_POSITION	= 4,	//存储词，词频，词位置信息
		};

		struct PATH_INPUT
		{
			firtex::plugin::fieldid_t	sid;
			char*						path;
		};
		struct BUFFER_INPUT 
		{
			firtex::plugin::fieldid_t	sid;
			char*						buffer;
			int32_t						length;
		};

		class IReader;
		struct READER_INPUT 
		{
			firtex::plugin::fieldid_t	sid;
			IReader*					reader;
		};
		//{09B42270-8ACD-4e8a-B125-A79B46003F5D}
		class IDocumentSchema : public firtex::com::IUnknown
		{
		public:		
			static const firtex::com::FX_IID iid;
			virtual firtex::com::FX_HRESULT FX_STDMETHODCALLTYPE addKeywordItem(/* [in] */firtex::com::FX_BSTR name,
																		  /* [out] */firtex::plugin::fieldid_t* pid) = 0;
			virtual firtex::com::FX_HRESULT FX_STDMETHODCALLTYPE addUnIndexedItem(/* [in] */firtex::com::FX_BSTR name,
																			/* [out] */firtex::plugin::fieldid_t* pid) = 0;
			virtual firtex::com::FX_HRESULT FX_STDMETHODCALLTYPE addTextItem(/* [in] */firtex::com::FX_BSTR name,
																	   /* [in] */firtex::plugin::store_t store,
																	   /* [in] */firtex::plugin::forwardindex_t termVector,
																	   /* [out] */firtex::plugin::fieldid_t* pid) = 0;
			virtual firtex::com::FX_HRESULT FX_STDMETHODCALLTYPE addUnStoredItem(/* [in] */firtex::com::FX_BSTR name,
																		   /* [in] */firtex::plugin::forwardindex_t termVector,
																		   /* [out] */firtex::plugin::fieldid_t* pid) = 0;
			virtual firtex::com::FX_HRESULT FX_STDMETHODCALLTYPE addBinaryItem(/* [in] */firtex::com::FX_BSTR name,
																		 /* [in] */firtex::com::bool_t bCompress,
																		 /* [out] */firtex::plugin::fieldid_t* pid) = 0;
		};

		// {31F417A2-5111-4e49-8D14-EA1D63F6CD5D}
		class IDocumentInput : public firtex::com::IUnknown
		{
		public:
			static const firtex::com::FX_IID iid;
			virtual firtex::com::FX_HRESULT FX_STDMETHODCALLTYPE getPathInput(/* [out] */PATH_INPUT** ppPathInput) = 0;
			virtual firtex::com::FX_HRESULT FX_STDMETHODCALLTYPE getBufferInputs(/* [out] */BUFFER_INPUT*** pppBufferInput,/* [out] */int32_t* pNumBuffers) = 0;
			virtual firtex::com::FX_HRESULT FX_STDMETHODCALLTYPE getReaderInputs(/* [out] */READER_INPUT*** pppReaderInput,/* [out] */int32_t* pNumReaders) = 0;
		};

		// {C92FA9F4-86FD-48c1-AB00-B16C78D2983F}
		class IDocument : public firtex::com::IUnknown
		{
		public:		
			static const firtex::com::FX_IID iid;			
			virtual firtex::com::FX_HRESULT FX_STDMETHODCALLTYPE addTextField(/* [in] */firtex::plugin::fieldid_t id,/* [in] */firtex::plugin::STR* pStr,firtex::com::bool_t bNoCopy) = 0;
			virtual firtex::com::FX_HRESULT FX_STDMETHODCALLTYPE addBinaryField(/* [in] */firtex::plugin::fieldid_t id,/* [in] */firtex::plugin::DATARECORD* pBinary) = 0;
			virtual firtex::com::FX_HRESULT FX_STDMETHODCALLTYPE addNumberField(/* [in] */firtex::plugin::fieldid_t id,/* [in] */firtex::plugin::NUMBERVAR* pNumber) = 0;
			virtual firtex::com::FX_HRESULT FX_STDMETHODCALLTYPE setEncoding(/* [in] */firtex::plugin::CHARSET _charset) = 0;
			virtual firtex::com::FX_HRESULT FX_STDMETHODCALLTYPE getEncoding(/* [out] [retval] */firtex::plugin::CHARSET* retVal) = 0;

		};

		// {750C8BE7-1858-4c61-A97C-C864CF552EFD}
		class IReader : public firtex::com::IUnknown
		{
		public:		
			static const firtex::com::FX_IID iid;
			virtual firtex::com::FX_HRESULT FX_STDMETHODCALLTYPE read(/* [in] [retval] */byte* data,/* [in] */int32_t length) = 0;
			virtual firtex::com::FX_HRESULT FX_STDMETHODCALLTYPE readWithoutCopy(/* [out] [retval] */byte** data,/* [out] [retval] */int32_t* length) = 0;
			virtual firtex::com::FX_HRESULT FX_STDMETHODCALLTYPE close() = 0;
			virtual firtex::com::FX_HRESULT FX_STDMETHODCALLTYPE getFilePointer(/* [out] [retval] */int64_t* position) = 0;
			virtual firtex::com::FX_HRESULT FX_STDMETHODCALLTYPE seek(/* [in] */int64_t position) = 0;
			virtual firtex::com::FX_HRESULT FX_STDMETHODCALLTYPE isEof(/* [out] [retval] */firtex::com::bool_t* bEof) = 0;
			virtual firtex::com::FX_HRESULT FX_STDMETHODCALLTYPE length(/* [out] [retval] */int64_t* len) = 0;
		};

		enum TOKENENUM
		{
			TOKEN_NONE		= 0x0,
			TOKEN_WORD		= 0x01,	//词
			TOKEN_DATE		= 0x02,	//时间日期
			TOKEN_NAME		= 0x04,	//人名
			TOKEN_COMPANY	= 0x08,	//机构名
			TOKEN_EMAIL		= 0x10,	//Email
			TOKEN_NUM		= 0x20,	//数字
			TOKEN_ALPHA		= 0x40,	//字母
			TOKEN_ALPNUM	= 0x80,	//数字字母组合
			TOKEN_URL		= 0x100,//URL
		};


		typedef uint32_t	TOKENTYPE;
		typedef int32_t		TERMID;
		// {E6F0F10C-DF0B-411a-9A3E-EA2BCD4DF256}
		class ITokens : public firtex::com::IUnknown
		{
		public:		
			static const firtex::com::FX_IID iid;

			virtual firtex::com::FX_HRESULT FX_STDMETHODCALLTYPE getNumTokens(/* [out] [retval]*/int32_t* nNumTokens) = 0;
			virtual firtex::com::FX_HRESULT FX_STDMETHODCALLTYPE getMaxTokens(/* [out] [retval]*/int32_t* nMaxTokens) = 0;
			virtual firtex::com::FX_HRESULT FX_STDMETHODCALLTYPE setMaxTokens(/* [in] */int32_t maxTokens) = 0;
			virtual firtex::com::FX_HRESULT FX_STDMETHODCALLTYPE clear() = 0;
			virtual firtex::com::FX_HRESULT FX_STDMETHODCALLTYPE getBuffer(/* [out] [retval] */byte** buf) = 0;
			virtual firtex::com::FX_HRESULT FX_STDMETHODCALLTYPE addToken(/* [in] */char* pszToken,int32_t nTokenLen,/* [out] [retval] */firtex::com::bool_t* bSuc) = 0;
			//virtual firtex::com::FX_HRESULT FX_STDMETHODCALLTYPE addTokenX(/* [in] */,/* [out] [retval] */firtex::com::bool_t* bSuc) = 0;
		};			
	
		//{D6CC19E5-3FEA-4c96-B50C-C176080E89C5}
		class IParserPlugin : public firtex::com::IUnknown
		{		
		public:
			static const firtex::com::FX_IID iid;
						
			virtual firtex::com::FX_HRESULT FX_STDMETHODCALLTYPE getIdentifier(/* [out] [retval] */firtex::com::FX_BSTR* retVal) = 0;
			virtual firtex::com::FX_HRESULT FX_STDMETHODCALLTYPE getDocType(/* [out] [retval] */firtex::com::FX_BSTR* retVal) = 0;
			virtual firtex::com::FX_HRESULT FX_STDMETHODCALLTYPE setParam(/* [in] */firtex::com::FX_BSTR pszParam) = 0;
			virtual firtex::com::FX_HRESULT FX_STDMETHODCALLTYPE getParam(/* [in] */firtex::com::FX_BSTR pszParamName,/* [out] [retval] */firtex::com::FX_BSTR* pszParamValue,/* [out] [retval] */firtex::com::bool_t* bSuc) = 0;			
			virtual firtex::com::FX_HRESULT FX_STDMETHODCALLTYPE defineSchema(/* [in] [out] [retval] */IDocumentSchema* pSchema,/* [out] */firtex::plugin::fieldid_t* pid) = 0;
			virtual firtex::com::FX_HRESULT FX_STDMETHODCALLTYPE parseInternal(/* [in] */IDocumentInput* pDocInput,
																			   /* [in] */IDocument* pResultDoc) = 0;
		};

		// {7D7D876B-CCE4-4fe0-8389-A20106A828A3}
		class IAnalyzerPlugin : public firtex::com::IUnknown
		{		
		public:
			static const firtex::com::FX_IID iid;
			
			virtual firtex::com::FX_HRESULT FX_STDMETHODCALLTYPE getIdentifier(/* [out] [retval] */firtex::com::FX_BSTR* retVal) = 0;
			virtual firtex::com::FX_HRESULT FX_STDMETHODCALLTYPE getTokenType(/* [out] [retval] */TOKENTYPE* retVal) = 0;
			virtual firtex::com::FX_HRESULT FX_STDMETHODCALLTYPE getRecommendIndexer(/* [out] [retval] */ firtex::com::FX_BSTR* retVal) = 0;
			virtual firtex::com::FX_HRESULT FX_STDMETHODCALLTYPE nextTokensInternal(/* [in] */ITokens* pUnAnalyzedTokenVector,ITokens** ppAnalyzedTokenVector) = 0;
		};
	}
}
#endif //__cplusplus

#endif
