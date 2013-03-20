#ifndef _COMINTERFACES_H_
#define _COMINTERFACES_H_

/* this function should only be called from within Com.h */
#ifndef _COM_H_
	#error The include file 'ComInterfaces.h' should not be called directly.
#endif

#ifdef __cplusplus
namespace firtex
{
	namespace com 
	{
#endif

	//////////////////////////////////////////////////////
	// Helpful Defines
	//////////////////////////////////////////////////////

#ifndef __cplusplus

	#define START_C_INTERFACE( iname ) \
		struct iname##Vtbl { \
		FX_BEGIN_INTERFACE \
		FX_HRESULT ( FX_STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( \
		struct iname __RPC_FAR * This, \
		FX_REFIID riid, \
		void __RPC_FAR *__RPC_FAR *ppvObject); \
		uint32_t ( FX_STDMETHODCALLTYPE __RPC_FAR *AddRef )( \
		struct iname __RPC_FAR * This); \
		uint32_t ( FX_STDMETHODCALLTYPE __RPC_FAR *Release )( \
		struct iname __RPC_FAR * This);

	#define END_C_INTERFACE( iname ) \
		FX_END_INTERFACE \
		}; \
		struct iname { \
		const struct iname##Vtbl __RPC_FAR *lpVtbl; \
		}; \
		EXTERN_C FX_IID const IID_##iname;

#endif // __cplusplus

	//////////////////////////////////////////////////////
	// IUnknown
	//////////////////////////////////////////////////////

#ifdef __cplusplus
	/* uuid {00000000-0000-0000-C000-000000000046} */
	class IUnknown 
	{
	public:
		static const firtex::com::FX_IID iid;

		virtual FX_HRESULT FX_STDMETHODCALLTYPE QueryInterface( /* [in] */ FX_REFIID riid, /* [out] */ void __RPC_FAR *__RPC_FAR *ppvObject ) = 0;
		virtual uint32_t FX_STDMETHODCALLTYPE AddRef( void ) = 0;
		virtual uint32_t FX_STDMETHODCALLTYPE Release( void ) = 0;
	};
#else
	START_C_INTERFACE( IUnknown )
		END_C_INTERFACE( IUnknown );

#endif /* IUnknown */

	//////////////////////////////////////////////////////
	// IClassFactory
	//////////////////////////////////////////////////////

#ifdef __cplusplus
	/* uuid {00000001-0000-0000-C000-000000000046} */
	class IClassFactory : public IUnknown  
	{
	public:
		static const firtex::com::FX_IID iid;

		virtual firtex::com::FX_HRESULT FX_STDMETHODCALLTYPE CreateInstance( /* [in] */ IUnknown __RPC_FAR* pUnkOuter, /* [in] */ FX_REFIID riid, /* [out] */ void __RPC_FAR *__RPC_FAR * ppvObject ) = 0;
		virtual firtex::com::FX_HRESULT FX_STDMETHODCALLTYPE LockServer( /* [in] */ int32_t fLock ) = 0;
	};
#else
	START_C_INTERFACE( IClassFactory )
		FX_HRESULT ( FX_STDMETHODCALLTYPE __RPC_FAR *CreateInstance )(
	struct IClassFactory __RPC_FAR * This,
		/* [in] */ IUnknown __RPC_FAR* pUnkOuter, 
		/* [in] */ FX_REFIID riid, 
		/* [out] */ void __RPC_FAR *__RPC_FAR * ppvObject);

		FX_HRESULT ( FX_STDMETHODCALLTYPE __RPC_FAR *LockServer )(
	struct IClassFactory __RPC_FAR * This,
		/* [in] */ int32_t fLock);
		END_C_INTERFACE( IClassFactory );
#endif /* IClassFactory */

	//////////////////////////////////////////////////////
	// ICreateErrorInfo
	//////////////////////////////////////////////////////

#ifdef __cplusplus
	/* uuid {00000001-0000-0000-C000-000000000046} */
	class ICreateErrorInfo : public IUnknown 
	{
	public:
		static FX_IID const iid;

