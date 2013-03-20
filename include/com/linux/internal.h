#include "com/Com.h"
#include <pthread.h>

/* Windows maps COINIT values to 0x80 for apartment threaded, 0x140
 * for free threaded, and 0 for uninitialized apartments. There is
 * no real advantage in us doing this and certainly no release version
 * of an app should be poking around with these flags. So we need a
 * special value for uninitialized */
#define COINIT_UNINITIALIZED 0x100

namespace firtex
{
	namespace com 
	{
		namespace internal 
		{
			struct apartment_t 
			{
				int32_t				lock_count;
				uint32_t			model;
				IErrorInfo*			errorinfo;
				pthread_t			tid;
				pthread_mutex_t 	mutex;
				apartment_t*		parent;
				apartment_t*		next;
				apartment_t*		prev;
			};

			enum log_level 
			{
				ERROR=1,WARN,INFO,NONE
			};

			struct marshal_id 
			{
				uint32_t	processid;
				uint32_t	objectid;	/* unique value corresp. IUnknown of object */
				FX_IID			iid;
			};


			struct thread_data_t 
			{
				apartment_t*	apartment;
			};

			class pthread_mutex_lock 
			{
			public:
				pthread_mutex_lock( pthread_mutex_t* m )
				{ m_ = m; ::pthread_mutex_lock( m ); };
				~pthread_mutex_lock()
				{ ::pthread_mutex_unlock( m_ ); };
			private:
				pthread_mutex_t* m_;
			};

			class zstr 
			{
			public:
				zstr() { s_ = 0; };
				zstr( char const* rhs );
				zstr( wchar_t const* rhs );
				~zstr();

				operator char const*() const { return s_; };

				zstr& operator=( char const* rhs );
				zstr& operator=( wchar_t const* rhs );
				bool append( char const* rhs );
				bool append( wchar_t const* rhs );

			private:
				char* s_;
				zstr( zstr const& );
			};

			class wzstr 
			{
			public:
				wzstr() { s_ = 0; };
				wzstr( char const* rhs );
				wzstr( wchar_t const* rhs );
				~wzstr();

				operator wchar_t const*() const { return s_; };

				wzstr& operator=( char const* rhs );
				wzstr& operator=( wchar_t const* rhs );
				bool append( char const* rhs );
				bool append( wchar_t const* rhs );

			private:
				wchar_t* s_;
				wzstr( wzstr const& );
			};

			template <class T>
				T* create() 
			{ 
				T* ret = 0; 
				try 
				{ 
					ret = new T(); 
				}
				catch (... ) 
				{
					// do nothing
				}
				return ret;
			}


			void log_printf( log_level, char const* format, ... );
			apartment_t* CreateApartment( uint32_t );
			void DestroyApartment( apartment_t* );
			thread_data_t*	GetThreadSpecificData();

			FX_HRESULT GetRegisteredClassObject( FX_REFCLSID rclsid, uint32_t dwClsContext, IUnknown** ppUnk);

			// InprocServer32 Module
			FX_HRESULT GetInprocClassObject( FX_REFCLSID rclsid, IUnknown** ppUnk );
			void	FreeUnusedInprocServers();

			// special hooks for UNIX implementation
			wchar_t const* regsvr32_get_filename();
			FX_HRESULT regsvr32_set_filename( wchar_t const* filename );

		};//end internal

	};//end com
}//end firtex

