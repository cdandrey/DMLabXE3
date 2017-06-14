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

		Synchronize(SearchCliqueTreangl);

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
}
//------------------------------------------------------------------------------


inline void TThreadCLQ::ToLog(const AnsiString &Message)
{
	FormMain->RichEditLog->Lines->Append(Message);
}
//------------------------------------------------------------------------------


s_t __fastcall TThreadCLQ::ToSet(const v_t &data)
{
	s_t s;
	for (unsigned i = 0; i < data.size(); ++i)
		s.insert(data.at(i));

	return s;
}
//---------------------------------------------------------------------------


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


AnsiString __fastcall TThreadCLQ::ToString(const s_t &Data)
{
	AnsiString Str = "";
	AnsiString Buffer = "";

	for (s_t::const_iterator it = Data.begin(); it != Data.end(); ++it)
		Str += Buffer.sprintf("%3d ",*it);

	if (Str == "")
		Str = "{}";

	return Str.Trim();
}
//---------------------------------------------------------------------------


AnsiString __fastcall TThreadCLQ::ToString(ss_t &data)
{
	AnsiString Str = "";
	AnsiString Buffer = "";

	int i = 1;
	for (ss_t::iterator it = data.begin(); it != data.end(); ++it) {

		Str += Buffer.sprintf("%d (%d) : ",i,it->size());
		Str += ToString(it.operator *()) + "\n";

		++i;
	}

	if (Str == "")
		Str = "{}";

	return Str.Trim();
}
//---------------------------------------------------------------------------


AnsiString __fastcall TThreadCLQ::ToString(vs_t &Data)
{
	AnsiString Str = "";
	AnsiString Buffer = "";

	for (int i = 1; i < Data.size(); ++i) {
		if (!Data.at(i).empty()) {
			Str += Buffer.sprintf("%d (%d) : ",i,Data.at(i).size());
			Str += ToString(Data.at(i)) + "\n";
		}
	}

	if (Str == "")
		Str = "{}";

	return Str.Trim();
}
//---------------------------------------------------------------------------


void __fastcall TThreadCLQ::SearchCliqueTreangl()
{
		FormMain->RichEditLog->Clear();

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
		AnsiString Str = "";

		QueryPerformanceCounter(&TimeBegin);

		ss_t treangls;
		v_t  degree(N,0);

		SplitOnTreangls(&treangls,&degree);

		Log += "Треугольные клики:\n";
		Log += ToString(treangls) + "\n\n";

		s_t sub_vertex;
		int vertex_max = MaxSubgraph(treangls,degree,&sub_vertex);

		Str.sprintf("Максимальное подмножество:\n\tвершина: %8d(%d) | ",vertex_max,sub_vertex.size());
		Log += Str + ToString(sub_vertex) + "\n\n";

		vs_t sub_graph(Vertex);
		BuildSubgraph(sub_vertex,&sub_graph);

		Log += "Подграф:\n" + ToString(sub_graph) + "\n\n";

		QueryPerformanceCounter(&TimeEnd);

		double Time = (double)(TimeEnd.QuadPart - TimeBegin.QuadPart) / Freq.QuadPart;

		ToLog(Log);

		ToConsol(Str.sprintf("\nКоличество треугольных клик: %8d ",Cnt));
		ToConsol(Str.sprintf("\nКоличество операций: %8d ",Q));
		ToConsol(Str.sprintf("\nВремя работы алгоритма: %8.8f мс",Time));

		ToConsol("\nМаксимальная клика найдена! Алгоритм завершился успешно.");

}
//---------------------------------------------------------------------------


void __fastcall TThreadCLQ::SplitOnTreangls(ss_t *treangls, v_t *degree)
{
	v_t visit(Vertex.size(),0);

	for (unsigned i = 0; i < Vertex.size(); ++i) {
		visit[i] = 1;
		v_t path(C_BASE_CLIQUE_LEN);
        path[0] = i;
		dfs(i,1,visit,&path,treangls,degree);
	}
}
//---------------------------------------------------------------------------


void __fastcall TThreadCLQ::dfs(
								unsigned u        ,
								unsigned level    ,
								v_t      visit    ,
								v_t     *path     ,
								ss_t    *treangls ,
								v_t     *degree
				)
{
	for (s_t::iterator it = Vertex.at(u).begin(); it != Vertex.at(u).end(); ++it) {
		unsigned v = static_cast<unsigned>(*it);
		++Q;
		if (!visit.at(v)) {
			visit[v] = 1;
			path->operator[](level) = v;
			++Q;
			if (level < C_BASE_CLIQUE_LEN - 1) {
				// go next level
				dfs(v,level + 1,visit,path,treangls,degree);
			} else {
				// check is the path a treangl
                ++Q;
				unsigned va = path->at(0);
				unsigned vc = path->at(level);
				if (Vertex.at(va).find(vc) != Vertex.at(va).end()) {
					// add new treangl
					ToConsol(ToString(*path));

					treangls->insert(ToSet(*path));
					for (unsigned i = 0; i < path->size(); ++i)
					++degree->operator[](path->at(i));

					++Cnt;
				}
			}
		}
	}
}
//------------------------------------------------------------------------------


int __fastcall TThreadCLQ::MaxSubgraph(const ss_t &treangls,const v_t &degree,s_t *sub_vertex)
{
	int vertex_max = 0;
	for (unsigned i = 1; i < N; ++i)
		if (degree.at(vertex_max) < degree.at(i))
			vertex_max = i;

	for (ss_t::const_iterator it = treangls.begin(); it != treangls.end(); ++it)
		if (it->find(vertex_max) != it->end()) {
			set_union(sub_vertex->begin(),sub_vertex->end(),
						it->begin(), it->end(),
						inserter(*sub_vertex,sub_vertex->begin()));
		}

	return vertex_max;
}
//------------------------------------------------------------------------------


void __fastcall TThreadCLQ::BuildSubgraph(const s_t &sub_vertex,vs_t *sub_graph)
{
	s_t vertex_remove;
	for (int i = 1; i <= N; ++i)
		if (sub_vertex.find(i) == sub_vertex.end())
			vertex_remove.insert(i);

	for (s_t::iterator it = vertex_remove.begin(); it != vertex_remove.end(); ++it){

		for (s_t::iterator jt = sub_graph->at(*it).begin(); jt != sub_graph->at(*it).end(); ++jt)
			sub_graph->at(*jt).erase(*it);

		sub_graph->at(*it).clear();
	}
}
