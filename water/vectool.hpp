// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_VECTOOL_HPP
#define WATER_VECTOOL_HPP
#include <water/water.hpp>
#include <water/numeric_limits.hpp>
#include <water/types/types.hpp>
#include <water/new_here.hpp>
#ifdef WATER_NO_CHEADERS
	#include <string.h>
#else
	#include <cstring>
	namespace water {
	using std::memcmp;
	using std::memcpy;
	using std::memmove;
	}
#endif
namespace water {

/*

This vector-tool is used by water::temporary::vector and water::vector

*/

namespace _ { namespace vectools {

	template<bool destruct_> struct
	 destruct {
		template<typename value_> static void do_it(value_* v) noexcept {
			v->~value_();
			}
		template<typename value_> static void do_it(value_* b, value_* e) noexcept {
			while(b != e) (--e)->~value_();
			}
		};
	template<> struct
	 destruct<false> {
		template<typename value_> static void do_it(value_*) noexcept {}
		template<typename value_> static void do_it(value_*, value_*) noexcept {}
		};
	
	template<typename value_, bool destruct_> struct
	 auto_destruct {
		value_ *b, *e;
		auto_destruct(value_ *a) noexcept : b(a), e(a)
			{}
		~auto_destruct() noexcept {
			destruct<destruct_>::do_it(b, e);
			}
		void dont() noexcept {
			b = e = 0;
			}
		};
	
	template<typename value_> class
	 referator {
		value_ const* my;
		public:
			referator(value_ const& a) noexcept :
				my(&a)
				{}
			referator& operator++() noexcept {
				return *this;
				}
			value_ const& operator*() const noexcept {
				return *my;
				}
		};
	
	template<typename value_> class
	 referator_move {
		value_* my;
		public:
			referator_move(value_ *a) noexcept :
				my(a)
				{}
			referator_move& operator++() noexcept {
				return *this;
				}
			value_&& operator*() const noexcept {
				return static_cast<value_&&>(*my);
				}
		};
		
	template<typename value_, bool memset_ = sizeof(value_) == 1> struct
	 fill {
		static void do_it(value_* b, value_ const& v, size_t s) {
			// s != 0
			size_t x = s < 16 ? s : 16;
			value_
				*i = b,
				*e = b + x;
			do *i = v; while(++i != e);
			e = b + s;
			while(i != e) {
				s = static_cast<size_t>(e - i);
				if(s > x) s = x;
				memcpy(i, b, s * sizeof(value_));
				x += s;
				i += s;
				}
			}
		};
	template<typename value_> struct
	 fill<value_, true> {
		static void do_it(value_* b, value_ const& v, size_t s) {
			memset(b, static_cast<int>(*static_cast<unsigned char const*>(static_cast<void const*>(&v))), s);
			}
		};
		
	template<typename value_, bool memcmp_ = !types::is_class_struct_union<value_>::result> struct
	 equal {
		static bool do_it(value_ const *a, value_ const *ae, value_ const *b, value_ const *be) {
			if(ae - a != be - b)
				return false;
			while(a != ae && *a == *b)
				++a, ++b;
			return a == ae;
			}
		};
	template<typename value_> struct
	 equal<value_, true> {
		static bool do_it(value_ const *a, value_ const *ae, value_ const *b, value_ const *be) {
			return (ae - a == be - b) && memcmp(a, b, static_cast<size_t>(ae - a) * sizeof(value_)) == 0;
			}
		};
	