		virtual FX_HRESULT FX_STDMETHODCALLTYPE SetGUID( 
			/* [in] */ FX_REFGUID rguid) = 0;

			virtual FX_HRESULT FX_STDMETHODCALLTYPE SetSource( 
			/* [in] */ wchar_t const __RPC_FAR* szSource) = 0;

			virtual FX_HRESULT FX_STDMETHODCALLTYPE SetDescription( 
			/* [in] */ wchar_t const __RPC_FAR* szDescription) = 0;

			virtual FX_HRESULT FX_STDMETHODCALLTYPE SetHelpFile( 
			/* [in] */ wchar_t const __RPC_FAR* szHelpFile) = 0;

			virtual FX_HRESULT FX_STDMETHODCALLTYPE SetHelpContext( 
			/* [in] */ uint32_t dwHelpContext) = 0;
	};
#else
	START_C_INTERFACE( ICreateErrorInfo )
		FX_HRESULT ( FX_STDMETHODCALLTYPE __RPC_FAR * SetGUID)( 
		ICreateErrorInfo __RPC_FAR * This,
		/* [in] */ FX_REFGUID rguid);

		FX_HRESULT ( FX_STDMETHODCALLTYPE __RPC_FAR * SetSource)( 
		ICreateErrorInfo __RPC_FAR * This,
		/* [in] */ wchar_t const __RPC_FAR* szSource);

		FX_HRESULT ( FX_STDMETHODCALLTYPE __RPC_FAR * SetDescription)( 
		ICreateErrorInfo __RPC_FAR * This,
		/* [in] */ wchar_t const __RPC_FAR* szDescription);

		FX_HRESULT ( FX_STDMETHODCALLTYPE __RPC_FAR * SetHelpFile)( 
		ICreateErrorInfo __RPC_FAR * This,
		/* [in] */ wchar_t const __RPC_FAR* szHelpFile);

		FX_HRESULT ( FX_STDMETHODCALLTYPE __RPC_FAR * SetHelpContext)( 
		ICreateErrorInfo __RPC_FAR * This,
		/* [in] */ uint32_t dwHelpContext);
		END_C_INTERFACE( ICreateErrorInfo );
#endif /* ICreateErrorInfo */

#ifdef __cplusplus
	/* uuid {00000001-0000-0000-C000-000000000046} */
	class IErrorInfo : public IUnknown  
	{
	public:
		static const firtex::com::FX_IID iid;

		virtual firtex::com::FX_HRESULT FX_STDMETHODCALLTYPE GetGUID( 
			/* [out] */ FX_GUID __RPC_FAR *pGUID) = 0;

			virtual firtex::com::FX_HRESULT FX_STDMETHODCALLTYPE GetSource( 
			/* [out] */ FX_BSTR __RPC_FAR *pBstrSource) = 0;

			virtual firtex::com::FX_HRESULT FX_STDMETHODCALLTYPE GetDescription( 
			/* [out] */ FX_BSTR __RPC_FAR *pBstrDescription) = 0;

			virtual firtex::com::FX_HRESULT FX_STDMETHODCALLTYPE GetHelpFile( 
			/* [out] */ FX_BSTR __RPC_FAR *pBstrHelpFile) = 0;

			virtual firtex::com::FX_HRESULT FX_STDMETHODCALLTYPE GetHelpContext( 
			/* [out] */ uint32_t __RPC_FAR *pdwHelpContext) = 0;
	};
#else
	START_C_INTERFACE( IErrorInfo )
		FX_HRESULT ( FX_STDMETHODCALLTYPE __RPC_FAR *GetGUID )( 
		IErrorInfo __RPC_FAR * This,
	/* [out] */ struct FX_GUID __RPC_FAR *pGUID);

		FX_HRESULT ( FX_STDMETHODCALLTYPE __RPC_FAR *GetSource )( 
		IErrorInfo __RPC_FAR * This,
		/* [out] */ FX_BSTR __RPC_FAR *pBstrSource);

		FX_HRESULT ( FX_STDMETHODCALLTYPE __RPC_FAR *GetDescription )( 
		IErrorInfo __RPC_FAR * This,
		/* [out] */ FX_BSTR __RPC_FAR *pBstrDescription);

