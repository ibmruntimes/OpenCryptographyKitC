
#ifndef ICC_LOADER_H
#define ICC_LOADER_H

#include <stdint.h>

#ifdef _WIN32
#include <windows.h>
#else
#include <dlfcn.h>
#endif

/* Define ICC_LINK before using it in function pointers */
#ifndef ICC_LINK
#if defined (_WIN32)
#define ICC_LINK __cdecl
#else
#define ICC_LINK
#endif
#endif

#include "icc.h"

struct ICC_FunctionTable_s {
    /* Library handle for the loaded library */
    void* lib_handle;
   ICC_CTX* (ICC_LINK* ICC_Init)(ICC_STATUS* status, const char* iccpath);
#ifdef _WIN32
   ICC_CTX* (ICC_LINK* ICC_InitW)(ICC_STATUS* status, const wchar_t* iccpath);
   int (ICC_LINK* gskiccs_pathW)(wchar_t* return_path, int path_len);
#endif
   int (ICC_LINK* gskiccs_path)(char* return_path, int path_len);
   int (ICC_LINK* gskiccs8_path)(char* return_path, int path_len);
   int (ICC_LINK* ICC_GetStatus)(ICC_CTX* pcb, ICC_STATUS* status);
   int (ICC_LINK* ICC_SetValue)(ICC_CTX* pcb, ICC_STATUS* status, ICC_VALUE_IDS_ENUM valueID, const void* value);
   int (ICC_LINK* ICC_GetValue)(ICC_CTX* pcb, ICC_STATUS* status, ICC_VALUE_IDS_ENUM valueID, void* value, int valueLength);
   int (ICC_LINK* ICC_Attach)(ICC_CTX* pcb, ICC_STATUS* status);
   int (ICC_LINK* ICC_Cleanup)(ICC_CTX* pcb, ICC_STATUS* status);
   int (ICC_LINK* ICC_SelfTest)(ICC_CTX* pcb, ICC_STATUS* status);

   const char* (ICC_LINK* ICC_OBJ_nid2sn)(ICC_CTX* pcb, int nid);

   int (ICC_LINK* ICC_RAND_bytes)(ICC_CTX* pcb, unsigned char* buf, int num);
   void (ICC_LINK* ICC_RAND_seed)(ICC_CTX* pcb, const void* buf, int num);

   unsigned long (ICC_LINK* ICC_ERR_get_error)(ICC_CTX* pcb);
   unsigned long (ICC_LINK* ICC_ERR_peek_error)(ICC_CTX* pcb);
   unsigned long (ICC_LINK* ICC_ERR_peek_last_error)(ICC_CTX* pcb);
   char* (ICC_LINK* ICC_ERR_error_string)(ICC_CTX* pcb, unsigned long e, char* buf);
   void (ICC_LINK* ICC_ERR_error_string_n)(ICC_CTX* pcb, unsigned long e, char* buf, size_t len);
   const char* (ICC_LINK* ICC_ERR_lib_error_string)(ICC_CTX* pcb, unsigned long e);
   const char* (ICC_LINK* ICC_ERR_func_error_string)(ICC_CTX* pcb, unsigned long e);
   const char* (ICC_LINK* ICC_ERR_reason_error_string)(ICC_CTX* pcb, unsigned long e);
   void (ICC_LINK* ICC_ERR_clear_error)(ICC_CTX* pcb);
   void (ICC_LINK* ICC_ERR_remove_state)(ICC_CTX* pcb, unsigned long pid);

   ICC_PRNG* (ICC_LINK* ICC_get_RNGbyname)(ICC_CTX* pcb, const char* algname);
   ICC_PRNG_CTX* (ICC_LINK* ICC_RNG_CTX_new)(ICC_CTX* pcb);
   void (ICC_LINK* ICC_RNG_CTX_free)(ICC_CTX* pcb, ICC_PRNG_CTX* prng_ctx);
   SP800_90STATE (ICC_LINK* ICC_RNG_CTX_Init)(ICC_CTX* pcb, ICC_PRNG_CTX* ctx, ICC_PRNG* alg, unsigned char* person, unsigned int personal, unsigned int strength, int prediction_resistance);
   SP800_90STATE (ICC_LINK* ICC_RNG_Generate)(ICC_CTX* pcb, ICC_PRNG_CTX* ctx, unsigned char* buffer, unsigned int n, unsigned char* adata, unsigned int adatal);
   SP800_90STATE (ICC_LINK* ICC_RNG_ReSeed)(ICC_CTX* pcb, ICC_PRNG_CTX* ctx, unsigned char* adata, unsigned int adatal);
   SP800_90STATE (ICC_LINK* ICC_RNG_CTX_ctrl)(ICC_CTX* pcb, ICC_PRNG_CTX* ctx, SP800_90CTRL type, int arg, void* ptr);
   void (ICC_LINK* ICC_GenerateRandomSeed)(ICC_CTX* pcb, ICC_STATUS* icc_stat, int seedLength, void* seed);

   int (ICC_LINK* ICC_BN_num_bits)(ICC_CTX* pcb, const ICC_BIGNUM* a);
   ICC_BIGNUM* (ICC_LINK* ICC_BN_new)(ICC_CTX* pcb);
   ICC_BIGNUM* (ICC_LINK* ICC_BN_bin2bn)(ICC_CTX* pcb, const unsigned char* s, int len, ICC_BIGNUM* ret);
   int (ICC_LINK* ICC_BN_bn2bin)(ICC_CTX* pcb, const ICC_BIGNUM* a, unsigned char* to);
   void (ICC_LINK* ICC_BN_clear_free)(ICC_CTX* pcb, ICC_BIGNUM* a);

   int (ICC_LINK* ICC_DES_random_key)(ICC_CTX* pcb, ICC_DES_cblock* ret);
   void (ICC_LINK* ICC_DES_set_odd_parity)(ICC_CTX* pcb, /*ICC_DES_cblock*/ void* key);

   ICC_EVP_PKEY* (ICC_LINK* ICC_EVP_PKEY_new)(ICC_CTX* pcb);
   void (ICC_LINK* ICC_EVP_PKEY_free)(ICC_CTX* pcb, ICC_EVP_PKEY* pkey);
   int (ICC_LINK* ICC_EVP_PKEY_cmp)(ICC_CTX* pcb, const ICC_EVP_PKEY* a, const ICC_EVP_PKEY* b);
   const ICC_EVP_CIPHER* (ICC_LINK* ICC_EVP_get_cipherbyname)(ICC_CTX* pcb, const char* name);
   const ICC_EVP_MD* (ICC_LINK* ICC_EVP_get_digestbyname)(ICC_CTX* pcb, const char* name);
   int (ICC_LINK* ICC_EVP_MD_size)(ICC_CTX* pcb, const ICC_EVP_MD* e);
   int (ICC_LINK* ICC_EVP_MD_block_size)(ICC_CTX* pcb, const ICC_EVP_MD* e);
   const ICC_EVP_MD* (ICC_LINK* ICC_EVP_MD_CTX_md)(ICC_CTX* pcb, const ICC_EVP_MD_CTX* e);
   ICC_EVP_MD_CTX* (ICC_LINK* ICC_EVP_MD_CTX_new)(ICC_CTX* pcb);
   int (ICC_LINK* ICC_EVP_MD_CTX_free)(ICC_CTX* pcb, ICC_EVP_MD_CTX* x);
   int (ICC_LINK* ICC_EVP_MD_CTX_copy)(ICC_CTX* pcb, ICC_EVP_MD_CTX* out, const ICC_EVP_MD_CTX* in);
   // in openssl EVP_MD_CTX_init is defined to be EVP_MD_CTX_reset
   void (ICC_LINK* ICC_EVP_MD_CTX_init)(ICC_CTX* pcb, ICC_EVP_MD_CTX* ctx);
   int (ICC_LINK* ICC_EVP_MD_CTX_cleanup)(ICC_CTX* pcb, ICC_EVP_MD_CTX* ctx);
   int (ICC_LINK* ICC_EVP_MD_type)(ICC_CTX* pcb, const ICC_EVP_MD* md);

   int (ICC_LINK* ICC_EVP_EncryptInit)(ICC_CTX* pcb, ICC_EVP_CIPHER_CTX* ctx, const ICC_EVP_CIPHER* type, const unsigned char* key, const unsigned char* iv);
   int (ICC_LINK* ICC_EVP_EncryptUpdate)(ICC_CTX* pcb, ICC_EVP_CIPHER_CTX* ctx, unsigned char* out, int* outl, const unsigned char* in, int inl);
   int (ICC_LINK* ICC_EVP_EncryptFinal)(ICC_CTX* pcb, ICC_EVP_CIPHER_CTX* ctx, unsigned char* out, int* outl);
   int (ICC_LINK* ICC_EVP_DecryptInit)(ICC_CTX* pcb, ICC_EVP_CIPHER_CTX* ctx, const ICC_EVP_CIPHER* type, const unsigned char* key, const unsigned char* iv);
   int (ICC_LINK* ICC_EVP_DecryptUpdate)(ICC_CTX* pcb, ICC_EVP_CIPHER_CTX* ctx, unsigned char* out, int* outl, const unsigned char* in, int inl);
   int (ICC_LINK* ICC_EVP_DecryptFinal)(ICC_CTX* pcb, ICC_EVP_CIPHER_CTX* ctx, unsigned char* out, int* outl);
   int (ICC_LINK* ICC_EVP_OpenInit)(ICC_CTX* pcb, ICC_EVP_CIPHER_CTX* ctx, const ICC_EVP_CIPHER* type, unsigned char* ek, int ekl, unsigned char* iv, ICC_EVP_PKEY* priv);
   int (ICC_LINK* ICC_EVP_OpenFinal)(ICC_CTX* pcb, ICC_EVP_CIPHER_CTX* ctx, unsigned char* out, int* outl);
   int (ICC_LINK* ICC_EVP_SealInit)(ICC_CTX* pcb, ICC_EVP_CIPHER_CTX* ctx, const ICC_EVP_CIPHER* type, unsigned char** ek, int* ekl, unsigned char* iv, ICC_EVP_PKEY** pubk, int npubk);
   int (ICC_LINK* ICC_EVP_SealFinal)(ICC_CTX* pcb, ICC_EVP_CIPHER_CTX* ctx, unsigned char* out, int* outl);
   int (ICC_LINK* ICC_EVP_SignFinal)(ICC_CTX* pcb, ICC_EVP_MD_CTX* ctx, unsigned char* md, unsigned int* s, ICC_EVP_PKEY* pkey);
   int (ICC_LINK* ICC_EVP_VerifyFinal)(ICC_CTX* pcb, ICC_EVP_MD_CTX* ctx, unsigned char* sigbuf, unsigned int siglen, ICC_EVP_PKEY* pkey);
   ICC_EVP_ENCODE_CTX* (ICC_LINK* ICC_EVP_ENCODE_CTX_new)(ICC_CTX* pcb);
   int (ICC_LINK* ICC_EVP_ENCODE_CTX_free)(ICC_CTX* pcb, ICC_EVP_ENCODE_CTX* x);
   void (ICC_LINK* ICC_EVP_EncodeInit)(ICC_CTX* pcb, ICC_EVP_ENCODE_CTX* ctx);
   void (ICC_LINK* ICC_EVP_EncodeUpdate)(ICC_CTX* pcb, ICC_EVP_ENCODE_CTX* ctx, unsigned char* out, int* outl, const unsigned char* in, int inl);
   void (ICC_LINK* ICC_EVP_EncodeFinal)(ICC_CTX* pcb, ICC_EVP_ENCODE_CTX* ctx, unsigned char* out, int* outl);
   void (ICC_LINK* ICC_EVP_DecodeInit)(ICC_CTX* pcb, ICC_EVP_ENCODE_CTX* ctx);
   int (ICC_LINK* ICC_EVP_DecodeUpdate)(ICC_CTX* pcb, ICC_EVP_ENCODE_CTX* ctx, unsigned char* out, int* outl, const unsigned char* in, int inl);
   int (ICC_LINK* ICC_EVP_DecodeFinal)(ICC_CTX* pcb, ICC_EVP_ENCODE_CTX* ctx, unsigned char* out, int* outl);
   int (ICC_LINK* ICC_EVP_DigestInit)(ICC_CTX* pcb, ICC_EVP_MD_CTX* ctx, const ICC_EVP_MD* type);
   int (ICC_LINK* ICC_EVP_DigestUpdate)(ICC_CTX* pcb, ICC_EVP_MD_CTX* ctx, const void* d, unsigned int cnt);
   int (ICC_LINK* ICC_EVP_DigestFinal)(ICC_CTX* pcb, ICC_EVP_MD_CTX* ctx, unsigned char* md, unsigned int* s);

   int (ICC_LINK* ICC_EVP_DigestSignInit)(ICC_CTX* pcb, ICC_EVP_MD_CTX* ctx, ICC_EVP_PKEY_CTX** pctx, const ICC_EVP_MD* type, void* e, ICC_EVP_PKEY* pkey);
   int (ICC_LINK* ICC_EVP_DigestSignFinal)(ICC_CTX* pcb, ICC_EVP_MD_CTX* ctx, unsigned char* sigret, size_t* siglen);
   int (ICC_LINK* ICC_EVP_DigestVerifyInit)(ICC_CTX* pcb, ICC_EVP_MD_CTX* ctx, ICC_EVP_PKEY_CTX** pctx, const ICC_EVP_MD* type, void* e, ICC_EVP_PKEY* pkey);
   int (ICC_LINK* ICC_EVP_DigestVerifyFinal)(ICC_CTX* pcb, ICC_EVP_MD_CTX* ctx, unsigned char* sig, size_t siglen);
   int (ICC_LINK* ICC_EVP_PKEY_id)(ICC_CTX* pcb, const ICC_EVP_PKEY* pkey);

