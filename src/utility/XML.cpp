#include "utility/typedefs.h"
#include "utility/XML.h"
#include "utility/Misc.h"

#include <fstream>
using namespace std;


namespace firtex
{
	namespace utility
	{
		//////////////////////////////////////////////////////////////////////
		// CXMLNode construction

		CXMLNode::CXMLNode(CXMLElement* pParent, const tchar* pszName)
		{
			m_nNode		= xmlNode;
			m_pParent	= pParent;
			if ( pszName ) m_sName = pszName;
		}

		CXMLNode::~CXMLNode()
		{
		}

		//////////////////////////////////////////////////////////////////////
		// CXMLNode parsing

		bool CXMLNode::parseMatch(const tchar*& pszBase, const tchar* pszToken)
		{
			const tchar* pszXML = pszBase;
			int nParse = 0;

			for ( ; *pszXML == ' ' || *pszXML == '\t' || *pszXML == '\r' || *pszXML == '\n' ; pszXML++, nParse++ );
			if ( ! *pszXML ) return false;

			for ( ; *pszXML && *pszToken ; pszXML++, pszToken++, nParse++ )
			{
				if ( *pszXML != *pszToken ) return false;
			}

			pszBase += nParse;

			return true;
		}

		bool CXMLNode::parseIdentifier(const tchar*& pszBase, tstring& strIdentifier)
		{
			const tchar* pszXML = pszBase;
			int nParse = 0;

			for ( ; *pszXML == ' ' || *pszXML == '\t' || *pszXML == '\r' || *pszXML == '\n' ; pszXML++, nParse++ );
			if ( ! *pszXML ) return false;

			int nIdentifier = 0;
			for ( ; *pszXML && ( _istalnum( *pszXML ) || *pszXML == ':' || *pszXML == '_' ) ; pszXML++, nIdentifier++ );
			if ( ! nIdentifier ) return false;

			pszBase += nParse;
			_tcsncpy( CMisc::getBuffer(strIdentifier,nIdentifier), pszBase, nIdentifier );			
			pszBase += nIdentifier;

			return true;
		}

		//////////////////////////////////////////////////////////////////////
		// CXMLNode string to value

		tstring CXMLNode::stringToValue(const tchar*& pszXML, int nLength)
		{
			tstring strValue;

			if ( ! nLength || ! *pszXML ) return strValue;

			tchar* pszValue = CMisc::getBuffer(strValue,nLength);
			tchar* pszOut = pszValue;

			tchar* pszNull = (tchar*)pszXML + nLength;
			tchar cNull = *pszNull;
			*pszNull = 0;

			while ( *pszXML && pszXML < pszNull )
			{
				if ( _istspace( *pszXML ) )
				{
					if ( pszValue != pszOut ) *pszOut++ = ' ';
					pszXML++;
					while ( *pszXML && _istspace( *pszXML ) ) pszXML++;
					if ( ! *pszXML || pszXML >= pszNull ) break;
				}

				if ( *pszXML == '&' )
				{
					pszXML++;
					if ( ! *pszXML || pszXML >= pszNull ) break;

					if ( _tcsnicmp( pszXML, _T("amp;"), 4 ) == 0 )
					{
						*pszOut++ = '&';
						pszXML += 4;
					}
					else if ( _tcsnicmp( pszXML, _T("lt;"), 3 ) == 0 )
					{
						*pszOut++ = '<';
						pszXML += 3;
					}
					else if ( _tcsnicmp( pszXML, _T("gt;"), 3 ) == 0 )
					{
						*pszOut++ = '>';
						pszXML += 3;
					}
					else if ( _tcsnicmp( pszXML, _T("quot;"), 5 ) == 0 )
					{
						*pszOut++ = '\"';
						pszXML += 5;
					}
					else if ( _tcsnicmp( pszXML, _T("apos;"), 5 ) == 0 )
					{
						*pszOut++ = '\'';
						pszXML += 5;
					}
					else if ( _tcsnicmp( pszXML, _T("nbsp;"), 5 ) == 0 )
					{
						*pszOut++ = ' ';
						pszXML += 5;
					}
					else if ( *pszXML == '#' )
					{
						int nChar;
						pszXML++;
						if ( ! *pszXML || pszXML >= pszNull || ! _istdigit( *pszXML ) ) break;

						if ( _stscanf( pszXML, _T("%lu;"), &nChar ) == 1 )
						{
							*pszOut++ = (tchar)nChar;
							while ( *pszXML && *pszXML != ';' ) pszXML++;
							if ( ! *pszXML || pszXML >= pszNull ) break;
							pszXML++;
						}
					}
					else
					{
						*pszOut++ = '&';
					}
				}
				else
				{
					*pszOut++ = *pszXML++;
				}
			}

			//ASSERT( pszNull == pszXML );
			*pszNull = cNull;

			//ASSERT( pszOut - pszValue <= nLength );
			//strValue.ReleaseBuffer( (int)( pszOut - pszValue ) );

			return strValue;
		}

