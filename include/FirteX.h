//
// Copyright(C) 2005--2006 Institute of Computing Tech, Chinese Academy of Sciences. 
// All rights reserved.
// This file is part of FirteX (www.firtex.org)
//
// Use of the FirteX is subject to the terms of the software license set forth in 
// the LICENSE file included with this software, and also available at
// http://www.firtex.org/license.html
//
// Author	: ¹ùÈð½Ü(GuoRuijie)
// Email	: ruijieguo@software.ict.ac.cn,ruijieguo@gmail.com
// Created	: 2006/5/20
//

#ifndef _FIRTEX_H
#define _FIRTEX_H

#include "utility/StdHeader.h"
#include "utility/SpeedProbe.h"
#include "utility/Configurator.h"
#include "utility/ParamParser.h"
#include "utility/Config.h"

#include "parser/PlainParser.h"
#include "parser/ParserFactory.h"

#include "analyzer/AnalyzerFactory.h"
#include "analyzer/StandardAnalyzer.h"

#include "collection/FileListCollection.h"
#include "collection/DirCollection.h"
#include "collection/CollectionFactory.h"
#include "collection/FirteXCollection.h"
#include "collection/CollectionFactory.h"

#include "index/IndexWriter.h"
#include "index/Index.h"
#include "index/IndexReader.h"
#include "index/MultiIndexBarrelReader.h"
#include "index/ForwardIndexReader.h"

#include "search/IndexSearcher.h"
#include "search/TermQuery.h"
#include "search/QueryParser.h"

#include "plugin/Plugins.h"


#endif
