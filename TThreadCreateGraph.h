//---------------------------------------------------------------------------

#ifndef TThreadCreateGraphH
#define TThreadCreateGraphH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include "Typedef.h"
#include "TRandom.h"
//---------------------------------------------------------------------------
class TThreadCreateGraph : public TThread
{
private:
	/**** переменные потока ****/

	map<int,func_t> FuncPoint;    // указатели на функции потока

	/**** выходные данные ****/

	int E;                          // количество ребер

	vs_t  Edges;            // список ребер
	vv_t  Matrix;                   // матрица смежности
	vs_t  VertexAdd;                // список вершин графа дополнения
	vs_t  Vertex;           // список вершин

	/**** функции отправки сообщений и вывода данных в главную форму ****/

	inline void __fastcall ToConsol(const AnsiString &Message);    // добовляем сообщение в Consol главной формы

	       void __fastcall ToGraph();       // добовляет созданный граф в Graphs и ListBoxGraphs

	inline void __fastcall Lock();
	inline void __fastcall UnLock();

	/**** функции генерации графа ****/

	void __fastcall GenerateRand();
	void __fastcall GenerateHand();

	inline int __fastcall UniformFromVectorErase(v_t &_Vector);
	vs_t       __fastcall ToInt(const vstr_t &Str);    // функция преобразует строковый список вершин в десятичный

protected:

	void __fastcall Execute();

public:

	__fastcall TThreadCreateGraph(bool CreateSuspended);

	__property Terminated;  // сигнал прерывания работы потока

	/**** входные данные потока ****/

	int FuncExecut;         // метод создания графа

	AnsiString FileName;    // имя файла графа

	int N;                  // количество вершин
	int A;                  // плотность

	vstr_t VertexStr;       // списко вершин в строковом формате
};
//---------------------------------------------------------------------------
#endif