		//////////////////////////////////////////////////////////////////////
		// CXMLNode value to string

#define V2S_APPEND(x,y)	\
	if ( (x) > nOut ) \
		{ \
		nOut += (x) + 16; \
		pszOut = CMisc::getBuffer(strXML, nLen + nOut ) + nLen; \
		} \
		{ for ( const tchar* pszIn = (y) ; *pszIn ; nOut--, nLen++ ) *pszOut++ = *pszIn++; }

		void CXMLNode::valueToString(const tchar* pszValue, tstring& strXML)
		{
			int nLen = (int)strXML.length();
			int nOut = (int)_tcslen( pszValue );
			tchar* pszOut = CMisc::getBuffer(strXML, nLen + nOut ) + nLen;

			for ( ; *pszValue ; pszValue++ )
			{
#ifdef UNICODE
				int nChar = (int)(unsigned short)*pszValue;
#else
				int nChar = (int)(unsigned char)*pszValue;
#endif

				switch ( nChar )
				{
				case '&':
					V2S_APPEND( 5, _T("&amp;") );
					break;
				case '<':
					V2S_APPEND( 4, _T("&lt;") );
					break;
				case '>':
					V2S_APPEND( 4, _T("&gt;") );
					break;
				case '\"':
					V2S_APPEND( 6, _T("&quot;") );
					break;
				case '\'':
					V2S_APPEND( 6, _T("&apos;") );
					break;
				default:
					if ( nChar > 127 )
					{
						tchar item[20];
						_stprintf(item,_T("&#%lu;"),nChar);
						tstring strItem = item;						
						//V2S_APPEND( (int)strItem.length(), strItem.c_str());
						*pszOut++=nChar;
					}
					else if ( nOut > 0 )
					{
						*pszOut++ = nChar;
						nOut--;
						nLen++;
					}
					else
					{						
						nOut += 16;
						pszOut = CMisc::getBuffer(strXML, nLen + nOut ) + nLen;
						*pszOut++ = nChar;
						nOut--;
						nLen++;
					}
					break;
				}
			}

			//strXML.resize( nLen );
		}

		//////////////////////////////////////////////////////////////////////
		// CXMLNode serialize
/*
#ifdef _AFX

		void CXMLNode::serialize(CArchive& ar)
		{
			if ( ar.IsStoring() )
			{
				ar << m_sName;
				ar << m_sValue;
			}
			else
			{
				ar >> m_sName;
				ar >> m_sValue;
			}
		}

#endif*/

		//////////////////////////////////////////////////////////////////////
		// CXMLNode string helper

