//---------------------------------------------------------------------------

#pragma hdrstop

#include "TRandom.h"
//---------------------------------------------------------------------------

TRandom::TRandom()
{
	LARGE_INTEGER seed;
	QueryPerformanceCounter(&seed);
	ENG.seed((int)seed.QuadPart);
}
//---------------------------------------------------------------------------


void __fastcall TRandom::SetUniform(int Min,int Max)
{
	UNIFORM = tr1::uniform_int<int>(Min,Max);
}
//---------------------------------------------------------------------------


int __fastcall TRandom::Uniform()
{
	return UNIFORM(ENG);
}
//---------------------------------------------------------------------------


int __fastcall TRandom::UniformFromVectorErase(v_t& _Vector)
{
	tr1::uniform_int<int> uniform(0,_Vector.size() - 1);

	int Index = uniform(ENG);

	int Value = *(_Vector.begin() + Index);

	_Vector.erase(_Vector.begin() + Index);

	return Value;
}
//---------------------------------------------------------------------------


void __fastcall TRandom::SetNormal(double Mean,double Sigma)
{
	NORMAL = tr1::normal_distribution<double>(Mean,Sigma);
}
//---------------------------------------------------------------------------


double __fastcall TRandom::Normal()
{
	return NORMAL(ENG);
}
//---------------------------------------------------------------------------


int __fastcall TRandom::Normal(int Min,int Max)
{
	if (NORMAL.min() <= Min || NORMAL.max() >= Max)
		return Min;

	double Value;

	do
	{
		Value = NORMAL(ENG);
	}
	while((Value < Min)||(Value > Max));

	return Value;
}
//---------------------------------------------------------------------------


int __fastcall TRandom::NormalFromVectorErase(v_t& _Vector)
{
	double Mean = (double)(_Vector.size() - 1) / 2.0;

	if (Mean <= 0)
		return Mean;

	SetNormal(Mean,NORMAL.sigma());

	int Index = Normal(0,_Vector.size() - 1);

	int Value = *(_Vector.begin() + Index);

	_Vector.erase(_Vector.begin() + Index);

	return Value;
}
//---------------------------------------------------------------------------


void __fastcall TRandom::SetGamma(double Mean)
{
	// Mean = Alpha * Beta, по умолчанию Beta = 1.0 тогда Mean == Alpha
	// Sigma = Alpha * Beta^2 , по умолчанию  Beta = 1.0 тогда Sigma == Alpha
	GAMMA = tr1::gamma_distribution<double>(Mean);
}
//---------------------------------------------------------------------------


double __fastcall TRandom::Gamma()
{
	return GAMMA(ENG);
}
//---------------------------------------------------------------------------


int __fastcall TRandom::Gamma(int Min,int Max)
{
	if (GAMMA.min() <= Min || GAMMA.max() >= Max)
		return Min;

	double Value;

	do
	{
		Value = GAMMA(ENG);
	}
	while((Value < Min)||(Value > Max));

	return Value;
}
//---------------------------------------------------------------------------


int __fastcall TRandom::GammaFromVectorErase(v_t& _Vector)
{
	double Mean = (double)(_Vector.size() - 1) / 2.0;

	if (Mean <= 0)
		return Mean;

	SetGamma(Mean);

	int Index = Gamma(0,_Vector.size() - 1);

	int Value = *(_Vector.begin() + Index);

	_Vector.erase(_Vector.begin() + Index);

	return Value;
}
//---------------------------------------------------------------------------


void __fastcall TRandom::SetExpon(double Mean)
{
	// Lambda == 1 / Mean
	EXPON = tr1::exponential_distribution<double>(1.0 / Mean);
}
//---------------------------------------------------------------------------


double __fastcall TRandom::Expon()
{
	return EXPON(ENG);
}
//---------------------------------------------------------------------------


int __fastcall TRandom::Expon(int Min,int Max)
{
	if (EXPON.min() <= Min || EXPON.max() >= Max)
		return Min;

	double Value;

	do
	{
		Value = EXPON(ENG);
	}
	while((Value < Min)||(Value > Max));

	return Value;
}
//---------------------------------------------------------------------------


int __fastcall TRandom::ExponFromVectorErase(v_t& _Vector)
{
	double Mean = (double)(_Vector.size() - 1) / 2.0;

	if (Mean <= 0)
		return Mean;

	SetExpon(Mean);

	int Index = Expon(0,_Vector.size() - 1);

	int Value = *(_Vector.begin() + Index);

	_Vector.erase(_Vector.begin() + Index);

	return Value;
}
//---------------------------------------------------------------------------

#pragma package(smart_init)
