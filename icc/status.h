/*************************************************************************
// Copyright IBM Corp. 2023
//
// Licensed under the Apache License 2.0 (the "License"). You may not use
// this file except in compliance with the License. You can obtain a copy
// in the file LICENSE in the source distribution.
*************************************************************************/

#if !defined(STATUS_H)
#define STATUS_H

#define ICC_FATAL 0x80  /*!< Internal flag used by the status routines to trigger fatal error handling */

extern void setErrorState(void);
extern int getErrorState(void);

/* setErrorState with optional message */
extern void SetFatalError(const char *msg,const char *file, int line);


extern int SetStatusOK(ICClib *pcb,ICC_STATUS * stat);

/* Out of memory error. Special case - we can't allocate scratch buffers so do it directly 
*/
extern int SetStatusMem(ICClib *pcb,ICC_STATUS * stat,char *file,int line);

/* Set status with message and ICC line number */
extern int SetStatusLn(ICClib *pcb,ICC_STATUS * stat,int majRc,int minRc, const char *msg,const char *file,int
 line);
/* Set status with message,extra data and ICC line number */
extern int SetStatusLn2(ICClib *pcb,ICC_STATUS * stat,int majRc,int minRc, const char *msg,const char *details,const char *file,int line);

extern int OpenSSLError(ICClib *iccLib,ICC_STATUS *icc_stat,char *file,int line);

extern int ICC_utoa(char *buf,unsigned base, unsigned int n);

/* Handles unsigned longs plus zero pad or truncation */
extern int ICC_ultoa(char *buf, unsigned base, unsigned long n, unsigned int digits);

int ICC_GetLastError(ICC_STATUS *stat);

extern void ICC_strlcat(char *base,const char *msg,unsigned int maxlen);

#if defined(DEBUG)
/*!
  @brief
  Development/debug code.
  Prints byte strings in hex.
  This code has no security impacts. Anything you can do with this, you can
  do with a debugger.
  @param bytes input buffer
  @param len length of input buffer
*/  
extern void iccPrintBytes(unsigned char bytes[], int len);
#endif 

#endif