		void CXMLNode::uniformString(tstring& str)
		{
			// non-alphanumeric characters which will not be ignored
			static const tchar* pszOK = _T("'-&/,;#()");

			firtex::string_utils::trimleft(str);
			firtex::string_utils::trimright(str);

			bool bSpace = true;

			for ( size_t nPos = 0 ; nPos < str.length() ; nPos++ )
			{
#ifdef _UNICODE
				int nChar = (int)(unsigned short)str.at( nPos );
#else
				int nChar = (int)(unsigned char)str.at( nPos );
#endif

				if ( nChar <= 32 )
				{
					if ( bSpace )
					{
						str = str.substr(0,nPos) + str.substr(nPos + 1 );
						//str = str.Left( nPos ) + str.Mid( nPos + 1 );
						nPos--;
					}
					else
					{
						if ( nChar != 32 ) str[nPos] = 32;
						bSpace = true;
					}
				}
				else if ( ! _istalnum( nChar ) && nChar < 0xC0 && _tcschr( pszOK, nChar ) == NULL )
				{
					//str = str.Left( nPos ) + str.Mid( nPos + 1 );
					str = str.substr(0,nPos) + str.substr(nPos + 1 );
					nPos--;
				}
				else
				{
					bSpace = false;
				}
			}
		}

		//////////////////////////////////////////////////////////////////////
		// CXMLElement construction

		CXMLElement::CXMLElement(CXMLElement* pParent, const tchar* pszName) : CXMLNode( pParent, pszName )
		{
			m_nNode = xmlElement;
		}

		CXMLElement::~CXMLElement()
		{
			deleteAllElements();
			deleteAllAttributes();
		}

		//////////////////////////////////////////////////////////////////////
		// CXMLElement clone

		CXMLElement* CXMLElement::clone(CXMLElement* pParent)
		{
			CXMLElement* pClone = new CXMLElement( pParent, m_sName.c_str() );

			for ( AttributeIterator iter = getAttributeIterator() ; hasNextAttribute(iter) ; )
			{
				CXMLAttribute* pAttribute = getNextAttribute( iter )->clone( pClone );
				tstring strName( pAttribute->m_sName );

				firtex::string_utils::toLower(strName);				
				pClone->m_pAttributes.insert(AttributePair(strName, pAttribute ));
			}

			for ( ElementIterator iter = getElementIterator() ; hasNextElement(iter) ; )
			{
				CXMLElement* pElement = getNextElement( iter );
				pClone->m_pElements.push_back( pElement->clone( pClone ) );
			}

			pClone->m_sValue = m_sValue;

			return pClone;
		}

		//////////////////////////////////////////////////////////////////////
		// CXMLElement delete

		void CXMLElement::deleteAllElements()
		{
			ElementIterator iter = m_pElements.begin();
			while (iter != m_pElements.end())
			{
				delete (*iter);
				iter++;
			}
			m_pElements.clear();			
		}

		void CXMLElement::deleteAllAttributes()
		{
			AttributeIterator iter = m_pAttributes.begin();
			while (iter != m_pAttributes.end())
			{
				delete iter->second;
				iter++;
			}
			m_pAttributes.clear();
		}

		//////////////////////////////////////////////////////////////////////
		// CXMLElement to string

