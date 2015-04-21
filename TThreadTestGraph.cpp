// ---------------------------------------------------------------------------
#pragma hdrstop

#include "TThreadTestGraph.h"
#include "TFormMain.h"

#pragma package(smart_init)
// ---------------------------------------------------------------------------

__fastcall TThreadTestGraph::TThreadTestGraph(bool CreateSuspended)
	: TThread(CreateSuspended)
{
	QueryPerformanceFrequency(&Freq);

	Algorithms[EQUA] = EquaSearchCover;	// 1
	Algorithms[FREQ] = FreqSearchCover; // 2
	Algorithms[FULL] = FullSearchCover; // 3
	Algorithms[INDS] = IndsSearchCover; // 4
	Algorithms[RANG] = RangSearchCover; // 5
	Algorithms[VERT] = VertSearchCover; // 6
	Algorithms[VPRE] = VpreSearchCover; // 7
	Algorithms[VREC] = VrecSearchCover;	// 8

	//	Algorithms[RSRV] = AbsbSearchCover;	- reserve
}
// ---------------------------------------------------------------------------


inline void __fastcall TThreadTestGraph::Lock()
{
	FormMain->ActionsLock();
	FormMain->ThreadExecut = THR_SEARCH_COVER;
	Application->ProcessMessages();
}
//------------------------------------------------------------------------------


inline void __fastcall TThreadTestGraph::UnLock()
{
	FormMain->ActionsUnLock();
	FormMain->ThreadExecut = THR_NONE;
	Application->ProcessMessages();
}
//------------------------------------------------------------------------------


inline void __fastcall TThreadTestGraph::SetParamGraph(unsigned int f,
										 			   unsigned int s,
													   unsigned int &_n,
													   unsigned int &_a,
													   unsigned int &_t)
{
	switch (TypeParams) {

	case CHANGE_N_A:

		_n = ParamFirst.at(f);
		_a = ParamSecond.at(s);
		_t = ParamConst;
		return;

	case CHANGE_N_T:

		_n = ParamFirst.at(f);
		_t = ParamSecond.at(s);
		_a = ParamConst;
		return;

	case CHANGE_A_T:

		_a = ParamFirst.at(f);
		_t = ParamSecond.at(s);
		_n = ParamConst;
		return;
	}
}
//------------------------------------------------------------------------------


void __fastcall TThreadTestGraph::ReturnErrorGraph()
{
	graph_t *Graph = new graph_t;

	Graph->FileName  = "ERROR_GRAPH_" + FileName;
	Graph->N         = Vertex.size() - 1;
	Graph->E         = Edges.size();
	Graph->A         = (200 * Graph->E) / (Graph->N * (Graph->N - 1));;
	Graph->Edges     = Edges;
	Graph->Matrix    = Matrix;
	Graph->Vertex    = Vertex;
	Graph->VertexAdd = VertexAdd;

	FormMain->PageControlMain->ActivePageIndex = 0;
	FormMain->GraphToList(Graph);
	FormMain->ListBoxGraphs->ItemIndex = FormMain->Graphs.size() - 1;
	FormMain->ListBoxGraphsClick(FormMain);

	Application->ProcessMessages();
}
//------------------------------------------------------------------------------


inline void __fastcall TThreadTestGraph::ToConsol(const AnsiString &Message)
{
	FormMain->ToConsol(Message);
	Application->ProcessMessages();
}


inline void __fastcall TThreadTestGraph::OpenAutoSaveFile(const AnsiString &Name)
{
	if (AutoSave) {
		AutoSaveFile.open(Name.c_str());
	}
}
//------------------------------------------------------------------------------


inline void __fastcall TThreadTestGraph::ToAutoSaveFile(const AnsiString &Data)
{
	if (AutoSave) {
		AutoSaveFile << Data.c_str() << endl;
	}
}
//------------------------------------------------------------------------------


inline void __fastcall TThreadTestGraph::ToAutoSaveFile(double Data)
{
	AutoSaveFile << Data << endl;
}
//------------------------------------------------------------------------------


inline void __fastcall TThreadTestGraph::ToAutoSaveFile(const vd_t &Data)
{
	for (int i = 0; i < Data.size(); ++i)
		AutoSaveFile << Data.at(i) << " ";

	AutoSaveFile << endl;
}
//------------------------------------------------------------------------------


void __fastcall TThreadTestGraph::Execute()
{
	try {

		// ���������� ������� ������� ������ �� ��������� ��� ������
		FreeOnTerminate = true;

		Synchronize(Lock);

		switch (TypeTest) {

		case TEST_MO:

				Synchronize(TestMO);
				break;

		//case TEST_CKO:

		  //		Synchronize(TestCKO);
			//	break;

		case TEST_COMP:

				Synchronize(TestCOMP);
				break;

		default: ;
		}

	} __finally {

		Synchronize(UnLock);
	}
}
//------------------------------------------------------------------------------


