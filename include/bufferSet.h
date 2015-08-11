#pragma once

#include <array>
#include <vector>
#include <cassert>

#include "tools.h"
#include "print.h"

namespace stencil{
	

	template <typename Elem, unsigned Dimensions, unsigned Copies = 2>
	struct BufferSet: public utils::Printable{

		static const unsigned copies = Copies;
		const std::array<unsigned, Dimensions> dimensions;
		std::array<std::vector<Elem>, copies> storage;

// ~~~~~~~~~~~~~~~~~~~~~~~ Constructor  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		BufferSet(const std::vector<Elem>& data, const std::array<unsigned, Dimensions>& dimensions)
			: dimensions(dimensions), storage({std::vector<Elem>(data.begin(), data.end())})
		{ 
			for(int i=1; i < copies; ++i) {
				//storage[i].insert(storage[i].begin(), data.begin(), data.end());
				storage[i].resize(getSize());
			}

		}

		BufferSet(std::vector<Elem>&& data, const std::array<unsigned, Dimensions>& dimensions)
			: dimensions(dimensions), storage({std::move(data)})
		{ 
			for(int i=1; i < copies; ++i) {
				storage[i].resize(getSize());
			}
		}

// ~~~~~~~~~~~~~~~~~~~~~~~ getters  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		Elem* getPointer(unsigned copy = 0){
			assert( storage[copy].size() == getSize());
			return storage[copy].data();
		}

		std::vector<Elem> getData(unsigned copy = 0){
			return storage[copy];
		}

		unsigned getSize(){
			unsigned acum =1;
			for (unsigned i =0; i< Dimensions; ++i) acum *= dimensions[i];
			return acum;
		}

// ~~~~~~~~~~~~~~~~~~~~~~~ Comparison ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		bool operator == (const BufferSet<Elem, Dimensions, Copies>& o){

			for (int c = 0; c < Copies; ++c){
				if (storage[c].size() != o.storage[c].size()) return false;
				for (int i = 0; i< storage[c].size(); ++i){
					if (storage[c][i] != o.storage[c][i]) return false;
				}
			}
			return true;
		}

		bool operator != (const BufferSet<Elem, Dimensions, Copies>& o){
			return !(*this == o);
		}

// ~~~~~~~~~~~~~~~~~~~~~~~ other tools ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		std::ostream& printTo(std::ostream& out) const{
			
			out << "Bufferset[";
			for (const auto& i : dimensions) out << i << ",";
			out << "]x" << copies;

		//	out << " {";
		//	for (const auto& i : storage[0]) out << (unsigned)i << ",";
		//	out << "}";

			return out;
		}
	};

// ~~~~~~~~~~~~~~~~~~~~~~~ external Getters  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		#define FOR_DIMENSION(N) \
			template<typename E, unsigned D, unsigned C>\
			inline typename std::enable_if< is_eq<D, N>::value, E&>::type

		FOR_DIMENSION(1) getElem(BufferSet<E,D,C>& b, unsigned i, unsigned t = 0){
			assert(i<b.dimensions[0] && "i out of range");
			return b.storage[t%b.copies][i];
		}

		FOR_DIMENSION(2) getElem(BufferSet<E,D,C>& b, unsigned i, unsigned j, unsigned t = 0){
			assert(i<b.dimensions[0] && "i out of range");
			assert(j<b.dimensions[1] && "j out of range");
			return b.storage[t%b.copies][i+j*b.dimensions[0]];
		}
		
		FOR_DIMENSION(3) getElem(BufferSet<E,D,C>& b, unsigned i, unsigned j, unsigned k, unsigned t = 0){
			assert(i<b.dimensions[0] && "i out of range");
			assert(j<b.dimensions[1] && "j out of range");
			assert(k<b.dimensions[2] && "k out of range");
			return b.storage[t%b.copies][i+j*b.dimensions[0]+k*b.dimensions[1]*b.dimensions[0]];
		}
		
		#undef FOR_DIMENSION

		#define FROM_DIMENSION(N) \
			template<typename E, unsigned D, unsigned C>\
			inline typename std::enable_if< is_ge<D, N>::value, int>::type

		FROM_DIMENSION(1) getW(BufferSet<E,D,C>& b){
			return b.dimensions[0];
		}
		FROM_DIMENSION(2) getH(BufferSet<E,D,C>& b){
			return b.dimensions[1];
		}
		FROM_DIMENSION(3) getD(BufferSet<E,D,C>& b){
			return b.dimensions[2];
		}

		#undef FROM_DIMENSION
}
