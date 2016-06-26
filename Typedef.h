#ifndef Unit_TypedefH
#define Unit_TypedefH

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iostream>
#include <iterator>
#include <list>
#include <map>
#include <Math.hpp>
#include <numeric>
#include <random>
#include <set>
#include <stdio.h>
#include <string>
#include <StrUtils.hpp>
#include <vcl.h>
#include <vector>


#define INFIN 999999  // �������������

// �������������� �������
#define THR_NONE           0
#define THR_GENERATE_GRAPH 1
#define THR_GENERATE_TEST  2
#define THR_SEARCH_COVER   3
#define THR_TESTING        4

// ��� ����������� �������������� �����
#define TEST_MO   21  // ���.��������
#define TEST_CKO  22  // �������������������� ����������
#define TEST_COMP 23  // ���������� ��� ��������� ������

// ������� ������������ ���������� ����������� ������
#define CHANGE_N_A 31
#define CHANGE_N_T 32
#define CHANGE_A_T 33

// ���� �������
#define NONE        100    // �� ���� ������� �� �����������
#define GEN_RAND    101    // ��������� ���������� �����
#define GEN_HAND    102    // ��������� ����� �� ��������� ������ ������
#define GEN_RE      103    // ��������� ��������� ��� ���������� �����
#define CREATE_TEST 104

// ���� ����������
#define RANG        200    // ����� ���������� �������� �������� �������
#define FULL        201
#define INDS        202
#define FREQ        203
#define VERT        204
#define VPRE        205
#define VREC        206
#define EQUA        207
#define NIND        208

// �������� ����������
#define STR_RANG  "�������� �����"
#define STR_FULL  "������� ������ �������"
#define STR_INDS  "����� ����������� ��������"
#define STR_FREQ  "��������� �����"
#define STR_VERT  "����� ������"
#define STR_VPRE  "����� ������ � ���������"
#define STR_VREC  "����� ������ � ���������"
#define STR_EQUA  "����� ���������"
#define STR_NIND  "����� ����� ����������� ��������"

#define LIST_DIFF   200    // ������� ����� id ���������� � �� ������� � ������

// ��������� ����������
#define MESSAGE_BEGIN             "  -- "
#define MESSAGE_CLEAR             "clear"
#define MESSAGE_INIT              ">>"
#define MESSAGE_GEN_GRAPH         "��������� ����...\n"
#define MESSAGE_OPEN_GRAPH        "����������� ����...\n"
#define MESSAGE_SAVE_GRAPH        "����������� ����...\n"
#define MESSAGE_SAVE_LGRAPH       "����������� ��� ����� � ������...\n"
#define MESSAGE_SEARCH_COVER_RANG "���� ����� ������������ ���������� �������� �������� �������...\n"

#define PARAM_COUNT   7
#define QO   0
#define TE   1
#define PE   2
#define RL 	 3
#define RE   4
#define FE 	 5
#define NS	 6

#define STR_QO "���������� ��������"
#define STR_TE "����� ����������"
#define STR_PE "����������� ����������"
#define STR_RL "������������� ����� ��������"
#define STR_RE "������������� ������"
#define STR_FE "������� ��������� ������"
#define STR_NS "���������� ����������� ��������"

#define SUFIX_GRAPH      ".graph"
#define SUFIX_TEST       ".test"

using namespace std;

/* ����������� ����� */
typedef vector<bool>   vb_t;
typedef vector<vb_t>   vvb_t;

typedef vector<int>        v_t;
typedef vector<double>     vd_t;
typedef vector<v_t>        vv_t;
typedef vector<vd_t>       vvd_t;
typedef vector<vv_t>       vvv_t;
typedef vector<vvd_t>      vvvd_t;
typedef vector<AnsiString> vstr_t;

typedef set<int>       s_t;
typedef set<s_t>       ss_t;
typedef vector<s_t>    vs_t;
typedef vector<vs_t>   vvs_t;

typedef multiset<int>  ms_t;
typedef vector<ms_t>   vms_t;

typedef pair<int,int>           p_t;
typedef vector<p_t>             vp_t;
typedef pair<s_t,s_t>           ps_t;
typedef vector<pair<s_t,s_t> >  vps_t;
typedef set<pair<s_t,s_t> >     sps_t;

typedef void __fastcall (__closure *func_t)(void);

