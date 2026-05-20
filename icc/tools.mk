#
# Misc tools for R&D and PD
#

#
# FIPS_mem_collector was tested on x86/Z and didn't really help
# i.e. loop pver memory as a delay rather than a timing loop

# FIPS_lt_filter was an attempt to improve the histogram filter
# by making it effectively much wider. Also tested on x86/Z and
# didn't improve matters enough
#

TOOLS =	 \
	$(SDK_DIR)/GenRndData$(EXESUFX) \
	$(SDK_DIR)/GenRndData2$(EXESUFX) \
	smalltest$(EXESUFX) \
	smalltest4$(EXESUFX) \
	nist_algs$(EXESUFX) \
	sha256x$(EXESUFX)

# Not currently built because it's the same as GenRndData
# 	GenRndDataFIPS$(EXESUFX)

# Disabled. Tried, didn't work
#	FIPS_mem_collector$(EXESUFX) \
#	FIPS_filter_lt$(EXESUFX) \

#
# RNG data collection code.
#
GENRND_OBJS = GenRndData$(OBJSUFX) platform$(OBJSUFX) \
	timer_entropy$(OBJSUFX) nist_algs$(OBJSUFX) \
	noise_to_entropy$(OBJSUFX) timer_fips$(OBJSUFX) \
	TRNG_FIPS$(OBJSUFX) looper$(OBJSUFX) \
	$(ASMOBJS)

GENRNDFIPS_OBJS =  GenRndDataFIPS$(OBJSUFX) platform$(OBJSUFX) \
	timer_entropy$(OBJSUFX) nist_algs$(OBJSUFX) \
	noise_to_entropy$(OBJSUFX) timer_fips$(OBJSUFX) \
	TRNG_FIPS$(OBJSUFX) looper$(OBJSUFX) \
	$(ASMOBJS)

#- Build RND data generator executable

#- Compile RNG data generator
GenRndData$(OBJSUFX): tools/GenRndData.c 
	$(CC) $(CFLAGS) -I./ -I$(ZLIB_DIR) -I$(OSSLINC_DIR) -I$(OSSL_DIR) -I$(SDK_DIR) tools/GenRndData.c $(ASM_TWEAKS)

GenRndData: $(GENRND_OBJS) $(SLIBCRYPTO)
	$(LD) $(LDFLAGS) $(GENRND_OBJS) $(LDLIBS) $(SLIBCRYPTO)

GenRndData$(EXESUFX): $(GENRND_OBJS) $(SLIBCRYPTO)
	$(LD) $(LDFLAGS) $(GENRND_OBJS) $(ICCLIB) $(LDLIBS) $(SLIBCRYPTO)

$(SDK_DIR)/GenRndData$(EXESUFX): $(SDK_DIR) GenRndData$(EXESUFX)
	$(CP) GenRndData$(EXESUFX) $@


#- Compile hash check tool
sha256x$(OBJSUFX): tools/sha256x.c 
	$(CC) $(CFLAGS) -I./ -I$(OSSLINC_DIR)  tools/sha256x.c 

sha256x: $(SDK_DIR) sha256x$(OBJSUFX) $(SLIBCRYPTO)
	$(LD) $(LDFLAGS) sha256x$(OBJSUFX) $(SLIBCRYPTO) $(LDLIBS)
	$(CP) sha256x $(SDK_DIR)/

sha256x.exe: $(SDK_DIR) sha256x$(OBJSUFX) $(SLIBCRYPTO)
	$(LD) $(LDFLAGS) sha256x$(OBJSUFX) $(SLIBCRYPTO) $(SLIBSSL) $(LDLIBS)
	$(CP) sha256x.exe $(SDK_DIR)/

#- Build FIPS RND data generator executable
#- Compile RNG data generator
#- Build RNG data generator executable


# This version runs on the module (see iccpkg for step library version)
#- Compile newer RNG data generator
GenRndData2$(OBJSUFX): tools/GenRndData2.c $(SDK_DIR)/icc.h $(SDK_DIR)/icc_a.h $(SDK_DIR)/iccglobals.h
	$(CC) $(CFLAGS) -I $(SDK_DIR) tools/GenRndData2.c

GenRndData2$(EXESUFX): GenRndData2$(OBJSUFX) $(ICCLIB)
	$(LD) $(LDFLAGS) GenRndData2$(OBJSUFX) $(ICCLIB) $(LDLIBS)

$(SDK_DIR)/GenRndData2$(EXESUFX): $(SDK_DIR) GenRndData2$(EXESUFX)
	$(CP) GenRndData2$(EXESUFX) $@