		tstring CXMLElement::toString(bool bHeader, bool bNewline)
		{
			tstring strXML;
			if ( bHeader ) strXML = _T("<?xml version=\"1.0\" encoding=\"UTF-8\" ?>");
			if ( bNewline ) strXML += _T("\r\n");
			toString( strXML, bNewline );
			//ASSERT( strXML.GetLength() == _tcslen(strXML) );
			return strXML;
		}

#define WRITE_TABS for(nTab = 0;nTab < nDepth;nTab++){strXML += _T("\t");}
		void CXMLElement::toString(tstring& strXML, bool bNewline,int nDepth)
		{
			int nTab = 0;
			if(bNewline)
				WRITE_TABS;

			strXML += '<' + m_sName;
			
			for ( AttributeIterator iter1 = getAttributeIterator(); hasNextAttribute(iter1) ; )
			{
				strXML += ' ';
				CXMLAttribute* pAttribute = getNextAttribute( iter1 );
				pAttribute->toString( strXML );
			}

			ElementIterator iter = getElementIterator();

			if ( (hasNextElement(iter) == false) && m_sValue.empty() )
			{
				strXML += _T("/>");
				if ( bNewline ) strXML += _T("\r\n");
				return;
			}

			strXML += '>';
			if ( bNewline && hasNextElement(iter) )
			{
				strXML += _T("\r\n");				
			}

			while ( hasNextElement(iter) )
			{
				CXMLElement* pElement = getNextElement( iter );				
				pElement->toString( strXML, bNewline, nDepth + 1);
			}

			valueToString( m_sValue.c_str(), strXML );
			if((getElementCount() > 0) && bNewline)
				WRITE_TABS;
			strXML += _T("</") + m_sName + '>';
			if ( bNewline ) strXML += _T("\r\n");			
		}

		//////////////////////////////////////////////////////////////////////
		// CXMLElement from string

		CXMLElement* CXMLElement::fromString(const tchar* pszXML, bool bHeader)
		{
			CXMLElement* pElement	= NULL;
			const tchar* pszElement		= NULL;

			try
			{
				if ( parseMatch( pszXML, _T("<?xml version=\"") ) )
				{
					pszElement = _tcsstr( pszXML, _T("?>") );
					if ( ! pszElement ) return false;
					pszXML = pszElement + 2;
				}
				else if ( bHeader ) return NULL;

				while ( parseMatch( pszXML, _T("<!--") ) )
				{
					pszElement = _tcsstr( pszXML, _T("-->") );
					if ( ! pszElement || *pszElement != '-' ) return false;
					pszXML = pszElement + 3;
				}

				if ( parseMatch( pszXML, _T("<!DOCTYPE") ) )
				{
					pszElement = _tcsstr( pszXML, _T(">") );
					if ( ! pszElement ) return false;
					pszXML = pszElement + 1;
				}

				while ( parseMatch( pszXML, _T("<!--") ) )
				{
					pszElement = _tcsstr( pszXML, _T("-->") );
					if ( ! pszElement || *pszElement != '-' ) return false;
					pszXML = pszElement + 3;
				}

				pElement = new CXMLElement();

				if ( ! pElement->parseString( pszXML ) )
				{
					delete pElement;
					pElement = NULL;
				}
			}
			catch (...)
			{
//#ifndef LIB_COM
//				FIRTEX_CLOG(level::warn) << _T("parse xml file failed:") << endl;
//#endif
				//TODO:
			}

			return pElement;
		}

		bool CXMLElement::parseString(const tchar*& strXML)
		{
			if ( ! parseMatch( strXML, _T("<") ) ) return false;

			if ( ! parseIdentifier( strXML, m_sName ) ) return false;

			const tchar* pszEnd = strXML + _tcslen( strXML );

			while ( ! parseMatch( strXML, _T(">") ) )
			{
				if ( parseMatch( strXML, _T("/") ) )
				{
					return parseMatch( strXML, _T(">") );
				}

				if ( ! *strXML || strXML >= pszEnd ) return false;

				CXMLAttribute* pAttribute = new CXMLAttribute( this );

				if ( pAttribute->parseString( strXML ) )
				{
					tstring strName( pAttribute->m_sName );					
					firtex::string_utils::toLower(strName);					
					AttributeIterator iter = m_pAttributes.find(strName);
					if(iter != m_pAttributes.end())
					{
						delete iter->second;
						m_pAttributes.erase(iter);
					}					
					m_pAttributes.insert(AttributePair(strName,pAttribute));
				}
				else
				{
					delete pAttribute;
					return false;
				}
			}

			tstring strClose = _T("</");
			strClose += m_sName + '>';

			while ( true )
			{
				if ( ! *strXML || strXML >= pszEnd ) return false;

				const tchar* pszElement = _tcschr( strXML, '<' );
				if ( ! pszElement || *pszElement != '<' ) return false;

				if ( pszElement > strXML )
				{
					//if ( m_sValue.length() && m_sValue.substr(0,1) != ' ' ) m_sValue += ' ';
					m_sValue += stringToValue( strXML, (int)( pszElement - strXML ) );					
//					ASSERT( strXML == pszElement );
					if ( strXML != pszElement ) return false;
				}

				if ( parseMatch( strXML, strClose.c_str() ) )
				{
					break;
				}
				else if ( parseMatch( strXML, _T("<!--") ) )
				{
					pszElement = _tcsstr( strXML, _T("-->") );
					if ( ! pszElement || *pszElement != '-' ) return false;
					strXML = pszElement + 3;
				}
				else
				{
					CXMLElement* pElement = new CXMLElement( this );

					if ( pElement->parseString( strXML ) )
					{
						m_pElements.push_back( pElement );
					}
					else
					{
						delete pElement;
						return false;
					}
				}
			}

			return true;
		}

