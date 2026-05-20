/* Copyright IBM Corp. 2023

 Licensed under the Apache License 2.0 (the "License"). You may not use
 this file except in compliance with the License. You can obtain a copy
 in the file LICENSE in the source distribution.
 */
 
/*
  sigtest.c
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

#if 0
#include <stdbool.h>
#else
#define bool int
#define false 0
#define true  1
#endif

#if defined(_WIN32)
#include <windows.h>
#else
#include <stdlib.h>
#endif

# include "icc.h"
/* comment this line out if linking to shared lib import library */
#define DYNAMIC_LOAD_ICC
#ifdef DYNAMIC_LOAD_ICC

#define ICC_FP_NAME pfn_Table

#ifdef ICC_FP_NAME
/* dynamic load library from file name */
# define MAP_ICC_API
# include "icc_loader.h"

ICC_Function_Table fn_Table ;
ICC_Function_Table* pfn_Table = &fn_Table;
#endif
#else
#if defined(JGSK_WRAP)
/* Using JCC_ namespace - reserved for Java JNI */
#include "jcc_a.h"
#else
#if defined(ICKC_WRAP)
/* Using ICKC_ namespace */
#include "ickc_a.h"
#endif
#endif
#endif



/* We want to use PKCS1 and PKCS8 encodings for i2d/d2i */
/* these are all independent bits that can be combined */
/* public is pkcs1 or binary (default) */
/* private is none, raw or pkcs8 */
enum ed { none = 0, raw = 1, pkcs1 = 2, pkcs8 = 4 };

/* Function to read hex string from a file */
static char* read_hex_string_from_file(const char* filename)
{
   FILE* fp = NULL;
   char* hexbuf = NULL;
   long filesize = 0;
   size_t bytes_read = 0;

   fp = fopen(filename, "r");
   if (!fp) {
      printf("Error: Cannot open file '%s'\n", filename);
      return NULL;
   }

   fseek(fp, 0, SEEK_END);
   filesize = ftell(fp);
   fseek(fp, 0, SEEK_SET);

   if (filesize <= 0) {
      printf("Error: File '%s' is empty\n", filename);
      fclose(fp);
      return NULL;
   }

   hexbuf = (char*)malloc(filesize + 1);
   if (!hexbuf) {
      printf("Error: Memory allocation failed\n");
      fclose(fp);
      return NULL;
   }

   bytes_read = fread(hexbuf, 1, filesize, fp);
   fclose(fp);
   
   if (bytes_read != (size_t)filesize) {
      printf("Error: Failed to read file '%s'\n", filename);
      free(hexbuf);
      return NULL;
   }
   hexbuf[bytes_read] = '\0';

   return hexbuf;
}

/* Helper function to print byte arrays in hexadecimal */

static int hex2bin(unsigned char* bin, const char* hexString, size_t hexlen)
{
   /* note hex string may contain spaces so bin len not be exactly hex len / 2, but always less than or equal to */
   unsigned char *r = bin;
   unsigned char byte = 0;
   bool first = true;
   size_t i;
   for (i = 0; i < hexlen; i++)
   {
      char c = hexString[i];
      if (c == ' ' || c == '\t' || c == '\n' || c == '\r')
         continue; /* ignore white space */

      /* convert to binary */
      if ('0' <= c && c <= '9') {
         c -= '0';
      }
      else if ('A' <= c && c <= 'F') {
         c -= 'A';
         c += 10;
      }
      else if ('a' <= c && c <= 'f') {
         c -= 'a';
         c += 10;
      }
      else if (i == hexlen - 1 && !c) {
         /* ignore null at end */
         continue;
      }
      else {
         /* non hex char */
         return -1;
      }

      if (first) {
         byte = (unsigned char) ((c & 0x0F) << 4);
         first = false;
      }
      else {
         /* finish this byte and append to result */
         byte |= (unsigned char)(c & 0x0F);
         *r++ = byte;
         first = true;
      }
   }
   if (!first) {
      /* didn't get second char - error */
      return -2;
   }

   /* return binary length */
   return r - bin;
}

static
void fprintBstr(FILE* fp, const char* S, const unsigned char* A, size_t L)
{
   size_t i;

   fprintf(fp, "%s", S);

   for (i = 0; i < L; i++)
      fprintf(fp, "%02X", A[i]);

   if (L == 0)
      fprintf(fp, "00");

   fprintf(fp, "\n");
}

static
void
count_up(unsigned char* b, size_t n)
{
   size_t i;
   for (i = 0; i < n; i++) {
      b[i] = (unsigned char)i;
   }
}


/* Key buffer definitions */
struct kbuf_s {
   unsigned char* data;
   size_t len;
};
typedef struct kbuf_s kbuf;

void kbuf_dup(kbuf* t, const kbuf* s)
{
   if (t && s && s->data && s->len) {
      t->data = malloc(s->len);
      if (t->data) {
         memcpy(t->data, s->data, s->len);
         t->len = s->len;
      }
   }
}

/* Public key - encoded */
struct pkbuf_s {
   int nid; /* ICC key id */
   kbuf der;
};
typedef struct pkbuf_s pkbuf;

