//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "TThreadCreateGraph.h"
#include "TFormMain.h"

#pragma package(smart_init)
//---------------------------------------------------------------------------
//   ����� ������� ���� ����� �� ���� ��������:
//
//       1. ������� ��������� ���� G � �����������:
//            -- ���������� ������ - N;
//            -- ���������� ����� - E = (A * N * (N - 1)) / 200;
//
//       2. ������� ���� G �� ��������� ������ ������ VertexStr
//
//       3. ����������� ������������ ���� G � ���� �� �����������
//
//---------------------------------------------------------------------------

__fastcall TThreadCreateGraph::TThreadCreateGraph(bool CreateSuspended)
	: TThread(CreateSuspended)
{
	FuncPoint[GEN_RAND] = GenerateRand;
	FuncPoint[GEN_HAND] = GenerateHand;
	FuncPoint[GEN_RE]   = GenerateRand;

	FuncExecut = NONE;
}
//---------------------------------------------------------------------------


inline void __fastcall TThreadCreateGraph::ToConsol(const AnsiString &Message)
{
	FormMain->ToConsol(Message);
	Application->ProcessMessages();
}
//------------------------------------------------------------------------------


void __fastcall TThreadCreateGraph::ToGraph()
{
	graph_t *Graph = new graph_t;

	Graph->FileName  = FileName;
	Graph->N         = N;
	Graph->A         = A;
	Graph->E         = E;
	Graph->Edges     = Edges;
	Graph->Matrix    = Matrix;
	Graph->Vertex    = Vertex;
	Graph->VertexAdd = VertexAdd;

    int Index = FormMain->ListBoxGraphs->ItemIndex;
	if (FuncExecut == GEN_RE)
		FormMain->Graphs[Index] = Graph;

	if (FuncExecut == GEN_RAND || FuncExecut == GEN_HAND) {
		FormMain->GraphToList(Graph);
		Index = FormMain->Graphs.size() - 1;
	}

	FormMain->ListBoxGraphs->ItemIndex = Index;
	FormMain->ListBoxGraphsClick(FormMain);

	Application->ProcessMessages();
}
//------------------------------------------------------------------------------


inline void __fastcall TThreadCreateGraph::Lock()
{
	FormMain->ActionsLock();
	FormMain->ThreadExecut = THR_GENERATE_GRAPH;
	Application->ProcessMessages();
}
//------------------------------------------------------------------------------


inline void __fastcall TThreadCreateGraph::UnLock()
{
	FormMain->ActionsUnLock();
	FormMain->ThreadExecut = THR_NONE;
	Application->ProcessMessages();
}
//------------------------------------------------------------------------------


void __fastcall TThreadCreateGraph::Execute()
{
	try {

		FreeOnTerminate = true;

		Synchronize(Lock);
		Synchronize(FuncPoint[FuncExecut]);

	} __finally {

		Synchronize(UnLock);
	}
}
//---------------------------------------------------------------------------


