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
// Email	: ruijieguo@software.ict.ac.cn
// Created	: 2005/11/20
// Modified	: 2006/10/15 add uri,encoding,status and so on.-by GuoRuijie
//

#ifndef __DOCUMENT_H
#define __DOCUMENT_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../utility/StdHeader.h"
#include "doctypedef.h"
#include "../utility/charsetdef.h"
#include "Field.h"
#include "DocumentSchema.h"
#include "../utility/Config.h"
#include <string>
#include <map>
#include <vector>

using namespace std;
using namespace firtex::utility;

namespace firtex
{
	namespace plugin
	{
		class IDocument; 
		class IDocumentInput;
	}
}

namespace firtex
{
	namespace document
	{			
		class CDocumentInput
		{
		public:
			struct PathInput
			{
			public:
				PathInput(schemaid_t _sid,const tchar* _docPath)
					:sid(_sid)					
					,docPath(_docPath)
				{}				
				PathInput()
					:sid(-1)
				{
				}
				~PathInput()
				{					
				}
			public:
				schemaid_t	sid;
				CFXString	docPath;	
			};			

			struct BufferInput
			{
			public:
				BufferInput(schemaid_t _sid,char* _buffer,size_t len)
					:sid(_sid)					
					,buffer(_buffer)
					,length(len)
					,bOwnBuffer(false)
				{}				
				BufferInput()
					:sid(-1)
					,buffer(NULL)
					,bOwnBuffer(false)
				{
				}
				~BufferInput()
				{				
					if(bOwnBuffer && buffer)
					{
						delete[] buffer;
						buffer = NULL;
					}
				}
			public:
				schemaid_t	sid;
				char*		buffer;	
				size_t		length;
				bool		bOwnBuffer;
			};		
			struct ReaderInput
			{
			public:
				ReaderInput(schemaid_t _sid,CReader* _reader)
					:sid(_sid)					
					,reader(_reader)
				{}				
				ReaderInput()
					:sid(-1)
					,reader(NULL)
				{
				}
				~ReaderInput()
				{				
					if(reader)
					{
						delete reader;
						reader = NULL;
					}
				}
			public:
				schemaid_t	sid;
				CReader*	reader;	
			};		

		public:
			CDocumentInput();
			CDocumentInput(schemaid_t sid,const tchar* szDocPath);
			CDocumentInput(schemaid_t sid,char* _buffer,int32_t len);			
			virtual ~CDocumentInput();
		public:			
			/** 
			 * 设置文档路径
			 * @param sid 路径所在字段的schema id,由CDocumentSchema定义
			 * @param szDocPath 文档路径
			 */
			inline void	setPathInput(schemaid_t sid,const tchar* szDocPath);
			
			/** 
			 * 设置文档的Buffer
			 * @param _off 在Buffer数组中的位置
			 * @param sid Buffer所字段的schema id,由CDocumentSchema定义
			 * @param buf Buffer,内部不作拷贝，仅保存指针
			 */
			inline void	setBufferInput(int32_t _off,schemaid_t sid,char* buf,size_t len);

			/** 
			 * 设置文档的Reader
			 * @param _off 在Reader数组中的位置
			 * @param sid Reader所字段的schema id,由CDocumentSchema定义
			 * @param _reader Reader,内部不作拷贝，仅保存指针
			 */
			inline void	setReaderInput(int32_t _off,schemaid_t sid,CReader* _reader);

			/**
			 * 获取PathInput			 
			 * @return PathInput对象,内部对象外部不能删除
			 */
			inline PathInput*	getPathInput();
			/**
			 * 获取BufferInput
			 * @param numBuffers BufferInput数目
			 * @return BufferInput数组,内部对象外部不能删除
			 */
			inline BufferInput**	getBufferInputs(int32_t& numBuffers);

			/**
			 * 获取ReaderInput
			 * @param numReaders ReaderInput数目
			 * @return ReaderInput数组,内部对象外部不能删除
			 */
			inline ReaderInput**	getReaderInputs(int32_t& numReaders);