/* Private key - holds context */
struct skbuf_s {
   ICC_EVP_PKEY_CTX* ctx; /* ICC context for the key */
   ICC_EVP_PKEY* key; /* the key */
   int nid; /* ICC key id */
   kbuf der;
};
typedef struct skbuf_s skbuf;

/* Signature buffer */
typedef kbuf sbuf;

/* Generate signature key pair - return 0 for success */

int
SignatureEVP_gen(ICC_CTX* ctx, const char* nm, pkbuf* p_pkc, skbuf* p_skc, enum ed encdec)
{
   ICC_EVP_PKEY_CTX* evp_sp; /* key context */
   ICC_EVP_PKEY* pa = NULL; /* key pair */
   int rv = ICC_OSSL_SUCCESS;

   const int nid = ICC_OBJ_txt2nid(ctx, nm);
   if (!nid) {
      return 1; /* Unsupported algorithm */
   }

   evp_sp = ICC_EVP_PKEY_CTX_new_id(ctx, nid, NULL);
   if (!evp_sp) {
      /* try newer API */
      evp_sp = ICC_EVP_PKEY_CTX_new_from_name(ctx, NULL, nm, NULL);
      if (!evp_sp) {
         return 2; /* Failed to create key context */
      }
   }
   rv = ICC_EVP_PKEY_keygen_init(ctx, evp_sp);
   if (rv != ICC_OSSL_SUCCESS) {
      if (evp_sp) {
         ICC_EVP_PKEY_CTX_free(ctx, evp_sp);
      }
      return 3; /* Keygen initialization failed */
   }

   rv = ICC_EVP_PKEY_keygen(ctx, evp_sp, &pa);
   if (rv != ICC_OSSL_SUCCESS) {
      if (evp_sp) {
         ICC_EVP_PKEY_CTX_free(ctx, evp_sp);
      }
      if (pa) {
         ICC_EVP_PKEY_free(ctx, pa);
      }
      return 4; /* Key generation failed */
   }

   /* public key */

   p_pkc->nid = nid;

   /* encode (always) */
   {
      int len;
      unsigned char* pp = NULL;
      if (encdec & pkcs1) {
         /* PKCS1 */
         /* this variant encodes the OID for the key type rather than a raw encoding */
         len = ICC_i2d_PUBKEY(ctx, pa, NULL);
         if (len <= 0) {
            return 5;
         }
         p_pkc->der.len = len;
         pp = p_pkc->der.data = malloc(len);
         rv = ICC_i2d_PUBKEY(ctx, pa, &pp);
      }
      else {
         /* binary */
         len = ICC_i2d_PublicKey(ctx, pa, NULL);
         if (len <= 0) {
            return 5; /* Invalid public key length */
         }
         p_pkc->der.len = len;
         pp = p_pkc->der.data = malloc(len);
         rv = ICC_i2d_PublicKey(ctx, pa, &pp);
      }
      if (rv <= 0) {
         return 6; /* Failed to encode public key */
      }
   }

   /* private key */

   p_skc->nid = nid;

   /* encode (optional) */

   {
      int len;
      unsigned char* pp = NULL;
      if (encdec & pkcs8) {
         ICC_PKCS8_PRIV_KEY_INFO* p8 = ICC_EVP_PKEY2PKCS8(ctx, pa);
         if (!p8) {
            return 11;
         }
         len = ICC_i2d_PKCS8_PRIV_KEY_INFO(ctx, p8, NULL);
         p_skc->der.len = len;
         pp = p_skc->der.data = malloc(len);
         rv = ICC_i2d_PKCS8_PRIV_KEY_INFO(ctx, p8, &pp);
         if (rv <= 0) {
            return 7; /* Failed to encode private key */
         }
      }
      else if (encdec & raw) {
         len = ICC_i2d_PrivateKey(ctx, pa, NULL);
         p_skc->der.len = len;
         pp = p_skc->der.data = malloc(len);
         rv = ICC_i2d_PrivateKey(ctx, pa, &pp);
         if (rv <= 0) {
            return 7; /* Failed to encode private key */
         }
      }
   }

   p_skc->ctx = evp_sp;
   p_skc->key = pa;

   /* verify encodings */
   /* reconstruct keys from encoding */
   {
      int len;
      ICC_EVP_PKEY* npa = NULL; /* For decoded key */
      const unsigned char* pp = NULL;

      /* public */
      pp = p_pkc->der.data;
      len = (int)p_pkc->der.len;
      if (encdec & pkcs1) {
         /* reconstruct key from encoding */
         npa = ICC_d2i_PUBKEY(ctx, &npa, &pp, len);
      }
      else {
         /* Reconstruct public key from encoding and type */
         npa = ICC_d2i_PublicKey(ctx, p_pkc->nid, &npa, &pp, len);
      }
      if (!npa) {
         return 1; /* Failed to reconstruct public key */
      }

      if (1 != ICC_EVP_PKEY_cmp(ctx, pa, npa)) { /*compare pubkey and decoded key */
         printf("warning - public key encode/decode missmatch\n");
          /* return 20; */
      }
      ICC_EVP_PKEY_free(ctx, npa);
      npa = NULL;

      /* private */
      pp = p_skc->der.data;
      len = (int)p_skc->der.len;
      if (encdec & pkcs8) {
         ICC_PKCS8_PRIV_KEY_INFO* p8 = NULL;
         p8 = ICC_d2i_PKCS8_PRIV_KEY_INFO(ctx, NULL, &pp, len);
         if (!p8) {
            return 11;
         }
         npa = ICC_EVP_PKCS82PKEY(ctx, p8);
         ICC_PKCS8_PRIV_KEY_INFO_free(ctx, p8);
         if (!npa) {
            return 9;
         }
      }
      else if (encdec & raw) {
         npa = ICC_d2i_PrivateKey(ctx, p_skc->nid, &npa, &pp, len);
         if (!npa) {
            return 9;
         }
      }

      if(npa) {
         size_t keylen = ICC_EVP_PKEY_size(ctx, pa);
         size_t kl = ICC_EVP_PKEY_size(ctx, npa);
         if (keylen == 0 || kl != keylen) {
            printf("warning - key size missmatch %d != %d\n", (int)keylen, (int)kl);
            return 22;
         }
         if (1 != ICC_EVP_PKEY_cmp(ctx, pa, npa)) {
            printf("warning - private key encode/decode missmatch\n");
            /* return 21; */
         }
      }

      ICC_EVP_PKEY_free(ctx, npa);
   }

   return 0;
}

