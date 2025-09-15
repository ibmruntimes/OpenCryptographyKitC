/*************************************************************************
// Copyright IBM Corp. 2023
//
// Licensed under the Apache License 2.0 (the "License"). You may not use
// this file except in compliance with the License. You can obtain a copy
// in the file LICENSE in the source distribution.
*************************************************************************/

/*************************************************************************
// Description: Provides a layer of abstraction/indirection for platform
//              specific code.
//
*************************************************************************/

#if defined(OS400)
#ifndef _GETNMI_H
#define _GETNMI_H

#ifdef  __cplusplus    
extern "C" {           
#endif                 

/* for reading a service program (library) */
typedef struct READ_PGM_T  { 
   void *	pPgm;
  } READ_PGM_T;

int	init_read_pgm(READ_PGM_T * pRead, char * pathfilename);
int	read_pgm(READ_PGM_T *pReadPgm, void** buf, int * pBytesRead);
void	term_read_pgm(READ_PGM_T* pRead); 


#ifdef  __cplusplus          
}                            
#endif                       
                             

#endif	/* _GETNMI_H */
#endif	/* OS400 */