   ICC_EVP_CIPHER_CTX* (ICC_LINK* ICC_EVP_CIPHER_CTX_new)(ICC_CTX* pcb);
   int (ICC_LINK* ICC_EVP_CIPHER_CTX_free)(ICC_CTX* pcb, ICC_EVP_CIPHER_CTX* x);
   void (ICC_LINK* ICC_EVP_CIPHER_CTX_init)(ICC_CTX* pcb, ICC_EVP_CIPHER_CTX* a);
   int (ICC_LINK* ICC_EVP_CIPHER_CTX_cleanup)(ICC_CTX* pcb, ICC_EVP_CIPHER_CTX* a);
   int (ICC_LINK* ICC_EVP_CIPHER_CTX_set_key_length)(ICC_CTX* pcb, ICC_EVP_CIPHER_CTX* x, int keylen);
   int (ICC_LINK* ICC_EVP_CIPHER_CTX_set_padding)(ICC_CTX* pcb, ICC_EVP_CIPHER_CTX* c, int pad);
   int (ICC_LINK* ICC_EVP_CIPHER_block_size)(ICC_CTX* pcb, const ICC_EVP_CIPHER* cipher);
   int (ICC_LINK* ICC_EVP_CIPHER_key_length)(ICC_CTX* pcb, const ICC_EVP_CIPHER* cipher);
   int (ICC_LINK* ICC_EVP_CIPHER_iv_length)(ICC_CTX* pcb, const ICC_EVP_CIPHER* cipher);
   int (ICC_LINK* ICC_EVP_CIPHER_type)(ICC_CTX* pcb, const ICC_EVP_CIPHER* cipher);
   const ICC_EVP_CIPHER* (ICC_LINK* ICC_EVP_CIPHER_CTX_cipher)(ICC_CTX* pcb, const ICC_EVP_CIPHER_CTX* e);

   ICC_HMAC_CTX* (ICC_LINK* ICC_HMAC_CTX_new)(ICC_CTX* pcb);
   void (ICC_LINK* ICC_HMAC_CTX_free)(ICC_CTX* pcb, ICC_HMAC_CTX* ctx);
   int (ICC_LINK* ICC_HMAC_Init)(ICC_CTX* pcb, ICC_HMAC_CTX* ctx, const void* key, int key_len, const ICC_EVP_MD* md);
   int (ICC_LINK* ICC_HMAC_Update)(ICC_CTX* pcb, ICC_HMAC_CTX* ctx, const unsigned char* data, int len);
   int (ICC_LINK* ICC_HMAC_Final)(ICC_CTX* pcb, ICC_HMAC_CTX* ctx, unsigned char* md, unsigned int* len);

   ICC_CMAC_CTX* (ICC_LINK* ICC_CMAC_CTX_new)(ICC_CTX* pcb);
   void (ICC_LINK* ICC_CMAC_CTX_free)(ICC_CTX* pcb, ICC_CMAC_CTX* ctx);
   int (ICC_LINK* ICC_CMAC_Init)(ICC_CTX* pcb, ICC_CMAC_CTX* ctx, const ICC_EVP_CIPHER* cipher, unsigned char* key, unsigned int keylen);
   int (ICC_LINK* ICC_CMAC_Update)(ICC_CTX* pcb, ICC_CMAC_CTX* ctx, const unsigned char* data, unsigned int len);
   int (ICC_LINK* ICC_CMAC_Final)(ICC_CTX* pcb, ICC_CMAC_CTX* ctx, unsigned char* md, unsigned int len);

   ICC_DSA* (ICC_LINK* ICC_d2i_DSA_PUBKEY)(ICC_CTX* pcb, ICC_DSA** a, const unsigned char** pp, long length);
   int (ICC_LINK* ICC_i2d_DSA_PUBKEY)(ICC_CTX* pcb, const ICC_DSA* a, unsigned char** pp);

   ICC_ECDSA_SIG* (ICC_LINK* ICC_ECDSA_SIG_new)(ICC_CTX* pcb);
   void (ICC_LINK* ICC_ECDSA_SIG_free)(ICC_CTX* pcb, ICC_ECDSA_SIG* sig);
   int (ICC_LINK* ICC_i2d_ECDSA_SIG)(ICC_CTX* pcb, const ICC_ECDSA_SIG* sig, unsigned char** pp);
   ICC_ECDSA_SIG* (ICC_LINK* ICC_d2i_ECDSA_SIG)(ICC_CTX* pcb, ICC_ECDSA_SIG** sig, const unsigned char** pp, long len);
   int (ICC_LINK* ICC_ECDSA_sign)(ICC_CTX* pcb, int type, const unsigned char* dgst, int dgstlen, unsigned char* sig, unsigned int* siglen, ICC_EC_KEY* eckey);
   int (ICC_LINK* ICC_ECDSA_verify)(ICC_CTX* pcb, int type, const unsigned char* dgst, int dgstlen, const unsigned char* sig, int siglen, ICC_EC_KEY* eckey);
   int (ICC_LINK* ICC_ECDSA_size)(ICC_CTX* pcb, const ICC_EC_KEY* eckey);
   int (ICC_LINK* ICC_ECDSA_sign_setup)(ICC_CTX* pcb, ICC_EC_KEY* eckey, ICC_BN_CTX* ctx_in, ICC_BIGNUM** kinvp, ICC_BIGNUM** rp);
   ICC_ECDSA_SIG* (ICC_LINK* ICC_ECDSA_do_sign_ex)(ICC_CTX* pcb, const unsigned char* dgst, int dlen, const ICC_BIGNUM* kinv, const ICC_BIGNUM* rp, ICC_EC_KEY* eckey);
   int (ICC_LINK* ICC_ECDSA_do_verify)(ICC_CTX* pcb, const unsigned char* dgst, int dgst_len, const ICC_ECDSA_SIG* sig, ICC_EC_KEY* eckey);

   int (ICC_LINK* ICC_EVP_PKEY_set1_EC_KEY)(ICC_CTX* pcb, ICC_EVP_PKEY* pkey, ICC_EC_KEY* key);
   ICC_EC_KEY* (ICC_LINK* ICC_EVP_PKEY_get1_EC_KEY)(ICC_CTX* pcb, ICC_EVP_PKEY* pkey);
   ICC_EC_KEY* (ICC_LINK* ICC_EC_KEY_new_by_curve_name)(ICC_CTX* pcb, int nid);
   ICC_EC_KEY* (ICC_LINK* ICC_EC_KEY_new)(ICC_CTX* pcb);
   void (ICC_LINK* ICC_EC_KEY_free)(ICC_CTX* pcb, ICC_EC_KEY* eckey);
   int (ICC_LINK* ICC_EC_KEY_generate_key)(ICC_CTX* pcb, ICC_EC_KEY* eckey);
   const ICC_EC_GROUP* (ICC_LINK* ICC_EC_KEY_get0_group)(ICC_CTX* pcb, const ICC_EC_KEY* eckey);
   int (ICC_LINK* ICC_EC_METHOD_get_field_type)(ICC_CTX* pcb, const ICC_EC_METHOD* meth);
   const ICC_EC_METHOD* (ICC_LINK* ICC_EC_GROUP_method_of)(ICC_CTX* pcb, const ICC_EC_GROUP* group);
   ICC_EC_POINT* (ICC_LINK* ICC_EC_POINT_new)(ICC_CTX* pcb, const ICC_EC_GROUP* group);
   void (ICC_LINK* ICC_EC_POINT_free)(ICC_CTX* pcb, ICC_EC_POINT* ecpoint);
   int (ICC_LINK* ICC_EC_POINT_get_affine_coordinates_GFp)(ICC_CTX* pcb, const ICC_EC_GROUP* group, const ICC_EC_POINT* point, ICC_BIGNUM* x, ICC_BIGNUM* y, ICC_BN_CTX* ctx);
   int (ICC_LINK* ICC_EC_POINT_set_affine_coordinates_GFp)(ICC_CTX* pcb, const ICC_EC_GROUP* group, ICC_EC_POINT* point, const ICC_BIGNUM* x, const ICC_BIGNUM* y, ICC_BN_CTX* bn_ctx);
   int (ICC_LINK* ICC_EC_POINT_get_affine_coordinates_GF2m)(ICC_CTX* pcb, const ICC_EC_GROUP* group, const ICC_EC_POINT* point, ICC_BIGNUM* x, ICC_BIGNUM* y, ICC_BN_CTX* ctx);
   int (ICC_LINK* ICC_EC_POINT_set_affine_coordinates_GF2m)(ICC_CTX* pcb, const ICC_EC_GROUP* group, ICC_EC_POINT* point, const ICC_BIGNUM* x, const ICC_BIGNUM* y, ICC_BN_CTX* bn_ctx);
   const ICC_EC_POINT* (ICC_LINK* ICC_EC_KEY_get0_public_key)(ICC_CTX* pcb, const ICC_EC_KEY* eckey);
   int (ICC_LINK* ICC_EC_KEY_set_public_key)(ICC_CTX* pcb, ICC_EC_KEY* eckey, const ICC_EC_POINT* pub_key);
   const ICC_BIGNUM* (ICC_LINK* ICC_EC_KEY_get0_private_key)(ICC_CTX* pcb, const ICC_EC_KEY* eckey);
   int (ICC_LINK* ICC_EC_KEY_set_private_key)(ICC_CTX* pcb, ICC_EC_KEY* eckey, const ICC_BIGNUM* priv_key);
   int (ICC_LINK* ICC_ECDH_compute_key)(ICC_CTX* pcb, void* out, size_t outlen, const ICC_EC_POINT* pub_key, ICC_EC_KEY* eckey, void* (*KD_FUNC)(const void* in, size_t inlen, void* out, size_t* outlen));
   ICC_EC_KEY* (ICC_LINK* ICC_d2i_ECPrivateKey)(ICC_CTX* pcb, ICC_EC_KEY** eckey, const unsigned char** in, long len);
   int (ICC_LINK* ICC_i2d_ECPrivateKey)(ICC_CTX* pcb, ICC_EC_KEY* eckey, unsigned char** out);
   ICC_EC_KEY* (ICC_LINK* ICC_d2i_ECParameters)(ICC_CTX* pcb, ICC_EC_KEY** eckey, const unsigned char** in, long len);
   int (ICC_LINK* ICC_i2d_ECParameters)(ICC_CTX* pcb, ICC_EC_KEY* eckey, unsigned char** out);
   int (ICC_LINK* ICC_EC_POINT_is_on_curve)(ICC_CTX* pcb, const ICC_EC_GROUP* group, const ICC_EC_POINT* point, ICC_BN_CTX* ctx);
   int (ICC_LINK* ICC_EC_POINT_is_at_infinity)(ICC_CTX* pcb, const ICC_EC_GROUP* group, const ICC_EC_POINT* point);
   int (ICC_LINK* ICC_EC_KEY_check_key)(ICC_CTX* pcb, const ICC_EC_KEY* eckey);
   int (ICC_LINK* ICC_EC_POINT_mul)(ICC_CTX* pcb, const ICC_EC_GROUP* group, ICC_EC_POINT* r, const ICC_BIGNUM* g_scalar, const ICC_EC_POINT* point, const ICC_BIGNUM* p_scalar, ICC_BN_CTX* bn_ctx);
   int (ICC_LINK* ICC_EC_GROUP_get_order)(ICC_CTX* pcb, const ICC_EC_GROUP* group, ICC_BIGNUM* order, ICC_BN_CTX* bn_ctx);
   ICC_EC_POINT* (ICC_LINK* ICC_EC_POINT_dup)(ICC_CTX* pcb, const ICC_EC_POINT* point, const ICC_EC_GROUP* group);
   ICC_X509_ALGOR* (ICC_LINK* ICC_PKCS5_pbe_set)(ICC_CTX* pcb, int alg, int iter, const unsigned char* salt, int saltlen);
   ICC_X509_ALGOR* (ICC_LINK* ICC_PKCS5_pbe2_set)(ICC_CTX* pcb, const ICC_EVP_CIPHER* cipher, int iter, unsigned char* salt, int saltlen);
   ICC_X509_ALGOR* (ICC_LINK* ICC_PKCS5_pbe2_set_iv)(ICC_CTX* pcb, const ICC_EVP_CIPHER* cipher, int iter, unsigned char* salt, int saltlen, unsigned char* aiv, int prf_nid);
   unsigned char* (ICC_LINK* ICC_PKCS12_pbe_crypt)(ICC_CTX* pcb, ICC_X509_ALGOR* algor, const char* pass, int passlen, unsigned char* in, int inlen, unsigned char** data, int* datalen, int en_de);
   void (ICC_LINK* ICC_X509_ALGOR_free)(ICC_CTX* pcb, ICC_X509_ALGOR* alg);
   int (ICC_LINK* ICC_OBJ_txt2nid)(ICC_CTX* pcb, const char* text);
   int (ICC_LINK* ICC_EVP_EncodeBlock)(ICC_CTX* pcb, unsigned char* to, const unsigned char* from, int n);
   int (ICC_LINK* ICC_EVP_DecodeBlock)(ICC_CTX* pcb, unsigned char* to, const unsigned char* from, int n);