void __fastcall TThreadCreateGraph::GenerateRand()
{
	try {

		ToConsol(MESSAGE_CLEAR);
		ToConsol("create-rand-graph " + FileName);

		if (!(N > 0 && A > 0 && A <= 100)) {

			ToConsol("������! ����������� ������� ��������� �����.");
			ToConsol("���������� ������ ������ ���� ������ 0, ��������� �� 1 �� 100");
			ToConsol("���� �� ������.");

			return;
		}

		register int i = 0;    // ������ �����

		ToConsol("������� ��������� ������� ����� �����...");

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

		// ��������� ������������� ����������
		register int fVertex = 0;
		register int sVertex = 0;

		int EdgesRequir = (A * N * (N - 1)) / 200; // ��������� ���������� �����
		int EdgesCreate = 0;                       // ���������� ��������� �����

		v_t VertexNums(N); // ��������� ���� ������ �����, ��������� ���������� � �������� ������, ��� ����� 0 ���������� ������� 1 � �.�.
		VertexNums.reserve(N);

		iota(VertexNums.begin(), VertexNums.end(), 1);// ��������� ������ �� 1 �� ����� �������, �.� N

		// ������� ��������� ��������� �����
		TRandom *Random = new TRandom();

		// ��������� ��������� �����
		fVertex = Random->UniformFromVectorErase(VertexNums); // �������� ������ �������

		for (i = 1; i <= N - 1; ++i) {

			if (Terminated) {
				ToConsol("������� ����������! ���� �� ������.");
				return;
			}

			// �� ������� ������ �������� ����� ������� � ������� �� �� �������
			// ��� ������ ����������� ��������� ����� ����� ������� ������� �� ����� ��� �� �������
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

			// ��� ��������� ������� �����������, ��������� ����� ����� ���������
			// �� ��� �������������� �������
			fVertex = sVertex;

			// ������������ ���������� ��������� �����
			++EdgesCreate;

			ToConsol("�������� �����: " + IntToStr(EdgesCreate)
					+ "  /  " + IntToStr(EdgesRequir - EdgesCreate));
		}

		// ������������� �������� ��������� ��������� ����� �� 1 �� N - ���������� ������
		Random->SetUniform(1,N);

		// ������� ����� ����� �� ��������� ����������
		while (EdgesCreate < EdgesRequir) {
			// ���������� ��������� �������, ���� ���� ������ �� ����� ���� �����
			// ��� ��� �� ���� ������� ������
			do {

				if (Terminated) {
					ToConsol("������� ����������! ���� �� ������.");
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

			// ������������ ���������� ��������� �����
			++EdgesCreate;

			ToConsol("�������� �����: " + IntToStr(EdgesCreate)
					 + "  /  " + IntToStr(EdgesRequir - EdgesCreate));
		}

		// ���������� �������� ��������� �����
		E = Edges.size();
		A = (200 * E) / (N * (N - 1));

		// ���������� ��������� ����
		ToGraph();

		ToConsol("���� " + FileName + " - ������� ������.");

	} catch(...) {
		ToConsol("����������� ������! ���� �� ������.");
	}
}
//------------------------------------------------------------------------------


void __fastcall TThreadCreateGraph::GenerateHand()
{
	try {

		ToConsol(MESSAGE_CLEAR);
		ToConsol("create-hand-graph " + FileName);

		register int i;    // ������ �����

		for (i = 0; i < VertexStr.size(); ++i)
			ToConsol(VertexStr[i]);

		ToConsol("");

		// ������������� ��������� ������ ������� ������ � ����������

		if (VertexStr.size() == 0) {
			ToConsol("������! �� ����� ������ ������.\n���� �� ������.");
			return;
		}

		ToConsol("������������� ��������� ������ ������ ������ � ����������...");

		Vertex = ToInt(VertexStr);

		if (Vertex.size() == 0)
			return;

		ToConsol("��������� ������� ��������� � ���� ����������...");

		N = Vertex.size() - 1;

		s_t SetNums;
		for (i = 1; i <= N; ++i)
			SetNums.insert(i);

		VertexAdd = vs_t(N + 1,SetNums);
		VertexAdd[0].clear();
		VertexAdd.reserve(N + 1);

		Matrix = vv_t(N + 1,v_t(N + 1,0));

		for (i = 1; i <= N; ++i) {

			VertexAdd[i].erase(i);

			for (s_t::iterator it = Vertex[i].begin();
				 it != Vertex[i].end(); ++it)
			{
				if (Terminated) {
					ToConsol("������� ����������! ���� �� ������.");
					return;
				}

				if ((Matrix[i][*it] == 0)
					 && (*it != i))
				{
					Edges.push_back(s_t());
					Edges[Edges.size() - 1].insert(i);
					Edges[Edges.size() - 1].insert(*it);

					VertexAdd[i].erase(*it);
					VertexAdd[*it].erase(i);

					Matrix[i][*it] = 1;
					Matrix[*it][i] = 1;
				}
			}

			ToConsol("������������� ����� ��� �������: " + IntToStr(i));
		}

		// ���������� ��������� �����
		E = Edges.size();
		A = (200 * E) / (N * (N - 1));

		// ���������� ��������� ����
		ToGraph();
		ToConsol("���� " + FileName + " - ������� ������.");

	} catch(...) {
		ToConsol("����������� ������! ���� �� ������.");
	}
}
//---------------------------------------------------------------------------


vs_t __fastcall TThreadCreateGraph::ToInt(const vstr_t &Str)
{
	register int i;                       // ������ �����
	AnsiString Numbers = "0123456789";    // ������ ����� ��� �������������� � ���������� ������

	s_t   RowNums;      // ������ ���� �����
	s_t   VertexNums;   // ������ ���� ������
	map<int,set<int> > Incedens;     // ���� - ����� �������, �������� - ������ ������ � �������� ���� �����


	for (i = 0; i < Str.size(); ++i) {

		if (Terminated) {
			ToConsol("������� ����������! ���� �� ������.");
			return vs_t();
		}

		// ���������� ������ ������
		if (Str[i].Length() == 0)
			continue;

		register int Pos = 1;
		register int PosDiv = Str[i].Pos(":");

		AnsiString iVertStr;

		// ���������� ������ �������
		int iVertInt = -1;

		while (Pos < PosDiv && PosDiv <= Str[i].Length()) {

			if (Numbers.Pos(Str[i][Pos]) != 0)
				iVertStr += Str[i][Pos];
			else
				if (iVertStr != "")
					break;

			++Pos;
		}

		if (iVertStr != "") {

			// �������� �������� ����� �������
			iVertInt = StrToInt(iVertStr);
			VertexNums.insert(iVertInt);

			// ��������� ��� �� ������� � ����� �������
			int OldSize = RowNums.size();
			RowNums.insert(iVertInt);
			if (RowNums.size() == OldSize) {
				ToConsol("������! ����������� ����� ������ ������.\n"
						 + AnsiString("������ ������� � ������ - ") + IntToStr(i + 1)
				         + "\n���� �� ������.");

				return vs_t();
			}

		} else {

			 ToConsol("������! ����������� ����� ������ ������.\n"
					  + AnsiString("������ � ������ - ") + IntToStr(i + 1)
			          + "\n���� �� ������.");

			 return vs_t();
		}

		// ���������� ������ ������ � �������� ������� ������
		s_t Inc;

		iVertStr = "";

		Pos = PosDiv + 1;

		do {
			// �������� �����
			while (Pos <= Str[i].Length()) {

				if (Numbers.Pos(Str[i][Pos]) != 0)
					iVertStr += Str[i][Pos];
				else
					break;

				++Pos;
			}

			// ��������� ��������� �����
			if (iVertStr != "") {

				VertexNums.insert(StrToInt(iVertStr));
				Inc.insert(StrToInt(iVertStr));
				iVertStr = "";
			}

			// ���������� ����������� �����
			while (Pos <= Str[i].Length()) {

				if (Numbers.Pos(Str[i][Pos]) != 0)
					break;

				++Pos;
			}

		} while (Pos <= Str[i].Length());


		Incedens[iVertInt] = Inc;

	}    // end for i < Str.size()

	map<int,int> SortNums;

	// �������� ������� � ������� �����������
	s_t::iterator it = VertexNums.begin();
	for (i = 1; i <= VertexNums.size(); ++i) {
		SortNums[i] = *it;
		++it;
	}

	// ��������� ���������� ������� ���������
	vs_t vertex(VertexNums.size() + 1,s_t());

	for (int i = 1; i < VertexNums.size(); ++i) {

		for (s_t::iterator it = Incedens[SortNums[i]].begin();
			 it != Incedens[SortNums[i]].end(); ++it)
		{
			vertex[i].insert(SortNums[*it]);
			vertex[SortNums[*it]].insert(i);
		}
	}

	return vertex;
}
// ---------------------------------------------------------------------------


