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
			 * �����ĵ�·��
			 * @param sid ·�������ֶε�schema id,��CDocumentSchema����
			 * @param szDocPath �ĵ�·��
			 */
			inline void	setPathInput(schemaid_t sid,const tchar* szDocPath);
			
			/** 
			 * �����ĵ���Buffer
			 * @param _off ��Buffer�����е�λ��
			 * @param sid Buffer���ֶε�schema id,��CDocumentSchema����
			 * @param buf Buffer,�ڲ�����������������ָ��
			 */
			inline void	setBufferInput(int32_t _off,schemaid_t sid,char* buf,size_t len);

			/** 
			 * �����ĵ���Reader
			 * @param _off ��Reader�����е�λ��
			 * @param sid Reader���ֶε�schema id,��CDocumentSchema����
			 * @param _reader Reader,�ڲ�����������������ָ��
			 */
			inline void	setReaderInput(int32_t _off,schemaid_t sid,CReader* _reader);

			/**
			 * ��ȡPathInput			 
			 * @return PathInput����,�ڲ������ⲿ����ɾ��
			 */
			inline PathInput*	getPathInput();
			/**
			 * ��ȡBufferInput
			 * @param numBuffers BufferInput��Ŀ
			 * @return BufferInput����,�ڲ������ⲿ����ɾ��
			 */
			inline BufferInput**	getBufferInputs(int32_t& numBuffers);

			/**
			 * ��ȡReaderInput
			 * @param numReaders ReaderInput��Ŀ
			 * @return ReaderInput����,�ڲ������ⲿ����ɾ��
			 */
			inline ReaderInput**	getReaderInputs(int32_t& numReaders);


			/** 
			 * Ԥ��Buffer����ռ�
			 * @param _count Ԥ�������С
			 */
			void	reserveBuffers(int32_t _count);

			/** 
			 * Ԥ��Reader����ռ�
			 * @param _count Ԥ�������С
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
			PathInput*			m_pPathInput;			///һ���ĵ�ֻ��һ���ĵ�·��

			BufferInput**		m_ppBufferInput;		///һ���ĵ������ж��Buffer
			int32_t				m_numBufferInput;		///���鳤��

			ReaderInput**		m_ppReaderInput;		///һ���ĵ������ж��Reader
			int32_t				m_numReaderInput;		///���鳤��

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
			 * ��ȡ/�����ĵ���ID
			 */
			void		setDocID(docid_t did){m_docid = did;}
			docid_t		getDocID(){return m_docid;}

			/**
			 * ��ȡ/�����ĵ���boostֵ,boostֵ���ڼ���ʱ���ĵ����,
			 * boostԽ��,��Խ�������ĵ��ķ�ֵ,Ĭ�������boostΪ1.0
			 */
			float		getBoost(){return m_fBoost;}
			void		setBoost(float fBoost){m_fBoost = fBoost;}

			/**
			 * ��ȡ/�����ĵ��ı����ʽ
			 */
			charset_t	getEncoding(){return m_docEncoding;}
			void		setEncoding(charset_t enc){m_docEncoding = enc;}

			/**
			 * ��ȡ/�����ĵ��ı����ʽ
			 */
			tstring		getEncodingName(){return charset::typeToName(m_docEncoding);}
			void		setEncodingName(const tchar* szEnc){m_docEncoding = charset::nameToType(szEnc);}

			/**
			 * ��ȡ/�����ĵ�������
			 */
			doctype_t	getDocType(){return m_docType;}
			void		setDocType(doctype_t t){m_docType = t;}

			/**
			 * ��ȡ/�����ĵ��ĵ�ǰ����״̬
			 */
			docstatus_t	getStatus(){return m_docStatus;}
			void		setStatus(docstatus_t ds){m_docStatus = ds;}

			/**
			 * ��ȡ�ĵ���Input����
			 * @return �ĵ���Input�����ڲ�ά�����ⲿ����ɾ��
			 */
			CDocumentInput*	getInput(){return m_pInput;}

			/**
			 * �����ĵ�·��
			 * @param szDocPath �ĵ�·��
			 */
			inline void		setDocPath(schemaid_t _sid,const tchar* szDocPath);			
		public:
			/** 
			 * ��ȡ��Document�����COM�ӿ�
			 */
			firtex::plugin::IDocument*		getInterface();		
		public:
			/**
			 * �����ĵ���schema,������ʹ��CDocument����ǰ����,���ú�CDocument�ڲ��Ѵ��ڵ��ֶ���Ϣ�����
			 * @param schema schema����,����Ϊ�ڲ�����
			 */
			void				setSchema(CDocumentSchema* pSchema);

			/**
			 * ��ȡ�ĵ�schema������
			 * @return �ڲ�schema�����ⲿ����ɾ��
			 */
			CDocumentSchema*	getSchema(){return m_pDocSchema;}

			/**
			 * �����ĵ���schema,������ʹ��CDocument����ǰ����,���ú�CDocument�ڲ��Ѵ��ڵ��ֶ���Ϣ�����
			 * @param schema schema����,����Ϊ�ڲ�����
			 */
			void				updateSchema(CDocumentSchema* pSchema = NULL);

			/** 
			 * ����ĵ��ֶ�
			 * @param pField ������ֶΣ����ڲ�����ɾ��
			 */
			int32_t				addField(CField* pField);		
			
			/**
			 * �����ֶα������ֶ�ֵ��ָ���ֶΣ������ڵ���{@link applySchema()}�����ĵ�ģʽ�󷽿ɵ������к�����			 
			 * @param id �ֶα�ţ����ֵ��{@link applySchema(CDocumentSchema* schema)}��schema�������ֶα��һ��
			 * @param bNoCopy �Ƿ񿽱���Ӧ����������
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
			 * �����ֶ�����ȡ�ֶ�
			 * @param name �ֶ���
			 * @return �ֶζ���,�粻������Ϊname���ֶ��򷵻�NULL
			 */
			CField*	getField(const tchar* name);
			CField* operator[](const tchar* name);		

			/**
			 * �����ֶα�Ż�ȡ�ֶ�
			 * @param fid �ֶα��
			 * @return �ֶζ���,�粻�����򷵻�NULL
			 */
			CField* getField(fieldid_t fid);

			/**
			 * �����ֶ����ʱ��˳��(ע�⣬�����ֶεı��)��ȡ�ֶ�
			 * @param pos �ֶ����ʱ��˳���
			 * @return �ֶζ���,�粻�����򷵻�NULL
			 */
			CField* operator[](int32_t pos);

			/**
			 * �ж��ĵ��Ƿ��Ѿ��޸�
			 * @return true:�ĵ��Ѿ��޸� false �ĵ�δ�޸�
			 */
			bool	isModified(){return m_bModified;}
			
			/**
			 * �����ĵ����޸ı�־
			 * @param bMod ���û�������ĵ���־��true����,false���,Ĭ��Ϊ����
			 */
			void	setModify(bool bMod = true){m_bModified = bMod;}

			/**
			 * Ԥ��ȷ���ֶ���Ŀ�������ֶ�����,��������ѷ������Զ����
			 * @param numFields �ֶ���Ŀ
			 */ 
			void	reserveFields(int32_t numFields);

			/**
			 * ����ĵ��������ֶ���Ϣ
			 */
			void	clear();

			/**
			 * �Ե�����ʽ�����ĵ��ĸ����ֶ�
			 */
			void	startIterator(){m_nIterator = 0;}
			bool	hasNext(){return (m_nIterator < m_numFields);}
			CField* next(){return m_ppFields[m_nIterator++].field;}

		protected:
			/**
			 * ����CDocument�����װ��COM�ӿ�
			 * @return ����COM�ӿ�ָ��,�ڲ������ⲿ����ɾ��
			 */
			firtex::plugin::IDocument*	wrapInterface();

			/**
			 * ����m_ppFields����
			 */
			void						growFields();

			/**
			 * ���m_ppInherentFields
			 */
			void						clearInherent();
		private:			
			docid_t					m_docid;			///�ĵ��ڲ�ID
			float					m_fBoost;			///�ĵ�����ֵ��ֵԽ���������Խǰ
			charset_t		m_docEncoding;		///�ĵ�����
			doctype_t		m_docType;					///�ĵ�����			
			docstatus_t	m_docStatus;					///�ĵ�����״̬
			bool					m_bModified;		///�ĵ��޸ı�־

			CDocumentInput*			m_pInput;			///�ĵ���·����ʾ���ĵ�ΪBuffer��ʽʱ��Ч
			
			EXTEND_FIELD*			m_ppFields;			///Document�������ֶ�
			int32_t					m_numFields;		///m_ppFields����Ĵ�С
			int32_t					m_maxFields;

			EXTEND_FIELD*			m_ppInherentFields;	///���ڸ���schemaid���ٶ�λ
			int32_t					m_numInherentFields;///m_ppInherentFields�����С			

			CDocumentSchema*		m_pDocSchema;		///�ĵ���schema��Ϣ
			bool					m_bSchemaOwner;
			int32_t					m_nIterator;		///for iterator

			/// COM�ӿ�
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
