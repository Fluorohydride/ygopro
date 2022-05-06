/*
   Basic Unicode string class for Irrlicht.
   Copyright (c) 2009-2011 John Norman
   Copyright (c) 2022 Edoardo Lolletti

   This software is provided 'as-is', without any express or implied
   warranty. In no event will the authors be held liable for any
   damages arising from the use of this software.

   Permission is granted to anyone to use this software for any
   purpose, including commercial applications, and to alter it and
   redistribute it freely, subject to the following restrictions:

   1. The origin of this software must not be misrepresented; you
	  must not claim that you wrote the original software. If you use
	  this software in a product, an acknowledgment in the product
	  documentation would be appreciated but is not required.

   2. Altered source versions must be plainly marked as such, and
	  must not be misrepresented as being the original software.

   3. This notice may not be removed or altered from any source
	  distribution.

   The original version of this class can be located at:
   http://irrlicht.suckerfreegames.com/

   John Norman
   john@suckerfreegames.com
*/

#ifndef __IRR_USTRING_H_INCLUDED__
#define __IRR_USTRING_H_INCLUDED__

#include <cstring>
#include <cstdlib>
#include <cstdint>

#include <utility>
#include <iterator>


namespace irr {

typedef char32_t uchar32_t;
typedef char16_t uchar16_t;
typedef char uchar8_t;

namespace core {

namespace unicode {

//! The unicode replacement character.  Used to replace invalid characters.
constexpr uchar32_t UTF_REPLACEMENT_CHARACTER = 0xFFFD;

//! Convert a UTF-16 surrogate pair into a UTF-32 character.
//! \param high The high value of the pair.
//! \param low The low value of the pair.
//! \return The UTF-32 character expressed by the surrogate pair.
inline constexpr uchar32_t toUTF32(uchar16_t high, uchar16_t low) {
	// Convert the surrogate pair into a single UTF-32 character.
	uchar32_t x = ((high & ((1 << 6) - 1)) << 10) | (low & ((1 << 10) - 1));
	uchar32_t wu = ((high >> 6) & ((1 << 5) - 1)) + 1;
	return (wu << 16) | x;
}

} // end namespace unicode


//! UTF-16 string class.
class ustring16 {
	//! UTF-16 surrogate start values.
	static constexpr uint16_t UTF16_HI_SURROGATE = 0xD800;
	static constexpr uint16_t UTF16_LO_SURROGATE = 0xDC00;

	//! Is a UTF-16 code point a surrogate?
	static constexpr bool UTF16_IS_SURROGATE(uchar16_t c) {
		return (c & 0xF800) == UTF16_HI_SURROGATE;
	}
	static constexpr bool UTF16_IS_SURROGATE_HI(uchar16_t c) {
		return (c & 0xFC00) == UTF16_HI_SURROGATE;
	}
	static constexpr bool UTF16_IS_SURROGATE_LO(uchar16_t c) {
		return (c & 0xFC00) == UTF16_LO_SURROGATE;
	}
public:

	typedef uchar32_t access;