   int (ICC_LINK* ICC_EVP_PKEY_set1_RSA)(ICC_CTX* pcb, ICC_EVP_PKEY* pkey, ICC_RSA* key);
   ICC_RSA* (ICC_LINK* ICC_EVP_PKEY_get1_RSA)(ICC_CTX* pcb, ICC_EVP_PKEY* pkey);
   int (ICC_LINK* ICC_i2d_PrivateKey)(ICC_CTX* pcb, ICC_EVP_PKEY* a, unsigned char** pp);
   int (ICC_LINK* ICC_i2d_PublicKey)(ICC_CTX* pcb, ICC_EVP_PKEY* a, unsigned char** pp);
   ICC_EVP_PKEY* (ICC_LINK* ICC_d2i_PrivateKey)(ICC_CTX* pcb, int type, ICC_EVP_PKEY** a, const unsigned char** pp, long length);
   ICC_EVP_PKEY* (ICC_LINK* ICC_d2i_PublicKey)(ICC_CTX* pcb, int type, ICC_EVP_PKEY** a, const unsigned char** pp, long length);
   ICC_EVP_PKEY* (ICC_LINK* ICC_d2i_PUBKEY)(ICC_CTX* pcb, ICC_EVP_PKEY** a, const unsigned char** pp, long length);
   int (ICC_LINK* ICC_i2d_PUBKEY)(ICC_CTX* pcb, ICC_EVP_PKEY* a, unsigned char** pp);
   int (ICC_LINK* ICC_i2d_RSAPrivateKey)(ICC_CTX* pcb, ICC_RSA* a, unsigned char** pp);
   int (ICC_LINK* ICC_i2d_RSAPublicKey)(ICC_CTX* pcb, ICC_RSA* a, unsigned char** pp);
   ICC_RSA* (ICC_LINK* ICC_RSA_new)(ICC_CTX* pcb);
   ICC_RSA* (ICC_LINK* ICC_RSA_generate_key)(ICC_CTX* pcb, int bits, unsigned long e, void (*callback)(int, int, void*), void* cb_arg);
   int (ICC_LINK* ICC_RSA_check_key)(ICC_CTX* pcb, ICC_RSA* r);
   void (ICC_LINK* ICC_RSA_free)(ICC_CTX* pcb, ICC_RSA* r);
   int (ICC_LINK* ICC_RSA_private_encrypt)(ICC_CTX* pcb, int flen, const unsigned char* from, unsigned char* to, ICC_RSA* rsa, int padding);
   int (ICC_LINK* ICC_RSA_private_decrypt)(ICC_CTX* pcb, int flen, const unsigned char* from, unsigned char* to, ICC_RSA* rsa, int padding);
   int (ICC_LINK* ICC_RSA_public_encrypt)(ICC_CTX* pcb, int flen, const unsigned char* from, unsigned char* to, ICC_RSA* rsa, int padding);
   int (ICC_LINK* ICC_RSA_public_decrypt)(ICC_CTX* pcb, int flen, const unsigned char* from, unsigned char* to, ICC_RSA* rsa, int padding);
   int (ICC_LINK* ICC_RSA_sign)(ICC_CTX* pcb, int type, const unsigned char* dgst, int dlen, unsigned char* sig, unsigned int* siglen, ICC_RSA* rsa);
   int (ICC_LINK* ICC_RSA_verify)(ICC_CTX* pcb, int type, const unsigned char* dgst, int dgst_len, const unsigned char* sigbuf, int siglen, ICC_RSA* rsa);
   void (ICC_LINK* ICC_RSA_blinding_off)(ICC_CTX* pcb, ICC_RSA* rsa);
   int (ICC_LINK* ICC_RSA_size)(ICC_CTX* pcb, ICC_RSA* rsa);

   ICC_BN_CTX* (ICC_LINK* ICC_BN_CTX_new)(ICC_CTX* pcb);
   void (ICC_LINK* ICC_BN_CTX_free)(ICC_CTX* pcb, ICC_BN_CTX* bn_ctx);
   int (ICC_LINK* ICC_BN_mod_exp)(ICC_CTX* pcb, ICC_BIGNUM* r, ICC_BIGNUM* a, ICC_BIGNUM* p, ICC_BIGNUM* m, ICC_BN_CTX* ctx);
   int (ICC_LINK* ICC_BN_div)(ICC_CTX* pcb, ICC_BIGNUM* dv, ICC_BIGNUM* rem, const ICC_BIGNUM* a, const ICC_BIGNUM* d, ICC_BN_CTX* ctx);
   int (ICC_LINK* ICC_BN_cmp)(ICC_CTX* pcb, ICC_BIGNUM* a, ICC_BIGNUM* b);
   int (ICC_LINK* ICC_BN_add)(ICC_CTX* pcb, ICC_BIGNUM* r, ICC_BIGNUM* a, ICC_BIGNUM* b);
   int (ICC_LINK* ICC_BN_sub)(ICC_CTX* pcb, ICC_BIGNUM* r, ICC_BIGNUM* a, ICC_BIGNUM* b);
   int (ICC_LINK* ICC_BN_mod_mul)(ICC_CTX* pcb, ICC_BIGNUM* r, ICC_BIGNUM* a, ICC_BIGNUM* b, const ICC_BIGNUM* m, ICC_BN_CTX* ctx);
   int (ICC_LINK* ICC_BN_is_prime_fasttest_ex)(ICC_CTX* pcb, const ICC_BIGNUM* a, int checks, ICC_BN_CTX* ctx_passed, int do_trial_division, void* cb);
   int (ICC_LINK* ICC_BN_X931_generate_Xpq)(ICC_CTX* pcb, ICC_BIGNUM* Xp, ICC_BIGNUM* Xq, int bits, ICC_BN_CTX* ctx);
   int (ICC_LINK* ICC_BN_X931_generate_prime_ex)(ICC_CTX* pcb, ICC_BIGNUM* p, ICC_BIGNUM* p1, ICC_BIGNUM* p2, ICC_BIGNUM* Xp1, ICC_BIGNUM* Xp2, const ICC_BIGNUM* Xp, const ICC_BIGNUM* e, ICC_BN_CTX* ctx, void* cb);
   const ICC_BIGNUM* (ICC_LINK* ICC_BN_value_one)(ICC_CTX* pcb);
   int (ICC_LINK* ICC_BN_set_word)(ICC_CTX* pcb, ICC_BIGNUM* a, unsigned long w);
   unsigned long (ICC_LINK* ICC_BN_get_word)(ICC_CTX* pcb, ICC_BIGNUM* a);
   void (ICC_LINK* ICC_ASN1_OBJECT_free)(ICC_CTX* pcb, ICC_ASN1_OBJECT* a);
   int (ICC_LINK* ICC_OBJ_obj2txt)(ICC_CTX* pcb, char* buf, int buf_len, const ICC_ASN1_OBJECT* a, int no_name);

   int (ICC_LINK* ICC_EVP_CIPHER_CTX_ctrl)(ICC_CTX* pcb, ICC_EVP_CIPHER_CTX* ctx, int type, int arg, void* ptr);

   ICC_EVP_PKEY_CTX* (ICC_LINK* ICC_EVP_PKEY_CTX_new_from_name)(ICC_CTX* pcb, void* lib, const char* alg, const char * provider);
   ICC_EVP_PKEY_CTX* (ICC_LINK* ICC_EVP_PKEY_CTX_new_from_pkey)(ICC_CTX* pcb, void* lib, ICC_EVP_PKEY* pkey, const char* propquery);
   ICC_EVP_PKEY_CTX* (ICC_LINK* ICC_EVP_PKEY_CTX_new_id)(ICC_CTX* pcb, int id, void* e);
   int (ICC_LINK* ICC_PKCS5_PBKDF2_HMAC)(ICC_CTX* pcb, const char* pass, int passlen, const unsigned char* salt, int saltlen, int iters, const ICC_EVP_MD* digest, int keylen, unsigned char* out);
   void (ICC_LINK* ICC_CRYPTO_free)(ICC_CTX* pcb, void* str);
   int (ICC_LINK* ICC_EC_GROUP_get_degree)(ICC_CTX* pcb, const ICC_EC_GROUP* group);
   int (ICC_LINK* ICC_EC_GROUP_get_curve_GFp)(ICC_CTX* pcb, const ICC_EC_GROUP* group, ICC_BIGNUM* p, ICC_BIGNUM* a, ICC_BIGNUM* b, ICC_BN_CTX* ctx);
   int (ICC_LINK* ICC_i2o_ECPublicKey)(ICC_CTX* pcb, ICC_EC_KEY* a, unsigned char** out);
   ICC_EC_KEY* (ICC_LINK* ICC_o2i_ECPublicKey)(ICC_CTX* pcb, ICC_EC_KEY** a, const unsigned char** in, long len);

   ICC_EVP_PKEY* (ICC_LINK* ICC_EVP_PKCS82PKEY)(ICC_CTX* pcb, ICC_PKCS8_PRIV_KEY_INFO* p8);
   ICC_PKCS8_PRIV_KEY_INFO* (ICC_LINK* ICC_EVP_PKEY2PKCS8)(ICC_CTX* pcb, ICC_EVP_PKEY* pkey);
   void (ICC_LINK* ICC_PKCS8_PRIV_KEY_INFO_free)(ICC_CTX* pcb, ICC_PKCS8_PRIV_KEY_INFO* p8);
   ICC_PKCS8_PRIV_KEY_INFO* (ICC_LINK* ICC_d2i_PKCS8_PRIV_KEY_INFO)(ICC_CTX* pcb, ICC_PKCS8_PRIV_KEY_INFO* p8, const unsigned char** pp, long length);
   int (ICC_LINK* ICC_i2d_PKCS8_PRIV_KEY_INFO)(ICC_CTX* pcb, ICC_PKCS8_PRIV_KEY_INFO* p8, unsigned char** pp);
   int (ICC_LINK* ICC_i2d_ECPKParameters)(ICC_CTX* pcb, const ICC_EC_GROUP* group, unsigned char** out);
   ICC_EC_GROUP* (ICC_LINK* ICC_d2i_ECPKParameters)(ICC_CTX* pcb, ICC_EC_GROUP** groupP, const unsigned char** in, long len);
   int (ICC_LINK* ICC_EC_GROUP_get_curve_GF2m)(ICC_CTX* pcb, const ICC_EC_GROUP* group, ICC_BIGNUM* p, ICC_BIGNUM* a, ICC_BIGNUM* b, ICC_BN_CTX* ctx);
   const ICC_EC_POINT* (ICC_LINK* ICC_EC_GROUP_get0_generator)(ICC_CTX* pcb, ICC_EC_GROUP* group);
   void (ICC_LINK* ICC_EC_GROUP_free)(ICC_CTX* pcb, ICC_EC_GROUP* group);
   int (ICC_LINK* ICC_EC_KEY_set_group)(ICC_CTX* pcb, ICC_EC_KEY* key, const ICC_EC_GROUP* group);
   ICC_EC_KEY* (ICC_LINK* ICC_EC_KEY_dup)(ICC_CTX* pcb, const ICC_EC_KEY* ec);
   int (ICC_LINK* ICC_EVP_PKEY_size)(ICC_CTX* pcb, const ICC_EVP_PKEY* pkey);
   ICC_EVP_PKEY_CTX* (ICC_LINK* ICC_EVP_PKEY_CTX_new)(ICC_CTX* pcb, ICC_EVP_PKEY* pkey, void* e);
   void (ICC_LINK* ICC_EVP_PKEY_CTX_free)(ICC_CTX* pcb, ICC_EVP_PKEY_CTX* pkey_ctx);
   int (ICC_LINK* ICC_DH_check_pub_key)(ICC_CTX* pcb, const ICC_DH* dh, const ICC_BIGNUM* pub_key, int* codes);
   int (ICC_LINK* ICC_EVP_BytesToKey)(ICC_CTX* pcb, const ICC_EVP_CIPHER* type, const ICC_EVP_MD* md, const unsigned char* salt, const unsigned char* data, int datal, int count, unsigned char* key, unsigned char* iv);
   ICC_ASN1_OBJECT* (ICC_LINK* ICC_OBJ_txt2obj)(ICC_CTX* pcb, const char* s, int no_name);
   int (ICC_LINK* ICC_EC_GROUP_get_curve_name)(ICC_CTX* pcb, const ICC_EC_GROUP* group);
   int (ICC_LINK* ICC_EVP_CIPHER_flags)(ICC_CTX* pcb, ICC_EVP_CIPHER* e);
   int (ICC_LINK* ICC_EC_POINT_oct2point)(ICC_CTX* pcb, const ICC_EC_GROUP* group, ICC_EC_POINT* p, const unsigned char* buf, size_t len, ICC_BN_CTX* ctx);
   size_t (ICC_LINK* ICC_EC_POINT_point2oct)(ICC_CTX* pcb, const ICC_EC_GROUP* group, const ICC_EC_POINT* p, int form, unsigned char* buf, size_t len, ICC_BN_CTX* ctx);
   void (ICC_LINK* ICC_EVP_CIPHER_do_all_sorted)(ICC_CTX* pcb, void (*fn)(const void* ciph, const char* from, const char* to, void* x), void* arg);
   void (ICC_LINK* ICC_EVP_MD_do_all_sorted)(ICC_CTX* pcb, void (*fn)(const void* ciph, const char* from, const char* to, void* x), void* arg);
   size_t (ICC_LINK* ICC_EC_get_builtin_curves)(ICC_CTX* pcb, ICC_EC_builtin_curve* r, size_t nitems);
   int (ICC_LINK* ICC_EVP_PKEY_generate)(ICC_CTX* pcb, ICC_EVP_PKEY_CTX* ctx, ICC_EVP_PKEY** ppkey);
   int (ICC_LINK* ICC_EVP_PKEY_keygen_init)(ICC_CTX* pcb, ICC_EVP_PKEY_CTX* ctx);
   int (ICC_LINK* ICC_EVP_PKEY_keygen)(ICC_CTX* pcb, ICC_EVP_PKEY_CTX* ctx, ICC_EVP_PKEY** ppkey);
   int (ICC_LINK* ICC_EVP_PKEY_paramgen_init)(ICC_CTX* pcb, ICC_EVP_PKEY_CTX* ctx);
   int (ICC_LINK* ICC_EVP_PKEY_paramgen)(ICC_CTX* pcb, ICC_EVP_PKEY_CTX* ctx, ICC_EVP_PKEY** ppkey);
   int (ICC_LINK* ICC_EVP_PKEY_derive_init)(ICC_CTX* pcb, ICC_EVP_PKEY_CTX* ctx);
   int (ICC_LINK* ICC_EVP_PKEY_derive_set_peer)(ICC_CTX* pcb, ICC_EVP_PKEY_CTX* ctx, ICC_EVP_PKEY* peer);
   int (ICC_LINK* ICC_EVP_PKEY_derive)(ICC_CTX* pcb, ICC_EVP_PKEY_CTX* ctx, unsigned char* key, size_t* keylen);
   int (ICC_LINK* ICC_EVP_PKEY_set1_tls_encodedpoint)(ICC_CTX* pcb, ICC_EVP_PKEY* pkey, const unsigned char* pt, size_t ptlen);
   size_t (ICC_LINK* ICC_EVP_PKEY_get1_tls_encodedpoint)(ICC_CTX* pcb, ICC_EVP_PKEY* pkey, unsigned char** ppt);
   int (ICC_LINK* ICC_EVP_DigestSign)(ICC_CTX* pcb, ICC_EVP_MD_CTX* md_ctx, unsigned char* sigret, size_t* siglen, const unsigned char* tbs, size_t tbslen);
   int (ICC_LINK* ICC_EVP_DigestVerify)(ICC_CTX* pcb, ICC_EVP_MD_CTX* md_ctx, const unsigned char* sigret, size_t siglen, const unsigned char* tbs, size_t tbslen);
   int (ICC_LINK* ICC_EVP_PKEY_get_raw_private_key)(ICC_CTX* pcb, const ICC_EVP_PKEY* pkey, unsigned char* pri, size_t* len);
   int (ICC_LINK* ICC_EVP_PKEY_get_raw_public_key)(ICC_CTX* pcb, const ICC_EVP_PKEY* pkey, unsigned char* pub, size_t* len);
   ICC_EVP_PKEY* (ICC_LINK* ICC_EVP_PKEY_new_CMAC_key)(ICC_CTX* pcb, void* e, const unsigned char* priv, size_t len, const ICC_EVP_CIPHER* cipher);
   int (ICC_LINK* ICC_EVP_DigestFinalXOF)(ICC_CTX* pcb, ICC_EVP_MD_CTX* mdctx, unsigned char* md, size_t len);
   int (ICC_LINK* ICC_EVP_PKEY_CTX_ctrl)(ICC_CTX* pcb, ICC_EVP_PKEY_CTX* ctx, int keytype, int optype, int cmd, int p1, void* p2);
   int (ICC_LINK* ICC_EVP_PKEY_CTX_ctrl_str)(ICC_CTX* pcb, ICC_EVP_PKEY_CTX* pctx, const char* type, const char* value);
   const unsigned char* (ICC_LINK* ICC_EVP_CIPHER_CTX_iv)(ICC_CTX* pcb, const ICC_EVP_CIPHER_CTX* ctx);
   ICC_EVP_PKEY* (ICC_LINK* ICC_EVP_PKEY_new_raw_private_key)(ICC_CTX* pcb, int type, void* e, const unsigned char* priv, size_t len);
   ICC_EVP_PKEY* (ICC_LINK* ICC_EVP_PKEY_new_raw_public_key)(ICC_CTX* pcb, int type, void* e, const unsigned char* pub, size_t len);

