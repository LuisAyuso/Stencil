#pragma once

#include <array>
#include <vector>
#include <cassert>

#include "tools.h"
#include "print.h"

namespace stencil{


	template <unsigned Dimensions>
	class Hyperspace : public utils::Printable{

		struct Scope {
			int a;
			int b;
			int da;
			int db;
		};

		std::array<Scope, Dimensions> scopes;
		unsigned step;

	public:

// ~~~~~~~~~~~~~~~~~~~~~~~ ORTODOX  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		Hyperspace( const Hyperspace<Dimensions>& o)
			: scopes(o.scopes), step(o.step)
		{
			for (auto i = 0; i < Dimensions; ++i){
				assert(scopes[i].a <= scopes[i].b);
				if (scopes[i].a == scopes[i].b){
					assert(scopes[i].da <=0 && scopes[i].db >= 0);
				}
			}
		}

		Hyperspace(Hyperspace<Dimensions>&& o)
			: scopes(std::move(o.scopes)), step(o.step)
		{
			for (auto i = 0; i < Dimensions; ++i){
				assert(scopes[i].a <= scopes[i].b);
				if (scopes[i].a == scopes[i].b){
					assert(scopes[i].da <=0 && scopes[i].db >= 0);
				}
			}
		}

		Hyperspace<Dimensions> operator=(const Hyperspace<Dimensions>& o){
			scopes = o.scopes;
			step = o.step;

			for (auto i = 0; i < Dimensions; ++i){
				assert(scopes[i].a <= scopes[i].b);
				if (scopes[i].a == scopes[i].b){
					assert(scopes[i].da <=0 && scopes[i].db >= 0);
				}
			}
		}
		Hyperspace<Dimensions> operator=(Hyperspace<Dimensions>&& o){
			std::swap(scopes, o.scopes);
			step = o.step;

			for (auto i = 0; i < Dimensions; ++i){
				assert(scopes[i].a <= scopes[i].b);
				if (scopes[i].a == scopes[i].b){
					assert(scopes[i].da <=0 && scopes[i].db >= 0);
				}
			}

		}

// ~~~~~~~~~~~~~~~~~~~~~~~ Spetialized ctors  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		Hyperspace( int xa, int xb, int dxa, int dxb, unsigned s=0)
				: scopes ({Scope{xa,xb,dxa,dxb}}), step(s){
			static_assert ( Dimensions == 1 , "this constructor is only allowed for 1D");
			for (auto i = 0; i < Dimensions; ++i){
				assert(scopes[i].a <= scopes[i].b);
				if (scopes[i].a == scopes[i].b){
					assert(scopes[i].da <=0 && scopes[i].db >= 0);
				}
			}
		}

		Hyperspace( int xa, int xb, int dxa, int dxb,
		            int ya, int yb, int dya, int dyb, unsigned s=0)
				: scopes ({Scope{xa,xb,dxa,dxb}, Scope{ya,yb,dya,dyb}}), step(s){
			static_assert ( Dimensions == 2 , "this constructor is only allowed for 2D");
			for (auto i = 0; i < Dimensions; ++i){
				assert(scopes[i].a <= scopes[i].b);
				if (scopes[i].a == scopes[i].b){
					assert(scopes[i].da <=0 && scopes[i].db >= 0);
				}
			}
		}

		Hyperspace( int xa, int xb, int dxa, int dxb,
		            int ya, int yb, int dya, int dyb,
		            int za, int zb, int dza, int dzb, unsigned s=0)
				: scopes ({Scope{xa,xb,dxa,dxb}, Scope{ya,yb,dya,dyb}, Scope{za,zb,dza,dzb}}), step(s){
			static_assert ( Dimensions == 2 , "this constructor is only allowed for 3D");
			for (auto i = 0; i < Dimensions; ++i){
				assert(scopes[i].a <= scopes[i].b);
				if (scopes[i].a == scopes[i].b){
					assert(scopes[i].da <=0 && scopes[i].db >= 0);
				}
			}
		}

