/*************************************************************************
// Copyright IBM Corp. 2023
//
// Licensed under the Apache License 2.0 (the "License"). You may not use
// this file except in compliance with the License. You can obtain a copy
// in the file LICENSE in the source distribution.
*************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>  // For uint32_t type

/* Include the header which now defines ICC_LINK */
#include "icc_loader.h"

#ifdef _WIN32
#include <windows.h>
#else
#include <dlfcn.h>
#endif

#if defined(_MSC_VER)
#define snprintf _snprintf
#endif

/* Define platform-specific macros for library handling */
#ifdef _WIN32
#define LOAD_LIBRARY(name) LoadLibraryExA(name, NULL, LOAD_WITH_ALTERED_SEARCH_PATH)
#define GET_PROC_ADDRESS(handle, name) GetProcAddress(handle, name)
#define FREE_LIBRARY(handle) FreeLibrary(handle)
#define LIB_ERROR() GetLastError()
#define PATH_SEPARATOR '\\'
#else
#define LOAD_LIBRARY(name) dlopen(name, RTLD_NOW | RTLD_GLOBAL)
#define GET_PROC_ADDRESS(handle, name) dlsym(handle, name)
#define FREE_LIBRARY(handle) dlclose(handle)
#define LIB_ERROR() dlerror()
#define PATH_SEPARATOR '/'
#endif

/* Function to convert library name based on platform
// If Windows and .so is specified, convert to .dll
// If Unix and .dll is specified, convert to .so */
static char* convert_lib_name(const char* lib_path) {
    if (lib_path == NULL) {
        return NULL;
    }

    char* result = strdup(lib_path);
    if (result == NULL) {
        return NULL;
    }

    char* filename = result;
    
    /* Find the last path separator to get the filename */
    char* last_sep = strrchr(result, PATH_SEPARATOR);
    if (last_sep != NULL) {
        filename = last_sep + 1;
    }

    /* Find the extension */
    char* ext = strrchr(filename, '.');
    if (ext != NULL) {
#ifdef _WIN32
        // On Windows, convert .so to .dll
        if (strcmp(ext, ".so") == 0) {
            strcpy(ext, ".dll");
        }
#else
        // On Unix, convert .dll to .so
        if (strcmp(ext, ".dll") == 0) {
            strcpy(ext, ".so");
        }
#endif
    }

    return result;
}

/* Function to get a function address from the library */
static void* get_function_address(LIB_HANDLE lib_handle, const char* func_name) {
    if (lib_handle == NULL || func_name == NULL) {
        return NULL;
    }
    
    void* func_addr = GET_PROC_ADDRESS(lib_handle, func_name);
    
    return func_addr;
}

// Default stub function that returns 0
static int stub_default(ICC_CTX* pcb) {
    (void)pcb;  // Suppress unused parameter warning
    return 0;
}

/* Function to load the ICC library and return the library handle */
LIB_HANDLE load_icc_library(const char* lib_path)
{
    LIB_HANDLE lib_handle = NULL;
    const char* lib_names[] = { "gsk8iccs", "jgsk8iccs", "ickc" };
    char full_path[1024];

    if (lib_path != NULL) {
        char* converted_path = convert_lib_name(lib_path);
        if (converted_path != NULL) {
            lib_handle = LOAD_LIBRARY(converted_path);
            free(converted_path);
            if (lib_handle == NULL) {
                return NULL;
            }
        }
    }

    if (lib_path == NULL) {
        for (int i = 0; i < sizeof(lib_names) / sizeof(lib_names[0]); i++) {
#ifdef _WIN32
            snprintf(full_path, sizeof(full_path), "%s.dll", lib_names[i]);
#elif defined(__APPLE__)
            snprintf(full_path, sizeof(full_path), "lib%s.dylib", lib_names[i]);
#else
            snprintf(full_path, sizeof(full_path), "lib%s.so", lib_names[i]);
#endif
            lib_handle = LOAD_LIBRARY(full_path);
            if (lib_handle != NULL) {
                break;
            }
        }
    }

    if (lib_handle == NULL) {
        for (int i = 0; i < sizeof(lib_names) / sizeof(lib_names[0]); i++) {
#ifdef _WIN32
            snprintf(full_path, sizeof(full_path), "%s_64.dll", lib_names[i]);
#elif defined(__APPLE__)
           snprintf(full_path, sizeof(full_path), "lib%s_64.dylib", lib_names[i]);
#else
            snprintf(full_path, sizeof(full_path), "lib%s_64.so", lib_names[i]);
#endif
            lib_handle = LOAD_LIBRARY(full_path);
            if (lib_handle != NULL) {
                break;
            }
        }
    }

    return lib_handle;
}

