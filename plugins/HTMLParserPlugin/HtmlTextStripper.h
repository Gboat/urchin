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
// Created	: 2006/5/5
//

#ifndef _HTMLTEXTSTRIPPER_H
#define _HTMLTEXTSTRIPPER_H


#include "HTMLParser/WWWLib.h"
#include "HTMLParser/WWWInit.h"
#include "HTMLParser/WWWHTML.h"

#include <string>
#include <list>
using namespace std;



class CHtmlTextStripper;
class CContext
{
public:
	CContext(HTRequest* request,CHtmlTextStripper* text):m_pRequest(request),m_text(text)
	{		
		HTRequest_setContext(request, (void *)this); 			
	}
	~CContext()
	{
		m_text = NULL;
		m_pRequest = NULL;
	}
public:
	
	inline static CHtmlTextStripper* getHtmlText(HTRequest * request)
	{
		return ((CContext *) HTRequest_context(request))->m_text;
	}
	inline CHtmlTextStripper* getHtmlText()
	{
		return ((CContext *) HTRequest_context(m_pRequest))->m_text;
	}
protected:
	HTRequest *		m_pRequest;
	CHtmlTextStripper*		m_text;			
};		

typedef struct _TextAnchor 
{
	struct _TextAnchor *	next;
	int						number;		/* For user interface */
	int						start;		/* Characters */
	int						extent;		/* Characters */
	HTChildAnchor *			anchor;
} TextAnchor;


#define MAX_META 5

class CHtmlTextStripper
{
public:
	typedef enum _HtmlMeta
	{
		META_TITLE		= 0,
		META_KEYWORDS,
		META_DESCRIPTION,	
		META_CHARSET,
		META_TEXT,
	}HtmlMeta;
public:
	CHtmlTextStripper(void);
	~CHtmlTextStripper(void);
public:
	bool		parse(const char* filename);
	bool		parse(const string& filename);

	bool		parse(char* text,size_t len);

	/*bool	parseURL(const char* url);
	bool	parseURL(const string& url);*/

	void		reset();

	/**
	 * get text of html
	 * @param length length of text
	 * @return the text
	 */
	const char*	getText(size_t& length);	

	/**
	 * get meta data of html
	 * @param length length of text
	 * @return the text
	 */
	const char*	getMeta(HtmlMeta _meta,size_t& length);

	/** 
	 * extract a metadata or not
	 * @param _meta meta type
	 * @param bPresent extract meta data or not
	 */
	void		setMetaPresent(HtmlMeta _meta,bool bPresent);

	/** 
	 * extract a metadata or not
	 * @param _meta meta type
	 * @return true for extracting the meta data
	 */
	inline bool		getMetaPresent(HtmlMeta _meta);
protected:	
	inline size_t	outputData(const char* buf,size_t len,HtmlMeta type = META_TEXT);
	inline void		growMetaBuffer(size_t len = 0);
	inline void		growTextBuffer(size_t len = 0);
	inline void		extractCharset(const char* szCharset);	
protected:
	void			addAnchor(TextAnchor * anc);
public:
	static void init();
	static void uninit();
public:
	static HText*	newText (HTRequest * request,HTParentAnchor * anchor,HTStream *outstrm);
	static BOOL		deleteText(HText * self);
	static void		addText (HText * text, const char * str, int length);	
	static void		beginAnchor (HText * text,int elem_num, int attr_num, HTChildAnchor * anc,const BOOL *present, const char **value);
	static void		endAnchor (HText * text);
	static void		beginElement (HText * text,int elem_num, const BOOL * present, const char ** value);
	static void		endElement (HText * text, int elem_num);

	static int		requestTerminater (HTRequest * request, HTResponse * response,void * param, int status);

	static HTParentAnchor* nodeAnchor (HText * text);
protected:
	HTRequest*		m_pRequest;
	CContext*		m_pContext;
	HTAnchor*		m_pAnchor;
	HText*			m_pCurrentText;

	list<TextAnchor*> m_anchorList;

	char*			m_pMetaBuffer;
	size_t			m_nMetaBufSize;
	size_t			m_nMetaBufUsed;	

	char*			m_pTextBuffer;
	size_t			m_nTextBufSize;
	size_t			m_nTextBufUsed;	