void __fastcall TThreadTestGraph::TestMO()
{
	try {

		ToConsol(MESSAGE_CLEAR);
		ToConsol("���������� ���������� �����: " + FileName);
		OpenAutoSaveFile(FileName);

		// ������ ����������� �������� ���������� ������������
		FormMain->Tests[TestIndex]->SuccefullMO = false;
		FormMain->ListViewTestType->Items->Item[0]->SubItems->Strings[0] = "";

		for (unsigned int i = 0; i < Alg.size(); ++i) {

			Result[Alg.at(i)] = vvvd_t(PARAM_COUNT,
								vvd_t(ParamFirst.size(),
								vd_t(ParamSecond.size(),0))
								);
		}

		register unsigned int TestNum = ParamFirst.size() *
										ParamSecond.size() *
										Count;	// ������� ������

		register int AlgNum; // ����� ������������ ���������

		AnsiString Message;	// ��������� ��������� � �������

		for (unsigned int f = 0; f < ParamFirst.size(); ++f) {

			for (unsigned int s = 0; s < ParamSecond.size(); ++s) {

				unsigned int n = 0;	// ���������� ������ �����
				unsigned int a = 0;	// ��������� �����
				unsigned int t = 0;	// ���������� ����� ���������� ���������

				// ���������� ��������� ������������� �����
				SetParamGraph(f,s,n,a,t);

				// ��������� Count ��� ����� ������������ ���������� �������� ��� ������ ������
				for (register unsigned int k = 0; k < Count; ++k) {

					if (Terminated)
						break;

					ToConsol(Message.sprintf("%d - ���������: %d %d %d",TestNum,n,a,t));
					ToConsol("\t��������� ����...");

					CreateGraph(n,a); // ������� ������� �����

					// ������� ����������� �������� ���������� ��������
					for (unsigned int i = 0; i < Alg.size(); ++i) {

						if (Terminated)
							break;

						AlgNum = Alg.at(i);

						Algorithms[AlgNum]();

						Result[AlgNum][QO][f][s] += Q;
						Result[AlgNum][TE][f][s] += TimeExecut;
						Result[AlgNum][RL][f][s] += (double) LenCover / n;
						Result[AlgNum][NS][f][s] += NumSets;


						if (LenMinCover != 0) {

							Result[AlgNum][RE][f][s]
								+= (double) (LenCover - LenMinCover) / LenMinCover;

							if (LenCover > LenMinCover)
								++Result[AlgNum][FE][f][s];
						}
					}    // for Alg

					-- TestNum;
				}	// for k

				// ��������� �������������� �������� ������������� ��� ������� ������
				for (unsigned int i = 0; i < Alg.size(); ++i) {

					AlgNum = Alg.at(i);

					for (unsigned int j = 0; j < PARAM_COUNT; ++j)
						Result[AlgNum][j][f][s] =
							(double) Result[AlgNum][j][f][s] / Count;

					if (Result[AlgNum][TE][f][s] > 0)
						Result[AlgNum][PE][f][s] =
							1 - exp(-((double) t / (1000 * Result[AlgNum][TE][f][s])));
				}
			}

			if (AutoSave) {

				ToAutoSaveFile(f);
				for (int i = 0; i < PARAM_COUNT; ++i)
					for (int j = 0; j < Alg.size(); ++j)
						ToAutoSaveFile(Result[Alg.at(j)][i][f]);
            }
		}

		// ���������� ���������� ������������
		FormMain->Tests[TestIndex]->MO = Result;
		FormMain->Tests[TestIndex]->SuccefullMO = true;
		FormMain->ListViewTestType->Items->Item[0]->SubItems->Strings[0] = "�������";
		FormMain->TestResultToRichEdit(TestIndex,TEST_MO);

		AutoSaveFile.close();
		ToConsol("������������ ���������� �������.");

	} catch (...) {
		ToConsol("����������� ������! ������������ ���������� � �������.");
		FormMain->Tests[TestIndex]->SuccefullMO = false;
		FormMain->ListViewTestType->Items->Item[0]->SubItems->Strings[0] = "";
		AutoSaveFile.close();
	}
}
//-----------------------------------------------------------------------------


void __fastcall TThreadTestGraph::TestCOMP()
{
	try {

		ToConsol(MESSAGE_CLEAR);
		ToConsol("���������� ���������� �����: " + FileName);

		register unsigned int TestNum = ParamFirst.size() *
										ParamSecond.size() *
										Count;	// ������� ������

		AnsiString Message;	// ��������� ��������� � �������

		for (unsigned int f = 0; f < ParamFirst.size(); ++f) {

			for (unsigned int s = 0; s < ParamSecond.size(); ++s) {

				unsigned int n = 0;	// ���������� ������ �����
				unsigned int a = 0;	// ��������� �����
				unsigned int t = 0;	// ���������� ����� ���������� ���������

				// ���������� ��������� ������������� �����
				SetParamGraph(f,s,n,a,t);

				// ��������� Count ��� ����� ������������ ���������� �������� ��� ������ ������
				for (register unsigned int k = 0; k < Count; ++k) {

					if (Terminated)
						break;

					ToConsol(Message.sprintf("%d - ���������: %d %d %d",TestNum,n,a,t));
					ToConsol("\t��������� ����...");

					CreateGraph(n,a); // ������� ������� �����

					// ������� ����������� �������� ���������� ��������
					Algorithms[Alg.at(0)]();
					unsigned int LenCoverLast = LenCover;

					for (unsigned int i = 1; i < Alg.size(); ++i) {

						if (Terminated)
							break;

						Algorithms[Alg.at(i)]();

						if (LenCoverLast != LenCover) {

							ToConsol("������� ������������� �������! ������������ ���������� �������.");
							ReturnErrorGraph();
							return;
						}
					}

					-- TestNum;
				}	// for k
			}
		}

		ToConsol("������������ ������� �� ����! ������������ ���������� �������.");

	} catch (...) {
		ToConsol("����������� ������! ������������ ���������� � �������.");
	}
}
//-----------------------------------------------------------------------------


void __fastcall TThreadTestGraph::CreateGraph(unsigned int N,unsigned int A)
{
	try {

		if (!(N > 0 && A > 0 && A <= 100)) {

			ToConsol("������! ����������� ������� ��������� �����.");
			return;
		}

		register unsigned int i = 0;

		Edges = vs_t();

		Matrix = vv_t(N + 1, v_t(N + 1,0));
		Matrix.reserve(N + 1);

		Vertex = vs_t(N + 1, s_t());
		Vertex.reserve(N + 1);

		s_t SetNums;
		for (i = 1; i <= N; ++i)
			SetNums.insert(i);

		VertexAdd = vs_t(N + 1,SetNums);
		VertexAdd[0].clear();
		VertexAdd.reserve(N + 1);

		for (i = 1; i <= N; ++i)
			VertexAdd[i].erase(i);

		register unsigned int fVertex = 0;
		register unsigned int sVertex = 0;

		unsigned int EdgesRequir = (A * N * (N - 1)) / 200;
		unsigned int EdgesCreate = 0;

		v_t VertexNums(N);
		VertexNums.reserve(N);

		iota(VertexNums.begin(), VertexNums.end(), 1);

		TRandom *Random = new TRandom();

		fVertex = Random->UniformFromVectorErase(VertexNums);

		for (i = 1; i <= N - 1; ++i) {

			if (Terminated) {
				ToConsol("������� ����������! ������������ ����������.");
				return;
			}

			sVertex = Random->UniformFromVectorErase(VertexNums);

			s_t NewEdge;
			NewEdge.insert(fVertex);
			NewEdge.insert(sVertex);
			Edges.push_back(NewEdge);

			Matrix[fVertex][sVertex] = 1;
			Matrix[sVertex][fVertex] = 1;

			Vertex[fVertex].insert(sVertex);
			Vertex[sVertex].insert(fVertex);

			VertexAdd[fVertex].erase(sVertex);
			VertexAdd[sVertex].erase(fVertex);

			fVertex = sVertex;

			++EdgesCreate;
		}

		Random->SetUniform(1,N);

		while (EdgesCreate < EdgesRequir) {
			do {

				if (Terminated) {
					ToConsol("������� ����������! ������������ ����������.");
					return;
				}

				fVertex = Random->Uniform();
				sVertex = Random->Uniform();

			} while ((Matrix[fVertex][sVertex] == 1) || (fVertex == sVertex));

			s_t NewEdge;
			NewEdge.insert(fVertex);
			NewEdge.insert(sVertex);
			Edges.push_back(NewEdge);

			Matrix[fVertex][sVertex] = 1;
			Matrix[sVertex][fVertex] = 1;

			Vertex[fVertex].insert(sVertex);
			Vertex[sVertex].insert(fVertex);

			VertexAdd[fVertex].erase(sVertex);
			VertexAdd[sVertex].erase(fVertex);

			++EdgesCreate;
		}

		//E = Edges.size();
		//A = (200 * E) / (N * (N - 1));  // �������� ��������� �����

	} catch(...) {
		ToConsol("����������� ������ �� ����� �������� �����!\n������������ ���������� � �������.");
	}
}
// ---------------------------------------------------------------------------


