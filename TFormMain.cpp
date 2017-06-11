//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "TFormMain.h"
//---------------------------------------------------------------------------

#pragma package(smart_init)
#pragma resource "*.dfm"
TFormMain *FormMain;
//---------------------------------------------------------------------------


__fastcall TFormMain::TFormMain(TComponent* Owner)
	: TForm(Owner)
{
	ThreadExecut = THR_NONE;


	// codes and names of algorithm
	ListViewAlgMIS->Items->Item[0]->Indent = RANG;    // ранговый
	ListViewAlgMIS->Items->Item[1]->Indent = FULL;    // быстрый полный перебор
	ListViewAlgMIS->Items->Item[2]->Indent = INDS;    // независимых множеств
	ListViewAlgMIS->Items->Item[3]->Indent = NIND;    // новый метод независимых множеств
	ListViewAlgMIS->Items->Item[4]->Indent = NINU;    // новый метод независимых множеств упрощенный
	ListViewAlgMIS->Items->Item[5]->Indent = FREQ;    // частотный
	ListViewAlgMIS->Items->Item[6]->Indent = VERT;    // метод вершин
	ListViewAlgMIS->Items->Item[7]->Indent = EQUA;    // метод уравнений

	ListViewAlgMIS->Items->Item[0]->Caption = STR_RANG;    // ранговый
	ListViewAlgMIS->Items->Item[1]->Caption = STR_FULL;    // быстрый полный перебор
	ListViewAlgMIS->Items->Item[2]->Caption = STR_INDS;    // независимых множеств
	ListViewAlgMIS->Items->Item[3]->Caption = STR_NIND;    // новый метод независимых множеств
	ListViewAlgMIS->Items->Item[4]->Caption = STR_NINU;    // новый метод независимых множеств упрощенный
	ListViewAlgMIS->Items->Item[5]->Caption = STR_FREQ;    // частотный
	ListViewAlgMIS->Items->Item[6]->Caption = STR_VERT;    // метод вершин
	ListViewAlgMIS->Items->Item[7]->Caption = STR_EQUA;    // метод уравнений

	ListViewTestType->Items->Item[0]->Indent = TEST_MO; 	// математическое ожидание
	ListViewTestType->Items->Item[1]->Indent = TEST_CKO;	// среднеквадратическое отклонение
	ListViewTestType->Items->Item[2]->Indent = TEST_COMP;	// сравнение методов

	SaveDialog->Filter = AnsiString("Граф (*") + SUFIX_GRAPH +
						 AnsiString(")|*") + SUFIX_GRAPH +
						 AnsiString("|Тест (*") + SUFIX_TEST +
						 AnsiString(")|*") + SUFIX_TEST;

	OpenDialog->Filter = AnsiString("Граф (*") + SUFIX_GRAPH +
						 AnsiString(")|*") + SUFIX_GRAPH +
						 AnsiString("|Тест (*") + SUFIX_TEST +
						 AnsiString(")|*") + SUFIX_TEST;

	Alg = new alg_t();
	ParamAlg = new param_alg_t();
}
//---------------------------------------------------------------------------


void TFormMain::ActionsLock()
{
	// действия меню "Файл"
	ActionNew->Enabled      = false;
	ActionOpen->Enabled     = false;
	ActionSave->Enabled     = false;
	ActionSaveList->Enabled = false;
	ActionReCreate->Enabled = false;
	ActionDel->Enabled      = false;

	// действия меню "Операции"
	ActionRun->Enabled    = false;
	ActionRunAll->Enabled = false;

	// действия меню "Отчеты"
	ActionAlgLogView->Enabled = false;

	PageControlMain->Enabled = false;

	Consol->ReadOnly = true;
}
//---------------------------------------------------------------------------


void TFormMain::ActionsUnLock()
{
	// действия меню "Файл"
	ActionNew->Enabled        = true;
	ActionOpen->Enabled       = true;
	ActionSave->Enabled       = true;
	ActionSaveList->Enabled   = true;
	ActionReCreate->Enabled = true;
	ActionDel->Enabled        = true;

	// действия меню "Операции"
	ActionRun->Enabled        = true;
	ActionRunAll->Enabled     = true;

	// действия меню "Отчеты"
	ActionAlgLogView->Enabled = true;
//	ActionAutoSaveTest->Enabled = true;

	PageControlMain->Enabled = true;
	Consol->ReadOnly = true;
}
//---------------------------------------------------------------------------


void __fastcall TFormMain::ActionNewExecute(TObject *Sender)
{
	switch (PageControlMain->ActivePageIndex) {

	// действия относятся к графу
	case 0:

		FormNewGraph->ShowModal();

		if (FormNewGraph->ModalResult) {

			ThrCreateGraph = new TThreadCreateGraph(true);

			ThrCreateGraph->FileName  = FormNewGraph->EditName->Text;
			ThrCreateGraph->N         = FormNewGraph->N;
			ThrCreateGraph->A         = FormNewGraph->A;
			ThrCreateGraph->VertexStr = FormNewGraph->VertexStr;

			if (FormNewGraph->RadioButtonRand->Checked)
				ThrCreateGraph->FuncExecut = GEN_RAND;
			else if (FormNewGraph->RadioButtonHand->Checked)
				ThrCreateGraph->FuncExecut = GEN_HAND;

			ThrCreateGraph->Resume();
		}

		break;

	// действия относятся к тестам
	case 1:;
		FormNewTest->ShowModal();

		if (FormNewTest->ModalResult) {

			ThrCreateTest = new TThreadCreateTest(true);

			ThrCreateTest->FileName = FormNewTest->EditName->Text;

			ThrCreateTest->RangeN   = FormNewTest->EditRangeN->Text;
			ThrCreateTest->RangeA   = FormNewTest->EditRangeA->Text;
			ThrCreateTest->RangeT   = FormNewTest->EditRangeT->Text;

			ThrCreateTest->CountStr = FormNewTest->EditCount->Text;

			for (int i = 0; i < FormNewTest->ListViewAlg->Items->Count; ++i)
				if (FormNewTest->ListViewAlg->Items->Item[i]->Checked)
					ThrCreateTest->Alg.push_back(FormNewTest->ListViewAlg->Items->Item[i]->Indent);

			ThrCreateTest->Resume();
		}

		break;

	default:;
	}
}
//---------------------------------------------------------------------------


