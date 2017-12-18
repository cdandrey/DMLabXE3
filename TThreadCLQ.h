//---------------------------------------------------------------------------

#ifndef TThreadCLQH
#define TThreadCLQH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include "Typedef.h"
//---------------------------------------------------------------------------
class TThreadCLQ : public TThread
{
	static const unsigned C_BASE_CLIQUE_LEN = 3;

	LARGE_INTEGER TimeBegin;      // момент начала работы алгоритма
	LARGE_INTEGER TimeEnd;        // момент окончания работы алгоритма
	LARGE_INTEGER Freq;           // частота процессора

	/**** output data ****/

	int64_t    Q;                 // количество операций
    int64_t    Cnt;
	v_t        Cover;             // вершинное покрытие
	AnsiString LogShort;          // краткий отчет
	AnsiString Log;               // отчет
	AnsiString Note;              // заметки

	/**** функции взаимодействия потока с главной формой приложения ****/

	inline void            ToConsol(const AnsiString &Message);
	inline void            ToLog(const AnsiString &Message);

		   void __fastcall Lock();
		   void __fastcall UnLock();

		   AnsiString __fastcall ToString(const v_t &Data,int Begin = 0);
		   AnsiString __fastcall ToString(const s_t &Data,AnsiString Tab = "");
		   AnsiString __fastcall ToString(ss_t &Data);
		   AnsiString __fastcall ToString(vs_t &Data,AnsiString Tab = "");

		   s_t        __fastcall ToSet(const v_t &data);

		   void       __fastcall SearchCliqueTreangl();
		   void       __fastcall SearchCliqueTreangl2();

		   void       __fastcall SplitOnTreanglsAll(
									 unsigned    u       ,
									 unsigned    level   ,
									 const vs_t &lsa     ,
									 v_t         visit   ,
									 v_t        *path    ,
									 vs_t       *treangls
								 );

		   void       __fastcall SplitOnTreanglsTop(
									 unsigned    u        ,
									 unsigned    level    ,
									 const vs_t &lsa      ,
									 v_t         visit    ,
									 v_t        *path     ,
									 vs_t       *treangls
								 );

		   void       __fastcall SplitOnTreanglsOnce(
									 unsigned    u        ,
									 unsigned    level    ,
									 const vs_t &lsa      ,
									 v_t         visit    ,
									 v_t        *path     ,
									 vs_t       *trgl
								 );

		   void       __fastcall Sort(const vs_t &lsa,v_t *mv,v_t *mvr,vs_t *lsa_sort);

		   void       __fastcall MergeTreanglsAll(s_t *clq_max,const vs_t &trgl,const vs_t &lsa);
		   void       __fastcall MergeTreanglsLine(unsigned u,s_t clq,s_t *clq_max,const vs_t &trgl,const vs_t &lsa);
           void       __fastcall LastChecked(const vs_t &lsa,s_t *clq);
		   bool       __fastcall IsFull(const s_t &vertex,const vs_t &list_adjacent);
		   bool       __fastcall IsMerge(const s_t &clq,const s_t &trgl,const vs_t &lsa);

           //
		   void       __fastcall SplitOnTreangls(ss_t *treangls, vs_t *degree);
		   void       __fastcall SplitOnTreangls(vs_t *degree);
		   void       __fastcall SplitOnTreangls3(vs_t *treangls);
		   int        __fastcall MaxSubgraph(const vs_t  &degree);
		   void       __fastcall BuildSubgraph(const s_t &sub_vertex,vs_t *sub_graph);

		   void       __fastcall ExtractMaxClique(s_t *sub_vertex,vs_t *sub_graph);
		   void       __fastcall ExtractMaxClique2(
								s_t  *clique,
								ss_t &treangls,
								v_t  &treangls_degree,
								vs_t &list_adjacent
							);

		   void       __fastcall ExtractMaxClique2Log(
								s_t  *clique,
								ss_t &treangls,
								v_t  &treangls_degree,
								vs_t &list_adjacent
							);

		   void       __fastcall dfs(
								unsigned u              ,
								unsigned level          ,
								v_t      visit          ,
								v_t     *path           ,
								ss_t    *treangls       ,
								v_t     *treangls_degree,
								vs_t    *list_adjacent
						);

		   void       __fastcall dfs(
								unsigned u        ,
								unsigned level    ,
								v_t      visit    ,
								v_t     *path     ,
								ss_t    *treangls ,
								vs_t    *degree
						);

		   void       __fastcall dfs(
								unsigned u        ,
								unsigned level    ,
								v_t      visit    ,
								v_t     *path     ,
								vs_t    *degree
						);


		   // graph coloring
		   void __fastcall SearchColoringBipartite();
		   void __fastcall BuildEdgesCover(const vs_t &ls,vp_t *edges);
		   void __fastcall BuildEdgesCover(unsigned v,const vs_t &ls,unsigned *cnt,v_t *visit,vp_t *edges);
		   void __fastcall BuildBipartGraph(const vs_t &ls,const vp_t &edges,unsigned *coloring,v_t *mis);
		   void __fastcall BuildBipartGraph(int v,const vs_t &ls,vs_t *parts);
           bool __fastcall IsConnect(int v,const s_t &part,const vs_t &ls);

protected:
	void __fastcall Execute();

public:
	__fastcall TThreadCLQ(bool CreateSuspended);

	__property Terminated;  // сигнал прерывания работы потока

	AnsiString FileName;    // name of graph
	int N;                  // numbers of vertex
	int GraphIndex;         // number of graph in list

	vs_t  Vertex;           // list adjacent of vertex
	vs_t  VertexAdd;        // list adjacent of vertex of graphs complement

	bool WriteLog;

	unsigned SelectAlg;
	unsigned SelTreangls;
	unsigned SelSort;
	unsigned SelMerge;
    unsigned SelLastChecked;

	v_t ListFuncExecut;    // список функций которые необходимо выполнить потоку
	map<int,func_t> FuncPoint;    // указатели на функции потока
    int FuncExecut;

};
//---------------------------------------------------------------------------
#endif