/**** ��������������� ������� ���������� ������ ���������� �������� ****/


void __fastcall TThreadTestGraph::SearchExtremCover(s_t &VertexSet_,vs_t &Vertex_,v_t &Cover_)
{
	if (VertexSet_.size() == 0)
		return;

	s_t::iterator it = VertexSet_.begin();

	while (it != VertexSet_.end()) {

		int Num    = *it;
		int Degree = Vertex_.at(Num).size();

		++Q;

		if (Degree == 1) {

			// '�������' �������
			int NumAdjacent = *Vertex_.at(Num).begin();
			Cover_.push_back(NumAdjacent);
			VertexErase(NumAdjacent,VertexSet_,Vertex_);

			it = VertexSet_.begin();

		} else if (Degree == VertexSet_.size() - 1) {

			// ������� ��������� �� ����� ��������� �����

			Cover_.push_back(Num);
			VertexErase(Num,VertexSet_,Vertex_);

			it = VertexSet_.begin();

		} else {

			++Q;
			++it;
		}
	}    // end while VertexSet
}
//---------------------------------------------------------------------------


void __fastcall TThreadTestGraph::SearchExtremIndep(
										s_t  &_VertexSet,
										vs_t &_Vertex,
										v_t  &_Indep
									)
{
	bool ExtremIs = false;

	++Q;

	if (_VertexSet.size() == 0)
		return;

	if (_VertexSet.size() == 1) {
		_Indep.push_back(*_VertexSet.begin());
		_Vertex[*_VertexSet.begin()].clear();
		_VertexSet.erase(*_VertexSet.begin());
		return;
	}

	register int Degree = -1;;
	register int Num = -1;
	int NumAdjacent = -1;

	s_t::iterator it = _VertexSet.begin();

	while (it != _VertexSet.end()) {

		Num    = *it;
		Degree = _Vertex.at(Num).size();

		++Q;

		switch (Degree) {

		case 0:

			++Q;

			_Indep.push_back(Num);
			_Vertex[Num].clear();
			it = _VertexSet.erase(it);

			break;

		case 1:

			ExtremIs = true;

			_Indep.push_back(Num);
			NumAdjacent = *_Vertex.at(Num).begin();
			VertexErase(NumAdjacent,_Vertex);
			_VertexSet.erase(NumAdjacent);
			_VertexSet.erase(Num);

			it = _VertexSet.begin();

			break;

		default:

			++Q;

			if (Degree == _VertexSet.size() - 1) {

				ExtremIs = true;

				VertexErase(Num,_Vertex);
				_VertexSet.erase(Num);
				it = _VertexSet.begin();

			} else {

				++it;
			}
		}
	}
}
//---------------------------------------------------------------------------


inline void TThreadTestGraph::VertexErase(
									int  I,
									vs_t &Vertex_
								)
{
	for (s_t::iterator itAdjacent = Vertex_.at(I).begin();
		 itAdjacent != Vertex_.at(I).end(); ++itAdjacent)
	{
		++Q;
		Vertex_[*itAdjacent].erase(I);
	}

	Vertex_[I].clear();
}
//---------------------------------------------------------------------------


inline void TThreadTestGraph::VertexErase(
									int  I,
									s_t  &_VertexSet,
									vs_t &_Vertex
								)
{
	for (s_t::iterator itAdjacent = _Vertex.at(I).begin();
		 itAdjacent != _Vertex.at(I).end(); ++itAdjacent)
	{
		++Q;
		_Vertex[*itAdjacent].erase(I);
		if (_Vertex.at(*itAdjacent).size() == 0)
			_VertexSet.erase(*itAdjacent);
	}

	_Vertex[I].clear();
	_VertexSet.erase(I);
}
//---------------------------------------------------------------------------


inline void TThreadTestGraph::VertexAdjacentErase(
									int   I,
									s_t   &VertexSet_,
									vs_t  &Vertex_
								)
{
	s_t VertexSetErase(Vertex_.at(I));
	for (s_t::const_iterator itErase = VertexSetErase.begin();
		 itErase != VertexSetErase.end(); ++itErase)
	{
		++Q;
		Vertex_[*itErase].erase(I);

		VertexErase(*itErase,VertexSet_,Vertex_);
	}

	Vertex_[I].clear();
	VertexSet_.erase(I);
}
//---------------------------------------------------------------------------


inline void TThreadTestGraph::VertexToIndep(
									int   I,
									s_t   &_VertexSet,
									vs_t  &_Vertex,
									v_t   &_Indep
								)
{
	for (s_t::iterator itErase = _Vertex.at(I).begin();
		 itErase != _Vertex.at(I).end(); ++itErase)
	{
		++Q;
		_Vertex[*itErase].erase(I);

		VertexErase(*itErase,_Vertex);
		_VertexSet.erase(*itErase);
	}

	_Indep.push_back(I);

	_Vertex[I].clear();
	_VertexSet.erase(I);
}
//---------------------------------------------------------------------------


inline int TThreadTestGraph::VpreMinDegree(const s_t &VertexSet,const vs_t &Vertex)
{
	register int Size;
	register int MinSize = 99999;
	register int MinCount = 0;

	for (s_t::const_iterator it = VertexSet.begin();
		 it != VertexSet.end(); ++it)
	{
		Size = Vertex.at(*it).size();
		if (MinSize > Size) {

			MinSize = Size;
			MinCount = 1;

		} else {

			if (MinSize == Size)
				++MinCount;
		}
	}

	return MinCount;
}
//---------------------------------------------------------------------------


