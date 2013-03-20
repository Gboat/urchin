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
// Created	: 2006/5/6
//
#ifndef _LOGGER_H
#define _LOGGER_H

#include "StdHeader.h"
#include "thread/FirteXThreads.h"
#include <ostream>
#include <fstream>
#include <iosfwd>

#ifdef _UNICODE
#define tstreambuf std::wstreambuf
#define tostream std::wostream
#else
#define tstreambuf std::streambuf
#define tostream std::ostream
#endif

namespace firtex
{
	namespace utility
	{
		//////////////////////////////////////////////////////////////////////////
		//log_delegater
		template<class CharT>
		class log_delegater
		{
		public:
			log_delegater(){}
			~log_delegater(){}
		public:
			virtual void log(const CharT* text)=0;		
		};

		#ifdef _UNICODE
			typedef log_delegater<wchar_t> tlog_delegater;
		#else
			typedef log_delegater<char> tlog_delegater;
		#endif      


		//////////////////////////////////////////////////////////////////////////
		//basic_logbuf
		template <class CharT, class TraitsT = std::char_traits<CharT> >
		class basic_logbuf : 
			public std::basic_stringbuf<CharT, TraitsT>
		{
		public:
			basic_logbuf():std::basic_stringbuf<CharT, TraitsT>(),m_delegater(NULL)
			{
			}
			basic_logbuf(log_delegater<CharT>* delegater):m_delegater(delegater)
			{
			}
			virtual ~basic_logbuf()
			{
				//sync();
				m_delegater = NULL;
			}

		public:
			void			setDelegater(log_delegater<CharT>* delegater){m_delegater = delegater;}
			log_delegater<CharT>*	getDelegater(){return m_delegater;}
		protected:
			int sync()
			{
				output_log_string(this->str().c_str());
				str(std::basic_string<CharT>());    // Clear the string buffer

				return 0;
			}

			void output_log_string(const CharT *text) {if(m_delegater) m_delegater->log(text);}
		protected:
			log_delegater<CharT>*		m_delegater;
		};	

		//////////////////////////////////////////////////////////////////////////
		//basic_logostream
		template<class CharT, class TraitsT = std::char_traits<CharT> >
		class basic_logostream : 
			public std::basic_ostream<CharT, TraitsT>
		{
		public:

			basic_logostream() : std::basic_ostream<CharT, TraitsT>
				(new basic_logbuf<CharT, TraitsT>()) 
			{
			}
			basic_logostream(log_delegater<CharT>* delegater) : std::basic_ostream<CharT, TraitsT>
				(new basic_logbuf<CharT, TraitsT>(delegater)) 
			{
			}
			~basic_logostream() 
			{
				delete this->rdbuf(); 
			}
		public:
			void			setDelegater(log_delegater<CharT>* delegater){((basic_logbuf<CharT, TraitsT>*)this->rdbuf())->setDelegater(delegater);}
			log_delegater<CharT>*	getDelegater(){return ((basic_logbuf<CharT, TraitsT>*)this->rdbuf())->getDelegater();}
		public:
			DEFINE_MUTEX(LOCK);
		};

		typedef basic_logostream<char>    logostream;
		typedef basic_logostream<wchar_t> wlogostream;

		//////////////////////////////////////////////////////////////////////////
		//stream_delegater
		template<class CharT, class TraitsT = std::char_traits<CharT> >
		class stream_delegater: public log_delegater<CharT>
		{
		public:
			stream_delegater(std::basic_ostream<CharT,TraitsT>* os):m_ostream(os)
			{
			}
			~stream_delegater()
			{
				m_ostream = NULL;
			}
		public:		
			void	log(const CharT* text)
			{
				if(m_ostream)
					(*m_ostream) << text;
			}
		protected:			
			std::basic_ostream<CharT,TraitsT>*	m_ostream;
		};

		//////////////////////////////////////////////////////////////////////////
		//file_delegater
		template<class CharT, class TraitsT = std::char_traits<CharT> >
		class file_delegater: public log_delegater<CharT>
		{
		public:
			file_delegater(const char* logfile)
			{
				m_ostream.open(logfile,ios_base::out);
				if(!m_ostream.is_open())
				{
					FIRTEX_THROW2(FILEIO_ERROR,_T("open log file error."));
				}
			}
			~file_delegater()
			{
				if(m_ostream.is_open())
				{
					m_ostream.close();					
				}
			}
		public:		
			void	log(const CharT* text);
		protected:			
			std::basic_ofstream<CharT,TraitsT>	m_ostream;
		};

		template<>
			inline void file_delegater<char>::log(const char* text)
		{
			if(text)
			{
				m_ostream << text;
			}
		}
		template<>
			inline void file_delegater<wchar_t>::log(const wchar_t* text)
		{
			if(text)
				m_ostream << text;
		}
		//////////////////////////////////////////////////////////////////////////
		//basic_logger
		typedef unsigned int level_type;

