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

	FuncExecut = NONE;

	FuncPoint[CLQ_TRE1] = SearchCliqueTreangl;
//	FuncPoint[CLQ_TRE2] = SearchCliqueTreangl2;

}
//---------------------------------------------------------------------------
void __fastcall TThreadCLQ::Execute()
{
	NameThreadForDebugging(System::String(L"ThreadCLQ"));
	//---- Place thread code here ----
	try {

		Synchronize(Lock);

//		for (int i = 0; i < ListFuncExecut.size(); ++i) {
//			FuncExecut = ListFuncExecut[i];
//			Synchronize(FuncPoint[FuncExecut]);
//		}
//		Synchronize(SearchCliqueTreangl);

		SelSort        = SelectAlg & 1;
		SelTreangls    = (SelectAlg>>1) & 3;
		SelMerge       = (SelectAlg>>3) & 1;
		SelLastChecked = (SelectAlg>>4) & 1;

		Synchronize(SearchCliqueTreanglSel);

	} __finally {

		Synchronize(UnLock);
	}
}
//---------------------------------------------------------------------------

/**** ������� �������������� ������ � ������� ������ ���������� ****/


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


AnsiString __fastcall TThreadCLQ::ToString(const v_t &Data,int Begin)
{
	AnsiString Str = "";
	AnsiString Buffer = "";

	for (unsigned i = Begin; i < Data.size(); ++i) {

		if (Data.at(i) < 10)
			Str += Buffer.sprintf("  %d ",Data.at(i));
		else if (Data.at(i) > 9 && Data.at(i) < 100)
			Str += Buffer.sprintf(" %d ",Data.at(i));
		else
			Str += Buffer.sprintf("%d ",Data.at(i));
	}

	if (Str == "")
		Str = "{}";

	return Str.Trim();
}
//---------------------------------------------------------------------------


