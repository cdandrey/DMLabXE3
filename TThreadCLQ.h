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

	inline void            ToConsol(const AnsiString &Message);
	inline void            ToLog(const AnsiString &Message);

		   void __fastcall Lock();
		   void __fastcall UnLock();

		   AnsiString __fastcall ToString(v_t &Data);
		   AnsiString __fastcall ToString(const s_t &Data);
		   AnsiString __fastcall ToString(ss_t &Data);
		   AnsiString __fastcall ToString(vs_t &Data);

		   s_t        __fastcall ToSet(const v_t &data);

		   void       __fastcall SearchCliqueTreangl();
		   void       __fastcall SplitOnTreangls(ss_t *treangls, v_t *degree);
		   int        __fastcall MaxSubgraph(
									const ss_t &treangls,
									const v_t  &degree,
									s_t        *sub_vertex
								 );
		   void       __fastcall BuildSubgraph(
									const s_t &sub_vertex,
									vs_t      *sub_graph
								 );

		   void       __fastcall ExtractMaxClique(s_t *sub_vertex,vs_t *sub_graph);
		   bool       __fastcall IsFull(const s_t &vertex,const vs_t &list_adjacent);

		   void       __fastcall dfs(
								unsigned u        ,
								unsigned level    ,
								v_t      visit    ,
								v_t     *path     ,
								ss_t    *treangls ,
		   						v_t     *degree
		   				);

protected:
	void __fastcall Execute();

public:
	__fastcall TThreadCLQ(bool CreateSuspended);

	__property Terminated;  // ������ ���������� ������ ������

	AnsiString FileName;    // name of graph
	int N;                  // numbers of vertex
	int GraphIndex;         // number of graph in list

	vs_t  Edges;            // list of edges
	vs_t  Vertex;           // list adjacent of vertex
	vs_t  VertexAdd;        // list adjacent of vertex of graphs complement

    bool WriteLog;

};
//---------------------------------------------------------------------------
#endif