			/** 
			 * 预留Buffer数组空间
			 * @param _count 预留数组大小
			 */
			void	reserveBuffers(int32_t _count);

			/** 
			 * 预留Reader数组空间
			 * @param _count 预留数组大小
			 */
			void	reserveReaders(int32_t _count);


			/** 
			 * is there delayed schema for parser
			 * @return true for has
			 */
			inline bool	hasDelayedSchema();
			/**
			 * set delayed schema id for parser
			 * @param _sid schema id form parser
			 */
			inline void setDelayedSchema(schemaid_t _sid);
		public:
			firtex::plugin::IDocumentInput*		getInterface();
		protected:
			firtex::plugin::IDocumentInput*		wrapInterface();
		protected:
			PathInput*			m_pPathInput;			///一个文档只有一个文档路径

			BufferInput**		m_ppBufferInput;		///一个文档可能有多个Buffer
			int32_t				m_numBufferInput;		///数组长度

			ReaderInput**		m_ppReaderInput;		///一个文档可能有多个Reader
			int32_t				m_numReaderInput;		///数组长度

			schemaid_t*			m_pDelayedSchemaID;		///delayed schema id for parser
		private:
			firtex::plugin::IDocumentInput*		m_pInterface;
		};		
	
		class CDocument
		{
			struct EXTEND_FIELD 
			{
				CField*		field;
				bool		extend;
			};
			
		public:
			CDocument(int32_t numFields);
			CDocument(CDocumentSchema* pSchema);
			~CDocument();
		public:
			/**
			 * 获取/设置文档的ID
			 */
			void		setDocID(docid_t did){m_docid = did;}
			docid_t		getDocID(){return m_docid;}

			/**
			 * 获取/设置文档的boost值,boost值用于检索时的文档打分,
			 * boost越高,就越能提升文档的分值,默认情况下boost为1.0
			 */
			float		getBoost(){return m_fBoost;}
			void		setBoost(float fBoost){m_fBoost = fBoost;}

			/**
			 * 获取/设置文档的编码格式
			 */
			charset_t	getEncoding(){return m_docEncoding;}
			void		setEncoding(charset_t enc){m_docEncoding = enc;}

			/**
			 * 获取/设置文档的编码格式
			 */
			tstring		getEncodingName(){return charset::typeToName(m_docEncoding);}
			void		setEncodingName(const tchar* szEnc){m_docEncoding = charset::nameToType(szEnc);}

			/**
			 * 获取/设置文档的类型
			 */
			doctype_t	getDocType(){return m_docType;}
			void		setDocType(doctype_t t){m_docType = t;}

			/**
			 * 获取/设置文档的当前处理状态
			 */
			docstatus_t	getStatus(){return m_docStatus;}
			void		setStatus(docstatus_t ds){m_docStatus = ds;}

			/**
			 * 获取文档的Input对象
			 * @return 文档的Input对象，内部维护，外部不能删除
			 */
			CDocumentInput*	getInput(){return m_pInput;}

			/**
			 * 设置文档路径
			 * @param szDocPath 文档路径
			 */
			inline void		setDocPath(schemaid_t _sid,const tchar* szDocPath);			
		public:
			/** 
			 * 获取本Document对象的COM接口
			 */
			firtex::plugin::IDocument*		getInterface();		
		public:
			/**
			 * 设置文档的schema,必须在使用CDocument对象前调用,调用后CDocument内部已存在的字段信息将清除
			 * @param schema schema对象,不作为内部对象
			 */
			void				setSchema(CDocumentSchema* pSchema);

			/**
			 * 获取文档schema的引用
			 * @return 内部schema对象，外部不能删除
			 */
			CDocumentSchema*	getSchema(){return m_pDocSchema;}

			/**
			 * 设置文档的schema,必须在使用CDocument对象前调用,调用后CDocument内部已存在的字段信息将清除
			 * @param schema schema对象,不作为内部对象
			 */
			void				updateSchema(CDocumentSchema* pSchema = NULL);

