/*************************************************************************
// Copyright IBM Corp. 2023
//
// Licensed under the Apache License 2.0 (the "License").  You may not use
// this file except in compliance with the License.  You can obtain a copy
// in the file LICENSE in the source distribution.
*************************************************************************/

/*************************************************************************
// Description: Data tables for SP800-90 Hash structures and self test
//
*************************************************************************/

#include "icclib.h"
#include "SP800-90.h"
#include "SP800-90i.h"
#include "ds.h"
#include "utils.h"




static const unsigned char C00[1] = {0x00};
static const unsigned char C01[1] = {0x01};
static const unsigned char C02[1] = {0x02};
static const unsigned char C03[1] = {0x03};

/*! 
   Hash based PRNG's 
*/


/*!
  @brief SP800-90 Hash derivation function
  @param pctx a PRNG context
  @param in the input buffer
  @param out the output buffer
  @param outl the desired number of output bytes
  @note Be careful not to pass in a pointer to an object
        that's also the output here.
*/
static void Hash_df(SP800_90PRNG_Data_t *pctx,DS *in, unsigned char *out, unsigned int outl)
{
  int j;
  unsigned char counter = 1;

  unsigned char outbits[4]; 
  unsigned int digestL = 0;
  unsigned char *ptr = NULL;
  unsigned int n = 0;

  uint2BS(outl*8,outbits);

  /* Note the ordering, 
     result must be (counter || no_of_bits || input )
     Input is already in the input stack so we insert no_of_bits
     then insert the counter in front of that
  */
  DS_Insert(in,4,outbits);
  DS_Insert(in,1,&counter);
  while(outl > 0) {
    if( 1 != EVP_DigestInit(pctx->ctx.md_ctx,pctx->alg.md) ) {
      pctx->error_reason = ERRAT("Digest Init failed");
      pctx->state = SP800_90ERROR;
      EVP_MD_CTX_reset(pctx->ctx.md_ctx);
      return;
    }
    DS_Reset(in);
    while(in->total) {
      DS_Extract(in,&n,&ptr);
      if( 1 != EVP_DigestUpdate(pctx->ctx.md_ctx,ptr,n) ) {
	pctx->error_reason = ERRAT("Digest Init failed");
	pctx->state = SP800_90ERROR;
	EVP_MD_CTX_reset(pctx->ctx.md_ctx);
	return;
      }
    } 
    if( 1 != EVP_DigestFinal(pctx->ctx.md_ctx,pctx->T, &digestL) ) {
     	pctx->error_reason = ERRAT("Digest Final failed");
	pctx->state = SP800_90ERROR;
	EVP_MD_CTX_reset(pctx->ctx.md_ctx);
	return; 
    }
    EVP_MD_CTX_reset(pctx->ctx.md_ctx);
    j = (outl > digestL) ? digestL : outl;
    memcpy(out,pctx->T,j);
    out += j;
    outl -= j;
    counter ++;
  }
  /* Debugging aid as much as anything, 
     T should be zero if not being used 
  */
  memset(pctx->T,0,digestL);  
  return;
}

/*!
  @brief Instantiate function for HASH based PRNG's
  @param ctx a partially initialized PRNG context
  @param ein a pointer to the entropy input buffer. (May be NULL)
  @param einl a pointer to the length of the provided entropy
  @param nonce additional entropy
  @param nonl a pointer to the length of the provided entropy
  @param person a pointer to the personalization data
  @param perl a pointer to the length of the personalization data
 */
static SP800_90STATE SHA_Instantiate(PRNG_CTX *ctx,
			      unsigned char *ein, unsigned int einl,
			      unsigned char *nonce, unsigned int nonl,
			      unsigned char *person,unsigned int perl)
{
  DS seedDS;
  SP800_90PRNG_Data_t *pctx = (SP800_90PRNG_Data_t *)ctx;

  memset(pctx->V,0,pctx->prng->seedlen);
  pctx->alg.md = EVP_get_digestbyname(pctx->prng->specific);
  if( NULL == pctx->alg.md) {
    pctx->error_reason = ERRAT("Could not obtain digest");
    pctx->state = SP800_90ERROR;
    return pctx->state;
  }
  if(NULL == pctx->ctx.md_ctx) {
    pctx->ctx.md_ctx = EVP_MD_CTX_new();
  }
  DS_Init(&seedDS);
  DS_Append(&seedDS,einl,ein);
  DS_Append(&seedDS,nonl,nonce);
  DS_Append(&seedDS,perl,person);
  Hash_df(pctx,&seedDS,pctx->V,pctx->prng->seedlen);

  DS_Init(&seedDS);
  DS_Append(&seedDS,1,C00);
  DS_Append(&seedDS,pctx->prng->seedlen,pctx->V);
  Hash_df(pctx,&seedDS,pctx->C,pctx->prng->seedlen);


  return pctx->state;
}
/*!
  @brief ReSeed function for HASH based PRNG's
  @param ctx an initialized PRNG context
  @param ein a pointer to the entropy input buffer. (May be NULL)
  @param einl a pointer to the length of the provided entropy
  @param adata additional user provided data
  @param adatal a pointer to the length of the provided data

*/
static SP800_90STATE SHA_ReSeed(PRNG_CTX *ctx,
			 unsigned char *ein, unsigned int einl,
			 unsigned char *adata,unsigned int adatal)
{
  DS seedDS;
  SP800_90PRNG_Data_t *pctx = (SP800_90PRNG_Data_t *)ctx;

  /* Update V, but note V is input, can't be output as well 
     so we'll use C as a scratch location
     C is recreated from the new V in the next step
     
     V = Hash_DF( 01 || V || ein || adata) 
  */ 
  DS_Init(&seedDS);
  DS_Append(&seedDS,1,C01);
  DS_Append(&seedDS,pctx->prng->seedlen,pctx->V);
  DS_Append(&seedDS,einl,ein);
  DS_Append(&seedDS,adatal,adata);
  Hash_df(pctx,&seedDS,pctx->C,pctx->prng->seedlen);

  /* copy =>  V */
  memcpy(pctx->V,pctx->C,pctx->prng->seedlen);

  /* Now generate the new C 
     C = Hash_DF(00 || V)
  */
  DS_Init(&seedDS);
  DS_Append(&seedDS,1,C00);
  DS_Append(&seedDS,pctx->prng->seedlen,pctx->V);
  Hash_df(pctx,&seedDS,pctx->C,pctx->prng->seedlen);

  return pctx->state;
 
}

/*!
  @brief Generate function for HASH based PRNG's
  @param ctx an initialized PRNG context
  @param buffer a pointer to the PRNG data destination
  @param blen Number of bytes of PRNG data to supply
  @param adata additional user provided data
  @param adatal a pointer to the length of the provided data

*/