bool TThreadTestGraph::IsCovered(const vs_t &Edg,const v_t &Cov)
{
	bool Is = false;

	for (unsigned i = 0; i < Edg.size(); ++i){

		Is = false;

		for (s_t::const_iterator it = Edg.at(i).begin();
			it != Edg.at(i).end(); ++it)
		{
			for (unsigned j = 0; j < Cov.size(); ++j){

				++Q;
				if (Cov.at(j) == *it){
					Is = true;
					break;
                }
			}

			if (Is)
				break;  // edge is covered, go to check next edge
		}

		if (!Is)
			break;	// edge is not covered,break check and return result
	}

	return Is;
}
//---------------------------------------------------------------------------


/**** ������� ������ ������������ ��������� �������� ����� ****/

//// - reserve methods
//void __fastcall TThreadTestGraph::RsrvSearchCover()
//{
//	try {
//
//		ToConsol("\t����������� ����� ����������...");
//
//		Q = 0;
//		TimeExecut = 0;
//		LenCover = 0;
//		NumSets = 0;
//
//		LARGE_INTEGER TimeBegin;
//		LARGE_INTEGER TimeEnd;
//
//		QueryPerformanceCounter(&TimeBegin);
//
//		unsigned n = Vertex.size() - 1;
//
//		//v_t Cover;
//
//		//LenCover = Cover.size();
//
//		QueryPerformanceCounter(&TimeEnd);
//
//		TimeExecut = (double)(TimeEnd.QuadPart - TimeBegin.QuadPart) / Freq.QuadPart;
//
//	} catch (...){
//		ToConsol("������ ���������� ������ ����������! ������������ ���������� � �������.");
//	}
//}
////------------------------------------------------------------------------------


void __fastcall TThreadTestGraph::EquaSearchCover()
{
	try {

		ToConsol("\t����������� ��������� ����� 3...");

		Q = 0;
		TimeExecut = 0;
		LenCover = 0;
		NumSets = 0;

		LARGE_INTEGER TimeBegin;
		LARGE_INTEGER TimeEnd;

		QueryPerformanceCounter(&TimeBegin);

		unsigned n = Vertex.size() - 1;
		s_t VertexSet;
		for (int i = 1; i <= n; ++i)
			VertexSet.insert(i);

		vs_t Vertex_(Vertex);
		vv_t CoverTemp;
		v_t Cover;

		v_t HEdgesCount(3,0);
		v_t HCoverLen(3,0);

		int EdgesNum = 0;
		int Step = 1;

		while ((EdgesNum = EdgesCalculate(VertexSet,Vertex_)) > 0) {

			SearchExtremCover(VertexSet,Vertex_,Cover);

			if (Terminated)
				break;

			int v1 = Max(VertexSet,Vertex_);
			VertexSet.erase(v1);
			int v2 = Max(VertexSet,Vertex_);

			if (v1 == -1 || v2 == -1)
				break;

			VertexSet.insert(v1);

			vs_t HVertexSet(3,VertexSet);
			vvs_t HVertex(3,Vertex_);
			vv_t HCover(3,Cover);

			HCover[0].push_back(v1);
			HCover[0].push_back(v2);
			VertexErase(v1,HVertexSet[0],HVertex[0]);
			VertexErase(v2,HVertexSet[0],HVertex[0]);
			SearchExtremCover(HVertexSet[0],HVertex[0],HCover[0]);

			HCover[1].push_back(v1);
			VertexErase(v1,HVertexSet[1],HVertex[1]);
			copy(HVertex.at(1).at(v2).begin(),HVertex.at(1).at(v2).end(),back_inserter(HCover[1]));
			VertexAdjacentErase(v2,HVertexSet[1],HVertex[1]);
			SearchExtremCover(HVertexSet[1],HVertex[1],HCover[1]);

			HCover[2].push_back(v2);
			VertexErase(v2,HVertexSet[2],HVertex[2]);
			copy(HVertex.at(2).at(v1).begin(),HVertex.at(2).at(v1).end(),back_inserter(HCover[2]));
			VertexAdjacentErase(v1,HVertexSet[2],HVertex[2]);
			SearchExtremCover(HVertexSet[2],HVertex[2],HCover[2]);

			for (unsigned int i = 0; i < HEdgesCount.size(); ++i){
				HEdgesCount[i] = EdgesCalculate(HVertexSet.at(i),HVertex.at(i));
				HCoverLen[i] = HCover.at(i).size();
				if (HEdgesCount.at(i) == 0)
					CoverTemp.push_back(HCover.at(i));
			}

			int HMax = Max(Edges.size(),0,1,HEdgesCount,HCoverLen);
			HMax = Max(Edges.size(),HMax,2,HEdgesCount,HCoverLen);

			VertexSet.swap(HVertexSet.at(HMax));
			Vertex_.swap(HVertex.at(HMax));
			Cover.swap(HCover.at(HMax));
		}

		for (unsigned i = 0; i < CoverTemp.size(); ++i){

			v_t ICover(CoverTemp.at(i));
			ICover.reserve(CoverTemp.at(i).size());

			for (unsigned j = 0; j < ICover.size(); ++j){

				int tmp = *ICover.begin();
				ICover.erase(ICover.begin());

				if (IsCovered(Edges,ICover)){
					CoverTemp[i] = ICover;
					break;
				}

				ICover.push_back(tmp);
			}
		}

		int MinCoverLen = INFIN;
		for (int i = 0; i < CoverTemp.size(); ++i)
			if (MinCoverLen > CoverTemp.at(i).size())
				MinCoverLen = CoverTemp.at(i).size();

		if (MinCoverLen > Cover.size())
			MinCoverLen = Cover.size();

		QueryPerformanceCounter(&TimeEnd);

		LenCover = MinCoverLen;

		QueryPerformanceCounter(&TimeEnd);

		TimeExecut = (double)(TimeEnd.QuadPart - TimeBegin.QuadPart) / Freq.QuadPart;

	} catch (...){
		ToConsol("������ ���������� ���������� ������ 3! ������������ ���������� � �������.");
	}
}
//------------------------------------------------------------------------------


inline int TThreadTestGraph::EdgesCountRemove(int I,const vs_t& Vertex_)
{
	int CountRemove = 0;
	int CountIntersection = 0;
	s_t VertexInc(Vertex_.at(I));
	for (s_t::const_iterator it = VertexInc.begin(); it != VertexInc.end(); ++it){
		CountRemove += Vertex_.at(*it).size();
		s_t Result;
		set_intersection(VertexInc.begin(),VertexInc.end(),
						 Vertex_.at(*it).begin(),Vertex_.at(*it).end(),
						 inserter(Result,Result.begin()));

		CountIntersection += Result.size();
	}

	return CountRemove - (CountIntersection / 2);
}
//---------------------------------------------------------------------------