typedef struct _param_cover {

	AnsiString Q;        // ���������� �������� ����������� ����������
	AnsiString T;        // ����� � �� ���������� ����������
	AnsiString Cover;    // ������ ������ ���������� ����������� ��������� ���������
	AnsiString LenCover; // ����� ������������ ���������� ��������
	AnsiString LogShort; // ��� �������������� ����� ����
	AnsiString Log;      // ����� ���������� ���������

} param_cover_t;

// ��������� ������ �����
typedef struct _graph {

	AnsiString FileName;

	int N;  // ���������� ������
	int A;  // ���������
	int E;  // ���������� �����

	// ������ �����: ������ - ����� �����,
	// ������� - ������� ������� ��������� �����
	vs_t Edges;

	// ������� ���������: ������� - ������ ������,
	// 1 - ������� ��������� ������, 0 - ���
	vv_t Matrix;

	// ������ ������: ������ - ����� �������
	// ������� - ��������� ������� ������ � �������� ���� ����������
	vs_t Vertex;

	// ������ ������ ����� ����������, ���� ������� i � Veretx ��������� �
	// �������� j, �� � VertexAdd ��� � ��������
	vs_t VertexAdd;

	// ������ ��������� �������� ��������� ������� �����������
	// ���� - ����� ���������, ������� - ��������� ������ ���������� ��������
	map<int,param_cover_t> ParamCovers;

} graph_t;

typedef vector<graph_t*> vg_t;

typedef struct _test {

	AnsiString FileName;    	// �������� ����� �����

	unsigned int TypeParams;    // ������� ������������ ����������

	unsigned int Count;         // ���������� ������
	unsigned int ParamConst;	// �������������� ��������

	v_t ParamFirst;    			// �������� �������� ������� ���������
	v_t ParamSecond;    		// �������� �������� ������� ���������

	v_t Alg;           			// ����������� ���������

	map<int,vvvd_t> MO; 		// MO ������������� ���������� ������ ���.��������
	map<int,vvvd_t> CKO; 		// CKO ������������� ���������� ������ ���.��������

	bool SuccefullMO;       	// ������� ��� �� ���� �������
	bool SuccefullCKO;      	// ������� ��� ��� ���� �������

	_test() {

		FileName = "";
		TypeParams = 0;
		Count = 0;
		ParamConst = 0;
		ParamFirst = v_t();
		ParamSecond = v_t();
		Alg = v_t();

		MO = map<int,vvvd_t>();
		CKO = map<int,vvvd_t>();

		SuccefullMO = false;
		SuccefullCKO = false;
	}

} test_t;


// ��������� ����� �������� ���������� � ����������
typedef struct _alg {

	map<AnsiString,int> Id;
	map<int,AnsiString> Name;

	_alg() {

		Id[STR_RANG] = RANG;
		Id[STR_FULL] = FULL;
		Id[STR_FREQ] = FREQ;
		Id[STR_INDS] = INDS;
		Id[STR_VPRE] = VPRE;
		Id[STR_VREC] = VREC;
		Id[STR_VERT] = VERT;
		Id[STR_EQUA] = EQUA;
//		Id[STR_RSRV] = RSRV;   - reserve

		Name[RANG]   = STR_RANG;
		Name[FULL]   = STR_FULL;
		Name[FREQ]   = STR_FREQ;
		Name[INDS]   = STR_INDS;
		Name[VPRE]   = STR_VPRE;
		Name[VREC]   = STR_VREC;
		Name[VERT]   = STR_VERT;
		Name[EQUA]   = STR_EQUA;
//		Name[RSRV]   = STR_RSRV; - reserve
	}
} alg_t;


// ��������� ����� �������� ��������� ��������� � ���������
typedef struct _param_alg {

	map<AnsiString,int> Id;
	map<int,AnsiString> Name;

	_param_alg() {

		Id[STR_QO] = QO;
		Id[STR_TE] = TE;
		Id[STR_PE] = PE;
		Id[STR_RL] = RL;
		Id[STR_RE] = RE;
		Id[STR_FE] = FE;
		Id[STR_NS] = NS;

		Name[QO] = STR_QO;
		Name[TE] = STR_TE;
		Name[PE] = STR_PE;
		Name[RL] = STR_RL;
		Name[RE] = STR_RE;
		Name[FE] = STR_FE;
		Name[NS] = STR_NS;
	}
} param_alg_t;

#endif
