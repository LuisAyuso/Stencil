#pragma once

#include <array>
#include <vector>
#include <cassert>

#include "tools.h"
#include "print.h"
#include "tools/instrument.h"

namespace stencil{

	struct CutWithSlopes{
		int cut_point;
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
		//unsigned step;

	public:

		static const unsigned dimensions = Dimensions;

// ~~~~~~~~~~~~~~~~~~~~~~~ ORTODOX  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		Hyperspace() = default;
			

		Hyperspace( const Hyperspace<Dimensions>& o)
			: scopes(o.scopes) // , step(o.step)
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
			//step = o.step;

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
				: scopes ({Scope{xa,xb,dxa,dxb}})  // , step(s)
		{
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
				: scopes ({Scope{xa,xb,dxa,dxb}, Scope{ya,yb,dya,dyb}}) //, step(s)
		{
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
				: scopes ({Scope{xa,xb,dxa,dxb}, Scope{ya,yb,dya,dyb}, Scope{za,zb,dza,dzb}}) //, step(s)
		{
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
				//: step(s)
		{ 

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

//	int getStep() const{
//		return step;
//	}
//
//	void increaseStep(){ 
//		step++;
//	}

// ~~~~~~~~~~~~~~~~~~~~~~~ Spliting tools ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		typedef std::array<Hyperspace<Dimensions>,3> CutDim_exact;

		/**
		 * 	Split dimension, one central triagle plus two inverted triangles afterwards
		 */
		template <unsigned Dim>
		static inline CutDim_exact split_W(const Hyperspace<Dimensions>& hyp, int da, int db){

			SPLIT_INSTRUMENT(hyp);
			//std::cout << "W " << cut_point << "  " << da << ":" << db << std::endl;
			//std::cout << hyp << std::endl;

			auto central = hyp;
			central.scopes[Dim].da = da;
			central.scopes[Dim].db = db;

			auto left = hyp;
			left.scopes[Dim].b = left.scopes[Dim].a;
			//left.step ++;
	
			auto right = hyp;
			right.scopes[Dim].a = right.scopes[Dim].b;
			//right.step ++;

			return {{central, left, right}};
		}

		/**
		 * 	Split dimension, one two triangles, one inverted triangle afterwards
		 */
		template <unsigned Dim>
		static inline CutDim_exact split_M(int cut_point, const Hyperspace<Dimensions>& hyp, int da, int db){

			assert (cut_point > hyp.scopes[Dim].a && "can not cut on bounduary");
			assert (cut_point < hyp.scopes[Dim].b && "can not cut on bounduary");

			SPLIT_INSTRUMENT(hyp);
			//std::cout << "  -cut " << hyp << " @ " << cut_point << "  " << da << ":" << db << std::endl;


			auto left = hyp;
			left.scopes[Dim].b = cut_point;
			left.scopes[Dim].db = db;

			auto right = hyp;
			right.scopes[Dim].a = cut_point;
			right.scopes[Dim].da = da;

			auto central = hyp;
			central.scopes[Dim].a = cut_point;
			central.scopes[Dim].b = cut_point;
			central.scopes[Dim].da = -1 * da;
			central.scopes[Dim].db = -1 * db;
			//central.step ++;

			//std::cout << "     - " << left << std::endl;
			//std::cout << "     - " << right << std::endl;
			//std::cout << "     - " << central << std::endl;

			return {{left, right, central}};
		}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~ Once again, turn slopes into template paramenters ~~~~~~~~~~~~~~~~~~~~~~~~~~

		/**
		 * 	Split dimension, one central triagle plus two inverted triangles afterwards
		 */
		template <unsigned Dim, int Slope>
		static inline CutDim_exact split_W2(const Hyperspace<Dimensions>& hyp){

			SPLIT_INSTRUMENT(hyp);
			//std::cout << "W " << cut_point <<  std::endl;
			//std::cout << hyp << std::endl;

			auto central = hyp;
			central.scopes[Dim].da = Slope;
			central.scopes[Dim].db = -Slope;

			auto left = hyp;
			left.scopes[Dim].b = left.scopes[Dim].a;
			//left.step ++;
	
			auto right = hyp;
			right.scopes[Dim].a = right.scopes[Dim].b;
			//right.step ++;

			return {{central, left, right}};
		}
		/**
		 * 	Split dimension, one two triangles, one inverted triangle afterwards
		 */
		template <unsigned Dim, int Slope>
		static inline CutDim_exact split_M2(int cut_point, const Hyperspace<Dimensions>& hyp){

			assert (cut_point > hyp.scopes[Dim].a && "can not cut on bounduary");
			assert (cut_point < hyp.scopes[Dim].b && "can not cut on bounduary");

			SPLIT_INSTRUMENT(hyp);
			//std::cout << "  -cut " << hyp << " @ " << cut_point << std::endl;

			auto left = hyp;
			left.scopes[Dim].b = cut_point;
			left.scopes[Dim].db = -1 * Slope;

			auto right = hyp;
			right.scopes[Dim].a = cut_point;
			right.scopes[Dim].da = Slope;

			auto central = hyp;
			central.scopes[Dim].a = cut_point;
			central.scopes[Dim].b = cut_point;
			central.scopes[Dim].da = -1 * Slope;
			central.scopes[Dim].db = Slope;
			//central.step ++;

			//std::cout << "     - " << left << std::endl;
			//std::cout << "     - " << right << std::endl;
			//std::cout << "     - " << central << std::endl;

			return {{left, right, central}};
		}

// ~~~~~~~~~~~~~~~~~~~~~~~~ Generic cut with n dimensions ~~~~~~~~~~~~~~~~~~~~~~~~~~

		typedef std::vector<Hyperspace<Dimensions>> CutDim;

		template <unsigned Dim>
		static inline CutDim split_1d(int cut_point, const Hyperspace<Dimensions>& hyp, int da, int db){

			if (cut_point == hyp.scopes[Dim].a) return {hyp};
			if (cut_point == hyp.scopes[Dim].b) return {hyp};

			assert(cut_point > hyp.scopes[Dim].a  && "nonsense split");
			assert(cut_point < hyp.scopes[Dim].b  && "nonsense split");

			// if the dimension pressents a shape like an inverted V: split it in M
			if (hyp.scopes[Dim].da >= 0 && hyp.scopes[Dim].db <= 0) {
				auto partition = split_M<Dim> (cut_point,  hyp, da, db);
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

	// ~~~~~~~~~~~  As template paramenters ~~~~~~~~~~~~~~
		template <unsigned Dim >
		inline CutDim split_slopes(const CutWithSlopes& cut) const{
			return split_1d<Dim> (cut.cut_point, *this, cut.da, cut.db);
		}

		template <unsigned Dim, typename ... Cuts>
		inline CutDim split_slopes(const CutWithSlopes& cut, Cuts ... cuts) const{

			auto tmp = split_slopes<Dim+1>(cuts...);

			CutDim res;
			for (const auto& hyp : tmp){
				auto x = split_1d<Dim> (cut.cut_point, hyp, cut.da, cut.db);
				res.insert(res.end(), x.begin(), x.end());
			}

			return res;
		}

		template <typename ... Cuts>
		inline CutDim split_slopes(Cuts ... cuts) const{

			return split_slopes<0>(cuts...);
		}

	// ~~~~~~~~~~~  As array ~~~~~~~~~~~~~~

		template <unsigned Dim, unsigned long Cuts>
		inline CutDim split_slopes_same_dim(const std::array<CutWithSlopes, Cuts>& cuts) const{

			auto curHyp = *this;
			CutDim res;
			for (const auto& cut : cuts){

				// each cut produces left/right + midle
				auto tmp = split_1d<Dim> (cut.cut_point, curHyp, cut.da, cut.db);

				res.push_back(tmp[0]);
				res.push_back(tmp[2]);

				curHyp = tmp[1];
			}
			res.push_back(curHyp);

			return res;
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
			//return o.step == step;
		}


// ~~~~~~~~~~~~~~~~~~~~~~~ print ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		std::ostream& printTo(std::ostream& out) const{

			out << "Hyp";
			for (const auto& s : scopes) {
				out << "[" << s.a << "," << s.b << "](" << s.da << "," << s.db << ")";
			}
			//out << " p(" << step << ")";	
			return out;
		}
		
	};




} // namespace stencil