		//////////////////////////////////////////////////////////////////////
		// CXMLElement from bytes

		CXMLElement* CXMLElement::fromBytes(byte* pByte, size_t nByte, bool bHeader)
		{
			tstring strXML;

			if ( nByte >= 2 && ( ( pByte[0] == 0xFE && pByte[1] == 0xFF ) || ( pByte[0] == 0xFF && pByte[1] == 0xFE ) ) )
			{
				nByte = nByte / 2 - 1;

				if ( pByte[0] == 0xFE && pByte[1] == 0xFF )
				{
					pByte += 2;

					for ( uint32_t nSwap = 0 ; nSwap < (uint32_t)nByte ; nSwap ++ )
					{
						register char nTemp = pByte[ ( nSwap << 1 ) + 0 ];
						pByte[ ( nSwap << 1 ) + 0 ] = pByte[ ( nSwap << 1 ) + 1 ];
						pByte[ ( nSwap << 1 ) + 1 ] = nTemp;
					}
				}
				else
				{
					pByte += 2;
				}

	
				memcpy(CMisc::getBuffer(strXML,nByte),pByte,nByte*sizeof(tchar));				
			}
			else
			{
				if ( nByte >= 3 && pByte[0] == 0xEF && pByte[1] == 0xBB && pByte[2] == 0xBF )
				{
					pByte += 3; nByte -= 3;
				}

				size_t nWide = CMisc::utf8towcs(NULL,0,(const char*)pByte,nByte);
#ifdef UNICODE
				CMisc::utf8towcs(CMisc::getBuffer(strXML,nWide),pByte,nByte);
#else
				wchar_t* buf = new wchar_t[nWide + 1];
				CMisc::utf8towcs(buf,nWide,(const char*)pByte,nByte);				
				buf[nWide] = 0;
				size_t nLen = CMisc::_wcstombs(NULL,0,buf,nWide);
				CMisc::_wcstombs(CMisc::getBuffer(strXML,nLen),nLen,buf,nWide);
				delete[] buf;
#endif		
			}

			return fromString( strXML.c_str(), bHeader );
		}

		//////////////////////////////////////////////////////////////////////
		// CXMLElement from file

		CXMLElement* CXMLElement::fromFile(const tstring& sPath, bool bHeader /* = false */)
		{
			FILE* hFile = NULL;
			byte* buf = NULL;
			try
			{
				hFile = _tfopen(sPath.c_str(),_T("rb"));
				size_t len = 0;				
				if(hFile)
				{
					fseek(hFile,0,SEEK_END);
					len = ftell(hFile);
					fseek(hFile,0,SEEK_SET);
					if(len <= 0)
					{
						fclose(hFile);
						return NULL;
					}
					buf = new byte[len + 1];				
					fread(buf,sizeof(char),len,hFile);
					buf[len] = '\0';
					CXMLElement* pXML = fromBytes(buf,len,bHeader);
					delete[] buf;
					fclose(hFile);
					hFile = NULL;
					return pXML;
				}
			}
			catch (...)
			{
				if(hFile)
					fclose(hFile);
				if(buf)
					delete[] buf;
			}				
			return NULL;
		}