static SP800_90STATE SHA_Generate(PRNG_CTX *ctx,
			   unsigned char *buffer,unsigned blen,
			   unsigned char *adata,unsigned adatal)
{
 
  SP800_90PRNG_Data_t *pctx = (SP800_90PRNG_Data_t *)ctx;
  unsigned int l = 0;
  int j = 0;

  /* If additional input != NULL ... */
  if( (NULL != adata) && (0 != adatal)) {
    /* w = Hash( */

    if( 1 != EVP_DigestInit(pctx->ctx.md_ctx,pctx->alg.md) ) {
      pctx->error_reason = ERRAT("Digest Init failed");
      pctx->state = SP800_90ERROR;
      EVP_MD_CTX_reset(pctx->ctx.md_ctx);
      return pctx->state;
    }
    /* 0x02 || */
    if( 1 != EVP_DigestUpdate(pctx->ctx.md_ctx,C02,1) ) {
      pctx->error_reason = ERRAT("Digest Update failed");
      pctx->state = SP800_90ERROR;
      EVP_MD_CTX_reset(pctx->ctx.md_ctx);
      return pctx->state;
    }
    /* V || */
    if( 1 != EVP_DigestUpdate(pctx->ctx.md_ctx,pctx->V,pctx->prng->seedlen) ) {
      pctx->error_reason = ERRAT("Digest Update failed");
      pctx->state = SP800_90ERROR;
      EVP_MD_CTX_reset(pctx->ctx.md_ctx);
      return pctx->state;
    }

    /* additional_input ) */
    if( 1 != EVP_DigestUpdate(pctx->ctx.md_ctx,adata,adatal) ) {
      pctx->error_reason = ERRAT("Digest Update failed");
      pctx->state = SP800_90ERROR;
      EVP_MD_CTX_reset(pctx->ctx.md_ctx);
      return pctx->state;
    } 
    if( 1 != EVP_DigestFinal(pctx->ctx.md_ctx,pctx->T,&l)  ) {
      pctx->error_reason = ERRAT("Digest Final failed");
      pctx->state = SP800_90ERROR;
      EVP_MD_CTX_reset(pctx->ctx.md_ctx);
      return pctx->state;
    }
    EVP_MD_CTX_reset(pctx->ctx.md_ctx);
   /* V = V +w (mod 2^ Seedlen) */
    /* w is tail aligned */
    Add(pctx->V,pctx->V,pctx->prng->seedlen,pctx->T,pctx->prng->OBL);

  }
	 
  /* Returned bits = Hashgen(requested,V) */

  memcpy(pctx->T,pctx->V,pctx->prng->seedlen);
	 
  while(blen > 0) {
    if( 1 != EVP_DigestInit(pctx->ctx.md_ctx,pctx->alg.md) ) {
      pctx->error_reason = ERRAT("Digest Init failed");
      pctx->state = SP800_90ERROR;
      EVP_MD_CTX_reset(pctx->ctx.md_ctx);
      return pctx->state;
    }
    if( 1 != EVP_DigestUpdate(pctx->ctx.md_ctx,pctx->T,pctx->prng->seedlen) ) {
      pctx->error_reason = ERRAT("Digest Update failed");
      pctx->state = SP800_90ERROR;
      EVP_MD_CTX_reset(pctx->ctx.md_ctx);
      return pctx->state;
    }
    /* Can't just hash directly into buffer, it may not have enough room
       for the hash result
    */
    if( 1 != EVP_DigestFinal(pctx->ctx.md_ctx,pctx->eBuf,&l) )  {
      pctx->error_reason = ERRAT("Digest Final failed");
      pctx->state = SP800_90ERROR;
      EVP_MD_CTX_reset(pctx->ctx.md_ctx);
      return pctx->state;
    }
    EVP_MD_CTX_reset(pctx->ctx.md_ctx);
    /* data = data +1 */
    Add(pctx->T,pctx->T,pctx->prng->seedlen,(unsigned char *)C01,1);
    j = (blen > l) ? l: blen;
    memcpy(buffer,pctx->eBuf,j);
    buffer += j;
    blen -= j;
  }
  /* create H in pctx->T 
     H = Hash(0x03 || V )
  */
  memset(pctx->T,0,pctx->prng->seedlen);

  if( 1 != EVP_DigestInit(pctx->ctx.md_ctx,pctx->alg.md) ) {
    pctx->error_reason = ERRAT("Digest Init failed");
    pctx->state = SP800_90ERROR;
    EVP_MD_CTX_reset(pctx->ctx.md_ctx);
    return pctx->state;
  }

  if( 1 != EVP_DigestUpdate(pctx->ctx.md_ctx,C03,1) ) {
    pctx->error_reason = ERRAT("Digest Update failed");
    pctx->state = SP800_90ERROR;
    EVP_MD_CTX_reset(pctx->ctx.md_ctx);
    return pctx->state;
  }
  if( 1 != EVP_DigestUpdate(pctx->ctx.md_ctx,pctx->V,pctx->prng->seedlen) ) {
    pctx->error_reason = ERRAT("Digest Update failed");
    pctx->state = SP800_90ERROR;
    EVP_MD_CTX_reset(pctx->ctx.md_ctx);
    return pctx->state;
  }
  if( 1 != EVP_DigestFinal(pctx->ctx.md_ctx,pctx->T,&l) ) {
      pctx->error_reason = ERRAT("Digest Final failed");
      pctx->state = SP800_90ERROR;
      EVP_MD_CTX_reset(pctx->ctx.md_ctx);
      return pctx->state;
  }
  EVP_MD_CTX_reset(pctx->ctx.md_ctx);
  /* V = V + H  ... */

  /* H is tail aligned*/
  Add(pctx->V,pctx->V,pctx->prng->seedlen,pctx->T,pctx->prng->OBL);

  /* V = V + H + C ... , no probs here, V & C are both seedlen long */
  Add(pctx->V,pctx->V,pctx->prng->seedlen,pctx->C,0);
  memset(pctx->T,0,pctx->prng->seedlen);
  /* V = V + H + C + reeseed_counter */
  Add(pctx->V,pctx->V,pctx->prng->seedlen,pctx->CallCount.c,4);
	
  return pctx->state;
}
/*!
  @brief Cleanup function for HASH based PRNG's
  @param ctx The PRNG_CTX to cleanup
  All allocated data is released. (Only the EVP_MD_CTX)

*/
static SP800_90STATE SHA_Cleanup(PRNG_CTX *ctx)
{
  SP800_90PRNG_Data_t *pctx = (SP800_90PRNG_Data_t *)ctx;

  if(NULL != pctx->ctx.md_ctx) {
    EVP_MD_CTX_free(pctx->ctx.md_ctx);
    pctx->ctx.md_ctx = NULL;
  }
  return pctx->state; 
}

/*!
  Representation of no input data for PRNG self tests
*/
static const StringBuf NONE =
  {0,0,
   {
     0x00
   }
  };

static const StringBuf SHA512_112IntEin = 
  {0,24,
    {
      0xd9,0x56,0xca,0xa2,0x40,0x39,0xe7,0x6f,
      0x58,0x61,0x6e,0x09,0x69,0xaf,0xa2,0xd7,
      0xb7,0x08,0x74,0x01,0xee,0x2d,0x87,0x77
    }
};
static const StringBuf SHA512_112IntNon =
  {0,12, 
   {
     0x32,0xa2,0xef,0x15,0x98,0x3e,0x3c,0x1f,
     0x66,0xe6,0x03,0x2a
   }
  };
