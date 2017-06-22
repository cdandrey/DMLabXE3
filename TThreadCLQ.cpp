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
	FormMain->ThreadExecut = THR_SEARCH_CLIQUE;
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
			ToConsol("Ошибка! Не задан граф. Максимальная клика не найдена.");
			return;
		}

		Q        = 0;
        Cnt      = 0;
		Cover    = v_t();
		LogShort = "\n\n";
		Log      = "\n\n";
		AnsiString Str = "";

		QueryPerformanceCounter(&TimeBegin);

		s_t sub_vertex;
		vs_t sub_graph(Vertex);

		if (WriteLog) {

			ss_t treangls;
			vs_t  degree(N,set<int>());

			SplitOnTreangls(&treangls,&degree);

			if (Terminated) {
				ToConsol("Процесс остановлен! Максимальная клика не найдена.");
				return;
			}

			Log += "Треугольные клики:\n";
			Log += ToString(treangls) + "\n\n";

			int vertex_max = MaxSubgraph(degree);

			sub_vertex = degree.at(vertex_max);
			sub_vertex.insert(vertex_max);

			Str.sprintf("Максимальное подмножество:\n\tвершина: %8d(%d) | ",vertex_max,sub_vertex.size());
			Log += Str + ToString(sub_vertex) + "\n\n";

			BuildSubgraph(sub_vertex,&sub_graph);

			Log += "Подграф:\n" + ToString(sub_graph) + "\n\n";

		}
		else {
			vs_t  degree(N,set<int>());

			SplitOnTreangls(&degree);

			if (Terminated) {
				ToConsol("Процесс остановлен! Максимальная клика не найдена.");
				return;
			}

			int vertex_max = MaxSubgraph(degree);

			sub_vertex = degree.at(vertex_max);
			sub_vertex.insert(vertex_max);

			BuildSubgraph(sub_vertex,&sub_graph);
		}

		if (Terminated) {
			ToConsol("Процесс остановлен! Максимальная клика не найдена.");
			return;
		}

		ExtractMaxClique(&sub_vertex,&sub_graph);
		unsigned clq_size =  sub_vertex.empty() ? 2 : sub_vertex.size();

		Log += Str.sprintf("Максимальная клика (%4d ) : ",clq_size);
		Log += ToString(sub_vertex) + "\n";

		QueryPerformanceCounter(&TimeEnd);

		double Time = static_cast<double>(TimeEnd.QuadPart - TimeBegin.QuadPart) / Freq.QuadPart;

		if (Terminated) {
			ToConsol("Процесс остановлен! Максимальная клика не найдена.");
			return;
		}

		ToConsol(Str.sprintf("\nКоличество треугольных клик: %8d ",Cnt));
		ToConsol(Str.sprintf("\nКоличество операций: %8d ",Q));
		ToConsol(Str.sprintf("\nВремя работы алгоритма: %8.8f с",Time));

		ToConsol("\nМаксимальная клика найдена.");

		Log += Str.sprintf("\nКоличество треугольных клик: %8d ",Cnt);
		Log += Str.sprintf("\nКоличество операций: %8d ",Q);
		Log += Str.sprintf("\nВремя работы алгоритма: %8.8f с",Time);

		ToLog(Log);
}
//---------------------------------------------------------------------------


void __fastcall TThreadCLQ::SplitOnTreangls(ss_t *treangls, vs_t *degree)
{
	v_t visit(Vertex.size(),0);
	AnsiString Str;

	for (unsigned i = 1; i < Vertex.size(); ++i) {

		if (Terminated)
			return;

		ToConsol(Str.sprintf("Поиск треугольных клик: вершина: %8d ",i));
		visit[i] = 1;
		v_t path(C_BASE_CLIQUE_LEN);
		path[0] = i;
		dfs(i,1,visit,&path,treangls,degree);
	}
}
//---------------------------------------------------------------------------


void __fastcall TThreadCLQ::SplitOnTreangls(vs_t *degree)
{
	v_t visit(Vertex.size(),0);
	AnsiString Str;

	for (unsigned i = 1; i < Vertex.size(); ++i) {

		if (Terminated)
			return;

		ToConsol(Str.sprintf("Поиск треугольных клик: вершина: %8d ",i));
		visit[i] = 1;
		v_t path(C_BASE_CLIQUE_LEN);
		path[0] = i;
		dfs(i,1,visit,&path,degree);
	}
}
//---------------------------------------------------------------------------