		void CXMLElement::toFile(const tstring& sPath, bool bHeader)
		{
			FILE* hFile = NULL;
			try
			{
				tstring sXML = toString(bHeader,true);
				hFile = _tfopen(sPath.c_str(),_T("wb"));
				if(hFile)
				{
#ifdef UNICODE
					size_t nMulti = CMisc::wcstoutf8(NULL,sXML.c_str(),sXML.length());
					char* buf = new char[nMulti + 1];
					CMisc::wcstoutf8(buf,sXML.c_str(),sXML.length());
					buf[nMulti] = 0;
					fwrite(buf,sizeof(char),nMulti,hFile);
					delete[] buf;
#else
					size_t nWide = CMisc::_mbstowcs(NULL,0,sXML.c_str(),sXML.length());
					wchar_t* wide = new wchar_t[nWide + 1];
					CMisc::_mbstowcs(wide,nWide,sXML.c_str(),sXML.length());			
					wide[nWide] = 0;

					size_t nMulti = CMisc::wcstoutf8(NULL,0,wide,nWide);
					char* multi = new char[nMulti + 1];
					CMisc::wcstoutf8(multi,nMulti,wide,nWide);
					multi[nMulti] = 0;
					fwrite(multi,sizeof(char),nMulti,hFile);
					delete[] wide;
					delete[] multi;
#endif
					fclose(hFile);
					hFile = NULL;
				}
			}
			catch (...)
			{
				if(hFile)
				{
					fclose(hFile);
				}
			}		
		}
		//////////////////////////////////////////////////////////////////////
		// CXMLElement equality

		bool CXMLElement::equals(CXMLElement* pXML) 
		{
			if ( this == NULL || pXML == NULL ) return false;
			if ( pXML == this ) return true;

			if ( m_sName != pXML->m_sName ) return false;
			if ( m_sValue != pXML->m_sValue ) return false;

			if ( getAttributeCount() != pXML->getAttributeCount() ) return false;
			if ( getElementCount() != pXML->getElementCount() ) return false;

			for ( AttributeIterator iter = getAttributeIterator() ; hasNextAttribute(iter); )
			{
				CXMLAttribute* pAttribute1 = getNextAttribute( iter );
				CXMLAttribute* pAttribute2 = pXML->getAttribute( pAttribute1->m_sName.c_str() );
				if ( pAttribute2 == NULL ) return false;
				if ( ! pAttribute1->equals( pAttribute2 ) ) return false;
			}

			ElementIterator iter1 = getElementIterator();
			ElementIterator iter2 = pXML->getElementIterator();

			for ( ; hasNextElement(iter1) && pXML->hasNextElement(iter2) ; )
			{
				CXMLElement* pElement1 = getNextElement( iter1 );
				CXMLElement* pElement2 = pXML->getNextElement( iter2 );
				if ( pElement1 == NULL || pElement2 == NULL ) return false;
				if ( ! pElement1->equals( pElement2 ) ) return false;
			}

			if ( (hasNextElement(iter1) != false) || (pXML->hasNextElement(iter2) != false) ) return false;

			return true;
		}

		//////////////////////////////////////////////////////////////////////
		// CXMLElement recursive word accumulation