static const StringBuf SHA512_112GenEin = 
  {0,24,
   {
     0x7b,0xa5,0xa5,0x22,0x58,0x0b,0x41,0xe1,
     0xa4,0xf5,0x40,0xf9,0xfe,0x3d,0xaa,0xf9,
     0x5d,0xf7,0x72,0x74,0x0a,0x19,0x96,0x51
   }
  };
static const StringBuf SHA512_112Result =
  {0,64, 
   {
     0x91,0x73,0xFF,0x1B,0x3B,0xD0,0x42,0x11,
     0xF0,0x90,0xC0,0xC6,0x58,0xCD,0x9C,0xA9,
     0x8A,0xC1,0xD7,0x7E,0x1E,0x3A,0x96,0xD1,
     0x1D,0xE6,0x72,0xD1,0xEC,0xF0,0xE3,0x70,
     0x8C,0x7E,0x42,0x42,0xF9,0x40,0xDF,0x4E,
     0x5B,0x34,0x52,0x96,0x72,0x04,0x4A,0x10,
     0x9A,0xB8,0xF1,0xDB,0xEB,0x6A,0xBB,0x39,
     0x30,0x69,0x0A,0x92,0x28,0xD0,0xE5,0x7C
   }
  };

/*! \known SHA-512 PRNG, 128 bit strength 

     */
static const StringBuf SHA512_128IntEin =
  {0,32,
   {
     0x7a,0xd0,0x8c,0x07,0xe2,0x6f,0xf3,0xff,
     0xd5,0x00,0x1b,0x94,0x82,0xad,0x71,0x5d,
     0xb3,0xc5,0xff,0x11,0x24,0x42,0xed,0xb2,
     0x25,0x9a,0x3a,0xfd,0x72,0xc9,0xb5,0x10
   }
  };
static const StringBuf SHA512_128IntNon =
  {0,16,
   {
     0x04,0xb1,0x90,0x55,0x10,0x69,0xf0,0x4e,
     0xe6,0x63,0x2b,0x76,0xda,0x26,0xa3,0xd0 
   }
  };
static const StringBuf SHA512_128GenEin =
  {0,32,
   {
     0x3b,0xe4,0xbf,0xaa,0x70,0xa9,0x2f,0x0a,
     0xab,0x37,0xc2,0xe6,0xab,0x89,0xf6,0x25,
     0xc9,0xbc,0xe6,0xcd,0x54,0x9b,0xc9,0x32,
     0x29,0x6c,0xad,0x60,0x95,0xfc,0x73,0xc5
   }
  };
static const StringBuf SHA512_128Result =
  {0,64,
   {
     0x06,0xC0,0xAA,0x2F,0x10,0x13,0x8F,0x79,
     0x64,0xC4,0x84,0x23,0x55,0xAD,0xDF,0x7E,
     0xD2,0x9A,0x8E,0xCC,0x6B,0x52,0x0A,0xE6,
     0x1A,0x76,0x37,0xCD,0x07,0x69,0xE2,0x2E,
     0x3C,0x9B,0xA0,0xC4,0xE2,0x4B,0xD5,0xBD,
     0x66,0x06,0x34,0x05,0xE5,0x9D,0xB7,0xC1,
     0x87,0x03,0x69,0xD1,0xEC,0xB2,0x7D,0x08,
     0x8C,0x00,0xF7,0x6B,0x1C,0x8B,0x2A,0x1D
   }
  };
/*! \known SHA-512 192 bit strength
  INSTANTIATE Hash_DRBG SHA-512 with PREDICTION RESISTANCE ENABLED
*/
static const StringBuf SHA512_192IntEin = 
  {0,32,
     {
       0x7a,0xd0,0x8c,0x07,0xe2,0x6f,0xf3,0xff,
       0xd5,0x00,0x1b,0x94,0x82,0xad,0x71,0x5d,
       0xb3,0xc5,0xff,0x11,0x24,0x42,0xed,0xb2,
       0x25,0x9a,0x3a,0xfd,0x72,0xc9,0xb5,0x10
     }
    };
static const StringBuf SHA512_192IntNon = 
  {0,16,
     {
       0x04,0xb1,0x90,0x55,0x10,0x69,0xf0,0x4e,
       0xe6,0x63,0x2b,0x76,0xda,0x26,0xa3,0xd0
     }
    };
static const StringBuf SHA512_192GenEin =
  {0,32,
     {
       0x3b,0xe4,0xbf,0xaa,0x70,0xa9,0x2f,0x0a,
       0xab,0x37,0xc2,0xe6,0xab,0x89,0xf6,0x25,
       0xc9,0xbc,0xe6,0xcd,0x54,0x9b,0xc9,0x32,
       0x29,0x6c,0xad,0x60,0x95,0xfc,0x73,0xc5
     }
    };
static const StringBuf SHA512_192Result =
    {0,64,
     { 
       0x06,0xc0,0xaa,0x2f,0x10,0x13,0x8f,0x79,
       0x64,0xc4,0x84,0x23,0x55,0xad,0xdf,0x7e,
       0xd2,0x9a,0x8e,0xcc,0x6b,0x52,0x0a,0xe6,
       0x1a,0x76,0x37,0xcd,0x07,0x69,0xe2,0x2e,
       0x3c,0x9b,0xa0,0xc4,0xe2,0x4b,0xd5,0xbd,
       0x66,0x06,0x34,0x05,0xe5,0x9d,0xb7,0xc1,
       0x87,0x03,0x69,0xd1,0xec,0xb2,0x7d,0x08,
       0x8c,0x00,0xf7,0x6b,0x1c,0x8b,0x2a,0x1d
     }
    };

/*! \known SHA-512 256 bit strength
  INSTANTIATE Hash_DRBG SHA-512 with NO PREDICTION RESISTANCE
*/
static const StringBuf SHA512_256IntEin = 
  {0,32,
     {
       0xee,0x41,0x94,0x2a,0x7f,0xff,0xfe,0xc7,
       0x3c,0xf6,0x5a,0x2f,0xad,0xb5,0x72,0xad,
       0x88,0xb8,0x17,0x8d,0x2e,0x9b,0xbb,0xe3,
       0x6a,0x7f,0x4f,0x99,0x67,0xbb,0x59,0xbd
     }
    };
StringBuf SHA512_256IntNon = 
  {0,16,
     {
       0x93,0xd0,0xca,0xac,0x1f,0x57,0xe7,0x9f,
       0x3a,0x95,0xb3,0xd0,0x89,0xe2,0x8d,0x84
     }
    };

static const StringBuf SHA512_256IntPer =
  {0,32,
     {
       0x46,0x1a,0x63,0x07,0xa1,0x95,0x71,0x5a,
       0x45,0x89,0x0a,0x44,0x91,0x52,0xca,0x8a,
       0x29,0x88,0x8e,0x88,0x7f,0x81,0x9f,0xcc,
       0x9e,0x08,0x1e,0xf0,0x38,0x5d,0xb0,0xb6
     }
    };