void __fastcall TFormMain::ActionOpenExecute(TObject *Sender)
{
	// открываем диалоговое окно
	if (!OpenDialog->Execute())
		return;

	Consol->Lines->Clear();

	register int i;

	for (i = 0; i < OpenDialog->Files->Count; ++i) {

		AnsiString File = OpenDialog->Files->operator [](i);

		// если тип открываемого файла является графом - открываем граф
		if (File.Pos(SUFIX_GRAPH) != 0){

			StatusBar->SimpleText = MESSAGE_OPEN_GRAPH;
			Consol->Lines->Append(AnsiString(MESSAGE_OPEN_GRAPH) + "  " + ExtractFileName(File));
			Application->ProcessMessages();

			graph_t *OpenGraph = new graph_t;

			if (GraphFromFile(OpenGraph,File)) {

				GraphToList(OpenGraph);
				GraphToRichEdit(OpenGraph);

				Consol->Lines->Append("Граф успешно открыт.\n");

			} else {

				Consol->Lines->Append("Произошла ошибка.");
				Consol->Lines->Append("Граф не был открыт.\n");
				delete OpenGraph;

			}    // end if GraphFromFile

		}    // end if SUFIX_GRAPH

	}    // end for Files->count

	StatusBar->SimpleText = MESSAGE_INIT;
}
//---------------------------------------------------------------------------


void __fastcall TFormMain::ActionSaveExecute(TObject *Sender)
{
	register int Index = -1;
	AnsiString FileSave = "";

	switch (PageControlMain->ActivePageIndex) {

	// действия относятся к графу
	case 0:

		Index = ListBoxGraphs->ItemIndex;

		if (Index < 0 || Index >= Graphs.size())
			return;

		SaveDialog->FileName = Graphs[Index]->FileName;
		SaveDialog->FilterIndex = 1;
		SaveDialog->DefaultExt = AnsiString("*") + SUFIX_GRAPH;

		StatusBar->SimpleText = MESSAGE_SAVE_GRAPH;

		Consol->Lines->Clear();
		Consol->Lines->Append(MESSAGE_SAVE_GRAPH);

		// открываем диалоговое окно
		if (!SaveDialog->Execute()) {
			Consol->Lines->Append("Действие было отмененно пользователем.");
			Consol->Lines->Append(Graphs[Index]->FileName + " - не сохранен.");
			break;
		}

		// сохраняем граф
		if (GraphToFile(Graphs[Index],SaveDialog->FileName)) {

			// если имя графа было измененно делаем эту замену в списке
			FileSave = ExtractFileName(SaveDialog->FileName);
			FileSave = ReplaceStr(FileSave,SUFIX_GRAPH,"");

			if (FileSave != Graphs[Index]->FileName) {

				Graphs[Index]->FileName = FileSave;
				ListBoxGraphs->Items[Index].Text = Graphs[Index]->FileName;
				GraphToRichEdit(Graphs[Index]);
			}

			Consol->Lines->Append(Graphs[Index]->FileName + " - успешно сохранен.");

		} else {
			Consol->Lines->Append("Произошла ошибка.");
			Consol->Lines->Append(Graphs[Index]->FileName + " - не сохранен.");
		}

		break;

	// действия относятся к тестам
	case 1:;

	default:;
	}

	StatusBar->SimpleText = MESSAGE_INIT;
}
//---------------------------------------------------------------------------


void __fastcall TFormMain::ActionSaveListAccept(TObject *Sender)
{
	// заключаем функцию в try - finally что бы обязательно выполнить UnLock()
	try {

		register int Index  = -1;
		AnsiString DirSave  = "";
		AnsiString FileSave = "";

		switch (PageControlMain->ActivePageIndex) {

		// действия относятся к графу
		case 0:

			Index = ListBoxGraphs->ItemIndex;

			DirSave = ActionSaveList->Folder + "\\";

			if (Graphs.size() == 0)
				return;

			if (DirSave == NULL)
				return;

			StatusBar->SimpleText = MESSAGE_SAVE_GRAPH;
			Consol->Lines->Clear();
			ActionsLock();

			for (Index = 0; Index < Graphs.size(); ++Index) {

				FileSave = DirSave + Graphs[Index]->FileName + SUFIX_GRAPH;

				Consol->Lines->Append(MESSAGE_SAVE_GRAPH);
				Application->ProcessMessages();

				// сохраняем граф
				if (GraphToFile(Graphs[Index],FileSave)) {

					Consol->Lines->Append(Graphs[Index]->FileName + " - успешно сохранен.");

				} else {

					Consol->Lines->Append("Произошла ошибка.");
					Consol->Lines->Append(Graphs[Index]->FileName + " - не сохранен.");

				}    // enf if GraphToFile

			}    // end for i < Graphs.size()

			break;

		// действия относятся к тестам
		case 1:;

		default:;

		}    // end switch ActivePage

	} __finally {

		StatusBar->SimpleText = MESSAGE_INIT;
		ActionsUnLock();
	}
}
//---------------------------------------------------------------------------


void __fastcall TFormMain::ActionDelExecute(TObject *Sender)
{
	int Index = -1;

	switch (PageControlMain->ActivePageIndex) {

	// действия относятся к графу
	case 0:

		Index = ListBoxGraphs->ItemIndex;

		if ((Index >= 0) && (Index < Graphs.size())) {

			delete Graphs[Index];
			Graphs.erase(Graphs.begin() + Index);

			ListBoxGraphs->Items->Delete(Index);

			RichEditGraph->Clear();
			RichEditLog->Clear();

			ActionAlgLogView->Checked = false;
			ParamCoversToListView();
			ParamCoversToRichEdit();
		}

		break;

	// действия относятся к тестам
	case 1:;

	default:;
	}
}
//---------------------------------------------------------------------------


