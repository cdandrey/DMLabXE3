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
	/**** ���������� ������ ****/

	map<int,func_t> FuncPoint;    // ��������� �� ������� ������

	/**** �������� ������ ****/

	int E;                          // ���������� �����

	vs_t  Edges;            // ������ �����
	vv_t  Matrix;                   // ������� ���������
	vs_t  VertexAdd;                // ������ ������ ����� ����������
	vs_t  Vertex;           // ������ ������

	/**** ������� �������� ��������� � ������ ������ � ������� ����� ****/

	inline void __fastcall ToConsol(const AnsiString &Message);    // ��������� ��������� � Consol ������� �����

	       void __fastcall ToGraph();       // ��������� ��������� ���� � Graphs � ListBoxGraphs

	inline void __fastcall Lock();
	inline void __fastcall UnLock();

	/**** ������� ��������� ����� ****/

	void __fastcall GenerateRand();
	void __fastcall GenerateHand();

	inline int __fastcall UniformFromVectorErase(v_t &_Vector);
	vs_t       __fastcall ToInt(const vstr_t &Str);    // ������� ����������� ��������� ������ ������ � ����������

protected:

	void __fastcall Execute();

public:

	__fastcall TThreadCreateGraph(bool CreateSuspended);

	__property Terminated;  // ������ ���������� ������ ������

	/**** ������� ������ ������ ****/

	int FuncExecut;         // ����� �������� �����

	AnsiString FileName;    // ��� ����� �����

	int N;                  // ���������� ������
	int A;                  // ���������

	vstr_t VertexStr;       // ������ ������ � ��������� �������
};
//---------------------------------------------------------------------------
#endif