/* Sign a message - return 0 for success, non-zero otherwise */

int
SignatureEVP_sign(ICC_CTX* ctx, sbuf* sig, const skbuf* p_skc, const sbuf* msg, enum ed encdec, const char* hash)
{
   int rc;
   size_t siglen = 0;
   unsigned char* signature;

   ICC_EVP_PKEY* skey = p_skc->key;
   if (!skey) {
      /* reconstruct key from encoding */
      const unsigned char* pp = p_skc->der.data;
      if (encdec & pkcs8) {
         ICC_PKCS8_PRIV_KEY_INFO* p8 = NULL;
         p8 = ICC_d2i_PKCS8_PRIV_KEY_INFO(ctx, NULL, &pp, (long)p_skc->der.len);
         if (!p8) {
            return 11;
         }
         skey = ICC_EVP_PKCS82PKEY(ctx, p8);
         ICC_PKCS8_PRIV_KEY_INFO_free(ctx, p8);
      }
      else {
         skey = ICC_d2i_PrivateKey(ctx, p_skc->nid, &skey, &pp, (long)p_skc->der.len);
      }
      if (!skey) {
         return 9;
      }
   }

   /*
   * May need to hash if sign alg is limited in size.
   * Note that PQC hash internally so no hashing reqired.
   */
   if (!hash) {
      ICC_EVP_PKEY_CTX* skc = p_skc->ctx; /* Private key context */

      if (!skc) {
         /* create context from key */
         skc = ICC_EVP_PKEY_CTX_new(ctx, skey, NULL);
         if (!skc) {
            return 10; /* no context */
         }
      }

      rc = ICC_EVP_PKEY_sign_init(ctx, skc);
      if (rc != ICC_OSSL_SUCCESS) {
         return 1; /* Sign initialization failed */
      }

      rc = ICC_EVP_PKEY_sign(ctx, skc, NULL, &siglen, msg->data, msg->len);
      if (rc != ICC_OSSL_SUCCESS) {
         return 2; /* Failed to get signature length */
      }

      signature = malloc(siglen);
      if (!signature) {
         return 3; /* Memory allocation failed */
      }

      rc = ICC_EVP_PKEY_sign(ctx, skc, signature, &siglen, msg->data, msg->len);
      if (rc != ICC_OSSL_SUCCESS) {
         free(signature);
         return 4; /* Signing failed */
      }

      if (!p_skc->ctx) {
         /* free temp ctx */
         ICC_EVP_PKEY_CTX_free(ctx, skc);
      }
   }
   else {
      /* need to hash / sign */
      unsigned int slen;
      ICC_EVP_MD_CTX* md = NULL;
      md = ICC_EVP_MD_CTX_new(ctx);
      ICC_EVP_MD_CTX_init(ctx, md);
      {
         const ICC_EVP_MD* mdt = NULL; /* does not need to be freed */
         mdt = ICC_EVP_get_digestbyname(ctx, hash);
         rc = ICC_EVP_SignInit(ctx, md, mdt);
         if (rc != ICC_OSSL_SUCCESS) {
            return 5;
         }
      }
      rc = ICC_EVP_SignUpdate(ctx, md, msg->data, (unsigned int)msg->len);
      if (rc != ICC_OSSL_SUCCESS) {
         return 6;
      }

      rc = ICC_EVP_SignFinal(ctx, md, NULL, &slen, skey);
      if (rc != ICC_OSSL_SUCCESS) {
         if (!p_skc->key) {
            /* clean up temporary */
            ICC_EVP_PKEY_free(ctx, skey);
         }
         return 7;
      }
      signature = malloc(slen);
      if (!signature) {
         if (!p_skc->key) {
            /* clean up temporary */
            ICC_EVP_PKEY_free(ctx, skey);
         }
         return 3; /* Memory allocation failed */
      }
      rc = ICC_EVP_SignFinal(ctx, md, signature, &slen, skey);
      if (rc != ICC_OSSL_SUCCESS) {
         free(signature);
         if (!p_skc->key) {
            /* clean up temporary */
            ICC_EVP_PKEY_free(ctx, skey);
         }
         return 4; /* Signing failed */
      }
      siglen = slen;

      ICC_EVP_MD_CTX_free(ctx, md);
   }

   sig->data = signature;
   sig->len = siglen;

   if (!p_skc->key) {
      /* clean up temporary */
      ICC_EVP_PKEY_free(ctx, skey);
   }

   return 0; /* Success */
}