void __fastcall TFormMain::ActionReCreateExecute(TObject *Sender)
{
	int index = -1;

	switch (PageControlMain->ActivePageIndex) {

	// действия относятся к графу
	case 0:

		index = ListBoxGraphs->ItemIndex;

		if ((index >= 0) && (index < Graphs.size())) {

			ThrCreateGraph             = new TThreadCreateGraph(true);
			ThrCreateGraph->FuncExecut = GEN_RE;
			ThrCreateGraph->FileName   = Graphs[index]->FileName;
			ThrCreateGraph->N          = Graphs[index]->N;
			ThrCreateGraph->A          = Graphs[index]->A;
			ThrCreateGraph->Resume();
		}

		break;

	// действия относятся к тестам
	case 1:;

	default:;
	}
}
//---------------------------------------------------------------------------


void __fastcall TFormMain::ActionStopExecute(TObject *Sender)
{
	switch (ThreadExecut) {

	case THR_NONE:
		ToConsol("Выполняемых процессов нет.");
		break;

	case THR_GENERATE_GRAPH:
		ThrCreateGraph->Terminate();
		break;

	case THR_SEARCH_COVER:
		ThrSearchCover->Terminate();
		break;

	default :
		ToConsol("Ошибка! Выполняется неизвестный процесс.");
	}
}
//---------------------------------------------------------------------------


void __fastcall TFormMain::ActionRunAllExecute(TObject *Sender)
{
	if (PageControlMain->ActivePageIndex == ForGraph) {

		// действия относятся к графу

		int GraphIndex = ListBoxGraphs->ItemIndex;

		if (GraphIndex < 0 || GraphIndex > ListBoxGraphs->Count) {
			ToConsol("Невозможно выполнить алгоритм - не выбран граф!");
			return;
		}

		v_t CheckedFunc;

		for (int i = 0; i < ListViewAlgMIS->Items->Count; ++i)
			if (ListViewAlgMIS->Items->Item[i]->Checked)
				CheckedFunc.push_back(ListViewAlgMIS->Items->Item[i]->Indent);

		if (CheckedFunc.size() > 0) {

			ThrSearchCover = new TThreadSearchCover(true);
			ThrSearchCover->ListFuncExecut = CheckedFunc;
			ThrSearchCover->GraphIndex     = GraphIndex;
			ThrSearchCover->FileName       = Graphs[GraphIndex]->FileName;
			ThrSearchCover->N              = Graphs[GraphIndex]->N;
			ThrSearchCover->Edges          = Graphs[GraphIndex]->Edges;
			ThrSearchCover->Vertex         = Graphs[GraphIndex]->Vertex;
			ThrSearchCover->VertexAdd      = Graphs[GraphIndex]->VertexAdd;

			ThrSearchCover->Resume();
		}
	} else if (PageControlMain->ActivePageIndex == ForTestGraph) {

		// действия относятся к тесту

		int TestIndex = ListBoxTests->ItemIndex;

		if (TestIndex < 0 || TestIndex > ListBoxTests->Count) {
			ToConsol("Невозможно выполнить тестирование - не выбран тест!");
			return;
		}

		ThrTestGraph                = new TThreadTestGraph(true);

		ThrTestGraph->TestIndex 	= TestIndex;

		ThrTestGraph->FileName 		= Tests.at(TestIndex)->FileName;
		ThrTestGraph->TypeParams 	= Tests.at(TestIndex)->TypeParams;
		ThrTestGraph->TypeTest = TEST_CKO;
		ThrTestGraph->Count 		= Tests.at(TestIndex)->Count;
		ThrTestGraph->ParamConst 	= Tests.at(TestIndex)->ParamConst;
		ThrTestGraph->ParamFirst 	= Tests.at(TestIndex)->ParamFirst;
		ThrTestGraph->ParamSecond 	= Tests.at(TestIndex)->ParamSecond;
		ThrTestGraph->Alg 			= Tests.at(TestIndex)->Alg;

		ThrTestGraph->Resume();
	}
}
//---------------------------------------------------------------------------


