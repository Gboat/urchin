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

#include <iostream>


#include "HtmlTextStripper.h"

using namespace std;

#ifdef WIN32
#define strcasecmp stricmp
#endif


typedef struct _HTStream 
{			
	const HTStreamClass *	isa;			
}HTStream;


/**	Notes on struct _Htext:
 *	next_line is valid iff state is false.
 *	top_of_screen line means the line at the top of the screen
 *			or just under the title if there is one.
 */
typedef struct _HText 
{
	HTParentAnchor *	anchor;
	CHtmlTextStripper*	pStripper;			
	size_t				numChars;			//字符数	
	int					lastAnchorNum;		/* user number */
	TextAnchor *		currentAnchor;
	bool				bSkip;	
	bool				bIsTitle;
}HText;


//////////////////////////////////////////////////////////////////////////
//CHtmlTextStripper

int CHtmlTextStripper::m_nRef = 0;

CHtmlTextStripper::CHtmlTextStripper(void)
{			
	m_nMetaBufSize = 1024;
	m_pMetaBuffer = new char[m_nMetaBufSize];
	m_nMetaBufUsed = 0;

	m_nTextBufSize = 32*1024;
	m_pTextBuffer = new char[m_nTextBufSize];
	m_nTextBufUsed = 0;
	

	m_pRequest = HTRequest_new();
	m_pContext = new CContext(m_pRequest,this);
	m_pAnchor = NULL;

	m_bNeedAnchor = true;
	m_bNeedAnchorText = true;	//是否需要锚文本

	m_pCurrentText = NULL;

	if(m_nRef == 0)
	{
		m_nRef++;
		init();
	}

	for (size_t i = 0;i<MAX_META;i++)
	{
		m_metaOffsets[i] = 0;
		m_metaLengths[i] = 0;
		m_metaPresents[i] = false;
	}
	m_metaPresents[META_TEXT] = true;	
}

CHtmlTextStripper::~CHtmlTextStripper(void)
{
	reset();
	if(m_pContext)
	{
		delete m_pContext;
		m_pContext = NULL;
	}

	if(m_pRequest)
	{
		HTRequest_delete(m_pRequest);				
		m_pRequest = NULL;
	}			

	if(m_pMetaBuffer)
	{
		delete[] m_pMetaBuffer;
		m_pMetaBuffer = NULL;
		m_nMetaBufSize = 0;
	}
	if(m_pTextBuffer)
	{
		delete[] m_pTextBuffer;
		m_pTextBuffer = NULL;
		m_nTextBufSize = 0;
	}

	m_nRef--;
	if(m_nRef <= 0)
		uninit();

	if(m_pCurrentText)
	{
		delete m_pCurrentText;
		m_pCurrentText = NULL;
	}
	
}

void CHtmlTextStripper::init()
{			
	//HTProfile_newNoCacheClient("HtmlText","1.0");
	HTHost_setEventTimeout(15000);
	HTList * converters = HTFormat_conversion();
	HTMLInit(converters);
	if (!HText_registerCDCallback (newText, deleteText)) 
	{
		//FIRTEX_CLOG(level::err) << "HText new/delete callback registration failed." <<endl;				
	}			
	if (!HText_registerTextCallback (addText)) 
	{
		//FIRTEX_CLOG(level::err) << "HText addText callback registration failed." <<endl;				
	}
	//if (!HText_registerLinkCallback (beginAnchor)) 
	//{
	//	//FIRTEX_CLOG(level::err) << "HText link callback registration failed." <<endl;				
	//}
	if (!HText_registerElementCallback (beginElement, endElement)) 
	{
		//FIRTEX_CLOG(level::err) << "HText element callback registration failed." <<endl;				
	}
	HTNet_addAfter(requestTerminater, NULL, NULL, HT_ALL, HT_FILTER_LAST);
	HTAlert_setInteractive(NO);
}
void CHtmlTextStripper::uninit()
{
	//HTProfile_delete();
	/* Clean up all the global preferences */
	HTFormat_deleteAll();

	HTLibTerminate();
}


void CHtmlTextStripper::addAnchor(TextAnchor * anc)
{
	m_anchorList.push_back(anc);	
}

bool CHtmlTextStripper::parse(const string& filename)
{
	return parse(filename.c_str());
}
bool CHtmlTextStripper::parse(const char* filename)
{		
	
	char*	raw = NULL;
	int		len = 0;
	FILE* hFile = NULL;
	try
	{
		char*	raw = NULL;
		int		len = 0;
		FILE* hFile = fopen(filename,"rb");
		if(hFile)
		{
			fseek(hFile,0,SEEK_END);
			len = ftell(hFile);
			fseek(hFile,0,SEEK_SET);
			raw = new char[len + 1];
			fread(raw,1,len,hFile);
			raw[len] = 0;
			fclose(hFile);

			HTStream * curSgml = HTMLToPlain(m_pRequest,
												NULL,/*void *		param,*/
												NULL,/*HTFormat		input_format,*/
												NULL,/*HTFormat		output_format,*/
												NULL);/*HTStream *	output_stream);*/
			curSgml->isa->put_string(curSgml,raw);
			curSgml->isa->_free(curSgml);/*release the resource of output_stream also*/
			
			fclose(hFile);
			delete[] raw;
		}
	}
	catch (...)
	{
		if(hFile)
			fclose(hFile);
		if(raw)
			delete[] raw;
		return false;
	}
#if 0
	FILE* f = fopen("F:\\html.txt","wb");
	fwrite(m_pMetaBuffer,1,m_textSize,f);
	fclose(f);
#endif
	return true;//(ret == YES);
}

