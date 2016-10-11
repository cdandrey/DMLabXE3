//---------------------------------------------------------------------------

#ifndef TThreadSearchCoverH
#define TThreadSearchCoverH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include "Typedef.h"
//---------------------------------------------------------------------------
class TThreadSearchCover : public TThread
{

public:

	__fastcall TThreadSearchCover(bool CreateSuspended);

	__property Terminated;  // сигнал прерывания работы потока

	/**** входные данные потока ****/

	v_t ListFuncExecut;    // список функций которые необходимо выполнить потоку
	int FuncExecut;        // текущая выполняемая потоком функция


	AnsiString FileName;    // имя файла графа
	int N;                  // количество вершин
	int GraphIndex;         // номер графа в списке

	vs_t  Edges;            // список ребер
	vs_t  Vertex;           // список вершин
	vs_t  VertexAdd;        // список вершин графа дополнения


protected:

	void __fastcall Execute();

private:

	/**** переменные потока ****/

	map<int,func_t> FuncPoint;    // указатели на функции потока

	LARGE_INTEGER TimeBegin;      // момент начала работы алгоритма
	LARGE_INTEGER TimeEnd;        // момент окончания работы алгоритма
	LARGE_INTEGER Freq;           // частота процессора

	/**** выходные данные ****/

	double     Q;                 // количество операций
	v_t        Cover;             // вершинное покрытие
	AnsiString LogShort;          // краткий отчет
	AnsiString Log;               // отчет

	/**** функции взаимодействия потока с главной формой приложения ****/

	inline void 		  ToConsol(const AnsiString &Message);    // добовляем сообщение в Consol главной формы
		   void 		  ToCover();     // добовляет найденное минимальное покрытие в MapCovers
		   v_t            CoverToIndep(const v_t &cover);

	       void __fastcall Lock();
		   void __fastcall UnLock();

	/**** функции преобразования данных в текст ****/

	static AnsiString __fastcall ToString(const vp_t &data);                 // для рангового алгоритма
	static AnsiString __fastcall ToString(int ColCount,const vv_t &H);   // для рангового алгоритма
	static AnsiString __fastcall ToString(AnsiString Tab, const s_t  &SetData, const vs_t &VecSetData);
	static AnsiString __fastcall ToString(AnsiString Tab, const vs_t &data);
	static AnsiString __fastcall ToString(AnsiString infin, const v_t &data);  // для рангового алгоритма меняет значение INFIN на символ infin
	static AnsiString __fastcall ToString(const v_t  &data);
	static AnsiString __fastcall ToString(const s_t  &data);
	static AnsiString __fastcall ToString(const ps_t &data);
	static AnsiString __fastcall ToString(const sps_t &Data);
	static AnsiString __fastcall ToString(int count);

	static AnsiString __fastcall ToStringEdges(const s_t  &VertexSet,
											   const vs_t &Vertex_,
											   const AnsiString &Tab,
											   bool Numeration = false);

	static AnsiString __fastcall ToStringVertex(const s_t  &VertexSet,
												const vs_t &Vertex_,
												const AnsiString &Tab,
											    bool Short = false);
	/**** функции поиска минимального вершиного покрытия графа ****/

    // частотный метод поиска вершинных покрытий
	void __fastcall FreqSearchCover();

	// метод быстрого полного перебора
	void __fastcall FullSearchCover();

	v_t  __fastcall FullSearchIndep(
					   AnsiString LogRowBegin,
					   int        I,
					   s_t        VertexSet,
					   vs_t       Vertex,
					   v_t        Indep
					);

	// метод независимых множеств
	void __fastcall IndsSearchCover();
	void __fastcall NindSearchCover();
	void __fastcall IndsUnionSets(int n,set<pair<s_t,s_t> > *pSets,set<pair<s_t,s_t> > *pFullSets);
	void __fastcall IndsUnionSets(set<pair<s_t,s_t> > *pSets);
	void __fastcall IndsRemoveUsedVertex(const pair<s_t,s_t> &UsedSet,set<pair<s_t,s_t> > *pSets);
	ps_t __fastcall IndsBuildFullSet(int n,ps_t FullSet,sps_t Sets);

	// ранговый метод
	void __fastcall RangSearchCover();

	// метод вершин
	void __fastcall VertSearchCover();

	// метод вершин с прогнозом
	void __fastcall VpreSearchCover();
	inline int VpreMinDegree(const s_t &VertexSet,const vs_t &Vertex);

	// метод вершин с рекурсией
	void __fastcall VrecSearchCover();
	v_t  __fastcall VrecSearchIndep(
					   AnsiString LogRowBegin,
					   int        I,
					   s_t        VertexSet,
					   vs_t       VertexInc,
					   v_t        Indep
					);

	// метод уравнений
	void __fastcall EquaSearchCover();
	inline int EdgesCountRemove(int I,const vs_t &Vertex_);
	inline int Max(const s_t &VertexSet,const vs_t &Vertex);       				// функция возвращает итератор на вершину с максимальной степенью
	inline int Max(int I1,int I2,const v_t &EdgesCount,const v_t &CoverCount);	// фукнция возвращает лучший прогноз по заданному критерию
	inline int Max(int EdgesCountBegin,int I1,int I2,const v_t &EdgesCount,const v_t &CoverCount);	// фукнция возвращает лучший прогноз по заданному критерию


	/**** вспомогательные функции алгоритмов поиска вершинного покрытия ****/

	// функция получает вершинное покрытие из независимого множества
	inline v_t CoverFromIndep(s_t VertexSet,const v_t &Indep);

	// функции поиска связаных со всеми, висячих и изолированных вершин
	void __fastcall SearchExtremCover(
						AnsiString LevelLog,
						s_t        &_VertexSet,
						vs_t       &_VertexInc,
						v_t        &_Cover
					);

	void __fastcall SearchExtremCover(
						s_t        &_VertexSet,
						vs_t       &_VertexInc,
						v_t        &_Cover
					);

	void __fastcall SearchExtremIndep(
						AnsiString LevelLog,
						s_t        &_VertexSet,
						vs_t       &_VertexInc,
						v_t        &_Indep
					);

	void __fastcall SearchExtremIndep(
						s_t  &_VertexSet,
						vs_t &_VertexInc,
						v_t  &_Indep
					);

	// функция удаляет вершину и ее связи из графа
	inline void VertexErase(
					int  I,
					vs_t &_VertexInc
				);

	inline void VertexErase(
					int  I,
					s_t  &_VertexSet,
					vs_t &_VertexInc
				);

	// фукнция удаляет из графа все вершины смежные с вершинной I
	inline void VertexAdjacentErase(
					int   I,
					s_t   &VertexSet_,
					vs_t  &Vertex_
				);

	// функция включает вершину в нез.множество,
	// удаляет ее и связаные с ней вершины из графа
	inline void VertexToIndep(
					int  I,
					s_t  &_VertexSet,
					vs_t &_VertexInc,
					v_t  &_Indep
				);

	// функция подсчета количества ребер в графе по вектору инциденций
	int EdgesCalculate(const s_t &VertexSet,const vs_t &Vertex_) const {
		int num = 0;
		for (s_t::const_iterator it = VertexSet.begin();
			it != VertexSet.end(); ++it)
			num += Vertex_.at(*it).size();
		return num / 2;
	}

	bool IsCovered(const vs_t &Edg,const v_t &Cov);
};
//---------------------------------------------------------------------------
#endif