/* Function to load the ICC functions using the library handle */
int load_icc_functions(LIB_HANDLE lib_handle, ICC_FunctionTable* table)
{
    int rv = 0;
    const char* api_prefix = NULL;

    if (table == NULL || lib_handle == NULL) {
        return -1;
    }

    memset(table, 0, sizeof(ICC_FunctionTable));
    
    /* If we loaded the library but don't know which prefix to use,
    // try to determine it by looking for a known function with each prefix */
    if (api_prefix == NULL) {
        static const char* api_prefixes[] = { "ICC", "JCC", "ICKC", "ICCC", "ICCN" };
        const char* test_func = "_EVP_DigestInit";
        void* func_ptr = NULL;

        for (int i = 0; i < sizeof(api_prefixes) / sizeof(api_prefixes[0]); i++) {
            char func_name[256];
            snprintf(func_name, sizeof(func_name), "%s%s", api_prefixes[i], test_func);
            func_ptr = (void*)GET_PROC_ADDRESS(lib_handle, func_name);
            if (func_ptr != NULL) {
                api_prefix = api_prefixes[i];
                break; /* API prefix detected */
            }
        }
        
        if (api_prefix == NULL) {
            api_prefix = api_prefixes[0]; /* Using default API prefix */
        }
    }
    
    /* Store the library handle for potential future use */
    table->lib_handle = lib_handle;
    
    /* notice that the names in our table ALWAYS have ICC_ prefix but the symbols in the 
    ** library may be ICC_ JCC_ ICKC_ etc.
    * func_name cannot have the prefix because it may be different in the library from our table
    */
#define LoadFun_(func_name, fprefix) \
    { \
        char full_name[256]; \
        snprintf(full_name, sizeof(full_name), "%s_%s", api_prefix, #func_name); \
        void* func_addr = get_function_address(lib_handle, full_name); \
        memcpy((void **)(& table->fprefix ## func_name), &func_addr, sizeof(void*)); \
    }
#define LoadFun(x) LoadFun_(x, ICC_)

#define LoadFunStub(func_name, stub_func) \
    { \
        char full_name[256]; \
        snprintf(full_name, sizeof(full_name), "%s_%s", api_prefix, #func_name); \
        void* func_addr = get_function_address(lib_handle, full_name); \
        memcpy(&table->func_name, &func_addr, sizeof(void*)); \
        if (table->func_name == NULL) { \
            /* Stub used for function */ \
            void* stub_ptr = (void*)stub_func; \
            memcpy(&table->func_name, &stub_ptr, sizeof(void*)); \
        } \
    }

#define LoadFunStubDefault(func_name) \
    { \
        char full_name[256]; \
        snprintf(full_name, sizeof(full_name), "%s_%s", api_prefix, #func_name); \
        void* func_addr = get_function_address(lib_handle, full_name); \
        memcpy(&table->func_name, &func_addr, sizeof(void*)); \
        if (table->func_name == NULL) { \
            /* Default stub used for function */ \
            void* stub_ptr = (void*)stub_default; \
            memcpy(&table->func_name, &stub_ptr, sizeof(void*)); \
        } \
    }

    /* Load core functions */
    LoadFun(Init);
#if 0
    /* need to figure these out from 'cs' implementation */
#ifdef _WIN32
    LoadFun(InitW);
    LoadFun(gskiccs_pathW);
#endif
    LoadFun(gskiccs_path);
    LoadFun(gskiccs8_path);
#endif
    LoadFun(GetStatus);
    LoadFun(SetValue);
    LoadFun(GetValue);
    LoadFun(Attach);
    LoadFun(Cleanup);
    LoadFun(SelfTest);

    /* Load object functions */
    LoadFun(OBJ_nid2sn);

    /* Load error handling functions */
    LoadFun(ERR_get_error);
    LoadFun(ERR_peek_error);
    LoadFun(ERR_peek_last_error);
    LoadFun(ERR_error_string);
    LoadFun(ERR_error_string_n);
    LoadFun(ERR_lib_error_string);
    LoadFun(ERR_func_error_string);
    LoadFun(ERR_reason_error_string);
    LoadFun(ERR_clear_error);
    LoadFun(ERR_remove_state);

    /* Load random number generation functions */
    LoadFun(RAND_seed);
    LoadFun(RAND_bytes);
    LoadFun(get_RNGbyname);
    LoadFun(RNG_CTX_new);
    LoadFun(RNG_CTX_free);
    LoadFun(RNG_CTX_Init);
    LoadFun(RNG_Generate);
    LoadFun(RNG_ReSeed);
    LoadFun(RNG_CTX_ctrl);
    LoadFun(GenerateRandomSeed);

    /* Load bignum functions */
    LoadFun(BN_num_bits);
    LoadFun(BN_new);
    LoadFun(BN_bin2bn);
    LoadFun(BN_bn2bin);
    LoadFun(BN_clear_free);
    LoadFun(DES_random_key);
    LoadFun(DES_set_odd_parity);

    /* Load EVP functions */
    LoadFun(EVP_PKEY_new);
    LoadFun(EVP_PKEY_free);
    LoadFun(EVP_PKEY_cmp);
    LoadFun(EVP_get_cipherbyname);
    LoadFun(EVP_get_digestbyname);
    LoadFun(EVP_MD_CTX_new);
    LoadFun(EVP_MD_CTX_free);
    LoadFun(EVP_MD_CTX_copy);
    LoadFun(EVP_MD_CTX_init);
    LoadFun(EVP_MD_CTX_cleanup);
    LoadFun(EVP_MD_type);
    LoadFun(EVP_MD_size);
    LoadFun(EVP_MD_block_size);
    LoadFun(EVP_MD_CTX_md);

    /* Load EVP digest functions */
    LoadFun(EVP_DigestInit);
    LoadFun(EVP_DigestUpdate);
    LoadFun(EVP_DigestFinal);
    LoadFun(EVP_DigestSignInit);
    LoadFun(EVP_DigestSignFinal);
    LoadFun(EVP_DigestVerifyInit);
    LoadFun(EVP_DigestVerifyFinal);
    LoadFun(EVP_PKEY_id);

    /* Load EVP cipher functions */
    LoadFun(EVP_DecryptInit);
    LoadFun(EVP_DecryptUpdate);
    LoadFun(EVP_DecryptFinal);
    LoadFun(EVP_EncryptInit);
    LoadFun(EVP_EncryptUpdate);
    LoadFun(EVP_EncryptFinal);

    /* Load EVP envelope functions */
    LoadFun(EVP_OpenInit);
    LoadFun(EVP_OpenFinal);
    LoadFun(EVP_SealInit);
    LoadFun(EVP_SealFinal);
    LoadFun(EVP_SignFinal);
    LoadFun(EVP_VerifyFinal);
    LoadFun(EVP_ENCODE_CTX_new);
    LoadFun(EVP_ENCODE_CTX_free);
    LoadFun(EVP_EncodeInit);
    LoadFun(EVP_EncodeUpdate);
    LoadFun(EVP_EncodeFinal);
    LoadFun(EVP_DecodeInit);
    LoadFun(EVP_DecodeUpdate);
    LoadFun(EVP_DecodeFinal);

    /* Load EVP cipher property functions */
    LoadFun(EVP_CIPHER_key_length);
    LoadFun(EVP_CIPHER_iv_length);
    LoadFun(EVP_CIPHER_type);
    LoadFun(EVP_CIPHER_CTX_cipher);
    LoadFun(EVP_CIPHER_CTX_set_key_length);
    LoadFun(EVP_CIPHER_block_size);
    LoadFun(EVP_CIPHER_CTX_new);
    LoadFun(EVP_CIPHER_CTX_init);
    LoadFun(EVP_CIPHER_CTX_free);
    LoadFun(EVP_CIPHER_CTX_cleanup);
    LoadFun(EVP_CIPHER_CTX_set_padding);

    /* Load HMAC functions */
    LoadFun(HMAC_CTX_new);
    LoadFun(HMAC_CTX_free);
    LoadFun(HMAC_Init);
    LoadFun(HMAC_Update);
    LoadFun(HMAC_Final);

    /* Load CMAC functions */
    LoadFun(CMAC_CTX_new);
    LoadFun(CMAC_CTX_free);
    LoadFun(CMAC_Init);
    LoadFun(CMAC_Update);
    LoadFun(CMAC_Final);

    /* Load ECDSA functions */
    LoadFun(ECDSA_SIG_new);
    LoadFun(ECDSA_SIG_free);
    LoadFun(i2d_ECDSA_SIG);
    LoadFun(d2i_ECDSA_SIG);
    LoadFun(ECDSA_sign);
    LoadFun(ECDSA_verify);
    LoadFun(ECDSA_size);
    LoadFun(ECDSA_sign_setup);
    LoadFun(ECDSA_do_sign_ex);
    LoadFun(ECDSA_do_verify);

    /* Load EC key functions */
    LoadFun(EVP_PKEY_set1_EC_KEY);
    LoadFun(EVP_PKEY_get1_EC_KEY);
    LoadFun(EC_KEY_new_by_curve_name);
    LoadFun(EC_KEY_new);
    LoadFun(EC_KEY_free);
    LoadFun(EC_KEY_generate_key);
    LoadFun(EC_KEY_get0_group);
    LoadFun(EC_METHOD_get_field_type);
    LoadFun(EC_GROUP_method_of);
    LoadFun(EC_POINT_new);
    LoadFun(EC_POINT_free);
    LoadFun(EC_POINT_get_affine_coordinates_GFp);
    LoadFun(EC_POINT_set_affine_coordinates_GFp);
    LoadFun(EC_POINT_get_affine_coordinates_GF2m);
    LoadFun(EC_POINT_set_affine_coordinates_GF2m);
    LoadFun(EC_KEY_get0_public_key);
    LoadFun(EC_KEY_set_public_key);
    LoadFun(EC_KEY_get0_private_key);
    LoadFun(EC_KEY_set_private_key);
    LoadFun(ECDH_compute_key);
    LoadFun(d2i_ECPrivateKey);
    LoadFun(i2d_ECPrivateKey);
    LoadFun(d2i_ECParameters);
    LoadFun(i2d_ECParameters);
    LoadFun(EC_POINT_is_on_curve);
    LoadFun(EC_POINT_is_at_infinity);
    LoadFun(EC_KEY_check_key);
    LoadFun(EC_POINT_mul);
    LoadFun(EC_GROUP_get_order);
    LoadFun(EC_POINT_dup);

    /* Load PKCS5 functions */
    LoadFun(PKCS5_pbe_set);
    LoadFun(PKCS5_pbe2_set);
    LoadFun(PKCS5_pbe2_set_iv);
    LoadFun(PKCS12_pbe_crypt);
    LoadFun(X509_ALGOR_free);
    LoadFun(OBJ_txt2nid);
    LoadFun(EVP_EncodeBlock);
    LoadFun(EVP_DecodeBlock);

    /* Load EVP PKEY functions */
    LoadFun(EVP_PKEY_set1_RSA);
    LoadFun(EVP_PKEY_get1_RSA);
    LoadFun(i2d_PUBKEY);
    LoadFun(d2i_PUBKEY);
    LoadFun(i2d_PrivateKey);
    LoadFun(i2d_PublicKey);
    LoadFun(d2i_PrivateKey);
    LoadFun(d2i_PublicKey);
    LoadFun(i2d_RSAPrivateKey);
    LoadFun(i2d_RSAPublicKey);

    /* Load RSA functions */
    LoadFun(RSA_new);
    LoadFun(RSA_generate_key);
    LoadFun(RSA_check_key);
    LoadFun(RSA_free);
    LoadFun(RSA_private_encrypt);
    LoadFun(RSA_private_decrypt);
    LoadFun(RSA_public_encrypt);
    LoadFun(RSA_public_decrypt);
    LoadFun(RSA_sign);
    LoadFun(RSA_verify);
    LoadFun(RSA_blinding_off);
    LoadFun(RSA_size);

    /* Load BN context functions */
    LoadFun(BN_CTX_new);
    LoadFun(BN_CTX_free);
    LoadFun(BN_mod_exp);
    LoadFun(BN_div);
    LoadFun(BN_cmp);
    LoadFun(BN_add);
    LoadFun(BN_sub);
    LoadFun(BN_mod_mul);
    LoadFun(BN_is_prime_fasttest_ex);
    LoadFun(BN_X931_generate_Xpq);
    LoadFun(BN_X931_generate_prime_ex);
    LoadFun(BN_value_one);
    LoadFun(BN_set_word);
    LoadFun(BN_get_word);
    LoadFun(ASN1_OBJECT_free);
    LoadFun(OBJ_obj2txt);

    /* Load EVP cipher control functions */
    LoadFun(EVP_CIPHER_CTX_ctrl);

    /* Load EVP PKEY context functions */
    LoadFun(EVP_PKEY_CTX_new_from_name);
    LoadFun(EVP_PKEY_CTX_new_from_pkey);
    LoadFun(EVP_PKEY_CTX_new_id);
    LoadFun(PKCS5_PBKDF2_HMAC);
    LoadFun(CRYPTO_free);
    LoadFun(EC_GROUP_get_degree);
    LoadFun(EC_GROUP_get_curve_GFp);
    LoadFun(EC_GROUP_get_curve_GF2m);
    LoadFun(EC_GROUP_get0_generator);
    LoadFun(i2o_ECPublicKey);
    LoadFun(o2i_ECPublicKey);

    /* Load PKCS8 functions */
    LoadFun(EVP_PKCS82PKEY);
    LoadFun(EVP_PKEY2PKCS8);
    LoadFun(PKCS8_PRIV_KEY_INFO_free);
    LoadFun(d2i_PKCS8_PRIV_KEY_INFO);
    LoadFun(i2d_PKCS8_PRIV_KEY_INFO);
    LoadFun(i2d_ECPKParameters);
    LoadFun(d2i_ECPKParameters);
    LoadFun(EC_GROUP_free);
    LoadFun(EC_KEY_set_group);
    LoadFun(EC_KEY_dup);
    LoadFun(EVP_PKEY_size);
    LoadFun(EVP_PKEY_CTX_new);
    LoadFun(EVP_PKEY_CTX_free);
    LoadFun(DH_check_pub_key);
    LoadFun(EVP_BytesToKey);
    LoadFun(OBJ_txt2obj);
    LoadFun(EC_GROUP_get_curve_name);
    LoadFun(EVP_CIPHER_flags);
    LoadFun(EC_POINT_oct2point);
    LoadFun(EC_POINT_point2oct);
    LoadFun(EVP_CIPHER_do_all_sorted);
    LoadFun(EVP_MD_do_all_sorted);
    LoadFun(EC_get_builtin_curves);

    /* Load EVP PKEY generation functions */
    LoadFun(EVP_PKEY_generate);
    LoadFun(EVP_PKEY_keygen_init);
    LoadFun(EVP_PKEY_keygen);
    LoadFun(EVP_PKEY_paramgen_init);
    LoadFun(EVP_PKEY_paramgen);
    LoadFun(EVP_PKEY_derive_init);
    LoadFun(EVP_PKEY_derive_set_peer);
    LoadFun(EVP_PKEY_derive);
    LoadFun(EVP_PKEY_set1_tls_encodedpoint);
    LoadFun(EVP_PKEY_get1_tls_encodedpoint);
    LoadFun(EVP_DigestSign);
    LoadFun(EVP_DigestVerify);
    LoadFun(EVP_PKEY_get_raw_private_key);
    LoadFun(EVP_PKEY_get_raw_public_key);
    LoadFun(EVP_PKEY_new_CMAC_key);
    LoadFun(EVP_DigestFinalXOF);
    LoadFun(EVP_PKEY_CTX_ctrl);
    LoadFun(EVP_PKEY_CTX_ctrl_str);
    LoadFun(EVP_CIPHER_CTX_iv);
    LoadFun(EVP_PKEY_new_raw_private_key);
    LoadFun(EVP_PKEY_new_raw_public_key);

    /* Load EVP PKEY encrypt/decrypt functions */
    LoadFun(EVP_PKEY_encrypt_new);
    LoadFun(EVP_PKEY_decrypt_new);
    LoadFun(EVP_PKEY_decrypt);
    LoadFun(EVP_PKEY_encrypt);
    LoadFun(EVP_PKEY_bits);
    LoadFun(EVP_PKEY_decrypt_init);
    LoadFun(EVP_PKEY_encrypt_init);
    LoadFun(EVP_PKEY_verify_recover_init);
    LoadFun(EVP_PKEY_verify_recover);
    LoadFun(EVP_PKEY_sign_init);
    LoadFun(EVP_PKEY_sign);
    LoadFun(EVP_PKEY_verify_init);
    LoadFun(EVP_PKEY_verify);

    /* Load EVP PKEY encapsulate/decapsulate functions */
    LoadFun(EVP_PKEY_encapsulate_init);
    LoadFun(EVP_PKEY_auth_encapsulate_init);
    LoadFun(EVP_PKEY_encapsulate);
    LoadFun(EVP_PKEY_decapsulate_init);
    LoadFun(EVP_PKEY_auth_decapsulate_init);
    LoadFun(EVP_PKEY_decapsulate);

    /* Load KDF functions */
    LoadFun(EVP_KDF_fetch);
    LoadFun(EVP_KDF_CTX_new);
    LoadFun(EVP_KDF_derive);
    LoadFun(OSSL_PARAM_construct_uint32);
    LoadFun(OSSL_PARAM_construct_octet_string);
    LoadFun(OSSL_PARAM_construct_end);

    /*	Load DSA functions */
    LoadFun(d2i_DSA_PUBKEY);
    LoadFun(i2d_DSA_PUBKEY);
    LoadFun(EVP_PKEY_set1_DSA);
    LoadFun(EVP_PKEY_get1_DSA);
    LoadFun(DSA_dup_DH);
    LoadFun(DSA_sign);
    LoadFun(DSA_verify);
    LoadFun(DSA_size);
    LoadFun(DSA_SIG_new);
    LoadFun(DSA_SIG_free);
    LoadFun(d2i_DSA_SIG);
    LoadFun(i2d_DSA_SIG);
    LoadFun(DSA_new);
    LoadFun(DSA_free);
    LoadFun(DSA_generate_key);
    LoadFun(DSA_generate_parameters);
    LoadFun(d2i_DSAparams);
    LoadFun(i2d_DSAparams);
    LoadFun(d2i_DSAPublicKey);
    LoadFun(i2d_DSAPublicKey);
    LoadFun(d2i_DSAPrivateKey);
    LoadFun(i2d_DSAPrivateKey);

    /* Load DH functions */
    LoadFun(EVP_PKEY_set1_DH);
    LoadFun(EVP_PKEY_get1_DH);
    LoadFun(DH_new);
    LoadFun(DH_generate_key);
    LoadFun(DH_check);
    LoadFun(DH_free);
    LoadFun(DH_size);
    LoadFun(DH_compute_key);
    LoadFun(DH_generate_parameters);
    LoadFun(DH_get_PublicKey);
    LoadFun(d2i_DHparams);
    LoadFun(i2d_DHparams);
    LoadFun(DH_get_PrivateKey);
    LoadFun(DH_get0_pqg);
    LoadFun(DH_set0_pqg);
    LoadFun(dh_builtin_genparams_with_q);

    /* Load cipher context flag functions */
    LoadFun(EVP_CIPHER_CTX_flags);
    LoadFun(EVP_CIPHER_CTX_set_flags);
    LoadFun(AES_GCM_GenerateIV_NIST);
    LoadFun(EVP_CIPHER_CTX_copy);

    /* Load AES GCM functions */
    LoadFun(AES_GCM_CTX_ctrl);
    LoadFun(AES_GCM_CTX_new);
    LoadFun(AES_GCM_CTX_free);
    LoadFun(AES_GCM_Init);
    LoadFun(AES_GCM_EncryptUpdate);
    LoadFun(AES_GCM_EncryptFinal);
    LoadFun(AES_GCM_DecryptUpdate);
    LoadFun(AES_GCM_DecryptFinal);
    LoadFun(AES_GCM_GenerateIV);
    LoadFun(GHASH);

    /* Load AES CCM functions */
    LoadFun(AES_CCM_Encrypt);
    LoadFun(AES_CCM_Decrypt);

    /* Load RSA fix function */
    LoadFun(RSA_FixEncodingZeros);

    /* Load SP800 functions */
    LoadFun(SP800_108_get_KDFbyname);
    LoadFun(SP800_108_KDF);
    LoadFun(RSA_X931_derive_ex);
    LoadFun(EC_GROUP_set_asn1_flag);
    LoadFun(SP800_38F_KW);

    /* Load HKDF functions */
    LoadFun(HKDF);
    LoadFun(HKDF_Extract);
    LoadFun(HKDF_Expand);
    LoadFun(crypto_kem_ctx_new);

    return rv;
}

/* Function to unload the ICC library and free resources */
int unload_icc_functions(ICC_FunctionTable* table) {
    if (table == NULL) {
        return -1;
    }

    /* Free the library handle if it exists */
    if (table->lib_handle != NULL) {
        FREE_LIBRARY(table->lib_handle);
        table->lib_handle = NULL;
    }

    /* Clear the function table */
    memset(table, 0, sizeof(ICC_FunctionTable));

    return 0;
}