   /* these two are ICC functions that implement the OpenSSL function names hidden by the following two */
   int (ICC_LINK* ICC_EVP_PKEY_encrypt_new)(ICC_CTX* pcb, ICC_EVP_PKEY_CTX* ectx, unsigned char* out, size_t* outlen, const unsigned char* in, size_t inlen);
   int (ICC_LINK* ICC_EVP_PKEY_decrypt_new)(ICC_CTX* pcb, ICC_EVP_PKEY_CTX* ectx, unsigned char* out, size_t* outlen, const unsigned char* in, size_t inlen);
   /* these two are ICC functions that clash with the OpenSSL function names */
   int (ICC_LINK* ICC_EVP_PKEY_decrypt)(ICC_CTX* pcb, unsigned char* dec_key, unsigned char* enc_key, int enc_key_len, ICC_EVP_PKEY* private_key);
   int (ICC_LINK* ICC_EVP_PKEY_encrypt)(ICC_CTX* pcb, unsigned char* enc_key, unsigned char* key, int key_len, ICC_EVP_PKEY* pub_key);

   int (ICC_LINK* ICC_EVP_PKEY_bits)(ICC_CTX* pcb, const ICC_EVP_PKEY* pkey);
   int (ICC_LINK* ICC_EVP_PKEY_decrypt_init)(ICC_CTX* pcb, ICC_EVP_PKEY_CTX* pctx);
   int (ICC_LINK* ICC_EVP_PKEY_encrypt_init)(ICC_CTX* pcb, ICC_EVP_PKEY_CTX* pctx);
   int (ICC_LINK* ICC_EVP_PKEY_verify_recover_init)(ICC_CTX* pcb, ICC_EVP_PKEY_CTX* pctx);
   int (ICC_LINK* ICC_EVP_PKEY_verify_recover)(ICC_CTX* pcb, ICC_EVP_PKEY_CTX* pctx, unsigned char* rout, size_t* routlen, const unsigned char* sig, size_t siglen);
   int (ICC_LINK* ICC_EVP_PKEY_sign_init)(ICC_CTX* pcb, ICC_EVP_PKEY_CTX* pctx);
   int (ICC_LINK* ICC_EVP_PKEY_sign)(ICC_CTX* pcb, ICC_EVP_PKEY_CTX* pctx, unsigned char* sig, size_t* siglen, const unsigned char* tbs, size_t tbslen);
   int (ICC_LINK* ICC_EVP_PKEY_verify_init)(ICC_CTX* pcb, ICC_EVP_PKEY_CTX* pctx);
   int (ICC_LINK* ICC_EVP_PKEY_verify)(ICC_CTX* pcb, ICC_EVP_PKEY_CTX* pctx, const unsigned char* sig, size_t siglen, const unsigned char* tbs, size_t tbslen);

   int (ICC_LINK* ICC_EVP_PKEY_encapsulate_init)(ICC_CTX* pcb, ICC_EVP_PKEY_CTX* ctx, const unsigned char* params);
   int (ICC_LINK* ICC_EVP_PKEY_auth_encapsulate_init)(ICC_CTX* pcb, ICC_EVP_PKEY_CTX* ctx, ICC_EVP_PKEY* authpub, const unsigned char* params);
   int (ICC_LINK* ICC_EVP_PKEY_encapsulate)(ICC_CTX* pcb, ICC_EVP_PKEY_CTX* ctx, unsigned char* wrappedkey, size_t* wrappedkeylen, unsigned char* genkey, size_t* genkeylen);
   int (ICC_LINK* ICC_EVP_PKEY_decapsulate_init)(ICC_CTX* pcb, ICC_EVP_PKEY_CTX* ctx, const unsigned char* params);
   int (ICC_LINK* ICC_EVP_PKEY_auth_decapsulate_init)(ICC_CTX* pcb, ICC_EVP_PKEY_CTX* ctx, ICC_EVP_PKEY* authpub, const unsigned char* params);
   int (ICC_LINK* ICC_EVP_PKEY_decapsulate)(ICC_CTX* pcb, ICC_EVP_PKEY_CTX* ctx, unsigned char* unwrapped, size_t* unwrappedlen, const unsigned char* wrapped, size_t wrappedlen);

   ICC_EVP_KDF* (ICC_LINK* ICC_EVP_KDF_fetch)(ICC_CTX* pcb, void* libctx, const char* algorithm, const char* properties);
   int (ICC_LINK* ICC_EVP_KDF_derive)(ICC_CTX* pcb, ICC_EVP_KDF_CTX* ctx, unsigned char* key, size_t keylen, const ICC_OSSL_PARAM** params);
   ICC_EVP_KDF_CTX* (ICC_LINK* ICC_EVP_KDF_CTX_new)(ICC_CTX* pcb, ICC_EVP_KDF* kdf);
   ICC_OSSL_PARAM* (ICC_LINK* ICC_OSSL_PARAM_construct_uint32)(ICC_CTX* pcb, const char* key, unsigned int* buf);
   ICC_OSSL_PARAM* (ICC_LINK* ICC_OSSL_PARAM_construct_octet_string)(ICC_CTX* pcb, const char* key, void* buf, size_t bsize);
   ICC_OSSL_PARAM* (ICC_LINK* ICC_OSSL_PARAM_construct_end)(ICC_CTX* pcb);

   int      (ICC_LINK* ICC_EVP_PKEY_set1_DSA)(ICC_CTX* pcb, ICC_EVP_PKEY* pkey, ICC_DSA* key);
   ICC_DSA* (ICC_LINK* ICC_EVP_PKEY_get1_DSA)(ICC_CTX* pcb, ICC_EVP_PKEY* pkey);
   ICC_DH* (ICC_LINK* ICC_DSA_dup_DH)(ICC_CTX* pcb, const ICC_DSA* r);
   int (ICC_LINK* ICC_DSA_sign)(ICC_CTX* pcb, int type, const unsigned char* dgst, int dlen, unsigned char* sig, unsigned int* siglen, ICC_DSA* dsa);
   int (ICC_LINK* ICC_DSA_verify)(ICC_CTX* pcb, int type, const unsigned char* dgst, int dgst_len, const unsigned char* sigbuf, int siglen, ICC_DSA* dsa);
   int (ICC_LINK* ICC_DSA_size)(ICC_CTX* pcb, const ICC_DSA* dsa);
   ICC_DSA_SIG* (ICC_LINK* ICC_DSA_SIG_new)(ICC_CTX* pcb);
   void (ICC_LINK* ICC_DSA_SIG_free)(ICC_CTX* pcb, ICC_DSA_SIG* sig);
   ICC_DSA_SIG* (ICC_LINK* ICC_d2i_DSA_SIG)(ICC_CTX* pcb, ICC_DSA_SIG** a, const unsigned char** pp, long length);
   int (ICC_LINK* ICC_i2d_DSA_SIG)(ICC_CTX* pcb, const ICC_DSA_SIG* a, unsigned char** pp);

   ICC_DSA* (ICC_LINK* ICC_DSA_new)(ICC_CTX* pcb);
   void     (ICC_LINK* ICC_DSA_free)(ICC_CTX* pcb, ICC_DSA* dsa);
   int      (ICC_LINK* ICC_DSA_generate_key)(ICC_CTX* pcb, ICC_DSA* a);
   ICC_DSA* (ICC_LINK* ICC_DSA_generate_parameters)(ICC_CTX* pcb, int bits, const unsigned char* seed, int seed_len, int* counter_ret, unsigned long* h_ret, void (*callback)(int, int, void*), void* cb_arg);
   ICC_DSA* (ICC_LINK* ICC_d2i_DSAparams)(ICC_CTX* pcb, ICC_DSA** a, const unsigned char** pp, long length);
   int      (ICC_LINK* ICC_i2d_DSAparams)(ICC_CTX* pcb, const ICC_DSA* a, unsigned char** pp);
   ICC_DSA* (ICC_LINK* ICC_d2i_DSAPublicKey)(ICC_CTX* pcb, ICC_DSA** a, const unsigned char** pp, long length);
   int      (ICC_LINK* ICC_i2d_DSAPublicKey)(ICC_CTX* pcb, const ICC_DSA* a, unsigned char** pp);
   ICC_DSA* (ICC_LINK* ICC_d2i_DSAPrivateKey)(ICC_CTX* pcb, ICC_DSA** a, const unsigned char** pp, long length);
   int      (ICC_LINK* ICC_i2d_DSAPrivateKey)(ICC_CTX* pcb, const ICC_DSA* a, unsigned char** pp);

   int (ICC_LINK* ICC_EVP_PKEY_set1_DH)(ICC_CTX* pcb, ICC_EVP_PKEY* pkey, ICC_DH* key);
   ICC_DH* (ICC_LINK* ICC_EVP_PKEY_get1_DH)(ICC_CTX* pcb, ICC_EVP_PKEY* pkey);

   ICC_DH* (ICC_LINK* ICC_DH_new)(ICC_CTX* pcb);
   int (ICC_LINK* ICC_DH_generate_key)(ICC_CTX* pcb, ICC_DH* dh);
   int (ICC_LINK* ICC_DH_check)(ICC_CTX* pcb, const ICC_DH* dh, int* codes);
   void (ICC_LINK* ICC_DH_free)(ICC_CTX* pcb, ICC_DH* dh);
   int (ICC_LINK* ICC_DH_size)(ICC_CTX* pcb, const ICC_DH* dh);
   int (ICC_LINK* ICC_DH_compute_key)(ICC_CTX* pcb, unsigned char* key, ICC_BIGNUM* pub_key, ICC_DH* dh);
   ICC_DH* (ICC_LINK* ICC_DH_generate_parameters)(ICC_CTX* pcb, int prime_len, int generator, void (*callback)(int, int, void*), void* cb_arg);
   const ICC_BIGNUM* (ICC_LINK* ICC_DH_get_PublicKey)(ICC_CTX* pcb, const ICC_DH* dh);
   ICC_DH* (ICC_LINK* ICC_d2i_DHparams)(ICC_CTX* pcb, ICC_DH** a, const unsigned char** pp, long length);
   int (ICC_LINK* ICC_i2d_DHparams)(ICC_CTX* pcb, const ICC_DH* a, unsigned char** pp);
   const ICC_BIGNUM* (ICC_LINK* ICC_DH_get_PrivateKey)(ICC_CTX* pcb, const ICC_DH* dh);
   void (ICC_LINK* ICC_DH_get0_pqg)(ICC_CTX* pcb, const ICC_DH* dh, const ICC_BIGNUM** p, const ICC_BIGNUM** q, const ICC_BIGNUM** g);
   int (ICC_LINK* ICC_DH_set0_pqg)(ICC_CTX* pcb, ICC_DH* dh, ICC_BIGNUM* p, ICC_BIGNUM* q, ICC_BIGNUM* g);
   int (ICC_LINK* ICC_dh_builtin_genparams_with_q)(ICC_CTX* pcb, ICC_DH* dh, int prime_len, int generator, void* cb);

