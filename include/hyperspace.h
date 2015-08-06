#pragma once

#include <array>
#include <vector>
#include <cassert>

#include "tools.h"

namespace stencil{


	template <unsigned Dimensions>
	class Hyperspace : public utils::Printable{

		struct Scope {
			unsigned a;
			unsigned b;
			int da;
			int db;
		};

		std::array<Scope, Dimensions> scopes;
		

	public:
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

		Hyperspace( unsigned xa, unsigned xb, int dxa, int dxb)
				: scopes ({Scope{xa,xb,dxa,dxb}}){
			static_assert ( Dimensions == 1 , "this constructor is only allowed for 1D spaced");

		}

		Hyperspace( unsigned xa, unsigned xb, int dxa, int dxb,
		            unsigned ya, unsigned yb, int dya, int dyb)
				: scopes ({Scope{xa,xb,dxa,dxb}, Scope{ya,yb,dya,dyb}}){
			static_assert ( Dimensions == 2 , "this constructor is only allowed for 2D spaced");
		}

		Hyperspace( unsigned xa, unsigned xb, int dxa, int dxb,
		            unsigned ya, unsigned yb, int dya, int dyb,
		            unsigned za, unsigned zb, int dza, int dzb)
				: scopes ({Scope{xa,xb,dxa,dxb}, Scope{ya,yb,dya,dyb}, Scope{za,zb,dza,dzb}}){
			static_assert ( Dimensions == 2 , "this constructor is only allowed for 3D spaced");
		}

		Hyperspace(std::array<unsigned, Dimensions> a,
				   std::array<unsigned, Dimensions> b,
				   std::array<int,      Dimensions> da,
				   std::array<int,      Dimensions> db) { 
			for (unsigned i = 0; i< Dimensions; ++i)
				scopes[i] = {a[i], b[i], da[i], db[i]};
		}


// ~~~~~~~~~~~~~~~~~~~~~~~ Spliting tools ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		typedef std::vector<Hyperspace<Dimensions>> CutDim;

		CutDim split_1d(unsigned dimension, int split_v, const Hyperspace<Dimensions>& Hyp)const{


			auto left = Hyp;
			left.scopes[dimension].b = split_v;
			auto right = Hyp;
			right.scopes[dimension].a = split_v;

			auto inverted = Hyp;
			inverted.scopes[dimension].a = split_v;
			inverted.scopes[dimension].b = split_v;
			inverted.scopes[dimension].da = -1 * inverted.scopes[dimension].da;
			inverted.scopes[dimension].db = -1 * inverted.scopes[dimension].db;

			return {left, right, inverted};
		}

		template <unsigned Dim, typename ... Cuts>
		CutDim split(int cut) const{
			if (cut == 0) return {*this};
			return split_1d(Dim, cut, *this);
		}

		template <unsigned Dim, typename ... Cuts>
		CutDim split(int cut, Cuts ... cuts) const{
			if (cut == 0) return split<Dim+1>(cuts...);

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