/* Verify a signature - return 0 for success, non-zero for failure */
int
SignatureEVP_verify(ICC_CTX* ctx, const pkbuf* p_pkc, const sbuf* msg, const sbuf* sig, enum ed encdec, const char* hash)
{
   int rc = -1;
   ICC_EVP_PKEY* pa = NULL;
   ICC_EVP_PKEY_CTX* evp_pk = NULL;

   /* public key */
   {
      const unsigned char* pp = p_pkc->der.data;
      long len = (long)p_pkc->der.len;

      if (encdec & pkcs1) {
         /* reconstruct key from encoding */
         pa = ICC_d2i_PUBKEY(ctx, &pa, &pp, (long)len);
      }
      else {
         /* Reconstruct public key from encoding and type */
         pa = ICC_d2i_PublicKey(ctx, p_pkc->nid, &pa, &pp, len);
      }
      if (!pa) {
         return 1; /* Failed to reconstruct public key */
      }
   }

   /* EVP context */
   evp_pk = ICC_EVP_PKEY_CTX_new(ctx, pa, NULL);
   if (!evp_pk) {
      /* try new API */
      evp_pk = ICC_EVP_PKEY_CTX_new_from_pkey(ctx, NULL, pa, NULL);
      if (!evp_pk) {
         ICC_EVP_PKEY_free(ctx, pa);
         return 2; /* Failed to create public key context */
      }
   }

   if (!hash) {
      rc = ICC_EVP_PKEY_verify_init(ctx, evp_pk);
      if (rc != ICC_OSSL_SUCCESS) {
         ICC_EVP_PKEY_free(ctx, pa);
         ICC_EVP_PKEY_CTX_free(ctx, evp_pk);
         return 3; /* Verification initialization failed */
      }

      rc = ICC_EVP_PKEY_verify(ctx, evp_pk, sig->data, sig->len, msg->data, msg->len);
   }
   else {
      /* need to hash / verify */
      
      ICC_EVP_MD_CTX* md = NULL;
      md = ICC_EVP_MD_CTX_new(ctx);
      ICC_EVP_MD_CTX_init(ctx, md);
      {
         const ICC_EVP_MD* mdt = NULL; /* does not need to be freed */
         mdt = ICC_EVP_get_digestbyname(ctx, hash);
         rc = ICC_EVP_VerifyInit(ctx, md, mdt);
         if (rc != ICC_OSSL_SUCCESS) {
            ICC_EVP_MD_CTX_free(ctx, md);
            ICC_EVP_PKEY_free(ctx, pa);
            ICC_EVP_PKEY_CTX_free(ctx, evp_pk);
            return 5;
         }
      }
      rc = ICC_EVP_VerifyUpdate(ctx, md, msg->data, (unsigned int)msg->len);
      if (rc != ICC_OSSL_SUCCESS) {
         ICC_EVP_MD_CTX_free(ctx, md);
         ICC_EVP_PKEY_free(ctx, pa);
         ICC_EVP_PKEY_CTX_free(ctx, evp_pk);
         return 6;
      }

      rc = ICC_EVP_VerifyFinal(ctx, md, sig->data, (unsigned int)sig->len, pa);
      ICC_EVP_MD_CTX_free(ctx, md);
   }

   /* clean up */
   ICC_EVP_PKEY_free(ctx, pa);
   ICC_EVP_PKEY_CTX_free(ctx, evp_pk);
 
   /* final status check of the verification call */
   if (rc < 0) {
      return 4; /* Verification error */
   }
   if (rc == 0) {
      return 5; /* Verification failed */
   }

   return 0; /* Verification succeeded */
}