/* NIST result, truncated from 7168 bits */
static const StringBuf SHA512_256Result =
  {0,64,
     { 
       0xE7,0xFF,0xC2,0x0B,0x33,0x84,0xEE,0xBD,
       0x83,0xBA,0x03,0x90,0xE8,0x38,0x62,0xF7,
       0x7B,0xCC,0xB4,0x55,0xA6,0x78,0xAD,0x9C,
       0xA2,0x7C,0xD3,0xEB,0xE5,0x75,0x27,0x89,
       0x03,0x15,0xAA,0x2F,0x4A,0x1C,0x52,0xD5,
       0xEB,0xFB,0x7F,0xE2,0x63,0x4E,0xDC,0xE3,
       0xFC,0xD7,0x33,0xD2,0x88,0xE4,0x76,0x04,
       0x5B,0x9A,0xC8,0x4B,0x64,0x15,0xF0,0x8D
     }
    };


/*!
  Control structures for each PRNG type.
  Note that NIST specifies that there has to be a self test 
  at "some specified interval", so now and then i.e. at each 
  (SELF_TEST_AT) you may be slow getting a new PRNG context.
  When we self test, we'll do so for each strength, the overhead
  isn't great compared with the complexity of tracking the
  individual PRNG allowed strengths/instantiations
*/

/*!
  \FIPS Data structure defining the capabilities and limits
  of the SHA512 PRNG
*/

SP800_90PRNG_t sha512PRNG = {
  SP800_SHA512, /*!< type */
  888/8,        /*!< Retained seedlen */ 
  (1<<27),      /*!< Max nonce */
  (1<<27),      /*!< Max personalization data */ 
  (1<<27),      /*!< Max AAD */
  (1<<11),      /*!< Max bytes/request */
  0x00FFFFFFL,  /*!< Max calls between reseeds (< standard specifies) */
  512/8,        /*!< Block size */
  (1<<27),      /*!< Max allowed entropy input */   
  {112,
   128,
   192,
   256
  },            /*!< Supported security strengths */
  "SHA512",
  "SHA512",     /*!< Algorithm "name" */
  1,              /*!< Has a derivation function */
  Inst,
  Res,
  Gen,
  Cln,
  SHA_Instantiate, /*!< Instantiate method */
  SHA_ReSeed,      /*!< ReSeed method */
  SHA_Generate,    /*!< Generate method */
  SHA_Cleanup,     /*!< Cleanup method */
  SP800_IS_FIPS,  /*!< Are we a FIPS approved mode (do we pass the NIST tests) */
  SELF_TEST_AT,    /*!< health check interval */
  0,               /*!< Health check counter  */
  { 
    {
      &SHA512_112IntEin,
      &SHA512_112IntNon,
      &NONE,
      &NONE,
      &SHA512_112GenEin,
      &SHA512_112Result
    },
    {
      &SHA512_128IntEin,
      &SHA512_128IntNon,
      &NONE,
      &NONE,
      &SHA512_128GenEin,
      &SHA512_128Result
    },
    {
      &SHA512_192IntEin,
      &SHA512_192IntNon,
      &NONE,
      &NONE,
      &SHA512_192GenEin,
      &SHA512_192Result
    },
    {
      &SHA512_256IntEin,
      &SHA512_256IntNon,
      &SHA512_256IntPer,
      &NONE,
      &NONE,
      &SHA512_256Result
    },
  },
};

/*! \known SHA-384 PRNG known answer test data at 112 bit strength
*/
static const StringBuf SHA384_112IntEin = 
  {0,24,
   {0xd9,0x56,0xca,0xa2,0x40,0x39,0xe7,0x6f,
    0x58,0x61,0x6e,0x09,0x69,0xaf,0xa2,0xd7,
    0xb7,0x08,0x74,0x01,0xee,0x2d,0x87,0x77
   }
  };
static const StringBuf SHA384_112IntNon = 
  {0,16,
   {0x32,0xa2,0xef,0x15,0x98,0x3e,0x3c,0x1f,
    0x66,0xe6,0x03,0x2a,0x00,0x01,0x02,0x03
   }
  };
static const StringBuf SHA384_112GenEin =
  {0,24,
   {
     0x7b,0xa5,0xa5,0x22,0x58,0x0b,0x41,0xe1,
     0xa4,0xf5,0x40,0xf9,0xfe,0x3d,0xaa,0xf9,
     0x5d,0xf7,0x72,0x74,0x0a,0x19,0x96,0x51
   }
  };
static const StringBuf SHA384_112Result =
  {0,48,
   {
     0xE4,0xB6,0xBA,0x78,0x86,0x77,0x24,0x62,
     0x99,0xE9,0xF5,0x0F,0x3D,0x4A,0xC0,0x82,
     0xD9,0xFE,0x98,0x28,0x64,0x0A,0xF7,0x65,
     0x2E,0x9B,0x4C,0x04,0x45,0xA4,0x06,0x55,
     0x35,0xDF,0xFF,0xF6,0x3B,0x34,0xC9,0xAF,
     0x29,0xD1,0x50,0xE9,0x76,0x5B,0xC1,0xB8
   }
  };
/*! 
  \known SHA-384 PRNG, 128 bit strength 
*/
static const StringBuf SHA384_128IntEin =
  {0,32,
   {
     0x5d,0xae,0xbf,0x2d,0x31,0x79,0x35,0xa3,
     0x5c,0xba,0xfe,0xba,0x69,0xfd,0xe5,0x67,
     0x2a,0xb8,0x14,0xd7,0xb7,0xf1,0x2c,0xb9,
     0xa4,0x32,0xa6,0xcb,0x84,0xb1,0x6b,0xa4
    }
  };
static const StringBuf SHA384_128IntNon =
  {0,16,
   {
     0x85,0x84,0xde,0x95,0x9a,0x4b,0xc1,0xfd,
     0x6d,0x56,0x0d,0x6d,0x30,0xa5,0xbc,0x21
   }
  };
static const StringBuf SHA384_128GenEin =
  {0,32,
   {
     0x7b,0xc1,0x29,0x2c,0x59,0x60,0xd8,0x10,
     0x8a,0x03,0xd1,0xc3,0x29,0x9a,0xa9,0xe3,
     0x67,0x11,0xf6,0x0c,0x74,0xa7,0xdf,0x5a,
     0xae,0x8d,0xe3,0x1a,0x0a,0xc2,0xe7,0xd1
   }
  };
static const StringBuf SHA384_128Result =
  {0,48,
   {
     0xD3,0xC8,0xCE,0xD1,0x3E,0xDE,0xD3,0xA8,
     0xD9,0x14,0x23,0x0B,0xBF,0x15,0xC9,0x5A,
     0xB0,0x67,0x60,0x0C,0x17,0x5A,0x02,0x46,
     0xFC,0x6C,0x22,0x13,0xCF,0xDC,0x96,0x84,
     0x24,0x83,0xFD,0x39,0x5D,0x3B,0x18,0x7D,
     0x96,0x2B,0xA1,0x39,0x9E,0x0C,0xA3,0x8C
   }
  };

/* \known SHA-384 PRNG test vector, 192 bit security strength 
   INSTANTIATE Hash_DRBG SHA-384 with PREDICTION RESISTANCE ENABLED
*/

static const StringBuf SHA384_192IntEin =
  {0,32,
   {
     0x5d,0xae,0xbf,0x2d,0x31,0x79,0x35,0xa3,
     0x5c,0xba,0xfe,0xba,0x69,0xfd,0xe5,0x67,
     0x2a,0xb8,0x14,0xd7,0xb7,0xf1,0x2c,0xb9,
     0xa4,0x32,0xa6,0xcb,0x84,0xb1,0x6b,0xa4
   }
  };