	size_t			m_metaOffsets[MAX_META];		///offset of meta in m_pMetaBuffer
	size_t			m_metaLengths[MAX_META];		///length of meta
	bool			m_metaPresents[MAX_META];		///need meta data or not, true for yes	
	bool			m_bNeedAnchor;					//是否需要Anchor
	bool			m_bNeedAnchorText;				//是否需要锚文本

	static int		m_nRef;
};

//////////////////////////////////////////////////////////////////////////
//inline functions
inline size_t CHtmlTextStripper::outputData(const char* buf,size_t len,HtmlMeta type)
{			
	if(type == META_TEXT)
	{
		size_t newLen = m_nTextBufUsed + len + 1;
		if( newLen > m_nTextBufSize)
		{				
			growTextBuffer(newLen);
		}	
		memcpy(m_pTextBuffer + m_nTextBufUsed,buf,len);		
		m_nTextBufUsed += len;		
	}
	else 
	{
		size_t newLen = m_nMetaBufUsed + len + 1;
		if( newLen > m_nMetaBufSize)
		{				
			growMetaBuffer(newLen);
		}	
		memcpy(m_pMetaBuffer + m_nMetaBufUsed,buf,len);
		if(m_metaLengths[type] == 0)
			m_metaOffsets[type] = m_nMetaBufUsed;		
		m_nMetaBufUsed += len;
		m_pMetaBuffer[m_nMetaBufUsed++] = 0;
		m_metaLengths[type] += len;
	}	

	return len;
}
inline void CHtmlTextStripper::growMetaBuffer(size_t len)
{
	size_t newLen = m_nMetaBufSize << 2;
	if(len > newLen)
		newLen = len;
	char* tmp = new char[newLen];
	if(m_nMetaBufUsed > 0)
		memcpy(tmp,m_pMetaBuffer,m_nMetaBufUsed);
	delete[] m_pMetaBuffer;
	m_pMetaBuffer = tmp;
	m_nMetaBufSize = newLen;
}
inline void CHtmlTextStripper::growTextBuffer(size_t len)
{
	size_t newLen = m_nTextBufSize << 2;
	if(len > newLen)
		newLen = len;
	char* tmp = new char[newLen];
	if(m_nTextBufUsed > 0)
		memcpy(tmp,m_pTextBuffer,m_nTextBufUsed);
	delete[] m_pTextBuffer;
	m_pTextBuffer = tmp;
	m_nTextBufSize = newLen;
}
inline void CHtmlTextStripper::extractCharset(const char* szCharset)
{
	char* tmp = (char*)szCharset;
	while(*tmp)
	{
		if(*tmp >= 'A' && *tmp <= 'Z')
			*tmp += 'a' - 'A';
		tmp++;
	}	
	tmp = strstr((char*)szCharset,"charset");
	if(tmp)
	{
		tmp += 7;
		while(*tmp && *tmp != '=')
			tmp++;	
		tmp++;
		size_t len = strlen(tmp);		
		if( m_nMetaBufUsed + len > m_nMetaBufSize)
		{				
			growMetaBuffer(m_nMetaBufUsed + len);
		}	
		strcpy(m_pMetaBuffer + m_nMetaBufUsed,tmp);
		m_pMetaBuffer[m_nMetaBufUsed + len] = 0;
		m_metaOffsets[META_CHARSET] = m_nMetaBufUsed;
		m_metaLengths[META_CHARSET] = len;
		m_nMetaBufUsed += len + 1;
	}
}

inline const char* CHtmlTextStripper::getMeta(HtmlMeta _meta,size_t& length)
{
	if(_meta == META_TEXT)
		return getText(length);
	length = m_metaLengths[_meta];
	if(length <= 0)
		return NULL;
	return (const char*)(m_pMetaBuffer + m_metaOffsets[_meta]);	
}
inline const char* CHtmlTextStripper::getText(size_t& length)
{
	length = m_nTextBufUsed;
	if(length)
		return m_pTextBuffer;	
	return NULL;
}

inline bool CHtmlTextStripper::getMetaPresent(HtmlMeta _meta)
{
	return m_metaPresents[_meta];
}

#endif
