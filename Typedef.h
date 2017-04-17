#ifndef Unit_TypedefH
#define Unit_TypedefH

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <cstdint>
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
#include <string>
#include <StrUtils.hpp>
#include <vector>


#define INFIN 999999  // бесконечность

// идентификаторы потоков
#define THR_NONE           0
#define THR_GENERATE_GRAPH 1
#define THR_GENERATE_TEST  2
#define THR_SEARCH_COVER   3
#define THR_TESTING        4

// тип вычисляемой характеристики теста
#define TEST_MO   21  // мат.ожидание
#define TEST_CKO  22  // среднеквадратическое отклонение
#define TEST_COMP 23  // сравнивать два выбранных метода

// вариант изменяющихся параметров тестируемых графов
#define CHANGE_N_A 31
#define CHANGE_N_T 32
#define CHANGE_A_T 33

// коды функций
#define NONE        100    // ни один процесс не выполняется
#define GEN_RAND    101    // генерация случайного графа
#define GEN_HAND    102    // генерация графа по заданному списку вершин
#define GEN_RE      103    // повторная генерация уже созданного графа
#define CREATE_TEST 104

// коды алгоритмов
#define RANG        200    // поиск вершинного покрытия ранговым методом
#define FULL        201
#define INDS        202
#define NIND        203
#define NINU        204
#define FREQ        205
#define VERT        206
#define EQUA        207
#define VPRE        208
#define VREC        209

// названия алгоритмов
//#define STR_RANG  "Ранговый метод"
//#define STR_FULL  "Быстрый полный перебор"
//#define STR_INDS  "Метод независимых множеств"
//#define STR_NIND  "Новый метод независимых множеств"
//#define STR_NINU  "Новый метод независимых множеств упрощенный"
//#define STR_FREQ  "Частотный метод"
//#define STR_VERT  "Метод вершин"
//#define STR_VPRE  "Метод вершин с прогнозом"
//#define STR_VREC  "Метод вершин с рекурсией"
//#define STR_EQUA  "Метод уравнений"

#define STR_RANG  "Rangovii metod"
#define STR_FULL  "Bistrii polnii perebor"
#define STR_INDS  "Metod nezavisimih mnojestv"
#define STR_NIND  "Novii metod nezavisimih mnojestv"
#define STR_NINU  "Novii metod nezavisimih mnojestv s odnim poglosheniem"
#define STR_FREQ  "Chastotnii metod"
#define STR_VERT  "Metod derevo putei odno NMNM"//"Metod vershin"
#define STR_VPRE  "Metod vershin s prognozom"
#define STR_VREC  "Metod vershin s recursiei"
#define STR_EQUA  "Metod derevo putei"

#define LIST_DIFF   200    // разница между id алгоритмов и их номеров в списке

// состояние приложения
#define MESSAGE_BEGIN             "  -- "
#define MESSAGE_CLEAR             "clear"
#define MESSAGE_INIT              ">>"
#define MESSAGE_GEN_GRAPH         "Создается граф...\n"
#define MESSAGE_OPEN_GRAPH        "Открывается граф...\n"
#define MESSAGE_SAVE_GRAPH        "Сохраняется граф...\n"
#define MESSAGE_SAVE_LGRAPH       "Сохраняются все графы в списке...\n"
#define MESSAGE_SEARCH_COVER_RANG "Идет поиск минимального вершинного покрытия ранговым методом...\n"

#define PARAM_COUNT   7
#define QO   0
#define TE   1
#define PE   2
#define RL 	 3
#define RE   4
#define FE 	 5
#define NS	 6

#define STR_QO "КОЛИЧЕСТВО ОПЕРАЦИЙ"
#define STR_TE "ВРЕМЯ ВЫПОЛНЕНИЯ"
#define STR_PE "ВЕРОЯТНОСТЬ ВЫПОЛНЕНИЯ"
#define STR_RL "ОТНОСИТЕЛЬНАЯ ДЛИНА ПОКРЫТИЯ"
#define STR_RE "ОТНОСИТЕЛЬНАЯ ОШИБКА"
#define STR_FE "ЧАСТОТА ПОЯВЛЕНИЯ ОШИБКИ"
#define STR_NS "КОЛИЧЕСТВО НЕЗАВИСИМЫХ МНОЖЕСТВ"

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
typedef set<uint8_t>   s8_t;
typedef set<s8_t>      ss8_t;
typedef vector<s_t>    vs_t;
typedef vector<vs_t>   vvs_t;

