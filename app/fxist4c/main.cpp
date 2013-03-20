
#include "FirteX.h"

using namespace firtex::index;
using namespace firtex::utility;
using namespace firtex::parser;
using namespace firtex::analyzer;
using namespace firtex::store;
using namespace firtex::search;
using namespace firtex::collection;

#if defined(_MSC_VER)
#include <crtdbg.h>
#endif

#include <iostream>
using namespace std;


void indexFile(const tchar* filepath,const tchar* indexpath,bool create);
void overview(const tchar* indexpath);
void searchIndex(const tchar* indexpath,const tchar* field);
void searchFromFile(const tchar* indexpath,const tchar* queryFile,const tchar* field);
void overviewTerms(const tchar* indexpath,int dump);
void overviewDocuments(const tchar* indexpath);
void overviewTermVectors(const tchar* indexpath);
void indexMerge(const tchar* indexpath,bool optimize);
void displayDoc(CDocument* pDoc,score_t s);

/**
 * load queries
 * @param pszQueryFile query file path
 * @param field query field
 */
void			loadQueries(vector<string>&queries,const tchar* pszQueryFile);

/**
 * load query file
 * @param pszQueryFile query file path
 * @param field query field
 */
void			loadQueryFile(vector<string>&queries,const tchar* pszQueryFile);

/**
 * load TREC topic file
 * @param pszQueryFile query file path
 * @param field query field
 */
void			loadTRECTopic(vector<string>&queries,const tchar* pszTopicFile);

void usage()
{
	printf("Usage:\n");
	printf("\tfxist4c [/?] | [?] | [argfile] ]\n");
	printf("Where:\n");
	printf("\targfile\t\tIndex argument file.\n");
	printf("\t/? | ?\t\tDisplay this help message.\n");
	printf("Examples:\n");
	printf("\tfxist4c\t\tDisplay Menu.\n");
	printf("\tfxist4c config.xml Build index use config.xml as argument file.\n");
}

