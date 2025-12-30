
include platforms/${OPENSSL_LIBVER}/UNIX_like.mk

ASM_OBJS =  ppccap.o  ppccpuid.o \
        sha1-ppc.o sha256-ppc.o sha512-ppc.o \
        aes-ppc.o aes_cbc.o aes_core.o \
        ppc64-mont.o bn-ppc.o ppc-mont.o \
        camellia.o cmll_misc.o cmll_cbc.o \
        c_enc.o bf_enc.o des_enc.o \
        rc4_enc.o rc4_skey.o \
	auxv.o vpaes-ppc.o aesp8-ppc.o \
	sha256p8-ppc.o sha512p8-ppc.o \
	ghashp8-ppc.o


BUILD_OBJS = $(BASE_OBJS) $(ASM_OBJS)

#
# Config file options for this platform
#
#TWEAKS="ICC_TRNG=TRNG_ALT2"
#
tmp/dummyfile: $(SLIBCRYPTO)
	$(MKDIR) tmp
	( \
		cd tmp; \
		$(AR) x ../$(SLIBCRYPTO); \
		cd ..; \
        )
	touch $@

$(ASMOBJS): asm/aix64/rng-ppc.s
	$(CC) $(CFLAGS) asm/aix64/rng-ppc.s