typedef multiset<int>  ms_t;
typedef vector<ms_t>   vms_t;

typedef pair<int,int>           p_t;
typedef vector<p_t>             vp_t;
typedef pair<s_t,s_t>           ps_t;
typedef vector<pair<s_t,s_t> >  vps_t;
typedef set<pair<s_t,s_t> >     sps_t;

typedef struct{
	s8_t x;
	s8_t y;
} path_t;

template <class T>
T next_it(T iter)
{
	T iter_next = iter;
	return ++iter_next;
}

template <class T>
T prev_it(T iter)
{
	T iter_next = iter;
	return --iter_next;
}

typedef void __fastcall (__closure *func_t)(void);

typedef struct _param_cover {

	AnsiString Q;        // количество операций затраченное алгоритмом
	AnsiString T;        // время в мс затраченое алгоритмом
	AnsiString Cover;    // номера вершин являющихся минимальным вершинным покрытием
	AnsiString LenCover; // длина минимального вершинного покрытия
	AnsiString LogShort; // все характеристики кроме лога
	AnsiString Log;      // отчет выполнения алгоритма

} param_cover_t;

// структура данных графа
typedef struct _graph {

	AnsiString FileName;

	int N;  // количество вершин
	int A;  // плотность
	int E;  // количество ребер

	// список ребер: индекс - номер ребар,
	// элемент - вершины которые соеденяет ребро
	vs_t Edges;

	// матрица смежности: индексы - номера вершин,
	// 1 - вершины соедиются ребром, 0 - нет
	vv_t Matrix;

	// список вершин: индекс - номер вершины
	// элемент - множество номеров вершин с которыми есть соединения
	vs_t Vertex;

	// список вершин графа дополнения, если вершина i в Veretx соединена с
	// вершиной j, то в VertexAdd нет и наоборот
	vs_t VertexAdd;

	// список вершинных покрытий найденных разными алгоритмами
	// ключ - номер алгоритма, элемент - структура данных вершинного покрытия
	map<int,param_cover_t> ParamCovers;

} graph_t;

typedef vector<graph_t*> vg_t;

typedef struct _test {

	AnsiString FileName;    	// название файла теста

	unsigned int TypeParams;    // вариант изменяющихся параметров

	unsigned int Count;         // количество тестов
	unsigned int ParamConst;	// неизменяющийся параметр

	v_t ParamFirst;    			// диапозон значений первого параметра
	v_t ParamSecond;    		// диапозон значений второго параметра

	v_t Alg;           			// тестируемые алгоритмы

	map<int,vvvd_t> MO; 		// MO характеристик алгоритмов поиска мин.покрытия
	map<int,vvvd_t> CKO; 		// CKO характеристик алгоритмов поиска мин.покрытия

	bool SuccefullMO;       	// признак что МО было найдены
	bool SuccefullCKO;      	// признак что СКО было найдены

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


// структура связи индексов алгоритмов с названиями
typedef struct _alg {

	map<AnsiString,int> Id;
	map<int,AnsiString> Name;

	_alg() {

		Id[STR_RANG] = RANG;
		Id[STR_FULL] = FULL;
		Id[STR_FREQ] = FREQ;
		Id[STR_INDS] = INDS;
		Id[STR_NIND] = NIND;
		Id[STR_NINU] = NINU;
		Id[STR_VERT] = VERT;
		Id[STR_EQUA] = EQUA;

		Name[RANG]   = STR_RANG;
		Name[FULL]   = STR_FULL;
		Name[FREQ]   = STR_FREQ;
		Name[INDS]   = STR_INDS;
		Name[NIND]   = STR_NIND;
		Name[NINU]   = STR_NINU;
		Name[VERT]   = STR_VERT;
		Name[EQUA]   = STR_EQUA;
	}
} alg_t;


// структура связи названия параметра алгоритма с названием
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
