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

	public:

// ~~~~~~~~~~~~~~~~~~~~~~~ ORTODOX  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		Hyperspace( const Hyperspace<Dimensions>& o)
			: scopes(o.scopes)
			{}

		Hyperspace(Hyperspace<Dimensions>&& o)
			: scopes(std::move(o.scopes))
			{}

		Hyperspace<Dimensions> operator=(const Hyperspace<Dimensions>& o){
			scopes = o.scopes;
		}
		Hyperspace<Dimensions> operator=(Hyperspace<Dimensions>&& o){
			std::swap(scopes, o.scopes);
		}

// ~~~~~~~~~~~~~~~~~~~~~~~ Spetialized ctors  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		Hyperspace( int xa, int xb, int dxa, int dxb)
				: scopes ({Scope{xa,xb,dxa,dxb}}){
			static_assert ( Dimensions == 1 , "this constructor is only allowed for 1D spaced");

		}

		Hyperspace( int xa, int xb, int dxa, int dxb,
		            int ya, int yb, int dya, int dyb)
				: scopes ({Scope{xa,xb,dxa,dxb}, Scope{ya,yb,dya,dyb}}){
			static_assert ( Dimensions == 2 , "this constructor is only allowed for 2D spaced");
		}

		Hyperspace( int xa, int xb, int dxa, int dxb,
		            int ya, int yb, int dya, int dyb,
		            int za, int zb, int dza, int dzb)
				: scopes ({Scope{xa,xb,dxa,dxb}, Scope{ya,yb,dya,dyb}, Scope{za,zb,dza,dzb}}){
			static_assert ( Dimensions == 2 , "this constructor is only allowed for 3D spaced");
		}

		Hyperspace(std::array<int, Dimensions> a,
				   std::array<int, Dimensions> b,
				   std::array<int,      Dimensions> da,
				   std::array<int,      Dimensions> db) { 
			for (int i = 0; i< Dimensions; ++i)
				scopes[i] = {a[i], b[i], da[i], db[i]};
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

		CutDim split_1d(unsigned dimension, int split_v, const Hyperspace<Dimensions>& Hyp)const{

			if (split_v == scopes[dimension].a) return {Hyp};
			if (split_v == scopes[dimension].b) return {Hyp};

			auto left = Hyp;
			assert(left.scopes[dimension].b > split_v);
			left.scopes[dimension].b = split_v;
			assert(left.scopes[dimension].a < left.scopes[dimension].b);

			auto right = Hyp;
			assert(right.scopes[dimension].a < split_v);
			right.scopes[dimension].a = split_v;
			assert(right.scopes[dimension].a < right.scopes[dimension].b);

			auto inverted = Hyp;
			inverted.scopes[dimension].a = split_v;
			inverted.scopes[dimension].b = split_v;
			inverted.scopes[dimension].da = -1 * inverted.scopes[dimension].da;
			inverted.scopes[dimension].db = -1 * inverted.scopes[dimension].db;

			return {left, right, inverted};
		}

		template <unsigned Dim, typename ... Cuts>
		CutDim split(int cut) const{
			return split_1d(Dim, cut, *this);
		}

		template <unsigned Dim, typename ... Cuts>
		CutDim split(int cut, Cuts ... cuts) const{

			auto tmp = split<Dim+1>(cuts...);
			CutDim res;

			for (const auto& hyp : tmp){
				auto x = split_1d(Dim, cut, hyp);
				res.insert(res.end(), x.begin(), x.end());
			}

			return res;
		}
		
		template <typename ... Cuts>
		CutDim split( Cuts ... cuts) const{

			return split<0>(cuts...);
		}

// ~~~~~~~~~~~~~~~~~~~~~~~ print ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		std::ostream& printTo(std::ostream& out) const{

			out << "Hyp";
			for (const auto& s : scopes) {
				out << "[" << s.a << "," << s.b << "](" << s.da << "," << s.db << ")";	
			}
			return out;
		}
		
	};




} // namespace stencil