void __fastcall TFormMain::ActionRunExecute(TObject *Sender)
{
	if (PageControlMain->ActivePageIndex == 0) {

		// действия относятся к графу

		int GraphIndex = ListBoxGraphs->ItemIndex;

		if (GraphIndex < 0 || GraphIndex > ListBoxGraphs->Count) {
			ToConsol("Невозможно выполнить алгоритм - не выбран граф!");
			return;
		}

		// определяем какой алгоритм нужно выполнить и заносим его в вектор
		v_t CheckedFunc;
		for (int i = 0; i < ListViewAlgMIS->Items->Count; ++i)
			if (ListViewAlgMIS->Items->Item[i]->Checked) {
				CheckedFunc.push_back(ListViewAlgMIS->Items->Item[i]->Indent);
				break;
			}

		if (CheckedFunc.size() > 0) {

			ThrSearchCover                 = new TThreadSearchCover(true);
			ThrSearchCover->ListFuncExecut = CheckedFunc;
			ThrSearchCover->GraphIndex     = GraphIndex;
			ThrSearchCover->FileName       = Graphs[GraphIndex]->FileName;
			ThrSearchCover->N              = Graphs[GraphIndex]->N;
			ThrSearchCover->Edges          = Graphs[GraphIndex]->Edges;
			ThrSearchCover->Vertex         = Graphs[GraphIndex]->Vertex;
			ThrSearchCover->VertexAdd      = Graphs[GraphIndex]->VertexAdd;
			ThrSearchCover->WriteLog       = !ToolButtonAlgLogNonWrite->Down;

			ThrSearchCover->Resume();

		} else {
			ToConsol("Не выбран алгоритм!");
			return;
		}

	} else if (PageControlMain->ActivePageIndex == 1) {

		// действия относятся к тесту

        // определяем индек теста в списки и проверяем его корректность
		int TestIndex = ListBoxTests->ItemIndex;

		if (TestIndex < 0 || TestIndex > ListBoxTests->Count) {
			ToConsol("Невозможно выполнить тестирование - не выбран тест!");
			return;
		}

		// проверка выбран тип вычисляемых характеристик тестом
		if (!ListViewTestType->Selected) {
			ToConsol("Невыбран тип вычисляемой характеристики тестом! Тест не запущен.");
			return;
		}

		// в Ident  хранится тип вычисляемых характеристик тестом
		unsigned int TestType = ListViewTestType->Selected->Indent;

		// проверка наличия минимум двух алгоритмов для режима сравнения
		if ((TestType == TEST_COMP)&&(Tests.at(TestIndex)->Alg.size() == 1)) {
			ToConsol("В выбраном тесте исследуется только один алгоритм, сравнение выполнить не возможно!");
			return;
        }

		// создаем поток и запускаем его
		ThrTestGraph                = new TThreadTestGraph(true);

		ThrTestGraph->TestIndex 	= TestIndex;

		ThrTestGraph->FileName 		= Tests.at(TestIndex)->FileName;
		ThrTestGraph->TypeParams 	= Tests.at(TestIndex)->TypeParams;
		ThrTestGraph->TypeTest 		= TestType;
		ThrTestGraph->Count 		= Tests.at(TestIndex)->Count;
		ThrTestGraph->ParamConst 	= Tests.at(TestIndex)->ParamConst;
		ThrTestGraph->ParamFirst 	= Tests.at(TestIndex)->ParamFirst;
		ThrTestGraph->ParamSecond 	= Tests.at(TestIndex)->ParamSecond;
		ThrTestGraph->Alg 			= Tests.at(TestIndex)->Alg;

		ThrTestGraph->AutoSave      = MenuAutoSaveTest->Checked;

		ThrTestGraph->Resume();
	}
}
//---------------------------------------------------------------------------


void __fastcall TFormMain::ActionAlgLogViewExecute(TObject *Sender)
{
	RichEditLog->Lines->Clear();

	int GraphIndex = ListBoxGraphs->ItemIndex;

	if (GraphIndex < 0 || GraphIndex >= Graphs.size())
		return;

	if (ActionAlgLogView->Checked) {

		// в режиме отображения подробного отчета определяем алгоритм
		// который выделен и его отчет существует или если такого нет,
		// то определяем первый отмеченый алгоритм с отчетом и выводим его
		// иначе выводим короткие отчеты
		int AlgSelect = -1;

		if (ListViewAlgMIS->Selected
			&& ListViewAlgMIS->Selected->SubItems->Strings[0] != "") {

			AlgSelect = ListViewAlgMIS->Selected->Indent;

		} else {

			for (int i = 0; i < ListViewAlgMIS->Items->Count; ++i)
				if (ListViewAlgMIS->Items->Item[i]->Checked
					&& ListViewAlgMIS->Items->Item[i]->SubItems->Strings[0] != "")
			{
				AlgSelect = ListViewAlgMIS->Items->Item[i]->Indent;
				ListViewAlgMIS->Items->Item[i]->Selected = true;
				break;
			}
		}

		if (AlgSelect != -1) {
			RichEditLog->Lines->Append(Graphs[GraphIndex]->ParamCovers[AlgSelect].LogShort);
			RichEditLog->Lines->Append("\n");
			RichEditLog->Lines->Append(Graphs[GraphIndex]->ParamCovers[AlgSelect].Log);
		} else {
			ParamCoversToRichEdit();
		}

	} else {
		// в режиме вывода коротких отчетов просто выводим все существующие
		// короткие отчеты выбранных алгоритмов
		ParamCoversToRichEdit();
	}
}
//---------------------------------------------------------------------------

 /*
void __fastcall TFormMain::ViewTestResults(TTestGraph *Test)
{
	RichEditTestResults->Clear();

	AnsiString line = "Параметры теста:";
	RichEditTestResults->Lines->Add(line);

	line = "Вершины:" + EditNBEGIN->Text + " - "
		   + EditNEND->Text + " / " + EditNSTEP->Text;
	RichEditTestResults->Lines->Add(line);

	line = "Плотность:" + EditABEGIN->Text + " - "
		   + EditAEND->Text + " / " + EditASTEP->Text;
	RichEditTestResults->Lines->Add(line);

	line = "Количество тестов:" + EditCOUNTTEST->Text;
	RichEditTestResults->Lines->Add(line);

	line = "Вероятность выполнения, мс:" + EditACCEPTTIME->Text;
	RichEditTestResults->Lines->Add(line);

	line = "Результаты:";
	RichEditTestResults->Lines->Add(line);
	RichEditTestResults->Lines->Add("");
	int num = ListTests->ItemIndex;
	RichEditTestResults->Lines->Add(TESTS[num]->toString(TESTS[num]->MO));

	TDateTime dt = TDateTime::CurrentDateTime();
	AnsiString time = dt.DateTimeString();
	time = ReplaceStr(time,":",".");
	time = ReplaceStr(time," ","_");

	AnsiString file = "test_" + time + ".txt";

	ofstream out(file.c_str());

	for (int i = 0; i < RichEditTestResults->Lines->Count; ++i) {

		AnsiString line = RichEditTestResults->Lines->Strings[i];

		out << line.c_str() << endl;
	}
}     */
//-----------------------------------------------------------------------------