			/** 
			 * 添加文档字段
			 * @param pField 待添加字段，由内部负责删除
			 */
			int32_t				addField(CField* pField);		
			
			/**
			 * 根据字段编号添加字段值到指定字段，必须在调用{@link applySchema()}设置文档模式后方可调用下列函数，			 
			 * @param id 字段编号，编号值和{@link applySchema(CDocumentSchema* schema)}的schema参数的字段编号一致
			 * @param bNoCopy 是否拷贝对应参数的内容
			 */
			inline void addField(fieldid_t id,const char* value);
			inline void addField(fieldid_t id,const wchar_t* value);
			inline void addField(fieldid_t id,char* value,size_t len,bool bNoCopy);
			inline void addField(fieldid_t id,wchar_t* value,size_t len,bool bNoCopy);
			inline void addField(fieldid_t id,const str_t& value,bool bNoCopy);
			inline void addField(fieldid_t id,const wstr_t& value,bool bNoCopy);
			inline void addField(fieldid_t id,CReader* reader,bool bDelete);
			inline void addField(fieldid_t id,const CNumber& number);
			inline void addField(fieldid_t id,const numbervar& number);
			inline void addField(fieldid_t id,const datarecord& datarec,bool bNoCopy);		

			inline CField* addField(fieldid_t id);		
		public:
			/**
			 * 根据字段名获取字段
			 * @param name 字段名
			 * @return 字段对象,如不存在名为name的字段则返回NULL
			 */
			CField*	getField(const tchar* name);
			CField* operator[](const tchar* name);		

			/**
			 * 根据字段编号获取字段
			 * @param fid 字段编号
			 * @return 字段对象,如不存在则返回NULL
			 */
			CField* getField(fieldid_t fid);

			/**
			 * 根据字段添加时的顺序(注意，不是字段的编号)获取字段
			 * @param pos 字段添加时的顺序号
			 * @return 字段对象,如不存在则返回NULL
			 */
			CField* operator[](int32_t pos);

			/**
			 * 判断文档是否已经修改
			 * @return true:文档已经修改 false 文档未修改
			 */
			bool	isModified(){return m_bModified;}
			
			/**
			 * 设置文档已修改标志
			 * @param bMod 设置还是清除文档标志，true设置,false清除,默认为设置
			 */
			void	setModify(bool bMod = true){m_bModified = bMod;}

			/**
			 * 预先确定字段数目，分配字段数组,如果数组已分配则自动清空
			 * @param numFields 字段数目
			 */ 
			void	reserveFields(int32_t numFields);

			/**
			 * 清除文档的所有字段信息
			 */
			void	clear();

			/**
			 * 以迭代方式访问文档的各个字段
			 */
			void	startIterator(){m_nIterator = 0;}
			bool	hasNext(){return (m_nIterator < m_numFields);}
			CField* next(){return m_ppFields[m_nIterator++].field;}

		protected:
			/**
			 * 将本CDocument对象封装成COM接口
			 * @return 返回COM接口指针,内部对象外部不能删除
			 */
			firtex::plugin::IDocument*	wrapInterface();

			/**
			 * 增长m_ppFields数组
			 */
			void						growFields();

			/**
			 * 清空m_ppInherentFields
			 */
			void						clearInherent();
		private:			
			docid_t					m_docid;			///文档内部ID
			float					m_fBoost;			///文档提升值，值越大检索排序越前
			charset_t		m_docEncoding;		///文档编码
			doctype_t		m_docType;					///文档类型			
			docstatus_t	m_docStatus;					///文档处理状态
			bool					m_bModified;		///文档修改标志

			CDocumentInput*			m_pInput;			///文档的路径标示，文档为Buffer形式时无效
			
			EXTEND_FIELD*			m_ppFields;			///Document包含的字段
			int32_t					m_numFields;		///m_ppFields数组的大小
			int32_t					m_maxFields;

			EXTEND_FIELD*			m_ppInherentFields;	///用于根据schemaid快速定位
			int32_t					m_numInherentFields;///m_ppInherentFields数组大小			