   unsigned long (ICC_LINK* ICC_EVP_CIPHER_CTX_flags)(ICC_CTX* pcb, const ICC_EVP_CIPHER_CTX* ctx);
   void (ICC_LINK* ICC_EVP_CIPHER_CTX_set_flags)(ICC_CTX* pcb, ICC_EVP_CIPHER_CTX* ctx, int flags);
   int (ICC_LINK* ICC_AES_GCM_GenerateIV_NIST)(ICC_CTX* pcb, ICC_AES_GCM_CTX* aes_gcm_ctx, int ivlen, unsigned char* iv);
   int (ICC_LINK* ICC_EVP_CIPHER_CTX_copy)(ICC_CTX* pcb, ICC_EVP_CIPHER_CTX* out, const ICC_EVP_CIPHER_CTX* in);

   int (ICC_LINK* ICC_AES_GCM_CTX_ctrl)(ICC_CTX* pcb, ICC_AES_GCM_CTX* aes_gcm_ctx, int mode, int accel, void* ptr);
   ICC_AES_GCM_CTX* (ICC_LINK* ICC_AES_GCM_CTX_new)(ICC_CTX* pcb);
   void (ICC_LINK* ICC_AES_GCM_CTX_free)(ICC_CTX* pcb, ICC_AES_GCM_CTX* aes_gcm_ctx);
   int (ICC_LINK* ICC_AES_GCM_Init)(ICC_CTX* pcb, ICC_AES_GCM_CTX* aes_gcm_ctx, unsigned char* iv, unsigned long ivlen, unsigned char* key, unsigned int keylen);
   int (ICC_LINK* ICC_AES_GCM_EncryptUpdate)(ICC_CTX* pcb, ICC_AES_GCM_CTX* aes_gcm_ctx, unsigned char* aad, unsigned long aadlen, unsigned char* data, unsigned long datalen, unsigned char* out, unsigned long* outlen);
   int (ICC_LINK* ICC_AES_GCM_DecryptUpdate)(ICC_CTX* pcb, ICC_AES_GCM_CTX* aes_gcm_ctx, unsigned char* aad, unsigned long aadlen, unsigned char* data, unsigned long datalen, unsigned char* out, unsigned long* outlen);
   int (ICC_LINK* ICC_AES_GCM_EncryptFinal)(ICC_CTX* pcb, ICC_AES_GCM_CTX* aes_gcm_ctx, unsigned char* out, unsigned long* outlen, unsigned char* hash);
   int (ICC_LINK* ICC_AES_GCM_DecryptFinal)(ICC_CTX* pcb, ICC_AES_GCM_CTX* aes_gcm_ctx, unsigned char* out, unsigned long* outlen, unsigned char* hash, unsigned int hlen);
   int (ICC_LINK* ICC_AES_GCM_GenerateIV)(ICC_CTX* pcb, ICC_AES_GCM_CTX* aes_gcm_ctx, unsigned char* out);
   void (ICC_LINK* ICC_GHASH)(ICC_CTX* pcb, ICC_AES_GCM_CTX* gcm_ctx, unsigned char* H, unsigned char* Hash, unsigned char* data, unsigned long datalen);

   int (ICC_LINK* ICC_AES_CCM_Encrypt)(ICC_CTX* pcb, unsigned char* nonce, unsigned int nlen, unsigned char* key, unsigned int keylen, unsigned char* aad, unsigned long aadlen, unsigned char* data, unsigned long datalen, unsigned char* out, unsigned long* outlen, unsigned int taglen);
   int (ICC_LINK* ICC_AES_CCM_Decrypt)(ICC_CTX* pcb, unsigned char* nonce, unsigned int nlen, unsigned char* key, unsigned int keylen, unsigned char* aad, unsigned long aadlen, unsigned char* data, unsigned long datalen, unsigned char* out, unsigned long* outlen, unsigned int taglen);

   int (ICC_LINK* ICC_RSA_FixEncodingZeros)(ICC_CTX* pcb, ICC_RSA* pri, const ICC_RSA* pub, int allowDisableBlinding);

   const ICC_KDF* (ICC_LINK* ICC_SP800_108_get_KDFbyname)(ICC_CTX* pcb, char* kdfname);
   int (ICC_LINK* ICC_SP800_108_KDF)(ICC_CTX* pcb, const ICC_KDF* xctx, unsigned char* Ki, unsigned int Kilen, unsigned char* Label, unsigned int Llen, unsigned char* Context, unsigned int Clen, unsigned char* K0, unsigned int L);
   int (ICC_LINK* ICC_RSA_X931_derive_ex)(ICC_CTX* pcb, ICC_RSA* rsa, ICC_BIGNUM* p1, ICC_BIGNUM* p2, ICC_BIGNUM* q1, ICC_BIGNUM* q2, const ICC_BIGNUM* Xp1, const ICC_BIGNUM* Xp2, const ICC_BIGNUM* Xp, const ICC_BIGNUM* Xq1, const ICC_BIGNUM* Xq2, const ICC_BIGNUM* Xq, const ICC_BIGNUM* e, void* cb);
   void (ICC_LINK* ICC_EC_GROUP_set_asn1_flag)(ICC_CTX* pcb, ICC_EC_GROUP* group, int flag);
   int (ICC_LINK* ICC_SP800_38F_KW)(ICC_CTX* pcb, unsigned char* in, int inl, unsigned char* out, int* outl, unsigned char* key, int kl, unsigned int flags);

   unsigned char* (ICC_LINK* ICC_HKDF)(ICC_CTX* pcb, const ICC_EVP_MD* evp_md, const unsigned char* salt, size_t salt_len, const unsigned char* key, size_t key_len, const unsigned char* info, size_t info_len, unsigned char* okm, size_t okm_len);
   unsigned char* (ICC_LINK* ICC_HKDF_Extract)(ICC_CTX* pcb, const ICC_EVP_MD* evp_md, const unsigned char* salt, size_t salt_len, const unsigned char* key, size_t key_len, unsigned char* prk, size_t* prk_len);
   unsigned char* (ICC_LINK* ICC_HKDF_Expand)(ICC_CTX* pcb, const ICC_EVP_MD* evp_md, const unsigned char* prk, size_t prk_len, const unsigned char* info, size_t info_len, unsigned char* okm, size_t okm_len);

   /* Kyber */
   ICC_EVP_PKEY_CTX* (ICC_LINK* ICC_crypto_kem_ctx_new)(ICC_CTX* pcb, int k);
   void (ICC_LINK* ICC_crypto_kem_ctx_free)(ICC_CTX* pcb, ICC_EVP_PKEY_CTX* ctx);
   int (ICC_LINK* ICC_crypto_kem_keypair)(ICC_CTX* pcb, const ICC_EVP_PKEY_CTX* ctx, unsigned char* pkc, unsigned char* skc);
   int (ICC_LINK* ICC_crypto_kem_enc)(ICC_CTX* pcb, const ICC_EVP_PKEY_CTX* ctx, unsigned char*, unsigned char*, const unsigned char*);
   int (ICC_LINK* ICC_crypto_kem_dec)(ICC_CTX* pcb, const ICC_EVP_PKEY_CTX* ctx, unsigned char*, const unsigned char*, const unsigned char*);
   size_t (ICC_LINK* ICC_kem_bytes)(ICC_CTX* pcb, const ICC_EVP_PKEY_CTX* ctx);
   size_t (ICC_LINK* ICC_kem_ciphertext_bytes)(ICC_CTX* pcb, const ICC_EVP_PKEY_CTX* ctx);
   size_t (ICC_LINK* ICC_kem_secretkey_bytes)(ICC_CTX* pcb, const ICC_EVP_PKEY_CTX* ctx);
   size_t (ICC_LINK* ICC_kem_publickey_bytes)(ICC_CTX* pcb, const ICC_EVP_PKEY_CTX* ctx);

   /* Dilithium */
   ICC_EVP_PKEY_CTX* (ICC_LINK* ICC_crypto_sign_ctx_new)(ICC_CTX* pcb, int d);
   void (ICC_LINK* ICC_crypto_sign_ctx_free)(ICC_CTX* pcb, ICC_EVP_PKEY_CTX* ctx);
   int (ICC_LINK* ICC_crypto_sign_keypair)(ICC_CTX* pcb, const ICC_EVP_PKEY_CTX* ctx, unsigned char* pk, unsigned char* sk);
   int (ICC_LINK* ICC_crypto_sign)(ICC_CTX* pcb, const ICC_EVP_PKEY_CTX* ctx, unsigned char*, size_t*, const unsigned char*, size_t, const unsigned char*);
   int (ICC_LINK* ICC_crypto_sign_open)(ICC_CTX* pcb, const ICC_EVP_PKEY_CTX* ctx, unsigned char*, size_t*, const unsigned char*, size_t, const unsigned char*);
   size_t (ICC_LINK* ICC_dil_bytes)(ICC_CTX* pcb, const ICC_EVP_PKEY_CTX* ctx);
   size_t (ICC_LINK* ICC_dil_secretkey_bytes)(ICC_CTX* pcb, const ICC_EVP_PKEY_CTX* ctx);
   size_t (ICC_LINK* ICC_dil_publickey_bytes)(ICC_CTX* pcb, const ICC_EVP_PKEY_CTX* ctx);

   int (ICC_LINK* ICC_argon2_hash)(ICC_CTX* pcb, const uint32_t a, const uint32_t b, const uint32_t c, const void* d, const size_t d_len, const void* e,
      const size_t e_len, void* f, const size_t f_len, char* g, const size_t g_len, int i, const uint32_t h);
   int (ICC_LINK* ICC_argon2_verify)(ICC_CTX* pcb, const char* a, const void* pwd, const size_t pwd_len, int i);
   const char* (ICC_LINK* ICC_argon2_error_message)(ICC_CTX* pcb, int m);
   size_t(ICC_LINK* ICC_argon2_encodedlen)(ICC_CTX* pcb, uint32_t a, uint32_t b, uint32_t c, uint32_t d, uint32_t e, int i);

};

typedef struct ICC_FunctionTable_s ICC_FunctionTable;
typedef ICC_FunctionTable ICC_Function_Table;
#endif
/* Define platform-specific type for library handle */
#ifdef _WIN32
#define LIB_HANDLE HMODULE
#else
#define LIB_HANDLE void*
#endif

/* Load the ICC library and return the library handle */
LIB_HANDLE load_icc_library(const char* libpath);

/* Load ICC functions using the library handle */
int load_icc_functions(LIB_HANDLE lib_handle, ICC_FunctionTable* table);

/* Unload the ICC library and free resources */
int unload_icc_functions(ICC_FunctionTable* table);

#ifdef MAP_ICC_API

/* ICC_FP_NAME must be defined to be the reference to the name table
// e.g.
//ICC_FUNCTION_LIST *global_pFunctionList;
//#define ICC_FP_NAME global_pFunctionList */

#ifndef ICC_FP_NAME
#pragma message("Warning: ICC_FP_NAME undefined, using default")
#define ICC_FP_NAME pfn_Table
#endif

/* this is the dynamic linked APIs that we are using
// add them if they are coming up unresolved

// regexp to convert from above function table to defines
// find: ^[^(]*\([A-Z,_]*\* (?<a>[^)]*).*$
// replace: #define ${a}(...) (ICC_FP_NAME->${a})(__VA_ARGS__) */

#define                   ICC_Init(...) (ICC_FP_NAME->ICC_Init)(__VA_ARGS__)
#if defined(_WIN32)
#define                   ICC_InitW(...) (ICC_FP_NAME->ICC_InitW)(__VA_ARGS__)
#define                   gskiccs_pathW(...) (ICC_FP_NAME->gskiccs_pathW)(__VA_ARGS__)
#endif
#define               gskiccs_path(...) (ICC_FP_NAME->gskiccs_path)(__VA_ARGS__)
#define              gskiccs8_path(...) (ICC_FP_NAME->gskiccs8_path)(__VA_ARGS__)
#define              ICC_GetStatus(...) (ICC_FP_NAME->ICC_GetStatus)(__VA_ARGS__)
#define               ICC_SetValue(...) (ICC_FP_NAME->ICC_SetValue)(__VA_ARGS__)
#define               ICC_GetValue(...) (ICC_FP_NAME->ICC_GetValue)(__VA_ARGS__)
#define                 ICC_Attach(...) (ICC_FP_NAME->ICC_Attach)(__VA_ARGS__)
#define                ICC_Cleanup(...) (ICC_FP_NAME->ICC_Cleanup)(__VA_ARGS__)

#define           ICC_ERR_get_error(...) (ICC_FP_NAME->          ICC_ERR_get_error)(__VA_ARGS__)
#define          ICC_ERR_peek_error(...) (ICC_FP_NAME->         ICC_ERR_peek_error)(__VA_ARGS__)
#define     ICC_ERR_peek_last_error(...) (ICC_FP_NAME->    ICC_ERR_peek_last_error)(__VA_ARGS__)
#define        ICC_ERR_error_string(...) (ICC_FP_NAME->       ICC_ERR_error_string)(__VA_ARGS__)
#define      ICC_ERR_error_string_n(...) (ICC_FP_NAME->     ICC_ERR_error_string_n)(__VA_ARGS__)
#define    ICC_ERR_lib_error_string(...) (ICC_FP_NAME->   ICC_ERR_lib_error_string)(__VA_ARGS__)
#define   ICC_ERR_func_error_string(...) (ICC_FP_NAME->  ICC_ERR_func_error_string)(__VA_ARGS__)
#define ICC_ERR_reason_error_string(...) (ICC_FP_NAME->ICC_ERR_reason_error_string)(__VA_ARGS__)
#define         ICC_ERR_clear_error(...) (ICC_FP_NAME->        ICC_ERR_clear_error)(__VA_ARGS__)
#define        ICC_ERR_remove_state(...) (ICC_FP_NAME->       ICC_ERR_remove_state)(__VA_ARGS__)

