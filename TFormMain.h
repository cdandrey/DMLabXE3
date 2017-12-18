//---------------------------------------------------------------------------

#ifndef TFormMainH
#define TFormMainH
//---------------------------------------------------------------------------
#include "Typedef.h"
#include "TThreadSearchCover.h"
#include "TThreadCreateGraph.h"
#include "TThreadCreateTest.h"
#include "TThreadTestGraph.h"
#include "TFormNewGraph.h"
#include "TFormNewTest.h"
#include "TThreadCLQ.h"

#include <System.Actions.hpp>
#include <System.Classes.hpp>
#include <Vcl.ActnList.hpp>
#include <Vcl.CheckLst.hpp>
#include <Vcl.ComCtrls.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.Dialogs.hpp>
#include <Vcl.ExtCtrls.hpp>
#include <Vcl.ImgList.hpp>
#include <Vcl.Menus.hpp>
#include <Vcl.StdActns.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.ToolWin.hpp>
#include <Vcl.Grids.hpp>
#include <Vcl.ValEdit.hpp>
#include <System.Actions.hpp>
#include <System.ImageList.hpp>
#include <Vcl.CategoryButtons.hpp>

//---------------------------------------------------------------------------
class TFormMain : public TForm
{
__published:	// IDE-managed Components
	TPageControl *PageControlMain;
	TTabSheet *TabSheetGraph;
	TTabSheet *TabSheetTestGraph;
	TGroupBox *GroupBoxGraphs;
	TListBox *ListBoxGraphs;
	TGroupBox *GroupBoxAlg;
	TPanel *PanelResult;
	TPageControl *PageControlTestResult;
	TTabSheet *TabSheetTestResultList;
	TRichEdit *RichEditTestResults;
	TTabSheet *TabSheetTestResultChart;
	TRichEdit *RichEditGraph;
	TMainMenu *MainMenu;
	TMenuItem *File;
	TMenuItem *MenuOpen;
	TMenuItem *MenuSaveAs;
	TActionList *ActionList;
	TFileExit *ActionExit;
	TMenuItem *Exit;
	TMenuItem *SeparateReGenerate;
	TMenuItem *MenuNew;
	TMenuItem *Operations;
	TMenuItem *MenuRun;
	TMenuItem *SeparateStop;
	TMenuItem *MenuAlgStop;
	TSplitter *SplitterConsol;
	TMenuItem *MenulDel;
	TMenuItem *SeparateExit;
	TMenuItem *SeparateDel;
	TMenuItem *MenuGraphReGenerate;
	TToolBar *ToolBarMain;
	TToolButton *ToolButtonOpen;
	TToolButton *ToolButtonSave;
	TToolButton *ToolButtonNew;
	TToolButton *ToolButtonSeparate2;
	TToolButton *ToolButtonDel;
	TToolButton *ToolButtonRun;
	TToolButton *ToolButtonStop;
	TImageList *ImageList;
	TToolButton *ToolButtonReCreate;
	TToolButton *ToolButtonSeparate1;
	TStatusBar *StatusBar;
	TSplitter *SplitterGroupBoxGraph;
	TSplitter *SplitterListBoxGraph;
	TGroupBox *GroupBoxConsol;
	TMenuItem *MenuSaveList;
	TAction *ActionNew;
	TAction *ActionDel;
	TAction *ActionReCreate;
	TAction *ActionStop;
	TAction *ActionRun;
	TSaveDialog *SaveDialog;
	TAction *ActionSave;
	TOpenDialog *OpenDialog;
	TAction *ActionOpen;
	TBrowseForFolder *ActionSaveList;
	TListView *ListViewAlgMIS;
	TRichEdit *RichEditLog;
	TSplitter *SplitterListViewAlg;
	TToolButton *ToolButtonSeparate3;
	TToolButton *ToolButtonAlgLogView;
	TAction *ActionAlgLogView;
	TMenuItem *MenuReports;
	TMenuItem *MenuReports_AlgLogView;
	TMemo *Consol;
	TAction *ActionRunAll;
	TToolButton *ToolButtonRunAll;
	TMenuItem *MenuRunAll;
	TGroupBox *GroupBoxListTest;
	TSplitter *Splitter1;
	TListBox *ListBoxTests;
	TRichEdit *RichEditTest;
	TSplitter *Splitter2;
	TGroupBox *GroupBoxTestResults;
	TSplitter *Splitter3;
	TListView *ListViewTestType;
	TMenuItem *MenuAutoSaveTest;
	TToolButton *ToolButtonAlgLogWrite;
	TPageControl *PageControlAlg;
	TTabSheet *TabSheetMIS;
	TTabSheet *TabSheetCLQ;
	TListView *ListViewAlgCLQ;
	TAction *ActionAlgLogWrite;
	TMenuItem *MenuReports_AlgLogWrite;
	TMenuItem *N2;
	TMenuItem *MenuReports_GraphLogShort;
	TAction *ActionGraphLogShort;
	TTabSheet *TSH_CLR;
	TRadioGroup *RG_CLQ_MERGE;
	TRadioGroup *RG_CLQ_TREANGLS;
	TRadioGroup *RG_CLQ_SORT;
	TCheckBox *CHB_CLQ_LAST_CHECKED;
	TRadioGroup *RG_MIS_SPPA_PAIR;
	TListView *LV_ALG_CLR;