			CDocumentSchema*		m_pDocSchema;		///文档的schema信息
			bool					m_bSchemaOwner;
			int32_t					m_nIterator;		///for iterator

			/// COM接口
			firtex::plugin::IDocument*		m_pInterface;
			
		};

		//////////////////////////////////////////////////////////////////////////
		///CDocumentInput inline functions
		inline void	CDocumentInput::setPathInput(schemaid_t _sid,const tchar* szDocPath)
		{
			if(this->m_pPathInput == NULL)
				this->m_pPathInput = new PathInput(_sid,szDocPath);
			else 
			{
				if(this->m_pPathInput->sid == -1)
					this->m_pPathInput->sid = _sid;
				this->m_pPathInput->docPath.assign(szDocPath);
			}
			if((this->m_pPathInput->sid == -1) && (m_pDelayedSchemaID != &(this->m_pPathInput->sid)) )
			{
				if(m_pDelayedSchemaID)
					FIRTEX_THROW3(ILLEGALARGUMENT_ERROR,_T("More than one delayed schema id for parser!"));
                m_pDelayedSchemaID = &(this->m_pPathInput->sid);			
			}
		}
		inline void	CDocumentInput::setBufferInput(int32_t _off,schemaid_t _sid,char* buf,size_t len)
		{
			if(_off >= this->m_numBufferInput)
				reserveBuffers(_off + 1);
			if(this->m_ppBufferInput[_off] == NULL)
				this->m_ppBufferInput[_off] = new BufferInput(_sid,buf,len);
			else 
			{
				if(this->m_ppBufferInput[_off]->sid == -1)
					this->m_ppBufferInput[_off]->sid = _sid;
				this->m_ppBufferInput[_off]->buffer = buf;
				this->m_ppBufferInput[_off]->length = len;
			}
			if((this->m_ppBufferInput[_off]->sid == -1) && (m_pDelayedSchemaID != &(this->m_ppBufferInput[_off]->sid)) )
			{
				if(m_pDelayedSchemaID)
					FIRTEX_THROW3(ILLEGALARGUMENT_ERROR,_T("More than one delayed schema id for parser!"));
				m_pDelayedSchemaID = &(this->m_ppBufferInput[_off]->sid);
			}
		}
		inline void	CDocumentInput::setReaderInput(int32_t _off,schemaid_t _sid,CReader* _reader)
		{
			if(_off >= this->m_numBufferInput)
				reserveReaders(_off + 1);
			if(this->m_ppReaderInput[_off] == NULL)
				this->m_ppReaderInput[_off] = new ReaderInput(_sid,_reader);
			else 
			{
				if(this->m_ppReaderInput[_off]->sid == -1)
					this->m_ppReaderInput[_off]->sid = _sid;
				this->m_ppReaderInput[_off]->reader = _reader;
			}
			if((this->m_ppReaderInput[_off]->sid == -1) && (m_pDelayedSchemaID != &(this->m_ppReaderInput[_off]->sid)) )
			{
				if(m_pDelayedSchemaID)
					FIRTEX_THROW3(ILLEGALARGUMENT_ERROR,_T("More than one delayed schema id for parser!"));
				m_pDelayedSchemaID = &(this->m_ppReaderInput[_off]->sid);
			}
		}
		inline bool	CDocumentInput::hasDelayedSchema()
		{
			return ((m_pDelayedSchemaID != NULL) && (*m_pDelayedSchemaID == -1) );
		}
		inline void CDocumentInput::setDelayedSchema(schemaid_t _sid)
		{
			if(m_pDelayedSchemaID)
				*m_pDelayedSchemaID = _sid;
		}
		inline CDocumentInput::PathInput* CDocumentInput::getPathInput()
		{
			return m_pPathInput;
		}
		inline CDocumentInput::BufferInput** CDocumentInput::getBufferInputs(int32_t& numBuffers)
		{
			numBuffers = m_numBufferInput;
			return m_ppBufferInput;
		}
		