/* Signature test function - generates keys, signs a message, and verifies the signature */
int
PQC_sign_test(ICC_CTX* ctx, const char* algname, const char* hash, int verbose, int encdec, const kbuf* pub, const kbuf* pri, const sbuf* msg, const sbuf* sig)
{
   FILE* fp_rsp = stdout;
   int ret_val;

   pkbuf pk = { 0 }; /* public key */
   skbuf sk = { 0 }; /* private/secret key */

   if (verbose) {
      printf("Algorithm : %s\n", algname);
      printf("Hash : %s\n", hash ? hash : "NULL");
   }

   if ((pub && pub->data) && (pri && pri->data)) {
      /* use supplied keys */
      const int nid = ICC_OBJ_txt2nid(ctx, algname);

      kbuf_dup(&pk.der, pub);
      pk.nid = nid;
      kbuf_dup(&sk.der, pri);
      sk.nid = nid;
   }
   else
   {
      /* Generate the public/private keypair */
      if (verbose) {
         printf("keygen\n");
      }
      if ((ret_val = SignatureEVP_gen(ctx, algname, &pk, &sk, encdec)) != 0) {
         printf("Error: SignatureEVP_gen(ctx, %s, &pk, &sk, %d) returned <%d>\n", algname, encdec, ret_val);
         return 1;
      }
   }
   if (verbose) {
      fprintf(fp_rsp, "pk encoding length = %d\n", (int)pk.der.len);
      fprintf(fp_rsp, "pk %s\n", (encdec & pkcs1) ? "pkcs1" : "raw");
      fprintBstr(fp_rsp, "", pk.der.data, pk.der.len);

      fprintf(fp_rsp, "sk encoding length = %d\n", (int)sk.der.len);
      if (encdec & (raw | pkcs8)) {
         fprintf(fp_rsp, "sk %s\n", (encdec & pkcs8) ? "pkcs8" : "raw");
         fprintBstr(fp_rsp, "", sk.der.data, sk.der.len);
      }
   }

   {
      int keylen = 0;
      keylen = ICC_EVP_PKEY_size(ctx, sk.key);
      if (verbose) {
         fprintf(fp_rsp, "key size = %d\n", keylen);
      }
   }

   /* get rid of gen context */
   if (sk.ctx) {
      ICC_EVP_PKEY_CTX_free(ctx, sk.ctx);
      sk.ctx = NULL;
   }

   if (encdec & (raw|pkcs8)) {
      /* delete ICC key and context so we use private encoding */
      if (sk.key) {
         ICC_EVP_PKEY_free(ctx, sk.key);
         sk.key = NULL;
      }
   }

   {
      sbuf signature = { 0,0 };
      kbuf_dup(&signature, sig);

      if (!signature.data) {
         if (verbose) {
            printf("sign\n");
         }
         if ((ret_val = SignatureEVP_sign(ctx, &signature, &sk, msg, encdec, hash)) != 0) {
            printf("SignatureEVP_sign returned <%d>\n", ret_val);
            return 2;
         }
         if (verbose == 2) {
            fprintBstr(fp_rsp, "signature =\n", signature.data, signature.len);
         }
      }

      fprintf(fp_rsp, "\n");
      if (verbose) {
         printf("verify\n");
      }
      {
         ret_val = SignatureEVP_verify(ctx, &pk, msg, &signature, encdec, hash);
         if (ret_val != 0) {
            printf("SignatureEVP_verify failed with code <%d>\n", ret_val);
            return 3;
         }
         else {
            if (verbose)
               printf("Signature verification succeeded.\n");
         }
      }
      if (signature.data) {
         free(signature.data);
         signature.data = NULL;
      }
   }

   /* Clean up allocated resources */
   if (pk.der.data) {
      free(pk.der.data);
      pk.der.data = NULL;
   }
   if (sk.ctx) {
      ICC_EVP_PKEY_CTX_free(ctx, sk.ctx);
      sk.ctx = NULL;
   }
   if (sk.key) {
      ICC_EVP_PKEY_free(ctx, sk.key);
      sk.key = NULL;
   }
   if (sk.der.data) {
      free(sk.der.data);
      sk.der.data = NULL;
   }

   return 0;
}

/* Performance test function - measures time for key generation, signing, and verification */
int
alg_performance(ICC_CTX* ctx, const char* algname, const char* hash, sbuf* msg, enum ed encdec)
{
   clock_t start, end;
   double elapsed_time;
   int ret_val;
   pkbuf pk = { 0 }; /* public key */
   skbuf sk = { 0 }; /* private/secret key */
   printf("Performance testing for algorithm: %s\n", algname);

   /* Measure key generation time */
   start = clock();
   ret_val = SignatureEVP_gen(ctx, algname, &pk, &sk, encdec);
   end = clock();
   if (ret_val != 0) {
      printf("Error: SignatureEVP_gen(ctx, %s, &pk, &sk, %d) returned <%d>\n", algname, encdec, ret_val);
      return 1;
   }

   elapsed_time = (double)(end - start) / CLOCKS_PER_SEC;
   printf("Generating key pair succeeded\n");
   printf("Elapsed time: %f seconds\n", elapsed_time);

   /* get rid of gen context */
   if (sk.ctx) {
      ICC_EVP_PKEY_CTX_free(ctx, sk.ctx);
      sk.ctx = NULL;
   }

   if (encdec & (raw | pkcs8)) {
      /* delete ICC key and context so we use private encoding */
      if (sk.key) {
         ICC_EVP_PKEY_free(ctx, sk.key);
         sk.key = NULL;
      }
   }

   /* Measure signing time */
   sbuf signature;

   /* sometimes op is quicker than clock can resolve so do multiple until we get a time */
   {
      size_t i;
      end = start = clock();
      for (i = 0; end == start; i++) {
         ret_val = SignatureEVP_sign(ctx, &signature, &sk, msg, encdec, hash);
         end = clock();

         if (ret_val != 0) {
            printf("SignatureEVP_sign failed with code <%d>\n", ret_val);
            return 2;
         }
      }

      elapsed_time = (double)(end - start) / i / CLOCKS_PER_SEC;
   }
   printf("Generating signatures succeeded\n");
   printf("Elapsed time: %f seconds\n", elapsed_time);

   /* Measuring verification time */
   {
      size_t i;
      end = start = clock();
      for (i = 0; end == start; i++) {
         ret_val = SignatureEVP_verify(ctx, &pk, msg, &signature, encdec, hash);
         end = clock();

         if (ret_val != 0) {
            printf("SignatureEVP_verify failed with code <%d>\n", ret_val);
            return 3;
         }
      }

      elapsed_time = (double)(end - start) / i / CLOCKS_PER_SEC;
   }
   printf("Signatures verification succeeded\n");
   printf("Elapsed time: %f seconds\n", elapsed_time);

   /* Clean up allocated resources */
   if (signature.data) {
      free(signature.data);
      signature.data = NULL;
   }
   if (pk.der.data) {
      free(pk.der.data);
      pk.der.data = NULL;
   }
   if (sk.ctx) {
      ICC_EVP_PKEY_CTX_free(ctx, sk.ctx);
      sk.ctx = NULL;
   }
   if (sk.key) {
      ICC_EVP_PKEY_free(ctx, sk.key);
      sk.key = NULL;
   }
   if (sk.der.data) {
      free(sk.der.data);
      sk.der.data = NULL;
   }

   return 0;
}