static const StringBuf SHA384_192IntNon = 
  {0,16,
   {
     0x85,0x84,0xde,0x95,0x9a,0x4b,0xc1,0xfd,
     0x6d,0x56,0x0d,0x6d,0x30,0xa5,0xbc,0x21
   }
  };
static const StringBuf SHA384_192GenEin =
  {0,32,
     {
       0x7b,0xc1,0x29,0x2c,0x59,0x60,0xd8,0x10,
       0x8a,0x03,0xd1,0xc3,0x29,0x9a,0xa9,0xe3,
       0x67,0x11,0xf6,0x0c,0x74,0xa7,0xdf,0x5a,
       0xae,0x8d,0xe3,0x1a,0x0a,0xc2,0xe7,0xd1
     }
    };
static const StringBuf SHA384_192Result =
    {0,48,
     { 
       0xd3,0xc8,0xce,0xd1,0x3e,0xde,0xd3,0xa8,
       0xd9,0x14,0x23,0x0b,0xbf,0x15,0xc9,0x5a,
       0xb0,0x67,0x60,0x0c,0x17,0x5a,0x02,0x46,
       0xfc,0x6c,0x22,0x13,0xcf,0xdc,0x96,0x84,
       0x24,0x83,0xfd,0x39,0x5d,0x3b,0x18,0x7d,
       0x96,0x2b,0xa1,0x39,0x9e,0x0c,0xa3,0x8c     
     }
    };

/*! \known SHA256 strength 256
  INSTANTIATE Hash_DRBG SHA-384 with NO PREDICTION RESISTANCE
*/
static const StringBuf SHA384_256IntEin = 
  {0,32,
     {
       0x0f,0xa6,0x50,0x97,0x5e,0x53,0x5b,0xae,
       0x2a,0xc2,0x2d,0x2c,0xb8,0x15,0x34,0xfa,
       0x5f,0x6b,0x36,0x3d,0x64,0xe8,0x4d,0xfd,
       0x13,0x01,0x22,0x10,0x21,0x0b,0x68,0x4a,
     }
    };
static const StringBuf SHA384_256IntNon = 
  {0,16,
     {
       0x43,0x5f,0x0c,0xc2,0x4c,0xa5,0x9c,0x6f,
       0xa5,0x88,0x82,0xb6,0xc7,0xf1,0x15,0x5d
     }
    };

static const StringBuf SHA384_256IntPer =
  {0,32,
     {
       0x57,0x79,0x2c,0xab,0xd3,0x5e,0x62,0xa0,
       0xea,0xf3,0xd8,0x56,0x23,0x5c,0x68,0xff,
       0x7f,0xe9,0x32,0x62,0x1f,0x33,0xe8,0x12,
       0x39,0x79,0x5a,0xf0,0x3c,0x3b,0x43,0xe3
     }
    };

/* NIST result, truncated from 5376 bits */
static const StringBuf SHA384_256Result =
    {0,64,
     { 
       0x47,0x7c,0x4b,0xf9,0xd8,0x57,0xb5,0x07,
       0x14,0x39,0x3c,0x95,0xf9,0x2b,0x60,0xab,
       0x13,0xfd,0x66,0x83,0xbe,0xe9,0x6e,0xe3,
       0x17,0xe3,0xf8,0x59,0x59,0x25,0x86,0x2c,
       0xc4,0x50,0x67,0x81,0x39,0x9b,0x5a,0xfd,
       0x80,0x6f,0xc3,0x3c,0xec,0x27,0x9e,0x43,
       0xc0,0x18,0xab,0xfd,0x53,0x24,0xce,0x69,
       0x40,0x5d,0xb8,0x63,0xb5,0x47,0x01,0xf5,
     }
    };

/*!
  \FIPS Data structure defining the capabilities and limits
  of the SHA384 PRNG
*/


SP800_90PRNG_t sha384PRNG = {
  SP800_SHA384,
  888/8,
  (1<<27),      /* Max nonce */
  (1<<27),      /* max personalization data */ 
  (1<<27),      /* max AAD */
  (1<<11),      /* max bytes/request */
  0x00FFFFFFL,  /* Max calls between reseeds (< standard specifies) */
  384/8,        /* Block size */
  (1<<27),      /* Max allowed entropy input */ 
  {112,
   128,
   192,
   256
  },
  "SHA384",
  "SHA384",
  1,              /*!< Has a derivation function */
  Inst,
  Res,
  Gen,
  Cln,
  SHA_Instantiate,
  SHA_ReSeed,
  SHA_Generate,
  SHA_Cleanup,
  SP800_IS_FIPS,
  SELF_TEST_AT,
  0,
  {
    {
      &SHA384_112IntEin,
      &SHA384_112IntNon,
      &NONE,
      &NONE,
      &SHA384_112GenEin,
      &SHA384_112Result
    },
    {
      &SHA384_128IntEin,
      &SHA384_128IntNon,
      &NONE,
      &NONE,
      &SHA384_128GenEin,
      &SHA384_128Result
    },
    {
      &SHA384_192IntEin,
      &SHA384_192IntNon,
      &NONE,
      &NONE,
      &SHA384_192GenEin,
      &SHA384_192Result
    },
    {
      &SHA384_256IntEin,
      &SHA384_256IntNon,
      &SHA384_256IntPer,
      &NONE,
      &NONE,
      &SHA384_256Result
 
    },
  }
};
 /*! \known SHA256 PRNG known answer test data */

static const StringBuf SHA256_112IntEin =
  {0,24,
   {0xd9,0x56,0xca,0xa2,0x40,0x39,0xe7,0x6f,
    0x58,0x61,0x6e,0x09,0x69,0xaf,0xa2,0xd7,
    0xb7,0x08,0x74,0x01,0xee,0x2d,0x87,0x77
   }
  };
static const StringBuf SHA256_112IntNon =
  {0,12, 
   {0x32,0xa2,0xef,0x15,0x98,0x3e,0x3c,0x1f,
    0x66,0xe6,0x03,0x2a
   }
  };
static const StringBuf SHA256_112GenEin = 
  {0,24,
   {
     0x7b,0xa5,0xa5,0x22,0x58,0x0b,0x41,0xe1,
     0xa4,0xf5,0x40,0xf9,0xfe,0x3d,0xaa,0xf9,
     0x5d,0xf7,0x72,0x74,0x0a,0x19,0x96,0x51
   }
  };
static const StringBuf SHA256_112Result = 
  {0,32,
   {
     0x87,0x72,0xE9,0xEF,0x03,0x4C,0xA5,0x19,
     0xE9,0x23,0x79,0x80,0x14,0x08,0xB1,0xB8,
     0xD2,0x22,0xEA,0x9F,0x27,0x87,0x1C,0x9D,
     0x98,0x97,0xC0,0xE3,0x55,0xDF,0x92,0x00
   }
  };
