//---------------------------------------------------------------------------

#ifndef TThreadCLQH
#define TThreadCLQH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include "Typedef.h"
//---------------------------------------------------------------------------
class TThreadCLQ : public TThread
{
	/**** ������� �������������� ������ � ������� ������ ���������� ****/

	inline void 		  ToConsol(const AnsiString &Message);    // ��������� ��������� � Consol ������� �����

		   void __fastcall Lock();
		   void __fastcall UnLock();

protected:
	void __fastcall Execute();

public:
	__fastcall TThreadCLQ(bool CreateSuspended);

	AnsiString FileName;    // ��� ����� �����
	int N;                  // ���������� ������
	int GraphIndex;         // ����� ����� � ������

	vs_t  Edges;            // ������ �����
	vs_t  Vertex;           // ������ ������
	vs_t  VertexAdd;        // ������ ������ ����� ����������
};
//---------------------------------------------------------------------------
#endif
