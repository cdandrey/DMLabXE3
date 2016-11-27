// ---------------------------------------------------------------------------

#ifndef TThreadTestGraphH
#define TThreadTestGraphH
// ---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include "TRandom.h"
#include "Typedef.h"

// ---------------------------------------------------------------------------
class TThreadTestGraph : public TThread {

private:

	/* промежуточные переменные */

	ofstream AutoSaveFile;

	LARGE_INTEGER Freq;	   // тактовая частота процессара, необходима для измерения времени выполнения

	vs_t Edges;            // список ребер графа
	vv_t Matrix;           // матрица смежности в двоичном виде
	vs_t Vertex;           // матрица смежности в десятичном виде
	vs_t VertexAdd;        // граф дополнение

	double Q;   	   	         // количество операций затраченное алгоритмом
	double TimeExecut;     		 // время выполнения алгоритма
	unsigned int LenCover; 		 // длина вершинного покрытия
	unsigned int LenMinCover;    // длина минимальное вершинного покрытия, вычисляется RANG и FULL
	unsigned int NumMinCovers; 	 // количество минимальных покрытий ( для методов независимых множеств)


	map<int,func_t> Algorithms;  // указатели на функции поиска минимального покрытия, которые необходимо тестировать

	/* выходные данные */

	map<int,vvvd_t> Result;

	/* функции взаимодействия потока с главной формой приложения */
	inline void __fastcall Lock();
	inline void __fastcall UnLock();

	inline void __fastcall SetParamGraph(unsigned int f,
										 unsigned int s,
										 unsigned int &_n,
										 unsigned int &_a,
										 unsigned int &_t);

		   void __fastcall ReturnErrorGraph();

	inline void __fastcall ToConsol(const AnsiString &Message);  // добовляем сообщение в Consol главной формы

	inline void __fastcall OpenAutoSaveFile(const AnsiString &Name);
	inline void __fastcall ToAutoSaveFile(const AnsiString &Data);
	inline void __fastcall ToAutoSaveFile(double Data);
	inline void __fastcall ToAutoSaveFile(const vd_t &Data);

	/* функции создания графа */
		   void __fastcall CreateGraph(unsigned int n,unsigned int a);
	inline int 	__fastcall UniformFromVectorErase(v_t &_Vector);

	/* функции поиска минимального покрытия */
		   void __fastcall SearchExtremCover(s_t &VertexSet_,vs_t &Vertex_,v_t &Cover_);
		   void __fastcall SearchExtremIndep(s_t &VertexSet_,vs_t &Vertex_,v_t &Indep_);    // функция поиска экстирмальных вершин
	inline void 		   VertexErase(int I, vs_t &Vertex_);							    // функция удаления вершины с графа
	inline void 		   VertexErase(int I,s_t  &VertexSet_,vs_t &Vertex_);			    // функция удаления вершины с графа
	inline void 		   VertexAdjacentErase(int   I,s_t   &VertexSet_,vs_t  &Vertex_);	// функция удаляет все смежные с I вершины из графа
	inline void 		   VertexToIndep(int I,s_t &VertexSet_,vs_t &Vertex_,v_t &Indep_);	// функция включения вершины графа в независимое множество
	inline int 			   VpreMinDegree(const s_t &VertexSet,const vs_t &Vertex);			// функция поиска вершины с минимальной степенью по заданному параметру
	       bool            IsCovered(const vs_t &Edg,const v_t &Cov);
	// функция подсчета количества ребер в графе по вектору инциденций
	int EdgesCalculate(const s_t &VertexSet,const vs_t &Vertex_) const {
		int num = 0;
		for (s_t::const_iterator it = VertexSet.begin();
			it != VertexSet.end(); ++it)
			num += Vertex_.at(*it).size();
		return num / 2;
	}

	//		   void __fastcall RsrvSearchCover(); - reserve
		   void __fastcall EquaSearchCover();	 // метод уравнений
	inline int			   EdgesCountRemove(int I,const vs_t &Vertex_);
	inline int 			   Max(const s_t &VertexSet,const vs_t &Vertex);
	inline int 			   Max(int I1,int I2,const v_t &EdgesCount,const v_t &CoverCount);
	inline int 			   Max(int EdgesCountBegin,int I1,int I2,const v_t &EdgesCount,const v_t &CoverLen);

		   void __fastcall FreqSearchCover();	// частотный метод

		   void __fastcall FullSearchCover(); 	//  метод быстрого полного перебора
		   v_t  __fastcall FullSearchIndep(int  I,s_t  VertexSet,vs_t Vertex,v_t  Indep);

		   void     __fastcall IndsSearchCover();	// метод независимых множеств
		   void     __fastcall NindSearchCover();
		   void     __fastcall NinuSearchCover();
		   void     __fastcall IndsUnionSets(int n,set<pair<s_t,s_t> > *pSets,unsigned *pLenCover);
		   void     __fastcall IndsUnionSets(set<pair<s_t,s_t> > *pSets);
		   void     __fastcall IndsUnionSetsAbsorb(int n,set<pair<s_t,s_t> > *pSets,unsigned *pLenCover);

		   void     __fastcall IndsRemoveUsedVertex(const pair<s_t,s_t> &UsedSet,set<pair<s_t,s_t> > *pSets);
		   unsigned __fastcall IndsBuildFullSet(int n,ps_t FullSet,sps_t Sets);

		   void __fastcall RangSearchCover();   // ранговый алгоритм поиска покртия

		   void __fastcall VertSearchCover();	// метод вершин

		   void __fastcall VpreSearchCover();   // метод вершин с прогнозом

		   void __fastcall VrecSearchCover();	// вершинный метод поиска минимального покрытия с рекурсией
		   v_t  __fastcall VrecSearchIndep(int I,s_t VertexSet,vs_t Vertex,v_t Indep);

	/* функции тестирования алгоритмов поиска минимальных покрытий графа*/
		   void __fastcall TestMO();
		   void __fastcall TestCOMP();

protected:

	void __fastcall Execute();

public:

	__fastcall TThreadTestGraph(bool CreateSuspended);

    __property Terminated;


	/* входные параметры тестирования */

	unsigned int TestIndex;     // номер теста в списке

	AnsiString FileName;

	unsigned int TypeParams;    // вариант изменяющихся параметров
	unsigned int TypeTest;      // тип вычислений: МО, СКО или сравнение двух алгоритмов

	unsigned int Count;         // количество тестов
	unsigned int ParamConst;    // неизменяющийся параметр

	v_t ParamFirst;             // диапозон значений первого параметра
	v_t ParamSecond;            // диапозон значений второго параметра

	v_t Alg;                    // тестируемые алгоритмы

	bool AutoSave;              // автосохранение результатов тестирования
};
// ---------------------------------------------------------------------------
#endif