AnsiString __fastcall TThreadCLQ::ToString(const s_t &Data)
{
	AnsiString Str = "";
	AnsiString Buffer = "";

	for (s_t::const_iterator it = Data.begin(); it != Data.end(); ++it) {
		if (*it < 10)
			Str += Buffer.sprintf("  %d ",*it);
		else if (*it > 9 && *it < 100)
			Str += Buffer.sprintf(" %d ",*it);
		else
			Str += Buffer.sprintf("%d ",*it);
	}

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

	for (int i = 0; i < Data.size(); ++i) {
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
			ToConsol("������! �� ����� ����. ������������ ����� �� �������.");
			return;
		}

		Q        = 0;
        Cnt      = 0;
		Cover    = v_t();
		LogShort = "\n\n";
		Log      = "\n\n";
		AnsiString Str = "";

		QueryPerformanceCounter(&TimeBegin);

        int vertex_max;
		s_t sub_vertex;
		s_t sub_vertex_max;
		vs_t sub_graph(Vertex);
		vs_t  degree(N + 1,set<int>());

		if (WriteLog) {

			ss_t treangls;

			SplitOnTreangls(&treangls,&degree);

			if (Terminated) {
				ToConsol("������� ����������! ������������ ����� �� �������.");
				return;
			}

			Log += "����������� �����:\n";
			Log += ToString(treangls) + "\n\n";

//			int vertex_max = MaxSubgraph(degree);
//
//			sub_vertex = degree.at(vertex_max);
//			sub_vertex.insert(vertex_max);

//			Str.sprintf("������������ ������������:\n\t�������: %8d(%d) | ",vertex_max,sub_vertex.size());
//			Log += Str + ToString(sub_vertex) + "\n\n";

//			BuildSubgraph(sub_vertex,&sub_graph);
//
//			Log += "�������:\n" + ToString(sub_graph) + "\n\n";

			for (int i = 1; i <= N; ++i) {

				ToConsol(Str.sprintf("����� ����� �� ������������: %8d",i));
				sub_vertex = degree.at(i);
				sub_vertex.insert(i);
				sub_graph = Vertex;
				BuildSubgraph(sub_vertex,&sub_graph);
				Log += Str.sprintf("����� ����� �� ������������: %8d | ",i);
				Log += ToString(sub_vertex);
				Log += "\n�������:\n" + ToString(sub_graph) + "\n";

				if (Terminated) {
					ToConsol("������� ����������! ������������ ����� �� �������.");
					return;
				}

				ExtractMaxClique(&sub_vertex,&sub_graph);

				unsigned clq_size =  sub_vertex.empty() ? 2 : sub_vertex.size();
				Log += Str.sprintf("����� (%4d ) : ",clq_size);
				Log += ToString(sub_vertex) + "\n\n";

				if (sub_vertex.size() > sub_vertex_max.size()) {
					vertex_max = i;
					sub_vertex_max = sub_vertex;
				}
			}
		}
		else {

			SplitOnTreangls(&degree);

			if (Terminated) {
				ToConsol("������� ����������! ������������ ����� �� �������.");
				return;
			}

//			int vertex_max = MaxSubgraph(degree);

//			sub_vertex = degree.at(vertex_max);
//			sub_vertex.insert(vertex_max);

			for (int i = 1; i <= N; ++i) {

				ToConsol(Str.sprintf("����� ����� �� ������������: %8d",i));
				sub_vertex = degree.at(i);
				sub_vertex.insert(i);
				sub_graph = Vertex;
				BuildSubgraph(sub_vertex,&sub_graph);

				if (Terminated) {
					ToConsol("������� ����������! ������������ ����� �� �������.");
					return;
				}

				ExtractMaxClique(&sub_vertex,&sub_graph);

				if (sub_vertex.size() > sub_vertex_max.size()) {
                    vertex_max = i;
					sub_vertex_max = sub_vertex;
				}
			}
		}

		unsigned clq_size =  sub_vertex_max.empty() ? 2 : sub_vertex_max.size();
		Log += Str.sprintf("\n\n���������� ����������� ����: %8d",Cnt);
		Log += Str.sprintf("\n������������ ������ ������������ �����: %8d | ",vertex_max);
		Log += ToString(degree.at(vertex_max));
		Log += Str.sprintf("\n������������ ����� (%4d ) : ",clq_size);
		Log += ToString(sub_vertex_max);

		QueryPerformanceCounter(&TimeEnd);

		double Time = static_cast<double>(TimeEnd.QuadPart - TimeBegin.QuadPart) / Freq.QuadPart;

		if (Terminated) {
			ToConsol("������� ����������! ������������ ����� �� �������.");
			return;
		}

		ToConsol(Str.sprintf("\n���������� ����������� ����: %8d ",Cnt));
		ToConsol(Str.sprintf("\n���������� ��������: %8d ",Q));
		ToConsol(Str.sprintf("\n����� ������ ���������: %8.8f �",Time));

		ToConsol("\n������������ ����� �������.");

		Log += Str.sprintf("\n���������� ��������: %8d ",Q);
		Log += Str.sprintf("\n����� ������ ���������: %8.8f �",Time);

		ToLog(Log);
}
//---------------------------------------------------------------------------