inline int TThreadTestGraph::Max(const s_t &VertexSet,const vs_t &Vertex)
{
	int MaxVertex = -1;
	int MaxDegree = 0;

	for (s_t::const_iterator it = VertexSet.begin(); it != VertexSet.end(); ++it)
		if (MaxDegree < Vertex.at(*it).size())
			MaxVertex = *it,
			MaxDegree = Vertex.at(*it).size();

	Q += VertexSet.size();

	return MaxVertex;
}
//---------------------------------------------------------------------------


inline int TThreadTestGraph::Max(int I1,int I2,const v_t &EdgesCount,const v_t &CoverLen)
{
	Q += 5;

	if (EdgesCount.at(I1) == 0)
		return I2;
	else if (EdgesCount.at(I2) == 0)
		return I1;
	else if (EdgesCount.at(I1) > EdgesCount.at(I2))
		return I1;
	else if (EdgesCount.at(I1) < EdgesCount.at(I2))
		return I2;
	else
		return CoverLen.at(I1) > CoverLen.at(I2) ? I2 : I1;
}
//---------------------------------------------------------------------------


inline int TThreadTestGraph::Max(int EdgesCountBegin,int I1,int I2,const v_t &EdgesCount,const v_t &CoverLen)
{
	Q += 5;

	double Rate1 = static_cast<double>(EdgesCountBegin - EdgesCount.at(I1)) / CoverLen.at(I1);
	double Rate2 = static_cast<double>(EdgesCountBegin - EdgesCount.at(I2)) / CoverLen.at(I2);

	if (EdgesCount.at(I1) == 0)
		return I2;
	else if (EdgesCount.at(I2) == 0)
		return I1;
	else if (Rate1 > Rate2)
		return I1;
	else if (Rate1 < Rate2)
		return I2;
	else
		return CoverLen.at(I1) > CoverLen.at(I2) ? I2 : I1;
}
//---------------------------------------------------------------------------


void __fastcall TThreadTestGraph::FreqSearchCover()
{
	try {

		ToConsol("\t����������� ��������� �����...");

		Q = 0;
		TimeExecut = 0;
		LenCover = 0;
		NumSets = 0;

		LARGE_INTEGER TimeBegin;
		LARGE_INTEGER TimeEnd;

		QueryPerformanceCounter(&TimeBegin);

		unsigned n = Vertex.size() - 1;

		s_t VertexSet;
		for (unsigned i = 1; i <= n; ++i)
			VertexSet.insert(i);

		v_t Cover;
		vs_t VertexInc(Vertex);

		while (VertexSet.size() > 0) {

			if (Terminated)
				return;

			SearchExtremCover(VertexSet,VertexInc,Cover);

			if (VertexSet.size() == 0)
				break;

			vector<multiset<int> > Degree(n + 1,multiset<int>());

			for (s_t::iterator it = VertexSet.begin();
				 it != VertexSet.end(); ++it)
			{
				for (s_t::iterator iit = VertexInc.at(*it).begin();
					iit != VertexInc.at(*it).end(); ++iit)
				{
					Degree[*it].insert(VertexInc.at(*iit).size());
				}
			}

			unsigned CountMax = 0;
			unsigned VertexMax = 0;
			unsigned CountDegree = 2;

			while (CountMax == 0) {

				if (Terminated)
					return;

				for (unsigned i = 1; i <= n; ++i) {

					Q += Degree.at(i).size();

					unsigned CountCurrent = count(Degree.at(i).begin(),
												  Degree.at(i).end(),
												  CountDegree);

					++Q;

					if (CountMax < CountCurrent) {

						CountMax = CountCurrent;
						VertexMax = i;
					}
				}

				++CountDegree;
			}

			Cover.push_back(VertexMax);

			VertexErase(VertexMax,VertexSet,VertexInc);
		}

		LenCover = Cover.size();

		QueryPerformanceCounter(&TimeEnd);

		TimeExecut = (double)(TimeEnd.QuadPart - TimeBegin.QuadPart) / Freq.QuadPart;

	} catch (...){
		ToConsol("������ ���������� ���������� ������! ������������ ���������� � �������.");
	}
}
//------------------------------------------------------------------------------


void __fastcall TThreadTestGraph::FullSearchCover()
{
	try {

		ToConsol("\t����������� ����� �������� ������� ��������...");

		Q = 0;
		TimeExecut = 0;
		LenCover = 0;
		NumSets = 0;

		LARGE_INTEGER TimeBegin;
		LARGE_INTEGER TimeEnd;

		QueryPerformanceCounter(&TimeBegin);

		unsigned int n = Vertex.size() - 1;

		s_t VertexSet;
		for (unsigned int i = 1; i <= n; ++i)
			VertexSet.insert(i);

		v_t MaxIndep;

		for (unsigned int i = 1; i <= n; ++i) {

			if (Terminated)
				return;

			v_t Indep = FullSearchIndep(i,VertexSet,Vertex,v_t());

			++Q;

			if (MaxIndep.size() < Indep.size())
				MaxIndep = Indep;
		}

		LenCover = n - MaxIndep.size();

		if (LenMinCover == 0)
			LenMinCover = LenCover;

		QueryPerformanceCounter(&TimeEnd);
		TimeExecut = (double)(TimeEnd.QuadPart - TimeBegin.QuadPart) / Freq.QuadPart;

	} catch (...){
		ToConsol("������ ���������� ������ �������� ��������! ������������ ���������� � �������.");
	}
}
//---------------------------------------------------------------------------


v_t __fastcall TThreadTestGraph::FullSearchIndep(
									   int  I,
									   s_t  VertexSet,
									   vs_t Vertex,
									   v_t  Indep
									)
{
	VertexToIndep(I,VertexSet,Vertex,Indep);

	SearchExtremIndep(VertexSet,Vertex,Indep);

	v_t MaxIndep(Indep);

	for (s_t::iterator it = VertexSet.begin();
		 it != VertexSet.end(); ++it)
	{
		if (Terminated)
			return v_t();

		v_t _Indep = FullSearchIndep(*it,VertexSet,Vertex,Indep);

		++Q;

		if (_Indep.size() > MaxIndep.size())
			MaxIndep = _Indep;
	}

    return MaxIndep;
}
//---------------------------------------------------------------------------