void menu()
{
	printf("\nFirteX 1.0.3.\n");
	printf("==================================\n");
	printf("Select the operation:\n");
	printf("	1 for build index files.\n");
	printf("	2 for overview of index .\n");	
	printf("	3 for search.\n");
	printf("	4 for search from file.\n");	
	printf("	5 for overview of documents.\n");
	printf("	6 for overview of terms.\n");
	printf("	7 for overview of term vectors.\n");
	printf("	8 for index merge.\n");
	printf("	9 for exit.\n");
	printf("==================================\n");
}
int main(int argc,char* argv[])
{	
	if(argc == 2)
	{
		if(!_tcsicmp(argv[1],_T("/?")) || !_tcsicmp(argv[1],_T("?")) 
			|| !_tcsicmp(argv[1],_T("-help")) || !_tcsicmp(argv[1],_T("/h"))
			|| !_tcsicmp(argv[1],_T("-h")) )
		{
			usage();
			return 0;
		}	
		
		try
		{
			CSpeedProbe p;
			p.begin();
			CIndexWriter::buildIndex(argv[1]);
			p.end();
			cout << "Build index success!finished in " << p.timeSpan() << " s." << endl;
		}
		catch(CFirteXException& fe)
		{
			printf("%s",fe.what());
		}		
	}
	else 
	{
		menu();
		int ch = fgetc(stdin);
		fgetc(stdin);
		tchar filepath[250],indexpath[250],field[250];
		try
		{
			while (ch != '9')
			{
				switch(ch)
				{
				case '1':// "1 for index files.";
					{
						printf("Location of text files to be indexed:\n");
						fgets(filepath,250,stdin);
						filepath[strlen(filepath)-1] = 0;
						printf("Location to store indexes:\n");
						fgets(indexpath,250,stdin);
						indexpath[strlen(indexpath)-1] = 0;
						printf("Create or Append if index already exist?(1 for create,2 for append)\n");
						ch = fgetc(stdin);
						fgetc(stdin);
						indexFile(filepath,indexpath,(ch!='2'));
					}
					break;
				case '2':// "2 for overview of index ."
					{				
						printf("Location of indexes:\n");
						fgets(indexpath,250,stdin);						
						indexpath[strlen(indexpath)-1] = 0;
						overview(indexpath);
					}
					break;
				case '3':// "3 for search."
					{				
						printf("Location of the indexes:\n");
						fgets(indexpath,250,stdin);
						indexpath[strlen(indexpath)-1] = 0;
						printf("Default query field: \n");
						fgets(field,250,stdin);
						field[strlen(field)-1] = 0;
						searchIndex(indexpath,field);
					}
					break;

				case '4':// "4 for search from file."
					{				
						printf("Location of indexes:\n");
						fgets(indexpath,250,stdin);
						indexpath[strlen(indexpath)-1] = 0;
						printf("Location of the query file(one query one line) or TREC topics file: \n");
						fgets(filepath,250,stdin);
						filepath[strlen(filepath)-1] = 0;
						printf("Query field: \n");
						fgets(field,250,stdin);
						field[strlen(field)-1] = 0;
						searchFromFile(indexpath,filepath,field);
					}
					break;
				
				case '5':// "5 for overview of documents."
					{
						printf("Location of the indexes:\n");
						fgets(indexpath,250,stdin);
						indexpath[strlen(indexpath)-1] = 0;						
						overviewDocuments(indexpath);
					}
					break;
				case '6':// "6 for overview of terms."
					{
						printf("Location of the indexes:\n");
						fgets(indexpath,250,stdin);
						indexpath[strlen(indexpath)-1] = 0;
						printf("Select the operation:\n");
						printf("\t1 dump documents\n\t2 dump doc id\n\t3 dump doc id and positions\n");
						ch = fgetc(stdin);
						fgetc(stdin);
						overviewTerms(indexpath,ch-'0');
					}
					break;
				case '7':// "7 for overview of term vectors."
					{
						printf("Location of the indexes:\n");
						fgets(indexpath,250,stdin);
						indexpath[strlen(indexpath)-1] = 0;						
						overviewTermVectors(indexpath);
					}
					break;
				case '8':// "8 for index merge."
					{
						printf("Location of the indexes:\n");
						fgets(indexpath,250,stdin);
						indexpath[strlen(indexpath)-1] = 0;
						printf("Merge or Optimize?(1 for merge,2 for optimize)\n");
						ch = fgetc(stdin);
						fgetc(stdin);
						indexMerge(indexpath,(ch=='2'));
					}
					break;
				case '9':
					return 0;
				default:
					printf("Invalid input.\n");
					return 0;
				}
				menu();
				ch = fgetc(stdin);
				fgetc(stdin);
			}
		}
		catch (CFirteXException& e)
		{
			cout << e.what() << endl;
		}
	}
	return 0;
}


void indexFile(const tchar* filepath,const tchar* indexpath,bool create)
{
	char ch;
	Configurator config;
	config.index.source = filepath;
	config.index.indexlocation = indexpath;
	config.index.accessmode = (create == false)?_T("a"):_T("w");

	printf("Please select the parser: \n");
	printf("\t1 for plain parser\n");
	printf("\t2 for html parser\n");
	printf("\t3 for pdf parser\n");	
	ch = fgetc(stdin);
	fgetc(stdin);
	if(ch == '1')
		config.parser.identifier = _T("parser.plain");
	else if(ch == '2')
		config.parser.identifier = _T("parser.html");
	else if(ch == '3')
		config.parser.identifier = _T("parser.pdf");	
	
	printf("Please select the collection type: \n");
	printf("\t1 for directory\n");
	printf("\t2 for file list \n");
	printf("\t3 for FirteX collection\n");	
	printf("\t4 for trec collection\n");	
	ch = fgetc(stdin);
	fgetc(stdin);
	if(ch == '1')
		config.collection.identifier = _T("collection.directory");
	else if(ch == '2')
		config.collection.identifier = _T("collection.filelist");
	else if(ch == '3')
		config.collection.identifier = _T("collection.firtex");
	else if(ch == '4')
		config.collection.identifier = _T("collection.trec");

	printf("Please select the index merge mode: \n");
	printf("\t1 for no merge\n");
	printf("\t2 for merge \n");
	printf("\t3 for optimize\n");		
	ch = fgetc(stdin);
	fgetc(stdin);
	if(ch == '1')
		config.mergestrategy.strategy = _T("NO");
	else if(ch == '2')
		config.mergestrategy.strategy = _T("HLM");
	else if(ch == '3')
		config.mergestrategy.strategy = _T("OPTIMIZE");
	else config.mergestrategy.strategy = _T("HLM");

	CSpeedProbe probe;
	try
	{
		CCollection* pCol = CIndexWriter::prepareBuildIndex(&config);		
		probe.begin();
		pCol->scan();
		probe.end();
		delete pCol;
	}
	catch (CFirteXException& fe) 
	{
		printf("build index failed:%s",fe.what());
	}
	
	cout << "Build index success!finished in " << probe.timeSpan() << " s." << endl;
}