//void __fastcall TThreadCLQ::SearchCliqueTreangl2()
//{
//		// select:
//		// all treangls
//		// full merge
//		//
//
//		FormMain->RichEditLog->Clear();
//
//		ToConsol("search-maximum-clique-2 " + FileName);
//
//		if (Vertex.size() == 0) {
//			ToConsol("������! �� ����� ����. ������������ ����� �� �������.");
//			return;
//		}
//
//		Q        = 0;
//        Cnt      = 0;
//		Cover    = v_t();
//		LogShort = "\n";
//		Log      = "\n";
//		AnsiString Str = "";
//
//		QueryPerformanceCounter(&TimeBegin);
//
//		s_t max_clique;
//
//		if (WriteLog) {
//
//			if (Terminated) {
//				ToConsol("������� ����������! ������������ ����� �� �������.");
//				return;
//			}
//
//			v_t visit(N + 1,0);
//
//			for (int i = 1; i <= N; ++i) {
//
//				if (Terminated)
//					return;
//
//				ToConsol(Str.sprintf("����� ����������� ����: �������: %8d ",i));
//				Log += "����� ����������� ����: �������:" + IntToStr(i) + "\n\n";
//
//				ss_t treangls;
//				v_t  treangls_degree(N + 1,0);
//				vs_t list_adjacent(N + 1,s_t());
//				v_t  path(C_BASE_CLIQUE_LEN);
//
//				path[0] = i;
//				visit[i] = 1;
//				dfs(i,1,visit,&path,&treangls,&treangls_degree,&list_adjacent);
//
//
////				s_t clique = list_adjacent.at(i);
////				clique.insert(i);
////				ExtractMaxClique2Log(&clique,treangls,treangls_degree,list_adjacent);
//
//				if (!treangls.empty()) {
//					s_t clique;
//					vs_t vtreangls;
//					for (ss_t::iterator it = treangls.begin(); it != treangls.end(); ++it)
//						vtreangls.push_back(*it);
//
//					MergeTreanglsAll(vtreangls,&clique);
//					Log += "--------------------------------------------------------\n\n";
//
//					if (max_clique.size() < clique.size())
//						max_clique.swap(clique);
//				}
//			}
//
//		}
//		else {
//
//			if (Terminated) {
//				ToConsol("������� ����������! ������������ ����� �� �������.");
//				return;
//			}
//
//			v_t visit(N + 1,0);
//
//			for (int i = 1; i <= N; ++i) {
//
//				if (Terminated)
//					return;
//
//				ToConsol(Str.sprintf("����� ����������� ����: �������: %8d ",i));
//
//				ss_t treangls;
//				v_t  treangls_degree(N + 1,0);
//				vs_t list_adjacent(N + 1,s_t());
//				v_t  path(C_BASE_CLIQUE_LEN);
//
//				path[0] = i;
//				visit[i] = 1;
//				dfs(i,1,visit,&path,&treangls,&treangls_degree,&list_adjacent);
//
////				s_t clique = list_adjacent.at(i);
////				clique.insert(i);
////				ExtractMaxClique2(&clique,treangls,treangls_degree,list_adjacent);
//
//				if (!treangls.empty()) {
//					s_t clique;
//					vs_t vtreangls;
//					for (ss_t::iterator it = treangls.begin(); it != treangls.end(); ++it)
//						vtreangls.push_back(*it);
//					MergeTreanglsAll(vtreangls,&clique);
//
//					if (max_clique.size() < clique.size())
//						max_clique.swap(clique);
//                }
//			}
//		}
//
//		unsigned clq_size =  max_clique.empty() ? 2 : max_clique.size();
//		Log += Str.sprintf("\n\n���������� ����������� ����: %8d",Cnt);
//		Log += Str.sprintf("\n������������ ����� (%4d ) : ",clq_size);
//		Log += ToString(max_clique);
//
//		QueryPerformanceCounter(&TimeEnd);
//
//		double Time = static_cast<double>(TimeEnd.QuadPart - TimeBegin.QuadPart) / Freq.QuadPart;
//
//		if (Terminated) {
//			ToConsol("������� ����������! ������������ ����� �� �������.");
//			return;
//		}
//
//		ToConsol(Str.sprintf("\n���������� ����������� ����: %8d ",Cnt));
//		ToConsol(Str.sprintf("\n���������� ��������: %8d ",Q));
//		ToConsol(Str.sprintf("\n����� ������ ���������: %8.8f �",Time));
//
//		ToConsol("\n������������ ����� �������.");
//
//		Log += Str.sprintf("\n���������� ��������: %8d ",Q);
//		Log += Str.sprintf("\n����� ������ ���������: %8.8f �",Time);
//
//		ToLog(Log);
//}
////---------------------------------------------------------------------------
//
//
void __fastcall TThreadCLQ::SearchCliqueTreanglSel()
{
		FormMain->RichEditLog->Clear();

		ToConsol("search-maximum-clique-sel " + FileName);

		if (Vertex.size() == 0) {
			ToConsol("������! �� ����� ����. ������������ ����� �� �������.");
			return;
		}

		Q        = 0;
        Cnt      = 0;
		Cover    = v_t();
		LogShort = "\n";
		Log      = "\n";
		AnsiString Str = "";

		s_t max_clique;
		v_t visit(N + 1,0);

		vs_t lsa;
		v_t  mv(Vertex.size());
		v_t  mvr(Vertex.size());


		if (SelSort) {
			Sort(Vertex,&mv,&mvr,&lsa);
			Log += "����� ����������� �������:\n\n";
			v_t m(lsa.size());
			for (unsigned i = 0; i < m.size(); ++i)
				m[i] = i;
			Log += "�������� ������:\t" + ToString(m,1) + "\n";
			Log += "����-��� ������:\t" + ToString(mvr,1) + "\n\n";

			Log += "����-��� ������:\t" + ToString(m,1) + "\n";
			Log += "�������� ������:\t" + ToString(mv,1) + "\n\n";
		} else {
			lsa = Vertex;
		}

		QueryPerformanceCounter(&TimeBegin);

		if (WriteLog) {

			if (Terminated) {
				ToConsol("������� ����������! ������������ ����� �� �������.");
				return;
			}

			if (SelSort) {
				Log += "��������������� ������ ���������:\n\n";
				Log += ToString(lsa);
				Log += "\n\n";
			}

			if (SelTreangls < 2) {
				for (int i = 1; i <= N; ++i) {

					if (Terminated)
						return;

					ToConsol(Str.sprintf("����� ����������� ����: �������: %8d ",i));
					Log += "����� ����������� ����: �������:" + IntToStr(i) + "\n\n";

					vs_t treangls;
					v_t  path(C_BASE_CLIQUE_LEN);

					path[0] = i;
					visit[i] = 1;

					switch (SelTreangls) {
					case 0: SplitOnTreanglsAll(i,1,lsa,visit,&path,&treangls);
							break;
					case 1: SplitOnTreanglsTop(i,1,lsa,visit,&path,&treangls);
							break;
					}

					Log += ToString(treangls);
					Log += "\n\n����������� �������������:\n";

					if (!treangls.empty()) {
						s_t clique;

						switch (SelMerge) {
						case 0: MergeTreanglsAll(&clique,treangls,lsa);
								break;
						case 1: MergeTreanglsLine(0,treangls.at(0),&clique,treangls,lsa);
								break;
						}

						Log += "  " + ToString(clique) + "\n\n";
						Log += "--------------------------------------------------------\n\n";

						if (max_clique.size() < clique.size())
							max_clique.swap(clique);
					}
				}

			} else {  //  SelTreangsl == 2

				ToConsol(Str.sprintf("����� ����������� ����:"));
				Log += "����� ����������� ����:\n\n";

				vs_t treangls;

				for (int i = 1; i <= N; ++i) {

					if (Terminated)
						return;

					v_t  path(C_BASE_CLIQUE_LEN);
					path[0] = i;
					visit[i] = 1;
					vs_t tmp;

					SplitOnTreanglsOnce(i,1,lsa,visit,&path,&tmp);

					if (!tmp.empty())
						treangls.push_back(tmp[0]);
				}

				Log += ToString(treangls);
				Log += "\n\n����������� �������������:";

				if (!treangls.empty()) {

					s_t clique;

					switch (SelMerge) {
					case 0: MergeTreanglsAll(&clique,treangls,lsa);
							break;
					case 1: MergeTreanglsLine(0,treangls.at(0),&clique,treangls,lsa);
							break;
					}

					Log += "  " + ToString(clique) + "\n\n";
					Log += "--------------------------------------------------------\n\n";

					if (max_clique.size() < clique.size())
						max_clique.swap(clique);
				}
			}   // SelTreangls

		}
		else {

			if (Terminated) {
				ToConsol("������� ����������! ������������ ����� �� �������.");
				return;
			}

			if (SelTreangls < 2) {
				for (int i = 1; i <= N; ++i) {

					if (Terminated)
						return;

					ToConsol(Str.sprintf("����� ����������� ����: �������: %8d ",i));

					vs_t treangls;
					v_t  path(C_BASE_CLIQUE_LEN);

					path[0] = i;
					visit[i] = 1;

					switch (SelTreangls) {
					case 0: SplitOnTreanglsAll(i,1,lsa,visit,&path,&treangls);
							break;
					case 1: SplitOnTreanglsTop(i,1,lsa,visit,&path,&treangls);
							break;
					}

					if (!treangls.empty()) {
						s_t clique;

						switch (SelMerge) {
						case 0: MergeTreanglsAll(&clique,treangls,lsa);
								break;
						case 1: MergeTreanglsLine(0,treangls.at(0),&clique,treangls,lsa);
								break;
						}

						if (max_clique.size() < clique.size())
							max_clique.swap(clique);
					}
				}

			} else {  //  SelTreangsl == 2

				ToConsol(Str.sprintf("����� ����������� ����:"));

				vs_t treangls;

				for (int i = 1; i <= N; ++i) {

					if (Terminated)
						return;

					v_t  path(C_BASE_CLIQUE_LEN);
					path[0] = i;
					visit[i] = 1;
					vs_t tmp;

					SplitOnTreanglsOnce(i,1,lsa,visit,&path,&tmp);

					if (!tmp.empty())
						treangls.push_back(tmp[0]);
				}

				if (!treangls.empty()) {
					s_t clique;

					switch (SelMerge) {
					case 0: MergeTreanglsAll(&clique,treangls,lsa);
							break;
					case 1: MergeTreanglsLine(0,treangls.at(0),&clique,treangls,lsa);
							break;
					}

					if (max_clique.size() < clique.size())
						max_clique.swap(clique);
				}
			}   			// SelTreangls
		}  				 	// LogWrite

		if (SelLastChecked)
			LastChecked(lsa,&max_clique);

		unsigned clq_size =  max_clique.empty() ? 2 : max_clique.size();
		Log += Str.sprintf("\n\n���������� ����������� ����: %8d",Cnt);
		Log += Str.sprintf("\n������������ ����� (%4d ) : ",clq_size);

		if (SelSort){
			s_t tmp;
			for (s_t::iterator it = max_clique.begin(); it != max_clique.end(); ++it)
				tmp.insert(mv[*it]);
			max_clique.swap(tmp);
		}

		Log += ToString(max_clique);

		QueryPerformanceCounter(&TimeEnd);

		double Time = static_cast<double>(TimeEnd.QuadPart - TimeBegin.QuadPart) / Freq.QuadPart;

		if (Terminated) {
			ToConsol("������� ����������! ������������ ����� �� �������.");
			return;
		}

		ToConsol(Str.sprintf("\n���������� ����������� ����: %8d ",Cnt));
		ToConsol(Str.sprintf("\n���������� ��������: %8d ",Q));
		ToConsol(Str.sprintf("\n����� ������ ���������: %8.8f �",Time));

		ToConsol("\n������������ ����� �������.");

		Log += Str.sprintf("\n���������� ��������: %8d ",Q);
		Log += Str.sprintf("\n����� ������ ���������: %8.8f �",Time);

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

		ToConsol(Str.sprintf("����� ����������� ����: �������: %8d ",i));
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

		ToConsol(Str.sprintf("����� ����������� ����: �������: %8d ",i));
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

					degree->at(path->at(0)).insert(path->at(1));
					degree->at(path->at(0)).insert(path->at(2));
					degree->at(path->at(1)).insert(path->at(0));

					++Cnt;
				}
			}
		}
	}
}
//------------------------------------------------------------------------------