void __fastcall TThreadTestGraph::IndsSearchCover()
{
	try {

		ToConsol("\t����������� ����� ����������� ��������...");

		Q = 0;
		TimeExecut = 0;
		LenCover = INFIN;
		NumSets = 0;

		LARGE_INTEGER TimeBegin;
		LARGE_INTEGER TimeEnd;

		QueryPerformanceCounter(&TimeBegin);

		set<pair<s_t,s_t> > Sets;

		unsigned n = Vertex.size() - 1;

		unsigned FullSetsSize = 0;
		unsigned FullSetsSizePrev = 0;

		for (unsigned i = 1; i <= n; ++i)
			for (s_t::iterator it = VertexAdd.at(i).begin();
				 it != VertexAdd.at(i).end(); ++it)
			{
				if (*it > i) {

					pair<s_t,s_t> S;

					S.second.insert(i);
					S.second.insert(*it);

					set_union(Vertex.at(i).begin(),Vertex.at(i).end(),
							  Vertex.at(*it).begin(),Vertex.at(*it).end(),
							  inserter(S.first, S.first.begin()));
					Q += (Vertex.at(i).size() + Vertex.at(*it).size());

					if ((S.first.size() + S.second.size()) == n) {
						++FullSetsSize;
						if (S.first.size() < LenCover)
							LenCover = S.first.size();
					} else {
						Sets.insert(S);
					}
				}
		   }

		unsigned SetsSize = Sets.size();
		unsigned SetsSizePrev = 0;

		while (
			   (SetsSize > 0) &&
			   (SetsSize != SetsSizePrev) &&
			   (FullSetsSize != FullSetsSizePrev || FullSetsSize == 0)
			  )
		{
			SetsSizePrev = SetsSize;
			FullSetsSizePrev = FullSetsSize;

			set<pair<s_t,s_t> > NewSets = Sets;

			bool Absorb = false;

			for (set<pair<s_t,s_t> >::iterator it = Sets.begin();
				it != Sets.end(); ++it)
			{
				set<pair<s_t,s_t> >::iterator it_next = it;

				++it_next;

				pair<s_t,s_t> S;
				S.first = it->first;

				unsigned CountIdent = 0;

				for (; it_next != Sets.end(); ++it_next) {

					if (Terminated)
						return;

					if (it->first == it_next->first) {

						Absorb = true;

						if (CountIdent == 0)
							NewSets.erase(*it);

						NewSets.erase(*it_next);

						set_union(it->second.begin(), it->second.end(),
							it_next->second.begin(), it_next->second.end(),
							inserter(S.second, S.second.begin()));

						if (S.second.size() + S.first.size() == n) {

							++FullSetsSize;
							if (S.first.size() < LenCover)
								LenCover = S.first.size();

							Absorb = false;
						}

						Q += it->second.size() + it_next->second.size();

						++CountIdent;

					} else {
						break;
					}
				}

				if (CountIdent > 0 && Absorb)
					NewSets.insert(S);

				for (int i = 0; i < CountIdent; ++i)
					++it;
			}

			Sets = NewSets;

			NewSets.clear();

			for (set<pair<s_t,s_t> >::iterator it = Sets.begin();
				 it != --Sets.end(); ++it)
			{
				set<pair<s_t,s_t> >::iterator it_next = it;
				++it_next;

				for (;it_next != Sets.end(); ++it_next) {

					if (Terminated)
						return;

					s_t Intersect;

					set_intersection(it->first.begin(), it->first.end(),
							it_next->second.begin(), it_next->second.end(),
							inserter(Intersect, Intersect.begin()));

					Q += (it->first.size() + it_next->second.size());

					if (Intersect.size() == 0) {

						set_intersection(it->second.begin(), it->second.end(),
								it_next->first.begin(), it_next->first.end(),
								inserter(Intersect, Intersect.begin()));

						Q += (it->second.size() + it_next->first.size());

						if (Intersect.size() == 0) {

							pair<s_t,s_t> S;

							set_union(it->first.begin(), it->first.end(),
									it_next->first.begin(), it_next->first.end(),
									inserter(S.first, S.first.begin()));

							set_union(it->second.begin(), it->second.end(),
									it_next->second.begin(), it_next->second.end(),
									inserter(S.second, S.second.begin()));

							if ((S.first.size() + S.second.size()) == n) {
								++FullSetsSize;
								if (S.first.size() < LenCover)
									LenCover = S.first.size();
							} else {
								NewSets.insert(S);
							}
						}
					}
				}
			}

			Sets = NewSets;
			SetsSize = Sets.size();

		}

		NumSets = FullSetsSize;

		if (LenMinCover == 0)
			LenMinCover = LenCover;

		QueryPerformanceCounter(&TimeEnd);
		TimeExecut = (double)(TimeEnd.QuadPart - TimeBegin.QuadPart) / Freq.QuadPart;

	} catch (...){
		ToConsol("������ ���������� ������ ����������� ��������! ������������ ���������� � �������.");
	}
}
//---------------------------------------------------------------------------


