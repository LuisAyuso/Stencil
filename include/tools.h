
#pragma once


#define MAX(n,m) ((n>m)?n:m)
#define MIN(n,m) ((n<m)?n:m)
#define ABS(n) ((n<0)?-n:n)



	namespace {
		template <unsigned N, unsigned M>
		struct is_eq{
			const static bool value = (N==M);
		};
	}


	namespace {
		template <unsigned N, unsigned M>
		struct is_ge{
			const static bool value = (N>=M);
		};
	}


