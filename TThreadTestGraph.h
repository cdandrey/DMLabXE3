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

	/* ������������� ���������� */

	ofstream AutoSaveFile;

	LARGE_INTEGER Freq;	   // �������� ������� ����������, ���������� ��� ��������� ������� ����������

	vs_t Edges;            // ������ ����� �����
	vv_t Matrix;           // ������� ��������� � �������� ����
	vs_t Vertex;           // ������� ��������� � ���������� ����
	vs_t VertexAdd;        // ���� ����������

	double Q;   	   	         // ���������� �������� ����������� ����������
	double TimeExecut;     		 // ����� ���������� ���������
	unsigned int LenCover; 		 // ����� ���������� ��������
	unsigned int LenMinCover;    // ����� ����������� ���������� ��������, ����������� RANG � FULL
	unsigned int NumSets;		 // ���������� ����������� ��������


	map<int,func_t> Algorithms;  // ��������� �� ������� ������ ������������ ��������, ������� ���������� �����������

	/* �������� ������ */

	map<int,vvvd_t> Result;

	/* ������� �������������� ������ � ������� ������ ���������� */
	inline void __fastcall Lock();
	inline void __fastcall UnLock();

	inline void __fastcall SetParamGraph(unsigned int f,
										 unsigned int s,
										 unsigned int &_n,
										 unsigned int &_a,
										 unsigned int &_t);

		   void __fastcall ReturnErrorGraph();

	inline void __fastcall ToConsol(const AnsiString &Message);  // ��������� ��������� � Consol ������� �����

	inline void __fastcall OpenAutoSaveFile(const AnsiString &Name);
	inline void __fastcall ToAutoSaveFile(const AnsiString &Data);
	inline void __fastcall ToAutoSaveFile(double Data);
	inline void __fastcall ToAutoSaveFile(const vd_t &Data);

	/* ������� �������� ����� */
		   void __fastcall CreateGraph(unsigned int n,unsigned int a);
	inline int 	__fastcall UniformFromVectorErase(v_t &_Vector);

	/* ������� ������ ������������ �������� */
		   void __fastcall SearchExtremCover(s_t &VertexSet_,vs_t &Vertex_,v_t &Cover_);
		   void __fastcall SearchExtremIndep(s_t &VertexSet_,vs_t &Vertex_,v_t &Indep_);    // ������� ������ ������������� ������
	inline void 		   VertexErase(int I, vs_t &Vertex_);							    // ������� �������� ������� � �����
	inline void 		   VertexErase(int I,s_t  &VertexSet_,vs_t &Vertex_);			    // ������� �������� ������� � �����
	inline void 		   VertexAdjacentErase(int   I,s_t   &VertexSet_,vs_t  &Vertex_);	// ������� ������� ��� ������� � I ������� �� �����
	inline void 		   VertexToIndep(int I,s_t &VertexSet_,vs_t &Vertex_,v_t &Indep_);	// ������� ��������� ������� ����� � ����������� ���������
	inline int 			   VpreMinDegree(const s_t &VertexSet,const vs_t &Vertex);			// ������� ������ ������� � ����������� �������� �� ��������� ���������
	       bool            IsCovered(const vs_t &Edg,const v_t &Cov);
	// ������� �������� ���������� ����� � ����� �� ������� ����������
	int EdgesCalculate(const s_t &VertexSet,const vs_t &Vertex_) const {
		int num = 0;
		for (s_t::const_iterator it = VertexSet.begin();
			it != VertexSet.end(); ++it)
			num += Vertex_.at(*it).size();
		return num / 2;
	}

	//		   void __fastcall RsrvSearchCover(); - reserve
		   void __fastcall EquaSearchCover();	 // ����� ���������
	inline int			   EdgesCountRemove(int I,const vs_t &Vertex_);
	inline int 			   Max(const s_t &VertexSet,const vs_t &Vertex);
	inline int 			   Max(int I1,int I2,const v_t &EdgesCount,const v_t &CoverCount);
	inline int 			   Max(int EdgesCountBegin,int I1,int I2,const v_t &EdgesCount,const v_t &CoverLen);

		   void __fastcall FreqSearchCover();	// ��������� �����

		   void __fastcall FullSearchCover(); 	//  ����� �������� ������� ��������
		   v_t  __fastcall FullSearchIndep(int  I,s_t  VertexSet,vs_t Vertex,v_t  Indep);

		   void __fastcall IndsSearchCover();	// ����� ����������� ��������

		   void __fastcall RangSearchCover();   // �������� �������� ������ �������

		   void __fastcall VertSearchCover();	// ����� ������

		   void __fastcall VpreSearchCover();   // ����� ������ � ���������

		   void __fastcall VrecSearchCover();	// ��������� ����� ������ ������������ �������� � ���������
		   v_t  __fastcall VrecSearchIndep(int I,s_t VertexSet,vs_t Vertex,v_t Indep);

	/* ������� ������������ ���������� ������ ����������� �������� �����*/
		   void __fastcall TestMO();
		   void __fastcall TestCOMP();

protected:

	void __fastcall Execute();

public:

	__fastcall TThreadTestGraph(bool CreateSuspended);

    __property Terminated;


	/* ������� ��������� ������������ */

	unsigned int TestIndex;     // ����� ����� � ������

	AnsiString FileName;

	unsigned int TypeParams;    // ������� ������������ ����������
	unsigned int TypeTest;      // ��� ����������: ��, ��� ��� ��������� ���� ����������

	unsigned int Count;         // ���������� ������
	unsigned int ParamConst;    // �������������� ��������

	v_t ParamFirst;             // �������� �������� ������� ���������
	v_t ParamSecond;            // �������� �������� ������� ���������

	v_t Alg;                    // ����������� ���������

	bool AutoSave;              // �������������� ����������� ������������
};
// ---------------------------------------------------------------------------
#endif