/*
bool __fastcall TFormMain::ControlInputParam(TTestGraph* test)
{

	try {
		if (!test->NBEGIN || !test->NEND || !test->NSTEP) {
			ShowMessage(
				"Ошибка задания диапозона изменения количества вершин! Все значения должны быть больше 0.");
			return false;
		}

		if (test->NBEGIN > test->NEND) {
			ShowMessage(
				"Ошибка задания диапозона изменения количества вершин! Конечное значени должно быть больше либо равно начального значения.");
			return false;
		}

		if (!test->ABEGIN || !test->AEND || !test->ASTEP) {
			ShowMessage(
				"Ошибка задания диапозона изменения плотности! Все значения должны быть больше 0.");
			return false;
		}

		if (test->ABEGIN > test->AEND) {
			ShowMessage(
				"Ошибка задания диапозона изменения плотности! Конечное значени должно быть больше либо равно начального значения.");
			return false;
		}

		if (!test->COUNTTEST) {
			ShowMessage(
				"Ошибка задания количества тестов! Значение должны быть больше 0.");
			return false;
		}

		if (!test->ACCEPTTIME) {
			ShowMessage(
				"Ошибка задания вероятного времени выполнения! Значение должны быть больше 0.");
			return false;
		}

		if (!(test->ALG.size() > 0 || test->ALG_EXACT != -1)) {
			ShowMessage("Не выбран ни один из алгоритмов для тестирования!");
			return false;
		}

		return true;
	}
	catch (...) {
		return false;
	}
}      */
// ---------------------------------------------------------------------------


void __fastcall TFormMain::ButtonNewTestClick(TObject *Sender)
{

 /*	try {

		if (PageControlMain->ActivePageIndex != 1)
			return;

		TESTS.push_back(new TTestGraph());

		TESTS[TESTS.size() - 1]->NBEGIN = StrToInt(EditNBEGIN->Text);
		TESTS[TESTS.size() - 1]->NEND = StrToInt(EditNEND->Text);
		TESTS[TESTS.size() - 1]->NSTEP = StrToInt(EditNSTEP->Text);

		TESTS[TESTS.size() - 1]->ABEGIN = StrToInt(EditABEGIN->Text);
		TESTS[TESTS.size() - 1]->AEND = StrToInt(EditAEND->Text);
		TESTS[TESTS.size() - 1]->ASTEP = StrToInt(EditASTEP->Text);

		TESTS[TESTS.size() - 1]->COUNTTEST = StrToInt(EditCOUNTTEST->Text);
		TESTS[TESTS.size() - 1]->ACCEPTTIME = StrToInt(EditACCEPTTIME->Text);

		TESTS[TESTS.size() - 1]->COMPARE = CheckBoxCompare->Checked;

		algorithms alg;
		int alg_exact = -1;
		for (int i = 0; i < CheckListBoxAlg->Items->Count; i++) {

			if (CheckListBoxAlg->Checked[i]) {

				int alg_check = alg.id[CheckListBoxAlg->Items->Strings[i]];

				if ((alg_check == RANG) || (alg_check == FULL && alg_exact != RANG))
					alg_exact = alg_check;
				else
					TESTS[TESTS.size() - 1]->ALG.push_back(alg_check);
			}
		}

		TESTS[TESTS.size() - 1]->ALG_EXACT = alg_exact;

		if (!ControlInputParam(TESTS[TESTS.size() - 1]))
			return;

		ListTests->AddItem("Тест-" + IntToStr((int)TESTS.size()), Sender);

		ListTests->ItemIndex = TESTS.size() - 1;

		return;
	}
	catch (...) {
		return;
	}  */
}
// ---------------------------------------------------------------------------


void __fastcall TFormMain::ButtonStopTestClick(TObject *Sender)
{
/*	TESTS[ListTests->ItemIndex]->ThreadRunMO->Terminate();
//	TESTS[ListTests->ItemIndex]->ThreadRunMO->Graph->ThreadTRang->Handle;
//	TerminateThread((void *)TESTS[ListTests->ItemIndex]->ThreadRunMO->Graph->ThreadTRang->Handle,0);

	AnsiString Str = AnsiString("Получена команда прерывания. Тестирование закончится") +
					 AnsiString("после выполнения текущего алгоритма...");

	ListTestProgress->Items->Add(Str);

	Application->ProcessMessages();*/
}
//---------------------------------------------------------------------------


bool TFormMain::GraphFromFile(graph_t *Graph,const AnsiString &File)
{
	ifstream in(File.c_str(),ifstream::in);

	try {

		if (in.is_open()) {

			Graph->FileName = ExtractFileName(File);
			Graph->FileName = ReplaceStr(Graph->FileName,SUFIX_GRAPH,"");

			in >> Graph->N;

			Graph->Vertex = vector<set<int> >(Graph->N + 1,set<int>());

			register int i = 0;
			set<int> SetNums;
			for (i = 1; i <= Graph->N; ++i)
				SetNums.insert(i);

			Graph->VertexAdd = vector<set<int> >(Graph->N + 1,SetNums);
			Graph->VertexAdd[0].clear();
			Graph->VertexAdd.reserve(Graph->N + 1);

			Graph->Matrix = vector<vector<int> >(Graph->N + 1,vector<int>(Graph->N + 1,0));

			i = 1;
			bool Succefull = true;
			register int Len = 0;

			while ((i <= Graph->N) && (!in.eof())) {

				Graph->VertexAdd[i].erase(i);

				in >> Len;

				register int j = 0;
				register int Val = 0;

				while ((j < Len) && (!in.eof())) {

					in >> Val;

					if ((Graph->Matrix[i][Val] == 0)
						&& (Val != i))
					{
						Graph->Edges.push_back(set<int>());
						Graph->Edges[Graph->Edges.size() - 1].insert(i);
						Graph->Edges[Graph->Edges.size() - 1].insert(Val);

						Graph->Vertex[i].insert(Val);
						Graph->Vertex[Val].insert(i);

						Graph->VertexAdd[i].erase(Val);
						Graph->VertexAdd[Val].erase(i);

						Graph->Matrix[i][Val] = 1;
						Graph->Matrix[Val][i] = 1;
					}

					++j;
				}

				if (j != Len)
					Succefull = false;

				++i;
			}

			if (i != Graph->N + 1)
				Succefull = false;

			Graph->E = Graph->Edges.size();
			Graph->A = (double) (200 * Graph->E) / (Graph->N * (Graph->N - 1));

			in.close();
			return Succefull;

		} else {
			return false;
		}

	} catch(...) {

		in.close();
		return false;
    }
}
//---------------------------------------------------------------------------