	void __fastcall ListBoxGraphsClick(TObject *Sender);

	void __fastcall ButtonNewTestClick(TObject *Sender);
	void __fastcall ButtonStopTestClick(TObject *Sender);
	void __fastcall ActionNewExecute(TObject *Sender);
	void __fastcall ActionDelExecute(TObject *Sender);
	void __fastcall ActionReCreateExecute(TObject *Sender);
	void __fastcall ActionStopExecute(TObject *Sender);
	void __fastcall ActionRunExecute(TObject *Sender);
	void __fastcall ActionSaveExecute(TObject *Sender);
	void __fastcall ActionOpenExecute(TObject *Sender);
	void __fastcall ActionSaveListAccept(TObject *Sender);
	void __fastcall ActionAlgLogViewExecute(TObject *Sender);
	void __fastcall ListViewAlgMISClick(TObject *Sender);
	void __fastcall ActionRunAllExecute(TObject *Sender);
	void __fastcall ListBoxTestsClick(TObject *Sender);
	void __fastcall ListViewTestTypeClick(TObject *Sender);
	void __fastcall ListViewAlgMISDblClick(TObject *Sender);
	void __fastcall ListViewAlgCLQDblClick(TObject *Sender);
	void __fastcall ToolButtonAlgLogWriteClick(TObject *Sender);
	void __fastcall ActionGraphLogShortExecute(TObject *Sender);
	void __fastcall ListViewAlgCLQClick(TObject *Sender);
	void __fastcall LV_ALG_CLRDblClick(TObject *Sender);

public:

	__fastcall TFormMain(TComponent* Owner);

	void ActionsLock();
	void ActionsUnLock();

	bool GraphFromFile(graph_t *Graph,const AnsiString &File);  // открывает граф из файла FileName
	bool GraphDIMACSFromFile(graph_t *Graph,const AnsiString &File);  // открывает граф из файла FileName
	bool GraphToFile(graph_t *Graph,const AnsiString &File);    // записывает созданный граф в файл FileName

	void GraphToList(graph_t *Graph);
	void GraphToRichEdit(graph_t *Graph);

	void ParamCoversToListView();
	void ParamCoversToRichEdit();

	void TestResultToRichEdit(unsigned int Index,unsigned int TypeResult);
	void TestToList(test_t *Test);
	void TestToRichEdit(test_t *Test);

	void ToConsol(const AnsiString &Message);

	vector<graph_t*> Graphs;
	vector<test_t*> Tests;

	int ThreadExecut;

	//	bool __fastcall ControlInputParam(TTestGraph* test);


	//void __fastcall ViewTestResults(TTestGraph *Test);

private:

	enum ExecutOperation {
		ForGraph = 0,
		ForTestGraph
	};

	TDateTime Time;

	TThreadSearchCover *ThrSearchCover;
	TThreadCreateGraph *ThrCreateGraph;

	TThreadCreateTest *ThrCreateTest;
	TThreadTestGraph  *ThrTestGraph;

    TThreadCLQ *ThrClique;

	alg_t 		*Alg;
	param_alg_t *ParamAlg;

};
//---------------------------------------------------------------------------
extern PACKAGE TFormMain *FormMain;
//---------------------------------------------------------------------------
#endif