#- FIPS specific RNG data generator	

GenRndDataFIPS$(OBJSUFX): tools/GenRndDataFIPS.c 
	$(CC) $(CFLAGS) -I./ -I$(ZLIB_DIR) -I$(OSSLINC_DIR) -I$(OSSL_DIR) -I$(SDK_DIR) tools/GenRndDataFIPS.c  $(ASM_TWEAKS)

GenRndDataFIPS$(EXESUFX) : $(GENRNDFIPS_OBJS)
	$(LD) $(LDFLAGS) $(GENRNDFIPS_OBJS) $(LDLIBS)


#- Build ICC test executables

smalltest$(OBJSUFX): tools/smalltest.c $(SDK_DIR)/icc.h $(SDK_DIR)/icc_a.h $(SDK_DIR)/iccglobals.h
	$(CC) $(CFLAGS) -I./ -I $(SDK_DIR) tools/smalltest.c

smalltest$(EXESUFX): $(ICCDLL) $(ICCLIB) smalltest$(OBJSUFX) 
	$(LD) $(LDFLAGS) smalltest$(OBJSUFX) $(ICCLIB) $(LDLIBS) 

smalltest4$(OBJSUFX):  tools/smalltest4.c $(SDK_DIR)/icc.h $(SDK_DIR)/icc_a.h $(SDK_DIR)/iccglobals.h
	-$(CC) $(CFLAGS)  -I./ -I $(SDK_DIR) tools/smalltest4.c

smalltest4$(EXESUFX): $(ICCDLL) $(ICCLIB) smalltest4$(OBJSUFX) 
	-$(LD) $(LDFLAGS) smalltest4$(OBJSUFX) $(ICCLIB) $(LDLIBS) 

# Integrity check API call test.

integ$(OBJSUFX):  integ.c $(SDK_DIR)/icc.h $(SDK_DIR)/icc_a.h $(SDK_DIR)/iccglobals.h
	-$(CC) $(CFLAGS)  -I./ -I $(SDK_DIR) tools/integ.c

integ$(EXESUFX): $(ICCDLL) $(ICCLIB) integ$(OBJSUFX) 
	-$(LD) $(LDFLAGS) integ$(OBJSUFX) $(ICCLIB) $(LDLIBS) 

test_NIST_algs$(OBJSUFX):  TRNG/test_NIST_algs.c 
	-$(CC) $(CFLAGS)  -I./  TRNG/test_NIST_algs.c

test_NIST_algs$(EXESUFX): test_NIST_algs$(OBJSUFX) nist_algs$(OBJSUFX) noise_to_entropy$(OBJSUFX)
	-$(LD) $(LDFLAGS)  test_NIST_algs$(OBJSUFX) nist_algs$(OBJSUFX) noise_to_entropy$(OBJSUFX) $(LDLIBS) 

#============================== TRNG test tools ================================


filter$(OBJSUFX): tools/filter.c  $(SDK_DIR) $(TRNG_DIR)/timer_fips.h
	-$(CC) $(CFLAGS) -I./ $(TRNG_DIRS) -DSTANDALONE_TE=1  tools/filter.c

filter$(EXESUFX): filter$(OBJSUFX)  nist_algs$(OBJSUFX) delta$(OBJSUFX) timer_entropy$(OBJSUFX) timer_fips$(OBJSUFX) $(ASMOBJS) tmp/tmp/dummyfile
	-$(LD) $(LDFLAGS) filter$(OBJSUFX)  nist_algs$(OBJSUFX) noise_to_entropy$(OBJSUFX) delta$(OBJSUFX)  timer_entropy$(OBJSUFX) timer_fips$(OBJSUFX) $(ASMOBJS) $(LDLIBS)
	-$(CP) filter$(EXESUFX) $(SDK_DIR)/filter$(EXESUFX)

sampler$(OBJSUFX): tools/sampler.c  $(SDK_DIR) $(TRNG_DIR)/timer_entropy.h $(TRNG_DIR)/stats.h 
	-$(CC) $(CFLAGS) -I./ $(TRNG_DIRS)  tools/sampler.c

sampler$(EXESUFX): sampler$(OBJSUFX)  nist_algs$(OBJSUFX) timer_entropy$(OBJSUFX) noise_to_entropy$(OBJSUFX) $(ASMOBJS) tmp/tmp/dummyfile
	-$(LD) $(LDFLAGS) sampler$(OBJSUFX)  nist_algs$(OBJSUFX) timer_entropy$(OBJSUFX) noise_to_entropy$(OBJSUFX) $(ASMOBJS) $(LDLIBS)
	-$(CP) sampler$(EXESUFX) $(SDK_DIR)/sampler$(EXESUFX)