		Hyperspace(std::array<int, Dimensions> a,
				   std::array<int, Dimensions> b,
				   std::array<int, Dimensions> da,
				   std::array<int, Dimensions> db, 
				   unsigned s=0) 
				: step(s){ 

			for (int i = 0; i< Dimensions; ++i)
				scopes[i] = {a[i], b[i], da[i], db[i]};

			for (auto i = 0; i < Dimensions; ++i){
				assert(scopes[i].a <= scopes[i].b);
				if (scopes[i].a == scopes[i].b){
					assert(scopes[i].da <=0 && scopes[i].db >= 0);
				}
			}
		}


// ~~~~~~~~~~~~~~~~~~~~~~~ Getters  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	std::array<int, Dimensions> getASlopes()const{
		std::array<int, Dimensions> res;
		for(int i =0; i< Dimensions; ++i){
			res[i] = scopes[i].da;
		}
		return res;
	}

	std::array<int, Dimensions> getBSlopes()const{
		std::array<int, Dimensions> res;
		for(int i =0; i< Dimensions; ++i){
			res[i] = scopes[i].db;
		}
		return res;
	}

	int a(int dimension) const{
		return scopes[dimension].a;
	}
	int b(int dimension) const{
		return scopes[dimension].b;
	}
	int da(int dimension) const{
		return scopes[dimension].da;
	}
	int db(int dimension) const{
		return scopes[dimension].db;
	}

// ~~~~~~~~~~~~~~~~~~~~~~~ Spliting tools ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		typedef std::vector<Hyperspace<Dimensions>> CutDim;

		/**
		 * 	Split dimension, one central triagle plus two inverted triangles afterwards
		 */
		static inline CutDim split_W(unsigned dimension, int split_value, const Hyperspace<Dimensions>& Hyp, int da, int db){

			CutDim res;

			res.push_back(Hyp);
			res[0].scopes[dimension].da = -1 * da;
			res[0].scopes[dimension].db = -1 * db;

			res.push_back(Hyp);
			res[1].scopes[dimension].b = res[1].scopes[dimension].a;
			res[1].step ++;

			res.push_back(Hyp);
			res[2].scopes[dimension].a = res[2].scopes[dimension].b;
			res[2].step ++;

			return res;
		}

		/**
		 * 	Split dimension, one two triangles, one inverted triangle afterwards
		 */
		static inline CutDim split_M(unsigned dimension, int split_value, const Hyperspace<Dimensions>& Hyp, int da, int db){

			CutDim res;

			res.push_back(Hyp);
			res[0].scopes[dimension].b = split_value;

			res.push_back(Hyp);
			res[1].scopes[dimension].a = split_value;

			res.push_back(Hyp);
			res[2].scopes[dimension].a = split_value;
			res[2].scopes[dimension].b = split_value;
			res[2].scopes[dimension].da = -1 * da;
			res[2].scopes[dimension].db = -1 * db;
			res[2].step ++;

			return res;
		}

		static inline CutDim split_1d(unsigned dimension, int split_value, const Hyperspace<Dimensions>& Hyp){

			if (split_value <= Hyp.scopes[dimension].a) return {Hyp};
			if (split_value >= Hyp.scopes[dimension].b) return {Hyp};

			// if the dimension pressents a shape in inverted V we split it in M
			if (Hyp.scopes[dimension].da >= 0 && Hyp.scopes[dimension].db <= 0) {
				return split_M(dimension, split_value,  Hyp, Hyp.da(dimension), Hyp.db(dimension));
			}

			// if the dimension pressents a shape in V we split it in W
			if (Hyp.scopes[dimension].da < 0 && Hyp.scopes[dimension].db > 0) {
				return split_W(dimension, split_value,  Hyp, Hyp.da(dimension), Hyp.db(dimension));
			}

			assert(false  && "this is not a valid geometry");
			return {};
		}

		template <unsigned Dim, typename ... Cuts>
		inline CutDim split(int cut) const{
			return split_1d(Dim, cut, *this);
		}

		template <unsigned Dim, typename ... Cuts>
		inline CutDim split(int cut, Cuts ... cuts) const{

			auto tmp = split<Dim+1>(cuts...);

			CutDim res;
			for (const auto& hyp : tmp){
				auto x = split_1d(Dim, cut, hyp);
				res.insert(res.end(), x.begin(), x.end());
			}

			return res;
		}
		
		template <typename ... Cuts>
		inline CutDim split(Cuts ... cuts) const{

			return split<0>(cuts...);
		}

// ~~~~~~~~~~~~~~~~~~~~~~~ print ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		std::ostream& printTo(std::ostream& out) const{

			out << "Hyp";
			for (const auto& s : scopes) {
				out << "[" << s.a << "," << s.b << "](" << s.da << "," << s.db << ")";
			}
			out << " p(" << step << ")";	
			return out;
		}
		
	};




} // namespace stencil
