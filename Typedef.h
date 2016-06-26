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


#define INFIN 999999  // бесконечность

// идентификаторы потоков
#define THR_NONE           0
#define THR_GENERATE_GRAPH 1
#define THR_GENERATE_TEST  2
#define THR_SEARCH_COVER   3
#define THR_TESTING        4

// тип вычисл€емой характеристики теста
#define TEST_MO   21  // мат.ожидание
#define TEST_CKO  22  // среднеквадратическое отклонение
#define TEST_COMP 23  // сравнивать два выбранных метода

// вариант измен€ющихс€ параметров тестируемых графов
#define CHANGE_N_A 31
#define CHANGE_N_T 32
#define CHANGE_A_T 33

// коды функций
#define NONE        100    // ни один процесс не выполн€етс€
#define GEN_RAND    101    // генераци€ случайного графа
#define GEN_HAND    102    // генераци€ графа по заданному списку вершин
#define GEN_RE      103    // повторна€ генераци€ уже созданного графа
#define CREATE_TEST 104

// коды алгоритмов
#define RANG        200    // поиск вершинного покрыти€ ранговым методом
#define FULL        201
#define INDS        202
#define FREQ        203
#define VERT        204
#define VPRE        205
#define VREC        206
#define EQUA        207
#define NIND        208

// названи€ алгоритмов
#define STR_RANG  "–анговый метод"
#define STR_FULL  "Ѕыстрый полный перебор"
#define STR_INDS  "ћетод независимых множеств"
#define STR_FREQ  "„астотный метод"
#define STR_VERT  "ћетод вершин"
#define STR_VPRE  "ћетод вершин с прогнозом"
#define STR_VREC  "ћетод вершин с рекурсией"
#define STR_EQUA  "ћетод уравнений"
#define STR_NIND  "Ќовый метод независимых множеств"

#define LIST_DIFF   200    // разница между id алгоритмов и их номеров в списке

// состо€ние приложени€
#define MESSAGE_BEGIN             "  -- "
#define MESSAGE_CLEAR             "clear"
#define MESSAGE_INIT              ">>"
#define MESSAGE_GEN_GRAPH         "—оздаетс€ граф...\n"
#define MESSAGE_OPEN_GRAPH        "ќткрываетс€ граф...\n"
#define MESSAGE_SAVE_GRAPH        "—охран€етс€ граф...\n"
#define MESSAGE_SAVE_LGRAPH       "—охран€ютс€ все графы в списке...\n"
#define MESSAGE_SEARCH_COVER_RANG "»дет поиск минимального вершинного покрыти€ ранговым методом...\n"

#define PARAM_COUNT   7
#define QO   0
#define TE   1
#define PE   2
#define RL 	 3
#define RE   4
#define FE 	 5
#define NS	 6

#define STR_QO " ќЋ»„≈—“¬ќ ќѕ≈–ј÷»…"
#define STR_TE "¬–≈ћя ¬џѕќЋЌ≈Ќ»я"
#define STR_PE "¬≈–ќя“Ќќ—“№ ¬џѕќЋЌ≈Ќ»я"
#define STR_RL "ќ“Ќќ—»“≈Ћ№Ќјя ƒЋ»Ќј ѕќ –џ“»я"
#define STR_RE "ќ“Ќќ—»“≈Ћ№Ќјя ќЎ»Ѕ ј"
#define STR_FE "„ј—“ќ“ј ѕќя¬Ћ≈Ќ»я ќЎ»Ѕ »"
#define STR_NS " ќЋ»„≈—“¬ќ Ќ≈«ј¬»—»ћџ’ ћЌќ∆≈—“¬"

#define SUFIX_GRAPH      ".graph"
#define SUFIX_TEST       ".test"

using namespace std;

/* определение типов */
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

	AnsiString Q;        // количество операций затраченное алгоритмом
	AnsiString T;        // врем€ в мс затраченое алгоритмом
	AnsiString Cover;    // номера вершин €вл€ющихс€ минимальным вершинным покрытием
	AnsiString LenCover; // длина минимального вершинного покрыти€
	AnsiString LogShort; // все характеристики кроме лога
	AnsiString Log;      // отчет выполнени€ алгоритма

} param_cover_t;

// структура данных графа
typedef struct _graph {

	AnsiString FileName;

	int N;  // количество вершин
	int A;  // плотность
	int E;  // количество ребер

	// список ребер: индекс - номер ребар,
	// элемент - вершины которые соеден€ет ребро
	vs_t Edges;

	// матрица смежности: индексы - номера вершин,
	// 1 - вершины соедиютс€ ребром, 0 - нет
	vv_t Matrix;

	// список вершин: индекс - номер вершины
	// элемент - множество номеров вершин с которыми есть соединени€
	vs_t Vertex;

	// список вершин графа дополнени€, если вершина i в Veretx соединена с
	// вершиной j, то в VertexAdd нет и наоборот
	vs_t VertexAdd;

	// список вершинных покрытий найденных разными алгоритмами
	// ключ - номер алгоритма, элемент - структура данных вершинного покрыти€
	map<int,param_cover_t> ParamCovers;

} graph_t;

typedef vector<graph_t*> vg_t;

typedef struct _test {

	AnsiString FileName;    	// название файла теста

	unsigned int TypeParams;    // вариант измен€ющихс€ параметров

	unsigned int Count;         // количество тестов
	unsigned int ParamConst;	// неизмен€ющийс€ параметр

	v_t ParamFirst;    			// диапозон значений первого параметра
	v_t ParamSecond;    		// диапозон значений второго параметра

	v_t Alg;           			// тестируемые алгоритмы

	map<int,vvvd_t> MO; 		// MO характеристик алгоритмов поиска мин.покрыти€
	map<int,vvvd_t> CKO; 		// CKO характеристик алгоритмов поиска мин.покрыти€

	bool SuccefullMO;       	// признак что ћќ было найдены
	bool SuccefullCKO;      	// признак что — ќ было найдены

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


// структура св€зи индексов алгоритмов с названи€ми
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


// структура св€зи названи€ параметра алгоритма с названием
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