#define           ICC_get_RNGbyname(...) (ICC_FP_NAME-> ICC_get_RNGbyname)(__VA_ARGS__)
#define             ICC_RNG_CTX_new(...) (ICC_FP_NAME-> ICC_RNG_CTX_new)(__VA_ARGS__)
#define            ICC_RNG_CTX_Init(...) (ICC_FP_NAME-> ICC_RNG_CTX_Init)(__VA_ARGS__)
#define            ICC_RNG_CTX_free(...) (ICC_FP_NAME-> ICC_RNG_CTX_free)(__VA_ARGS__)
#define ICC_RNG_ReSeed(...) (ICC_FP_NAME->ICC_RNG_ReSeed)(__VA_ARGS__)
#define ICC_RNG_CTX_ctrl(...) (ICC_FP_NAME->ICC_RNG_CTX_ctrl)(__VA_ARGS__)
#define      ICC_GenerateRandomSeed(...) (ICC_FP_NAME-> ICC_GenerateRandomSeed)(__VA_ARGS__)
#define               ICC_RAND_seed(...) (ICC_FP_NAME-> ICC_RAND_seed)(__VA_ARGS__)
#define              ICC_RAND_bytes(...) (ICC_FP_NAME-> ICC_RAND_bytes)(__VA_ARGS__)
#define            ICC_RNG_Generate(...) (ICC_FP_NAME-> ICC_RNG_Generate)(__VA_ARGS__)

#define   ICC_BN_num_bits(...) (ICC_FP_NAME-> ICC_BN_num_bits)(__VA_ARGS__)
#define        ICC_BN_new(...) (ICC_FP_NAME-> ICC_BN_new)(__VA_ARGS__)
#define     ICC_BN_bin2bn(...) (ICC_FP_NAME-> ICC_BN_bin2bn)(__VA_ARGS__)
#define     ICC_BN_bn2bin(...) (ICC_FP_NAME-> ICC_BN_bn2bin)(__VA_ARGS__)
#define ICC_BN_clear_free(...) (ICC_FP_NAME-> ICC_BN_clear_free)(__VA_ARGS__)

#define     ICC_DES_random_key(...) (ICC_FP_NAME-> ICC_DES_random_key)(__VA_ARGS__)
#define ICC_DES_set_odd_parity(...) (ICC_FP_NAME-> ICC_DES_set_odd_parity)(__VA_ARGS__)

#define           ICC_EVP_PKEY_new(...) (ICC_FP_NAME->ICC_EVP_PKEY_new)(__VA_ARGS__)
#define          ICC_EVP_PKEY_free(...) (ICC_FP_NAME->ICC_EVP_PKEY_free)(__VA_ARGS__)
#define           ICC_EVP_PKEY_cmp(...) (ICC_FP_NAME->ICC_EVP_PKEY_cmp)(__VA_ARGS__)
#define   ICC_EVP_get_cipherbyname(...) (ICC_FP_NAME->ICC_EVP_get_cipherbyname)(__VA_ARGS__)
#define   ICC_EVP_get_digestbyname(...) (ICC_FP_NAME->ICC_EVP_get_digestbyname)(__VA_ARGS__)
#define         ICC_EVP_MD_CTX_new(...) (ICC_FP_NAME->ICC_EVP_MD_CTX_new)(__VA_ARGS__)
#define        ICC_EVP_MD_CTX_free(...) (ICC_FP_NAME->ICC_EVP_MD_CTX_free)(__VA_ARGS__)
#define        ICC_EVP_MD_CTX_copy(...) (ICC_FP_NAME->ICC_EVP_MD_CTX_copy)(__VA_ARGS__)
#define        ICC_EVP_MD_CTX_init(...) (ICC_FP_NAME->ICC_EVP_MD_CTX_init)(__VA_ARGS__)
#define     ICC_EVP_MD_CTX_cleanup(...) (ICC_FP_NAME->ICC_EVP_MD_CTX_cleanup)(__VA_ARGS__)
#define            ICC_EVP_MD_size(...) (ICC_FP_NAME->ICC_EVP_MD_size)(__VA_ARGS__)
#define         ICC_EVP_DigestInit(...) (ICC_FP_NAME->ICC_EVP_DigestInit)(__VA_ARGS__)
#define       ICC_EVP_DigestUpdate(...) (ICC_FP_NAME->ICC_EVP_DigestUpdate)(__VA_ARGS__)
#define        ICC_EVP_DigestFinal(...) (ICC_FP_NAME->ICC_EVP_DigestFinal)(__VA_ARGS__)
#define     ICC_EVP_DigestSignInit(...) (ICC_FP_NAME->ICC_EVP_DigestSignInit)(__VA_ARGS__)
#define    ICC_EVP_DigestSignFinal(...) (ICC_FP_NAME->ICC_EVP_DigestSignFinal)(__VA_ARGS__)
#define   ICC_EVP_DigestVerifyInit(...) (ICC_FP_NAME->ICC_EVP_DigestVerifyInit)(__VA_ARGS__)
#define  ICC_EVP_DigestVerifyFinal(...) (ICC_FP_NAME->ICC_EVP_DigestVerifyFinal)(__VA_ARGS__)
#define            ICC_EVP_PKEY_id(...) (ICC_FP_NAME->ICC_EVP_PKEY_id)(__VA_ARGS__)
#define        ICC_EVP_DecryptInit(...) (ICC_FP_NAME->ICC_EVP_DecryptInit)(__VA_ARGS__)
#define      ICC_EVP_DecryptUpdate(...) (ICC_FP_NAME->ICC_EVP_DecryptUpdate)(__VA_ARGS__)
#define       ICC_EVP_DecryptFinal(...) (ICC_FP_NAME->ICC_EVP_DecryptFinal)(__VA_ARGS__)
#define        ICC_EVP_EncryptInit(...) (ICC_FP_NAME->ICC_EVP_EncryptInit)(__VA_ARGS__)
#define      ICC_EVP_EncryptUpdate(...) (ICC_FP_NAME->ICC_EVP_EncryptUpdate)(__VA_ARGS__)
#define       ICC_EVP_EncryptFinal(...) (ICC_FP_NAME->ICC_EVP_EncryptFinal)(__VA_ARGS__)
#define           ICC_EVP_OpenInit(...) (ICC_FP_NAME->ICC_EVP_OpenInit)(__VA_ARGS__)
#define          ICC_EVP_OpenFinal(...) (ICC_FP_NAME->ICC_EVP_OpenFinal)(__VA_ARGS__)
#define           ICC_EVP_SealInit(...) (ICC_FP_NAME->ICC_EVP_SealInit)(__VA_ARGS__)
#define          ICC_EVP_SealFinal(...) (ICC_FP_NAME->ICC_EVP_SealFinal)(__VA_ARGS__)
#define          ICC_EVP_SignFinal(...) (ICC_FP_NAME->ICC_EVP_SignFinal)(__VA_ARGS__)
#define        ICC_EVP_VerifyFinal(...) (ICC_FP_NAME->ICC_EVP_VerifyFinal)(__VA_ARGS__)
#define     ICC_EVP_ENCODE_CTX_new(...) (ICC_FP_NAME->ICC_EVP_ENCODE_CTX_new)(__VA_ARGS__)
#define    ICC_EVP_ENCODE_CTX_free(...) (ICC_FP_NAME->ICC_EVP_ENCODE_CTX_free)(__VA_ARGS__)
#define         ICC_EVP_EncodeInit(...) (ICC_FP_NAME->ICC_EVP_EncodeInit)(__VA_ARGS__)
#define       ICC_EVP_EncodeUpdate(...) (ICC_FP_NAME->ICC_EVP_EncodeUpdate)(__VA_ARGS__)
#define        ICC_EVP_EncodeFinal(...) (ICC_FP_NAME->ICC_EVP_EncodeFinal)(__VA_ARGS__)
#define         ICC_EVP_DecodeInit(...) (ICC_FP_NAME->ICC_EVP_DecodeInit)(__VA_ARGS__)
#define       ICC_EVP_DecodeUpdate(...) (ICC_FP_NAME->ICC_EVP_DecodeUpdate)(__VA_ARGS__)
#define        ICC_EVP_DecodeFinal(...) (ICC_FP_NAME->ICC_EVP_DecodeFinal)(__VA_ARGS__)
#define  ICC_EVP_CIPHER_block_size(...) (ICC_FP_NAME->ICC_EVP_CIPHER_block_size)(__VA_ARGS__)
#define  ICC_EVP_CIPHER_key_length(...) (ICC_FP_NAME->ICC_EVP_CIPHER_key_length)(__VA_ARGS__)
#define   ICC_EVP_CIPHER_iv_length(...) (ICC_FP_NAME->ICC_EVP_CIPHER_iv_length)(__VA_ARGS__)
#define        ICC_EVP_CIPHER_type(...) (ICC_FP_NAME->ICC_EVP_CIPHER_type)(__VA_ARGS__)
#define  ICC_EVP_CIPHER_CTX_cipher(...) (ICC_FP_NAME->ICC_EVP_CIPHER_CTX_cipher)(__VA_ARGS__)
#define     ICC_EVP_CIPHER_CTX_new(...) (ICC_FP_NAME->ICC_EVP_CIPHER_CTX_new)(__VA_ARGS__)
#define    ICC_EVP_CIPHER_CTX_init(...) (ICC_FP_NAME->ICC_EVP_CIPHER_CTX_init)(__VA_ARGS__)
#define    ICC_EVP_CIPHER_CTX_free(...) (ICC_FP_NAME->ICC_EVP_CIPHER_CTX_free)(__VA_ARGS__)
#define ICC_EVP_CIPHER_CTX_cleanup(...) (ICC_FP_NAME->ICC_EVP_CIPHER_CTX_cleanup)(__VA_ARGS__)
#define ICC_EVP_CIPHER_CTX_set_key_length(...) (ICC_FP_NAME->ICC_EVP_CIPHER_CTX_set_key_length)(__VA_ARGS__)
#define ICC_EVP_CIPHER_CTX_set_padding(...) (ICC_FP_NAME->ICC_EVP_CIPHER_CTX_set_padding)(__VA_ARGS__)

#define           ICC_HMAC_CTX_new(...) (ICC_FP_NAME->ICC_HMAC_CTX_new)(__VA_ARGS__)
#define          ICC_HMAC_CTX_free(...) (ICC_FP_NAME->ICC_HMAC_CTX_free)(__VA_ARGS__)
#define              ICC_HMAC_Init(...) (ICC_FP_NAME->ICC_HMAC_Init)(__VA_ARGS__)
#define            ICC_HMAC_Update(...) (ICC_FP_NAME->ICC_HMAC_Update)(__VA_ARGS__)
#define             ICC_HMAC_Final(...) (ICC_FP_NAME->ICC_HMAC_Final)(__VA_ARGS__)

#define           ICC_CMAC_CTX_new(...) (ICC_FP_NAME->ICC_CMAC_CTX_new)(__VA_ARGS__)
#define          ICC_CMAC_CTX_free(...) (ICC_FP_NAME->ICC_CMAC_CTX_free)(__VA_ARGS__)
#define              ICC_CMAC_Init(...) (ICC_FP_NAME->ICC_CMAC_Init)(__VA_ARGS__)
#define            ICC_CMAC_Update(...) (ICC_FP_NAME->ICC_CMAC_Update)(__VA_ARGS__)
#define             ICC_CMAC_Final(...) (ICC_FP_NAME->ICC_CMAC_Final)(__VA_ARGS__)

