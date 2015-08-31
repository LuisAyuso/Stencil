
#pragma once



namespace {


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~ Meta programing tools ~~~~~~~~~~~~~~~~~~~~~~~~~~

	template <unsigned N, unsigned M>
	struct is_eq{
		const static bool value = (N==M);
	};


	template <unsigned N, unsigned M>
	struct is_ge{
		const static bool value = (N>=M);
	};


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~ Comparison tools ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	template <typename T>
	inline T MAX(T n){
		return n;
	}

	template <typename T, typename ... ARGS>
	inline T MAX(T n, ARGS ... args){
		auto m = MAX(args...);
		return n>m?n:m;
	}

	template <typename T>
	inline T MIN(T n){
		return n;
	}

	template <typename T, typename ... ARGS>
	inline T MIN(T n, ARGS ... args){
		auto m = MIN(args...);
		return n<m?n:m;
	}

	template <typename T>
	inline T ABS(T n){
		return n<0? -n: n;
	}

}