		FX_HRESULT ( FX_STDMETHODCALLTYPE __RPC_FAR *GetHelpFile )( 
		IErrorInfo __RPC_FAR * This,
		/* [out] */ FX_BSTR __RPC_FAR *pBstrHelpFile);

		FX_HRESULT ( FX_STDMETHODCALLTYPE __RPC_FAR *GetHelpContext )( 
		IErrorInfo __RPC_FAR * This,
		/* [out] */ uint32_t __RPC_FAR *pdwHelpContext);
		END_C_INTERFACE( IErrorInfo )
#endif /* IClassFactory */

#ifdef __cplusplus
		/* uuid {00000146-0000-0000-C000-000000000046} */
	class IGlobalInterfaceTable : public IUnknown 
	{
	public:
		static const firtex::com::FX_IID iid;

		virtual FX_HRESULT FX_STDMETHODCALLTYPE RegisterInterfaceInGlobal( 
			/* [in] */ IUnknown __RPC_FAR *pUnk,
			/* [in] */ FX_REFIID riid,
			/* [out] */ uint32_t __RPC_FAR *pdwCookie) = 0;

			virtual FX_HRESULT FX_STDMETHODCALLTYPE RevokeInterfaceFromGlobal( 
			/* [in] */ uint32_t dwCookie) = 0;

			virtual FX_HRESULT FX_STDMETHODCALLTYPE GetInterfaceFromGlobal( 
			/* [in] */ uint32_t dwCookie,
			/* [in] */ FX_REFIID riid,
			/* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppv) = 0;
	};
#else
		START_C_INTERFACE( IGlobalInterfaceTable )
		FX_HRESULT ( FX_STDMETHODCALLTYPE __RPC_FAR *RegisterInterfaceInGlobal )( 
		IGlobalInterfaceTable __RPC_FAR * This,
		/* [in] */ IUnknown __RPC_FAR *pUnk,
		/* [in] */ FX_REFIID riid,
		/* [out] */ uint32_t __RPC_FAR *pdwCookie);

		FX_HRESULT ( FX_STDMETHODCALLTYPE __RPC_FAR *RevokeInterfaceFromGlobal )( 
		IGlobalInterfaceTable __RPC_FAR * This,
		/* [in] */ uint32_t dwCookie);

