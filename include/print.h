
#pragma once

#include <ostream>
#include <iostream>

namespace utils {
class Printable;
class VirtualPrintable;
}

namespace std {

	inline std::ostream& operator<<(std::ostream& out, const utils::VirtualPrintable& printable);

	template<typename T>
	typename std::enable_if<
		std::is_base_of<utils::Printable, T>::value &&
		!std::is_base_of<utils::VirtualPrintable, T>::value,
	 	std::ostream&>::type
	operator<<(std::ostream& out, const T& printable) {
		return printable.printTo(out);
	}
}


namespace utils{


	/**
	 * A class forming an interface for printable classes. Implementing this interface allows
	 * classes to be printed to output streams using a member function.
	 */
	struct Printable {

	};

	/**
	 * A base class for all printable objects within a type hierarchy depending on a virtual printTo function.
	 */
	struct VirtualPrintable : public Printable {

		/**
		 * Allow the output operator to access protected members.
		 */
		friend std::ostream& std::operator<<(std::ostream&, const VirtualPrintable&);
		
	protected:

		/**
		 * A method to be implemented by sub-classes allowing instances to be printed to the
		 * output stream.
		 *
		 * @param out the stream this instance should be printed to
		 * @return the stream passed as an argument
		 */
		virtual std::ostream& printTo(std::ostream& out) const =0;
	};


	/**
	 * A class forming an adapter from class supporting the output operator <<
	 * and classes implementing the printable interface.
	 */
	template<typename T>
	class PrintWrapper : public VirtualPrintable {
		const T& content;
	public:
		PrintWrapper(const T& content) : content(content) {};
		std::ostream& printTo(std::ostream& out) const {
			return out << content;
		}
	};

	template<typename T>
	PrintWrapper<T> toVirtualPrintable(const T& element) {
		return PrintWrapper<T>(element);
	}



	std::string getSizeHuman(size_t n){
	
		auto toString =[](size_t size, const std::string& suffix){
			std::string tmp;
			tmp.append(std::to_string(size));
			tmp.append(suffix);
			return tmp;
		};

		auto KB = 1024;
		auto MB = 1024*1024;
		auto GB = 1024*1024*1024;

		if (n > GB){
			return toString(n/GB, "GB");
		}
		else if (n > MB){
			return toString(n/MB, "MB");
		}
		else if (n > KB){
			return toString(n/KB, "KB");
		}
		return toString(n, "Bytes");
	
	}

} // end of namespace utils

namespace std {

	inline std::ostream& operator<<(std::ostream& out, const utils::VirtualPrintable& printable) {
		return printable.printTo(out);
	}


} // namespace std