void overview(const tchar* indexpath)
{
	CIndex* pIndex = new CIndex(indexpath,CIndex::ACCESS_READONLY);
	CIndexReader* pReader = pIndex->acquireReader();	

	printf("Index path:\t\t\t%s\n",indexpath);
	printf("Version:\t\t\t%s\n",pReader->getVersion().c_str());

	CBarrelsInfo* pBarrelsInfo = pReader->getBarrelsInfo();	
	printf("Number of index barrels:\t%d\n",pBarrelsInfo->getBarrelCount());

	printf("Number of documents:\t\t%d\n",pReader->numDocs());

	CFieldsInfo& fieldInfo = pReader->getFieldsInfo();
	printf("Number of fields:\t\t%d\n",fieldInfo.numFields());
	
	printf("Number of terms:\t\t%u\n",pReader->getNumTerms());

	printf("Number of deleted documents:\t%u\n",pReader->numDeletedDocs());

	CDateTime dt;
	pReader->lastModified(dt);
	printf("Last modified:\t\t\t%s\n",dt.format().c_str());
	
	//barrels
	printf("\nIndex barrels:\n");
	printf("\tname\t\tstart docid\tnumDocs\n");
	pBarrelsInfo->startIterator();
	CBarrelInfo* pBaInfo;
	while (pBarrelsInfo->hasNext())
	{
		pBaInfo = pBarrelsInfo->next();

		printf("\t%s\t\t%u\t\t%u\n",pBaInfo->getName().c_str(),pBaInfo->getMinDocID(),pBaInfo->getDocCount());
	}

	//fields
	printf("\nIndex fields:\n");
	printf("(S:´æ´¢ C:Ñ¹Ëõ A:·ÖÎö I:Ë÷Òý S:´ÊÐòÁÐ F:´ÊºÍ´ÊÆµ P:´Ê ¡¢´Ê ÆµºÍÎ»ÖÃ N:·ñ)\n");
	printf("\tname\t\tstore\tanalyze\tindex\tterm vector\n");
	
	fieldInfo.startIterator();
	CFieldInfo* pFieldInfo;	
	while (fieldInfo.hasNext())
	{
		pFieldInfo = fieldInfo.next();
		printf("\t%s\t\t",pFieldInfo->getName());
		
		fieldflag_t f= pFieldInfo->getFlag();
		if(pFieldInfo->isStored())
			printf("S\t");
		else 
		{
			if(pFieldInfo->isCompressed())
				printf("C\t");
			else 
				printf("N\t");
		}						
		if(pFieldInfo->isAnalyzed())
			printf("A\t");
		else 
			printf("N\t");
		if(pFieldInfo->isIndexed())
			printf("I\t");
		else 
			printf("N\t");

		if(pFieldInfo->isSequenceVector())
			printf("S\t");
		else if(pFieldInfo->isFreqVector())
			printf("F\t");
		else if(pFieldInfo->isPositionVector())
			printf("P\t");
		else 
			printf("N\n");
	}
	printf("\n");
	delete pIndex;
}
void searchIndex(const tchar* indexpath,const tchar* field)
{	
	string query;

	CAnalyzer* pAnalyzer = new CStandardAnalyzer();
	CIndex index(indexpath,CIndex::ACCESS_SEARCH);
	CIndexSearcher* pSearcher = new CIndexSearcher(index.acquireReader());

	tchar line[80];		

	while (true)
	{
		printf("Query: ");
		fgets(line,80,stdin);
		line[strlen(line)-1]=0;
		if ( strlen(line) == 0 )
			break;

		CQuery* q = CQueryParser::parse(line,field,pAnalyzer,false);
		CQueryHits* pHits = pSearcher->search(q,100);
		if(pHits)
		{
			cout << "Total Hits:" << pHits->getHitNum() << endl;
			pHits->startIterator();
			CDocument* pDoc;
			CQueryHits::CHitDoc* pHitDoc;
			while (pHits->hasNext())
			{					
				pHitDoc = pHits->nextHitDoc();
				pDoc = pHitDoc->doc();
				if(pDoc)
				{					
					displayDoc(pDoc,pHitDoc->score());
					delete pDoc;
				}
			}	
			delete pHits;
		}				
		delete q;			
	}

	delete pSearcher;	
	delete pAnalyzer;
}
void searchFromFile(const tchar* indexpath,const tchar* queryFile,const tchar* field)
{	
	vector<string>queries;
	loadQueries(queries,queryFile);
	if(queries.size() == 0)
	{
		cout << "No query loaded" << endl;
		return ;
	}

	CAnalyzer* pAnalyzer = new CStandardAnalyzer();
	CIndex index(indexpath,CIndex::ACCESS_SEARCH);
	CIndexSearcher* pSearcher = new CIndexSearcher(index.acquireReader());	

	size_t	numQueries = queries.size();
	CQuery** ppQueries = new CQuery*[numQueries];
	size_t i;
	for(i = 0;i<numQueries;i++)
	{
		ppQueries[i] = CQueryParser::parse(queries[i].c_str(),field,pAnalyzer,false);
	}
	
	CSpeedProbe probe;	
	CQueryHits* pHits; 
	int32_t totalRet = 0;
	probe.begin();	
	for(i = 0;i<numQueries;i++)
	{		
		cout << "searching:" << queries[i];
		pHits = pSearcher->search(ppQueries[i],100);
		if(pHits)
		{
			cout << " ,Hits:" << pHits->getHitNum() << endl;
			totalRet += pHits->getHitNum();
			delete pHits;
		}				
		else 
		{
			cout << ",No hits." <<endl;
		}
	}
	probe.end();

	cout << "Total Hits:" << totalRet << endl;

	cout << "Query Finished in " << probe.timeSpan() << " s." << endl;
	cout << "Query speed: " << (double)probe.timeSpan()/(double)numQueries << " s." << endl;

	for(i = 0;i<numQueries;i++)
	{
		delete ppQueries[i];
	}
	delete[] ppQueries;
	queries.clear();

	delete pSearcher;	
	delete pAnalyzer;
}

