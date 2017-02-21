﻿//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "TThreadSearchCover.h"
#include "TFormMain.h"
#include <deque>
#pragma package(smart_init)
//---------------------------------------------------------------------------
//   Поток выполняет поиск миниматльного вершинного покрытия графа, одним
//   из алгоритмов:
//
//       1. SearchCoverRang - ранговый метод
//       2. SearchCoverFull - метод быстрого полного перебора
//       3. SearchCoverInds - метод независимых множеств
//       4. SearchCoverNind - новый метод независимых множеств
//       5. SearchCoverNinu - новый метод независимых множеств с объединением
//       6. SearchCoverFreq - частотный метод
//       7. SearchCoverVert - метод вершин
//       8. SearchCoverAbsb - метод поглащений
//       9. SearchCoverEqua - метод уравнений
//
//       . SearchCoverVpre - метод вершин с прогнозом
//       . SearchCoverVrec - метод вершин с прогнозом и рекурсией
//
//    данные возвращаются с номер в списке GraphIndex
//---------------------------------------------------------------------------

__fastcall TThreadSearchCover::TThreadSearchCover(bool CreateSuspended)
	: TThread(CreateSuspended)
{
	QueryPerformanceFrequency(&Freq);

	FuncExecut = NONE;

	FuncPoint[FREQ] = FreqSearchCover;
	FuncPoint[FULL] = FullSearchCover;
	FuncPoint[INDS] = IndsSearchCover;
	FuncPoint[RANG] = RangSearchCover;
	FuncPoint[VERT] = VertSearchCover;
	FuncPoint[EQUA] = TreeSearchCover;
	FuncPoint[NIND] = NindSearchCover;
	FuncPoint[NINU] = NinuSearchCover;
}
//------------------------------------------------------------------------------


