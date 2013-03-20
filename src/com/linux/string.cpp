#include "com/Com.h"
#include "com/comerror.h"
#include <assert.h>

static char get_byte( firtex::com::FX_REFGUID g, unsigned i )
{
	return reinterpret_cast<char const*>(&g)[i];
}

static char hexdigits[]="0123456789ABCDEF";

static char upper_hex_char( char i )
{
	return hexdigits[ (i >> 4) & 0xF ];
}

static char lower_hex_char( char i )
{
	return hexdigits[ i & 0xF ];
}

FX_WINOLEAPI_(int) firtex::com::FX_StringFromGUID2(firtex::com::FX_REFGUID rguid, wchar_t* lpsz, int cbMax)
{
	//The string that the lpsz parameter receives has a format like that of the following sample:
	//{c200e360-38c5-11ce-ae62-08002b2b79ef}

	// make sure we have been given a string
	assert( lpsz );
	assert( sizeof(rguid) == 16 );
	// check that the string is long enough
	if ( cbMax < 39 ) return 0;
	// write out the output
	*lpsz++ = '{';
	*lpsz++ = upper_hex_char( get_byte( rguid, 0 ) );
	*lpsz++ = lower_hex_char( get_byte( rguid, 0 ) );
	*lpsz++ = upper_hex_char( get_byte( rguid, 1 ) );
	*lpsz++ = lower_hex_char( get_byte( rguid, 1 ) );
	*lpsz++ = upper_hex_char( get_byte( rguid, 2 ) );
	*lpsz++ = lower_hex_char( get_byte( rguid, 2 ) );
	*lpsz++ = upper_hex_char( get_byte( rguid, 3 ) );
	*lpsz++ = lower_hex_char( get_byte( rguid, 3 ) );
	*lpsz++ = '-';
	*lpsz++ = upper_hex_char( get_byte( rguid, 4 ) );
	*lpsz++ = lower_hex_char( get_byte( rguid, 4 ) );
	*lpsz++ = upper_hex_char( get_byte( rguid, 5 ) );
	*lpsz++ = lower_hex_char( get_byte( rguid, 5 ) );
	*lpsz++ = '-';
	*lpsz++ = upper_hex_char( get_byte( rguid, 6 ) );
	*lpsz++ = lower_hex_char( get_byte( rguid, 6 ) );
	*lpsz++ = upper_hex_char( get_byte( rguid, 7 ) );
	*lpsz++ = lower_hex_char( get_byte( rguid, 7 ) );
	*lpsz++ = '-';
	*lpsz++ = upper_hex_char( get_byte( rguid, 8 ) );
	*lpsz++ = lower_hex_char( get_byte( rguid, 8 ) );
	*lpsz++ = upper_hex_char( get_byte( rguid, 9 ) );
	*lpsz++ = lower_hex_char( get_byte( rguid, 9 ) );
	*lpsz++ = '-';
	*lpsz++ = upper_hex_char( get_byte( rguid, 10 ) );
	*lpsz++ = lower_hex_char( get_byte( rguid, 10 ) );
	*lpsz++ = upper_hex_char( get_byte( rguid, 11 ) );
	*lpsz++ = lower_hex_char( get_byte( rguid, 11 ) );
	*lpsz++ = upper_hex_char( get_byte( rguid, 12 ) );
	*lpsz++ = lower_hex_char( get_byte( rguid, 12 ) );
	*lpsz++ = upper_hex_char( get_byte( rguid, 13 ) );
	*lpsz++ = lower_hex_char( get_byte( rguid, 13 ) );
	*lpsz++ = upper_hex_char( get_byte( rguid, 14 ) );
	*lpsz++ = lower_hex_char( get_byte( rguid, 14 ) );
	*lpsz++ = upper_hex_char( get_byte( rguid, 15 ) );
	*lpsz++ = lower_hex_char( get_byte( rguid, 15 ) );
	*lpsz++ = '}';
	*lpsz++ = '\0';

	return 39;
}

firtex::com::FX_HRESULT firtex::com::FX_CLSIDFromString(
	wchar_t const* s,	        /* [in] string representation of guid */
	FX_CLSID *id)		/* [out] FX_GUID converted from string */
{
	uint8_t table[256];

  	if ( !s ) {
	 	s = L"{00000000-0000-0000-0000-000000000000}";
	}
  	else {  
  		// validate the string
  		if ( wcslen(s) != 38 ) return FX_CO_E_CLASSSTRING;
      		if ( s[0]!='{' || s[9]!='-' || s[14]!='-' || s[19]!='-' || s[24]!='-' || s[37]!='}' )
          		return FX_CO_E_CLASSSTRING;

      		for ( int i=1; i<37; i++) {
          	if ((i == 9)||(i == 14)||(i == 19)||(i == 24)) continue;
          	if ( !(( s[i] >= '0' && s[i] <= '9')  ||
                	(s[i] >= 'a' && s[i] <= 'f')  ||
                	(s[i] >= 'A' && s[i] <= 'F')))
              		return FX_CO_E_CLASSSTRING;
              	}
      	}

  	// quick lookup table
	memset(table, 0, 256);


	int i;
  	for (i = 0; i < 10; i++)
    		table['0' + i] = i;
	for (i = 0; i < 6; i++) {
    		table['A' + i] = i+10;
    		table['a' + i] = i+10;
  	}

  	// in form {XXXXXXXX-XXXX-XXXX-XXXX-XXXXXXXXXXXX}

  id->Data1 = (table[s[1]] << 28 | table[s[2]] << 24 | table[s[3]] << 20 | table[s[4]] << 16 |
               table[s[5]] << 12 | table[s[6]] << 8  | table[s[7]] << 4  | table[s[8]]);
  id->Data2 = table[s[10]] << 12 | table[s[11]] << 8 | table[s[12]] << 4 | table[s[13]];
  id->Data3 = table[s[15]] << 12 | table[s[16]] << 8 | table[s[17]] << 4 | table[s[18]];

  /* these are just sequential bytes */
  id->Data4[0] = table[s[20]] << 4 | table[s[21]];
  id->Data4[1] = table[s[22]] << 4 | table[s[23]];
  id->Data4[2] = table[s[25]] << 4 | table[s[26]];
  id->Data4[3] = table[s[27]] << 4 | table[s[28]];
  id->Data4[4] = table[s[29]] << 4 | table[s[30]];
  id->Data4[5] = table[s[31]] << 4 | table[s[32]];
  id->Data4[6] = table[s[33]] << 4 | table[s[34]];
  id->Data4[7] = table[s[35]] << 4 | table[s[36]];

  return FX_S_OK;
}