		inline CDocumentInput::ReaderInput** CDocumentInput::getReaderInputs(int32_t& numReaders)
		{
			numReaders = m_numReaderInput;
			return m_ppReaderInput;
		}

		//////////////////////////////////////////////////////////////////////////
		//CDocuemnt inline functions
		inline void CDocument::setDocPath(schemaid_t _sid,const tchar* szDocPath)
		{
			if(m_pInput == NULL)
				m_pInput = new CDocumentInput(_sid,szDocPath);
			m_pInput->setPathInput(_sid,szDocPath);
		}		

		inline int32_t CDocument::addField(CField* pField)
		{	
			fieldid_t fid = pField->getID();
			FIRTEX_ASSERT((( fid >= 0) && (fid < m_numInherentFields)),_T("field id is out of order."));
			bool bSetDelBit = true;
			if(m_ppInherentFields[fid].field == NULL)
			{
				m_ppInherentFields[fid].field = pField;
				m_ppInherentFields[fid].extend = true;		/// set in-use bit
				bSetDelBit = false;							///set no delete bit
			}	

			if(m_numFields == m_maxFields)
				growFields();
			m_ppFields[m_numFields].field = pField;
			m_ppFields[m_numFields++].extend = bSetDelBit;
			
			setModify();
			return (m_numFields - 1);
		}
		inline CField* CDocument::getField(fieldid_t fid)
		{
			for (int32_t i = 0;i<m_numFields;i++)
			{
				if(m_ppFields[i].field->getID() == fid)
					return m_ppFields[i].field;
			}			
			return NULL;
		}
		inline CField* CDocument::operator[](int32_t pos)
		{
			if (pos < 0 || pos >= m_numFields)
				return NULL;
			return m_ppFields[pos].field;			
		}
		inline CField* CDocument::operator[](const tchar* name)
		{
			return getField(name);
		}		
	
