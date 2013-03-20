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
// Created	: 2005/12/07
//

#ifndef _INDEX_H
#define _INDEX_H

#include "utility/StdHeader.h"
#include "../document/Document.h"
#include "../store/Directory.h"
#include "IndexWriter.h"
#include "IndexReader.h"

using namespace firtex;
using namespace firtex::store;

namespace firtex
{
	namespace index
	{		
		class CNormBytes;
		class CBarrelsInfo;
		class CIndex
		{
		public:
			typedef int32_t ACCESS_MODE;

			///��������ģʽ����			
			const static int32_t ACCESS_READONLY	= 0x00;		/// ֻ�������Զ��������������ݿ⣬���ǲ����޸ģ���Ӻ�ɾ���ļ���
																	/// read only,can read,search it but can't modify it,i.e.add files or delete files.
			const static int32_t ACCESS_CREATE		= 0x10;		/// ������������ӣ�ɾ���ĵ����ܶ�ȡ�����Ĳ������ݣ������ĵ�������ͳ����Ϣ�������ǲ��ܽ�����������������������ݿ��Ѿ��������Զ�ɾ��
																	/// write only,can add or delete files but can't search over it,will delete existed index database				
			const static int32_t ACCESS_APPEND		= 0x20;		/// �������������ǲ�ͬ��ACCESS_WRITE������������ݿ��Ѿ����ڣ���׷�����������ݶ���ɾ��������������
																	/// write only,but unlike ACCESS_WRITE,will append index data do existed index database.
			const static int32_t ACCESS_MODIFY		= 0x40;		/// �޸ģ����Ѵ��ڵ��������ݣ���������Ӧ���޸ģ����������ϲ���ɾ���ĵ���
																	/// modify, open exist database, can modify database,etc.merge index,delete documents
			const static int32_t ACCESS_SEARCH		= 0x01;		/// ���������Ѵ��ڵ��������ݣ����Զ�ȡ������Ϣ�������������޸�
																	/// search, open exist database, can read/search database, but can't modify it.
						
		public:
			CIndex(const tchar* indName,ACCESS_MODE am);
			CIndex(CDirectory* pDirectory,ACCESS_MODE am);
			CIndex(void);
			virtual ~CIndex(void);
		public:		
			/**
			 * ��ȡ�����İ汾��
			 * @return �汾���ַ�������ʽ������.�汾��
			 */
			tstring	getVersion(){return FIRTEX_VERSION;}

			/**
			 * ������
			 * @param name ����·��
			 * @param am �����ķ��ʷ�ʽ
			 */
			void	open(const tchar* path,ACCESS_MODE am);

			/**
			 * ������
			 * @param pDirectory �����洢�豸
			 * @param am �����ķ��ʷ�ʽ
			 */
			void	open(CDirectory* pDirectory,ACCESS_MODE am);

			/**
			 * �ر������ͷ���Դ
			 */
			void	close();										

			/**
			 * �����ĵ���schema,�����ڵ���acquireWriter()ǰ����,���ú�ɵ�schema��Ϣ�����
			 * @param pSchema schema����ָ��,����Ϊ�ڲ�����
			 */
			void				setDocSchema(CDocumentSchema* pSchema);

			/** ��ȡ�����ĵ�Schema */
			CDocumentSchema*	getSchema(){return m_pDocSchema;}

			/**
			 * ��ȡ������directory����
			 */
			CDirectory*		getDirectory(){return m_pDirectory;}

			/** 
			 * ������������
			 */
			void			setDirectory(CDirectory* pDir);

			/**
			 * ��ȡ����Ͱ��Ϣ
			 */
			CBarrelsInfo*	getBarrelsInfo(){return m_pBarrelsInfo;}

			/** get access mode */
			ACCESS_MODE		getAccessMode(){return m_accessMode;}

			/**
			 * is index modified?
			 * @return true if modified, otherwise false
			 */
			bool			isDirty(){return m_bDirty;}

			/**
			 * set or clear index modification state
			 * @param bDirty modification state
			 */
			void			setDirty(bool bDirty = true);

			/**
			 * ��ȡ������reader���󣬽���ACCESS_MODE = ACCESS_READONLY ��
			 * ACCESS_READWRITE �� ACCESS_READAPPENDʱ������Ч����
			 * @return reader���������
			 * @memory �ڲ������ⲿ����ɾ��
			 */
			CIndexReader*	acquireReader();

			/**
			 * ��ȡ������writer���󣬽���ACCESS_MODE = ACCESS_WRITE ��
			 * ACCESS_READWRITE �� ACCESS_APPEND��ACCEACCESS_READAPPENDʱ������Ч����
			 * @param pParser �ı���ʽ������,���ڲ�ά��
			 * @param pAnalyzer �ı����ݷ����������ڲ�ά��
			 * @param ms �����ϲ�����
			 * @return writer��������ã��ڲ������ⲿ����ɾ��			 
			 */
			CIndexWriter*	acquireWriter(CParser* pParser,CAnalyzer* pAnalyzer);

			/**
			 * ��ȡ������writer���󣬽���ACCESS_MODE = ACCESS_WRITE ��
			 * ACCESS_READWRITE �� ACCESS_APPEND��ACCEACCESS_READAPPENDʱ������Ч����			 
			 * @param ms �����ϲ�����
			 * @return writer��������ã��ڲ������ⲿ����ɾ��			 
			 */
			CIndexWriter*	acquireWriter();
		protected:
			/** �����������򹤳� */
			CFactoryFinder*		createFactoryFinderFromExistIndex();

			/** �����������ĵ�ģʽ */
			CDocumentSchema*	createSchemaFromExistIndex();

			/** ��ȡ�򹤳�*/
			CFactoryFinder*		getFactoryFinder();	
		protected:
			CDirectory*			m_pDirectory;		///�����洢λ��
            CIndexReader*		m_pReader;			///����������
			CIndexWriter*		m_pWriter;			///����д����
			CBarrelsInfo*		m_pBarrelsInfo;		///����ȫ����Ϣ
			ACCESS_MODE			m_accessMode;		///������ȡģʽ
			bool				m_bDirty;			///�����Ƿ���й��޸�

			CDocumentSchema*	m_pDocSchema;		///�ĵ���schema��Ϣ

			CFactoryFinder*		m_pFactoryFinder;	///field component factory finder		

			friend class CIndexWriter;
			friend class CIndexReader;
			friend class CIndexBarrelReader;
		};
	}
}

#endif
