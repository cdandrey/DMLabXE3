//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "TThreadCreateTest.h"
#include "TFormMain.h"

#pragma package(smart_init)
//---------------------------------------------------------------------------

//   Important: Methods and properties of objects in VCL can only be
//   used in a method called using Synchronize, for example:
//
//      Synchronize(&UpdateCaption);
//
//   where UpdateCaption could look like:
//
//      void __fastcall TThreadCreateTest::UpdateCaption()
//      {
//        Form1->Caption = "Updated in a thread";
//      }
//---------------------------------------------------------------------------

__fastcall TThreadCreateTest::TThreadCreateTest(bool CreateSuspended)
	: TThread(CreateSuspended)
{
}
//---------------------------------------------------------------------------


inline void __fastcall TThreadCreateTest::ToConsol(const AnsiString &Message)
{
	FormMain->ToConsol(Message);
	Application->ProcessMessages();
}
//------------------------------------------------------------------------------


inline void __fastcall TThreadCreateTest::Lock()
{
	FormMain->ActionsLock();
	FormMain->ThreadExecut = THR_GENERATE_TEST;
	Application->ProcessMessages();
}
//------------------------------------------------------------------------------


inline void __fastcall TThreadCreateTest::UnLock()
{
	FormMain->ActionsUnLock();
	FormMain->ThreadExecut = THR_NONE;
	Application->ProcessMessages();
}
//------------------------------------------------------------------------------


void __fastcall TThreadCreateTest::Execute()
{
	try {

		FreeOnTerminate = true;

		Synchronize(Lock);
		Synchronize(Create);

	} __finally {

		Synchronize(UnLock);
	}
}
//---------------------------------------------------------------------------


void __fastcall TThreadCreateTest::Create()
{
	try {

		ToConsol(MESSAGE_CLEAR);
		ToConsol("create-test " + FileName);

		test_t *Test = new test_t;

		Test->FileName = FileName;

		// ���������� ����� ��������� ����������
		v_t ValueN = LineToVec(RangeN);
		v_t ValueA = LineToVec(RangeA);
		v_t ValueT = LineToVec(RangeT);


		if ((ValueN.size() >= 1 && ValueA.size() >= 1 && ValueT.size() == 1) ||
			(ValueN.size() > 1 && ValueA.size() > 1 && ValueT.size() > 1) )
		{
			Test->ParamFirst = ValueN;
			Test->ParamSecond = ValueA;
			Test->ParamConst = ValueT.at(0);
			Test->TypeParams = CHANGE_N_A;
		} else if ((ValueN.size() >= 1 && ValueT.size() >= 1 && ValueA.size() == 1)) {
			Test->ParamFirst = ValueN;
			Test->ParamSecond = ValueT;
			Test->ParamConst = ValueA.at(0);
			Test->TypeParams = CHANGE_N_T;
		} else if (ValueA.size() > 1 && ValueT.size() > 1  && ValueN.size() == 1) {
			Test->ParamFirst = ValueA;
			Test->ParamSecond = ValueT;
			Test->ParamConst = ValueN.at(0);
			Test->TypeParams = CHANGE_A_T;
		} else {
			ToConsol("������! �������� ���� �� ������������ ����������� ������.");
			ToConsol("���� �� ������.");
			return;
		}

		Test->Count = TextToInt(CountStr);
		if (Test->Count < 1) {
			ToConsol("������! �� ������� ���������� ������.");
			ToConsol("���� �� ������.");
			return;
		}

		// ���������� ����� ��������� ���� �������������
		Test->Alg = Alg;
		if (Alg.size() < 1) {
			ToConsol("������! �� ������� ��������� ������� ���������� �����������.");
			ToConsol("���� �� ������.");
			return;
		}

		Test->SuccefullMO = false;
		Test->SuccefullCKO = false;

		FormMain->TestToList(Test);
		FormMain->ListBoxTestsClick(FormMain);

		Application->ProcessMessages();

		ToConsol("���� " + FileName + " - ������� ������.");

	} catch (...) {
		ToConsol("����������� ������! ���� �� ������.");
	}
}
//---------------------------------------------------------------------------


inline v_t __fastcall TThreadCreateTest::LineToVec(const AnsiString &Line)
{
	AnsiString Numbers = "0123456789";
	AnsiString StrNum = "";

	register int Pos = 1;
	register int Len = Line.Length();

	v_t RangeNums;
	s_t SetNums;

	char Ch;
	bool Range;

	while (Pos <= Len) {

		// �������� �����
		while (Pos <= Len) {

			Ch =  Line[Pos];

			if (Numbers.Pos(Ch) != 0)
				StrNum += Ch;
			else
				break;

			++Pos;
		}

		// ���������� ����������� �����
		Range = false;

		while (Pos <= Len) {

			Ch =  Line[Pos];

			if (Numbers.Pos(Ch) != 0)
				break;
			else
				if (Ch == '-')
					Range = true;

			++Pos;
		}

		// ��������� ��������� �����
		if (StrNum != "") {

			if ((Range) || (RangeNums.size() == 2))
				RangeNums.push_back(StrToInt(StrNum));
			else
				SetNums.insert(StrToInt(StrNum));

			StrNum = "";
		}
	}

	int NumBegin = 0;
	int NumEnd = -1;
	int NumStep = 5;

	switch (RangeNums.size()) {

	case 2: NumBegin = RangeNums.at(0);
			NumEnd = RangeNums.at(1);
			break;

	case 3: NumBegin = RangeNums.at(0);
			NumEnd = RangeNums.at(1);
			NumStep = RangeNums.at(2);
	}

	for (int Num = NumBegin; Num <= NumEnd; Num += NumStep)
		SetNums.insert(Num);

	v_t VecNums;
	for (s_t::iterator it = SetNums.begin(); it != SetNums.end(); ++it)
		VecNums.push_back(*it);

	return VecNums;
}
//---------------------------------------------------------------------------


inline  int __fastcall TThreadCreateTest::TextToInt(const AnsiString &Str)
{
	AnsiString Numbers = "0123456789";

	AnsiString Value;

	register int i = 1;

	while ((i <= Str.Length()) && (Numbers.Pos(Str.operator [](i)) != 0)) {

		Value += Str.operator [](i);
		++i;
	}

	if (Value == "")
		return 0;
	else
	    return StrToInt(Value);
}
//------------------------------------------------------------------------------

