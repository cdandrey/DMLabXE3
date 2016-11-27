//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "TThreadCreateGraph.h"
#include "TFormMain.h"

#pragma package(smart_init)
//---------------------------------------------------------------------------
//   Поток создает граф одним из трех способов:
//
//       1. Создает случайный граф G с параметрами:
//            -- количество вершин - N;
//            -- количество ребер - E = (A * N * (N - 1)) / 200;
//
//       2. Создает граф G по заданному списку вершин VertexStr
//
//       3. Пересоздает существующий граф G с теми же параметрами
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

			ToConsol("Ошибка! Неправильно заданны параметры графа.");
			ToConsol("Количество вершин должно быть больше 0, плотность от 1 до 100");
			ToConsol("Граф не создан.");

			return;
		}

		register int i = 0;    // индекс цикла

		ToConsol("Создаем случайным образом ребра графа...");

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

		// обьявляем промежуточные переменные
		register int fVertex = 0;
		register int sVertex = 0;

		int EdgesRequir = (A * N * (N - 1)) / 200; // требуемое количество ребер
		int EdgesCreate = 0;                       // количество созданных ребер

		v_t VertexNums(N); // множество всех вершин графа, нумерация начинается с нулевого номера, под номер 0 содержится вершина 1 и т.д.
		VertexNums.reserve(N);

		iota(VertexNums.begin(), VertexNums.end(), 1);// заполняем вектор от 1 до длины вектора, т.е N

		// создаем генератор случайных чисел
		TRandom *Random = new TRandom();

		// формируем связность графа
		fVertex = Random->UniformFromVectorErase(VertexNums); // выбираем первую вершину

		for (i = 1; i <= N - 1; ++i) {

			if (Terminated) {
				ToConsol("Процесс остановлен! Граф не создан.");
				return;
			}

			// из вектора вершин случайно берем вершину и удаляем ее из вектора
			// при каждой последующей генерации будет взята вершина которая до этого еще не бралась
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

			// для получения связной конструкции, следующее ребро будем соединять
			// из уже присоединенной вершины
			fVertex = sVertex;

			// подсчитываем количество созданных ребер
			++EdgesCreate;

			ToConsol("созданно ребер: " + IntToStr(EdgesCreate)
					+ "  /  " + IntToStr(EdgesRequir - EdgesCreate));
		}

		// устанавлисаем диапозон генерации случайных чисел от 1 до N - количество вершин
		Random->SetUniform(1,N);

		// создаем ребра графа до заданного количества
		while (EdgesCreate < EdgesRequir) {
			// генерируем случайные вершины, пока пара вершин не равна друг другу
			// или уже не была создана раньше
			do {

				if (Terminated) {
					ToConsol("Процесс остановлен! Граф не создан.");
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

			// подсчитываем количество созданных ребер
			++EdgesCreate;

			ToConsol("созданно ребер: " + IntToStr(EdgesCreate)
					 + "  /  " + IntToStr(EdgesRequir - EdgesCreate));
		}

		// определяем реальную плотность графа
		E = Edges.size();
		A = (200 * E) / (N * (N - 1));

		// возвращаем созданный граф
		ToGraph();

		ToConsol("Граф " + FileName + " - успешно создан.");

	} catch(...) {
		ToConsol("Неизвестная ошибка! Граф не создан.");
	}
}
//------------------------------------------------------------------------------


void __fastcall TThreadCreateGraph::GenerateHand()
{
	try {

		ToConsol(MESSAGE_CLEAR);
		ToConsol("create-hand-graph " + FileName);

		register int i;    // индекс цикла

		for (i = 0; i < VertexStr.size(); ++i)
			ToConsol(VertexStr[i]);

		ToConsol("");

		// преобразовуем строковый формат списока вершин в десятичный

		if (VertexStr.size() == 0) {
			ToConsol("Ошибка! Не задан список вершин.\nГраф не создан.");
			return;
		}

		ToConsol("Преобразовуем строковый формат списка вершин в десятичный...");

		Vertex = ToInt(VertexStr);

		if (Vertex.size() == 0)
			return;

		ToConsol("Формируем матрицу смежности и граф дополнение...");

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
					ToConsol("Процесс остановлен! Граф не создан.");
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

			ToConsol("сформированны связи для вершины: " + IntToStr(i));
		}

		// определяем плотность графа
		E = Edges.size();
		A = (200 * E) / (N * (N - 1));

		// возвращаем созданный граф
		ToGraph();
		ToConsol("Граф " + FileName + " - успешно создан.");

	} catch(...) {
		ToConsol("Неизвестная ошибка! Граф не создан.");
	}
}
//---------------------------------------------------------------------------


vs_t __fastcall TThreadCreateGraph::ToInt(const vstr_t &Str)
{
	register int i;                       // индекс цикла
	AnsiString Numbers = "0123456789";    // строка чисел для преобразования в десятичный формат

	s_t   RowNums;      // номера всех строк
	s_t   VertexNums;   // номера всех вершин
	map<int,set<int> > Incedens;     // ключ - номер вершины, значение - номера вершин с которыми есть связь


	for (i = 0; i < Str.size(); ++i) {

		if (Terminated) {
			ToConsol("Процесс остановлен! Граф не создан.");
			return vs_t();
		}

		// пропускаем пустые строки
		if (Str[i].Length() == 0)
			continue;

		register int Pos = 1;
		register int PosDiv = Str[i].Pos(":");

		AnsiString iVertStr;

		// определяем первую вершину
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

			// получаем значение новой вершины
			iVertInt = StrToInt(iVertStr);
			VertexNums.insert(iVertInt);

			// проверяем нет ли вершины с таким номером
			int OldSize = RowNums.size();
			RowNums.insert(iVertInt);
			if (RowNums.size() == OldSize) {
				ToConsol("Ошибка! Неправильно задан список вершин.\n"
						 + AnsiString("Повтор вершины в строке - ") + IntToStr(i + 1)
				         + "\nГраф не создан.");

				return vs_t();
			}

		} else {

			 ToConsol("Ошибка! Неправильно задан список вершин.\n"
					  + AnsiString("Ошибка в строке - ") + IntToStr(i + 1)
			          + "\nГраф не создан.");

			 return vs_t();
		}

		// определяем номера вершин с которыми связана первая
		s_t Inc;

		iVertStr = "";

		Pos = PosDiv + 1;

		do {
			// выделяем число
			while (Pos <= Str[i].Length()) {

				if (Numbers.Pos(Str[i][Pos]) != 0)
					iVertStr += Str[i][Pos];
				else
					break;

				++Pos;
			}

			// сохраняем найденное число
			if (iVertStr != "") {

				VertexNums.insert(StrToInt(iVertStr));
				Inc.insert(StrToInt(iVertStr));
				iVertStr = "";
			}

			// пропускаем разделитель числа
			while (Pos <= Str[i].Length()) {

				if (Numbers.Pos(Str[i][Pos]) != 0)
					break;

				++Pos;
			}

		} while (Pos <= Str[i].Length());


		Incedens[iVertInt] = Inc;

	}    // end for i < Str.size()

	map<int,int> SortNums;

	// нумеруем вершины в порядке возростания
	s_t::iterator it = VertexNums.begin();
	for (i = 1; i <= VertexNums.size(); ++i) {
		SortNums[i] = *it;
		++it;
	}

	// формируем десятичную матрицу смежности
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


