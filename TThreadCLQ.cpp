//---------------------------------------------------------------------------

#include <System.hpp>
#pragma hdrstop

#include "TThreadCLQ.h"
#include "TFormMain.h"
#pragma package(smart_init)
//---------------------------------------------------------------------------

//   Important: Methods and properties of objects in VCL can only be
//   used in a method called using Synchronize, for example:
//
//      Synchronize(&UpdateCaption);
//
//   where UpdateCaption could look like:
//
//      void __fastcall TThreadCLQ::UpdateCaption()
//      {
//        Form1->Caption = "Updated in a thread";
//      }
//---------------------------------------------------------------------------

__fastcall TThreadCLQ::TThreadCLQ(bool CreateSuspended)
	: TThread(CreateSuspended)
{
}
//---------------------------------------------------------------------------
void __fastcall TThreadCLQ::Execute()
{
	NameThreadForDebugging(System::String(L"ThreadCLQ"));
	//---- Place thread code here ----
	try {

		Synchronize(Lock);


	} __finally {

		Synchronize(UnLock);
	}
}
//---------------------------------------------------------------------------

/**** функции взаимодействия потока с главной формой приложения ****/


void __fastcall TThreadSearchCover::Lock()
{
	FormMain->ActionsLock();
	FormMain->ThreadExecut = THR_SEARCH_COVER;
	Application->ProcessMessages();
}
//------------------------------------------------------------------------------


void __fastcall TThreadCLQ::UnLock()
{
	FormMain->ActionsUnLock();
	FormMain->ThreadExecut = THR_NONE;
	Application->ProcessMessages();
}
//------------------------------------------------------------------------------


inline void TThreadCLQ::ToConsol(const AnsiString &Message)
{
	FormMain->ToConsol(Message);
	Application->ProcessMessages();
}
//------------------------------------------------------------------------------


