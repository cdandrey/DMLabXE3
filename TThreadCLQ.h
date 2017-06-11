//---------------------------------------------------------------------------

#ifndef TThreadCLQH
#define TThreadCLQH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include "Typedef.h"
//---------------------------------------------------------------------------
class TThreadCLQ : public TThread
{
	/**** функции взаимодействия потока с главной формой приложения ****/

	inline void 		  ToConsol(const AnsiString &Message);    // добовляем сообщение в Consol главной формы

		   void __fastcall Lock();
		   void __fastcall UnLock();

protected:
	void __fastcall Execute();

public:
	__fastcall TThreadCLQ(bool CreateSuspended);

	AnsiString FileName;    // имя файла графа
	int N;                  // количество вершин
	int GraphIndex;         // номер графа в списке

	vs_t  Edges;            // список ребер
	vs_t  Vertex;           // список вершин
	vs_t  VertexAdd;        // список вершин графа дополнения
};
//---------------------------------------------------------------------------
#endif
