// UpdateCallbackConsole.h

#ifndef __UPDATE_CALLBACK_CONSOLE_H
#define __UPDATE_CALLBACK_CONSOLE_H

#include <tools/methodids.h>
#include "../../../Common/StdOutStream.h"

#include "../Common/Update.h"

#include "PercentPrinter.h"

struct CErrorPathCodes
{
  FStringVector Paths;
  CRecordVector<DWORD> Codes;

  void AddError(const FString &path, DWORD systemError)
  {
    Paths.Add(path);
    Codes.Add(systemError);
  }
  void Clear()
  {
    Paths.Clear();
    Codes.Clear();
  }
};

class CCallbackConsoleBase
{
protected:
  CPercentPrinter _percent;

  CStdOutStream *_so;
  CStdOutStream *_se;

  void CommonError(const FString &path, DWORD systemError, bool isWarning);

  HRESULT ScanError_Base(const FString &path, DWORD systemError);
  HRESULT OpenFileError_Base(const FString &name, DWORD systemError);
  HRESULT ReadingFileError_Base(const FString &name, DWORD systemError);

public:
  bool NeedPercents() const { return _percent._so != NULL; };

  bool StdOutMode;

  bool NeedFlush;
  unsigned PercentsNameLevel;
  unsigned LogLevel;

  AString _tempA;
  UString _tempU;

  CCallbackConsoleBase():
      StdOutMode(false),
      NeedFlush(false),
      PercentsNameLevel(1),
      LogLevel(0)
      {}

  void SetWindowWidth(unsigned width) { _percent.MaxLen = width - 1; }

  void Init(CStdOutStream *outStream, CStdOutStream *errorStream, CStdOutStream *percentStream)
  {
    FailedFiles.Clear();

    _so = outStream;
    _se = errorStream;
    _percent._so = percentStream;
  }

  void ClosePercents2()
  {
    if (NeedPercents())
      _percent.ClosePrint(true);
  }

  void ClosePercents_for_so()
  {
    if (NeedPercents() && _so == _percent._so)
      _percent.ClosePrint(false);
  }


  CErrorPathCodes FailedFiles;
  CErrorPathCodes ScanErrors;

  HRESULT PrintProgress(const wchar_t *name, const char *command, bool showInLog);

};

class CUpdateCallbackConsole: public IUpdateCallbackUI2, public CCallbackConsoleBase
{
  // void PrintPropPair(const char *name, const wchar_t *val);

public:
  #ifndef _NO_CRYPTO
  bool PasswordIsDefined;
  UString Password;
  bool AskPassword;
  #endif



  bool DeleteMessageWasShown;
  Environment& enviro;

  CUpdateCallbackConsole(Environment& env)
      : DeleteMessageWasShown(false),
#ifndef _NO_CRYPTO
      PasswordIsDefined(false),
        AskPassword(false),
      #endif
        enviro(env)
      {}

  /*
  void Init(CStdOutStream *outStream)
  {
    CCallbackConsoleBase::Init(outStream);
  }
  */
  // ~CUpdateCallbackConsole() { if (NeedPercents()) _percent.ClosePrint(); }
  INTERFACE_IUpdateCallbackUI2(;)
};

#endif