bool TFormMain::GraphToFile(graph_t *Graph,const AnsiString &file)
{
	ofstream out;

	try {

		out.open(file.c_str());

		if (out.is_open()) {

			out << Graph->N << endl;

			// нулевая строка пустая, ее не сохраняем
			for (register int i = 1; i <= Graph->N; ++i) {

				out << Graph->Vertex[i].size() << endl;

				for (s_t::iterator it = Graph->Vertex[i].begin();
					 it != Graph->Vertex[i].end(); ++it)
					out << *it << " ";

				out << endl;
			}

			out.close();
			return true;

		} else {

			return false;
		}

	}catch(...) {

		out.close();
		return false;
	}
}
//---------------------------------------------------------------------------


void TFormMain::GraphToList(graph_t *Graph)
{
	Graphs.push_back(Graph);

	ListBoxGraphs->AddItem(Graphs[Graphs.size() - 1]->FileName,this);
	ListBoxGraphs->ItemIndex = Graphs.size() - 1;
}
//---------------------------------------------------------------------------


void TFormMain::GraphToRichEdit(graph_t *Graph) {

	// очищаем поле вывода
	RichEditGraph->Clear();

	if (Graph->N > 0 && Graph->A > 1 && Graph->A <= 100 && Graph->E > 0) {

		AnsiString Str = "ГРАФ : " + Graph->FileName + "\n\n";

		Str += "  вершин\t- "    + IntToStr(Graph->N) + "\n";
		Str += "  ребер\t\t- "   + IntToStr(Graph->E) + "\n";
		Str += "  плотность\t- " + IntToStr(Graph->A);

		// отображаем десятичную матрицу смежности в виде таблицы
		Str += "\n\nСПИСОК ВЕРШИН:\n\n";

		for (int i = 1; i < Graph->Vertex.size(); i++) {

			Str += IntToStr(i) + "  (";
			Str += IntToStr((int)Graph->Vertex[i].size()) + ")    :    ";

			for (s_t::const_iterator it = Graph->Vertex[i].begin();
				 it != Graph->Vertex[i].end(); ++it)
				Str += (IntToStr(*it) + "  ");

			Str += "\n";
		}

		// отображаем граф дополнение
		Str += "\n\nГРАФ ДОПОЛНЕНИЕ:\n\n";

		for (int i = 1; i < Graph->VertexAdd.size(); i++) {

			Str += IntToStr(i) + "  (";
			Str += IntToStr((int)Graph->VertexAdd[i].size()) + ")    :    ";

			for (s_t::const_iterator it = Graph->VertexAdd[i].begin();
				 it != Graph->VertexAdd[i].end(); ++it)
				Str += (IntToStr(*it) + "  ");

			Str += "\n";
		}

		// отображаем список всех ребер
		Str += "\n\nСПИСОК РЕБЕР (";

		Str += (IntToStr((int)Graph->Edges.size()) + "):\n\n");

		for (int i = 0; i < Graph->Edges.size(); ++i) {

			Str += IntToStr(i + 1);
			Str += "  -  (";
			Str += IntToStr(*Graph->Edges[i].begin());
			Str += "  v  ";
			Str += IntToStr(*(++Graph->Edges[i].begin()));
			Str += ")\n";
		}

		// отображаем двоичную матрицу смежности
		Str += "\n\nМАТРИЦА СМЕЖНОСТИ:\n\nX\t";

		for (int i = 1; i < Graph->Matrix.size(); ++i)
			Str += (IntToStr(i) + "\t");

		Str += "\n";

		for (int i = 1; i < Graph->Matrix.size(); ++i) {

			Str += (IntToStr(i) + "\t");

			for (int j = 1; j < Graph->Matrix[i].size(); ++j) {

				if (i != j)
					Str += (IntToStr(Graph->Matrix[i][j]) + "\t");
				else
					Str += "X\t";
			}

			Str += "\n";
		}

		RichEditGraph->Lines->Add(Str);
	}
}
//---------------------------------------------------------------------------


void __fastcall TFormMain::ListBoxGraphsClick(TObject *Sender)
{
	int GraphIndex = ListBoxGraphs->ItemIndex;

	if ((GraphIndex >= 0) && (GraphIndex < Graphs.size())) {

		// отображаем граф выделенный в списке
		GraphToRichEdit(Graphs[GraphIndex]);

		ParamCoversToListView();
		ActionAlgLogViewExecute(this);
	}
}
// ---------------------------------------------------------------------------


void __fastcall TFormMain::ListBoxTestsClick(TObject *Sender)
{
	int TestIndex = ListBoxTests->ItemIndex;

	if ((TestIndex >= 0) && (TestIndex < Tests.size())) {

		// отображаем граф выделенный в списке
		TestToRichEdit(Tests.at(TestIndex));

		RichEditTestResults->Lines->Clear();

		if (Tests.at(TestIndex)->SuccefullMO)
			TestResultToRichEdit(TestIndex,TEST_MO);
		else if  (Tests.at(TestIndex)->SuccefullCKO)
            TestResultToRichEdit(TestIndex,TEST_CKO);
	}
}
//---------------------------------------------------------------------------