bool CHtmlTextStripper::parse(char* text,size_t len)
{			
	try
	{		

		HTStream * curSgml = HTMLToPlain(m_pRequest,
											NULL,/*void *		param,*/
											NULL,/*HTFormat		input_format,*/
											NULL,/*HTFormat		output_format,*/
											NULL);/*HTStream *	output_stream);*/
		curSgml->isa->put_block(curSgml,text,len);	
		curSgml->isa->_free(curSgml);/*release the resource of output_stream also*/
	}
	catch (...)
	{				
		return false;
	}
#if 0
	FILE* f = fopen("F:\\html.txt","wb");
	fwrite(m_pMetaBuffer,1,m_textSize,f);
	fclose(f);
#endif
	return true;//(ret == YES);
}

//		bool CHtmlTextStripper::parseURL(const string& url)
//		{
//			return parseURL(url.c_str());
//		}
//		bool CHtmlTextStripper::parseURL(const char* url)
//		{
//			char * url2 = HTParse(url, NULL, PARSE_ALL);			
//			//char * url2 = HTParse("http://www.vckbase.com", NULL, PARSE_ALL);
//
//			if ((m_pAnchor = HTAnchor_findAddress(url2)) == NULL)
//				return false;			
//			
//			BOOL ret = HTLoadAnchor(m_pAnchor,m_pRequest);
//			if(ret != YES)
//			{
//				cout << "load home error." << endl;
//				return false;
//			}
//			HTEventList_loop(m_pRequest);
//			
//#if 0
//			FILE* f = fopen("F:\\html.txt","wb");
//			fwrite(m_pMetaBuffer,1,m_textSize,f);
//			fclose(f);
//#endif
//
//			return (ret == YES);
//		}


void CHtmlTextStripper::reset()
{	
	while (!m_anchorList.empty())
	{
		delete m_anchorList.front();
		m_anchorList.pop_front();
	}		
	for (size_t i = 0;i<MAX_META;i++)
	{
		m_metaOffsets[i] = 0;
		m_metaLengths[i] = 0;	
	}
	m_nMetaBufUsed = 0;
	m_nTextBufUsed = 0;
}

HText *	CHtmlTextStripper::newText (HTRequest * request,HTParentAnchor * anchor,HTStream *outstrm)
{
	CHtmlTextStripper* pThis = CContext::getHtmlText(request);
	HText* self;
	if(pThis->m_pCurrentText == NULL)
		self = pThis->m_pCurrentText = new HText;
	else self = pThis->m_pCurrentText;

	self->pStripper = pThis;
	self->bSkip = false;
	self->bIsTitle = false;
	self->anchor = anchor;
	self->numChars = 0;
	HTAnchor_setDocument(anchor, (void *) self);

	return self;
}
BOOL CHtmlTextStripper::deleteText(HText * self)
{
	if (self) 
	{
		HTAnchor_setDocument(self->anchor, NULL);
		if(self == self->pStripper->m_pCurrentText)
			self->pStripper->m_pCurrentText = NULL;
		delete self;				
		return YES;
	}
	return NO;
}

void CHtmlTextStripper::addText (HText * text, const char * str, int length)
{			
	if(text && text->bSkip)
		return ;
	
	HtmlMeta hm ;
	if(!text->bIsTitle)
		hm = META_TEXT;
	else 
		hm = META_TITLE;
	int i;
	char* p = (char*)str;
	for(i = 0;i<length;i++)
	{
		//if( (*p <= ' ' && *p > 0 && *p !='\n') || (*p == '\n' && text->text->m_lastLineLen > 0) )
		if( *p <= ' ' && *p > 0 && *p !='\n')
			p++;
		else break;
	}
	while(i < length && (*p >0 && *p <= ' '))
	{
		//if( (*p <= ' ' && *p > 0 && *p !='\n') || (*p == '\n' && text->text->m_lastLineLen > 0) )
		if(*p <= ' ' && *p > 0 && *p !='\n')
		{
			p++;
			i++;
			continue;
		}
		text->numChars += text->pStripper->outputData((const char*)p,1,hm);
		p++;
		i++;				
	}
	if(i < length)
	{
		text->numChars += text->pStripper->outputData((const char*)p,length - i,hm);
	}
}		