		/*tstring CXMLElement::getRecursiveWords()
		{
			tstring strWords;

			addRecursiveWords( strWords );
			CMisc::trim(strWords);			

			return strWords;
		}

		void CXMLElement::addRecursiveWords(tstring& strWords)
		{
			for ( AttributeIterator iter = getAttributeIterator() ; hasNextAttribute(iter) ; )
			{
				CXMLAttribute* pAttribute = getNextAttribute( iter );
				tstring strText = pAttribute->getName();


				if ( strText.find( ':' ) >= 0 ) continue;
				if ( strText.CompareNoCase( _T("SHA1") ) == 0 ) continue;	// NOTE: Shareaza Specific

				if ( strWords.GetLength() ) strWords += ' ';
				strWords += pAttribute->GetValue();
			}

			for ( POSITION pos = getElementIterator() ; pos ; )
			{
				getNextElement( pos )->AddRecursiveWords( strWords );
			}

			if ( m_sValue.GetLength() )
			{
				if ( strWords.GetLength() ) strWords += ' ';
				strWords += m_sValue;
			}
		}*/

		//////////////////////////////////////////////////////////////////////
		// CXMLAttribute construction

		const tchar* CXMLAttribute::xmlnsSchema		= _T("http://www.w3.org/2001/XMLSchema");
		const tchar* CXMLAttribute::xmlnsInstance	= _T("http://www.w3.org/2001/XMLSchema-instance");
		const tchar* CXMLAttribute::schemaName		= _T("xsi:noNamespaceSchemaLocation");

		CXMLAttribute::CXMLAttribute(CXMLElement* pParent, const tchar* pszName) : CXMLNode( pParent, pszName )
		{
			m_nNode = xmlAttribute;
		}

		CXMLAttribute::~CXMLAttribute()
		{
		}

		//////////////////////////////////////////////////////////////////////
		// CXMLAttribute clone

		CXMLAttribute* CXMLAttribute::clone(CXMLElement* pParent)
		{
			CXMLAttribute* pClone = new CXMLAttribute( pParent, m_sName.c_str() );
			pClone->m_sValue = m_sValue;
			return pClone;
		}

		//////////////////////////////////////////////////////////////////////
		// CXMLAttribute to string

		void CXMLAttribute::toString(tstring& strXML)
		{
			strXML += m_sName + _T("=\"");
			valueToString( m_sValue.c_str(), strXML );
			strXML += '\"';
		}

		//////////////////////////////////////////////////////////////////////
		// CXMLAttribute from string

		bool CXMLAttribute::parseString(const tchar*& strXML)
		{
			if ( ! parseIdentifier( strXML, m_sName ) ) return false;
			if ( ! parseMatch( strXML, _T("=") ) ) return false;

			if ( parseMatch( strXML, _T("\"") ) )
			{
				const tchar* pszQuote = _tcschr( strXML,  '\"' );
				if ( ! pszQuote || *pszQuote != '\"' ) return false;

				m_sValue = stringToValue( strXML, (int)( pszQuote - strXML ) );

				return parseMatch( strXML, _T("\"") );
			}
			else if ( parseMatch( strXML, _T("'") ) )
			{
				const tchar* pszQuote = _tcschr( strXML,  '\'' );
				if ( ! pszQuote || *pszQuote != '\'' ) return false;

				m_sValue = stringToValue( strXML, (int)( pszQuote - strXML ) );

				return parseMatch( strXML, _T("\'") );
			}
			else
			{
				return false;
			}
		}

		//////////////////////////////////////////////////////////////////////
		// CXMLAttribute equality

		bool CXMLAttribute::equals(CXMLAttribute* pXML) 
		{
			if ( this == NULL || pXML == NULL ) return false;
			if ( pXML == this ) return true;

			if ( m_sName != pXML->m_sName ) return false;
			if ( m_sValue != pXML->m_sValue ) return false;

			return true;
		}

		//////////////////////////////////////////////////////////////////////
		// CXMLAttribute serialize
/*
#ifdef _AFX

		void CXMLAttribute::serialize(CArchive& ar)
		{
			CXMLNode::Serialize( ar );
		}

#endif*/

	}
}

