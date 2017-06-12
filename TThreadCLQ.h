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

	LARGE_INTEGER TimeBegin;      // ������ ������ ������ ���������
	LARGE_INTEGER TimeEnd;        // ������ ��������� ������ ���������
	LARGE_INTEGER Freq;           // ������� ����������

	/**** output data ****/

	int64_t    Q;                 // ���������� ��������
    int64_t    Cnt;
	v_t        Cover;             // ��������� ��������
	AnsiString LogShort;          // ������� �����
	AnsiString Log;               // �����
	AnsiString Note;              // �������

	/**** ������� �������������� ������ � ������� ������ ���������� ****/

	inline void 		  ToConsol(const AnsiString &Message);    // ��������� ��������� � Consol ������� �����

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