void overviewTerms(const tchar* indexpath,int dump)
{
	tchar query[250];

	CStandardAnalyzer ca;	

	CIndex* pIndex = new CIndex(indexpath,CIndex::ACCESS_READONLY);
	CIndexReader* pReader = pIndex->acquireReader();	

	CTokens* pTokens = NULL;
	CToken* t;
	while (true)
	{
		printf("Term to read:\n");
		fgets(query,250,stdin);
		query[strlen(query) - 1] = 0;
		if ( strlen(query) == 0 )
			break;
		CStringReader sr(query);		
		pTokens = ca.nextTokens(&sr);
		if(pTokens->getNumTokens() <= 0)
		{
			printf("The analyze result has no tokens.\n");
			continue;
		}
		if(pTokens->getNumTokens() > 1)
		{
			printf("The analyze result has more than one token,it can only process one token.\n");
			continue;
		}
		pTokens->startIterator();
		if(pTokens->hasNext())
		{
			t = pTokens->next();
			CTerm* pTerm = t->toTerm(_T("contents"));
			CTermReader* pTermReader = pReader->termReader();			
			if(pTermReader->seek(pTerm))
			{
				if(dump == 1)
				{
					CTermDocFreqs* pTermDocs = pTermReader->termDocFreqs();
					cout << "docFreq:"<<pTermDocs->docFreq() << endl;
					while (pTermDocs->next())
					{
						displayDoc(pReader->document(pTermDocs->doc()),0.0);						
					}
					cout << endl;
					delete pTermDocs;
				}
				else if(dump == 2)
				{
					CTermDocFreqs* pTermDocs = pTermReader->termDocFreqs();
					cout << "docFreq:"<<pTermDocs->docFreq() << endl;
					while (pTermDocs->next())
					{
						cout << "(" << pTermDocs->doc();
						cout << "," <<  pTermDocs->freq() << ") ";						
					}
					cout << endl;
					delete pTermDocs;
				}	
				else 
				{
					CTermPositions* pPositions = pTermReader->termPositions();		
					cout << "docFreq:"<<pPositions->docFreq() << endl;
					int pos;
					while(pPositions->next())
					{
						cout << "(" << pPositions->doc();
						cout << "," << pPositions->freq();
						cout << "<";
						pos = pPositions->nextPosition();
						while(pos != -1)
						{			
							cout <<pos<<",";
							pos = pPositions->nextPosition();
						}
						cout << "> )";
						cout << endl;
					}
					delete pPositions;//*/
				}				
			}
			delete pTerm;
			delete pTermReader;
		}
	}	
	delete pIndex;
}

