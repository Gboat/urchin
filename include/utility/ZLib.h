#ifndef _ZLIB_H
#define  _ZLIB_H
#include "StdHeader.h"

namespace firtex
{
	namespace utility
	{
		class CZLib
		{
		public:
			CZLib(void);
			~CZLib(void);
		public:			
			/**
			 * compress data
			 * @param pInput the input data
			 * @param nInput the size of input data
			 * @param pnOutput the size of compressed data
			 * @param nSuggest the suggest size of compressed data
			 * @return the address of compressed data,pnOutput is it's size,must delete by user
			 */
			static byte* compress(const void* pInput,size_t nInput, size_t* pnOutput, size_t nSuggest = 0);
			/**
			 * decompress data
			 * @param pInput the input data
			 * @param nInput the size of input data
			 * @param pnOutput the size of decompressed data
			 * @param nSuggest the suggest size of decompressed data
			 * @return the address of decompressed data,pnOutput is it's size,must delete by user
			 */
			static byte* decompress(const void* pInput, size_t nInput, size_t* pnOutput, size_t nSuggest = 0);
		};
	}
}

#endif