void __fastcall TFormMain::ListViewAlgMISClick(TObject *Sender)
{
	unsigned int GraphIndex = ListBoxGraphs->ItemIndex;

	if (GraphIndex < 0 || GraphIndex >= Graphs.size())
		return;

	if (ActionAlgLogView->Checked) {

		// выводим подробный отчет выбранного алгоритма

		if (ListViewAlgMIS->Selected) {

			int Alg = ListViewAlgMIS->Selected->Indent;
			RichEditLog->Lines->Clear();
			RichEditLog->Lines->Append(Graphs[GraphIndex]->ParamCovers[Alg].LogShort);
			RichEditLog->Lines->Append("\n");
			RichEditLog->Lines->Append(Graphs[GraphIndex]->ParamCovers[Alg].Log);
		}

	} else {
		// выводим краткие отчеты всех алгоритмов
		ParamCoversToRichEdit();
	}
}
//---------------------------------------------------------------------------


void __fastcall TFormMain::ListViewAlgMISDblClick(TObject *Sender)
{
	int GraphIndex = ListBoxGraphs->ItemIndex;

	if (GraphIndex < 0 || GraphIndex > ListBoxGraphs->Count) {
		ToConsol("Невозможно выполнить алгоритм - не выбран граф!");
		return;
	}

	// определяем какой алгоритм нужно выполнить и заносим его в вектор
	v_t CheckedFunc;
	if (ListViewAlgMIS->Selected)
		CheckedFunc.push_back(ListViewAlgMIS->Selected->Indent);

	if (CheckedFunc.size() > 0) {

		ThrSearchCover                 = new TThreadSearchCover(true);
		ThrSearchCover->ListFuncExecut = CheckedFunc;
		ThrSearchCover->GraphIndex     = GraphIndex;
		ThrSearchCover->FileName       = Graphs[GraphIndex]->FileName;
		ThrSearchCover->N              = Graphs[GraphIndex]->N;
		ThrSearchCover->Edges          = Graphs[GraphIndex]->Edges;
		ThrSearchCover->Vertex         = Graphs[GraphIndex]->Vertex;
		ThrSearchCover->VertexAdd      = Graphs[GraphIndex]->VertexAdd;

		ThrSearchCover->Resume();

	}
}
//---------------------------------------------------------------------------


void __fastcall TFormMain::ListViewTestTypeClick(TObject *Sender)
{
	unsigned int TestIndex = ListBoxTests->ItemIndex;

	if (TestIndex < 0 || TestIndex >= Tests.size())
		return;

	if (ListViewTestType->Selected)
		TestResultToRichEdit(TestIndex,ListViewTestType->Selected->Indent);
}
//---------------------------------------------------------------------------


void TFormMain::ParamCoversToListView()
{
	// очищаем список
	for (register int i = 0; i < ListViewAlgMIS->Items->Count; ++i) {

		ListViewAlgMIS->Items->Item[i]->SubItems->Strings[0] = "";
		ListViewAlgMIS->Items->Item[i]->SubItems->Strings[1] = "";
		ListViewAlgMIS->Items->Item[i]->SubItems->Strings[2] = "";
		ListViewAlgMIS->Items->Item[i]->SubItems->Strings[3] = "";
	}

	// проверяем сущесвтует ли выделенный граф
	unsigned int GraphIndex = ListBoxGraphs->ItemIndex;

	if (GraphIndex < 0 || GraphIndex >= Graphs.size())
		return;

	// выводим данные по алгоритмам
	for (register int i = 0; i < ListViewAlgMIS->Items->Count; ++i) {

		int AlgId = ListViewAlgMIS->Items->Item[i]->Indent;  // в Indent хранится id алгоритма

		if (Graphs[GraphIndex]->ParamCovers.find(AlgId)
			!= Graphs[GraphIndex]->ParamCovers.end())
		{
			AnsiString Q = Graphs[GraphIndex]->ParamCovers[AlgId].Q;
			AnsiString T = Graphs[GraphIndex]->ParamCovers[AlgId].T;
			AnsiString LenCover = Graphs[GraphIndex]->ParamCovers[AlgId].LenCover;
			AnsiString Cover = Graphs[GraphIndex]->ParamCovers[AlgId].Cover;

			ListViewAlgMIS->Items->Item[i]->SubItems->Strings[0] = Q;
			ListViewAlgMIS->Items->Item[i]->SubItems->Strings[1] = T;
			ListViewAlgMIS->Items->Item[i]->SubItems->Strings[2] = LenCover;
			ListViewAlgMIS->Items->Item[i]->SubItems->Strings[3] = Cover;
		}
	}
}
//---------------------------------------------------------------------------


void TFormMain::ParamCoversToRichEdit()
{
	RichEditLog->Lines->Clear();

	unsigned int GraphIndex = ListBoxGraphs->ItemIndex;

	if (GraphIndex < 0 || GraphIndex >= Graphs.size())
		return;

	for (register int i = 0; i < ListViewAlgMIS->Items->Count; ++i) {

		int AlgId = -1;

		// отображаем только выбранные отчеты
		if (ListViewAlgMIS->Items->Item[i]->Checked) {
			AlgId = ListViewAlgMIS->Items->Item[i]->Indent; // в Indent хранится id алгоритма

			// проверяем существует ли отчет
			if (ListViewAlgMIS->Items->Item[i]->SubItems->Strings[0] != "") {

				RichEditLog->Lines->Append(Graphs[GraphIndex]->ParamCovers[AlgId].LogShort);
				RichEditLog->Lines->Append("\n");
			}
		}
	}    // end for i < ListViewAlgMIS
}
//---------------------------------------------------------------------------


