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
    QueryPerformanceFrequency(&Freq);
}
//---------------------------------------------------------------------------
void __fastcall TThreadCLQ::Execute()
{
	NameThreadForDebugging(System::String(L"ThreadCLQ"));
	//---- Place thread code here ----
	try {

		Synchronize(Lock);

        Synchronize(FormMain->RichEditLog->Clear);

		ToConsol("search-maximum-clique " + FileName);

		if (Vertex.size() == 0) {
			ToConsol("Ошибка! Не задан граф. Минимальное покрытие не найдено.");
			return;
		}

		Q        = 0;
        Cnt      = 0;
		Cover    = v_t();
		LogShort = "\n\n";
		Log      = "\n\n";

		QueryPerformanceCounter(&TimeBegin);

		v_t visit(Vertex.size(),0);

		for (unsigned i = 0; i < Vertex.size(); ++i) {
			visit[i] = 1;
			v_t treangl(C_BASE_CLIQUE_LEN);
			treangl[0] = i;
			dfs(i,1,visit,&treangl);
		}

		QueryPerformanceCounter(&TimeEnd);

		AnsiString Str = "";
		double Time = (double)(TimeEnd.QuadPart - TimeBegin.QuadPart) / Freq.QuadPart;

		ToConsol(Str.sprintf("\nКоличество треугольных клик: %8d ",Cnt));
		ToConsol(Str.sprintf("\nКоличество операций: %8d ",Q));
		ToConsol(Str.sprintf("\nВремя работы алгоритма: %8.8f мс",Time));

		ToConsol("\nМаксимальная клика найдена! Алгоритм завершился успешно.");

	} __finally {

		Synchronize(UnLock);
	}
}
//---------------------------------------------------------------------------

/**** функции взаимодействия потока с главной формой приложения ****/


void __fastcall TThreadCLQ::Lock()
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

    FormMain->RichEditLog->Lines->Append(Message);
}
//------------------------------------------------------------------------------

void __fastcall TThreadCLQ::dfs(unsigned u,unsigned level,v_t visit,v_t *treangl)
{
	for (s_t::iterator it = Vertex.at(u).begin(); it != Vertex.at(u).end(); ++it) {
		unsigned v = static_cast<unsigned>(*it);
		++Q;
		if (!visit.at(v)) {
			visit[v] = 1;
			treangl->operator[](level) = v;
            ++Q;
			if (level < C_BASE_CLIQUE_LEN - 1) {
				dfs(v,level + 1,visit,treangl);
			} else {
				ToConsol(ToString(*treangl));
                ++Cnt;
			}
		}
	}
}
//------------------------------------------------------------------------------

AnsiString __fastcall TThreadCLQ::ToString(v_t &Data)
{
	AnsiString Str = "";
	AnsiString Buffer = "";

	for (int i = 0; i < Data.size(); ++i)
		Str += Buffer.sprintf("%3d ",Data.at(i));

	if (Str == "")
		Str = "{}";

	return Str.Trim();
}
//---------------------------------------------------------------------------