static const StringBuf SHA256_128IntEin =
  {0,32,
   {
     0x50,0x21,0xb7,0x21,0xef,0x6a,0xa7,0xab,
     0xaa,0xba,0x55,0x43,0xd5,0x31,0xde,0x46,
     0xa1,0xa1,0x20,0x23,0x38,0xe4,0xf8,0x4b,
     0x1a,0x5e,0xbf,0x5c,0xb9,0xcf,0x06,0x8b
   }
  };
static const StringBuf SHA256_128IntNon =
  {0,16,
   {
     0xfc,0x6e,0xea,0xe2,0x1b,0x3f,0x8d,0x8f,
     0xe3,0x82,0x26,0xfe,0x65,0xc3,0x57,0x08
   }
  };
static const StringBuf SHA256_128GenEin =
  {0,32,
   {
     0x6a,0xe6,0x5a,0xc1,0xe3,0x74,0x03,0x8e,
     0x40,0x35,0x1c,0xd3,0x5d,0x2f,0xdd,0xf7,
     0xd4,0x2a,0xe8,0xe9,0x63,0x8c,0x57,0x1b,
     0xa0,0x73,0x04,0xc0,0xdf,0x13,0x19,0x43
   }
  };
static const StringBuf SHA256_128Result = 
  {0,32,
     {
       0x28,0x32,0x53,0x4F,0xEE,0xE8,0xBF,0x4A,
       0x98,0x29,0xA2,0x63,0xC0,0x40,0x1F,0x15,
       0x59,0xA9,0x52,0x40,0xCE,0x28,0xA8,0x7E,
       0x27,0x4B,0x15,0x75,0x03,0xA8,0x09,0x0F
     }
    };
/* \known SHA 256 strength 192
   INSTANTIATE Hash_DRBG SHA-256 with PREDICTION RESISTANCE ENABLED
*/
static const StringBuf SHA256_192IntEin =
  {0,32,
   {
     0x50,0x21,0xb7,0x21,0xef,0x6a,0xa7,0xab,
     0xaa,0xba,0x55,0x43,0xd5,0x31,0xde,0x46,
     0xa1,0xa1,0x20,0x23,0x38,0xe4,0xf8,0x4b,
     0x1a,0x5e,0xbf,0x5c,0xb9,0xcf,0x06,0x8b
   }
  };
static const StringBuf SHA256_192IntNon = 
  {0,16,
   {
     0xfc,0x6e,0xea,0xe2,0x1b,0x3f,0x8d,0x8f,
     0xe3,0x82,0x26,0xfe,0x65,0xc3,0x57,0x08
   }
  };
static const StringBuf SHA256_192GenEin =
  {0,32,
     {
       0x6a,0xe6,0x5a,0xc1,0xe3,0x74,0x03,0x8e,
       0x40,0x35,0x1c,0xd3,0x5d,0x2f,0xdd,0xf7,
       0xd4,0x2a,0xe8,0xe9,0x63,0x8c,0x57,0x1b,
       0xa0,0x73,0x04,0xc0,0xdf,0x13,0x19,0x43
     }
    };
static const StringBuf SHA256_192Result =
    {0,32,
     { 
       0x28,0x32,0x53,0x4f,0xee,0xe8,0xbf,0x4a,
       0x98,0x29,0xa2,0x63,0xc0,0x40,0x1f,0x15,
       0x59,0xa9,0x52,0x40,0xce,0x28,0xa8,0x7e,
       0x27,0x4b,0x15,0x75,0x03,0xa8,0x09,0x0f 
     }
    };


/*! \known SHA-256 strength 256
  INSTANTIATE Hash_DRBG SHA-256 with NO PREDICTION RESISTANCE
*/
static const StringBuf SHA256_256IntEin = 
  {0,32,
     {
       0x8e,0x9c,0x0d,0x25,0x75,0x22,0x04,0xf9,
       0xc5,0x79,0x10,0x8b,0x23,0x79,0x37,0x14,
       0x9f,0x2c,0xc7,0x0b,0x39,0xf8,0xee,0xef,
       0x95,0x0c,0x97,0x59,0xfc,0x0a,0x85,0x41
     }
    };
static const StringBuf SHA256_256IntNon = 
  {0,16,
     {
       0x76,0x9d,0x6d,0x67,0x00,0x4e,0x19,0x12,
       0x02,0x16,0x53,0xea,0xf2,0x73,0xd7,0xd6,
     }
    };

static const StringBuf SHA256_256IntPer =
  {0,32,
     {
       0x7f,0x7e,0xc8,0xae,0x9c,0x09,0x99,0x7d,
       0xbb,0x9e,0x48,0x7f,0xbb,0x96,0x46,0xb3,
       0x03,0x75,0xf8,0xc8,0x69,0x45,0x3f,0x97,
       0x5e,0x2e,0x48,0xe1,0x5d,0x58,0x97,0x4c
     }
    };
static const StringBuf SHA256_256Result =
    {0,64,
     { 
       0x16,0xe1,0x8c,0x57,0x21,0xd8,0xf1,0x7e,
       0x5a,0xa0,0x16,0x0b,0x7e,0xa6,0x25,0xb4,
       0x24,0x19,0xdb,0x54,0xfa,0x35,0x13,0x66,
       0xbb,0xaa,0x2a,0x1b,0x22,0x33,0x2e,0x4a,
       0x14,0x07,0x9d,0x52,0xfc,0x73,0x61,0x48,
       0xac,0xc1,0x22,0xfc,0xa4,0xfc,0xac,0xa4,
       0xdb,0xda,0x5b,0x27,0x33,0xc4,0xb3,0xec,
       0xb0,0xf2,0xee,0x63,0x11,0x61,0xdb,0x30,
     }
    };


/*!
  \FIPS Data structure defining the capabilities and limits
  of the SHA256 PRNG
*/

SP800_90PRNG_t sha256PRNG = {
  SP800_SHA256,
  440/8,
  (1<<27),      /* Max nonce */
  (1<<27),      /* max personalization data */ 
  (1<<27),      /* max AAD */
  (1<<11),      /* max bytes/request */
  0x00FFFFFFL,  /* Max calls between reseeds (< standard specifies) */
  256/8,        /* Block size */
  (1<<27),      /* Max allowed entropy input */ 
  {112,
   128,
   192,
   256
  },
  "SHA256",
  "SHA256",
  1,              /*!< Has a derivation function */
  Inst,
  Res,
  Gen,
  Cln,
  SHA_Instantiate,
  SHA_ReSeed,
  SHA_Generate,
  SHA_Cleanup,
  SP800_IS_FIPS,
  SELF_TEST_AT,
  0,
  {
    {
      &SHA256_112IntEin,
      &SHA256_112IntNon,
      &NONE,
      &NONE,
      &SHA256_112GenEin,
      &SHA256_112Result
    },
    {
      &SHA256_128IntEin,
      &SHA256_128IntNon,
      &NONE,
      &NONE,
      &SHA256_128GenEin,
      &SHA256_128Result
    },
    {
      &SHA256_192IntEin,
      &SHA256_192IntNon,
      &NONE,
      &NONE,
      &SHA256_192GenEin,
      &SHA256_192Result
    },
    {
      &SHA256_256IntEin,
      &SHA256_256IntNon,
      &SHA256_256IntPer,
      &NONE,
      &NONE,
      &SHA256_256Result
    },

  }
};