void TFormMain::TestResultToRichEdit(unsigned int Index,unsigned int TypeResult)
{
	if (Index < 0 || Index > Tests.size())
		return;

	map<int,vvvd_t> Result;

	switch (TypeResult) {

	case TEST_MO: if (!Tests.at(Index)->SuccefullMO)
					  return;
				  Result = Tests.at(Index)->MO;
				  break;

	case TEST_CKO: if (!Tests.at(Index)->SuccefullCKO)
					  return;
				   Result = Tests.at(Index)->CKO;
				   break;

	case TEST_COMP: return;

	default: ToConsol("Неизвестный тип характеристик алгоритмов.");
			 return;
	}

	RichEditTestResults->Clear();

	AnsiString Lines = "";
	AnsiString Str = "";

	for (unsigned int Param = 0; Param < PARAM_COUNT; ++Param) {

		Lines += ParamAlg->Name[Param] + ": \n\n";

		int AlgNum;

		for (unsigned int i = 0; i < Tests.at(Index)->Alg.size(); ++i) {

			AlgNum = Tests.at(Index)->Alg.at(i);

			Lines += Alg->Name[AlgNum] + "\n";

			for (unsigned int s = 0; s < Tests.at(Index)->ParamSecond.size(); ++s) {

				Lines += Str.sprintf("%u\t",Tests.at(Index)->ParamSecond.at(s));

				for (unsigned int f = 0; f < Tests.at(Index)->ParamFirst.size(); ++f)
						Lines += Str.sprintf("%.4f\t",
								Result[AlgNum][Param][f][s]);

				Lines += "\n";
			}

			Lines += "\n\n";
		}
	}

	RichEditTestResults->Lines->AddObject(Lines,this);
}
//---------------------------------------------------------------------------


void TFormMain::TestToList(test_t *Test)
{
	Tests.push_back(Test);

	ListBoxTests->AddItem(Tests[Tests.size() - 1]->FileName,this);
	ListBoxTests->ItemIndex = Tests.size() - 1;
}
//---------------------------------------------------------------------------



void TFormMain::TestToRichEdit(test_t *Test)
{
	// очищаем поле вывода
	RichEditTest->Clear();

	AnsiString Str = "ТЕСТ : " + Test->FileName + "\n\n";

	// отображаем параметры размерностей тестируемых графов

	v_t ValueN;
	v_t ValueA;
	v_t ValueT;

	switch (Test->TypeParams) {

	case CHANGE_N_A:

			ValueN = Test->ParamFirst;
			ValueA = Test->ParamSecond;
			ValueT.push_back(Test->ParamConst);
			break;

	case CHANGE_N_T:

			ValueN = Test->ParamFirst;
			ValueT = Test->ParamSecond;
			ValueA.push_back(Test->ParamConst);
			break;

	case CHANGE_A_T:

			ValueA = Test->ParamFirst;
			ValueT = Test->ParamSecond;
			ValueN.push_back(Test->ParamConst);
	}

	//
	Str += "  -- значения вершин тестируемых графов (";
	Str += IntToStr((int)ValueN.size()) + ") :\n\t";

	for (unsigned int i = 0; i < ValueN.size(); ++i)
		Str += (IntToStr(ValueN.at(i)) + "  ");

	Str += "\n\n";

	//
	Str += "  -- значения плотностей тестируемых графов (";
	Str += IntToStr((int)ValueA.size()) + ") :\n\t";

	for (unsigned int i = 0; i < ValueA.size(); ++i)
		Str += (IntToStr(ValueA.at(i)) + "  ");

	Str += "\n\n";

	//
	Str += "  -- значения допустимого времени выполнения алгоритмов (";
	Str += IntToStr((int)ValueT.size()) + ") : \n\t";

	for (unsigned int i = 0; i < ValueT.size(); ++i)
		Str += (IntToStr(ValueT.at(i)) + "  ");

	Str += "\n\n";

	//
	Str += "  -- количество тестов:\n\t" + IntToStr((int)Test->Count) + "\n\n";

	//
	Str += "  -- исследуемые алгоритмы:\n";

	map<int,AnsiString> NameAlg = alg_t().Name;

	for (unsigned int i = 0; i < Test->Alg.size(); ++i)
		Str += "\t-- " + NameAlg[Test->Alg.at(i)] + "\n";

	RichEditTest->Lines->AddObject(Str,this);
}
//------------------------------------------------------------------------------


void TFormMain::ToConsol(const AnsiString &Message)
{
	if (Message == MESSAGE_CLEAR) {
		Consol->Lines->Clear();
		return;
	}

	Consol->Lines->Append(MESSAGE_BEGIN + Message);

	int LastPos = Consol->Text.Length();
	Consol->Perform(EM_SETSEL, LastPos, LastPos);
	//Consol->Perform(EM_SCROLLCARET, 0, 0);
	Consol->SetFocus();
}
//---------------------------------------------------------------------------


void __fastcall TFormMain::ListViewAlgCLQDblClick(TObject *Sender)
{
	int GraphIndex = ListBoxGraphs->ItemIndex;

	if (GraphIndex < 0 || GraphIndex > ListBoxGraphs->Count) {
		ToConsol("Невозможно выполнить алгоритм - не выбран граф!");
		return;
	}

	// определяем какой алгоритм нужно выполнить и заносим его в вектор
	v_t CheckedFunc;
	if (ListViewAlgCLQ->Selected)
		CheckedFunc.push_back(ListViewAlgCLQ->Selected->Index);

	if (CheckedFunc.size() > 0) {

		ThrSearchCover                 = new TThreadSearchCover(true);
		ThrSearchCover->ListFuncExecut = CheckedFunc;
		ThrSearchCover->GraphIndex     = GraphIndex;
		ThrSearchCover->FileName       = Graphs[GraphIndex]->FileName;
		ThrSearchCover->N              = Graphs[GraphIndex]->N;
		ThrSearchCover->Edges          = Graphs[GraphIndex]->Edges;
		ThrSearchCover->Vertex         = Graphs[GraphIndex]->Vertex;
		ThrSearchCover->VertexAdd      = Graphs[GraphIndex]->VertexAdd;

		ThrSearchCover->Resume();
	}
}
//---------------------------------------------------------------------------

