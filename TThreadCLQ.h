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

	inline void 		  ToConsol(const AnsiString &Message);    // добовляем сообщение в Consol главной формы

		   void __fastcall Lock();
		   void __fastcall UnLock();

		   void __fastcall dfs(unsigned u,unsigned level,v_t visit,v_t *treangl);

		   AnsiString __fastcall ToString(v_t &Data);

protected:
	void __fastcall Execute();

public:
	__fastcall TThreadCLQ(bool CreateSuspended);

	AnsiString FileName;    // name of graph
	int N;                  // numbers of vertex
	int GraphIndex;         // number of graph in list

	vs_t  Edges;            // list of edges
	vs_t  Vertex;           // list adjacent of vertex
	vs_t  VertexAdd;        // list adjacent of vertex of graphs complement

};
//---------------------------------------------------------------------------
#endif