void __fastcall TThreadCLQ::dfs(
								unsigned u              ,
								unsigned level          ,
								v_t      visit          ,
								v_t     *path           ,
								ss_t    *treangls       ,
								v_t     *treangls_degree,
								vs_t    *list_adjacent
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
				dfs(v,level + 1,visit,path,treangls,treangls_degree,list_adjacent);

			} else {

				// check is the path a treangl
                ++Q;
				unsigned va = path->at(0);
				unsigned vc = path->at(level);
				if (Vertex.at(va).find(vc) != Vertex.at(va).end()) {

					// add new treangl

					treangls->insert(ToSet(*path));

					++treangls_degree->at(path->at(0));
					++treangls_degree->at(path->at(1));
					++treangls_degree->at(path->at(2));


					list_adjacent->at(path->at(0)).insert(path->at(1));
					list_adjacent->at(path->at(0)).insert(path->at(2));
					list_adjacent->at(path->at(1)).insert(path->at(0));
					list_adjacent->at(path->at(1)).insert(path->at(2));
					list_adjacent->at(path->at(2)).insert(path->at(0));
					list_adjacent->at(path->at(2)).insert(path->at(1));

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

					degree->at(path->at(0)).insert(path->at(1));
					degree->at(path->at(0)).insert(path->at(2));
					degree->at(path->at(1)).insert(path->at(0));

					++Cnt;
				}
			}
		}
	}
}
//------------------------------------------------------------------------------