static
char* algs[] =
{
   "rsaEncryption",
   "ML-DSA-44", /* "Dilithium_512",*/
   "ML-DSA-65", /* "Dilithium_768",*/
   "ML-DSA-87", /* "Dilithium_1024",*/
   /* sphincs */
   "SLH_DSA_SHA2_128s",
   "SLH_DSA_SHA2_128f",
   "SLH_DSA_SHA2_192s",
   "SLH_DSA_SHA2_192f",
   "SLH_DSA_SHA2_256s",
   "SLH_DSA_SHA2_256f",
   "SLH_DSA_SHAKE_128s",
   "SLH_DSA_SHAKE_128f",
   "SLH_DSA_SHAKE_192s",
   "SLH_DSA_SHAKE_192f",
   "SLH_DSA_SHAKE_256s",
   "SLH_DSA_SHAKE_256f",
   NULL
};

/* Map command line arguments to signature algorithm names */
static
const char* to_SIGNATURE_ALGNAME(int k)
{
   /* k is 1 based so adjust for 0 based index */
   if (k > sizeof(algs) / sizeof(algs[0]))
      return NULL;
   return algs[k-1];
}

static
void fcb(const char* a, int b, int c)
{
   printf("fcb:%s, %d, %d\n", a, b, c);
}

static
void tcb(const char* val1, const char* val2)
{
   printf("Tcb:%s, %s \n", val1, val2);
}

static
int OpenSSLError(ICC_CTX* ctx)
{
   long retcode = -1;
   unsigned max = 5;
   /* may be more than one error recorded so print them all */
   while (retcode) {
      retcode = ICC_ERR_get_error(ctx);
      if (retcode) {
         static char buf[4096];
         ICC_ERR_error_string(ctx, retcode, buf);
         printf("OpenSSL error %ld [%s]\n", retcode, buf);
      }
      /* infinite loop breaker */
      if (max == 0) break;
      max--;
   }
   return retcode;
}