		inline CField* CDocument::addField(fieldid_t id)
		{
			FIRTEX_ASSERT( (id >=0 && id < m_numInherentFields),_T("field id is out of range"));
			CField* pField = NULL;
			bool bSetDelBit = false;
			if(m_ppInherentFields[id].extend == false)///free 
			{
				pField = m_ppInherentFields[id].field;
				m_ppInherentFields[id].extend = true;		/// set in-use bit
			}
			else  /// in use 
			{
				pField = m_ppInherentFields[id].field->clone();
				bSetDelBit = true; /// need to delete when call clear().
			}			
			if(m_numFields == m_maxFields)
				growFields();
			m_ppFields[m_numFields].field = pField;
			m_ppFields[m_numFields++].extend = bSetDelBit;
			return pField;
		}
		inline void CDocument::addField(fieldid_t id,const char* value)
		{
			FIRTEX_ASSERT( (id >=0 && id < m_numInherentFields),_T("field id is out of range"));
			CField* pField = NULL;
			bool bSetDelBit = false;
			if(m_ppInherentFields[id].extend == false)///free 
			{
				pField = m_ppInherentFields[id].field;
				m_ppInherentFields[id].extend = true;		/// set in-use bit
			}
			else  /// in use 
			{
				pField = m_ppInherentFields[id].field->clone();
				bSetDelBit = true; /// need to delete when call clear().
			}			
			pField->textValue((char*)value,strlen(value),false);
            
			if(m_numFields == m_maxFields)
				growFields();
			m_ppFields[m_numFields].field = pField;
			m_ppFields[m_numFields++].extend = bSetDelBit;

		}
		inline void CDocument::addField(fieldid_t id,const wchar_t* value)
		{
			FIRTEX_ASSERT( (id >=0 && id < m_numInherentFields),_T("field id is out of range"));
			CField* pField = NULL;
			bool bSetDelBit = false;
			if(m_ppInherentFields[id].extend == false)///free 
			{
				pField = m_ppInherentFields[id].field;
				m_ppInherentFields[id].extend = true;		/// set in-use bit
			}
			else  /// in use 
			{
				pField = m_ppInherentFields[id].field->clone();
				bSetDelBit = true; /// need to delete when call clear().
			}	
			pField->textValue((wchar_t*)value,wcslen(value),false);
			if(m_numFields == m_maxFields)
				growFields();
			m_ppFields[m_numFields].field = pField;
			m_ppFields[m_numFields++].extend = bSetDelBit;
		}
		inline void CDocument::addField(fieldid_t id,char* value,size_t len,bool bNoCopy)
		{
			FIRTEX_ASSERT( (id >=0 && id < m_numInherentFields),_T("field id is out of range"));
			CField* pField = NULL;
			bool bSetDelBit = false;
			if(m_ppInherentFields[id].extend == false)///free 
			{
				pField = m_ppInherentFields[id].field;
				m_ppInherentFields[id].extend = true;
			}
			else  /// in use 
			{
				pField = m_ppInherentFields[id].field->clone();
				bSetDelBit = true; /// need to delete when call clear().
			}	
			pField->textValue(value,len,bNoCopy);
			if(m_numFields == m_maxFields)
				growFields();
			m_ppFields[m_numFields].field = pField;
			m_ppFields[m_numFields++].extend = bSetDelBit;
		}
		inline void CDocument::addField(fieldid_t id,wchar_t* value,size_t len,bool bNoCopy)
		{
			FIRTEX_ASSERT( (id >=0 && id < m_numInherentFields),_T("field id is out of range"));
			CField* pField = NULL;
			bool bSetDelBit = false;
			if(m_ppInherentFields[id].extend == false)///free 
			{
				pField = m_ppInherentFields[id].field;
				m_ppInherentFields[id].extend = true;		/// set in-use bit
			}
			else  /// in use 
			{
				pField = m_ppInherentFields[id].field->clone();
				bSetDelBit = true; /// need to delete when call clear().
			}	
			pField->textValue(value,len,bNoCopy);

			if(m_numFields == m_maxFields)
				growFields();
			m_ppFields[m_numFields].field = pField;
			m_ppFields[m_numFields++].extend = bSetDelBit;
		}

