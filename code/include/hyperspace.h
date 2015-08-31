#pragma once

#include <array>
#include <vector>
#include <cassert>

#include "tools.h"
#include "print.h"

namespace stencil{

	struct CutWithSlopes{
		int split_value;
		int da;
		int db;
	};


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

		static const unsigned dimensions = Dimensions;

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

//		Hyperspace(Hyperspace<Dimensions>&& o)
//			: scopes(std::move(o.scopes)), step(o.step)
//		{
//			for (auto i = 0; i < Dimensions; ++i){
//				assert(scopes[i].a <= scopes[i].b);
//				if (scopes[i].a == scopes[i].b){
//					assert(scopes[i].da <=0 && scopes[i].db >= 0);
//				}
//			}
//		}

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
//		Hyperspace<Dimensions> operator=(Hyperspace<Dimensions>&& o){
//			std::swap(scopes, o.scopes);
//			step = o.step;
//
//			for (auto i = 0; i < Dimensions; ++i){
//				assert(scopes[i].a <= scopes[i].b);
//				if (scopes[i].a == scopes[i].b){
//					assert(scopes[i].da <=0 && scopes[i].db >= 0);
//				}
//			}
//
//		}

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

	int& a(int dimension){
		return scopes[dimension].a;
	}
	int& b(int dimension){
	     return scopes[dimension].b;
	}
	int& da(int dimension){
	     return scopes[dimension].da;
	}
	int& db(int dimension){
		return scopes[dimension].db;
	}

	int getStep() const{
		return step;
	}
	int& getStep(){
		return step;
	}

// ~~~~~~~~~~~~~~~~~~~~~~~ Spliting tools ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		typedef std::array<Hyperspace<Dimensions>,3> CutDim_exact;

		/**
		 * 	Split dimension, one central triagle plus two inverted triangles afterwards
		 */
		template <unsigned Dim>
		static inline CutDim_exact split_W(const Hyperspace<Dimensions>& hyp, int da, int db){


			//std::cout << "W " << split_value << "  " << da << ":" << db << std::endl;
			//std::cout << hyp << std::endl;

			auto central = hyp;
			central.scopes[Dim].da = da;
			central.scopes[Dim].db = db;

			auto left = hyp;
			left.scopes[Dim].b = left.scopes[Dim].a;
			left.step ++;
	
			auto right = hyp;
			right.scopes[Dim].a = right.scopes[Dim].b;
			right.step ++;

			return {central, left, right};
		}

		/**
		 * 	Split dimension, one two triangles, one inverted triangle afterwards
		 */
		template <unsigned Dim>
		static inline CutDim_exact split_M(int split_value, const Hyperspace<Dimensions>& hyp, int da, int db){

			//std::cout << "M " << split_value << "  " << da << ":" << db << std::endl;

			auto left = hyp;
			left.scopes[Dim].b = split_value;
			left.scopes[Dim].db = db;

			auto right = hyp;
			right.scopes[Dim].a = split_value;
			right.scopes[Dim].da = da;

			auto central = hyp;
			central.scopes[Dim].a = split_value;
			central.scopes[Dim].b = split_value;
			central.scopes[Dim].da = -1 * da;
			central.scopes[Dim].db = -1 * db;
			central.step ++;

			return {left, right, central};
		}

// ~~~~~~~~~~~~~~~~~~~~~~~~ Generic cut with n dimensions ~~~~~~~~~~~~~~~~~~~~~~~~~~

		typedef std::vector<Hyperspace<Dimensions>> CutDim;

		template <unsigned Dim>
		static inline CutDim split_1d(int split_value, const Hyperspace<Dimensions>& hyp, int da, int db){

			if (split_value == hyp.scopes[Dim].a) return {hyp};
			if (split_value == hyp.scopes[Dim].b) return {hyp};

			assert(split_value > hyp.scopes[Dim].a  && "nonsense split");
			assert(split_value < hyp.scopes[Dim].b  && "nonsense split");

			// if the dimension pressents a shape like an inverted V: split it in M
			if (hyp.scopes[Dim].da >= 0 && hyp.scopes[Dim].db <= 0) {
				auto partition = split_M<Dim> (split_value,  hyp, da, db);
				return CutDim(partition.begin(), partition.end());
			}

			// if the dimension pressents a shape like a V: split it in W
			if (hyp.scopes[Dim].da < 0 || hyp.scopes[Dim].db > 0) {
				auto partition = split_W<Dim> (hyp, -hyp.scopes[Dim].da, -hyp.scopes[Dim].db);
				return CutDim(partition.begin(), partition.end());
			}

			assert(false  && "this is not a valid geometry");
			return {};
		}

		template <unsigned Dim, typename ... Cuts>
		inline CutDim split(int cut) const{
			return split_1d<Dim> (cut, *this, this->scopes[Dim].da, this->scopes[Dim].db);
		}

		template <unsigned Dim, typename ... Cuts>
		inline CutDim split(int cut, Cuts ... cuts) const{

			auto tmp = split<Dim+1>(cuts...);

			CutDim res;
			for (const auto& hyp : tmp){
				auto x = split_1d<Dim> (cut, hyp, hyp.scopes[Dim].da, hyp.scopes[Dim].db);
				res.insert(res.end(), x.begin(), x.end());
			}

			return res;
		}

		template <typename ... Cuts>
		inline CutDim split(Cuts ... cuts) const{

			return split<0>(cuts...);
		}

// ~~~~~~~~~~~~~~~~~~~~~~~~ Cut with slopes ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		template <unsigned Dim >
		inline CutDim split_slopes(const CutWithSlopes& cut) const{
			return split_1d<Dim> (cut.split_value, *this, cut.da, cut.db);
		}

		template <unsigned Dim, typename ... Cuts>
		inline CutDim split_slopes(const CutWithSlopes& cut, Cuts ... cuts) const{

			auto tmp = split_slopes<Dim+1>(cuts...);

			CutDim res;
			for (const auto& hyp : tmp){
				auto x = split_1d<Dim> (cut.split_value, hyp, cut.da, cut.db);
				res.insert(res.end(), x.begin(), x.end());
			}

			return res;
		}

		template <typename ... Cuts>
		inline CutDim split_slopes(Cuts ... cuts) const{

			return split_slopes<0>(cuts...);
		}

// ~~~~~~~~~~~~~~~~~~~~~~~ comparison ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		bool operator == (const Hyperspace<Dimensions>& o) const{

			for (int d = 0; d< Dimensions; ++d){
				if (scopes[d].a != o.scopes[d].a || scopes[d].b != o.scopes[d].b ||
					scopes[d].da != o.scopes[d].da || scopes[d].db != o.scopes[d].db ){
					return false;
				}
			}
			return true;
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