	template<typename value_, bool memcmp_ = types::is_unsigned<value_>::result && numeric_limits<value_>::digits == numeric_limits<unsigned char>::digits> struct
	 less {
		static bool do_it(value_ const *a, value_ const *ae, value_ const *b, value_ const *be) {
			// like std::lexicographical_compare
			ptrdiff_t
				as = ae - a,
				bs = be - b;
			value_ const *e = as < bs ? ae : a + bs;
			while(a != e && *a == *b) ++a, ++b;
			return b != be && (a == ae || *a < *b);
			}
		};
	template<typename value_> struct
	 less<value_, true> {
		static bool do_it(value_ const *a, value_ const *ae, value_ const *b, value_ const *be) {
			size_t
				as = static_cast<size_t>(ae - a),
				bs = static_cast<size_t>(be - b);
			if(!as) return bs != 0;
			if(!bs) return false;
			int c = memcmp(a, b, as < bs ? as : bs);
			return c < 0 || (c == 0 && as < bs);
			}
		};

}} //namespace _::vectools

template<typename value_, bool destruct_ = !has_trivial_destructor<value_>::result, bool memcpy_ = has_trivial_copy_constructor<value_>::result && has_trivial_copy_assign<value_>::result> struct
 vectool {
	template<typename input_iterator_>
	 static void assign(value_* vb, value_*& ve, input_iterator_ f, size_t fs) {
		// assign to existing memory
		// - vb is begin
		// - ve is end
		// - fs can be 0
		value_
			*i = vb,
			*e = vb + fs;
		if(e > ve) {
			e = ve;
			fs -= static_cast<size_t>(ve - vb);
			}
		else
			fs = 0;
		while(i != e) {
			*i++ = *f; // can throw
			++f;
			}
		if(fs) {
			e += fs;
			do {
				new(here(ve)) value_(*f); // can throw
				++ve;
				++f; // can throw?
				} while(ve != e);
			}
		else {
			destruct(e, ve);
			ve = e;
			}
		}
	static void assign_value(value_* vb, value_*& ve, value_ f, size_t fs) {
		// copy f because it might be in this
		assign(vb, ve, _::vectools::referator<value_>(f), fs);
		}
	template<typename input_iterator_>
	 static void construct(value_*& v, input_iterator_ f, size_t fs) {
		// copy construct
		// - fs == 0 is ok
		value_ *e = v + fs;
		while(v != e) {
			new(here(v)) value_(*f); // can throw
			++v;
			++f; // can throw?
			}
		}
	static void construct_move(value_*& v, value_ *f, value_ *fe) {
		while(f != fe)
			new(here(v++)) value_(static_cast<value_&&>(*f++));
		}
	static void construct_value(value_*& v, value_ const& f, size_t fs) {
		construct(v, _::vectools::referator<value_>(f), fs);
		}
	static void destruct(value_* v) noexcept {
		_::vectools::destruct<destruct_>::do_it(v);
		}
	static void destruct(value_* b, value_* e) noexcept {
		_::vectools::destruct<destruct_>::do_it(b, e);
		}
	static void erase(value_* b, value_* e, value_*& ve) {
		// erase
		// - b is begin of erase
		// - e is end of erase
		// - ve is vector-end
		while(e != ve) *b++ = static_cast<value_&&>(*e++); // can throw
		destruct(b, ve);
		ve = b;
		}
	template<typename input_iterator_>
	 static value_* insert(value_* vi, value_*& ve, input_iterator_ f, size_t fs) {
		// insert into large enough existing memory
		// - vi is insert position
		// - ve is end
		// - vi <= ve
		// - fs != 0
		//
		// vvvv0000
		// vviiiivv
		// if something throws, the values past the old end are destroyed here
		//
		size_t fs0 = fs;
		value_ *i = vi;
		_::vectools::auto_destruct<value_, destruct_> a1(ve + fs);
		if(size_t m = static_cast<size_t>(ve - vi)) {
			// move m values up, if fs >= m all is to uninited
			value_
				*vf = ve,
				*e = a1.b - (m < fs ? m : fs);
			do new(here(a1.b - 1)) value_(static_cast<value_&&>(*--vf)); while(--a1.b != e); // move to uninit (can throw)
			i = a1.b;
			while(vf != vi) *--i = static_cast<value_&&>(*--vf); // move to init (can throw)
			// assign into init
			if(m > fs) m = fs;
			fs -= m;
			e = (i = vi) + m;
			do {
				*i = *f; // can throw
				++f; // could throw?
				} while(++i != e);
			}
		if(fs) {
			_::vectools::auto_destruct<value_, destruct_> a2(i);
			i += fs;
			do {
				new(here(a2.e)) value_(*f); // can throw
				++a2.e;
				++f; // could throw?
				} while(a2.e != i);
			a2.dont();
			}
		a1.dont();
		ve += fs0;
		return vi;
		}
	template<typename input_iterator_>
	 static value_* insert_copy(value_*& ve, value_* c, value_* ci, value_ *ce, input_iterator_ f, size_t fs) {
		// insert into new from old
		// - ve is end of new (end == begin)
		// - c is begin of old
		// - ci is insert pos in old
		// - ce is end of old
		// - fs != 0
		// - c <= ci <= ce
		// - old can be empty (c == ci == ce)
		//
		while(c != ci) {
			new(here(ve)) value_(static_cast<value_&&>(*c++)); // can throw
			++ve;
			}
		value_
			*r = ve,
			*e = ve + fs;
		do {
			new(here(ve)) value_(*f); // can throw
			++ve;
			++f; // can throw?
			} while(ve != e);
		while(c != ce) {
			new(here(ve)) value_(static_cast<value_&&>(*c++)); // can throw
			++ve;
			}
		return r;
		}
	static value_* insert_value(value_* vi, value_*& ve, value_ f, size_t fs) {
		// f is a copy in case it is inside this
		return insert(vi, ve, _::vectools::referator_move<value_>(&f), fs);
		}
	static value_* insert_value_copy(value_*& ve, value_* c, value_* ci, value_ *ce, value_ const& f, size_t fs) {
		return insert_copy(ve, c, ci, ce, _::vectools::referator<value_>(f), fs);
		}
	static value_* insert_move(value_* vi, value_*& ve, value_&& f) {
		return insert(vi, ve, _::vectools::referator_move<value_>(&f), 1);
		}
	static value_* insert_move_copy(value_*& ve, value_* c, value_* ci, value_ *ce, value_&& f) {
		return insert_copy(ve, c, ci, ce, _::vectools::referator_move<value_>(&f), 1);
		}
	static bool equal(value_ const *a, value_ const *ae, value_ const *b, value_ const *be) {
		return _::vectools::equal<value_, false>::do_it(a, ae, b, be);
		}
	static bool less(value_ const *a, value_ const *ae, value_ const *b, value_ const *be) {
		return _::vectools::less<value_, false>::do_it(a, ae, b, be);
		}
	};

template<typename value_, bool destruct_> struct
 vectool<value_, destruct_, true> {
	static void assign(value_* vb, value_*& ve, value_ const* f, size_t fs) noexcept {
		if(fs) memcpy(vb, f, fs * sizeof(value_));
		vb += fs;
		if(vb < ve) destruct(vb, ve);
		ve = vb;
		}
	static void assign(value_* vb, value_*& ve, value_* f, size_t fs) noexcept {
		assign(vb, ve, static_cast<value_ const*>(f), fs);
		}
	template<typename input_iterator_>
	 static void assign(value_* vb, value_*& ve, input_iterator_ f, size_t fs) {
		no_memcpy::assign(vb, ve, f, fs);
		}
	static void assign_value(value_* vb, value_*& ve, value_ f, size_t fs) noexcept {
		// copy f because it might be in this
		if(fs) _::vectools::fill<value_>::do_it(vb, f, fs);
		vb += fs;
		if(vb < ve) destruct(vb, ve);
		ve = vb;
		}
	static void construct(value_*& v, value_ const* f, size_t fs) noexcept {
		if(fs) {
			memcpy(v, f, fs * sizeof(value_));
			v += fs;
			}
		}
	static void construct(value_*& v, value_* f, size_t fs) noexcept {
		construct(v, static_cast<value_ const*>(f), fs);
		}
	template<typename input_iterator_>
	 static void construct(value_*& v, input_iterator_ f, size_t fs) {
		no_memcpy::construct(v, f, fs);
		}
	static void construct_move(value_*& v, value_ *f, value_ *fe) {
		construct(v, static_cast<value_ const*>(f), static_cast<size_t>(fe - f));
		}
	static void construct_value(value_*& v, value_ const& f, size_t fs) {
		if(fs) {
			_::vectools::fill<value_>::do_it(v, f, fs);
			v += fs;
			}
		}
	static void destruct(value_* v) noexcept {
		_::vectools::destruct<destruct_>::do_it(v);
		}
	static void destruct(value_* b, value_* e) noexcept {
		_::vectools::destruct<destruct_>::do_it(b, e);
		}
	static void erase(value_* b, value_* e, value_*& ve) noexcept {
		size_t s = static_cast<size_t>(ve - e);
		if(s) memmove(b, e, s * sizeof(value_));
		s = static_cast<size_t>(e - b);
		destruct(ve - s, ve);
		ve -= s;
		}
	static value_* insert(value_* vi, value_*& ve, value_ const* f, size_t fs) noexcept {
		// memcpy can be used insetad of memmove if m >= fs, but it looks like it makes no difference at all
		if(size_t m = static_cast<size_t>(ve - vi))
			memmove(vi + fs, vi, m * sizeof(value_));
		memcpy(vi, f, fs * sizeof(value_));
		ve += fs;
		return vi;
		}
	static value_* insert(value_* vi, value_*& ve, value_* f, size_t fs) noexcept {
		return insert(vi, ve, static_cast<value_ const*>(f), fs);
		}
	template<typename input_iterator_>
	 static value_* insert(value_* vi, value_*& ve, input_iterator_ f, size_t fs) {
		size_t fs0 = fs;
		value_ *i = vi;
		_::vectools::auto_destruct<value_, destruct_> a1(0);
		if(size_t m = static_cast<size_t>(ve - vi)) {
			// move m values up, if fs >= m all is to uninited
			memmove(a1.b = vi + fs, vi, m * sizeof(value_));
			a1.e = a1.b + m;
			// assign into init
			if(m > fs) m = fs;
			fs -= m;
			value_ *e = i + m;
			do {
				*i = *f; // can throw?
				++f; // can throw?
				} while(++i != e);
			}
		if(fs) {
			_::vectools::auto_destruct<value_, destruct_> a2(i);
			i += fs;
			do {
				*a2.e = *f; // can throw?
				++a2.e;
				++f; // can throw?
				} while(a2.e != i);
			a2.dont();
			}
		a1.dont();
		ve += fs0;
		return vi;
		}
	template<typename input_iterator_>
	 static value_* insert_copy(value_*& ve, value_* c, value_* ci, value_ *ce, input_iterator_ f, size_t fs) {
		size_t s;
		if(s = static_cast<size_t>(ci - c))
			memcpy(ve, c, s * sizeof(value_));
		value_ *r = ve += s;
		construct(ve, f, fs); // can throw
		if(s = static_cast<size_t>(ce - ci)) {
			memcpy(ve, ci, s * sizeof(value_));
			ve += s;
			}
		return r;
		}
	static value_* insert_value(value_* vi, value_*& ve, value_ f, size_t fs) noexcept {
		// f is copy in case it is inside this
		if(size_t m = static_cast<size_t>(ve - vi))
			memmove(vi + fs, vi, m * sizeof(value_));
		_::vectools::fill<value_>::do_it(vi, f, fs);
		ve += fs;
		return vi;
		}
	static value_* insert_value_copy(value_*& ve, value_* c, value_* ci, value_ *ce, value_ const& f, size_t fs) noexcept {
		size_t s;
		if(s = static_cast<size_t>(ci - c))
			memcpy(ve, c, s * sizeof(value_));
		value_ *r = ve += s;
		_::vectools::fill<value_>::do_it(ve, f, fs);
		ve += fs;
		if(s = static_cast<size_t>(ce - ci)) {
			memcpy(ve, ci, s * sizeof(value_));
			ve += s;
			}
		return r;
		}
	static value_* insert_move(value_* vi, value_*& ve, value_&& f) noexcept {
		return insert_value(vi, ve, f, 1);
		}
	static value_* insert_move_copy(value_*& ve, value_* c, value_* ci, value_ *ce, value_&& f) noexcept {
		return insert_value_copy(ve, c, ci, ce, f, 1);
		}
	static bool equal(value_ const *a, value_ const *ae, value_ const *b, value_ const *be) {
		return _::vectools::equal<value_>::do_it(a, ae, b, be);
		}
	static bool less(value_ const *a, value_ const *ae, value_ const *b, value_ const *be) {
		return _::vectools::less<value_>::do_it(a, ae, b, be);
		}
	private:
		typedef vectool<value_, destruct_, false> no_memcpy;
	};

}
#endif
