#ifndef _ERROR_DLG_H
#define _ERROR_DLG_H

extern void ShowErrorDlg(const char* msg);
void  SendError(const char* account,const char* playerName,const char* log,const char* userDesc,bool bDebug);

#endif