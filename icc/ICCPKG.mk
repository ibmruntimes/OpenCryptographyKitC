#
# Make file for ICCPKG components
#

iccpkg: ICC_ver.txt $(ICC_ROOT)/package/ICCPKG.tar  \
	$(ICC_ROOT)/package/gsk_crypto.tar $(ICC_ROOT)/package/gsk_crypto_sdk.tar \
	$(ICC_ROOT)/package/jgsk_crypto.tar $(ICC_ROOT)/package/jgsk_crypto_sdk.tar

# ICC ICC_ and ICKC_ namespace

# include $(ICC_ROOT)/iccpkg/gsk_crypto.mk

$(ICC_ROOT)/package/gsk_crypto.tar: $(GSK_DIR)
	( \
		cd $(GSK_DIR)/; \
		$(TARCMD) ../gsk_crypto.tar * \
	)

$(ICC_ROOT)/package/gsk_crypto_sdk.tar: $(GSK_SDK)
	( \
		cd $(ICC_ROOT)/package/ ; \
		$(TARCMD) gsk_crypto_sdk.tar gsk_sdk ; \
	)

# Java JCC_ namspace

$(ICC_ROOT)/package/jgsk_crypto.tar: $(JGSK_DIR)
	( \
		cd $(JGSK_DIR)/; \
		$(TARCMD) ../jgsk_crypto.tar *; \
	)

$(ICC_ROOT)/package/jgsk_crypto_sdk.tar: $(JGSK_SDK)
	( \
		cd $(ICC_ROOT)/package/ ; \
		$(TARCMD) jgsk_crypto_sdk.tar jgsk_sdk ; \
	)

# ICCPKG

$(ICC_ROOT)/package/ICCPKG.tar: $(PACKAGE_DIR) $(ICC_ROOT)/iccpkg/gsk_wrap2.c
	$(MKDIR) $(PACKAGE_DIR)/sources
	$(MKDIR) $(PACKAGE_DIR)/sources/exports
	$(MKDIR) $(PACKAGE_DIR)/bvt
	$(MKDIR) $(PACKAGE_DIR)/bvt/icc
	$(MKDIR) $(PACKAGE_DIR)/iccpkg_sdk
	$(MKDIR) $(PACKAGE_DIR)/zlib
	$(MKDIR) $(PACKAGE_DIR)/zlib/include
	$(MKDIR) $(PACKAGE_DIR)/doc
# Copy the bits that end up in the iccpkg SDK
	$(CP) $(ICC_ROOT)/iccpkg/iccpkg_a.h $(PACKAGE_DIR)/iccpkg_sdk/icc_a.h
	$(CP) $(ICC_ROOT)/icc/icc.h $(PACKAGE_DIR)/iccpkg_sdk/
	$(CP) $(ICC_ROOT)/icc/iccglobals.h $(PACKAGE_DIR)/iccpkg_sdk/
	$(CP) $(SDK_DIR)/openssl$(EXESUFX) $(PACKAGE_DIR)/iccpkg_sdk/
# Copy the sources for ICCPKG component
	$(CP) $(ICC_ROOT)/iccpkg/gsk_wrap2.c $(PACKAGE_DIR)/sources/
	$(CP) $(ICC_ROOT)/iccpkg/gsk_wrap2_a.c $(PACKAGE_DIR)/sources/
# Copy the exports files
	$(CP) $(ICC_ROOT)/iccpkg/exports/* $(PACKAGE_DIR)/sources/exports/
# Copy the static libraries, ICC's and ICCPKG PKCS#11
	$(CP) $(SDK_DIR)/$(STLPRFX)icc$(STLSUFX) $(PACKAGE_DIR)/sources/
# Copy the zlib library and headers
	$(CP) $(STLPRFX)zlib$(STLSUFX) $(PACKAGE_DIR)/zlib/
	$(CP) $(ZLIB_DIR)/zlib.h $(PACKAGE_DIR)/zlib/include/
	$(CP) $(ZLIB_DIR)/zconf.h $(PACKAGE_DIR)/zlib/include/
# Copy the test case sources
	$(CP) icctest.c  $(PACKAGE_DIR)/bvt/icc/
	( \
		cd $(ICC_ROOT)/package; \
		$(TARCMD) ICCPKG.tar icc/* sources/* bvt/* iccpkg_sdk/* \
		  	zlib/*;  \
	)