void overviewDocuments(const tchar* indexpath)
{
	try
	{
		int i ;
		tchar tmp[80];
		docid_t docid;
		CIndex index(indexpath,CIndex::ACCESS_READONLY);
		CIndexReader* pReader = index.acquireReader();	
		do
		{
			printf("doc id (q for exit): ");
			fgets(tmp,79,stdin);
			tmp[strlen(tmp) - 1] = 0;
			if(!_tcsicmp(tmp,_T("q")))
				break;
			for(i = 0;i<(int)strlen(tmp);i++)
			{			
				if(tmp[i]<_T('0') || tmp[i] > _T('9'))
				{
					i = -1;
					break;
				}
			}
			if(i == -1)
			{
				printf("invalid doc id.\n");
			}
			else 
			{
				docid = atoi(tmp);
				if(docid >= pReader->maxDoc())
				{
					printf("there are only %u documents\n",pReader->maxDoc());					
				}
				else if(pReader->isDeleted(docid))
				{
					printf("doc:%u is deleted.\n",docid);
				}
				else 
				{
					displayDoc(pReader->document(docid),0.0);
				}
			}
		}while(_tcsicmp(tmp,_T("q")));		
	}
	catch(CFirteXException& fe)
	{		
		printf("error:overviewDocuments:%s\n",fe.what());
	}
};
void overviewTermVectors(const tchar* indexpath)
{
	CIndex* pIndex = new CIndex(indexpath,CIndex::ACCESS_READONLY);
	CIndexReader* pReader = pIndex->acquireReader();	
	
	CFieldsInfo& fieldsInfo = pReader->getFieldsInfo();		
	CFieldInfo* pFieldInfo;
	fieldsInfo.startIterator();
	bool hasTermVector = false;
	printf("\nid\tfield\tterm vector\n");
	while(fieldsInfo.hasNext())
	{
		pFieldInfo = fieldsInfo.next();
		if(pFieldInfo->isStoreForwardIndex())
		{
			printf("%u\t%s\t",pFieldInfo->getID(),pFieldInfo->getName());
			
			if(pFieldInfo->isSequenceVector())
				printf("S\n");
			else if(pFieldInfo->isFreqVector())
				printf("F\n");
			else if(pFieldInfo->isPositionVector())
				printf("P\n");
			else
			{
				printf("error\n");
				break;
			}
            hasTermVector = true;
		}	
	}
	if(hasTermVector == false)
	{
		printf("there are no field with term vector.\n");
		return;
	}
	printf("please select field's id to display term vector:\n");

	delete pIndex;
};