/*! \known SHA1 PRNG known answer test data, only one security level */
static const StringBuf SHA224_112IntEin =
  {0,24,
   {
     0xd9,0x56,0xca,0xa2,0x40,0x39,0xe7,0x6f,
     0x58,0x61,0x6e,0x09,0x69,0xaf,0xa2,0xd7,
     0xb7,0x08,0x74,0x01,0xee,0x2d,0x87,0x77
   }
  };
static const StringBuf SHA224_112IntNon =
  {0,12,
   {
     0x32,0xa2,0xef,0x15,0x98,0x3e,0x3c,0x1f,
     0x66,0xe6,0x03,0x2a
   }
  };
static const StringBuf SHA224_112GenEin =
  {0,24,
   {
     0x7b,0xa5,0xa5,0x22,0x58,0x0b,0x41,0xe1,
     0xa4,0xf5,0x40,0xf9,0xfe,0x3d,0xaa,0xf9,
     0x5d,0xf7,0x72,0x74,0x0a,0x19,0x96,0x51
   }
  };
static const StringBuf SHA224_112Result =
  {0,20,
   {
     0x9A,0x3D,0xFE,0x95,0xEE,0x24,0xF8,0x00,
     0x19,0x1E,0x83,0x32,0x21,0x24,0xFF,0xE6,
     0xAA,0xC3,0xCE,0xAC
   }
  };
static const StringBuf SHA224_128IntEin =
  {0,32,
   {
     0x50,0x21,0xb7,0x21,0xef,0x6a,0xa7,0xab,
     0xaa,0xba,0x55,0x43,0xd5,0x31,0xde,0x46,
     0xa1,0xa1,0x20,0x23,0x38,0xe4,0xf8,0x4b,
     0x1a,0x5e,0xbf,0x5c,0xb9,0xcf,0x06,0x8b
   }
  };
static const StringBuf SHA224_128IntNon =
  {0,16,
   {
     0xfc,0x6e,0xea,0xe2,0x1b,0x3f,0x8d,0x8f,
     0xe3,0x82,0x26,0xfe,0x65,0xc3,0x57,0x08
   }
  };
static const StringBuf SHA224_128GenEin =
  {0,32,
   {
     0x6a,0xe6,0x5a,0xc1,0xe3,0x74,0x03,0x8e,
     0x40,0x35,0x1c,0xd3,0x5d,0x2f,0xdd,0xf7,
     0xd4,0x2a,0xe8,0xe9,0x63,0x8c,0x57,0x1b,
     0xa0,0x73,0x04,0xc0,0xdf,0x13,0x19,0x43
   }
  };
static const StringBuf SHA224_128Result = 
  {0,32,
     {
       0xFB,0xB9,0xC0,0x3F,0x9C,0x65,0xCE,0x74,
       0xF3,0x92,0x07,0x03,0x90,0xA6,0xCB,0xC3,
       0x96,0xC3,0xFF,0xDB,0x65,0xF3,0x9A,0x34,
       0xBC,0xE3,0x61,0xC1,0x52,0x01,0xD2,0xA4
     }
    };
/*! \known SHA224 192 bit strength
  INSTANTIATE Hash_DRBG SHA-224 with PREDICTION RESISTANCE ENABLED
*/
static const StringBuf SHA224_192IntEin =
  {0,24,
   {
     0xd9,0x56,0xca,0xa2,0x40,0x39,0xe7,0x6f,
     0x58,0x61,0x6e,0x09,0x69,0xaf,0xa2,0xd7,
     0xb7,0x08,0x74,0x01,0xee,0x2d,0x87,0x77
   }
  };
static const StringBuf SHA224_192IntNon = 
  {0,12,
   {
     0x32,0xa2,0xef,0x15,0x98,0x3e,0x3c,0x1f,
     0x66,0xe6,0x03,0x2a
   }
  };
static const StringBuf SHA224_192GenEin =
  {0,24,
     {
       0x7b,0xa5,0xa5,0x22,0x58,0x0b,0x41,0xe1,
       0xa4,0xf5,0x40,0xf9,0xfe,0x3d,0xaa,0xf9,
       0x5d,0xf7,0x72,0x74,0x0a,0x19,0x96,0x51    
     }
    };


static const StringBuf SHA224_192Result =
    {0,24,
     { 
      0x9a,0x3d,0xfe,0x95,0xee,0x24,0xf8,0x00,
      0x19,0x1e,0x83,0x32,0x21,0x24,0xff,0xe6,
      0xaa,0xc3,0xce,0xac,0x94,0xc8,0xa6,0xb0 
     }
    };

static const StringBuf SHA224_256IntEin = 
  {0,64,
     {
       0x50,0x21,0xb7,0x21,0xef,0x6a,0xa7,0xab,
       0xaa,0xba,0x55,0x43,0xd5,0x31,0xde,0x46,
       0xa1,0xa1,0x20,0x23,0x38,0xe4,0xf8,0x4b,
       0x1a,0x5e,0xbf,0x5c,0xb9,0xcf,0x06,0x8b,
       0x28,0x32,0x53,0x4f,0xee,0xe8,0xbf,0x4a,
       0x98,0x29,0xa2,0x63,0xc0,0x40,0x1f,0x15,
       0x59,0xa9,0x52,0x40,0xce,0x28,0xa8,0x7e,
       0x27,0x4b,0x15,0x75,0x03,0xa8,0x09,0x0f
     }
    };
static const StringBuf SHA224_256IntNon = 
  {0,16,
     {
       0xfc,0x6e,0xea,0xe2,0x1b,0x3f,0x8d,0x8f,
       0xe3,0x82,0x26,0xfe,0x65,0xc3,0x57,0x08
     }
    };

static const StringBuf SHA224_256GenEin =
  {0,64,
     {
       0x6a,0xe6,0x5a,0xc1,0xe3,0x74,0x03,0x8e,
       0x40,0x35,0x1c,0xd3,0x5d,0x2f,0xdd,0xf7,
       0xd4,0x2a,0xe8,0xe9,0x63,0x8c,0x57,0x1b,
       0xa0,0x73,0x04,0xc0,0xdf,0x13,0x19,0x43,
       0x28,0x32,0x53,0x4f,0xee,0xe8,0xbf,0x4a,
       0x98,0x29,0xa2,0x63,0xc0,0x40,0x1f,0x15,
       0x59,0xa9,0x52,0x40,0xce,0x28,0xa8,0x7e,
       0x27,0x4b,0x15,0x75,0x03,0xa8,0x09,0x0f
     }
    };
static const StringBuf SHA224_256Result =
    {0,28,
     { 
       0x78,0x2A,0xA9,0x30,0xF5,0xEB,0x6D,0xD5,
       0x45,0x48,0x38,0xD7,0xF0,0x45,0x4F,0x3B,
       0xE0,0xCC,0xB8,0x82,0x8C,0xAB,0x0E,0x9E,
       0xCF,0xE1,0x1D,0xC0
     }
    };

/*!
  \FIPS Data structure defining the capabilities and limits
  of the SHA224 PRNG
*/