void __fastcall TThreadTestGraph::RangSearchCover()
{
	try {

		ToConsol("\t����������� �������� �����...");

		Q = 0;
		TimeExecut = 0;
		LenCover = 0;
		LenMinCover = 0;
		NumSets = 0;

		LARGE_INTEGER TimeBegin;
		LARGE_INTEGER TimeEnd;

		QueryPerformanceCounter(&TimeBegin);

		register int RowCount = Edges.size();

		register int ColCount = Vertex.size() - 1;

		vs_t Rows(RowCount + 1,s_t());
		Rows.reserve(RowCount + 1);

		vs_t Cols(ColCount + 1,s_t());
		Cols.reserve(ColCount + 1);

		register int i;

		for (i = 0; i < Edges.size(); ++i) {

			Cols[*Edges[i].begin()].insert(i + 1);
			Cols[*(++Edges[i].begin())].insert(i + 1);

			Rows[i + 1] = Edges[i];
		}

		Q = ColCount;

		vp_t ColsSort(ColCount + 1,p_t(0,0));
		for (i = 1; i <= ColCount; ++i) {
			ColsSort[i].first = Cols[i].size();
			ColsSort[i].second = i;
		}

		Q += (2 * ColCount);

		stable_sort(ColsSort.begin() + 1,ColsSort.end());

		reverse(ColsSort.begin() + 1,ColsSort.end());

		vvs_t ColsUnion(ColCount + 1,vs_t(2,s_t()));
		ColsUnion.reserve(ColCount + 1);

		for (i = 1; i <= ColCount; ++i)
		ColsUnion[i][1].insert(ColsSort[i].second);

		vv_t HeightBegin(ColCount + 1,v_t(RowCount + 1, 0));
		HeightBegin.reserve(ColCount + 1);

		vvv_t HeightUnion(ColCount + 1,vv_t(2,v_t(RowCount + 1,0)));
		HeightUnion.reserve(ColCount + 1);

		Q += RowCount;

		for (i = 1; i <= RowCount; ++i) {
			++Q;

			if (!Rows[i].erase(ColsSort[ColCount].second)) {

				HeightBegin[ColCount][i] = INFIN;
				HeightBegin[ColCount][0] = INFIN;

				HeightUnion[ColCount][1][i] = INFIN;
				HeightUnion[ColCount][1][0] = INFIN;
			} else {
				Rows[i].insert(ColsSort[ColCount].second);
			}
		}

		for (int col = ColCount - 1; col >= 1; --col) {

			s_t weight;

			for (int row = 1; row <= RowCount; ++row) {

				++Q;

				if (Rows[row].erase(ColsSort[col].second)) {

					Rows[row].insert(ColsSort[col].second);

					HeightBegin[col][row] = 0;
					HeightUnion[col][1][row] = 0;

				} else {

					++Q;

					if (HeightBegin[col + 1][row] == INFIN) {
						HeightBegin[col][row] = INFIN;
						HeightBegin[col][0] = INFIN;

						HeightUnion[col][1][row] = INFIN;
						HeightUnion[col][1][0] = INFIN;
					} else {
						HeightBegin[col][row]    = HeightBegin[col + 1][row] + 1;
						HeightUnion[col][1][row] = HeightBegin[col][row];

						++Q;

						if (HeightBegin[col][0] != INFIN) {
							weight.insert(HeightBegin[col][row]);
							HeightBegin[col][0] = weight.size();
							HeightUnion[col][1][0] = HeightBegin[col][0];
						}
					}
				}
			}
		}

		for (i = 1; i <= ColCount; ++i)
			if (HeightBegin[i][0] == 0) {
				QueryPerformanceCounter(&TimeEnd);
				TimeExecut = (double)(TimeEnd.QuadPart - TimeBegin.QuadPart) / Freq.QuadPart;
				LenCover = 1;
				if (i != ColCount)
					++LenCover;

				LenMinCover = LenCover;

				return;
			}

		Q += ColCount;

		for (register int rang = 1; rang < ColCount; ++rang) {

			vvv_t HeightNewUnion(ColCount + 1,vv_t(1,v_t(RowCount + 1,0)));
			HeightNewUnion.reserve(ColCount + 1);

			vvs_t ColsNewUnion(ColCount + 1,vs_t(1,s_t()));
			ColsNewUnion.reserve(ColCount + 1);

			for (register int col = rang + 1; col <= ColCount; ++col) {

				int num_curr_union = 0;

				for (register int col_prev = rang; col_prev < col; ++col_prev) {

					for (register int union_i = 1; union_i < HeightUnion[col_prev].size(); ++union_i) {

						if (Terminated)
							return;

						ColsNewUnion[col].push_back(ColsUnion[col_prev][union_i]);
						num_curr_union = ColsNewUnion[col].size() - 1;
						ColsNewUnion[col][num_curr_union].insert(ColsSort[col].second);

						HeightNewUnion[col].push_back(v_t(RowCount + 1,0));

						s_t ValuesHeight;

						Q += RowCount;

						for (int row = 1; row <= RowCount; ++row) {

							if (HeightUnion[col_prev][union_i][row] != 0) {
								HeightNewUnion[col][num_curr_union][row] = HeightBegin[col][row];
							}

							ValuesHeight.insert(HeightNewUnion[col][num_curr_union][row]);
						}

						++Q;

						if (*ValuesHeight.rbegin() == INFIN) {

							HeightNewUnion[col].pop_back();
							ColsNewUnion[col].pop_back();

						} else {

							++Q;

							if (*ValuesHeight.begin() == 0 && ValuesHeight.size() == 1) {

								QueryPerformanceCounter(&TimeEnd);
								TimeExecut = (double)(TimeEnd.QuadPart - TimeBegin.QuadPart) / Freq.QuadPart;

								LenCover = ColsNewUnion[col][num_curr_union].size();
								LenMinCover = LenCover;

								return;

							} else {

								HeightNewUnion[col][num_curr_union][0] = ValuesHeight.size() - 1;
							}
						}
					}
				}
			}

			HeightUnion = HeightNewUnion;
			ColsUnion = ColsNewUnion;

		}

	} catch (...){
		ToConsol("������ ���������� ��������� ������!  ������������ ���������� � �������.");
	}
}
//---------------------------------------------------------------------------


void __fastcall TThreadTestGraph::VertSearchCover()
{
	try {

		ToConsol("\t����������� ����� ������...");

		Q = 0;
		TimeExecut = 0;
		LenCover = 0;
		NumSets = 0;

		LARGE_INTEGER TimeBegin;
		LARGE_INTEGER TimeEnd;

		QueryPerformanceCounter(&TimeBegin);

		unsigned int n = Vertex.size() - 1;

		s_t VertexSet;
		for (int i = 1; i <= n; ++i)
			VertexSet.insert(i);

		v_t Indep;

		for (int i = 1; i <= n; ++i) {

			if (Terminated)
				return;

			s_t  _VertexSet(VertexSet);
			vs_t _Vertex(Vertex);
			v_t  _Indep;

			VertexToIndep(i,_VertexSet,_Vertex,_Indep);

			while (_VertexSet.size() > 0) {

				SearchExtremIndep(_VertexSet,_Vertex,_Indep);

				if (_VertexSet.size() == 0) {

					break;

				} else {

					int Max = 0;
					int MaxNum = 0;
					s_t MaxSet;

					for (s_t::iterator it = _VertexSet.begin();
						 it != _VertexSet.end(); ++it)
					{
						if (_Vertex.at(*it).size() > Max) {
							MaxNum = *it;
							MaxSet.clear();
							MaxSet.insert(*it);
						} else {
							if (_Vertex.at(*it).size() == Max)
								MaxSet.insert(*it);
						}
					}

					if (MaxSet.size() > 1) {

						int MaxExtremCount = 0;

						for (s_t::iterator it_max = MaxSet.begin();
							 it_max != MaxSet.end(); ++it_max)
						{

							int ExtremCount = 0;

							for (s_t::iterator it_adj = _Vertex.at(*it_max).begin();
								 it_adj != _Vertex.at(*it_max).end(); ++it_adj)
							{
								if (_Vertex.at(*it_adj).size() == 2)
									++ExtremCount;
							}

							if (ExtremCount > MaxExtremCount) {
								MaxNum = *it_max;
								MaxExtremCount = ExtremCount;
							}
						}
					}

					VertexErase(MaxNum,_Vertex);
					_VertexSet.erase(MaxNum);
				}

			}

			if (Indep.size() < _Indep.size())
				Indep = _Indep;

		}

		LenCover = n - Indep.size();

		QueryPerformanceCounter(&TimeEnd);
		TimeExecut = (double)(TimeEnd.QuadPart - TimeBegin.QuadPart) / Freq.QuadPart;

	} catch (...){
		ToConsol("������ ���������� ������ ������! ������������ ���������� � �������.");
	}
}
//---------------------------------------------------------------------------