void __fastcall TThreadCLQ::dfs(
								unsigned u        ,
								unsigned level    ,
								v_t      visit    ,
								v_t     *path     ,
								ss_t    *treangls ,
								vs_t    *degree
				)
{
	for (s_t::iterator it = Vertex.at(u).begin(); it != Vertex.at(u).end(); ++it) {

		if (Terminated)
			return;

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

					treangls->insert(ToSet(*path));
					for (unsigned i = 1; i < path->size(); ++i)
						degree->at(path->at(0)).insert(path->at(i));
						//++degree->operator[](path->at(0));

					++Cnt;
				}
			}
		}
	}
}
//------------------------------------------------------------------------------


void __fastcall TThreadCLQ::dfs(
								unsigned u        ,
								unsigned level    ,
								v_t      visit    ,
								v_t     *path     ,
								vs_t    *degree
				)
{
	for (s_t::iterator it = Vertex.at(u).begin(); it != Vertex.at(u).end(); ++it) {

		if (Terminated)
			return;

		unsigned v = static_cast<unsigned>(*it);
		++Q;
		if (!visit.at(v)) {
			visit[v] = 1;
			path->operator[](level) = v;
			++Q;
			if (level < C_BASE_CLIQUE_LEN - 1) {
				// go next level
				dfs(v,level + 1,visit,path,degree);
			} else {
				// check is the path a treangl
                ++Q;
				unsigned va = path->at(0);
				unsigned vc = path->at(level);
				if (Vertex.at(va).find(vc) != Vertex.at(va).end()) {
					// add new treangl

					for (unsigned i = 1; i < path->size(); ++i)
						degree->at(path->at(0)).insert(path->at(i));
//					for (unsigned i = 0; i < path->size() - 1; ++i) {
//						for (unsigned j = 0; j < i; ++j)
//							degree->at(path->at(i)).insert(path->at(j));
//						for (unsigned j = i + 1; j < path->size(); ++j)
//							degree->at(path->at(i)).insert(path->at(j));
//					}

					++Cnt;
				}
			}
		}
	}
}
//------------------------------------------------------------------------------


int __fastcall TThreadCLQ::MaxSubgraph(const vs_t &degree)
{
	ToConsol("Поиск максимального подграфа ...");

	int vertex_max = 0;
	for (unsigned i = 1; i < N; ++i)
		if (degree.at(vertex_max).size() < degree.at(i).size())
			vertex_max = i;

//	for (ss_t::const_iterator it = treangls.begin(); it != treangls.end(); ++it)
//		if (it->find(vertex_max) != it->end()) {
//			set_union(sub_vertex->begin(),sub_vertex->end(),
//						it->begin(), it->end(),
//						inserter(*sub_vertex,sub_vertex->begin()));
//		}

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
//------------------------------------------------------------------------------


void __fastcall TThreadCLQ::ExtractMaxClique(s_t *sub_vertex,vs_t *list_adjacent)
{
    ToConsol("Преобразование подграфа к полному подграфу ...");
	while (!IsFull(*sub_vertex,*list_adjacent) && !sub_vertex->empty()){

		s_t::iterator min_it = sub_vertex->begin();

		for (s_t::iterator it = next_it(min_it); it != sub_vertex->end(); ++it)
			if (list_adjacent->at(*min_it).size() > list_adjacent->at(*it).size())
				min_it = it;

//		Log += "Debug: min: " + IntToStr(*min_it) + "\n\n";

		for (s_t::iterator it = sub_vertex->begin(); it != min_it; ++it)
			list_adjacent->at(*it).erase(*min_it);

		for (s_t::iterator it = next_it(min_it); it != sub_vertex->end(); ++it)
			list_adjacent->at(*it).erase(*min_it);

		list_adjacent->at(*min_it).clear();
		sub_vertex->erase(min_it);

//		Log += "Debug: list_adjacent: \n\n" + ToString(*list_adjacent) + "\n\n";
//		Log += "Debug: sub_vertex: " + ToString(*sub_vertex) + "\n\n";
	}
}
//------------------------------------------------------------------------------


bool __fastcall TThreadCLQ::IsFull(const s_t &vertex,const vs_t &list_adjacent)
{
	for (s_t::const_iterator it = vertex.cbegin(); it != vertex.cend(); ++it)
		if (list_adjacent.at(*it).size() != vertex.size() - 1)
			return false;

	return true;
}
//------------------------------------------------------------------------------