faker$(OBJSUFX): tools/faker.c  $(SDK_DIR) $(TRNG_DIR)/timer_entropy.h $(TRNG_DIR)/stats.h 
	-$(CC) $(CFLAGS) -I./ $(TRNG_DIRS)  tools/faker.c

faker$(EXESUFX): faker$(OBJSUFX)  nist_algs$(OBJSUFX)  noise_to_entropy$(OBJSUFX) tmp/tmp/dummyfile
	-$(LD) $(LDFLAGS) faker$(OBJSUFX)  nist_algs$(OBJSUFX) noise_to_entropy$(OBJSUFX) $(LDLIBS)

FIPS_collector$(OBJSUFX): tools/FIPS_collector.c  $(SDK_DIR) $(TRNG_DIR)/timer_entropy.c $(TRNG_DIR)/timer_fips.c \
	$(TRNG_DIR)/nist_algs.c
	-$(CC) $(CFLAGS) -I./ $(TRNG_DIRS) tools/FIPS_collector.c


FIPS_collector$(EXESUFX): FIPS_collector$(OBJSUFX) looper$(OBJSUFX) tmp/tmp/dummyfile
	-$(LD) $(LDFLAGS) FIPS_collector$(OBJSUFX) looper$(OBJSUFX) $(ASMOBJS) $(LDLIBS)

#
# Basic collect plausible noise events
#

FIPS_filter$(OBJSUFX): tools/FIPS_filter.c  $(SDK_DIR) $(TRNG_DIR)/timer_fips.c $(TRNG_DIR)/nist_algs.c $(TRNG_DIR)/ext_filter.c 
	-$(CC) $(CFLAGS) -I./ $(TRNG_DIRS)  tools/FIPS_filter.c

FIPS_filter$(EXESUFX): FIPS_filter$(OBJSUFX) tmp/tmp/dummyfile
	-$(LD) $(LDFLAGS) FIPS_filter$(OBJSUFX) $(LDLIBS)

#
# Tried, didn't help
#
FIPS_mem_collector$(OBJSUFX): tools/FIPS_mem_collector.c  $(SDK_DIR) $(TRNG_DIR)/timer_entropy.c $(TRNG_DIR)/timer_fips.c \
	$(TRNG_DIR)/nist_algs.c
	-$(CC) $(CFLAGS) -I./ $(TRNG_DIRS) tools/FIPS_mem_collector.c

FIPS_mem_collector$(EXESUFX): FIPS_mem_collector$(OBJSUFX) looper$(OBJSUFX) tmp/tmp/dummyfile
	-$(LD) $(LDFLAGS) FIPS_mem_collector$(OBJSUFX) looper$(OBJSUFX) $(ASMOBJS) $(LDLIBS)




FIPS_filter_lt$(OBJSUFX): tools/FIPS_filter_lt.c  $(SDK_DIR) $(TRNG_DIR)/timer_fips.c $(TRNG_DIR)/nist_algs.c $(TRNG_DIR)/ext_filter.c 
	-$(CC) $(CFLAGS) -I./ $(TRNG_DIRS)  tools/FIPS_filter_lt.c

FIPS_filter_lt$(EXESUFX): FIPS_filter_lt$(OBJSUFX) tmp/tmp/dummyfile
	-$(LD) $(LDFLAGS) FIPS_filter_lt$(OBJSUFX) $(LDLIBS)

FIPS_filter_debias$(OBJSUFX): tools/FIPS_filter_debias.c  $(SDK_DIR) $(TRNG_DIR)/timer_fips.c $(TRNG_DIR)/nist_algs.c $(TRNG_DIR)/ext_filter.c \
 $(TRNG_DIR)/noise_to_entropy.h
	-$(CC) $(CFLAGS) -I./ $(TRNG_DIRS) tools/FIPS_filter_debias.c

FIPS_filter_debias$(EXESUFX): FIPS_filter_debias$(OBJSUFX) tmp/tmp/dummyfile
	-$(LD) $(LDFLAGS) FIPS_filter_debias$(OBJSUFX) $(LDLIBS)

# Offline test of distribution squeezing. data in, squeezed data out

squeeze$(OBJSUFX): tools/squeeze.c $(TRNG_DIR)/ext_filter.c 
	-$(CC) $(CFLAGS) -I./ $(TRNG_DIRS)  tools/squeeze.c

squeeze$(EXESUFX): squeeze$(OBJSUFX)
	-$(LD) $(LDFLAGS) squeeze$(OBJSUFX)
#============================= END TRNG components ================================