void __fastcall TThreadCLQ::SplitOnTreanglsAll(
								unsigned    u              ,
								unsigned    level          ,
								const vs_t &lsa            ,
								v_t         visit          ,
								v_t        *path           ,
								vs_t       *treangls
				)
{
	for (s_t::iterator it = lsa.at(u).begin(); it != lsa.at(u).end(); ++it) {

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
				SplitOnTreanglsAll(v,level + 1,lsa,visit,path,treangls);

			} else {

				// check is the path a treangl
				++Q;
				unsigned va = path->at(0);
				unsigned vc = path->at(level);
				if (lsa.at(va).find(vc) != lsa.at(va).end()) {

					// add new treangl

					treangls->push_back(ToSet(*path));

					++Cnt;
				}
			}
		}
	}
}
//------------------------------------------------------------------------------


void __fastcall TThreadCLQ::SplitOnTreanglsTop(
					unsigned    u        ,
					unsigned    level    ,
					const vs_t &lsa      ,
					v_t         visit    ,
					v_t        *path     ,
					vs_t       *treangls
				)
{
	for (s_t::iterator it = lsa.at(u).begin(); it != lsa.at(u).end(); ++it) {

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
				SplitOnTreanglsTop(v,level + 1,lsa,visit,path,treangls);

			} else {
				
				// check is the path a treangl
                		++Q;
				
				unsigned va = path->at(0);
				unsigned vc = path->at(level);

				if (lsa.at(va).find(vc) != lsa.at(va).end()) {

					// add new treangl

					treangls->push_back(ToSet(*path));

					++Cnt;
					return;
				}
			}
		}
	}
}
//------------------------------------------------------------------------------


