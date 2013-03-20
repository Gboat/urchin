#include "com/Com.h"
#include <assert.h>
#include <stdlib.h>
#include <wchar.h>

#define BSTR_TO_PTR(b) (reinterpret_cast<size_t*>(b) - 1)
#define BSTR_TO_CPTR(b) (reinterpret_cast<size_t const*>(b) - 1)
#define PTR_TO_BSTR(p) reinterpret_cast<wchar_t*>( reinterpret_cast<size_t*>(p)+1 );

FX_WINOLEAUTAPI_(firtex::com::FX_BSTR) firtex::com::FX_SysAllocString(const wchar_t * sz)
{
	// check for null string
	if ( !sz ) return 0;

	// first, count length
	const wchar_t* szend = sz;
	while( *szend ) ++szend;

	// delegate the rest of the responsibility
	return FX_SysAllocStringLen( sz, szend-sz );
}

//int16_t	firtex::com::SysReAllocString(FX_BSTR *, const wchar_t *);

FX_WINOLEAUTAPI_(firtex::com::FX_BSTR) firtex::com::FX_SysAllocStringLen(const wchar_t * pch, uint16_t cch)
{
	// allocate a new string
	void* ret = malloc( (cch+1) * sizeof(*pch) + sizeof(size_t) );
	if ( !ret ) return 0;

	// copy in data
	// length of string minus the null character
	*reinterpret_cast<size_t*>(ret) = cch;
	// copy in the text, including the null
	wchar_t* bstr = PTR_TO_BSTR(ret)
	if ( pch ) {
		memcpy( bstr, pch, cch * sizeof( *pch ) );
		bstr[cch] = 0;
	}
	else {
		memset( bstr, 0, (cch+1) * sizeof( *bstr ) );
	}
	// and we are done
	assert( bstr );
	assert( FX_SysStringLen(bstr) == cch );
	assert( bstr[ FX_SysStringLen(bstr) ] == 0 );
	return bstr;
}

int16_t	firtex::com::FX_SysReAllocStringLen(FX_BSTR * old, const wchar_t * str, uint16_t len)
{
    if ( *old ) {
		uint32_t newbytelen = (len+1) * sizeof(wchar_t);
		void* ptr = realloc( BSTR_TO_PTR(const_cast<wchar_t*>(*old)), newbytelen + sizeof(size_t) );
		if ( !ptr ) return 0;

		*old = PTR_TO_BSTR(ptr);
		*reinterpret_cast<size_t*>(ptr) = len;

		assert( *old );
		assert( (*old)[ FX_SysStringLen(*old) ] == 0 );


		if (str) {
			memcpy( const_cast<wchar_t*>(*old), str, len * sizeof(wchar_t) );
			const_cast<wchar_t*>(*old)[len] = 0;
		} 
		else {
			/* Subtle hidden feature: The old string data is still there
			* when 'in' is NULL!
			* Some Microsoft program needs it.
			*/
		}
    } 
	else {
		// allocate a new string
		*old = FX_SysAllocStringLen(str, len);
    }

    return 1;
}

void	firtex::com::FX_SysFreeString(FX_BSTR bstr)
{
	// check for null pointer
	if ( !bstr ) return;
	// extract original pointer
	size_t const* ptr = BSTR_TO_CPTR(bstr);
	free( const_cast<size_t*>(ptr) );
}

FX_WINOLEAUTAPI_(uint16_t) firtex::com::FX_SysStringLen(firtex::com::FX_BSTR bstr)
{
	// check for null pointer
	if ( !bstr ) return 0;
	// extract original pointer
	size_t const* ptr = BSTR_TO_CPTR(bstr);
	return *ptr;
}

FX_WINOLEAUTAPI_(uint16_t) firtex::com::FX_SysStringByteLen(firtex::com::FX_BSTR bstr)
{
	// extract original pointer
	size_t const* ptr = BSTR_TO_CPTR(bstr);
	return *ptr * sizeof(wchar_t);
}

//FX_BSTR	firtex::com::SysAllocStringByteLen(const char* psz, uint16_t len);
