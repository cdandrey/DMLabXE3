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

	__property Terminated;  // ������ ���������� ������ ������

	/**** ������� ������ ������ ****/

	v_t ListFuncExecut;    // ������ ������� ������� ���������� ��������� ������
	int FuncExecut;        // ������� ����������� ������� �������


	AnsiString FileName;    // ��� ����� �����
	int N;                  // ���������� ������
	int GraphIndex;         // ����� ����� � ������

	vs_t  Edges;            // ������ �����
	vs_t  Vertex;           // ������ ������
	vs_t  VertexAdd;        // ������ ������ ����� ����������


protected:

	void __fastcall Execute();

private:

	/**** ���������� ������ ****/

	map<int,func_t> FuncPoint;    // ��������� �� ������� ������

	LARGE_INTEGER TimeBegin;      // ������ ������ ������ ���������
	LARGE_INTEGER TimeEnd;        // ������ ��������� ������ ���������
	LARGE_INTEGER Freq;           // ������� ����������

	/**** �������� ������ ****/

	double     Q;                 // ���������� ��������
	v_t        Cover;             // ��������� ��������
	AnsiString LogShort;          // ������� �����
	AnsiString Log;               // �����

	/**** ������� �������������� ������ � ������� ������ ���������� ****/

	inline void 		  ToConsol(const AnsiString &Message);    // ��������� ��������� � Consol ������� �����
		   void 		  ToCover();     // ��������� ��������� ����������� �������� � MapCovers
		   v_t            CoverToIndep(const v_t &cover);

	       void __fastcall Lock();
		   void __fastcall UnLock();

	/**** ������� �������������� ������ � ����� ****/

	static AnsiString __fastcall ToString(const vp_t &data);                 // ��� ��������� ���������
	static AnsiString __fastcall ToString(int ColCount,const vv_t &H);   // ��� ��������� ���������
	static AnsiString __fastcall ToString(AnsiString Tab, const s_t  &SetData, const vs_t &VecSetData);
	static AnsiString __fastcall ToString(AnsiString Tab, const vs_t &data);
	static AnsiString __fastcall ToString(AnsiString infin, const v_t &data);  // ��� ��������� ��������� ������ �������� INFIN �� ������ infin
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
	/**** ������� ������ ������������ ��������� �������� ����� ****/

    // ��������� ����� ������ ��������� ��������
	void __fastcall FreqSearchCover();

	// ����� �������� ������� ��������
	void __fastcall FullSearchCover();

	v_t  __fastcall FullSearchIndep(
					   AnsiString LogRowBegin,
					   int        I,
					   s_t        VertexSet,
					   vs_t       Vertex,
					   v_t        Indep
					);

	// ����� ����������� ��������
	void __fastcall IndsSearchCover();
	void __fastcall NindSearchCover();
	void __fastcall IndsUnionSets(int n,set<pair<s_t,s_t> > *pSets,set<pair<s_t,s_t> > *pFullSets);
	void __fastcall IndsUnionSets(set<pair<s_t,s_t> > *pSets);
	void __fastcall IndsRemoveUsedVertex(const pair<s_t,s_t> &UsedSet,set<pair<s_t,s_t> > *pSets);
	ps_t __fastcall IndsBuildFullSet(int n,ps_t FullSet,sps_t Sets);

	// �������� �����
	void __fastcall RangSearchCover();

	// ����� ������
	void __fastcall VertSearchCover();

	// ����� ������ � ���������
	void __fastcall VpreSearchCover();
	inline int VpreMinDegree(const s_t &VertexSet,const vs_t &Vertex);

	// ����� ������ � ���������
	void __fastcall VrecSearchCover();
	v_t  __fastcall VrecSearchIndep(
					   AnsiString LogRowBegin,
					   int        I,
					   s_t        VertexSet,
					   vs_t       VertexInc,
					   v_t        Indep
					);

	// ����� ���������
	void __fastcall EquaSearchCover();
	inline int EdgesCountRemove(int I,const vs_t &Vertex_);
	inline int Max(const s_t &VertexSet,const vs_t &Vertex);       				// ������� ���������� �������� �� ������� � ������������ ��������
	inline int Max(int I1,int I2,const v_t &EdgesCount,const v_t &CoverCount);	// ������� ���������� ������ ������� �� ��������� ��������
	inline int Max(int EdgesCountBegin,int I1,int I2,const v_t &EdgesCount,const v_t &CoverCount);	// ������� ���������� ������ ������� �� ��������� ��������


	/**** ��������������� ������� ���������� ������ ���������� �������� ****/

	// ������� �������� ��������� �������� �� ������������ ���������
	inline v_t CoverFromIndep(s_t VertexSet,const v_t &Indep);

	// ������� ������ �������� �� �����, ������� � ������������� ������
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

	// ������� ������� ������� � �� ����� �� �����
	inline void VertexErase(
					int  I,
					vs_t &_VertexInc
				);

	inline void VertexErase(
					int  I,
					s_t  &_VertexSet,
					vs_t &_VertexInc
				);

	// ������� ������� �� ����� ��� ������� ������� � ��������� I
	inline void VertexAdjacentErase(
					int   I,
					s_t   &VertexSet_,
					vs_t  &Vertex_
				);

	// ������� �������� ������� � ���.���������,
	// ������� �� � �������� � ��� ������� �� �����
	inline void VertexToIndep(
					int  I,
					s_t  &_VertexSet,
					vs_t &_VertexInc,
					v_t  &_Indep
				);

	// ������� �������� ���������� ����� � ����� �� ������� ����������
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