	//! Iterator to iterate through a UTF-16 string.
#ifndef USTRING_NO_STL
	class _ustring16_const_iterator : public std::iterator <
		std::bidirectional_iterator_tag,	// iterator_category
		access,								// value_type
		ptrdiff_t,							// difference_type
		const access,						// pointer
		const access						// reference
	>
#else
	class _ustring16_const_iterator
#endif
	{
	public:
		typedef _ustring16_const_iterator _Iter;
		typedef std::iterator<std::bidirectional_iterator_tag, access, ptrdiff_t, const access, const access> _Base;
		typedef const access const_pointer;
		typedef const access const_reference;

		typedef typename _Base::value_type value_type;
		typedef typename _Base::difference_type difference_type;
		typedef typename _Base::difference_type distance_type;
		typedef typename _Base::pointer pointer;
		typedef const_reference reference;

		//! Constructors.
		_ustring16_const_iterator(const _Iter& i) : ref(i.ref), pos(i.pos) {}
		_ustring16_const_iterator(const ustring16& s) : ref(&s), pos(0) {}
		_ustring16_const_iterator(const ustring16& s, const u32 p) : ref(&s), pos(0) {
			if(ref->size_raw() == 0 || p == 0)
				return;

			// Go to the appropriate position.

			advance(p);
		}

		//! Test for equalness.
		bool operator==(const _Iter& iter) const {
			if(ref == iter.ref && pos == iter.pos)
				return true;
			return false;
		}

		//! Test for unequalness.
		bool operator!=(const _Iter& iter) const {
			return !operator==(iter);
		}

		//! Switch to the next full character in the string.
		_Iter& operator++() {
			// ++iterator
			if(pos == ref->size_raw()) return *this;
			if(is_utf32) {
				pos++;
			} else {
				if(isNextCodepointValidSurrogatePair())
					pos += 2;
				else
					pos++;
				if(pos > ref->size_raw())
					pos = ref->size_raw();
			}
			return *this;
		}

		//! Switch to the next full character in the string, returning the previous position.
		_Iter operator++(int) {
			// iterator++
			_Iter _tmp(*this);
			++*this;
			return _tmp;
		}

		//! Switch to the previous full character in the string.
		_Iter& operator--() {
			// --iterator
			if(pos == 0) return *this;
			if(is_utf32) {
				--pos;
			} else {
				const uchar16_t* a = reinterpret_cast<const uchar16_t*>(ref->data());
				--pos;
				if(UTF16_IS_SURROGATE_LO(a[pos]) && pos != 0 && UTF16_IS_SURROGATE_HI(a[pos - 1]))	// low surrogate, go back one more.
					--pos;
			}
			return *this;
		}

		//! Switch to the previous full character in the string, returning the previous position.
		_Iter operator--(int) {
			// iterator--
			_Iter _tmp(*this);
			--*this;
			return _tmp;
		}

		//! Advance a specified number of full characters in the string.
		//! \return Myself.
		_Iter& operator+=(const difference_type v) {
			if(v == 0) return *this;
			if(v < 0) return operator-=(v * -1);

			if(pos >= ref->size_raw())
				return *this;

			advance(v);

			return *this;
		}

		//! Go back a specified number of full characters in the string.
		//! \return Myself.
		_Iter& operator-=(const difference_type v) {
			if(v == 0) return *this;
			if(v > 0) return operator+=(v * -1);

			if(pos == 0)
				return *this;

			go_back(v);

			return *this;
		}

		//! Return a new iterator that is a variable number of full characters forward from the current position.
		_Iter operator+(const difference_type v) const {
			_Iter ret(*this);
			ret += v;
			return ret;
		}

		//! Return a new iterator that is a variable number of full characters backward from the current position.
		_Iter operator-(const difference_type v) const {
			_Iter ret(*this);
			ret -= v;
			return ret;
		}

		//! Returns the distance between two iterators.
		difference_type operator-(const _Iter& iter) const {
			// Make sure we reference the same object!
			if(ref != iter.ref)
				return difference_type();

			_Iter i = iter;
			difference_type ret;

			// Walk up.
			if(pos > i.pos) {
				while(pos > i.pos) {
					++i;
					++ret;
				}
				return ret;
			}

			// Walk down.
			while(pos < i.pos) {
				--i;
				--ret;
			}
			return ret;
		}

		//! Accesses the full character at the iterator's position.
		const_reference operator*() const {
			auto size = ref->size_raw();
			_IRR_DEBUG_BREAK_IF(pos >= size);
			if(is_utf32) {
				const uchar32_t* data = reinterpret_cast<const uchar32_t*>(ref->data());
				auto ch = data[pos];
				if(ch >= 0xFDD0 && ch <= 0xFDEF)
					return unicode::UTF_REPLACEMENT_CHARACTER;
				return ch;
			}
			const uchar16_t* data = reinterpret_cast<const uchar16_t*>(ref->data());
			auto hi = data[pos];
			if(!UTF16_IS_SURROGATE(hi)) {
				if(hi >= 0xFDD0 && hi <= 0xFDEF)
					return unicode::UTF_REPLACEMENT_CHARACTER;
				return static_cast<uchar32_t>(hi);
			}
			if((pos + 1 >= size) || !UTF16_IS_SURROGATE_HI(hi))
				return unicode::UTF_REPLACEMENT_CHARACTER;
			auto lo = data[pos + 1];
			if(!UTF16_IS_SURROGATE_LO(lo))
				return unicode::UTF_REPLACEMENT_CHARACTER;
			return unicode::toUTF32(hi, lo);
		}

		//! Accesses the full character at the iterator's position.
		const_pointer operator->() const {
			return operator*();
		}

		//! Accesses the full character at the iterator's position.
		pointer operator->() {
			return operator*();
		}

		//! Is the iterator at the start of the string?
		bool atStart() const {
			return pos == 0;
		}

		//! Is the iterator at the end of the string?
		bool atEnd() const {
			return pos >= ref->size_raw();
		}

		//! Moves the iterator to the start of the string.
		void toStart() {
			pos = 0;
		}

		//! Moves the iterator to the end of the string.
		void toEnd() {
			pos = ref->size_raw();
		}

		//! Returns the iterator's position.
		//! \return The iterator's position.
		u32 getPos() const {
			return pos;
		}

	protected:
		const ustring16* ref;
		u32 pos;
		static constexpr bool is_utf32 = sizeof(wchar_t) == 4;

		bool isNextCodepointValidSurrogatePair() const {
			if(is_utf32)
				return true;
			const uchar16_t* data = reinterpret_cast<const uchar16_t*>(ref->data());
			if(!UTF16_IS_SURROGATE_HI(data[pos]))
				return false;
			if((pos + 1) >= ref->size_raw())
				return false;
			return UTF16_IS_SURROGATE_LO(data[pos + 1]);
		}

		void advance(const difference_type v) {
			u32 sr = ref->size_raw();
			if(is_utf32) {
				pos += (u32)v;
			} else {
				u32 i = (u32)v;
				while(i != 0 && pos < sr) {
					if(isNextCodepointValidSurrogatePair()) {
						pos += 2;
					} else
						pos++;
					--i;
				}
			}
			if(pos > sr)
				pos = sr;
		}

		void go_back(const difference_type v) {
			if(is_utf32) {
				if(pos < (u32)v)
					pos = 0;
				pos -= v;
			} else {
				// Go to the appropriate position.
				// TODO: Don't force u32 on an x64 OS.  Make it agnostic.
				u32 i = (u32)v;
				const uchar16_t* a = reinterpret_cast<const uchar16_t*>(ref->data());
				while(i != 0 && pos != 0) {
					--pos;
					if(UTF16_IS_SURROGATE_LO(a[pos]) && pos != 0 && UTF16_IS_SURROGATE_HI(a[pos - 1]))
						--pos;
					--i;
				}
			}
		}
	};

