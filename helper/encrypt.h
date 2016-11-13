#ifndef __WING_ENCRYPT_H__
#define __WING_ENCRYPT_H__
#include <windows.h>  
BOOL WingEncryptFile(  PCHAR szSource, PCHAR szDestination, PCHAR szPassword );   
BOOL WingDecryptFile(  PCHAR szSource, PCHAR &szDestination, PCHAR szPassword );
#endif