void indexMerge(const tchar* indexpath,bool optimize)
{
	/*CIndexWriter* pWriter = new CIndexWriter(indexpath,NULL,false);	
	if(optimize)
		pWriter->optimizeIndex();	
	else 
		pWriter->mergeIndex();
	delete pWriter;
	pWriter = NULL;*/
}
void displayDoc(CDocument* pDoc,score_t s)
{
	CField* pField;
	cout << "============================================="<<endl;
	cout << "doc id:"<< pDoc->getDocID() << endl;
	pDoc->startIterator();
	while (pDoc->hasNext())
	{
		pField = pDoc->next();
		if(pField->getType() & CField::TEXT)
			cout << pField->getName()<< "\t->\t" << *(pField->textAValue());						
		else if(pField->getType() & CField::NUMBER)
		{
			//if(pField->getFieldType()==FIELD_DATE)
			{
				cout << pField->getName()<< "\t->\t" << CDateTime::format(pField->numberValue().value.i64val);				
			}
		}	
		if(s > 0.0)
			cout << "\tscore:" << s << endl;
		else cout << endl;
	}				
}

void loadQueries(vector<string>&queries,const tchar* pszQueryFile)
{
	char line[80];	
	FILE* file = fopen(pszQueryFile,"r");
	if(file == NULL)
	{
		FIRTEX_CLOG(level::err) << _T("open query file failed.") << FIRTEX_ENDL;			
	}
	if(fgets(line,80,file))
	{
		fclose(file);
		if(strstr(line,"<top>"))
			loadTRECTopic(queries,pszQueryFile);
		else loadQueryFile(queries,pszQueryFile);
	}
	else 
	{
		fclose(file);
	}
}

void loadQueryFile(vector<string>& queries,const tchar* pszQueryFile)
{
	char line[80];			
	int totalRet = 0;
	int totalLen = 0;

	FIRTEX_CLOG(level::info) << _T("Load query file:") << pszQueryFile << FIRTEX_ENDL;
	FILE* file = fopen(pszQueryFile,"r");
	if(file == NULL)
	{
		FIRTEX_CLOG(level::err) << _T("open query file failed.") << FIRTEX_ENDL;			
	}
	while (fgets(line,80,file))
	{		
		line[strlen(line)-1]=0;
		if ( strlen(line) == 0 )
			continue;
		totalLen += (int)strlen(line);
		queries.push_back(line);
		
	}
	fclose(file);
	if(queries.size() > 0)
	{		
		FIRTEX_CLOG(level::info) << _T("Total Queries:") << (int32_t)queries.size() << FIRTEX_ENDL;
		FIRTEX_CLOG(level::info) << _T("Average Query Length:") << totalLen/(int32_t)queries.size() << FIRTEX_ENDL;
	}
}
void loadTRECTopic(vector<string>&queries,const tchar* pszTopicFile)
{
	char query[80];		
	int totalRet = 0;
	int totalLen = 0;

	FIRTEX_CLOG(level::info) << _T("Load topic file:") << pszTopicFile << FIRTEX_ENDL;
	FILE* file = fopen(pszTopicFile,"r");
	if(file == NULL)
	{
		FIRTEX_CLOG(level::err) << _T("open topic file failed.") << FIRTEX_ENDL;			
	}
	fseek(file,0,SEEK_END);
	size_t len = ftell(file);
	char* buf = new char[len + 1];
	fseek(file,0,SEEK_SET);
	fread(buf,sizeof(char),len,file);
	fclose(file);
	buf[len] = 0;

	char* start = buf;
	char* eTag = strstr(start,"<title>");
	char* title;
	while (eTag)
	{
		eTag += 7;
		while (*eTag == ' ')
		{
			eTag++;
		}
		title = eTag;
		while ( *title != '\n' && *title != '\t')
		{
			title++;
		}
		strncpy(query,eTag,title - eTag);
		query[title - eTag] = 0;
		if ( strlen(query) > 0 )
		{
			totalLen += (int)strlen(query);					
			queries.push_back(query);
		}
		eTag = strstr(eTag,"<title>");
	}		
	delete[] buf;

	if(queries.size() > 0)
	{		
		FIRTEX_CLOG(level::info) << _T("Total Queries:") << (int32_t)queries.size() << FIRTEX_ENDL;
		FIRTEX_CLOG(level::info) << _T("Average Query Length:") << totalLen/(int32_t)queries.size() << FIRTEX_ENDL;
	}
}