void __fastcall TThreadCLQ::SplitOnTreanglsOnce(
					unsigned    u        ,
					unsigned    level    ,
					const vs_t &lsa      ,
					v_t         visit    ,
					v_t        *path     ,
					vs_t       *trgl
				)
{
	for (s_t::iterator it = lsa.at(u).begin(); it != lsa.at(u).end(); ++it) {

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
				SplitOnTreanglsOnce(v,level + 1,lsa,visit,path,trgl);

			} else {

				// check is the path a treangl
						++Q;

				unsigned va = path->at(0);
				unsigned vc = path->at(level);

				if (lsa.at(va).find(vc) != lsa.at(va).end()) {

					// add new treangl

					trgl->push_back(ToSet(*path));

					++Cnt;
					return;
				}
			}
		}

		if (trgl->size() > 0)
            return;
	}
}
//------------------------------------------------------------------------------


void __fastcall TThreadCLQ::MergeTreanglsAll(s_t *clq_max,const vs_t &trgl,const vs_t &lsa)
{
	if (trgl.size() == 1 ) {
		*clq_max = trgl[0];
	} else {
		for (unsigned i = 0; i < trgl.size() - 1; ++i) {

			if (Terminated)
				return;

			MergeTreanglsLine(i,trgl.at(i),clq_max,trgl,lsa);
		}
	}
}
//------------------------------------------------------------------------------