	typedef typename ustring16::_ustring16_const_iterator const_iterator;

	///----------------------///
	/// end iterator classes ///
	///----------------------///

	//! Default constructor
	ustring16()
		: data_(nullptr), size_(0xffffffff), size_raw_(0) {
	}


	//! Constructor from other string types
	ustring16(const ustring16& other)
		: data_(nullptr), size_(0xffffffff), size_raw_(0) {
		data_ = other.data();
		size_raw_ = other.size_raw();
	}

	template <class T>
	ustring16(const T& other)
		: data_(nullptr), size_(0), size_raw_(0) {
		assign(other.data(), other.size());
	}


	//! Constructor for copying a UTF-32 string from a pointer.
	ustring16(const wchar_t* const c)
		: data_(nullptr), size_(0), size_raw_(0) {
		assign(c);
	}


	//! Constructor for copying a UTF-32 from a pointer with a given length.
	ustring16(const wchar_t* const c, u32 length)
		: data_(nullptr), size_(0), size_raw_(0) {
		assign(c, length);
	}


	//! Assignment operator for other string types
	ustring16& operator=(const ustring16& other) {
		data_ = other.data();
		size_raw_ = other.size_raw();
		size_ = 0xffffffff;
		return *this;
	}

	template <class T>
	ustring16& operator=(const T& other) {
		assign(other.data(), other.size());
		return *this;
	}