/* Main function to parse arguments and execute signature tests */
int main(int argc, const char *argv[])
{
   const char* algname = NULL;
   const char* hash = NULL; /*eg, "SHA256"*/
   const char* iccPath = NULL;
   kbuf pubKey = { 0,0, };
   kbuf priKey = { 0,0, };
   sbuf msg = { 0,100 };
   sbuf sig = { 0,0 };
   bool isFips = false, wantFips = false;
   int verbose = 0; /* 1 = verbose, 2 = more verbose */
   bool wantTraceCB = false; /* Trace callback */
   bool wantFipsCB = false; /* FIPS callback */
   bool performanceTest = false; /* Performance test flag */
   enum ed encdec = none;
   int rv = 0;

   /* Parse command-line arguments */
   if(argc > 1) {
      const char* arg;
      int i;
      for( i = 1; rv == 0 && i < argc; i++) {
         arg = argv[i];
         if (0 == strcmp(arg, "-?")) {
            int j;
            printf("Usage: sigtest [-v|V] [-fips] [-fcb] [-tcb] [-p] [-path] [-alg <algorithm name>] [-h <hash algorithm name>] [-l <message data length>] [-ed <none|raw|pkcs[1|8]>] [<number (see list below)>]\n");
            printf("     -v       Verbose (dump keys), -V more verbose (dump signatures)\n");
            printf("     -fips    Request FIPS mode ICC\n");
            printf("     -fcb     Install a FIPS callback routine (prints message 'fcb:...')\n");
            printf("     -tcb     Install a TRACE callback routine (prints message 'tcb:...')\n");
            printf("     -alg     Refer following table...\n");
            printf("     -p       Run performance test for all algorithms\n");
            printf("     -path    icc library path\n");
            printf("     -pub     Specify public key PKCS1 encoding in hex\n");
            printf("     -pri     Specify private key PKCS8 encoding in hex\n");
            printf("     -msg     Specify message to sign hex\n");
            printf("     -sig     Specify signature in hex\n");
            printf("     -sigf    Specify signature file containing hex data\n");
            for ( j = 1; to_SIGNATURE_ALGNAME(j); j++) {
               printf(" %d    %s\n", j, to_SIGNATURE_ALGNAME(j));
            }
            printf("     -hash    OpenSSL/ICC hash function (e.g. SHA256)\n");
            printf("     -ed      Key Encoding\n");
            argc = 0;
            rv = 0;
         }
         else if (0 == strcmp(arg, "-fips")) {
            wantFips = true;
         }
         else if (0 == strcmp(arg, "-fcb")) {
            wantFipsCB = true;
            wantFips = true;
         }
         else if (0 == strcmp(arg, "-tcb")) {
            wantTraceCB = true;
         }
         else if (0 == strcmp(arg, "-h")) {
            i++;
            hash = argv[i];
         }
         else if (0 == strcmp(arg, "-l")) {
            i++;
            msg.len = atoi(argv[i]);
         }
         else if (0 == strcmp(arg, "-p")) {
            performanceTest = true;
         }
         else if (0 == strcmp(arg, "-path")) {
            i++;
            iccPath = argv[i];
         }
         else if (0 == strcmp(arg, "-v")) {
            verbose = 1;
         }
         else if (0 == strcmp(arg, "-V")) {
            verbose = 2;
         }
         else if (0 == strcmp(arg, "-ed")) {
            i++;
            arg = argv[i];
            if (!strcmp(arg, "none"))
               encdec = none;
            else if (!strcmp(arg, "raw"))
               encdec = raw;
            else if (!strcmp(arg, "pkcs1"))
               encdec = pkcs1;
            else if (!strcmp(arg, "pkcs8"))
               encdec = pkcs8;
            else if (!strcmp(arg, "pkcs"))
               encdec = pkcs1 | pkcs8;
            else {
               printf("%s: bad encoding, try -? to get help\n", arg);
               rv = -1;
            }
         }
         else if (0 == strcmp(arg, "-pub")) {
            i++;
            if (pubKey.data) free(pubKey.data);
            pubKey.data = malloc(strlen(argv[i]) / 2);
            pubKey.len = hex2bin(pubKey.data, argv[i], strlen(argv[i]));
         }
         else if (0 == strcmp(arg, "-pri")) {
            i++;
            if (priKey.data) free(priKey.data);
            priKey.data = malloc(strlen(argv[i]) / 2);
            priKey.len = hex2bin(priKey.data, argv[i], strlen(argv[i]));
         }
         else if (0 == strcmp(arg, "-msg")) {
            i++;
            if (msg.data) free(msg.data);
            msg.data = malloc(strlen(argv[i]) / 2);
            msg.len = hex2bin(msg.data, argv[i], strlen(argv[i]));
         }
         else if (0 == strcmp(arg, "-sig")) {
            i++;
            if (sig.data) free(sig.data);
            sig.data = malloc(strlen(argv[i]) / 2);
            sig.len = hex2bin(sig.data, argv[i], strlen(argv[i]));
         }
         else if (0 == strcmp(arg, "-sigf")) {
            i++;
            char* hexstr = read_hex_string_from_file(argv[i]);
            if (hexstr) {
               if (sig.data) free(sig.data);
               sig.data = malloc(strlen(hexstr) / 2);
               sig.len = hex2bin(sig.data, hexstr, strlen(hexstr));
               free(hexstr);
            } else {
               rv = -1;
            }
         }
         else if (0 == strcmp(arg, "-alg")) {
            i++;
            algname = argv[i];
         }
         else if (*arg == '-') {
            /* unknown setting - pass it on */
            printf("unknown arg %s", arg);
            rv = -1;
         }
         else {
            int k = 0;
            k = (int)atoi(arg);
            algname = to_SIGNATURE_ALGNAME(k);
            if (k == 0 || !algname) {
               printf("%s: bad argument, try -? to get help\n", arg);
               rv = -1;
            }
         }
      }
   }

   if (rv == 0)
   {
      ICC_STATUS status;
      ICC_CTX* icc_ctx = NULL;
#ifdef ICC_FP_NAME
      LIB_HANDLE lib_handle = load_icc_library(iccPath);
      if(lib_handle == NULL)
      {
         printf("Failed to load library %s\n", iccPath);
         rv = -1;
         goto free_pub_pri_data;
      }
      
      if(0 != load_icc_functions(lib_handle, pfn_Table))
      {
         printf("Failed to load functions \n");
         unload_icc_functions(pfn_Table); /* Properly unload the library to prevent memory leak */
         rv = -1;
         goto free_pub_pri_data;
      }
      /* Initialize ICC context */
      icc_ctx = ICC_Init(&status, NULL);
#else
      icc_ctx = ICC_Init(&status, iccPath);
#endif
      if (NULL == icc_ctx) {
         printf("ICC not initialized, exiting\n");
         if (iccPath)
            printf("icc path was: %s\n", iccPath);
         exit(1);
      }

      /*
      * Set FIPS only work before the attach
      */
      if (wantFips) {
         isFips = true;
         ICC_SetValue(icc_ctx, &status, ICC_FIPS_APPROVED_MODE, wantFips ? "on" : "off");
         if (ICC_OK != status.majRC)
         {
            printf("ICC_SetValue(ICC_FIPS_APPROVED_MODE) failed %d\n", rv);
            isFips = false;
         }
      }
      printf("FIPS %s.\n", isFips ? "on" : "off");

      /* Attach to ICC */
      if (ICC_ERROR == ICC_Attach(icc_ctx, &status)) {
         printf("ICC_Attach() failed, exiting\n");
         ICC_Cleanup(icc_ctx, &status);
         exit(1);
      }

      /* check and report the ICC version we found */
      {
         char iccversion[ICC_VALUESIZE+1];
         if (ICC_ERROR == ICC_GetValue(icc_ctx, &status, ICC_VERSION, (void*)iccversion, ICC_VALUESIZE)) {
            printf("ICC_GetValue() failed, exiting\n");
            ICC_Cleanup(icc_ctx, &status);
            exit(1);
         }
         iccversion[ICC_VALUESIZE] = '\0';
         printf("ICC_Version: %s\n", iccversion);
         if (strstr(iccversion, "8.6") != NULL) {
            printf("sigtest: %s\n", "ICC 8.6 not supported");
            exit(1);
         }
      }

      /*
      * Callbacks only work after the attach
      * Also, FIPS callback only works in FIPS mode
      */
      if (wantFipsCB) {
         typedef void (*CALLBACK_T)(const char*, int, int);
         CALLBACK_T x = fcb;

         rv = ICC_SetValue(icc_ctx, &status, ICC_FIPS_CALLBACK, &x);
         if (ICC_OK != status.majRC && isFips)
         {
            printf("ICC_SetValue(ICC_FIPS_CALLBACK) failed %d\n", rv);
            isFips = false;
         }
      }

      if (wantTraceCB) {
         typedef void (*TRACE_CALLBACK_T)(const char*, const char*);
         TRACE_CALLBACK_T x = tcb;
      
         rv = ICC_SetValue(icc_ctx, &status, ICC_TRACE_CALLBACK, &x);
         if (ICC_OK != status.majRC)
         {
            printf("ICC_SetValue(ICC_TRACE_CALLBACK) failed %d\n", rv);
         }
         ICC_GetValue(icc_ctx, &status, ICC_TRACE_CALLBACK, &x, sizeof(TRACE_CALLBACK_T));
      }
  
#if 0
      /* ICC has no DRGB so we can't do KAT on ICC */
      {
         unsigned char entropy_input[48];
         count_up(entropy_input, sizeof(entropy_input));
         randombytes_init(entropy_input, NULL, 256);
      }
#endif

      if (!msg.data) {
         msg.data = malloc(msg.len);
         if (msg.data) {
            count_up(msg.data, msg.len);
            if (verbose == 2) {
               fprintBstr(stdout, "message =\n", msg.data, msg.len);
            }
         }
      }

      if (performanceTest) {
         if (algname) {
            /* Run performance test for the specified algorithm */
            printf("\n--- Performance test for algorithm: %s ---\n", algname);
            rv = alg_performance(icc_ctx, algname, hash, &msg, encdec);
            if (rv) {
               OpenSSLError(icc_ctx);
               printf("%s: Error %d during performance test\n", algname, rv);
            }
         } 
         else {
            /* Run performance test for all algorithms */
            int i;
            for (i = 0; algs[i] != NULL; i++) {
               printf("\n--- Performance test for algorithm %d: %s ---\n", i+1, algs[i]);
               rv = alg_performance(icc_ctx, algs[i], hash, &msg, encdec);
               if (rv) {
                  OpenSSLError(icc_ctx);
                  printf("%s: Error %d during performance test\n", algs[i], rv);
               }
            }  
         } 
      } else {
         if (algname) {
            /* Execute the sigPQC_Sign_test for specified algname */
            rv = PQC_sign_test(icc_ctx, algname, hash, verbose, encdec, &pubKey, &priKey, &msg, &sig);
            if (rv) {
               OpenSSLError(icc_ctx);
               printf("%s: Error %d, try -? to get help\n", algname, rv);
            }
            else{
               printf("PQC_Sign_test for algorithm: %s successful\n", algname);
            }
         }
         else {
            /* Run PQC_Sign_test test for all algorithms */
            int i;
            for (i = 0; algs[i] != NULL; i++) {
               printf("\n--- PQC_Sign_test for algorithm %d: %s ---\n", i+1, algs[i]);
               rv = PQC_sign_test(icc_ctx, algs[i], hash, verbose, encdec, NULL, NULL, &msg, NULL);
               if (rv) {
                  OpenSSLError(icc_ctx);
                  printf("%s: Error %d during Signature test\n", algs[i], rv);
                  break;
               }
               else{
                  printf("PQC_Sign_testfor algorithm: %s successful\n", algs[i]);
               }
            }
         }
      }

      /* Clean up ICC context */
      ICC_Cleanup(icc_ctx, &status);
   }

free_pub_pri_data:
   if (pubKey.data) free(pubKey.data);
   if (priKey.data) free(priKey.data);
   if (msg.data) free(msg.data);
   if (sig.data) free(sig.data);

   return rv;
}