void __fastcall TThreadTestGraph::VpreSearchCover()
{
	try {
		ToConsol("\t����������� ����� ������ � ���������...");

		Q = 0;
		TimeExecut = 0;
		LenCover = 0;
		NumSets = 0;

		LARGE_INTEGER TimeBegin;
		LARGE_INTEGER TimeEnd;

		unsigned n = Vertex.size() - 1;

		s_t VertexSet;
		for (int i = 1; i <= n; ++i)
			VertexSet.insert(i);

		v_t Indep;

		for (int i = 1; i <= n; ++i) {

			if (Terminated)
				return;

			s_t  _VertexSet(VertexSet);
			vs_t _Vertex(Vertex);
			v_t  _Indep;

			v_t  MaxW1;

			int VertexInclud = i;

			while (_VertexSet.size() > 0) {

				VertexToIndep(VertexInclud,_VertexSet,_Vertex,_Indep);

				SearchExtremIndep(_VertexSet,_Vertex,_Indep);

				int VertexMaxW1 = 0;
				int MaxWeightW2 = 0;
				int MaxSumW = 0;

				for (s_t::iterator it = _VertexSet.begin();
					 it != _VertexSet.end(); ++it)
				{
					if (Terminated)
						return;

					s_t  VertexW2(_VertexSet);
					vs_t W2(_Vertex);
					v_t  W1(_Indep);

					VertexToIndep(*it,VertexW2,W2,W1);

					SearchExtremIndep(VertexW2,W2,W1);

					int WeightW2 = VpreMinDegree(VertexW2,W2);
					int SumW   = W1.size() + WeightW2;

					++Q;
					if (SumW > MaxSumW) {

						MaxSumW = SumW;
						MaxWeightW2 = WeightW2;
						VertexInclud = *it;
					} else {

						++Q;
						if (SumW == MaxSumW) {

							++Q;
							if ((MaxWeightW2 == 0) && (WeightW2 > 0)) {
								VertexInclud = *it;
								MaxWeightW2 = WeightW2;
							}
						}
					}

					++Q;
					if (W1.size() > MaxW1.size()) {

						MaxW1 = W1;
						VertexMaxW1 = *it;
					}
				}
			}

			if (_Indep.size() < MaxW1.size())
				_Indep = MaxW1;

			if (Indep.size() < _Indep.size())
				Indep = _Indep;

		}

		LenCover = n - Indep.size();

		QueryPerformanceCounter(&TimeEnd);
		TimeExecut = (double)(TimeEnd.QuadPart - TimeBegin.QuadPart) / Freq.QuadPart;

	} catch (...){
		ToConsol("������ ���������� ������ ������ � ���������! ������������ ���������� � �������.");
	}
}
//---------------------------------------------------------------------------


void __fastcall TThreadTestGraph::VrecSearchCover()
{
	try {

		ToConsol("\t����������� ����� ������ � ���������...");

		Q = 0;
		TimeExecut = 0;
		LenCover = 0;
		NumSets = 0;

		LARGE_INTEGER TimeBegin;
		LARGE_INTEGER TimeEnd;

		QueryPerformanceCounter(&TimeBegin);

		unsigned int n = Vertex.size() - 1;

		s_t VertexSet;
		for (unsigned int i = 1; i <= n; ++i)
			VertexSet.insert(i);

		v_t MaxIndep;

		for (unsigned int i = 1; i <= n; ++i) {

			if (Terminated)
				return;

			v_t Indep = VrecSearchIndep(i,VertexSet,Vertex,v_t());

			if (MaxIndep.size() < Indep.size())
				MaxIndep = Indep;

		}

		LenCover = n - MaxIndep.size();

		QueryPerformanceCounter(&TimeEnd);
		TimeExecut = (double)(TimeEnd.QuadPart - TimeBegin.QuadPart) / Freq.QuadPart;

	} catch (...){
		ToConsol("������ ���������� ������ ������ � ���������! ������������ ���������� � �������.");
	}
}
//---------------------------------------------------------------------------


v_t __fastcall TThreadTestGraph::VrecSearchIndep(
									   int  I,
									   s_t  VertexSet,
									   vs_t Vertex,
									   v_t  Indep
									)
{
	VertexToIndep(I,VertexSet,Vertex,Indep);

	SearchExtremIndep(VertexSet,Vertex,Indep);

	v_t MaxIndep(Indep);

	if (VertexSet.size() > 0) {

		v_t  MaxW1;
		int VertexMaxW1 = 0;

		int MaxSumW = 0;
		s_t VertexMaxSumW;

		for (s_t::iterator it = VertexSet.begin();
			 it != VertexSet.end(); ++it)
		{
			if (Terminated)
				return v_t();

			s_t  VertexW2(VertexSet);
			vs_t W2(Vertex);
			v_t  W1(Indep);

			VertexToIndep(*it,VertexW2,W2,W1);

			SearchExtremIndep(VertexW2,W2,W1);

			int WeightW2 = VpreMinDegree(VertexW2,W2);
			int SumW = W1.size() + WeightW2;

			++Q;
			if (SumW > MaxSumW) {

				MaxSumW = SumW;
				VertexMaxSumW.clear();
				VertexMaxSumW.insert(*it);

			} else {

				++Q;
				if (SumW == MaxSumW)
					VertexMaxSumW.insert(*it);
			}

			++Q;
			if (W1.size() > MaxW1.size()) {

				MaxW1 = W1;
				VertexMaxW1 = *it;
			}
		}    // end for it

		for (s_t::iterator it = VertexMaxSumW.begin();
			 it != VertexMaxSumW.end(); ++it)
		{
			v_t _Indep = VrecSearchIndep(*it,VertexSet,Vertex,Indep);

			if (_Indep.size() > MaxIndep.size())
				MaxIndep = _Indep;
		}

		if (MaxIndep.size() < MaxW1.size())
			MaxIndep = MaxW1;
	}

    return MaxIndep;
}
//---------------------------------------------------------------------------