		#define LOG_DEFINE_LEVEL(lvl, value) namespace level { const level_type lvl = (level_type)(value); }

		LOG_DEFINE_LEVEL(disable_all, -1)
		LOG_DEFINE_LEVEL(default_, 1000)
		// alias...
		LOG_DEFINE_LEVEL(default_level, 1000)
		LOG_DEFINE_LEVEL(enable_all, 0)

		LOG_DEFINE_LEVEL(fatal, 2000)
		LOG_DEFINE_LEVEL(err, 1600)
		LOG_DEFINE_LEVEL(warn, 1200)
		LOG_DEFINE_LEVEL(info, 1000)
		LOG_DEFINE_LEVEL(dbg, 800)


		#define FIRTEX_LOG(logger,log_level)\
			{if(tlogger::isEnabled(log_level) == false) {;} else { SCOPED_LOCK(logger.LOCK);logger.level(log_level)
		#define FIRTEX_ENDL	endl;}}
		#define FIRTEX_END _T("");}}

		#define ENABLE_LOGS(log_level) basic_logger::enableLogs(log_level);

		template<class CharT, class TraitsT = std::char_traits<CharT> >
		class basic_logger : public basic_logostream<CharT,TraitsT>
		{
		public:			
			basic_logger()
			{
				m_curLogLevel = level::info;
				m_delegater = new stream_delegater<CharT>(&std::cout);
				setDelegater(m_delegater);
				m_ownDelegater = true;
			}
			basic_logger(basic_ostream<CharT,TraitsT>& os)
			{
				m_curLogLevel = level::info;
				m_delegater = new stream_delegater<CharT>(&os);
				setDelegater(m_delegater);
			}
			~basic_logger()
			{
				this->flush();
				if((m_ownDelegater == true) && m_delegater)
				{
					delete m_delegater;
					m_delegater = NULL;
				}
			}
		public:
			basic_logger<CharT,TraitsT>& level(level_type level)
			{
				m_curLogLevel = level;
				//if(m_prefix.empty() == false)
					//(*this) << m_prefix;
				return *this;
			}

			bool	canLog(){return (m_curLogLevel >= basic_logger::m_logLevel);}			

			static bool isEnabled(level_type lvl){return (lvl >= m_logLevel);}
			static void	enableLogs(level_type lvl){m_logLevel = lvl;}
			static void	enableLogs(const basic_string<CharT,TraitsT>& logstr)
			{
				if(logstr == _T("default_level"))
				{
					enableLogs(level::default_level);
				}
				else if(logstr == _T("enable_all"))
				{
					enableLogs(level::enable_all);
				}
				else if(logstr == _T("disable_all"))
				{
					enableLogs(level::disable_all);
				}
				else if(logstr == _T("fatal"))
				{
					enableLogs(level::fatal);
				}
				else if(logstr == _T("err"))
				{
					enableLogs(level::err);
				}
				else if(logstr == _T("warn"))
				{
					enableLogs(level::warn);
				}
				else if(logstr == _T("info"))
				{
					enableLogs(level::info);
				}
				else if(logstr == _T("dbg"))
				{
					enableLogs(level::dbg);
				}
				else 
				{
					enableLogs(level::default_level);
				}
			}
		public:
			/**
			 * 设置logger stream
			 * @param wos logger stream
			 */
			void	reset(basic_ostream<CharT,TraitsT>& os)
			{
				if((m_ownDelegater == true) && m_delegater)
				{
					delete m_delegater;
				}
				m_delegater = new stream_delegater<CharT>(&os);
				setDelegater(m_delegater);
				m_ownDelegater = true;
			}			
			
			/**
			 * 设置log 文件
			 * @param logfile log文件
			 */
			void	reset(const CharT* logfile)
			{
				if((m_ownDelegater == true) && m_delegater)
				{
					delete m_delegater;
				}
				m_delegater = new file_delegater<CharT>(logfile);
				setDelegater(m_delegater);
				m_ownDelegater = true;
			}

			/**
			 * 设置logs输出代理
			 * @param delegater 输出代理，由外部维护和删除
			 */
			void reset(log_delegater<CharT>* delegater)
			{
				if((m_ownDelegater == true) && m_delegater)
				{
					delete m_delegater;
				}
				m_delegater = delegater;
				setDelegater(m_delegater);
				m_ownDelegater = false;
			}
		protected:		
			log_delegater<CharT>*	m_delegater;
			level_type				m_curLogLevel;
			bool					m_ownDelegater;
			
			basic_string<CharT>		m_prefix;			

            static level_type		m_logLevel;			
		};			
		//////////////////////////////////////////////////////////////////////////
		//
		
		

		typedef basic_logger<char> logger;
		typedef basic_logger<wchar_t> wlogger;
		#ifdef _UNICODE
			typedef wlogger tlogger;
		#else
			typedef logger tlogger;
		#endif      
	}
}

#endif