SP800_90PRNG_t sha224PRNG = {
  SP800_SHA224,
  440/8,
  (1<<27),      /* Max nonce */
  (1<<27),      /* max personalization data */ 
  (1<<27),      /* max AAD */
  (1<<11),      /* max bytes/request */
  0x00FFFFFFL,  /* Max calls between reseeds (< standard specifies) */
  224/8,        /* Block size */
  (1<<27),      /* Max allowed entropy input */
  {112,
   128,
   192,
   256
  },
  "SHA224",
  "SHA224",
  1,              /*!< Has a derivation function */
  Inst,
  Res,
  Gen,
  Cln,
  SHA_Instantiate,
  SHA_ReSeed,
  SHA_Generate,
  SHA_Cleanup,
  SP800_IS_FIPS,
  SELF_TEST_AT,
  0,
  {
    {
      &SHA224_112IntEin,
      &SHA224_112IntNon,
      &NONE,
      &NONE,
      &SHA224_112GenEin,
      &SHA224_112Result
    },
   {
      &SHA224_128IntEin,
      &SHA224_128IntNon,
      &NONE,
      &NONE,
      &SHA224_128GenEin,
      &SHA224_128Result
    },
   {
      &SHA224_192IntEin,
      &SHA224_192IntNon,
      &NONE,
      &NONE,
      &SHA224_192GenEin,
      &SHA224_192Result
    },
   {
      &SHA224_256IntEin,
      &SHA224_256IntNon,
      &NONE,
      &NONE,
      &SHA224_256GenEin,
      &SHA224_256Result
    }
  }
 };

/*! \known SHA1 PRNG known answer test data, 
  INSTANTIATE Hash_DRBG SHA-1 with NO PREDICTION RESISTANCE
*/
static const StringBuf SHA1_112IntEin =
  {0,16,
   {
     0xdc,0x10,0x6a,0xce,0x9f,0xf5,0x7c,0x68,
     0x13,0x1e,0xa2,0xee,0x75,0xc6,0x58,0x5a
   }
  };
static const StringBuf SHA1_112IntNon =
  {0,8,
   {
     0x6a,0x36,0x0c,0x6f,0x7b,0xd4,0x60,0x1e
   }
  };
static const StringBuf SHA1_112IntPer = 
  {0,16,
   {
     0x6b,0xd1,0x58,0x91,0x56,0x95,0x25,0x24,
     0xba,0x1f,0x9b,0x14,0x06,0x59,0xba,0xf2
   }
  };

static const StringBuf SHA1_112Result =
  {0,64,
   {
     0x36,0x54,0xD1,0x94,0xA7,0x57,0xD6,0x29,
     0x3C,0xCD,0x30,0x14,0x39,0xA2,0xF6,0x3E,
     0x81,0xCB,0xBB,0x03,0x1F,0x6B,0x47,0x87,
     0x0F,0xF0,0xC4,0x1C,0xF1,0x2A,0xF6,0x3F,
     0x1C,0x8E,0x4D,0x25,0xF4,0x4B,0x90,0x9F,
     0x27,0x6D,0xD0,0x92,0x37,0x3A,0x20,0xDB,
     0x2A,0xD6,0x68,0x06,0x52,0xCE,0x9A,0x87,
     0xBA,0x6E,0x56,0xEA,0xB2,0x01,0xCB,0xEC
   }
  };

/* NIST data truncated from 2240 bits 
static const StringBuf SHA1_112Result =
  {0,64,
   {
     0x3D,0x23,0xCB,0xFF,0xA8,0xFC,0xE3,0x77,
     0x68,0x3E,0xCA,0x0D,0xFE,0x2B,0xAF,0x75,
     0xA6,0xA9,0xA1,0x7E,0xFE,0x71,0x82,0x68,
     0xD2,0x86,0xB8,0xF7,0xCE,0xD7,0xEB,0x6B,
     0x97,0x63,0x3D,0xB2,0xB3,0x8D,0x38,0x1F,
     0xA4,0x16,0xA0,0x32,0xEE,0xB7,0x89,0x5E,
     0xCE,0xDD,0x82,0xE4,0xC4,0xC0,0x70,0x13,
     0xAF,0xE9,0x8A,0xCE,0xC8,0x62,0x91,0x77
   }
  };
*/

/*! \known SHA-1 128 bit strength
  INSTANTIATE Hash_DRBG SHA-1 with PREDICTION RESISTANCE ENABLED
*/
static const StringBuf SHA1_128IntEin =
  {0,16,
   {
    0xb6,0xda,0x6d,0xc2,0xad,0x08,0xba,0x10,
    0xf7,0x8e,0x6e,0x83,0x01,0x57,0x8a,0x52 
   }
  };
static const StringBuf SHA1_128IntNon =
  {0,8,
   {
     0x47,0xb4,0xda,0x6f,0x90,0x32,0xaf,0xc
   }
  };
static const StringBuf SHA1_128GenEin =
  {0,16,
   {
     0x7b,0xbb,0x14,0x85,0x07,0x4a,0xf4,0xd9,
     0x5a,0xad,0x86,0x66,0x3a,0xc8,0x8c,0xe6
   }
  };
static const StringBuf SHA1_128Result = 
  {0,20,
     {
       0x97,0x34,0xED,0x8A,0xD4,0x1A,0x59,0x6F,
       0x86,0x38,0x95,0x72,0xEA,0x7A,0x77,0x7B,
       0x08,0xB3,0x6E,0x7F
     }
    };

/*
static const StringBuf SHA1_128Result = 
    {0,20,
     {
      0x6b,0x17,0x70,0x42,0x17,0x7e,0x76,0x52,
      0xeb,0xdb,0xd8,0x5d,0x1a,0xc2,0x12,0x66,
      0x52,0x07,0x9a,0x6a 
     }
    };
*/



/*!
  \FIPS Data structure defining the capabilities and limits
  of the SHA51 PRNG
*/
SP800_90PRNG_t sha1PRNG = {
  SP800_SHA1,
  440/8,
  (1<<27),      /* Max nonce */
  (1<<27),      /* max personalization data */ 
  (1<<27),      /* max AAD */
  (1<<11),      /* max bytes/request */
  0x00FFFFFFL,  /* Max calls between reseeds (< standard specifies) */
  160/8,        /* Block size */
  (1<<27),      /* Max allowed entropy input */
  {112,
   128,
   0,
   0
  },
  "SHA1",
  "SHA1",
  1,              /*!< Has a derivation function */
  Inst,
  Res,
  Gen,
  Cln,
  SHA_Instantiate,
  SHA_ReSeed,
  SHA_Generate,
  SHA_Cleanup,
  SP800_NON_FIPS,
  SELF_TEST_AT,  
  0,
  {
    {
      &SHA1_112IntEin,
      &SHA1_112IntNon,
      &SHA1_112IntPer,
      &NONE,
      &NONE,
      &SHA1_112Result
    },
    {
      &SHA1_128IntEin,
      &SHA1_128IntNon,
      &NONE,
      &NONE,
      &SHA1_128GenEin,
      &SHA1_128Result
    },
    {
      &NONE,
      &NONE,
      &NONE,
      &NONE,
      &NONE,
      &NONE
    },
    {
      &NONE,
      &NONE,
      &NONE,
      &NONE,
      &NONE,
      &NONE
    },
  }
};