		FX_HRESULT ( FX_STDMETHODCALLTYPE __RPC_FAR *GetInterfaceFromGlobal )( 
		IGlobalInterfaceTable __RPC_FAR * This,
		/* [in] */ uint32_t dwCookie,
		/* [in] */ FX_REFIID riid,
		/* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppv);

		END_C_INTERFACE( IGlobalInterfaceTable )
#endif /* IGlobalInterfaceTable */

#ifdef __cplusplus
		/* uuid {00000002-0000-0000-C000-000000000046} */
	class IMarshal : public IUnknown 
	{
	public:
		static const firtex::com::FX_IID iid;

		virtual FX_HRESULT FX_STDMETHODCALLTYPE GetUnmarshalClass( 
			/* [in] */ FX_REFIID riid,
			/* [unique][in] */ void __RPC_FAR *pv,
			/* [in] */ uint32_t dwDestContext,
			/* [unique][in] */ void __RPC_FAR *pvDestContext,
			/* [in] */ uint32_t mshlflags,
			/* [out] */ FX_CLSID __RPC_FAR *pCid) = 0;

			virtual FX_HRESULT FX_STDMETHODCALLTYPE GetMarshalSizeMax( 
			/* [in] */ FX_REFIID riid,
			/* [unique][in] */ void __RPC_FAR *pv,
			/* [in] */ uint32_t dwDestContext,
			/* [unique][in] */ void __RPC_FAR *pvDestContext,
			/* [in] */ uint32_t mshlflags,
			/* [out] */ uint32_t __RPC_FAR *pSize) = 0;

			virtual FX_HRESULT FX_STDMETHODCALLTYPE MarshalInterface( 
			/* [unique][in] */ IStream __RPC_FAR *pStm,
			/* [in] */ FX_REFIID riid,
			/* [unique][in] */ void __RPC_FAR *pv,
			/* [in] */ uint32_t dwDestContext,
			/* [unique][in] */ void __RPC_FAR *pvDestContext,
			/* [in] */ uint32_t mshlflags) = 0;

			virtual FX_HRESULT FX_STDMETHODCALLTYPE UnmarshalInterface( 
			/* [unique][in] */ IStream __RPC_FAR *pStm,
			/* [in] */ FX_REFIID riid,
			/* [out] */ void __RPC_FAR *__RPC_FAR *ppv) = 0;

			virtual FX_HRESULT FX_STDMETHODCALLTYPE ReleaseMarshalData( 
			/* [unique][in] */ IStream __RPC_FAR *pStm) = 0;

			virtual FX_HRESULT FX_STDMETHODCALLTYPE DisconnectObject( 
			/* [in] */ uint32_t dwReserved) = 0;
	};
#else
		START_C_INTERFACE( IMarshal )
		FX_HRESULT ( FX_STDMETHODCALLTYPE __RPC_FAR *GetUnmarshalClass)(
		IMarshal __RPC_FAR* This, 
		/* [in] */ FX_REFIID riid,
		/* [unique][in] */ void __RPC_FAR *pv,
		/* [in] */ uint32_t dwDestContext,
		/* [unique][in] */ void __RPC_FAR *pvDestContext,
		/* [in] */ uint32_t mshlflags,
		/* [out] */ FX_CLSID __RPC_FAR *pCid);

		FX_HRESULT ( FX_STDMETHODCALLTYPE __RPC_FAR *GetMarshalSizeMax)( 
		IMarshal __RPC_FAR* This, 
		/* [in] */ FX_REFIID riid,
		/* [unique][in] */ void __RPC_FAR *pv,
		/* [in] */ uint32_t dwDestContext,
		/* [unique][in] */ void __RPC_FAR *pvDestContext,
		/* [in] */ uint32_t mshlflags,
		/* [out] */ uint32_t __RPC_FAR *pSize);

		FX_HRESULT ( FX_STDMETHODCALLTYPE __RPC_FAR *MarshalInterface)( 
		IMarshal __RPC_FAR* This, 
		/* [unique][in] */ IStream __RPC_FAR *pStm,
		/* [in] */ FX_REFIID riid,
		/* [unique][in] */ void __RPC_FAR *pv,
		/* [in] */ uint32_t dwDestContext,
		/* [unique][in] */ void __RPC_FAR *pvDestContext,
		/* [in] */ uint32_t mshlflags);

		FX_HRESULT ( FX_STDMETHODCALLTYPE __RPC_FAR *UnmarshalInterface)( 
		IMarshal __RPC_FAR* This, 
		/* [unique][in] */ IStream __RPC_FAR *pStm,
		/* [in] */ FX_REFIID riid,
		/* [out] */ void __RPC_FAR *__RPC_FAR *ppv);

		FX_HRESULT ( FX_STDMETHODCALLTYPE __RPC_FAR *ReleaseMarshalData)( 
		IMarshal __RPC_FAR* This, 
		/* [unique][in] */ IStream __RPC_FAR *pStm);

		FX_HRESULT ( FX_STDMETHODCALLTYPE __RPC_FAR *DisconnectObject)( 
		IMarshal __RPC_FAR* This, 
		/* [in] */ uint32_t dwReserved);
		END_C_INTERFACE( IMarshal )
#endif /* IMarshal */

#ifdef __cplusplus
		/* uuid {00000002-0000-0000-C000-000000000046} */
	class IMalloc : public IUnknown 
	{
	public:
		static const firtex::com::FX_IID iid;

		virtual void __RPC_FAR *FX_STDMETHODCALLTYPE Alloc( 
			/* [in] */ uint32_t cb) = 0;

			virtual void __RPC_FAR *FX_STDMETHODCALLTYPE Realloc( 
			/* [in] */ void __RPC_FAR *pv,
			/* [in] */ uint32_t cb) = 0;

			virtual void FX_STDMETHODCALLTYPE Free( 
			/* [in] */ void __RPC_FAR *pv) = 0;

			virtual uint32_t FX_STDMETHODCALLTYPE GetSize( 
			/* [in] */ void __RPC_FAR *pv) = 0;

			virtual int16_t FX_STDMETHODCALLTYPE DidAlloc( 
			void __RPC_FAR *pv) = 0;

		virtual void FX_STDMETHODCALLTYPE HeapMinimize( void) = 0;
	};
#else
	START_C_INTERFACE( IMalloc )
		void __RPC_FAR * ( FX_STDMETHODCALLTYPE __RPC_FAR *Alloc)( 
		IMalloc __RPC_FAR * This,
		/* [in] */ void __RPC_FAR *pv,
		/* [in] */ uint32_t cb);

		void __RPC_FAR *( FX_STDMETHODCALLTYPE __RPC_FAR *Realloc)( 
		IMalloc __RPC_FAR * This,
		/* [in] */ uint32_t dwCookie);

		void ( FX_STDMETHODCALLTYPE __RPC_FAR *Free )( 
		IMalloc __RPC_FAR * This,
		/* [in] */ void __RPC_FAR *pv);

		uint32_t ( FX_STDMETHODCALLTYPE __RPC_FAR *GetSize)( 
		IMalloc __RPC_FAR * This,
		/* [in] */ void __RPC_FAR *pv);

		int16_t ( FX_STDMETHODCALLTYPE __RPC_FAR *DidAlloc)( IMalloc __RPC_FAR * This,void __RPC_FAR *pv);

		void ( FX_STDMETHODCALLTYPE __RPC_FAR *HeapMinimize)(IMalloc __RPC_FAR * This);

	END_C_INTERFACE( IMalloc )
#endif /* IMalloc */

#ifdef __cplusplus

	class IMallocSpy : public IUnknown
	{
	public:		
		virtual uint32_t FX_STDMETHODCALLTYPE PreAlloc(uint32_t cbRequest) = 0;
		virtual void __RPC_FAR *FX_STDMETHODCALLTYPE PostAlloc(void __RPC_FAR *pActual) = 0;
		virtual void __RPC_FAR *FX_STDMETHODCALLTYPE PreFree(void __RPC_FAR *pRequest,bool_t fSpyed) = 0;
		virtual void FX_STDMETHODCALLTYPE PostFree(bool_t fSpyed) = 0;
		virtual uint32_t FX_STDMETHODCALLTYPE PreRealloc(void __RPC_FAR *pRequest,uint32_t cbRequest,void __RPC_FAR *__RPC_FAR *ppNewRequest,bool_t fSpyed) = 0;
		virtual void __RPC_FAR *FX_STDMETHODCALLTYPE PostRealloc(void __RPC_FAR *pActual,bool_t fSpyed) = 0;
		virtual void __RPC_FAR *FX_STDMETHODCALLTYPE PreGetSize(void __RPC_FAR *pRequest,bool_t fSpyed) = 0;
		virtual uint32_t FX_STDMETHODCALLTYPE PostGetSize(uint32_t cbActual,bool_t fSpyed) = 0;
		virtual void __RPC_FAR *FX_STDMETHODCALLTYPE PreDidAlloc(void __RPC_FAR *pRequest,bool_t fSpyed) = 0;
		virtual int16_t FX_STDMETHODCALLTYPE PostDidAlloc(void __RPC_FAR *pRequest,bool_t fSpyed,int16_t fActual) = 0;
		virtual void FX_STDMETHODCALLTYPE PreHeapMinimize( void) = 0;
		virtual void FX_STDMETHODCALLTYPE PostHeapMinimize( void) = 0;	
	};

#else 	/* C style interface */


	START_C_INTERFACE( IMallocSpy )
		uint32_t (FX_STDMETHODCALLTYPE __RPC_FAR *PreAlloc)(uint32_t cbRequest) ;
		void __RPC_FAR *(FX_STDMETHODCALLTYPE __RPC_FAR *PostAlloc)(void __RPC_FAR *pActual) ;
		void __RPC_FAR *(FX_STDMETHODCALLTYPE __RPC_FAR *PreFree)(void __RPC_FAR *pRequest,bool_t fSpyed) ;
		void (FX_STDMETHODCALLTYPE __RPC_FAR *PostFree)(bool_t fSpyed) ;
		uint32_t (FX_STDMETHODCALLTYPE __RPC_FAR *PreRealloc)(void __RPC_FAR *pRequest,uint32_t cbRequest,void __RPC_FAR *__RPC_FAR *ppNewRequest,bool_t fSpyed) ;
		void __RPC_FAR *(FX_STDMETHODCALLTYPE __RPC_FAR *PostRealloc)(void __RPC_FAR *pActual,bool_t fSpyed) ;
		void __RPC_FAR *(FX_STDMETHODCALLTYPE __RPC_FAR *PreGetSize)(void __RPC_FAR *pRequest,bool_t fSpyed) ;
		uint32_t (FX_STDMETHODCALLTYPE __RPC_FAR *PostGetSize)(uint32_t cbActual,bool_t fSpyed) ;
		void __RPC_FAR *(FX_STDMETHODCALLTYPE __RPC_FAR *PreDidAlloc)(void __RPC_FAR *pRequest,bool_t fSpyed) ;
		int16_t (FX_STDMETHODCALLTYPE __RPC_FAR *PostDidAlloc)(void __RPC_FAR *pRequest,bool_t fSpyed,int16_t fActual) ;
		void (FX_STDMETHODCALLTYPE __RPC_FAR *PreHeapMinimize)( void) ;
		void (FX_STDMETHODCALLTYPE __RPC_FAR * PostHeapMinimize)( void) ;	
	END_C_INTERFACE(IMallocSpy)	

#endif /* IMallocSpy */

#ifdef __cplusplus
		/* uuid {0c733a30-2a1c-11ce-ade5-00aa0044773d} */
	class ISequentialStream : public IUnknown  
	{
	public:
		static const firtex::com::FX_IID iid;

		virtual /* [local] */ FX_HRESULT FX_STDMETHODCALLTYPE Read( 
			/* [length_is][size_is][out] */ void __RPC_FAR *pv,
			/* [in] */ uint32_t cb,
			/* [out] */ uint32_t __RPC_FAR *pcbRead) = 0;

			virtual /* [local] */ FX_HRESULT FX_STDMETHODCALLTYPE Write( 
			/* [size_is][in] */ const void __RPC_FAR *pv,
			/* [in] */ uint32_t cb,
			/* [out] */ uint32_t __RPC_FAR *pcbWritten) = 0;
	};
#else
		START_C_INTERFACE( ISequentialStream )
		/* [local] */ FX_HRESULT ( FX_STDMETHODCALLTYPE __RPC_FAR *Read )( 
		ISequentialStream __RPC_FAR * This,
		/* [length_is][size_is][out] */ void __RPC_FAR *pv,
		/* [in] */ uint32_t cb,
		/* [out] */ uint32_t __RPC_FAR *pcbRead);

		/* [local] */ FX_HRESULT ( FX_STDMETHODCALLTYPE __RPC_FAR *Write )( 
		ISequentialStream __RPC_FAR * This,
		/* [size_is][in] */ const void __RPC_FAR *pv,
		/* [in] */ uint32_t cb,
		/* [out] */ uint32_t __RPC_FAR *pcbWritten);

		END_C_INTERFACE( ISequentialStream )
#endif /* ISequentialStream */

		/* Is this correct */
		typedef int64_t FILETIME;

#ifdef _MAC
	typedef struct tagSTATSTG
	{                      
		LPOLESTR pwcsName;
		FSSpec *pspec;
		DWORD type;
		ULARGE_INTEGER cbSize;
		FILETIME mtime;
		FILETIME ctime;
		FILETIME atime;
		DWORD grfMode;
		DWORD grfLocksSupported;
		FX_CLSID clsid;
		DWORD grfStateBits;
		DWORD reserved;
	} STATSTG;
#else //_MAC
	typedef struct  tagSTATSTG
	{
		wchar_t const __RPC_FAR * pwcsName;
		uint32_t type;
		uint64_t cbSize;
		FILETIME mtime;
		FILETIME ctime;
		FILETIME atime;
		uint32_t grfMode;
		uint32_t grfLocksSupported;
		FX_CLSID clsid;
		uint32_t grfStateBits;
		uint32_t reserved;
	}	STATSTG;

#endif //_MAC

#ifdef __cplusplus
	/* uuid {0000000c-0000-0000-C000-000000000046} */
	class IStream : public ISequentialStream  
	{
	public:
		static const firtex::com::FX_IID iid;

#ifdef _MAC
		struct STATSTG
		{                      
			LPOLESTR pwcsName;
			FSSpec *pspec;
			DWORD type;
			ULARGE_INTEGER cbSize;
			FILETIME mtime;
			FILETIME ctime;
			FILETIME atime;
			DWORD grfMode;
			DWORD grfLocksSupported;
			FX_CLSID clsid;
			DWORD grfStateBits;
			DWORD reserved;
		} STATSTG;
#else //_MAC
		struct STATSTG 
		{
			wchar_t const* pwcsName;
			uint32_t type;
			uint64_t cbSize;
			uint64_t mtime;
			uint64_t ctime;
			uint64_t atime;
			uint32_t grfMode;
			uint32_t grfLocksSupported;
			FX_CLSID clsid;
			uint32_t grfStateBits;
			uint32_t reserved;
		};
#endif // _MAC
		enum STGTY 
		{
			STGTY_STORAGE	= 1,
			STGTY_STREAM	= 2,
			STGTY_LOCKBYTES	= 3,
			STGTY_PROPERTY	= 4
		};
		enum STREAM_SEEK 
		{
			STREAM_SEEK_SET	= 0,
			STREAM_SEEK_CUR	= 1,
			STREAM_SEEK_END	= 2
		};
		/*enum LOCKTYPE 
		{
			LOCK_WRITE	= 1,
			LOCK_EXCLUSIVE	= 2,
			LOCK_ONLYONCE	= 4
		};*/

		virtual /* [local] */ FX_HRESULT FX_STDMETHODCALLTYPE Seek( 
			/* [in] */ int64_t dlibMove,
			/* [in] */ uint32_t dwOrigin,
			/* [out] */ uint64_t __RPC_FAR *plibNewPosition) = 0;

			virtual FX_HRESULT FX_STDMETHODCALLTYPE SetSize( 
			/* [in] */ uint64_t libNewSize) = 0;

			virtual /* [local] */ FX_HRESULT FX_STDMETHODCALLTYPE CopyTo( 
			/* [unique][in] */ IStream __RPC_FAR *pstm,
			/* [in] */ uint64_t cb,
			/* [out] */ uint64_t __RPC_FAR *pcbRead,
			/* [out] */ uint64_t __RPC_FAR *pcbWritten) = 0;

			virtual FX_HRESULT FX_STDMETHODCALLTYPE Commit( 
			/* [in] */ uint32_t grfCommitFlags) = 0;

			virtual FX_HRESULT FX_STDMETHODCALLTYPE Revert( void) = 0;

		virtual FX_HRESULT FX_STDMETHODCALLTYPE LockRegion( 
			/* [in] */ uint64_t libOffset,
			/* [in] */ uint64_t cb,
			/* [in] */ uint32_t dwLockType) = 0;

			virtual FX_HRESULT FX_STDMETHODCALLTYPE UnlockRegion( 
			/* [in] */ uint64_t libOffset,
			/* [in] */ uint64_t cb,
			/* [in] */ uint32_t dwLockType) = 0;

			virtual FX_HRESULT FX_STDMETHODCALLTYPE Stat( 
			/* [out] */ STATSTG __RPC_FAR *pstatstg,
			/* [in] */ uint32_t grfStatFlag) = 0;

			virtual FX_HRESULT FX_STDMETHODCALLTYPE Clone( 
			/* [out] */ IStream __RPC_FAR *__RPC_FAR *ppstm) = 0;
	};
#else
	START_C_INTERFACE( IStream )
		/* [local] */ FX_HRESULT ( FX_STDMETHODCALLTYPE __RPC_FAR *Read )( 
		IStream __RPC_FAR * This,
		/* [length_is][size_is][out] */ void __RPC_FAR *pv,
		/* [in] */ uint32_t cb,
		/* [out] */ uint32_t __RPC_FAR *pcbRead);

		/* [local] */ FX_HRESULT ( FX_STDMETHODCALLTYPE __RPC_FAR *Write )( 
		IStream __RPC_FAR * This,
		/* [size_is][in] */ const void __RPC_FAR *pv,
		/* [in] */ uint32_t cb,
		/* [out] */ uint32_t __RPC_FAR *pcbWritten);

		/* [local] */ FX_HRESULT ( FX_STDMETHODCALLTYPE __RPC_FAR *Seek )( 
		IStream __RPC_FAR * This,
		/* [in] */ int64_t dlibMove,
		/* [in] */ uint32_t dwOrigin,
		/* [out] */ uint64_t __RPC_FAR *plibNewPosition);

		FX_HRESULT ( FX_STDMETHODCALLTYPE __RPC_FAR *SetSize )( 
		IStream __RPC_FAR * This,
		/* [in] */ uint64_t libNewSize);

		/* [local] */ FX_HRESULT ( FX_STDMETHODCALLTYPE __RPC_FAR *CopyTo )( 
		IStream __RPC_FAR * This,
		/* [unique][in] */ IStream __RPC_FAR *pstm,
		/* [in] */ uint64_t cb,
		/* [out] */ uint64_t __RPC_FAR *pcbRead,
		/* [out] */ uint64_t __RPC_FAR *pcbWritten);

		FX_HRESULT ( FX_STDMETHODCALLTYPE __RPC_FAR *Commit )( 
		IStream __RPC_FAR * This,
		/* [in] */ uint32_t grfCommitFlags);

		FX_HRESULT ( FX_STDMETHODCALLTYPE __RPC_FAR *Revert )( 
		IStream __RPC_FAR * This);

	FX_HRESULT ( FX_STDMETHODCALLTYPE __RPC_FAR *LockRegion )( 
		IStream __RPC_FAR * This,
		/* [in] */ uint64_t libOffset,
		/* [in] */ uint64_t cb,
		/* [in] */ uint32_t dwLockType);

		FX_HRESULT ( FX_STDMETHODCALLTYPE __RPC_FAR *UnlockRegion )( 
		IStream __RPC_FAR * This,
		/* [in] */ uint64_t libOffset,
		/* [in] */ uint64_t cb,
		/* [in] */ uint32_t dwLockType);

		FX_HRESULT ( FX_STDMETHODCALLTYPE __RPC_FAR *Stat )( 
		IStream __RPC_FAR * This,
		/* [out] */ STATSTG __RPC_FAR *pstatstg,
		/* [in] */ uint32_t grfStatFlag);

		FX_HRESULT ( FX_STDMETHODCALLTYPE __RPC_FAR *Clone )( 
		IStream __RPC_FAR * This,
		/* [out] */ IStream __RPC_FAR *__RPC_FAR *ppstm);

		END_C_INTERFACE( IStream )
#endif /* IStream */

#ifdef __cplusplus
		/* uuid {DF0B3D60-548F-101B-8E65-08002B2BD119} */
	class ISupportErrorInfo : public IUnknown  
	{
	public:
		static const firtex::com::FX_IID iid;

		virtual firtex::com::FX_HRESULT FX_STDMETHODCALLTYPE InterfaceSupportsErrorInfo( /* [in] */ FX_REFIID riid ) = 0;
	};
#else
		START_C_INTERFACE( ISupportErrorInfo )
		FX_HRESULT ( FX_STDMETHODCALLTYPE __RPC_FAR *InterfaceSupportsErrorInfo )(
		ISupportErrorInfo __RPC_FAR * This,
		/* [in] */ FX_REFIID riid );
		END_C_INTERFACE( ISupportErrorInfo )
#endif /* ISupportErrorInfo */

#ifdef __cplusplus
	}// namespace com 
}; // namespace firtex 
#endif

#endif /* _COMINTERFACES_H_ */