		inline void CDocument::addField(fieldid_t id,const str_t& value,bool bNoCopy)
		{
			FIRTEX_ASSERT( (id >=0 && id < m_numInherentFields),_T("field id is out of range"));
			CField* pField = NULL;
			bool bSetDelBit = false;
			if(m_ppInherentFields[id].extend == false)///free 
			{
				pField = m_ppInherentFields[id].field;
				m_ppInherentFields[id].extend = true;
			}
			else  /// in use 
			{
				pField = m_ppInherentFields[id].field->clone();
				bSetDelBit = true; /// need to delete when call clear().
			}	
			pField->textValue(value,bNoCopy);

			if(m_numFields == m_maxFields)
				growFields();
			m_ppFields[m_numFields].field = pField;
			m_ppFields[m_numFields++].extend = bSetDelBit;
		}
		inline void CDocument::addField(fieldid_t id,const wstr_t& value,bool bNoCopy)
		{
			FIRTEX_ASSERT( (id >=0 && id < m_numInherentFields),_T("field id is out of range"));
			CField* pField = NULL;
			bool bSetDelBit = false;
			if(m_ppInherentFields[id].extend == false)///free 
			{
				pField = m_ppInherentFields[id].field;
				m_ppInherentFields[id].extend = true;		/// set in-use bit
			}
			else  /// in use 
			{
				pField = m_ppInherentFields[id].field->clone();
				bSetDelBit = true; /// need to delete when call clear().
			}	
			pField->textValue(value,bNoCopy);

			if(m_numFields == m_maxFields)
				growFields();
			m_ppFields[m_numFields].field = pField;
			m_ppFields[m_numFields++].extend = bSetDelBit;
		}
		inline void CDocument::addField(fieldid_t id,CReader* reader,bool bDelete)
		{
			FIRTEX_ASSERT( (id >=0 && id < m_numInherentFields),_T("field id is out of range"));
			CField* pField = NULL;
			bool bSetDelBit = false;
			if(m_ppInherentFields[id].extend == false)///free 
			{
				pField = m_ppInherentFields[id].field;
				m_ppInherentFields[id].extend = true;		/// set in-use bit
			}
			else  /// in use 
			{
				pField = m_ppInherentFields[id].field->clone();
				bSetDelBit = true; /// need to delete when call clear().
			}	
			pField->readerValue(reader,bDelete);

			if(m_numFields == m_maxFields)
				growFields();
			m_ppFields[m_numFields].field = pField;
			m_ppFields[m_numFields++].extend = bSetDelBit;
		}
		inline void CDocument::addField(fieldid_t id,const CNumber& number)
		{
			FIRTEX_ASSERT( (id >=0 && id < m_numInherentFields),_T("field id is out of range"));
			CField* pField = NULL;
			bool bSetDelBit = false;
			if(m_ppInherentFields[id].extend == false)///free 
			{
				pField = m_ppInherentFields[id].field;
				m_ppInherentFields[id].extend = true;		/// set in-use bit
			}
			else  /// in use 
			{
				pField = m_ppInherentFields[id].field->clone();
				bSetDelBit = true; /// need to delete when call clear().
			}	
			if(pField->isAnalyzed())
			{
				FIRTEX_CLOG(level::warn) << _T("CDocument::addField(fieldid_t id,const CNumber& number):Number value can't analyze,discard it.") <<FIRTEX_ENDL;
				return;
			}	
			pField->numberValue(number);

			if(m_numFields == m_maxFields)
				growFields();
			m_ppFields[m_numFields].field = pField;
			m_ppFields[m_numFields++].extend = bSetDelBit;
		}
		inline void CDocument::addField(fieldid_t id,const numbervar& number)
		{
			FIRTEX_ASSERT( (id >=0 && id < m_numInherentFields),_T("field id is out of range"));
			CField* pField = NULL;
			bool bSetDelBit = false;
			if(m_ppInherentFields[id].extend == false)///free 
			{
				pField = m_ppInherentFields[id].field;
				m_ppInherentFields[id].extend = true;		/// set in-use bit
			}
			else  /// in use 
			{
				pField = m_ppInherentFields[id].field->clone();
				bSetDelBit = true; /// need to delete when call clear().
			}	
			if(pField->isAnalyzed())
			{
				FIRTEX_CLOG(level::warn) << "CDocument::addField(fieldid_t id,const numbervar& number):Number value can't analyze,discard it." <<FIRTEX_ENDL;
				return;
			}	
			pField->numberValue(number);

			if(m_numFields == m_maxFields)
				growFields();
			m_ppFields[m_numFields].field = pField;
			m_ppFields[m_numFields++].extend = bSetDelBit;
		}
		inline void CDocument::addField(fieldid_t id,const datarecord& datarec,bool bNoCopy)
		{
			FIRTEX_ASSERT( (id >=0 && id < m_numInherentFields),_T("field id is out of range"));
			CField* pField = NULL;
			bool bSetDelBit = false;
			if(m_ppInherentFields[id].extend == false)///free 
			{
				pField = m_ppInherentFields[id].field;
				m_ppInherentFields[id].extend = true;		/// set in-use bit
			}
			else  /// in use 
			{
				pField = m_ppInherentFields[id].field->clone();
				bSetDelBit = true; /// need to delete when call clear().
			}	
			if(pField->isAnalyzed())
			{
				FIRTEX_CLOG(level::warn) << "CDocument::addField(fieldid_t id,const datarecord& datarec,bool bNoCopy):Binary value can't analyze,discard it." <<FIRTEX_ENDL;
				return;
			}	
			pField->binaryValue(datarec,bNoCopy);

			if(m_numFields == m_maxFields)
				growFields();
			m_ppFields[m_numFields].field = pField;
			m_ppFields[m_numFields++].extend = bSetDelBit;
		}
	}
}

#endif