#define ICC_d2i_DSA_PUBKEY(...) (ICC_FP_NAME->ICC_d2i_DSA_PUBKEY)(__VA_ARGS__)
#define ICC_i2d_DSA_PUBKEY(...) (ICC_FP_NAME->ICC_i2d_DSA_PUBKEY)(__VA_ARGS__)
#define  ICC_ECDSA_SIG_new(...) (ICC_FP_NAME->ICC_ECDSA_SIG_new)(__VA_ARGS__)
#define ICC_ECDSA_SIG_free(...) (ICC_FP_NAME->ICC_ECDSA_SIG_free)(__VA_ARGS__)
#define  ICC_i2d_ECDSA_SIG(...) (ICC_FP_NAME->ICC_i2d_ECDSA_SIG)(__VA_ARGS__)
#define  ICC_d2i_ECDSA_SIG(...) (ICC_FP_NAME->ICC_d2i_ECDSA_SIG)(__VA_ARGS__)
#define     ICC_ECDSA_sign(...) (ICC_FP_NAME->ICC_ECDSA_sign)(__VA_ARGS__)
#define   ICC_ECDSA_verify(...) (ICC_FP_NAME->ICC_ECDSA_verify)(__VA_ARGS__)
#define     ICC_ECDSA_size(...) (ICC_FP_NAME->ICC_ECDSA_size)(__VA_ARGS__)
#define ICC_EVP_PKEY_set1_EC_KEY(...) (ICC_FP_NAME->ICC_EVP_PKEY_set1_EC_KEY)(__VA_ARGS__)
#define ICC_EVP_PKEY_get1_EC_KEY(...) (ICC_FP_NAME->ICC_EVP_PKEY_get1_EC_KEY)(__VA_ARGS__)
#define ICC_EC_KEY_new_by_curve_name(...) (ICC_FP_NAME->ICC_EC_KEY_new_by_curve_name)(__VA_ARGS__)
#define ICC_EC_KEY_new(...) (ICC_FP_NAME->ICC_EC_KEY_new)(__VA_ARGS__)
#define ICC_EC_KEY_free(...) (ICC_FP_NAME->ICC_EC_KEY_free)(__VA_ARGS__)
#define ICC_EC_KEY_generate_key(...) (ICC_FP_NAME->ICC_EC_KEY_generate_key)(__VA_ARGS__)
#define ICC_EC_KEY_get0_group(...) (ICC_FP_NAME->ICC_EC_KEY_get0_group)(__VA_ARGS__)
#define ICC_EC_METHOD_get_field_type(...) (ICC_FP_NAME->ICC_EC_METHOD_get_field_type)(__VA_ARGS__)
#define ICC_EC_GROUP_method_of(...) (ICC_FP_NAME->ICC_EC_GROUP_method_of)(__VA_ARGS__)
#define ICC_EC_POINT_new(...) (ICC_FP_NAME->ICC_EC_POINT_new)(__VA_ARGS__)
#define ICC_EC_POINT_free(...) (ICC_FP_NAME->ICC_EC_POINT_free)(__VA_ARGS__)
#define ICC_EC_POINT_get_affine_coordinates_GFp(...) (ICC_FP_NAME->ICC_EC_POINT_get_affine_coordinates_GFp)(__VA_ARGS__)
#define ICC_EC_POINT_set_affine_coordinates_GFp(...) (ICC_FP_NAME->ICC_EC_POINT_set_affine_coordinates_GFp)(__VA_ARGS__)
#define ICC_EC_POINT_get_affine_coordinates_GF2m(...) (ICC_FP_NAME->ICC_EC_POINT_get_affine_coordinates_GF2m)(__VA_ARGS__)
#define ICC_EC_POINT_set_affine_coordinates_GF2m(...) (ICC_FP_NAME->ICC_EC_POINT_set_affine_coordinates_GF2m)(__VA_ARGS__)
#define ICC_EC_KEY_get0_public_key(...) (ICC_FP_NAME->ICC_EC_KEY_get0_public_key)(__VA_ARGS__)
#define ICC_EC_KEY_set_public_key(...) (ICC_FP_NAME->ICC_EC_KEY_set_public_key)(__VA_ARGS__)
#define ICC_EC_KEY_get0_private_key(...) (ICC_FP_NAME->ICC_EC_KEY_get0_private_key)(__VA_ARGS__)
#define ICC_EC_KEY_set_private_key(...) (ICC_FP_NAME->ICC_EC_KEY_set_private_key)(__VA_ARGS__)
#define ICC_ECDH_compute_key(...) (ICC_FP_NAME->ICC_ECDH_compute_key)(__VA_ARGS__)
#define ICC_d2i_ECPrivateKey(...) (ICC_FP_NAME->ICC_d2i_ECPrivateKey)(__VA_ARGS__)
#define ICC_i2d_ECPrivateKey(...) (ICC_FP_NAME->ICC_i2d_ECPrivateKey)(__VA_ARGS__)
#define ICC_d2i_ECParameters(...) (ICC_FP_NAME->ICC_d2i_ECParameters)(__VA_ARGS__)
#define ICC_i2d_ECParameters(...) (ICC_FP_NAME->ICC_i2d_ECParameters)(__VA_ARGS__)
#define ICC_EC_POINT_is_on_curve(...) (ICC_FP_NAME->ICC_EC_POINT_is_on_curve)(__VA_ARGS__)
#define ICC_EC_POINT_is_at_infinity(...) (ICC_FP_NAME->ICC_EC_POINT_is_at_infinity)(__VA_ARGS__)
#define ICC_EC_KEY_check_key(...) (ICC_FP_NAME->ICC_EC_KEY_check_key)(__VA_ARGS__)
#define ICC_EC_POINT_mul(...) (ICC_FP_NAME->ICC_EC_POINT_mul)(__VA_ARGS__)
#define ICC_EC_GROUP_get_order(...) (ICC_FP_NAME->ICC_EC_GROUP_get_order)(__VA_ARGS__)
#define ICC_EC_POINT_dup(...) (ICC_FP_NAME->ICC_EC_POINT_dup)(__VA_ARGS__)
#define ICC_PKCS5_pbe_set(...) (ICC_FP_NAME->ICC_PKCS5_pbe_set)(__VA_ARGS__)
#define ICC_PKCS5_pbe2_set(...) (ICC_FP_NAME->ICC_PKCS5_pbe2_set)(__VA_ARGS__)
#define ICC_PKCS5_pbe2_set_iv(...) (ICC_FP_NAME->ICC_PKCS5_pbe2_set_iv)(__VA_ARGS__)
#define ICC_PKCS12_pbe_crypt(...) (ICC_FP_NAME->ICC_PKCS12_pbe_crypt)(__VA_ARGS__)
#define ICC_X509_ALGOR_free(...) (ICC_FP_NAME->ICC_X509_ALGOR_free)(__VA_ARGS__)
#define ICC_EVP_EncodeBlock(...) (ICC_FP_NAME->ICC_EVP_EncodeBlock)(__VA_ARGS__)
#define ICC_EVP_DecodeBlock(...) (ICC_FP_NAME->ICC_EVP_DecodeBlock)(__VA_ARGS__)

#define ICC_EVP_PKEY_set1_RSA(...) (ICC_FP_NAME->ICC_EVP_PKEY_set1_RSA)(__VA_ARGS__)
#define ICC_EVP_PKEY_get1_RSA(...) (ICC_FP_NAME->ICC_EVP_PKEY_get1_RSA)(__VA_ARGS__)
#define ICC_i2d_PrivateKey(...) (ICC_FP_NAME->ICC_i2d_PrivateKey)(__VA_ARGS__)
#define ICC_i2d_PublicKey(...) (ICC_FP_NAME->ICC_i2d_PublicKey)(__VA_ARGS__)
#define ICC_d2i_PrivateKey(...) (ICC_FP_NAME->ICC_d2i_PrivateKey)(__VA_ARGS__)
#define ICC_d2i_PublicKey(...) (ICC_FP_NAME->ICC_d2i_PublicKey)(__VA_ARGS__)
#define ICC_i2d_RSAPrivateKey(...) (ICC_FP_NAME->ICC_i2d_RSAPrivateKey)(__VA_ARGS__)
#define ICC_i2d_RSAPublicKey(...) (ICC_FP_NAME->ICC_i2d_RSAPublicKey)(__VA_ARGS__)
#define ICC_d2i_PUBKEY(...) (ICC_FP_NAME->ICC_d2i_PUBKEY)(__VA_ARGS__)
#define ICC_i2d_PUBKEY(...) (ICC_FP_NAME->ICC_i2d_PUBKEY)(__VA_ARGS__)
#define ICC_RSA_new(...) (ICC_FP_NAME->ICC_RSA_new)(__VA_ARGS__)
#define ICC_RSA_generate_key(...) (ICC_FP_NAME->ICC_RSA_generate_key)(__VA_ARGS__)
#define ICC_RSA_check_key(...) (ICC_FP_NAME->ICC_RSA_check_key)(__VA_ARGS__)
#define ICC_RSA_free(...) (ICC_FP_NAME->ICC_RSA_free)(__VA_ARGS__)
#define ICC_RSA_private_encrypt(...) (ICC_FP_NAME->ICC_RSA_private_encrypt)(__VA_ARGS__)
#define ICC_RSA_private_decrypt(...) (ICC_FP_NAME->ICC_RSA_private_decrypt)(__VA_ARGS__)
#define ICC_RSA_public_encrypt(...) (ICC_FP_NAME->ICC_RSA_public_encrypt)(__VA_ARGS__)
#define ICC_RSA_public_decrypt(...) (ICC_FP_NAME->ICC_RSA_public_decrypt)(__VA_ARGS__)
#define ICC_RSA_sign(...) (ICC_FP_NAME->ICC_RSA_sign)(__VA_ARGS__)
#define ICC_RSA_verify(...) (ICC_FP_NAME->ICC_RSA_verify)(__VA_ARGS__)
#define ICC_RSA_blinding_off(...) (ICC_FP_NAME->ICC_RSA_blinding_off)(__VA_ARGS__)
#define ICC_RSA_size(...) (ICC_FP_NAME->ICC_RSA_size)(__VA_ARGS__)
#define ICC_EVP_CIPHER_CTX_ctrl(...) (ICC_FP_NAME->ICC_EVP_CIPHER_CTX_ctrl)(__VA_ARGS__)
#define ICC_OBJ_txt2nid(...) (ICC_FP_NAME->ICC_OBJ_txt2nid)(__VA_ARGS__)
#define ICC_EVP_PKEY_CTX_new_from_name(...) (ICC_FP_NAME->ICC_EVP_PKEY_CTX_new_from_name)(__VA_ARGS__)
#define ICC_EVP_PKEY_CTX_new_from_pkey(...) (ICC_FP_NAME->ICC_EVP_PKEY_CTX_new_from_pkey)(__VA_ARGS__)
#define ICC_CRYPTO_free(...) (ICC_FP_NAME->ICC_CRYPTO_free)(__VA_ARGS__)
#define ICC_i2o_ECPublicKey(...) (ICC_FP_NAME->ICC_i2o_ECPublicKey)(__VA_ARGS__)
#define ICC_o2i_ECPublicKey(...) (ICC_FP_NAME->ICC_o2i_ECPublicKey)(__VA_ARGS__)
#define ICC_EVP_PKCS82PKEY(...) (ICC_FP_NAME->ICC_EVP_PKCS82PKEY)(__VA_ARGS__)
#define ICC_EVP_PKEY2PKCS8(...) (ICC_FP_NAME->ICC_EVP_PKEY2PKCS8)(__VA_ARGS__)
#define ICC_PKCS8_PRIV_KEY_INFO_free(...) (ICC_FP_NAME->ICC_PKCS8_PRIV_KEY_INFO_free)(__VA_ARGS__)
#define ICC_d2i_PKCS8_PRIV_KEY_INFO(...) (ICC_FP_NAME->ICC_d2i_PKCS8_PRIV_KEY_INFO)(__VA_ARGS__)
#define ICC_i2d_PKCS8_PRIV_KEY_INFO(...) (ICC_FP_NAME->ICC_i2d_PKCS8_PRIV_KEY_INFO)(__VA_ARGS__)
#define ICC_i2d_ECPKParameters(...) (ICC_FP_NAME->ICC_i2d_ECPKParameters)(__VA_ARGS__)
#define ICC_d2i_ECPKParameters(...) (ICC_FP_NAME->ICC_d2i_ECPKParameters)(__VA_ARGS__)
#define ICC_EC_GROUP_free(...) (ICC_FP_NAME->ICC_EC_GROUP_free)(__VA_ARGS__)
#define ICC_EC_KEY_set_group(...) (ICC_FP_NAME->ICC_EC_KEY_set_group)(__VA_ARGS__)
#define ICC_EC_KEY_dup(...) (ICC_FP_NAME->ICC_EC_KEY_dup)(__VA_ARGS__)
#define ICC_EVP_PKEY_size(...) (ICC_FP_NAME->ICC_EVP_PKEY_size)(__VA_ARGS__)
#define ICC_EVP_PKEY_CTX_new(...) (ICC_FP_NAME->ICC_EVP_PKEY_CTX_new)(__VA_ARGS__)
#define ICC_EVP_PKEY_CTX_free(...) (ICC_FP_NAME->ICC_EVP_PKEY_CTX_free)(__VA_ARGS__)
#define ICC_EVP_CIPHER_flags(...) (ICC_FP_NAME->ICC_EVP_CIPHER_flags)(__VA_ARGS__)
#define ICC_OBJ_txt2obj(...) (ICC_FP_NAME->ICC_OBJ_txt2obj)(__VA_ARGS__)
#define ICC_EVP_PKEY_generate(...) (ICC_FP_NAME->ICC_EVP_PKEY_generate)(__VA_ARGS__)
#define ICC_EVP_PKEY_keygen_init(...) (ICC_FP_NAME->ICC_EVP_PKEY_keygen_init)(__VA_ARGS__)
#define ICC_EVP_PKEY_keygen(...) (ICC_FP_NAME->ICC_EVP_PKEY_keygen)(__VA_ARGS__)
#define ICC_EVP_PKEY_paramgen_init(...) (ICC_FP_NAME->ICC_EVP_PKEY_paramgen_init)(__VA_ARGS__)
#define ICC_EVP_PKEY_paramgen(...) (ICC_FP_NAME->ICC_EVP_PKEY_paramgen)(__VA_ARGS__)
#define ICC_EVP_PKEY_derive_init(...) (ICC_FP_NAME->ICC_EVP_PKEY_derive_init)(__VA_ARGS__)
#define ICC_EVP_PKEY_derive_set_peer(...) (ICC_FP_NAME->ICC_EVP_PKEY_derive_set_peer)(__VA_ARGS__)
#define ICC_EVP_PKEY_derive(...) (ICC_FP_NAME->ICC_EVP_PKEY_derive)(__VA_ARGS__)
#define ICC_EVP_PKEY_CTX_new_id(...) (ICC_FP_NAME->ICC_EVP_PKEY_CTX_new_id)(__VA_ARGS__)
#define ICC_PKCS5_PBKDF2_HMAC(...) (ICC_FP_NAME->ICC_PKCS5_PBKDF2_HMAC)(__VA_ARGS__)
#define ICC_EVP_PKEY_set1_tls_encodedpoint(...) (ICC_FP_NAME->ICC_EVP_PKEY_set1_tls_encodedpoint)(__VA_ARGS__)
#define ICC_EVP_PKEY_get1_tls_encodedpoint(...) (ICC_FP_NAME->ICC_EVP_PKEY_get1_tls_encodedpoint)(__VA_ARGS__)
#define ICC_EVP_PKEY_get_raw_private_key(...) (ICC_FP_NAME->ICC_EVP_PKEY_get_raw_public_key)(__VA_ARGS__)
#define ICC_EVP_PKEY_get_raw_public_key(...) (ICC_FP_NAME->ICC_EVP_PKEY_get_raw_public_key)(__VA_ARGS__)
#define ICC_EVP_PKEY_CTX_ctrl(...) (ICC_FP_NAME->ICC_EVP_PKEY_CTX_ctrl)(__VA_ARGS__)
#define ICC_EVP_PKEY_CTX_ctrl_str(...) (ICC_FP_NAME->ICC_EVP_PKEY_CTX_ctrl_str)(__VA_ARGS__)
#define ICC_EVP_PKEY_encrypt_new(...) (ICC_FP_NAME->ICC_EVP_PKEY_encrypt_new)(__VA_ARGS__)
#define ICC_EVP_PKEY_decrypt_new(...) (ICC_FP_NAME->ICC_EVP_PKEY_decrypt_new)(__VA_ARGS__)
#define ICC_EVP_PKEY_decrypt(...) (ICC_FP_NAME->ICC_EVP_PKEY_decrypt)(__VA_ARGS__)
#define ICC_EVP_PKEY_encrypt(...) (ICC_FP_NAME->ICC_EVP_PKEY_encrypt)(__VA_ARGS__)
#define ICC_EVP_PKEY_bits(...) (ICC_FP_NAME->ICC_EVP_PKEY_bits)(__VA_ARGS__)
#define ICC_EVP_PKEY_decrypt_init(...) (ICC_FP_NAME->ICC_EVP_PKEY_decrypt_init)(__VA_ARGS__)
#define ICC_EVP_PKEY_encrypt_init(...) (ICC_FP_NAME->ICC_EVP_PKEY_encrypt_init)(__VA_ARGS__)