	//! Assignment operator for UTF-16 strings
	ustring16& operator=(const wchar_t* const c) {
		assign(c);
		return *this;
	}


	//! Assignment operator for other strings.
	/** Note that this assumes that a correct unicode string is stored in the string. **/
	template <class B>
	ustring16& operator=(const B* const c) {
		static_assert(sizeof(B) == sizeof(wchar_t), "unsupported character size");
		*this = reinterpret_cast<const wchar_t* const>(c);

		return *this;
	}


	//! Returns the length of a ustring16 in full characters.
	//! \return Length of a ustring16 in full characters.
	u32 size() const {
		if(sizeof(wchar_t) == 4)
			return size_raw_;

		if(size_ != 0xffffffff)
			return size_;

		const uchar16_t* array = reinterpret_cast<const uchar16_t*>(data_);
		size_ = 0;
		for(u32 i = 0; i < size_raw_; ++i, ++size_) {
			if(UTF16_IS_SURROGATE_HI(array[i])) {
				if((i + 1) >= size_raw_)
					break;
				++i;
			}
		}
		return size_;
	}


	//! Informs if the ustring is empty or not.
	//! \return True if the ustring is empty, false if not.
	bool empty() const {
		return (size_raw() == 0);
	}


	//! Returns a pointer to the raw UTF-16 string data.
	//! \return pointer to C-style NUL terminated array of UTF-16 code points.
	const wchar_t* data() const {
		return data_;
	}


	//! assigns a UTF-16 string to this ustring16
	//! \param other The UTF-16 string to assign.
	//! \param length The length of the string to assign.
	//! \return A reference to our current string.
	void assign(const wchar_t* const other, u32 length = 0xffffffff) {

		// Calculate the size of the string to read in.
		if(length == 0xffffffff) {
			length = 0;
			auto* p = other;
			while(*p++) {
				++length;
			}
		}

		data_ = other;
		size_raw_ = length;
		size_ = 0xffffffff;
		validate();
	}


	//! Finds first occurrence of character.
	//! \param c The character to search for.
	//! \return Position where the character has been found, or -1 if not found.
	s32 findFirst(uchar32_t c) const {
		s32 pos = 0;
		for(auto t : *this) {
			if(c == t)
				return pos;
			++pos;
		}

		return -1;
	}


	//! Validate the existing ustring16, checking for valid surrogate pairs and checking for proper termination.
	//! \return A reference to our current string.
	void validate() {
		// Validate all unicode characters.
		if(sizeof(wchar_t) == 4) {
			size_ = size_raw_;
			return;
		}
	}


	//! Returns the raw number of UTF-16 code points in the string which includes the individual surrogates.
	//! \return The raw number of UTF-16 code points, excluding the trialing NUL.
	u32 size_raw() const {
		return size_raw_;
	}


	//! Returns an iterator to the beginning of the string.
	//! \return An iterator to the beginning of the string.
	const_iterator begin() const {
		return { *this };
	}


	//! Returns an iterator to the beginning of the string.
	//! \return An iterator to the beginning of the string.
	const_iterator cbegin() const {
		return begin();
	}


	//! Returns an iterator to the end of the string.
	//! \return An iterator to the end of the string.
	const_iterator end() const {
		const_iterator i{ *this };
		i.toEnd();
		return i;
	}


	//! Returns an iterator to the end of the string.
	//! \return An iterator to the end of the string.
	const_iterator cend() const {
		return end();
	}

private:

	//--- member variables

	const wchar_t* data_;
	size_t size_raw_;
	mutable size_t size_;
};

typedef ustring16 ustring;

} // end namespace core
} // end namespace irr

#endif
