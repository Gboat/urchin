#include "com/linux/internal.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>

static wchar_t *fx_wcsdup( const wchar_t *src )
{
	size_t buffer_len = wcslen( src ) + 1;
	wchar_t* ret = reinterpret_cast<wchar_t*>( malloc( buffer_len * sizeof(wchar_t) )  );
	if ( !ret ) return ret;
	wcscpy( ret, src );
	return ret;
}


firtex::com::internal::zstr::zstr( char const* rhs )
{
	// check for null string
	if ( !rhs ) {
		s_ = 0;
		return;
	}

	// duplicate the string
	s_ = strdup( rhs );
	// caller must check for failure
}

firtex::com::internal::zstr::zstr( wchar_t const* rhs )
{
	// check for null string
	if ( !rhs ) {
		s_ = 0;
		return;
	}

	// get the necessary buffer size
	wchar_t const* tmp = rhs;
	size_t buffer_len = wcsrtombs( 0, &tmp, 0, 0 ) + 1;

	// allocate a buffer
	s_ = reinterpret_cast<char*>( malloc( buffer_len ) );
	// check if allocation failed
	// caller must check for failure
	if ( !s_ ) return;
	tmp = rhs;
	wcsrtombs( s_, &tmp, buffer_len, 0 ); 
}

firtex::com::internal::zstr::~zstr()
{
	free( s_ );
}

firtex::com::internal::zstr& firtex::com::internal::zstr::operator=( char const* rhs )
{
	zstr buffer( rhs );

	char* tmp = buffer.s_;
	buffer.s_ = s_;
	s_ = tmp;
	return *this;
}

firtex::com::internal::zstr& firtex::com::internal::zstr::operator=( wchar_t const* rhs )
{
	zstr buffer( rhs );

	char* tmp = buffer.s_;
	buffer.s_ = s_;
	s_ = tmp;
	return *this;
}

bool firtex::com::internal::zstr::append( char const* rhs )
{
	if ( !rhs ) return true;

	if ( !s_ ) {
		s_ = strdup( rhs );
		return !!s_;
	}

	// create the new buffer
	size_t buffer_len = strlen( s_ ) + strlen( rhs ) + 1;
	char* ptr = reinterpret_cast<char*>( malloc( buffer_len ) );
	if ( !ptr ) return false;
	// copy in the data
	strcpy( ptr, s_ );
	strcat( ptr, rhs );
	assert( strlen(ptr)+1 == buffer_len );
	// swap buffers
	free( s_ );
	s_ = ptr;
	return true;
}

bool firtex::com::internal::zstr::append( wchar_t const* rhs )
{
	if ( !rhs ) return true;

	if ( !s_ ) {
		// construct a zstr version of the wzstr
		zstr ascii( rhs );
		if ( !rhs ) return false;

		s_ = ascii.s_;
		ascii.s_ = 0;
		return true;
	}

	// create the new buffer
	wchar_t const* tmp = rhs;
	size_t buffer_len1 = strlen( s_ ); // necessary buffer for lhs
	size_t buffer_len2 = wcsrtombs( 0, &tmp, 0, 0 ); // necessary buffer for rhs
	size_t buffer_len = buffer_len1 + buffer_len2 + 1;
	char* ptr = reinterpret_cast<char*>( malloc( buffer_len ) );
	if ( !ptr ) return false;
	// copy in the data
	strcpy( ptr, s_ );
	tmp = rhs;
	wcsrtombs( ptr + buffer_len1, &tmp, buffer_len2+1, 0 ); 
	assert( strlen(ptr)+1 == buffer_len );
	// swap buffers
	free( s_ );
	s_ = ptr;
	return true;
}

firtex::com::internal::wzstr::wzstr( char const* rhs )
{
	if ( !rhs ) {
		s_ = 0;
		return;
	}

	// get the necessary buffer size
	char const* tmp = rhs;
	size_t buffer_len = mbsrtowcs( 0, &tmp, 0, 0 ) + 1;

	// allocate a buffer
	s_ = reinterpret_cast<wchar_t*>( malloc( buffer_len * sizeof(wchar_t) ) );
	// check if allocation failed
	// caller must check for failure
	if ( !s_ ) return;
	tmp = rhs;
	mbsrtowcs( s_, &tmp, buffer_len, 0 ); 
}

firtex::com::internal::wzstr::wzstr( wchar_t const* rhs )
{
	// check for null string
	if ( !rhs ) {
		s_ = 0;
		return;
	}

	// duplicate the string
	s_ = fx_wcsdup( rhs );
	// caller must check for failure
}

firtex::com::internal::wzstr::~wzstr()
{
	free( s_ );
}

firtex::com::internal::wzstr& firtex::com::internal::wzstr::operator=( char const* rhs )
{
	wzstr buffer( rhs );

	wchar_t* tmp = buffer.s_;
	buffer.s_ = s_;
	s_ = tmp;
	return *this;
}

firtex::com::internal::wzstr& firtex::com::internal::wzstr::operator=( wchar_t const* rhs )
{
	wzstr buffer( rhs );

	wchar_t* tmp = buffer.s_;
	buffer.s_ = s_;
	s_ = tmp;
	return *this;
}

