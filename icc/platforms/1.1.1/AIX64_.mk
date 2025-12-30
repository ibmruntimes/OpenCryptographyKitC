include platforms/${OPENSSL_LIBVER}/Power.mk

ASM_OBJS += keccak1600-ppc64.o x25519-ppc64.o \
				ecp_nistz256.o ecp_nistz256-ppc64.o async.o async_null.o

$(ASMOBJS): asm/aix64/rng-ppc.s
	$(CC) $(CFLAGS) asm/aix64/rng-ppc.s


tmp/dummyfile: $(SLIBCRYPTO)
	$(MKDIR) tmp
	( cd tmp; \
	  $(AR) x ../$(SLIBCRYPTO); \
	  cd .. ; \
	)
	touch $@