#define ICC_EVP_PKEY_set1_DSA(...) (ICC_FP_NAME->ICC_EVP_PKEY_set1_DSA)(__VA_ARGS__)
#define ICC_EVP_PKEY_get1_DSA(...) (ICC_FP_NAME->ICC_EVP_PKEY_get1_DSA)(__VA_ARGS__)
#define ICC_DSA_new(...) (ICC_FP_NAME->ICC_DSA_new)(__VA_ARGS__)
#define ICC_DSA_free(...) (ICC_FP_NAME->ICC_DSA_free)(__VA_ARGS__)
#define ICC_DSA_generate_key(...) (ICC_FP_NAME->ICC_DSA_generate_key)(__VA_ARGS__)
#define ICC_DSA_generate_parameters(...) (ICC_FP_NAME->ICC_DSA_generate_parameters)(__VA_ARGS__)
#define ICC_d2i_DSAparams(...) (ICC_FP_NAME->ICC_d2i_DSAparams)(__VA_ARGS__)
#define ICC_i2d_DSAparams(...) (ICC_FP_NAME->ICC_i2d_DSAparams)(__VA_ARGS__)
#define ICC_d2i_DSAPublicKey(...) (ICC_FP_NAME->ICC_d2i_DSAPublicKey)(__VA_ARGS__)
#define ICC_i2d_DSAPublicKey(...) (ICC_FP_NAME->ICC_i2d_DSAPublicKey)(__VA_ARGS__)
#define ICC_d2i_DSAPrivateKey(...) (ICC_FP_NAME->ICC_d2i_DSAPrivateKey)(__VA_ARGS__)
#define ICC_i2d_DSAPrivateKey(...) (ICC_FP_NAME->ICC_i2d_DSAPrivateKey)(__VA_ARGS__)

#define ICC_EVP_PKEY_set1_DH(...) (ICC_FP_NAME->ICC_EVP_PKEY_set1_DH)(__VA_ARGS__)
#define ICC_EVP_PKEY_get1_DH(...) (ICC_FP_NAME->ICC_EVP_PKEY_get1_DH)(__VA_ARGS__)
#define ICC_EVP_PKEY_sign_init(...) (ICC_FP_NAME->ICC_EVP_PKEY_sign_init)(__VA_ARGS__)
#define ICC_EVP_PKEY_sign(...) (ICC_FP_NAME->ICC_EVP_PKEY_sign)(__VA_ARGS__)
#define ICC_EVP_PKEY_verify_init(...) (ICC_FP_NAME->ICC_EVP_PKEY_verify_init)(__VA_ARGS__)
#define ICC_EVP_PKEY_verify(...) (ICC_FP_NAME->ICC_EVP_PKEY_verify)(__VA_ARGS__)
#define ICC_EVP_PKEY_encapsulate_init(...) (ICC_FP_NAME->ICC_EVP_PKEY_encapsulate_init)(__VA_ARGS__)
#define ICC_EVP_PKEY_auth_encapsulate_init(...) (ICC_FP_NAME->ICC_EVP_PKEY_auth_encapsulate_init)(__VA_ARGS__)
#define ICC_EVP_PKEY_encapsulate(...) (ICC_FP_NAME->ICC_EVP_PKEY_encapsulate)(__VA_ARGS__)
#define ICC_EVP_PKEY_decapsulate_init(...) (ICC_FP_NAME->ICC_EVP_PKEY_decapsulate_init)(__VA_ARGS__)
#define ICC_EVP_PKEY_auth_decapsulate_init(...) (ICC_FP_NAME->ICC_EVP_PKEY_auth_decapsulate_init)(__VA_ARGS__)
#define ICC_EVP_PKEY_decapsulate(...) (ICC_FP_NAME->ICC_EVP_PKEY_decapsulate)(__VA_ARGS__)

#define ICC_EVP_KDF_fetch(...) (ICC_FP_NAME->ICC_EVP_KDF_fetch)(__VA_ARGS__)
#define ICC_EVP_KDF_CTX_new(...) (ICC_FP_NAME->ICC_EVP_KDF_CTX_new)(__VA_ARGS__)
#define ICC_EVP_KDF_derive(...) (ICC_FP_NAME->ICC_EVP_KDF_derive)(__VA_ARGS__)
#define ICC_OSSL_PARAM_construct_uint32(...) (ICC_FP_NAME->ICC_OSSL_PARAM_construct_uint32)(__VA_ARGS__)
#define ICC_OSSL_PARAM_construct_octet_string(...) (ICC_FP_NAME->ICC_OSSL_PARAM_construct_octet_string)(__VA_ARGS__)
#define ICC_OSSL_PARAM_construct_end(...) (ICC_FP_NAME->ICC_OSSL_PARAM_construct_end)(__VA_ARGS__)

#define ICC_DH_new(...) (ICC_FP_NAME->ICC_DH_new)(__VA_ARGS__)
#define ICC_DH_generate_key(...) (ICC_FP_NAME->ICC_DH_generate_key)(__VA_ARGS__)
#define ICC_DH_check(...) (ICC_FP_NAME->ICC_DH_check)(__VA_ARGS__)
#define ICC_DH_free(...) (ICC_FP_NAME->ICC_DH_free)(__VA_ARGS__)
#define ICC_DH_size(...) (ICC_FP_NAME->ICC_DH_size)(__VA_ARGS__)
#define ICC_DH_compute_key(...) (ICC_FP_NAME->ICC_DH_compute_key)(__VA_ARGS__)
#define ICC_DH_generate_parameters(...) (ICC_FP_NAME->ICC_DH_generate_parameters)(__VA_ARGS__)
#define ICC_DH_get_PublicKey(...) (ICC_FP_NAME->ICC_DH_get_PublicKey)(__VA_ARGS__)
#define ICC_d2i_DHparams(...) (ICC_FP_NAME->ICC_d2i_DHparams)(__VA_ARGS__)
#define ICC_i2d_DHparams(...) (ICC_FP_NAME->ICC_i2d_DHparams)(__VA_ARGS__)

#define ICC_AES_GCM_CTX_ctrl(...) (ICC_FP_NAME->ICC_AES_GCM_CTX_ctrl)(__VA_ARGS__)
#define ICC_AES_GCM_CTX_new(...) (ICC_FP_NAME->ICC_AES_GCM_CTX_new)(__VA_ARGS__)
#define ICC_AES_GCM_CTX_free(...) (ICC_FP_NAME->ICC_AES_GCM_CTX_free)(__VA_ARGS__)
#define ICC_AES_GCM_Init(...) (ICC_FP_NAME->ICC_AES_GCM_Init)(__VA_ARGS__)
#define ICC_AES_GCM_EncryptUpdate(...) (ICC_FP_NAME->ICC_AES_GCM_EncryptUpdate)(__VA_ARGS__)
#define ICC_AES_GCM_EncryptFinal(...) (ICC_FP_NAME->ICC_AES_GCM_EncryptFinal)(__VA_ARGS__)
#define ICC_AES_GCM_DecryptUpdate(...) (ICC_FP_NAME->ICC_AES_GCM_DecryptUpdate)(__VA_ARGS__)
#define ICC_AES_GCM_DecryptFinal(...) (ICC_FP_NAME->ICC_AES_GCM_DecryptFinal)(__VA_ARGS__)

#define ICC_AES_CCM_Encrypt(...) (ICC_FP_NAME->ICC_AES_CCM_Encrypt)(__VA_ARGS__)
#define ICC_AES_CCM_Decrypt(...) (ICC_FP_NAME->ICC_AES_CCM_Decrypt)(__VA_ARGS__)

#define ICC_RSA_FixEncodingZeros(...) (ICC_FP_NAME->ICC_RSA_FixEncodingZeros)(__VA_ARGS__)

#define ICC_SP800_108_get_KDFbyname(...) (ICC_FP_NAME->ICC_SP800_108_get_KDFbyname)(__VA_ARGS__)
#define           ICC_SP800_108_KDF(...) (ICC_FP_NAME->ICC_SP800_108_KDF)(__VA_ARGS__)
#define      ICC_RSA_X931_derive_ex(...) (ICC_FP_NAME->ICC_RSA_X931_derive_ex)(__VA_ARGS__)
#define  ICC_EC_GROUP_set_asn1_flag(...) (ICC_FP_NAME->ICC_EC_GROUP_set_asn1_flag)(__VA_ARGS__)
#define            ICC_SP800_38F_KW(...) (ICC_FP_NAME->ICC_SP800_38F_KW)(__VA_ARGS__)

#define         ICC_HKDF(...) (ICC_FP_NAME->ICC_HKDF)(__VA_ARGS__)
#define ICC_HKDF_Extract(...) (ICC_FP_NAME->ICC_HKDF_Extract)(__VA_ARGS__)
#define  ICC_HKDF_Expand(...) (ICC_FP_NAME->ICC_HKDF_Expand)(__VA_ARGS__)

#define    ICC_crypto_kem_ctx_new(...) (ICC_FP_NAME->ICC_crypto_kem_ctx_new)(__VA_ARGS__)
#define    ICC_crypto_kem_ctx_free(...) (ICC_FP_NAME->ICC_crypto_kem_ctx_free)(__VA_ARGS__)
#define    ICC_crypto_kem_keypair(...) (ICC_FP_NAME->ICC_crypto_kem_keypair)(__VA_ARGS__)
#define        ICC_crypto_kem_enc(...) (ICC_FP_NAME->ICC_crypto_kem_enc)(__VA_ARGS__)
#define        ICC_crypto_kem_dec(...) (ICC_FP_NAME->ICC_crypto_kem_dec)(__VA_ARGS__)
#define             ICC_kem_bytes(...) (ICC_FP_NAME->ICC_kem_bytes)(__VA_ARGS__)
#define  ICC_kem_ciphertext_bytes(...) (ICC_FP_NAME->ICC_kem_ciphertext_bytes)(__VA_ARGS__)
#define   ICC_kem_secretkey_bytes(...) (ICC_FP_NAME->ICC_kem_secretkey_bytes)(__VA_ARGS__)
#define   ICC_kem_publickey_bytes(...) (ICC_FP_NAME->ICC_kem_publickey_bytes)(__VA_ARGS__)
#define   ICC_crypto_sign_ctx_new(...) (ICC_FP_NAME->ICC_crypto_sign_ctx_new)(__VA_ARGS__)
#define  ICC_crypto_sign_ctx_free(...) (ICC_FP_NAME->ICC_crypto_sign_ctx_free)(__VA_ARGS__)
#define   ICC_crypto_sign_keypair(...) (ICC_FP_NAME->ICC_crypto_sign_keypair)(__VA_ARGS__)
#define           ICC_crypto_sign(...) (ICC_FP_NAME->ICC_crypto_sign)(__VA_ARGS__)
#define      ICC_crypto_sign_open(...) (ICC_FP_NAME->ICC_crypto_sign_open)(__VA_ARGS__)
#define             ICC_dil_bytes(...) (ICC_FP_NAME->ICC_dil_bytes)(__VA_ARGS__)
#define   ICC_dil_secretkey_bytes(...) (ICC_FP_NAME->ICC_dil_secretkey_bytes)(__VA_ARGS__)
#define   ICC_dil_publickey_bytes(...) (ICC_FP_NAME->ICC_dil_publickey_bytes)(__VA_ARGS__)

#define            ICC_argon2_hash(...) (ICC_FP_NAME->ICC_argon2_hash)(__VA_ARGS__)
#define          ICC_argon2_verify(...) (ICC_FP_NAME->ICC_argon2_verify)(__VA_ARGS__)
#define   ICC_argon2_error_message(...) (ICC_FP_NAME->ICC_argon2_error_message)(__VA_ARGS__)
#define      ICC_argon2_encodedlen(...) (ICC_FP_NAME->ICC_argon2_encodedlen)(__VA_ARGS__)

#define gsk_exp_init(...) (ICC_FP_NAME->gsk_exp_init)(__VA_ARGS__)

#endif