void __fastcall TThreadSearchCover::Execute()
{
	try {

		FreeOnTerminate = true;

		Synchronize(Lock);

		for (int i = 0; i < ListFuncExecut.size(); ++i) {
			FuncExecut = ListFuncExecut[i];
			Synchronize(FuncPoint[FuncExecut]);
		}

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


void __fastcall TThreadSearchCover::UnLock()
{
	FormMain->ActionsUnLock();
	FormMain->ThreadExecut = THR_NONE;
	Application->ProcessMessages();
}
//------------------------------------------------------------------------------


inline void TThreadSearchCover::ToConsol(const AnsiString &Message)
{
	FormMain->ToConsol(Message);
	Application->ProcessMessages();
}
//------------------------------------------------------------------------------


void TThreadSearchCover::ToCover()
{
	double Time = (double)(TimeEnd.QuadPart - TimeBegin.QuadPart) / Freq.QuadPart;

	sort(Cover.begin(),Cover.end());

	FormMain->Graphs[GraphIndex]->ParamCovers[FuncExecut].Q
		= FloatToStr(Q);
	FormMain->Graphs[GraphIndex]->ParamCovers[FuncExecut].T
		= FloatToStr(Time);

	FormMain->Graphs[GraphIndex]->ParamCovers[FuncExecut].Cover
		= ToString(CoverToIndep(Cover));
	FormMain->Graphs[GraphIndex]->ParamCovers[FuncExecut].LenCover
		= IntToStr(static_cast<int>(N - Cover.size()));

	FormMain->Graphs[GraphIndex]->ParamCovers[FuncExecut].LogShort
		= LogShort;

	FormMain->Graphs[GraphIndex]->ParamCovers[FuncExecut].LogShort
		+= "  -- Количество операций:\t";
	FormMain->Graphs[GraphIndex]->ParamCovers[FuncExecut].LogShort
		+= FormMain->Graphs[GraphIndex]->ParamCovers[FuncExecut].Q;

	FormMain->Graphs[GraphIndex]->ParamCovers[FuncExecut].LogShort
		+= "\n  -- Время выполнения, мс:\t";
	FormMain->Graphs[GraphIndex]->ParamCovers[FuncExecut].LogShort
		+= FormMain->Graphs[GraphIndex]->ParamCovers[FuncExecut].T;

	FormMain->Graphs[GraphIndex]->ParamCovers[FuncExecut].LogShort
		+= "\n  -- Длина множества:\t\t";
	FormMain->Graphs[GraphIndex]->ParamCovers[FuncExecut].LogShort
		+= FormMain->Graphs[GraphIndex]->ParamCovers[FuncExecut].LenCover;

	FormMain->Graphs[GraphIndex]->ParamCovers[FuncExecut].LogShort
		+= "\n  -- Максимальное независимое множество:\t";
	FormMain->Graphs[GraphIndex]->ParamCovers[FuncExecut].LogShort
		+= FormMain->Graphs[GraphIndex]->ParamCovers[FuncExecut].Cover;

	FormMain->Graphs[GraphIndex]->ParamCovers[FuncExecut].LogShort
		+= Note;

	FormMain->Graphs[GraphIndex]->ParamCovers[FuncExecut].Log = Log;

	// выводим отчеты в список и редактор
	FormMain->ParamCoversToListView();

	if (FormMain->ActionAlgLogView->Checked) {

		FormMain->RichEditLog->Lines->Clear();
		FormMain->RichEditLog->Lines->Append(FormMain->Graphs[GraphIndex]->ParamCovers[FuncExecut].LogShort);
		FormMain->RichEditLog->Lines->Append("\n");
		FormMain->RichEditLog->Lines->Append(FormMain->Graphs[GraphIndex]->ParamCovers[FuncExecut].Log);
	} else {
		FormMain->ParamCoversToRichEdit();
	}

	Application->ProcessMessages();
}
//------------------------------------------------------------------------------

v_t TThreadSearchCover::CoverToIndep(const v_t &cover)
{
	v_t indep;
	for (int i = 1; i <= N; ++i)
		if (find(cover.begin(),cover.end(),i) == cover.end())
			indep.push_back(i);

	return indep;
}
//------------------------------------------------------------------------------


/**** функции преобразования данных в текст ****/


AnsiString __fastcall TThreadSearchCover::ToString(const vp_t &Data)
{
	AnsiString Str = "";
	AnsiString Buffer = "";

	for (unsigned int i = 1; i < Data.size(); ++i)
		Str += Buffer.sprintf("%3d (%3d) ",i,Data.at(i).second);

	Str += "\n";

	return Str;
}
//---------------------------------------------------------------------------


AnsiString __fastcall TThreadSearchCover::ToString(int ColCount,const vv_t &H)
{
	AnsiString Str = "";
	AnsiString Buffer = "";

	for (int col = 1; col < ColCount; ++col) {

		Str += Buffer.sprintf("\n\tB + %3d + \t= ",col);

		int HSize = H.at(col).size();
		for (int row = 1; row < HSize; ++row) {

			if (H.at(col).at(row) != 0) {
				Str += Buffer.sprintf("%3d ",0);
			} else {
				Str += Buffer.sprintf("%3d ",row);
			}
		}

		Str += Buffer.sprintf("\n\tH + %3d + \t= ",col);
		for (int row = 1; row < HSize; ++row) {

			if (H.at(col).at(row) != 0) {
				if (H.at(col).at(row) != INFIN)
					Str += Buffer.sprintf("%3d ",H.at(col).at(row));
				else
					Str += Buffer.sprintf("%3s ",'#');
			} else {
				Str += Buffer.sprintf("%3d ",0);
			}
		}

		Str += Buffer.sprintf("\n\td + %3d + \t= ",col);

		if (H.at(col).at(0) == INFIN)
			Str += "#\n";
		else
			Str += Buffer.sprintf("%3d \n",H.at(col).at(0));
	}

	Str += "\n";

	return Str;
}
//---------------------------------------------------------------------------


AnsiString __fastcall TThreadSearchCover::ToString(
											  AnsiString Tab,
											  const s_t  &SetData,
											  const vs_t &VecSetData
										  )
{
	AnsiString Str = "";
	AnsiString Buffer = "";

	for (s_t::const_iterator it = SetData.begin(); it != SetData.end(); ++it) {

		Str += Tab + Buffer.sprintf("%3d (%3d) : ",*it,VecSetData.at(*it).size());
		Str += ToString(VecSetData.at(*it)) + "\n";
	}

	if (Str == "")
		Str = "{}";

	return Str;
}
//---------------------------------------------------------------------------


AnsiString __fastcall TThreadSearchCover::ToString(AnsiString Tab,const vs_t &Data)
{
	AnsiString Str = "";
	AnsiString Buffer = "";

	for (int i = 1; i < Data.size(); ++i) {

		Str += Tab
				+ Buffer.sprintf("%3d (%3d) : ",i,Data.at(i).size())
				+ ToString(Data.at(i)) + "\n";
	}

	return Str;
}
//---------------------------------------------------------------------------


AnsiString __fastcall TThreadSearchCover::ToString(AnsiString Tab,const ss_t &Data)
{
	AnsiString Str = "";
	AnsiString Buffer = "";

	int i = 1;
	for (ss_t::const_iterator it = Data.begin(); it != Data.end(); ++it) {

		Str += Tab
				+ Buffer.sprintf("%3d (%3d) : ",i,it->size())
				+ ToString(it.operator *()) + "\n";

		++i;
	}

	return Str;
}
//---------------------------------------------------------------------------


AnsiString __fastcall TThreadSearchCover::ToString(AnsiString Tab,const ss8_t &Data)
{
	AnsiString Str = "";
	AnsiString Buffer = "";

	int i = 1;
	for (ss8_t::const_iterator it = Data.begin(); it != Data.end(); ++it) {

		Str += Tab
				+ Buffer.sprintf("%3d (%3d) : ",i,it->size())
				+ ToString(it.operator *()) + "\n";

		++i;
	}

	return Str;
}
//---------------------------------------------------------------------------


AnsiString __fastcall TThreadSearchCover::ToString(const v_t &Data)
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


AnsiString __fastcall TThreadSearchCover::ToString(AnsiString infin,const v_t &Data)
{
	AnsiString Str = "";
	AnsiString Buffer = "";

	for (int i = 0; i < Data.size(); ++i)
		if (Data.at(i) == INFIN)
			Str += infin + " ";
		else
			Str += Buffer.sprintf("%3d ",Data.at(i));

	return Str.Trim();
}
//---------------------------------------------------------------------------


AnsiString __fastcall TThreadSearchCover::ToString(const s_t &Data)
{
	AnsiString Str = "";
	AnsiString Buffer = "";

	for (s_t::const_iterator it = Data.begin(); it != Data.end(); ++it)
        Str += Buffer.sprintf("%3d ",*it);

	return Str.Trim();
}
//---------------------------------------------------------------------------


AnsiString __fastcall TThreadSearchCover::ToString(const s8_t &Data)
{
	AnsiString Str = "";
	AnsiString Buffer = "";

	for (s8_t::const_iterator it = Data.begin(); it != Data.end(); ++it)
        Str += Buffer.sprintf("%3d ",*it);

	return Str.Trim();
}
//---------------------------------------------------------------------------


AnsiString __fastcall TThreadSearchCover::ToString(const pair<s_t,s_t> &Data)
{
	AnsiString Buffer = "";

	return   ToString(Data.first)
		   + Buffer.sprintf(" (%3d) \t\t-\t\t",Data.first.size())
		   + ToString(Data.second)
		   + Buffer.sprintf(" (%3d)\n",Data.first.size());
}
//---------------------------------------------------------------------------


AnsiString __fastcall TThreadSearchCover::ToString(const set<pair<s_t,s_t> > &Data)
{
	AnsiString Str = "";
	AnsiString Buffer = "";

	int k = 0;

	for (set<pair<s_t,s_t> >::const_iterator it = Data.begin();it != Data.end(); ++it)
		Str += Buffer.sprintf("   %3d.   ",++k) +  ToString(*it) + "\n";

	return Str;
}
//---------------------------------------------------------------------------


AnsiString __fastcall TThreadSearchCover::ToString(const vector<path_t> &Tree)
{
	AnsiString Str = "";
	AnsiString Buffer = "";

	int k = 0;

	for (int i = 0; i < Tree.size(); ++i) {
		s8_t z;
		for (int j = 1; j <= N; ++j)
			if (Tree[i].x.find(j) == Tree[i].x.end() && Tree[i].y.find(j) == Tree[i].y.end())
				z.insert(j);

		Str += Buffer.sprintf("\t%3d)\t",i) +
			   ToString(Tree[i].x) + " | " +
			   ToString(Tree[i].y) + " | " +
			   ToString(z) + "\n";
	}

	return Str;
}
//---------------------------------------------------------------------------


AnsiString __fastcall TThreadSearchCover::ToString(int count)
{
	AnsiString tab = "\t";

	for (int i = 0; i < count; ++i)
		tab += tab;

	return tab;
}
//---------------------------------------------------------------------------


AnsiString __fastcall TThreadSearchCover::ToStringEdges(const s_t  &VertexSet,
														const vs_t &Vertex_,
														const AnsiString &Tab,
														bool Numeration)
{
	AnsiString Str = "";
	AnsiString StrNum = "";

	int Num = 1;
	for (s_t::const_iterator it = VertexSet.begin(); it != VertexSet.end(); ++it)
		for (s_t::const_iterator itt = Vertex_.at(*it).begin(); itt != Vertex_.at(*it).end(); ++itt)
			if (*itt > *it) {

				if (Numeration)
					StrNum = IntToStr(Num) + ". ";

				Str += StrNum + "(" + IntToStr(*it) + ", " + IntToStr(*itt) + ")" + Tab;

				++Num;
			}

	Str.Delete(Str.Length() - Tab.Length() + 1,Tab.Length());

	if (Str == "")
		Str = "{}";

	return Str;
}
//---------------------------------------------------------------------------


AnsiString __fastcall TThreadSearchCover::ToStringVertex(const s_t  &VertexSet,
												const vs_t &Vertex_,
												const AnsiString &Tab,
												bool Short)
{
	AnsiString Str = "";

	for (s_t::const_iterator it = VertexSet.begin(); it != VertexSet.end(); ++it) {

		Str += IntToStr(*it);
		Str += "(" + IntToStr((int)Vertex_.at(*it).size()) + ")";
		Str = Short ? Str + Tab : Str + " : " + ToString(Vertex_.at(*it)) + Tab;
	}

	Str.Delete(Str.Length() - Tab.Length() + 1,Tab.Length());

	if (Str == "")
		Str = "{}";

	return Str;
}
//---------------------------------------------------------------------------



/**** вспомогательные функции алгоритмов поиска вершинного покрытия ****/


inline v_t TThreadSearchCover::CoverFromIndep(s_t VertexSet,const v_t &Indep)
{
	register int i;
	register int IndepSize = Indep.size();
	for (i = 0; i < IndepSize; ++i)
		VertexSet.erase(Indep.at(i));

    i = 0;
	v_t _Cover(VertexSet.size(),0);
	for (s_t::iterator it = VertexSet.begin();it != VertexSet.end(); ++it) {
		 _Cover[i] = *it;
		 ++i;
	}

	return _Cover;
}
//---------------------------------------------------------------------------

// функция производит поиск экстримальных вершин(изолированные, висячие,
// связанные со всеми) и включает их в вершинное покрытие
void __fastcall TThreadSearchCover::SearchExtremCover(
										AnsiString  LogRowBegin,
										s_t         &VertexSet_,
										vs_t        &Vertex_,
										v_t         &Cover_
									)
{
	// признак что были найдены связанные со всеми или висячие вершины
	// что бы обновлять или не обновлять список вершин в логе
	bool IsExtrem = false;

	if (VertexSet_.size() == 0)
		return;

	s_t::iterator it = VertexSet_.begin();

	while (it != VertexSet_.end()) {

		int Num    = *it;
		int Degree = Vertex_.at(Num).size();

		++Q;

		if (Degree == 1) {

			// 'висячая' вершина
			IsExtrem = true;

			Log += (LogRowBegin + "  -- 'висячая' : "
					+ IntToStr(Num) + " - удаляем\n");
			Log += (LogRowBegin + "  -- смежная с 'висячей' : "
					+ IntToStr(*Vertex_.at(Num).begin())
					+ " - включаем в вершинное покрытие\n");

			int NumAdjacent = *Vertex_.at(Num).begin();
			Cover_.push_back(NumAdjacent);
			VertexErase(NumAdjacent,VertexSet_,Vertex_);

			it = VertexSet_.begin();

		} else if (Degree == VertexSet_.size() - 1) {

			// вершина связанная со всеми вершинами графа

			IsExtrem = true;

			Log += (LogRowBegin
					+ "  -- вершина связанна со всеми вершинами графа : "
					+ IntToStr(Num) + " - добавляем в покрытие\n");

			Cover_.push_back(Num);

			VertexErase(Num,VertexSet_,Vertex_);
			it = VertexSet_.begin();

		} else {

			++Q;
			++it;
		}
	}    // end while VertexSet

//	if (IsExtrem && VertexSet_.size() > 0) {
//		Log += (LogRowBegin + "  -- текущий подграф : \n");
//		Log += ToString(LogRowBegin + "        ",VertexSet_,Vertex_);
//	}

	if (!IsExtrem)
		Log += LogRowBegin + "  -- экстримальные вершины отсутсвуют\n";
}
//---------------------------------------------------------------------------


void __fastcall TThreadSearchCover::SearchExtremCover(
										s_t         &VertexSet_,
										vs_t        &Vertex_,
										v_t         &Cover_
									)
{
	if (VertexSet_.size() == 0)
		return;

	s_t::iterator it = VertexSet_.begin();

	while (it != VertexSet_.end()) {

		int Num    = *it;
		int Degree = Vertex_.at(Num).size();

		++Q;

		if (Degree == 1) {

			// 'висячая' вершина
			int NumAdjacent = *Vertex_.at(Num).begin();
			Cover_.push_back(NumAdjacent);
			VertexErase(NumAdjacent,VertexSet_,Vertex_);

			it = VertexSet_.begin();

		} else if (Degree == VertexSet_.size() - 1) {

			// вершина связанная со всеми вершинами графа

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


// функция производит поиск экстримальных вершин(изолированные, висячие,
// связанные со всеми) и включает их в независимое множество
void __fastcall TThreadSearchCover::SearchExtremIndep(
										AnsiString  LogRowBegin,
										s_t         &_VertexSet,
										vs_t        &_Vertex,
										v_t         &_Indep
									)
{
	// признак что были найдены связанные со всеми или висячие вершины
	// что бы обновлять или не обновлять список вершин в логе
	bool IsExtrem = false;

	++Q;

	if (_VertexSet.size() == 0)
		return;

	Log += LogRowBegin + "  -- находим эктримальные вершины\n";

	register int Degree = -1;;
	register int Num = -1;
	int NumAdjacent = -1;

	s_t::iterator it = _VertexSet.begin();

	while (it != _VertexSet.end()) {

		Num    = *it;
		Degree = _Vertex.at(Num).size();

		++Q;

		switch (Degree) {

		// изолированная вершина
		case 0:

			++Q;

			Log += (LogRowBegin + "  -- изолированная вершина : "
					+ IntToStr(Num) + " - включаем в независимое множество\n");

			_Indep.push_back(Num);
			_Vertex[Num].clear();
			it = _VertexSet.erase(it);

			break;

		// 'висячая' вершина
		case 1:

			IsExtrem = true;

			Log += (LogRowBegin + "  -- 'висячая' вершина : "
					+ IntToStr(Num) + " - включаем в независимое множество\n");
			Log += (LogRowBegin + "  -- вершина смежная с 'висячей' : "
					+ IntToStr(*_Vertex.at(Num).begin()) + " - удаляем\n");

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

				// вершина связанная со всеми вершинами графа

				IsExtrem = true;

				Log += (LogRowBegin + "  -- вершина связанна со всеми вершинами графа : "
						+ IntToStr(Num) + " - удаляем\n");

				VertexErase(Num,_Vertex);
				_VertexSet.erase(Num);
				it = _VertexSet.begin();

			} else {

				++it;
			}
		}    // end switch Degree
	}    // end while VertexSet

	if (IsExtrem && _VertexSet.size() > 0) {
		Log += (LogRowBegin + "  -- текущий подграф : \n");
		Log += ToString(LogRowBegin + "        ",_VertexSet,_Vertex);
	}
}
//---------------------------------------------------------------------------


void __fastcall TThreadSearchCover::SearchExtremIndep(
										s_t         &_VertexSet,
										vs_t        &_Vertex,
										v_t         &_Indep
									)
{
	// признак что были найдены связанные со всеми или висячие вершины
	// что бы обновлять или не обновлять список вершин в логе
	bool IsExtrem = false;

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

		// изолированная вершина
		case 0:

			++Q;

			_Indep.push_back(Num);
			_Vertex[Num].clear();
			it = _VertexSet.erase(it);

			break;

		// 'висячая' вершина
		case 1:

			IsExtrem = true;

			_Indep.push_back(Num);
			NumAdjacent = *_Vertex.at(Num).begin();
			VertexErase(NumAdjacent,_Vertex);
			_VertexSet.erase(NumAdjacent);
			_VertexSet.erase(Num);

			it = _VertexSet.begin();

			break;

		default:

		// вершина связанная со всеми вершинами графа

			++Q;

			if (Degree == _VertexSet.size() - 1) {

				IsExtrem = true;

				VertexErase(Num,_Vertex);
				_VertexSet.erase(Num);
				it = _VertexSet.begin();

			} else {

				++it;
			}
		}    // end switch Degree
	}    // end while VertexSet
}
//---------------------------------------------------------------------------


inline void TThreadSearchCover::VertexErase(
									int  I,
									vs_t &_Vertex
								)
{
	for (s_t::iterator itAdjacent = _Vertex.at(I).begin();
		 itAdjacent != _Vertex.at(I).end(); ++itAdjacent)
	{
		++Q;
		_Vertex[*itAdjacent].erase(I);
	}

	_Vertex[I].clear();
}
//---------------------------------------------------------------------------


inline void TThreadSearchCover::VertexErase(
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


inline void TThreadSearchCover::VertexAdjacentErase(
									int   I,
									s_t   &VertexSet_,
									vs_t  &Vertex_
								)
{
	// удаляем все вершины с которыми связана вершина I
	s_t VertexSetErase(Vertex_.at(I));
	for (s_t::const_iterator itErase = VertexSetErase.begin();
		 itErase != VertexSetErase.end(); ++itErase)
	{
		// удалим связь вершины itErase с вершиной I, что бы избежать пересечения
		++Q;
		Vertex_[*itErase].erase(I);

		// удаляем вершину itErase
		VertexErase(*itErase,VertexSet_,Vertex_);
	}

	// удоляем вершину I
	Vertex_[I].clear();
	VertexSet_.erase(I);
}
//---------------------------------------------------------------------------


inline void TThreadSearchCover::VertexToIndep(
									int   I,
									s_t   &_VertexSet,
									vs_t  &_Vertex,
									v_t   &_Indep
								)
{
	// удаляем все вершины с которыми связана вершина I
	for (s_t::iterator itErase = _Vertex.at(I).begin();
		 itErase != _Vertex.at(I).end(); ++itErase)
	{
		// удалим связь вершины itErase с вершиной I, что бы избежать пересечения
		++Q;
		_Vertex[*itErase].erase(I);

		// удаляем вершину itErase
		VertexErase(*itErase,_Vertex);
		_VertexSet.erase(*itErase);
	}

	// добовляем вершину I в независимое множество
	_Indep.push_back(I);

	// удоляем вершину I
	_Vertex[I].clear();
	_VertexSet.erase(I);
}
//---------------------------------------------------------------------------


bool TThreadSearchCover::IsCovered(const vs_t &Edg,const v_t &Cov)
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


/**** функции поиска минимального вершиного покрытия графа ****/


void __fastcall TThreadSearchCover::FreqSearchCover()
{
	try {

		ToConsol(MESSAGE_CLEAR);
		ToConsol("search-cover freq " + FileName);

		if (Vertex.size() == 0) {
			ToConsol("Ошибка! Не задан граф. Минимальное покрытие не найдено.");
			return;
		}

		// задаем начальные данные характеристикам алгоритма
		Q        = 0;
		Cover    = v_t();
		LogShort = "ЧАСТОТНЫЙ МЕТОД\n\n";
		Log      = "Пошаговый отчет работы алгоритма: \n\n";
		QueryPerformanceCounter(&TimeBegin);

		// промежуточные данные
		ToConsol("Формируются промежуточные данные...");

		//создаем множество всех вершин графа
		s_t VertexSet;
		for (unsigned i = 1; i <= N; ++i)
			VertexSet.insert(i);

		// переходим к поиску минимального вершинного покрытия
		unsigned Step = 1;
		AnsiString LogBegin = "";
		vs_t VertexInc(Vertex);

		while (VertexSet.size() > 0) {

			if (Terminated) {
				ToConsol("Процесс остановлен! Минимальное покрытие не найдено.");
				return;
			}

			// поиск экстримальных вершин

			LogBegin = "." + IntToStr((int)Step);
			Log += LogBegin + "  -- текущий подграф: \n"
				   + ToString(LogBegin + "        ",VertexSet,VertexInc);
			Log += LogBegin + "  -- поиск экстримальных вершин\n";

			ToConsol("шаг " + LogBegin + " поиск экстримальных вершин");

			SearchExtremCover(LogBegin,VertexSet,VertexInc,Cover);
			Log += LogBegin + "  -- текущий подграф: \n"
				   + ToString(LogBegin + "        ",VertexSet,VertexInc);

			if (VertexSet.size() == 0)
				break;

			// поиск вершины связаной с наибольшим числом вершин,
			// которые имеют минимальную степень (частота)
			ToConsol("шаг " + LogBegin
					 + " поиск вершины с наибольшей частотой");

			vector<multiset<int> > Degree(N + 1,multiset<int>());

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

				if (Terminated) {
					ToConsol("Процесс остановлен! Минимальное покрытие не найдено.");
					return;
				}

				for (unsigned i = 1; i <= N; ++i) {

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

			Log += LogBegin
				   + "  -- вершина с максимальной частотой: "
				   + IntToStr((int)VertexMax) + " - добавляем в покрытие\n";

			Cover.push_back(VertexMax);

			VertexErase(VertexMax,VertexSet,VertexInc);

			Log += LogBegin + "  -- текущее покрытие: ("
				   + IntToStr((int)Cover.size()) + ") " + ToString(Cover) + "\n";

			++Step;
		}

		Log += "\n  -- максимальное независимое множество: ("
			   + IntToStr(static_cast<int>(N - Cover.size())) + ") "
			   + ToString(CoverToIndep(Cover)) + "\n";

		QueryPerformanceCounter(&TimeEnd);

		ToConsol("Максимальное независимое множество найдено! Алгоритм завершил работу.");

		ToCover();

	} catch (...){
		ToConsol("Неизвестная ошибка! Максимальное независимое множество не найдено.");
	}
}
//------------------------------------------------------------------------------


void __fastcall TThreadSearchCover::FullSearchCover()
{
	try {

		ToConsol(MESSAGE_CLEAR);
		ToConsol("search-cover full " + FileName);

		if (Vertex.size() == 0) {
			ToConsol("Ошибка! Не задан граф. Минимальное покрытие не найдено.");
			return;
		}

		// задаем начальные данные характеристикам алгоритма
		Q        = 0;
		Cover    = v_t();
		LogShort = "МЕТОД БЫСТРОГО ПОЛНОГО ПЕРЕБОРА\n\n";
		Log      = "Пошаговый отчет работы алгоритма: \n\n";
		QueryPerformanceCounter(&TimeBegin);

		// промежуточные данные
		ToConsol("Формируются промежуточные данные...");
		s_t VertexSet;    // множество номеров всех вершин графа
		for (int i = 1; i <= N; ++i)
			VertexSet.insert(i);

		v_t MaxIndep;    // максимальное назависимое множество

		// начинаем поиск максимального независимого множество
		// поочередно включая каждую вершину в нез.множество
		// среди N множеств выбираем мкасимальное

		ToConsol("Поиск максимального независимого множества...");
		for (int i = 1; i <= N; ++i) {

			if (Terminated) {
				ToConsol("Процесс остановлен! Минимальное покрытие не найдено.");
				return;
			}

			AnsiString StrI = IntToStr(i);
			Log += "\n  -- строим подграф G" + StrI + " :\n\n";

			ToConsol("шаг ." + StrI
					 + " - поиск независимого множества для подграфа G" + StrI);

			// поиск максимального независимого множества для i-го подграфа G
			v_t Indep = FullSearchIndep("",i,VertexSet,Vertex,v_t());

			Log += "\n  -- независимое множество подграфа G"
				   + StrI + " (" + IntToStr((int)Indep.size()) + ") "
				   + ToString(Indep) + "\n";

			++Q;

			if (MaxIndep.size() < Indep.size())
				MaxIndep = Indep;

		}    // for i <= N

		Log += "\n  -- максимальное независимое множество: ("
			   + IntToStr((int)MaxIndep.size()) + ") "
			   + ToString(MaxIndep) + "\n";

		ToConsol("Максимальное независимое множество найденно, находим вершинное покрытие...");

		// из независимого множесва получаем вершинное покрытие
		Cover = CoverFromIndep(VertexSet,MaxIndep);

		Log += "\n  -- максимальное независимое множество: ("
			   + IntToStr(static_cast<int>(N - Cover.size())) + ") "
			   + ToString(CoverToIndep(Cover)) + "\n";

		QueryPerformanceCounter(&TimeEnd);

		ToConsol("Максимальное независимое множество найдено! Алгоритм завершил работу.");

		ToCover();

	} catch (...){
		ToConsol("Неизвестная ошибка! Максимальное независимое множество не найдено.");
	}
}
//---------------------------------------------------------------------------


v_t __fastcall TThreadSearchCover::FullSearchIndep(
									   AnsiString LogRowBegin,
									   int        I,
									   s_t        VertexSet,
									   vs_t       Vertex,
									   v_t        Indep
									)
{
	LogRowBegin += "." + IntToStr(I);

	Log += LogRowBegin + "  -- включаем в независимое множество вершину: "
		   + IntToStr(I) + "\n";

	ToConsol("шаг " + LogRowBegin
			 + " - включаем вершину в независимое множество");

	VertexToIndep(I,VertexSet,Vertex,Indep);

	Log += LogRowBegin + "  -- текущий подграф: \n"
		   + ToString(LogRowBegin + "        ",VertexSet,Vertex);

	ToConsol("шаг " + LogRowBegin
			 + " - поиск экстримальных вершин");

	SearchExtremIndep(LogRowBegin,VertexSet,Vertex,Indep);

	v_t MaxIndep(Indep);

	Log += LogRowBegin + "  -- текущее независимое множество:  ("
		   + IntToStr((int)Indep.size()) + ")  "
		   + ToString(Indep) + "\n";

	if (VertexSet.size() > 0)
		 Log += LogRowBegin + "  -- оставшиеся вершины: "
		        + ToString(VertexSet) + " - для каждой находим нез.множество\n";

	for (s_t::iterator it = VertexSet.begin();
		 it != VertexSet.end(); ++it)
	{
		if (Terminated)
			return v_t();

		v_t _Indep = FullSearchIndep(LogRowBegin,*it,VertexSet,Vertex,Indep);

		++Q;

		if (_Indep.size() > MaxIndep.size())
			MaxIndep = _Indep;
	}

    return MaxIndep;
}
//---------------------------------------------------------------------------


void __fastcall TThreadSearchCover::IndsSearchCover()
{
	try {

		ToConsol("search-cover full " + FileName);

		if (Vertex.size() == 0) {
			ToConsol("Ошибка! Не задан граф. Минимальное покрытие не найдено.");
			return;
		}

		Q        = 0;
		Cover    = v_t();
		LogShort = "МЕТОД НЕЗАВИСИМЫХ МНОЖЕСТВ\n\n";
		Log      = "Пошаговый отчет работы алгоритма: \n\n";
		QueryPerformanceCounter(&TimeBegin);

		ToConsol("Формируем пары - покрытие / независимое множество...");

		Log += ".1 Формируем пары - покрытие / независимое множество\n\n"
			   "  -- сформированные множеста:\n\n";

		// множество всех пар множеств - покрытие/нез.множество
		// в паре множеств first - вершинные покрытия, second - независимое множесвтов
		set<pair<s_t,s_t> > Sets;

		// множество всех пар множеств - покрытие/нез.множество,
		// которые вместе являются множеством всех вершин графа
		set<pair<s_t,s_t> > FullSets;

		AnsiString FullStr = "";

		unsigned n = Vertex.size() - 1;
		for (unsigned i = 1; i <= n; ++i)
			for (s_t::iterator it = VertexAdd.at(i).begin();
				 it != VertexAdd.at(i).end(); ++it)
			{
				++Q;
				if (*it > i) {

					pair<s_t,s_t> S;

					S.second.insert(i);
					S.second.insert(*it);

					set_union(Vertex.at(i).begin(),Vertex.at(i).end(),
							  Vertex.at(*it).begin(),Vertex.at(*it).end(),
							  inserter(S.first, S.first.begin()));

					AnsiString Str =  ToString(S);

					Q += max(Vertex.at(i).size(),Vertex.at(*it).size());

					if ((S.first.size() + S.second.size()) == n) {
						FullSets.insert(S);
						FullStr += "  -- ." + IntToStr((int)FullSets.size()) + Str;
					} else {
						Sets.insert(S);
						Log += "  -- ." + IntToStr((int)Sets.size()) + Str;
                    }
				}
		   }

		Log += "\n  -- найденные полные множества:  - "  +
				IntToStr((int)FullSets.size()) + "\n" + FullStr;

		unsigned SetsSize = Sets.size();
		unsigned SetsSizePrev = 0;

		unsigned FullSetsSize = FullSets.size();
		unsigned FullSetsSizePrev = 0;


		bool Absorb = false;
		ToConsol("Созданно " + IntToStr((int)SetsSize) + " независимых множеств");
		ToConsol("Формируются непересекающиеся множества... ");
		Log += "\n.2 Формируем множества у которых не пересекаются покрытие и независмое множество\n\n";

		// формируем множества пока появляются новые множества
		// и появляются новые полные множества
		while (
			   (SetsSize > 0) &&
			   (SetsSize != SetsSizePrev) &&
			   (FullSetsSize != FullSetsSizePrev || FullSetsSize == 0)
			  )
		{
			//запоминаем старое количество множеств
			SetsSizePrev = SetsSize;

			//запоминаем старое количество полных множеств
			FullSetsSizePrev = FullSetsSize;

			//set<pair<s_t,s_t> > NewSets(Sets);

			unsigned CompSets = 1;    //номер множества для отображения ходя процесса
			AnsiString SetsSizeStr = " / " + IntToStr((int)SetsSize);

			// обееденяем пары у которых одинаковы вершинные покрытия - first

			IndsUnionSets(n,&Sets,&FullSets);
			set<pair<s_t,s_t> > NewSets;

			for (set<pair<s_t,s_t> >::iterator it = Sets.begin();
				 it != --Sets.end(); ++it)
			{
				ToConsol("сравнение множеств: " + IntToStr((int)CompSets) + SetsSizeStr);

				set<pair<s_t,s_t> >::iterator it_next = it;
				++it_next;

				for (;it_next != Sets.end(); ++it_next) {

					if (Terminated) {
						ToConsol("Процесс остановлен! Минимальное покрытие не найдено.");
						return;
					}

					s_t intersect;

					set_intersection(it->first.begin(), it->first.end(),
							it_next->second.begin(), it_next->second.end(),
							inserter(intersect, intersect.begin()));

					Q += max(it->first.size(),it_next->second.size());

					if (intersect.size() == 0) {

						set_intersection(it->second.begin(), it->second.end(),
								it_next->first.begin(), it_next->first.end(),
								inserter(intersect, intersect.begin()));

						Q += max(it->second.size(),it_next->first.size());

						if (intersect.size() == 0) {

							pair<s_t,s_t> S;

							set_union(it->first.begin(), it->first.end(),
									it_next->first.begin(), it_next->first.end(),
									inserter(S.first, S.first.begin()));

							set_union(it->second.begin(), it->second.end(),
									it_next->second.begin(), it_next->second.end(),
									inserter(S.second, S.second.begin()));

							if ((S.first.size() + S.second.size()) == n)
								FullSets.insert(S);
							else
								NewSets.insert(S);
						}
					}
				}    // end for it_next

				++CompSets;
			}    // end for it

			Sets = NewSets;
			SetsSize = Sets.size();
			FullSetsSize = FullSets.size();

			ToConsol("сформированно " + IntToStr((int)SetsSize) + " множеств");
			ToConsol("найдено " + IntToStr((int)(FullSetsSizePrev - FullSetsSize))
					  + " полных множеств");

			Log += "  -- сформированно " + IntToStr((int)SetsSize) + " множеств\n"
				   + "  -- найдено " + IntToStr((int)(FullSetsSizePrev - FullSetsSize))
				   + " полных множеств\n\n";

		}    // end while

		// сортируем покрытия в порядке возростания
		map<unsigned,vector<set<pair<s_t,s_t> >::iterator> > MapIterator;

		for (set<pair<s_t,s_t> >::iterator it = FullSets.begin(); it != FullSets.end(); ++it)
			MapIterator[it->first.size()].push_back(it);

		AnsiString Str = "Все вершинные покрытия и независимые множества графа:\n\n";
		Str += "(длина) покрытие - (длина) независимое множество\n";
		unsigned k = 1;
		for (unsigned i = 1; i <= n; ++i)
			if (MapIterator[i].size() > 0)
				for (unsigned j = 0; j < MapIterator[i].size(); ++j) {
					Str += "  -- " + IntToStr((int)k) + ".\t"
						   + ToString(MapIterator[i][j]->first)
						   + " (" + IntToStr((int)MapIterator[i][j]->first.size())
						   + ")\t\t-\t\t" + ToString(MapIterator[i][j]->second)
						   + " (" + IntToStr((int)MapIterator[i][j]->second.size())
						   + ")\n";

					if (k == 1) {
						s_t MinCover = MapIterator[i].at(j)->first;
						for (s_t::iterator it = MinCover.begin();
							 it !=  MinCover.end(); ++it)
							Cover.push_back(*it);
					}
					++k;
				}

		Log = Str + "\n" + Log;

		Log += ".3 Максимальное независимое множество: \n\n  -- ("
			   + IntToStr(static_cast<int>(N - Cover.size())) +  ") "
			   + ToString(CoverToIndep(Cover)) + "\n"
			   + "  -- в графе всего " + IntToStr((int)FullSets.size())
			   + " вершинных покрытия и независимых множеств.\n";

		QueryPerformanceCounter(&TimeEnd);

		ToConsol("Максимальное независимое множество найдено! Алгоритм завершил работу.");

		ToCover();

	} catch (...){
		ToConsol("Неизвестная ошибка! Максимальное независимое множество не найдено.");
	}
}
//---------------------------------------------------------------------------


void __fastcall TThreadSearchCover::NindSearchCover()
{
	try {

		ToConsol("search-cover nind " + FileName);

		if (Vertex.size() == 0) {
			ToConsol("Ошибка! Не задан граф. Минимальное покрытие не найдено.");
			return;
		}

		Q        = 0;
		Cover    = v_t();
		LogShort = "НОВЫЙ МЕТОД НЕЗАВИСИМЫХ МНОЖЕСТВ\n\n";
		Log      = "Пошаговый отчет работы алгоритма: \n\n";
		QueryPerformanceCounter(&TimeBegin);

		// множество всех пар множеств - покрытие/нез.множество
		// в паре множеств first - вершинные покрытия, second - независимое множесвтов
		set<pair<s_t,s_t> > Sets;
		set<pair<s_t,s_t> > FullSets;

		ToConsol("\t--\tФормируем пары X - Y, где X - вершинное покрытие, Y - независимое множество...");
		Log += "Шаг.1 Формируем пары X - Y, где X - вершинное покрытие, Y - независимое множество\n\n";

		for (unsigned i = 1; i <= N; ++i)
			for (s_t::iterator it = VertexAdd.at(i).begin();
				 it != VertexAdd.at(i).end(); ++it)
			{
				++Q;
				if (*it > i) {

					pair<s_t,s_t> S;

					S.second.insert(i);
					S.second.insert(*it);

					set_union(Vertex.at(i).begin(),Vertex.at(i).end(),
							  Vertex.at(*it).begin(),Vertex.at(*it).end(),
							  inserter(S.first, S.first.begin()));

					Q += max(Vertex.at(i).size(),Vertex.at(*it).size());

					if ((S.first.size() + S.second.size()) == N)
						FullSets.insert(S);
					else
						Sets.insert(S);
				}
		   }

		Log += "\t--\tсформированные полные множеста: " + IntToStr((int)FullSets.size()) + "\n\n";
		Log += ToString(FullSets) + "\n";
		Log += "\t--\tсформированные множеста: " + IntToStr((int)Sets.size()) + "\n\n";
		Log += ToString(Sets);

		ToConsol("\t--\tСозданно " + IntToStr((int)Sets.size()) + " независимых множеств");

		// Log += "\n\nШаг 2 Объеденяем пары множеств с одинаковым вершинным покрытием.\n\n";

		// IndsUnionSets(N,&Sets,&FullSets);
		// Log += ToString(Sets);

		Log += "\n\nШаг 2 Строим решения для каждой сфомированной пары.\n\n";
		
		// строим решения для каждой сфомированной пары
		int cnt = 1;
		AnsiString msg = "";
		for (set<pair<s_t,s_t> >::iterator it = Sets.begin(); it != Sets.end(); ++it)
		{
			++Q;

			if (Terminated) {
				ToConsol("Выполнение алгоритма успешно остановленна!");
				return;
			}

			Log += "\n-- пара:\t" + ToString(it->second);

			pair<s_t,s_t> FullSetTmp = IndsBuildFullSet(N,*it,Sets);

			if (FullSetTmp.first.size() + FullSetTmp.second.size() == N)
				FullSets.insert(FullSetTmp);

			ToConsol(msg.sprintf("\t%4d / %4d",cnt,Sets.size()));
			++cnt;
		}

		// сортируем покрытия в порядке возростания
		map<unsigned,vector<set<pair<s_t,s_t> >::iterator> > MapIterator;

		for (set<pair<s_t,s_t> >::iterator it = FullSets.begin(); it != FullSets.end(); ++it)
			MapIterator[it->first.size()].push_back(it);

		AnsiString Str = "Все вершинные покрытия и независимые множества графа:\n\n";
		Str += "(длина) покрытие - (длина) независимое множество\n";
		unsigned k = 1;
		for (unsigned i = 1; i <= N; ++i)
			if (MapIterator[i].size() > 0)
				for (unsigned j = 0; j < MapIterator[i].size(); ++j) {
					Str += "  -- " + IntToStr((int)k) + ".\t"
						   + ToString(MapIterator[i][j]->first)
						   + " (" + IntToStr((int)MapIterator[i][j]->first.size())
						   + ")\t\t-\t\t" + ToString(MapIterator[i][j]->second)
						   + " (" + IntToStr((int)MapIterator[i][j]->second.size())
						   + ")\n";

					if (k == 1) {
						s_t MinCover = MapIterator[i].at(j)->first;
						for (s_t::iterator it = MinCover.begin();
							 it !=  MinCover.end(); ++it)
							Cover.push_back(*it);
					}
					++k;
				}

		Log = Str + "\n" + Log;

		Log += "\n\nШаг 3 Результаты работы алгоритма: \n\n  -- максимальное независимое множество - "
			   + ToString(CoverToIndep(Cover))
			   + " ("
			   + IntToStr(static_cast<int>(N - Cover.size())) +  ")\n"
			   + "  -- количество вершинных покрытий и независимых множест в графе - "
			   + IntToStr((int)FullSets.size())
			   + "\n";

		QueryPerformanceCounter(&TimeEnd);

		ToConsol("Максимальное независимое множество найдено! Алгоритм завершил работу.");

		ToCover();

	} catch (...){
		ToConsol("Неизвестная ошибка! Максимальное независимое множество не найдено.");
	}
}
//---------------------------------------------------------------------------


void __fastcall TThreadSearchCover::NinuSearchCover()
{
	try {

		ToConsol("search-cover ninu " + FileName);

		if (Vertex.size() == 0) {
			ToConsol("Ошибка! Не задан граф. Минимальное покрытие не найдено.");
			return;
		}

		Q        = 0;
		Cover    = v_t();
		LogShort = "НОВЫЙ МЕТОД НЕЗАВИСИМЫХ МНОЖЕСТВ УПРОЩЕННЫЙ\n\n";
		Log      = "Пошаговый отчет работы алгоритма: \n\n";
		QueryPerformanceCounter(&TimeBegin);

		// множество всех пар множеств - покрытие/нез.множество
		// в паре множеств first - вершинные покрытия, second - независимое множесвтов
		set<pair<s_t,s_t> > Sets;
		set<pair<s_t,s_t> > FullSets;

		ToConsol("\t--\tФормируем пары X - Y, где X - вершинное покрытие, Y - независимое множество...");
		Log += "Шаг.1 Формируем пары X - Y, где X - вершинное покрытие, Y - независимое множество\n\n";

		for (unsigned i = 1; i <= N; ++i)
			for (s_t::iterator it = VertexAdd.at(i).begin();
				 it != VertexAdd.at(i).end(); ++it)
			{
				++Q;
				if (*it > i) {

					pair<s_t,s_t> S;

					S.second.insert(i);
					S.second.insert(*it);

					set_union(Vertex.at(i).begin(),Vertex.at(i).end(),
							  Vertex.at(*it).begin(),Vertex.at(*it).end(),
							  inserter(S.first, S.first.begin()));

					Q += max(Vertex.at(i).size(),Vertex.at(*it).size());

					if ((S.first.size() + S.second.size()) == N)
						FullSets.insert(S);
					else
						Sets.insert(S);
				}
		   }

		Log += "\t--\tсформированные полные множеста: " + IntToStr((int)FullSets.size()) + "\n\n";
		Log += ToString(FullSets) + "\n";
		Log += "\t--\tсформированные множеста: " + IntToStr((int)Sets.size()) + "\n\n";
		Log += ToString(Sets);

		ToConsol("\t--\tСозданно " + IntToStr((int)Sets.size()) + " независимых множеств");

		Log += "\n\nШаг 1.1 Объеденяем пары множеств с одинаковым вершинным покрытием.\n\n";

		IndsUnionSetsAbsorb(N,&Sets,&FullSets);
		Log += ToString(Sets);

		Log += "\n\nШаг 2 Строим решения для каждой сфомированной пары.\n\n";

		// строим решения для каждой сфомированной пары
		int cnt = 1;
		AnsiString msg = "";
		for (set<pair<s_t,s_t> >::iterator it = Sets.begin(); it != Sets.end(); ++it)
		{
			++Q;
			if (Terminated) {
				ToConsol("Выполнение алгоритма успешно остановленна!");
				return;
			}

			Log += "\n-- пара:\t" + ToString(it->second);

			pair<s_t,s_t> FullSetTmp = IndsBuildFullSet(N,*it,Sets);

			if (FullSetTmp.first.size() + FullSetTmp.second.size() == N)
				FullSets.insert(FullSetTmp);

			ToConsol(msg.sprintf("\t%4d / %4d",cnt,Sets.size()));
			++cnt;
		}

		// сортируем покрытия в порядке возростания
		map<unsigned,vector<sps_t::iterator> > MapIterator;

		for (set<pair<s_t,s_t> >::iterator it = FullSets.begin(); it != FullSets.end(); ++it)
			MapIterator[it->first.size()].push_back(it);

		AnsiString Str = "Все вершинные покрытия и независимые множества графа:\n\n";
		Str += "(длина) покрытие - (длина) независимое множество\n";

		unsigned k = 1;
		for (unsigned i = 1; i <= N; ++i)
			if (MapIterator[i].size() > 0)
				for (unsigned j = 0; j < MapIterator[i].size(); ++j) {
					Str += "  -- " + IntToStr((int)k) + ".\t"
						   + ToString(MapIterator[i][j]->first)
						   + " (" + IntToStr((int)MapIterator[i][j]->first.size())
						   + ")\t\t-\t\t" + ToString(MapIterator[i][j]->second)
						   + " (" + IntToStr((int)MapIterator[i][j]->second.size())
						   + ")\n";

					if (k == 1) {
						s_t MinCover = MapIterator[i].at(j)->first;
						for (s_t::iterator it = MinCover.begin();
							 it !=  MinCover.end(); ++it)
							Cover.push_back(*it);
					}
					++k;
				}

		Log = Str + "\n" + Log;

		Log += "\n\nШаг 3 Результаты работы алгоритма: \n\n  -- максимальное независимое множество - "
			   + ToString(CoverToIndep(Cover))
			   + " ("
			   + IntToStr(static_cast<int>(N - Cover.size())) +  ")\n"
			   + "  -- количество вершинных покрытий и независимых множест в графе - "
			   + IntToStr((int)FullSets.size())
			   + "\n";

		QueryPerformanceCounter(&TimeEnd);

		ToConsol("Максимальное независимое множество найдено! Алгоритм завершил работу.");

		ToCover();

	} catch (...){
		ToConsol("Неизвестная ошибка! Максимальное независимое множество не найдено.");
	}
}
//---------------------------------------------------------------------------


pair<s_t,s_t> __fastcall TThreadSearchCover::IndsBuildFullSet(int n,pair<s_t,s_t> FullSet,set<pair<s_t,s_t> > Sets)
{
	Sets.erase(FullSet);

	while (FullSet.first.size() + FullSet.second.size() < n && Sets.size() > 0) {
		
		++Q;

		if (Terminated)
			return FullSet;

		Log += "\n\t--\tудаляем вершины:";

		IndsRemoveUsedVertex(FullSet,&Sets);
		//IndsUnionSets(&Sets);

		Log += "\n\t--\tоставшиеся пары:\n" + ToString(Sets);

		++Q;
		if (Sets.size() > 0) {

			// search maximum set
			set<pair<s_t,s_t> >::iterator it_max = Sets.begin();
			for (set<pair<s_t,s_t> >::iterator it = Sets.begin(); it != Sets.end(); ++it){
				++Q;
				if (it_max->second.size() < it->second.size()) {
					it_max = it;
				} else if (it_max->second.size() == it->second.size()) {
					if (it_max->first.size() > it->first.size())
						it_max = it;
				}
			}

			Log += "\n\t--\tмаксимальное множество: " + ToString(it_max->second);

			//Q += max(FullSet.first.size(),it_max->first.size());
			//Q += max(FullSet.second.size(),it_max->second.size());

			set_union(FullSet.first.begin(),FullSet.first.end(),
					it_max->first.begin(),it_max->first.end(),
					inserter(FullSet.first,FullSet.first.begin()));

			set_union(FullSet.second.begin(),FullSet.second.end(),
					  it_max->second.begin(),it_max->second.end(),
					  inserter(FullSet.second,FullSet.second.begin()));

			Log += "\n\t--\tобъединенное с предыдущим: " + ToString(FullSet);
		}

	}

	Log += "\n\t--\tполное максимальное независимое множество найдено: ";
	Log += ToString(FullSet.second);

	return FullSet;
}
//---------------------------------------------------------------------------


pair<s_t,s_t> __fastcall TThreadSearchCover::IndsBuildFullSetAbsorb(int n,pair<s_t,s_t> FullSet,set<pair<s_t,s_t> > Sets)
{
	Sets.erase(FullSet);

	while (FullSet.first.size() + FullSet.second.size() < n && Sets.size() > 0) {

		++Q;	// подсчет улсловия цикла
		
		if (Terminated)
			return FullSet;

		Log += "\n\t--\tудаляем вершины:";

		IndsRemoveUsedVertex(FullSet,&Sets);
		IndsUnionSetsAbsorb(&Sets);

		Log += "\n\t--\tоставшиеся пары:\n" + ToString(Sets);

		++Q;
		if (Sets.size() > 0) {

			// search maximum set
			set<pair<s_t,s_t> >::iterator it_max = Sets.begin();
			for (set<pair<s_t,s_t> >::iterator it = Sets.begin(); it != Sets.end(); ++it){
				++Q;
				if (it_max->second.size() < it->second.size()) {
					it_max = it;
				} else if (it_max->second.size() == it->second.size()) {
					if (it_max->first.size() > it->first.size())
						it_max = it;
				}
			}

			Log += "\n\t--\tмаксимальное множество: " + ToString(it_max->second);

			//Q += max(FullSet.first.size(),it_max->first.size());
			//Q += max(FullSet.second.size(),it_max->second.size());

			set_union(FullSet.first.begin(),FullSet.first.end(),
					it_max->first.begin(),it_max->first.end(),
					inserter(FullSet.first,FullSet.first.begin()));

			set_union(FullSet.second.begin(),FullSet.second.end(),
					  it_max->second.begin(),it_max->second.end(),
					  inserter(FullSet.second,FullSet.second.begin()));

			Log += "\n\t--\tобъединенное с предыдущим: " + ToString(FullSet);
		}

	}

	Log += "\n\t--\tполное максимальное независимое множество найдено: ";
	Log += ToString(FullSet.second);

	return FullSet;
}
//---------------------------------------------------------------------------


void __fastcall TThreadSearchCover::IndsUnionSets(int n,set<pair<s_t,s_t> > *pSets,set<pair<s_t,s_t> > *pFullSets)
{
		bool Absorb = false;
		set<pair<s_t,s_t> > NewSets(*pSets);

		// объеденяем пары у которых одинаковы вершинные покрытия - first

		for (sps_t::iterator it = pSets->begin();it != pSets->end(); ++it)
		{

			pair<s_t,s_t> S;
			S.first = it->first;

			unsigned CountIdent = 0;
			sps_t::iterator it_next = it;
			++it_next;

			for (; it_next != pSets->end(); ++it_next) {

				if (Terminated)
					return;

				//Q += max(it->first.size(),it_next->first.size());
				++Q;
				if (it->first == it_next->first) {

					Absorb = true;

					if (CountIdent == 0)
						NewSets.erase(*it);

					NewSets.erase(*it_next);

					set_union(it->second.begin(), it->second.end(),
							it_next->second.begin(), it_next->second.end(),
							inserter(S.second, S.second.begin()));

					if (S.second.size() + S.first.size() == n) {
						pFullSets->insert(S);

						Absorb = false;
					}

					//Q += max(it->second.size(),it_next->second.size());

					++CountIdent;

				} else {
					// так как множества упорядочены в порядке возростания
					// то после первого несовпадения нужно переходить к следующему
					break;
				}
			}

			if (CountIdent > 0 && Absorb)
				NewSets.insert(S);

			for (unsigned i = 0; i < CountIdent; ++i)
				++it;
		}

		pSets->swap(NewSets);
}
//---------------------------------------------------------------------------


void __fastcall TThreadSearchCover::IndsUnionSets(set<pair<s_t,s_t> > *pSets)
{
		bool Absorb = false;
		set<pair<s_t,s_t> > NewSets(*pSets);

		// объеденяем пары у которых одинаковы вершинные покрытия - first

		for (set<pair<s_t,s_t> >::iterator it = pSets->begin();
			it != pSets->end(); ++it)
		{

			pair<s_t,s_t> S;
			S.first = it->first;

			unsigned CountIdent = 0;
			set<pair<s_t,s_t> >::iterator it_next = it;
			++it_next;

			for (; it_next != pSets->end(); ++it_next) {

				if (Terminated)
					return;

				//Q += max(it->first.size(),it_next->first.size());
				++Q;
				if (it->first == it_next->first) {

					Absorb = true;

					if (CountIdent == 0)
						NewSets.erase(*it);

					NewSets.erase(*it_next);

					set_union(it->second.begin(), it->second.end(),
							it_next->second.begin(), it_next->second.end(),
							inserter(S.second, S.second.begin()));

					//Q += max(it->second.size(),it_next->second.size());

					++CountIdent;

				} else {
					// так как множества упорядочены в порядке возростания
					// то после первого несовпадения нужно переходить к следующему
					break;
				}
			}

			if (CountIdent > 0 && Absorb)
				NewSets.insert(S);

			for (unsigned i = 0; i < CountIdent; ++i)
				++it;
		}

		pSets->swap(NewSets);
}
//---------------------------------------------------------------------------


void __fastcall TThreadSearchCover::IndsUnionSetsAbsorb(int n,sps_t *pSets,sps_t *pFullSets)
{
	// объеденяем пары у которых вершинные покрытия являются подмножеством другой пары

	v_t absorb(pSets->size(), 0);
	vv_t absorb_sets(pSets->size(), v_t());

	int i = 0;
	for (sps_t::iterator it = pSets->begin(); it != prev_it(pSets->end()); ++it) {

		if (absorb[i] == 1) {
			++i;
			continue;
		}

		int j = i + 1;

		for (sps_t::iterator jt = next_it(it); jt != pSets->end(); ++jt) {

			if (Terminated)
				return;

			if (absorb[j] == 1) {
				++j;
				continue;
			}

			s_t union_res;
			set_union(it->first.begin(), it->first.end(),
				jt->first.begin(), jt->first.end(),
				inserter(union_res,union_res.begin()));

			//Q += max(it->first.size(),jt->first.size());
			++Q;
			if (it->first.size() == union_res.size()) {
				absorb[j] = 1;
				absorb_sets[i].push_back(j);
			}
			else if (jt->first.size() == union_res.size()) {
				absorb[i] = 1;
				copy(absorb_sets.at(i).begin(), absorb_sets.at(i).end(), back_inserter(absorb_sets[j]));
				absorb_sets[j].push_back(i);
				break;
			}

			++j;
		}

		++i;
	}

	sps_t sets_new;
	i = 0;
	for (sps_t::iterator it = pSets->begin(); it != pSets->end(); ++it) {
		if (absorb[i] != 1) {
			if (absorb_sets[i].size() > 0) {
				s_t second_new;
				for (int j = 0; j < absorb_sets[i].size(); ++j) {
					sps_t::iterator jt = pSets->begin();
					advance(jt, absorb_sets[i][j]);
					set_union(it->second.begin(), it->second.end(),
						jt->second.begin(), jt->second.end(),
						inserter(second_new, second_new.begin()));
		
					//Q += max(it->second.size(),jt->second.size());
				}
				
				if (it->first.size() + second_new.size() == n)
					pFullSets->insert(make_pair(it->first, second_new));
				else
					sets_new.insert(make_pair(it->first, second_new));
			}
			else {
				sets_new.insert(*it);
			}
		}

		++i;
	}

	pSets->swap(sets_new);
}
//---------------------------------------------------------------------------


void __fastcall TThreadSearchCover::IndsUnionSetsAbsorb(sps_t *pSets)
{
	// объеденяем пары у которых вершинные покрытия являются подмножеством другой пары

	v_t absorb(pSets->size(), 0);
	vv_t absorb_sets(pSets->size(), v_t());

	int i = 0;
	for (sps_t::iterator it = pSets->begin(); it != prev_it(pSets->end()); ++it) {

		if (absorb[i] == 1) {
			++i;
			continue;
		}

		int j = i + 1;

		for (sps_t::iterator jt = next_it(it); jt != pSets->end(); ++jt) {

			if (Terminated)
				return;

			if (absorb[j] == 1) {
				++j;
				continue;
			}

			s_t union_res;
			set_union(it->first.begin(), it->first.end(),
				jt->first.begin(), jt->first.end(),
				inserter(union_res,union_res.begin()));

			//Q += max(it->first.size(),jt->first.size());
			++Q;
			if (it->first.size() == union_res.size()) {
				absorb[j] = 1;
				absorb_sets[i].push_back(j);
			}
			else if (jt->first.size() == union_res.size()) {
				absorb[i] = 1;
				copy(absorb_sets.at(i).begin(), absorb_sets.at(i).end(), back_inserter(absorb_sets[j]));
				absorb_sets[j].push_back(i);
				break;
			}

			++j;
		}

		++i;
	}

	sps_t sets_new;
	i = 0;
	for (sps_t::iterator it = pSets->begin(); it != pSets->end(); ++it) {
		if (absorb[i] != 1) {
			if (absorb_sets[i].size() > 0) {
				s_t second_new;
				for (int j = 0; j < absorb_sets[i].size(); ++j) {
					sps_t::iterator jt = pSets->begin();
					advance(jt, absorb_sets[i][j]);
					set_union(it->second.begin(), it->second.end(),
						jt->second.begin(), jt->second.end(),
						inserter(second_new, second_new.begin()));

					//Q += max(it->second.size(),jt->second.size());
				}
				
				sets_new.insert(make_pair(it->first, second_new));
			}
			else {
				sets_new.insert(*it);
			}
		}

		++i;
	}

	pSets->swap(sets_new);
}
//---------------------------------------------------------------------------


void __fastcall TThreadSearchCover::IndsRemoveUsedVertex(const pair<s_t,s_t> &UsedSet,set<pair<s_t,s_t> > *pSets)
{
	set<pair<s_t,s_t> > NewSets;

	for (set<pair<s_t,s_t> >::iterator itSets = pSets->begin();
		 itSets != --pSets->end(); ++itSets)
	{
		pair<s_t,s_t> NewPair(*itSets);

		if (Terminated) {
			ToConsol("Процесс остановлен! Минимальное покрытие не найдено.");
			return;
		}

		for (s_t::const_iterator it = UsedSet.first.begin(); it != UsedSet.first.end(); ++it) {
			NewPair.first.erase(*it);
			NewPair.second.erase(*it);
		}

		for (s_t::const_iterator it = UsedSet.second.begin(); it != UsedSet.second.end(); ++it) {
			NewPair.first.erase(*it);
			NewPair.second.erase(*it);
		}

		if (NewPair.first.size() != 0 || NewPair.second.size() != 0)
			NewSets.insert(NewPair);

	}

	pSets->swap(NewSets);
}
//---------------------------------------------------------------------------


void __fastcall TThreadSearchCover::RangSearchCover()
{
	try {

		ToConsol(MESSAGE_CLEAR);
		ToConsol("search-cover rang " + FileName);

		if (Edges.size() == 0) {
			ToConsol("Ошибка! Не задан граф. Минимальное покрытие не найдено.");
			return;
		}

		// задаем начальные данные характеристикам алгоритма
		Q        = 0;
		Cover    = v_t();
		LogShort = "РАНГОВЫЙ МЕТОД\n\n";
		Log      = "Пошаговый отчет работы алгоритма: \n\n";
		QueryPerformanceCounter(&TimeBegin);

		// формируем промежуточные данные

		ToConsol("Создаются промежуточные данные...");

		Log += "1. Формируем списко столбцов и строк: \n\n";

		// количество строк в матрице инциденций - количество ребер
		register int RowCount = Edges.size();

		// количество столбцов в матрице инциденций - количество вершин
		register int ColCount = N;

		// инциденции строк, индекс - номер строки,
		// множество - номера стобцов которые покрывают эту строку
		vs_t Rows(RowCount + 1,s_t());
		Rows.reserve(RowCount + 1);

		// инциденции столбцов, индекс - номер столбца,
		// множество - номера строк которые покрывает этот столбец
		vs_t Cols(ColCount + 1,s_t());
		Cols.reserve(ColCount + 1);

		register int i;

		for (i = 0; i < Edges.size(); ++i) {

			Cols[*Edges[i].begin()].insert(i + 1);
			Cols[*(++Edges[i].begin())].insert(i + 1);

			Rows[i + 1] = Edges[i];
		}

		Log += "  -- список строк:\n" + ToString("     ",Cols) + "\n";
		Log += "  -- список столбцов:\n" + ToString("     ",Rows) + "\n";

		// формируем промежуточные переменные

		// сортируем столбцы в порядке убывания степени
		Log += "2. Соритруем столбцы в порядке убывания:\n\n";

		Q = ColCount;

		vp_t ColsSort(ColCount + 1,p_t(0,0));
		for (i = 1; i <= ColCount; ++i) {
			ColsSort[i].first = Cols[i].size();    // степень i-го столбца
			ColsSort[i].second = i;                // i - номер столбца
		}

		Q += (2 * ColCount);

		// функция сортирует по first в порядке возрастания
		stable_sort(ColsSort.begin() + 1,ColsSort.end());

		// функция меняет порядок в векторе, нулевой элемент оставляем на месте
		reverse(ColsSort.begin() + 1,ColsSort.end());

		Log += "  -- новый номер (старый) :  " + ToString(ColsSort) + "\n";

		// вектор путей (сочетаний столбцов)
		vvs_t ColsUnion(ColCount + 1,vs_t(2,s_t()));
		ColsUnion.reserve(ColCount + 1);

		// создаем начальные пути для каждого столбца
		for (i = 1; i <= ColCount; ++i)
		ColsUnion[i][1].insert(ColsSort[i].second);

		// вектор калибровочных векторов
		Log += "3. Формируем калибровочные вектора и пути:\n\n";

		vv_t HeightBegin(ColCount + 1,v_t(RowCount + 1, 0));
		HeightBegin.reserve(ColCount + 1);

		// вектор сочетаний калибровочных векторов - калибровочные вектора путей (сочетания калибровочных векторов столбцов)
		vvv_t HeightUnion(ColCount + 1,vv_t(2,v_t(RowCount + 1,0)));
		HeightUnion.reserve(ColCount + 1);

		// создаем калибровочный вектор последнего столбца
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

		// создаем калибровочные вектора остальных столбцов
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
						HeightBegin[col][0] = INFIN;    // [0] - в нулевом элементе вес калибровочного вектора

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
			}    // for row
		}    // for col

		Log += "  -- B - пути, H - калибровочные вектора, d - длина пути, # - знак бесконечности\n\n"
		       + ToString(Cols.size(),HeightBegin);

		for (i = 1; i <= ColCount; ++i)
			if (HeightBegin[i][0] == 0) {
				QueryPerformanceCounter(&TimeEnd);
				Cover.push_back(ColsSort[i].second);
				if (i != ColCount)
					Cover.push_back(ColsSort[i + 1].second);

				Log += "  -- кратчайший путь найден;\n\nАлгоритм завершил работу,";
				Log += "кратчайший путь является минимальным покрытием.\n";

				ToConsol("Максимальное независимое множество найдено! Алгоритм завершил работу.");

				ToCover();

				return;
			}

		Q += ColCount;

		// минимальная степень калибровочного вектора пути, равная количеству различных значений в векторе
		// если есть хотя бы один такой элемент вектора равен INFIN равна INFIN - это означает что с этого пути не существует столбца
		// который даст покрытие всех строк, и равна 0 если все элементы вектора равны 0, это означает что путь покрыл все строки матрицы
		// путь у которого калибровочный вектор имеет наименьшую степерь, является минимальным покрытием
		Log += "4. Формируем все возможные кратчайшие пути:\n\n";

		ToConsol("Строим все возможные пути и определяем кратчайший из них...\n");

		AnsiString StrRang;
		AnsiString StrCol;

		for (register int rang = 1; rang < ColCount; ++rang) {

			StrRang = "ранг: " + IntToStr(rang) + " / " + IntToStr((int)ColCount - 1);

			vvv_t HeightNewUnion(ColCount + 1,vv_t(1,v_t(RowCount + 1,0)));
			HeightNewUnion.reserve(ColCount + 1);

			vvs_t ColsNewUnion(ColCount + 1,vs_t(1,s_t()));
			ColsNewUnion.reserve(ColCount + 1);

			for (register int col = rang + 1; col <= ColCount; ++col) {

				StrCol = "  столбец: " + IntToStr(col) + " / " + IntToStr((int)ColCount - 1);

				int num_curr_union = 0;

				Log += "  --    ранг: " + IntToStr(rang);
				Log += "    столбец: " + IntToStr(ColsSort[col].second) +"\n\n";

				for (register int col_prev = rang; col_prev < col; ++col_prev) {

					ToConsol(StrRang + StrCol + "  множество: "
							 + IntToStr(col_prev) + " / " + IntToStr(col - 1));

					for (register int union_i = 1; union_i < HeightUnion[col_prev].size(); ++union_i) {

						if (Terminated) {
							ToConsol("Процесс остановлен! Минимальное покрытие не найдено.");
							return;
						}

						// формируем новое обьединение col столбца с обьединением col_prev,union_i
						ColsNewUnion[col].push_back(ColsUnion[col_prev][union_i]);
						num_curr_union = ColsNewUnion[col].size() - 1;
						ColsNewUnion[col][num_curr_union].insert(ColsSort[col].second);

						// формируем новое обьединение калибровочного вектора col столбца с обьединением col_prev,union_i
						HeightNewUnion[col].push_back(v_t(RowCount + 1,0));

						s_t ValuesHeight;    //

						Q += RowCount;

						for (int row = 1; row <= RowCount; ++row) {

							if (HeightUnion[col_prev][union_i][row] != 0) {
								HeightNewUnion[col][num_curr_union][row] = HeightBegin[col][row];
							}

							ValuesHeight.insert(HeightNewUnion[col][num_curr_union][row]);
						}

						++Q;

						if (*ValuesHeight.rbegin() == INFIN) {

							Log += "  --    " + ToString(ColsNewUnion[col][num_curr_union]);
							Log += "        d = #";
							Log += "        H -  ";
							Log += ToString("#",HeightNewUnion[col][num_curr_union]) + "\n";

							HeightNewUnion[col].pop_back();
							ColsNewUnion[col].pop_back();

						} else {

							++Q;

							if (*ValuesHeight.begin() == 0 && ValuesHeight.size() == 1) {

								QueryPerformanceCounter(&TimeEnd);

								for (s_t::iterator it_col = ColsNewUnion[col][num_curr_union].begin();
									 it_col != ColsNewUnion[col][num_curr_union].end(); ++it_col)
									Cover.push_back(*it_col);

								Log += "  --    ";
								Log += ToString(ColsNewUnion[col][num_curr_union]);
								Log += "        d = 0        H -  ";
								Log += ToString("#",HeightNewUnion[col][num_curr_union]) + "\n\n";

								Log += "  -- кратчайший путь найден;\n\nАлгоритм завершил работу, ";
								Log += "кратчайший путь является минимальным покрытием.\n";

								ToConsol("Минимальное покрытие найдено! Алгоритм завершил работу.");

								ToCover();

								return;

							} else {

								Log += "  --    ";
								Log += ToString(ColsNewUnion[col][num_curr_union]);
								Log += "        d = ";
								Log += IntToStr((int)ValuesHeight.size() - 1);
								Log += "        H -  ";
								Log += ToString("#",HeightNewUnion[col][num_curr_union]) + "\n";

								HeightNewUnion[col][num_curr_union][0] = ValuesHeight.size() - 1;
							}
						}
					}     // end for union_i
				}    // end for col_prev

				Log += "\n";

			}    // end for col

			Log += "\n";

			HeightUnion = HeightNewUnion;
			ColsUnion = ColsNewUnion;

		} // end for rang

	} catch (...){
		ToConsol("Неизвестная ошибка! Максимальное независимое множество не найдено.");
	}
}
//---------------------------------------------------------------------------


void __fastcall TThreadSearchCover::VertSearchCover()
{
	try {

		ToConsol("search-cover full " + FileName);

		if (Vertex.size() == 0) {
			ToConsol("Ошибка! Не задан граф. Минимальное покрытие не найдено.");
			return;
		}

		// задаем начальные данные характеристикам алгоритма
		Q        = 0;
		Cover    = v_t();
		LogShort = "МЕТОД ВЕРШИН\n\n";
		Log      = "Пошаговый отчет работы алгоритма: \n\n";
		QueryPerformanceCounter(&TimeBegin);

		// промежуточные данные
		ToConsol("Формируются промежуточные данные...");
		s_t VertexSet;    // множество номеров всех вершин графа
		for (int i = 1; i <= N; ++i)
			VertexSet.insert(i);

		v_t Indep;    // максимальное назависимое множество

		// начинаем поиск максимального независимого множество
		// поочередно включая каждую вершину в нез.множество
		// среди N множеств выбираем мкасимальное

		ToConsol("Поиск максимального независимого множества...");
		for (int i = 1; i <= N; ++i) {

			if (Terminated) {
				ToConsol("Процесс остановлен! Минимальное покрытие не найдено.");
				return;
			}

			AnsiString StrI = IntToStr(i);
			Log += "\n." + StrI + "  -- строим подграф по вершине:"
				   + StrI + "\n";

			ToConsol("шаг ." + StrI
					 + " поиск независимого множества для подграфа G" + StrI);

			s_t  _VertexSet(VertexSet);
			vs_t _Vertex(Vertex);
			v_t  _Indep;

			// добавляем вершину в независимое множество
			VertexToIndep(i,_VertexSet,_Vertex,_Indep);

			int Step = 1;
			AnsiString LogRowBegin = "";

			while (_VertexSet.size() > 0) {

				LogRowBegin = "." + StrI + "." + IntToStr(Step);
				Log += LogRowBegin + "  -- текущий подграф: \n"
					   + ToString(LogRowBegin + "        ",_VertexSet,_Vertex);

				ToConsol("шаг " + LogRowBegin
					 + " поиск изолированных вершин, висячих и связанных со всеми");

				// ищем изолированные и висячие вершины
				SearchExtremIndep(LogRowBegin,_VertexSet,_Vertex,_Indep);

				if (_VertexSet.size() == 0)
					break;

				ToConsol("шаг " + LogRowBegin
						 + " поиск вершины дающей наибольшее число висячих вершин");

				// находим вершины с наибольшей степенью

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

				Log += LogRowBegin
					   + "  -- вершины с наибольшей степенью: "
					   + ToString(MaxSet) + "\n";

				// из вершин с наибольшей степенью берем ту которая
				// даст наибольшее число 'крайних'

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

				Log += LogRowBegin
					   + "  -- вершина дающая наибольшее число 'висячих' вершин: "
					   + IntToStr(MaxNum) + " - удаляем\n";

				// удаляем найденную вершину из графа
				VertexErase(MaxNum,_Vertex);
				_VertexSet.erase(MaxNum);

				Log += LogRowBegin + "  -- текущее независимое множество: ("
					   + IntToStr((int)_Indep.size()) + ") " + ToString(_Indep) + "\n";

				++Step;
			}    // while _VertexSet.size > 0

			Log += "\n" + StrI + "  -- найденное независимое множество: ("
				   + IntToStr((int)_Indep.size()) + ") "
				   + ToString(_Indep) + "\n";

			if (Indep.size() < _Indep.size())
				Indep = _Indep;

		}    // for i <= N

		Log += "\n  -- максимальное независимое множество: ("
			   + IntToStr((int)Indep.size()) + ") "
			   + ToString(Indep) + "\n";

		ToConsol("Максимальное независимое множество найденно, находим вершинное покрытие...");

		// из независимого множесва получаем вершинное покрытие
		Cover = CoverFromIndep(VertexSet,Indep);

		Log += "\n  -- максимальное независимое множество: ("
			   + IntToStr(static_cast<int>(N - Cover.size())) + ") "
			   + ToString(CoverToIndep(Cover)) + "\n";

		QueryPerformanceCounter(&TimeEnd);

		ToConsol("Максимальное независимое множество найдено! Алгоритм завершил работу.");

		ToCover();

	} catch (...){
		ToConsol("Неизвестная ошибка! Максимальное независимое множество не найдено.");
	}
}
//---------------------------------------------------------------------------


void __fastcall TThreadSearchCover::VpreSearchCover()
{
	try {

		ToConsol("search-cover full " + FileName);

		if (Vertex.size() == 0) {
			ToConsol("Ошибка! Не задан граф. Минимальное покрытие не найдено.");
			return;
		}

		// задаем начальные данные характеристикам алгоритма
		Q        = 0;
		Cover    = v_t();
		LogShort = "МЕТОД ВЕРШИН С ПРОГНОЗОМ\n\n";
		Log      = "Пошаговый отчет работы алгоритма: \n\n";
		QueryPerformanceCounter(&TimeBegin);

		// промежуточные данные
		ToConsol("Формируются промежуточные данные...");
		s_t VertexSet;    // множество номеров всех вершин графа
		for (int i = 1; i <= N; ++i)
			VertexSet.insert(i);

		v_t Indep;    // максимальное назависимое множество

		// начинаем поиск максимального независимого множество
		// поочередно включая каждую вершину в нез.множество
		// среди N множеств выбираем мкасимальное

		ToConsol("Поиск максимального независимого множества...");
		for (int i = 1; i <= N; ++i) {

			if (Terminated) {
				ToConsol("Процесс остановлен! Минимальное покрытие не найдено.");
				return;
			}

			AnsiString StrI = IntToStr(i);
			Log += "\n  -- строим подграф G" + StrI + " :\n\n";

			ToConsol("шаг ." + StrI
					 + " - поиск независимого множества для подграфа G" + StrI);

			s_t  _VertexSet(VertexSet);
			vs_t _Vertex(Vertex);
			v_t  _Indep;

			// максимальное независимое множество W1
			// оно может отличатся от _Indep так как мы выбираем вершину дающую
			// не максимальную W1 а максимальную сумму W1 и W2, в дальнейшем это может дать
			// либо большее либо меньшее W1, поэтому запоминаем максимальное MaxW1
			v_t  MaxW1;

			int VertexInclud = i;
			int Step = 1;

			while (_VertexSet.size() > 0) {

				AnsiString LogRowBegin = "." + StrI + "." + IntToStr(Step);

				Log += LogRowBegin + "  -- включаем в независимое множество вершину: "
					   + IntToStr(VertexInclud) + "\n";

				ToConsol("шаг " + LogRowBegin
				         + " - включаем вершину в независимое множество");

				VertexToIndep(VertexInclud,_VertexSet,_Vertex,_Indep);

				Log += LogRowBegin + "  -- текущий подграф: \n"
					   + ToString(LogRowBegin + "        ",_VertexSet,_Vertex);

				ToConsol("шаг " + LogRowBegin + " - поиск экстримальных вершин");

				SearchExtremIndep(LogRowBegin,_VertexSet,_Vertex,_Indep);

				Log += LogRowBegin + "  -- текущее независимое множество:  ("
					   + IntToStr((int)_Indep.size()) + ")  "
					   + ToString(_Indep) + "\n";

				int VertexMaxW1 = 0;    // вершина давшая максимальное нез.множество (MaxW1)
				int MaxWeightW2 = 0;
				int MaxSumW = 0;       // максимальная сумма W1 и W2

				if (_VertexSet.size() > 0) {

					Log += LogRowBegin + "  -- находим вершины с наибольшей суммой W1 и W2\n";

					ToConsol("шаг " + LogRowBegin + " - строятся прогнозы");
				}

				for (s_t::iterator it = _VertexSet.begin();
					 it != _VertexSet.end(); ++it)
				{
					if (Terminated) {
						ToConsol("Процесс остановлен! Минимальное покрытие не найдено.");
						return;
					}

					s_t  VertexW2(_VertexSet);
					vs_t W2(_Vertex);
					v_t  W1(_Indep);

					// включаем вершину в независимое множество
					VertexToIndep(*it,VertexW2,W2,W1);

					// ищем изолированные и "висячие" вершины
					SearchExtremIndep(VertexW2,W2,W1);

					// находим вершину которая дает решение с наибольшим независимым множеством
					int WeightW2 = VpreMinDegree(VertexW2,W2);
					int SumW   = W1.size() + WeightW2;

					Log += LogRowBegin + "  -- проверяем вершину " + IntToStr(*it);
					Log += " : W1 (" + IntToStr((int)W1.size());
					Log += ")   W2 (" + IntToStr(WeightW2);
					Log += ") - " + IntToStr(SumW) + "\n";

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

					// запоминаем максимальное независимое множество
					++Q;
					if (W1.size() > MaxW1.size()) {

						MaxW1 = W1;
						VertexMaxW1 = *it;
					}
				}    // end for it

				if (_VertexSet.size() > 0)
					Log += LogRowBegin + "  -- вершина с наибольшей суммой W1 и W2: "
						   + IntToStr(VertexInclud) + "\n";

				if (VertexMaxW1 > 0) {
					Log += LogRowBegin
						   + "  -- вершина дающая наибольшее W1 : "
						   + IntToStr(VertexMaxW1)+ "\n"
						   + LogRowBegin + "  -- запоминаем наибольшее W1: ("
						   + IntToStr((int)MaxW1.size()) + ") "
						   + ToString(MaxW1) +"\n";
				}

				++Step;

			}    // end while _VertexSet > 0

			if (_Indep.size() < MaxW1.size()) {

				_Indep = MaxW1;

				Log += "." + StrI + "  -- максимальное множество W1: ("
					   + IntToStr((int)MaxW1.size()) + ")  "
					   + ToString(MaxW1) + " - больше найденного оставляем его\n";
			}

			Log += "\n  -- независимое множество подграфа G"
				   + StrI + " (" + IntToStr((int)_Indep.size()) + ") "
				   + ToString(_Indep) + "\n";

			if (Indep.size() < _Indep.size())
				Indep = _Indep;

		}    // for i <= N

		Log += "\n  -- максимальное независимое множество: ("
			   + IntToStr((int)Indep.size()) + ") "
			   + ToString(Indep) + "\n";

		ToConsol("Максимальное независимое множество найденно, находим вершинное покрытие...");

		// из независимого множесва получаем вершинное покрытие
		Cover = CoverFromIndep(VertexSet,Indep);

		Log += "\n  -- максимальное независимое множество: ("
			   + IntToStr(static_cast<int>(N - Cover.size())) + ") "
			   + ToString(CoverToIndep(Cover)) + "\n";

		QueryPerformanceCounter(&TimeEnd);

		ToConsol("Максимальное независимое множество найдено! Алгоритм завершил работу.");

		ToCover();

	} catch (...){
		ToConsol("Неизвестная ошибка! Максимальное независимое множество не найдено.");
	}
}
//---------------------------------------------------------------------------


inline int TThreadSearchCover::VpreMinDegree(const s_t &VertexSet,const vs_t &Vertex)
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


void __fastcall TThreadSearchCover::VrecSearchCover()
{
	try {

		ToConsol("search-cover full " + FileName);

		if (Vertex.size() == 0) {
			ToConsol("Ошибка! Не задан граф. Минимальное покрытие не найдено.");
			return;
		}

		// задаем начальные данные характеристикам алгоритма
		Q        = 0;
		Cover    = v_t();
		LogShort = "МЕТОД ВЕРШИН С РЕКУРСИЕЙ\n\n";
		Log      = "Пошаговый отчет работы алгоритма: \n\n";
		QueryPerformanceCounter(&TimeBegin);

		// промежуточные данные
		ToConsol("Формируются промежуточные данные...");
		s_t VertexSet;    // множество номеров всех вершин графа
		for (int i = 1; i <= N; ++i)
			VertexSet.insert(i);

		v_t MaxIndep;    // максимальное назависимое множество

		// начинаем поиск максимального независимого множество
		// поочередно включая каждую вершину в нез.множество
		// среди N множеств выбираем мкасимальное

		ToConsol("Поиск максимального независимого множества...");
		for (int i = 1; i <= N; ++i) {

			if (Terminated) {
				ToConsol("Процесс остановлен! Минимальное покрытие не найдено.");
				return;
			}

			AnsiString StrI = IntToStr(i);
			Log += "\n  -- строим подграф G" + StrI + " :\n\n";

			ToConsol("шаг ." + StrI
					 + " - поиск независимого множества для подграфа G" + StrI);

			// поиск максимального независимого множества для i-го подграфа G
			v_t Indep = VrecSearchIndep("",i,VertexSet,Vertex,v_t());

			Log += "\n  -- независимое множество подграфа G"
				   + StrI + " (" + IntToStr((int)Indep.size()) + ") "
				   + ToString(Indep) + "\n";

			if (MaxIndep.size() < Indep.size())
				MaxIndep = Indep;

		}    // for i <= N

		Log += "\n  -- максимальное независимое множество: ("
			   + IntToStr((int)MaxIndep.size()) + ") "
			   + ToString(MaxIndep) + "\n";

		ToConsol("Максимальное независимое множество найденно, находим вершинное покрытие...");

		// из независимого множесва получаем вершинное покрытие
		Cover = CoverFromIndep(VertexSet,MaxIndep);

		Log += "\n  -- максимальное независимое множество: ("
			   + IntToStr(static_cast<int>(N - Cover.size())) + ") "
			   + ToString(CoverToIndep(Cover)) + "\n";

		QueryPerformanceCounter(&TimeEnd);

		ToConsol("Максимальное независимое множество найдено! Алгоритм завершил работу.");

		ToCover();

	} catch (...){
		ToConsol("Неизвестная ошибка! Максимальное независимое множество не найдено.");
	}
}
//---------------------------------------------------------------------------


v_t __fastcall TThreadSearchCover::VrecSearchIndep(
									   AnsiString LogRowBegin,
									   int        I,
									   s_t        VertexSet,
									   vs_t       Vertex,
									   v_t        Indep
									)
{
	LogRowBegin += "." + IntToStr(I);

	Log += LogRowBegin + "  -- включаем в независимое множество вершину: "
		   + IntToStr(I) + "\n";

	ToConsol("шаг " + LogRowBegin
			 + " - включаем вершину в независимое множество");

	VertexToIndep(I,VertexSet,Vertex,Indep);

	Log += LogRowBegin + "  -- текущий подграф: \n"
		   + ToString(LogRowBegin + "        ",VertexSet,Vertex);

	ToConsol("шаг " + LogRowBegin
			 + " - поиск экстримальных вершин");

	SearchExtremIndep(LogRowBegin,VertexSet,Vertex,Indep);

	v_t MaxIndep(Indep);

	Log += LogRowBegin + "  -- текущее независимое множество:  ("
		   + IntToStr((int)Indep.size()) + ")  "
		   + ToString(Indep) + "\n";

	if (VertexSet.size() > 0) {

		Log += LogRowBegin + "  -- находим вершины с наибольшей суммой W1 и W2\n";

		ToConsol("шаг " + LogRowBegin
				 + " - строятся прогнозы");

		// максимальное независимое множество W1
		// оно может отличатся от _Indep так как мы выбираем вершину дающую
		// не максимальную W1 а максимальную сумму W1 и W2, в дальнейшем это может дать
		// либо большее либо меньшее W1, поэтому запоминаем максимальное MaxW1
		v_t  MaxW1;
		int VertexMaxW1 = 0;    // вершина давшая максимальное нез.множество (MaxW1)

		int MaxSumW = 0;       // максимальная сумма W1 и W2
		s_t VertexMaxSumW;        // номера вершин дающие наибольшую сумму W1 и W2

		for (s_t::iterator it = VertexSet.begin();
			 it != VertexSet.end(); ++it)
		{
			if (Terminated) {
				ToConsol("Процесс остановлен! Минимальное покрытие не найдено.");
				return v_t();
			}

			s_t  VertexW2(VertexSet);
			vs_t W2(Vertex);
			v_t  W1(Indep);

			// включаем вершину в независимое множество
			VertexToIndep(*it,VertexW2,W2,W1);

			// ищем изолированные и "висячие" вершины
			SearchExtremIndep(VertexW2,W2,W1);

			// находим вершину которая дает решение с наибольшим независимым множеством
			int WeightW2 = VpreMinDegree(VertexW2,W2);
			int SumW = W1.size() + WeightW2;

			Log += LogRowBegin + "  -- проверяем вершину " + IntToStr(*it);
			Log += " : W1 (" + IntToStr((int)W1.size());
			Log += ")   W2 (" + IntToStr(WeightW2);
			Log += ") - " + IntToStr(SumW) + "\n";

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

			// запоминаем максимальное независимое множество
			++Q;
			if (W1.size() > MaxW1.size()) {

				MaxW1 = W1;
				VertexMaxW1 = *it;
			}
		}    // end for it

		Log += LogRowBegin
			   + "  -- вершина дающая наибольшее W1 : "
			   + IntToStr(VertexMaxW1)+ "\n"
			   + LogRowBegin + "  -- запоминаем наибольшее W1: ("
			   + IntToStr((int)MaxW1.size()) + ") "
			   + ToString(MaxW1) +"\n";

		Log += LogRowBegin + "  -- вершины с наибольшей суммой W1 и W2: "
			   + ToString(VertexMaxSumW) + " - для каждой находим нез.множество\n";

		for (s_t::iterator it = VertexMaxSumW.begin();
			 it != VertexMaxSumW.end(); ++it)
		{
			v_t _Indep = VrecSearchIndep(LogRowBegin,*it,VertexSet,Vertex,Indep);

			if (_Indep.size() > MaxIndep.size())
				MaxIndep = _Indep;
		}

		if (MaxIndep.size() < MaxW1.size()) {

			MaxIndep = MaxW1;

			Log += LogRowBegin + "  -- максимальное множество W1: ("
				   + IntToStr((int)MaxW1.size()) + ")  "
				   + ToString(MaxW1) + " - больше найденного оставляем его\n";
		}
	}

    return MaxIndep;
}
//---------------------------------------------------------------------------


void __fastcall TThreadSearchCover::EquaSearchCover()
{
	try {

		ToConsol("search-cover equa " + FileName);

		if (Vertex.size() == 0) {
			ToConsol("Ошибка! Не задан граф. Минимальное покрытие не найдено.");
			return;
		}

		// задаем начальные данные характеристикам алгоритма
		Q        = 0;
		Cover    = v_t();
		LogShort = "МЕТОД УРАВНЕНИЙ\n\n";
		Log      = "Пошаговый отчет работы алгоритма: \n\n";
		QueryPerformanceCounter(&TimeBegin);

		// промежуточные данные
		ToConsol("Формируются промежуточные данные...");
		s_t VertexSet;    // множество номеров всех вершин графа
		for (int i = 1; i <= N; ++i)
			VertexSet.insert(i);

		vs_t Vertex_(Vertex);
		vv_t CoverTemp;

		v_t HEdgesCount(3,0);
		v_t HCoverLen(3,0);
		vstr_t HLog(3,"");
		HLog[0] = "00";
		HLog[1] = "01";
		HLog[2] = "10";

		int EdgesNum = 0;
		int Step = 1;

		while ((EdgesNum = EdgesCalculate(VertexSet,Vertex_)) > 0) {

			AnsiString StrStep = IntToStr(Step);

			ToConsol(StrStep + " поиск экстримальных вершин графа");

			Log += StrStep + " Текущий граф:\n";
			Log += "\t-- вершины (" + IntToStr((int)VertexSet.size()) + ") : \n\t-- "
				   + ToStringVertex(VertexSet,Vertex_,"\n\t-- ") + "\n";
			Log += "\t-- ребра: " + ToStringEdges(VertexSet,Vertex_,"   ") + "\n";
			Log += "\t-- покрытие: " + ToString(Cover) + "\n\n";

			Log += StrStep + ".1. Поиск экстримальных вершин:\n\n";
			SearchExtremCover("",VertexSet,Vertex_,Cover);

			if (Terminated) {
				ToConsol("Процесс остановлен! Минимальное покрытие не найдено.");
				return;
			}

			// находим пару вершин связанных ребром максимально суммарной степенью
			ToConsol(StrStep + " поиск вершин с максимальной суммарной степенью");

			int v1 = Max(VertexSet,Vertex_);
			VertexSet.erase(v1);
			int v2 = Max(VertexSet,Vertex_);

			if (v1 == -1 || v2 == -1)
				break;

			VertexSet.insert(v1);

			Log += "\n" + StrStep + ".2. Вершины с максимальной суммарной степенью:\n";
			Log += "\t-- " + IntToStr(v1) + " : " + IntToStr(v2) + "\n";

			// строим систему из трех уровнений,
			ToConsol(StrStep + " решается система уравнений 00 01 10");

			Log += "\n" + StrStep + ".3 Прогнозы системы трех уравнений:\n";

			vs_t HVertexSet(3,VertexSet);
			vvs_t HVertex(3,Vertex_);
			vv_t HCover(3,Cover);

			// первое уравнение обе вершины принадлежат мин.покрытию
			Log += "\n\t-- прогноз 00:\n";
			HCover[0].push_back(v1);
			HCover[0].push_back(v2);
			VertexErase(v1,HVertexSet[0],HVertex[0]);
			VertexErase(v2,HVertexSet[0],HVertex[0]);
			SearchExtremCover("\t-- 00",HVertexSet[0],HVertex[0],HCover[0]);

			Log += "\t-- 00 ребра (" + IntToStr(EdgesCalculate(HVertexSet[0],HVertex[0])) + ") : "
				   + ToStringEdges(HVertexSet[0],HVertex[0],"   ") + "\n";
			Log += "\t-- 00 покрытие (" + IntToStr((int)HCover[0].size())
				   + ") : " + ToString(HCover[0]) + "\n";

			// второе уравнение 1-я вершина принадлежит 2-я не принадлежит
			Log += "\n\t-- прогноз 01:\n";
			HCover[1].push_back(v1);
			VertexErase(v1,HVertexSet[1],HVertex[1]);
			copy(HVertex.at(1).at(v2).begin(),HVertex.at(1).at(v2).end(),back_inserter(HCover[1]));
			VertexAdjacentErase(v2,HVertexSet[1],HVertex[1]);
			SearchExtremCover("\t-- 01",HVertexSet[1],HVertex[1],HCover[1]);

			Log += "\t-- 01 ребра (" + IntToStr(EdgesCalculate(HVertexSet[1],HVertex[1])) + ") : "
				   + ToStringEdges(HVertexSet[1],HVertex[1],"   ") + "\n";
			Log += "\t-- 01 покрытие (" + IntToStr((int)HCover[1].size())
					+ ") : " + ToString(HCover[1]) + "\n";

			// третье уравнение 1-я не принадлежит 2-я принадлежит
			Log += "\n\t-- прогноз 10:\n";
			HCover[2].push_back(v2);
			VertexErase(v2,HVertexSet[2],HVertex[2]);
			copy(HVertex.at(2).at(v1).begin(),HVertex.at(2).at(v1).end(),back_inserter(HCover[2]));
			VertexAdjacentErase(v1,HVertexSet[2],HVertex[2]);
			SearchExtremCover("\t-- 10",HVertexSet[2],HVertex[2],HCover[2]);

			Log += "\t-- 10 ребра (" + IntToStr(EdgesCalculate(HVertexSet[2],HVertex[2])) + ") : "
				   + ToStringEdges(HVertexSet[2],HVertex[2],"   ") + "\n";
			Log += "\t-- 10 покрытие (" + IntToStr((int)HCover[2].size())
					+ ") : " + ToString(HCover[2]) + "\n";

			for (unsigned int i = 0; i < HEdgesCount.size(); ++i){
				HEdgesCount[i] = EdgesCalculate(HVertexSet.at(i),HVertex.at(i));
				HCoverLen[i] = HCover.at(i).size();
				if (HEdgesCount.at(i) == 0)
					CoverTemp.push_back(HCover.at(i));
			}

			// находим уравнение дающее максимальное количество ребер
			// и минимальное вершинное покрытие после преобразований
			int HMax = Max(Edges.size(),0,1,HEdgesCount,HCoverLen);
			HMax = Max(Edges.size(),HMax,2,HEdgesCount,HCoverLen);

			Log += "\n\t-- применяем решение " + HLog.at(HMax) + "\n\n";

			// применяем преобразования в зависимости от полученного прогноза
			ToConsol(StrStep + " преобразование графа");

			VertexSet.swap(HVertexSet.at(HMax));
			Vertex_.swap(HVertex.at(HMax));
			Cover.swap(HCover.at(HMax));

			++Step;
		}

		Log += "Найденные полные решения: \n\n";
		for (int i = 0; i < CoverTemp.size(); ++i)
			Log += "\t-- " + IntToStr(i + 1) + ". (" + IntToStr((int)CoverTemp.at(i).size())
					+ ") : " + ToString(CoverTemp.at(i)) + "\n";

		// проверяем избыточность найденных покрытий
		for (unsigned i = 0; i < CoverTemp.size(); ++i){

			v_t ICover(CoverTemp.at(i));
			ICover.reserve(CoverTemp.at(i).size());

			for (unsigned j = 0; j < ICover.size(); ++j){

				int tmp = *ICover.begin();
				ICover.erase(ICover.begin());

//				Log += "\n\t-- " + IntToStr((int)i) + IntToStr((int)j) + " : " + ToString(ICover);
				if (IsCovered(Edges,ICover)){
					CoverTemp[i] = ICover;
					break;
				}

				ICover.push_back(tmp);
			}

//			Log += "\n";
		}
		Log += "\nИсключаем из покрытия избыточные вершины:\n\n";
		for (int i = 0; i < CoverTemp.size(); ++i)
			Log += "\t-- " + IntToStr(i + 1) + ". (" + IntToStr((int)CoverTemp.at(i).size())
					+ ") : " + ToString(CoverTemp.at(i)) + "\n";

		// поиск наименьшего решения
		int MinCoverLen = INFIN;
		int MinIndex = -1;
		for (int i = 0; i < CoverTemp.size(); ++i)
			if (MinCoverLen > CoverTemp.at(i).size())
				MinCoverLen = CoverTemp.at(i).size(),
				MinIndex = i;

		if (MinIndex == -1)
			CoverTemp.push_back(Cover),
			MinIndex = 0;

		Log += "\nМаксимальное независимое множество: (" + IntToStr(static_cast<int>(N - CoverTemp.at(MinIndex).size()))
				+ ") : " + ToString(CoverToIndep(CoverTemp.at(MinIndex))) + "\n";

		Cover.swap(CoverTemp.at(MinIndex));

		QueryPerformanceCounter(&TimeEnd);

		ToConsol("Максимальное независимое множество найдено! Алгоритм завершил работу.");

		ToCover();

	} catch (...){
		ToConsol("Неизвестная ошибка! Максимальное независимое множество не найдено.");
	}
}
//---------------------------------------------------------------------------


void __fastcall TThreadSearchCover::TreeSearchCover()
{
	try {

		ToConsol("search-cover tree " + FileName);

		if (Vertex.size() == 0) {
			ToConsol("Ошибка! Не задан граф. Минимальное покрытие не найдено.");
			return;
		}

		// задаем начальные данные характеристикам алгоритма
		Q        = 0;
		Cover    = v_t();
		LogShort = "МЕТОД ДЕРЕВО ПУТЕЙ\n\n";

		if (WriteLog)
			Log      = "Пошаговый отчет работы алгоритма: \n\n";
		else
			Log = "";

		QueryPerformanceCounter(&TimeBegin);

		AnsiString Buffer = "";
		ss8_t mnm_max;
		int cnt_mnm = 0;	// количество максимальных независимых множеств (МНМ)

		for (int i = 1; i < N; ++i) {

			vector<path_t> tree(1,path_t());

			tree[0].x.insert(i);
			for (s_t::iterator it = Vertex[i].begin(); it != Vertex[i].end(); ++it)
				tree[0].y.insert(static_cast<uint8_t>(*it));

			ToConsol(Buffer.sprintf("путь %d, ярус 0 ...",i));

			if (WriteLog) {
				Log += Buffer + "\n\n";
				Log += ToString(tree);
            }

			bool next_path_is_build = true;
			int k = 0;       	// номер яруса
			ss8_t mnm;

			while (next_path_is_build) {

				next_path_is_build = false;
				ToConsol(Buffer.sprintf("путь %d, ярус %d ...",i,++k));

				vector<path_t> tree_next;

				for (int j = 0; j < tree.size(); ++j) {

					path_t p = tree.at(j);      // current path
					int8_t v = *p.x.rbegin();   // last vertex of X set


					// checking rule z1 and z2 must be connect
					Q += p.y.size();
					s8_t z1;
					for (int8_t u = 1; u < v; ++u)
						if (p.y.find(u) == p.y.end() && p.x.find(u) == p.x.end())
						   z1.insert(u);

					s8_t z2;
					Q += p.y.size();
					for (int8_t u = v + 1; u <= N; ++u)
						if (p.y.find(u) == p.y.end())
						   z2.insert(u);

					if (!TreeIsConnect(z1,z2))
						continue;

					// build next path
					for (s8_t::iterator it = z2.begin(); it != z2.end(); ++it) {

						Q += 2;

						int8_t u = *it;

						path_t p_next;
						TreePathUnion(p,u,&p_next);
						if (p_next.x.size() + p_next.y.size() == N) {
							InsertMax(p_next.x,&mnm);
							++cnt_mnm;
						} else if (u < N) {

							++Q;

							s8_t::iterator itup = p_next.y.upper_bound(u);

							if ((itup == p_next.y.end()) ||
								(*itup - u > 1)         ||
								(distance(itup,p_next.y.end()) != N - *itup + 1))
							{
								next_path_is_build = true;
								tree_next.push_back(p_next);
							}
						}
					}
				}

				tree.swap(tree_next);

				if (WriteLog) {
					Log += Buffer.sprintf("\n\t %3d - ярус\n\n",k);
					Log += Buffer.sprintf("\t дерево путей:\n\n");
					Log += ToString(tree);
					Log += Buffer.sprintf("\n\t максимальные независимые множества:\n\n");
					Log += ToString("\t",mnm);
					Log += "\n";
				}
			}

			if ( mnm_max.size() > 0 && mnm.size() > 0 && (*mnm.begin()).size() > (*mnm_max.begin()).size()) {
				mnm_max.swap(mnm);
			} else if ( mnm_max.size() == 0 || ( mnm.size() > 0 && (*mnm.begin()).size() == (*mnm_max.begin()).size())) {
                mnm_max.insert(mnm.begin(),mnm.end());
			}
		}

		QueryPerformanceCounter(&TimeEnd);

		for (int8_t i = 1; i <= N; ++i) {
			if ((*mnm_max.begin()).find(i) == (*mnm_max.begin()).end())
			Cover.push_back(static_cast<int>(i));
		}

		if (WriteLog) {
			Log += Buffer.sprintf("\n алгоритм завершил работу\n\n");
			Log += Buffer.sprintf("наибольшие максимальные независимые множества:\n\n");
			Log += ToString("",mnm_max);
			Log += "\n";
		}

		Note = "";
		Note += Buffer.sprintf("\n\tколиечество МНМ:\t\t %d\n",cnt_mnm);
		Note += Buffer.sprintf("\tколиечество наибольших МНМ: \t%d\n",mnm_max.size());
		Note += ToString("\t",mnm_max);

		ToConsol("Максимальное независимое множество найдено! Алгоритм завершил работу.");

		ToCover();

	} catch (...){
		ToConsol("Неизвестная ошибка! Максимальное независимое множество не найдено.");
	}
}
//---------------------------------------------------------------------------

void __fastcall TThreadSearchCover::TreePathUnion(const path_t &p,int v,path_t *p_next)
{
	//  Xn == X + v
	p_next->x = p.x;
	p_next->x.insert(v);

	Q += p.x.size() + 1;

	// Yn = Y + Y(v)
	set_union(p.y.begin(),p.y.end(),
			  Vertex.at(v).begin(),Vertex.at(v).end(),
			  inserter(p_next->y,p_next->y.begin()));

	Q += p.y.size() + Vertex.at(v).size();

}
//---------------------------------------------------------------------------


bool __fastcall TThreadSearchCover::TreeIsConnect(const s8_t &z1,const s8_t &z2)
{
	if (z1.size() == 0)
		return true;

	Q += z1.size();
	Q += z2.size();

	for (s8_t::const_iterator it1 = z1.begin(); it1 != z1.end(); ++it1)
		for (s8_t::const_iterator it2 = z2.begin(); it2 != z2.end(); ++it2)
			if (Vertex[*it1].find(*it2) != Vertex[*it1].end())
				return true;

    return false;
}
//---------------------------------------------------------------------------


inline int TThreadSearchCover::EdgesCountRemove(int I,const vs_t& Vertex_)
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


inline int TThreadSearchCover::Max(const s_t &VertexSet,const vs_t &Vertex)
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


inline int TThreadSearchCover::Max(int I1,int I2,const v_t &EdgesCount,const v_t &CoverLen)
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


inline int TThreadSearchCover::Max(int EdgesCountBegin,int I1,int I2,const v_t &EdgesCount,const v_t &CoverLen)
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