HTParentAnchor* CHtmlTextStripper::nodeAnchor (HText * text)
{
	return text->anchor;
}
		
void CHtmlTextStripper::beginAnchor (HText * text,int elem_num, int attr_num, HTChildAnchor * anc,const BOOL *present, const char **value)
{
	/* this is because it's called as link callback */
	if (elem_num != HTML_A)
		return;

	TextAnchor * a = new TextAnchor();			
	a->start = text->numChars;
	a->extent = 0;						
	a->next = 0;
	a->anchor = anc;
	
	text->currentAnchor = a;

	if (HTAnchor_followMainLink((HTAnchor*)anc)) 
	{
		a->number = ++(text->lastAnchorNum);
	}
	else 
	{
		a->number = 0;
	}
}

void CHtmlTextStripper::endAnchor (HText * text)
{
	TextAnchor * a = text->currentAnchor;
	if (!a)			/* </A> without <A> */
		return;
	
	a->extent = text->numChars - a->start;
	text->currentAnchor = NULL;
	text->pStripper->addAnchor(a);
}
void CHtmlTextStripper::beginElement (HText * text,int elem_num, const BOOL * present, const char ** value)
{
	switch (elem_num) 
	{
	case HTML_SCRIPT:
		text->bSkip = true;
		break;
	case HTML_STYLE:
		text->bSkip = true;
		break;
	case HTML_TITLE:
		if(text->pStripper->m_metaPresents[META_TITLE])
		{
			text->bIsTitle = true;
		}
		break;
	case HTML_META:
		{
			CHtmlTextStripper* pStripper = text->pStripper;
			if( (pStripper->m_metaPresents[META_KEYWORDS]) && (pStripper->m_metaLengths[META_KEYWORDS] == 0))
			{
				if(present[HTML_META_NAME] && !strcasecmp(value[HTML_META_NAME],"keywords"))
				{
					if(present[HTML_META_CONTENT]) 
					{
						size_t len = strlen(value[HTML_META_CONTENT]);
						if( (len + pStripper->m_nMetaBufUsed) > pStripper->m_nMetaBufSize)
							pStripper->growMetaBuffer();
						strcpy(pStripper->m_pMetaBuffer + pStripper->m_nMetaBufUsed,value[HTML_META_CONTENT]);
						pStripper->m_metaOffsets[META_KEYWORDS] = pStripper->m_nMetaBufUsed;
						pStripper->m_nMetaBufUsed += len;
						pStripper->m_pMetaBuffer[pStripper->m_nMetaBufUsed++] = 0;
						pStripper->m_metaLengths[META_KEYWORDS] = len;
					}
					break;
				}
			}
			if(pStripper->m_metaPresents[META_DESCRIPTION] && (pStripper->m_metaLengths[META_DESCRIPTION] == 0))
			{
				if(present[HTML_META_NAME] && !strcasecmp(value[HTML_META_NAME],"description"))
				{
					if(present[HTML_META_CONTENT]) 
					{
						size_t len = strlen(value[HTML_META_CONTENT]);
						if( (len + pStripper->m_nMetaBufUsed) > pStripper->m_nMetaBufSize)
							pStripper->growMetaBuffer();
						strcpy(pStripper->m_pMetaBuffer + pStripper->m_nMetaBufUsed,value[HTML_META_CONTENT]);
						pStripper->m_metaOffsets[META_DESCRIPTION] = pStripper->m_nMetaBufUsed;
						pStripper->m_nMetaBufUsed += len;
						pStripper->m_pMetaBuffer[pStripper->m_nMetaBufUsed++] = 0;
						pStripper->m_metaLengths[META_DESCRIPTION] = len;

					}
					break;
				}
			}
			if(pStripper->m_metaPresents[META_CHARSET])
			{
				if(present[HTML_META_HTTP_EQUIV])
				{
					if(present[HTML_META_CONTENT] && !strcasecmp(value[HTML_META_HTTP_EQUIV],"content-type")) 
					{
						pStripper->extractCharset(value[HTML_META_CONTENT]);
					}
					break;
				}
			}
		}
		break;	
	default:
		break;
	}		
}

void CHtmlTextStripper::endElement (HText * text, int elem_num)
{
	text->bSkip = false;
	switch (elem_num) 
	{
	case HTML_A:
		//endAnchor (text);
		break;
	case HTML_TITLE:
		text->bIsTitle = false;
		break;
	default:
		break;
	}			
}
int CHtmlTextStripper::requestTerminater (HTRequest * request, HTResponse * response,void * param, int status) 
{			
	if (status!=HT_LOADED) 
	{
		// TODO :cout<< "Load couldn't be completed successfully\n";
	}			
	//HTEventList_stopLoop();
	return HT_OK;
}
void CHtmlTextStripper::setMetaPresent(HtmlMeta _meta,bool bPresent)
{
	m_metaPresents[_meta] = bPresent;
}