void __fastcall TThreadCLQ::MergeTreanglsLine(unsigned u,s_t clq,s_t *clq_max,const vs_t &trgl,const vs_t &lsa)
{
	for (unsigned v = u + 1; v < trgl.size(); ++v) {

		if (Terminated)
			return;

		++Q;

		bool is_poss = clq_max->size() < (clq.size() + (trgl.size() - v)*2);

		if (is_poss) {
			if (IsMerge(clq,trgl.at(v),lsa)) {

				s_t clq_next = clq;
				s_t::iterator it = next_it(trgl.at(v).begin());
				clq_next.insert(*it);
				clq_next.insert(*next_it(it));

				MergeTreanglsLine(v,clq_next,clq_max,trgl,lsa);

			}
		}
	}

	if (clq.size() > clq_max->size())
		*clq_max = clq;
}
//------------------------------------------------------------------------------


int __fastcall TThreadCLQ::MaxSubgraph(const vs_t &degree)
{
	ToConsol("����� ������������� �������� ...");

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
//    ToConsol("�������������� �������� � ������� �������� ...");
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


void __fastcall TThreadCLQ::ExtractMaxClique2(
							s_t  *clique,
							ss_t &treangls,
							v_t  &treangls_degree,
							vs_t &list_adjacent
							)
{
	while (!IsFull(*clique,list_adjacent) && !clique->empty()){

		s_t::iterator min_it = clique->begin();

		for (s_t::iterator it = next_it(min_it); it != clique->end(); ++it)
			if (treangls_degree.at(*min_it) > treangls_degree.at(*it))
				min_it = it;
			else if (treangls_degree.at(*min_it) == treangls_degree.at(*it) &&
					 list_adjacent.at(*min_it).size() < list_adjacent.at(*it).size())
				min_it = it;

		// remove triangls
		ss_t treangls_tmp;
		for (ss_t::iterator it = treangls.begin(); it != treangls.end(); ++it) {
			if (it->find(*min_it) != it->end()){
				for (s_t::iterator jt = it->begin(); jt != it->end(); ++jt)
				--treangls_degree[*jt];
			} else {
				treangls_tmp.insert(*it);
			}
		}

		treangls.swap(treangls_tmp);

		// remove vertex from list
		list_adjacent[*min_it].clear();
		int min = *min_it;
		clique->erase(min_it);

		for (s_t::iterator it = clique->begin(); it != clique->end(); ++it)
			list_adjacent.at(*it).erase(min);
	}
}
//------------------------------------------------------------------------------


void __fastcall TThreadCLQ::ExtractMaxClique2Log(
							s_t  *clique,
							ss_t &treangls,
							v_t  &treangls_degree,
							vs_t &list_adjacent
							)
{
	while (!IsFull(*clique,list_adjacent) && !clique->empty()){

		Log += "������������: \n\n" + ToString(treangls) + "\n\n";
		Log += "�������: \n\n" + ToString(treangls_degree) + "\n\n";
		Log += "������ �����: \n\n" + ToString(list_adjacent) + "\n\n";
		Log += "������� �����: " + ToString(*clique) + "\n\n";

		s_t::iterator min_it = clique->begin();

		for (s_t::iterator it = next_it(min_it); it != clique->end(); ++it)
			if (treangls_degree.at(*min_it) > treangls_degree.at(*it))
				min_it = it;
			else if (treangls_degree.at(*min_it) == treangls_degree.at(*it) &&
					 list_adjacent.at(*min_it).size() < list_adjacent.at(*it).size())
				min_it = it;

		Log += "������� ������� � ����������� ��������: " + IntToStr(*min_it) + "\n\n";

		// remove triangls
		ss_t treangls_tmp;
		for (ss_t::iterator it = treangls.begin(); it != treangls.end(); ++it) {
			if (it->find(*min_it) != it->end()){
				for (s_t::iterator jt = it->begin(); jt != it->end(); ++jt)
				--treangls_degree[*jt];
			} else {
				treangls_tmp.insert(*it);
			}
		}

		treangls.swap(treangls_tmp);

		// remove vertex from list
		list_adjacent[*min_it].clear();
		int min = *min_it;
		clique->erase(min_it);

		for (s_t::iterator it = clique->begin(); it != clique->end(); ++it)
			list_adjacent.at(*it).erase(min);
	}
}
//------------------------------------------------------------------------------


bool __fastcall TThreadCLQ::IsFull(const s_t &v,const vs_t &lsa)
{
	for (s_t::const_iterator it = v.cbegin(); it != v.cend(); ++it)
		if (lsa.at(*it).size() != v.size() - 1)
			return false;

	return true;
}
//------------------------------------------------------------------------------


bool __fastcall TThreadCLQ::IsMerge(const s_t &clq, const s_t &trgl,const vs_t &lsa)
{

	for (s_t::iterator itb = trgl.begin(); itb != trgl.end(); ++itb) {
		++Q;
		if (clq.find(*itb) == clq.end())
			for (s_t::iterator ita = clq.begin(); ita != clq.end(); ++ita) {
				++Q;
				if (lsa.at(*ita).find(*itb) == lsa.at(*ita).end())
					return false;
			}
	}

	return true;
}
//------------------------------------------------------------------------------



void __fastcall TThreadCLQ::Sort(const vs_t &lsa,v_t *mv,v_t *mvr,vs_t *lsa_sort)
{
	bool is_swap;

	for (unsigned i = 0; i < mv->size(); ++i)
		mv->at(i) = i;

	for (unsigned i = 0; i < lsa.size(); ++i) {

		is_swap = false;

		for (unsigned j = 1; j < lsa.size() - 1; ++j)

			if (lsa[mv->at(j)].size() < lsa[mv->at(j + 1)].size()) {
				int tmp = mv->at(j);
				mv->at(j) = mv->at(j + 1);
				mv->at(j + 1) = tmp;
                is_swap = true;
			}

		if (!is_swap)
			break;
	}

	for (unsigned i = 0; i < mvr->size(); ++i)
		mvr->at(mv->at(i)) = i;

	lsa_sort->push_back(s_t());
	for (unsigned i = 1; i < lsa.size(); ++i) {
		s_t v;
		for (s_t::iterator it = lsa[mv->at(i)].begin(); it != lsa[mv->at(i)].end(); ++it)
			v.insert(mvr->at(*it));

		lsa_sort->push_back(v);
	}
}
//------------------------------------------------------------------------------



void __fastcall TThreadCLQ::LastChecked(const vs_t &lsa,s_t *clq)
{
	if (clq->empty())
		return;

	s_t last;
	for (unsigned i = 1; i < lsa.size(); ++i) {
        ++Q;
		if (clq->find(i) == clq->end())
			last.insert(i);
	}

	for (s_t::iterator it = last.begin(); it != last.end(); ++it) {
		bool isAdd = true;
		for (s_t::iterator it_clq = clq->begin(); it_clq != clq->end(); ++it_clq) {
			++Q;
			if (lsa.at(*it_clq).find(*it) == lsa.at(*it_clq).end()) {
				isAdd = false;
                break;
			}
		}

		if (isAdd)
            clq->insert(*it);
	}
}
