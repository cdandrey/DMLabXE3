//---------------------------------------------------------------------------

#ifndef TRandomH
#define TRandomH

#include "Typedef.h"
#include <Classes.hpp>
#include <random>

//---------------------------------------------------------------------------

class TRandom
{
	private:

		tr1::mt19937 							ENG;

		tr1::uniform_int<int> 					UNIFORM;
		tr1::normal_distribution<double> 		NORMAL;
		tr1::gamma_distribution<double> 		GAMMA;
		tr1::exponential_distribution<double> 	EXPON;

	public:

		TRandom::TRandom();
		TRandom::~TRandom(){;}

		int  __fastcall Uniform();
		int  __fastcall UniformFromVectorErase(v_t& _Vector);
		void __fastcall SetUniform(int Min,int Max);

		double __fastcall Normal();
		int    __fastcall Normal(int Min,int Max);
		int    __fastcall NormalFromVectorErase(v_t& _Vector);
		void   __fastcall SetNormal(double Mean,double Sigma = 1.0);

		double __fastcall Gamma();
		int    __fastcall Gamma(int Min,int Max);
		int    __fastcall GammaFromVectorErase(v_t& _Vector);
		void   __fastcall SetGamma(double Mean);

		double __fastcall Expon();
		int    __fastcall Expon(int Min,int Max);
		int    __fastcall ExponFromVectorErase(v_t& _Vector);
		void   __fastcall SetExpon(double Mean);

};
#endif
