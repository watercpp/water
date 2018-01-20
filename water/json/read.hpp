// Copyright 2017-2018 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_JSON_READ_HPP
#define WATER_JSON_READ_HPP
#include <water/json/encoding.hpp>
#include <water/json/node.hpp>
#include <water/json/memory.hpp>
#include <water/json/read_number.hpp>
#include <water/json/read_string.hpp>
namespace water { namespace json {

// read
//
// parse json into nodes, using memory_.
//
// there is no need to keep this after reading, only the nodes matter.
//
template<typename memory_> class
 read {
	public:
		using memory_type = memory_;
		using node_type = node<memory_type>;
	private:
		memory_type *mymemory;
		memory_node *my = 0;
		char8_t
			*myat = 0,
			*myend = 0;
		bool
			myimprecise = false, // if any number is imprecise
			myoverflow = false,
			myerror_memory = false,
			myclip_name = false,
			myclip_string = false;
	public:
		read(memory_type& a) :
			mymemory{&a}
			{}
		explicit operator bool() const {
			return my != 0;
			}
		string<char const*> parse_error() const {
			if(myat == myend || myerror_memory)
				return {};
			return {
				static_cast<char const*>(static_cast<void const*>(myat)),
				static_cast<char const*>(static_cast<void const*>(myend))
				};
			}
		bool allocation_failed() const {
			return myerror_memory;
			}
		read& parse_in_place(char *begin, char *end) {
			reset();
			myat = static_cast<char8_t*>(static_cast<void*>(begin));
			myend = static_cast<char8_t*>(static_cast<void*>(end));
			myat = parse();
			return *this;
			}
		read& parse_in_place(void *pointer, size_t bytes) {
			// This expects each byte to contain 8 bits
			reset();
			if(pointer && bytes) {
				auto e = encoding(pointer, bytes);
				char8_t *p = static_cast<char8_t*>(pointer) + e.byte_order_mark();
				size_t s = bytes - e.byte_order_mark();
				if(e.utf32()) {
					if(s % 4)
						return *this;
					auto from = unicode::byterator<32>(p, e.big_endian());
					// can convert in place
					auto convert = unicode::utf_from_utf_verify<8, 32>(p, from, s / 4);
					if(!convert.size())
						return *this;
					s = convert.size();
					}
				else if(e.utf16()) {
					if(s % 2)
						return *this;
					auto from = unicode::byterator<16>(p, e.big_endian());
					auto length = unicode::utf_length<16>(from, s / 2);
					if(!length)
						return *this;
					p = static_cast<char8_t*>(mymemory->allocate_with_undo(length.utf8(), 1));
					if(!p)
						return *this;
					auto convert = unicode::utf_from_utf<8, 16>(p, from, s / 2);
					s = convert.size();
					___water_assert(convert.size() == length.utf8());
					}
				myat = p;
				myend = p + s;
				myat = parse();
				}
			return *this;
			}
		template<typename iterator_>
		 read& operator()(iterator_ begin, iterator_ end) {
			reset();
			if(unicode::utf_from_iterator<iterator_>::result == 8)
				copy_and_parse(begin, size(begin, end));
			else
				convert_and_parse(begin, end);
			return *this;
			}
		template<typename iterator_>
		 read& operator()(iterator_ begin, size_t size) {
			reset();
			if(unicode::utf_from_iterator<iterator_>::result == 8)
				copy_and_parse(begin, size);
			else
				convert_and_parse(begin, size);
			return *this;
			}
		node_type nodes() const {
			return {*mymemory, my};
			}
		bool imprecise_numbers() const {
			return myimprecise;
			}
		bool numbers_did_overflow() const {
			return myoverflow;
			}
		bool string_was_cut() const {
			return myclip_string;
			}
		bool object_name_was_cut() const {
			return myclip_name;
			}
	private:
		template<typename iterator_>
		 void copy_and_parse(iterator_ begin, size_t size) {
			if(size && (myat = static_cast<char8_t*>(mymemory->allocate_with_undo(size, 1)))) {
				myend = myat + size;
				auto to = myat;
				do {
					*to = static_cast<char8_t>(*begin);
					++begin;
					} while(++to != myend);
				myat = parse();
				}
			}
		template<typename iterator_, typename iterator_or_size_>
		 void convert_and_parse(iterator_ begin, iterator_or_size_ end_or_size) {
			auto length = unicode::utf_length_from(begin, end_or_size);
			if(length.utf8() && (myat = static_cast<char8_t*>(mymemory->allocate_with_undo(length.utf8(), 1)))) {
				auto convert = unicode::utf_from_utf(myat, begin, end_or_size);
				myend = convert.end();
				___water_assert(convert.size() == length.utf8());
				myat = parse();
				}
			}
		void reset() {
			my = 0;
			myat = myend = 0;
			myimprecise = myoverflow = myerror_memory = false;
			myclip_string = myclip_name = false;
			}
		memory_node* create() {
			myerror_memory = true;
			auto r = static_cast<memory_node*>(mymemory->allocate_with_undo(sizeof(memory_node))); // throws??
			myerror_memory = r == 0;
			return r;
			}
 		bool is_space(char8_t a) const {
 			// no unicode space, only these are allowed
 			return
 				a == 0x20 || // space
 				a == 0x0a || // lf
 				a == 0x0d || // cr
 				a == 0x09; // tab
 			}
 		template<unsigned size_> bool is(char const (&a)[size_]) {
 			if(myend - myat < static_cast<ptrdiff_t>(size_ - 1))
 				return false;
 			unsigned i = 0;
 			do {
 				if(myat[i] != static_cast<char8_t>(a[i]))
 					return false;
 				} while(++i != size_ - 1);
			myat += size_ - 1;
 			return true;
 			}
 		void skip_space() {
 			while(myat != myend && is_space(*myat)) ++myat;
 			}
 		char8_t* parse() {
			char8_t *r = myat;
			memory_node
				*previous = 0, // 0 if first inside object/array
				*in = 0; // inside non-empty object/array
			char8_t
				*name_begin = 0, // object name. empty name is fine
				*name_end = 0;
			while(myat != myend) {
				skip_space();
				if(myat == myend)
					break;
				r = myat;
				memory_node current = {};
				if(name_begin) {
					current.name = name_begin;
					if(string_size_clip(current.name_size, name_begin, name_end))
						myclip_name = true;
					}
				name_begin = name_end = 0;
				if(*myat == '"') {
					// "hello"
					// "hello": something (if inside object)
					char8_t
						*begin = ++myat,
						*end = read_string(myat, myend);
					if(!end)
						return myat; // this is where it went wrong
					// if in object, this must be a name followed by a :
					if(in && in->type == type::object && !current.name) {
						skip_space();
						if(myat == myend || *myat != ':')
							return r;
						++myat;
						name_begin = begin;
						name_end = end;
						continue;
						}
					current.type = type::string;
					current.string = begin;
					if(string_size_clip(current.size, begin, end))
						myclip_string = true;
 					}
				else if(in && in->type == type::object && !current.name) // inside object, must be "name":value
					return r;
				else if(*myat == '[' || *myat == '{') {
					current.type = *myat == '[' ? type::array : type::object;
					++myat;
					skip_space();
					if(myat == myend)
						return r;
					bool empty = current.type == type::array ? *myat == ']' : *myat == '}';
					if(!empty) {
						// go deeper
						memory_node *n = create();
						if(!n)
							return 0;
						*n = current;
						n->previous = previous;
						if(n->in = in) {
							if(in->size == static_cast<uint32_t>(-1))
								return 0;
							++in->size;
							}
						in = n;
						previous = 0;
						continue;
						}
					++myat;
					}
				else if(*myat == '-' || ('0' <= *myat && *myat <= '9')) {
					number n;
					if(!read_number(n, myat, myend))
						return r;
					current.type = type::number;
					current.extra = n.imprecise();
					current.exponent = n.exponent();
					current.integer = n.integer();
					myimprecise = myimprecise || n.imprecise();
					myoverflow = myoverflow || n.overflow();
					}
				else if(is("true")) {
					current.type = type::boolean;
					current.boolean = true;
					}
				else if(is("false")) {
					current.type = type::boolean;
					current.boolean = false;
					}
 				else if(!is("null")) // current is already null
					return r;
				
				memory_node *n = create();
				if(!n)
					return 0;
				*n = current;
				n->previous = previous;
				previous = n;
				if(n->in = in) {
					if(in->size == static_cast<uint32_t>(-1))
						return 0;
					++in->size;
					// after a value inside array/object: must be one of , ] }
					// if it was close ] } keep looking for more
					do {
						skip_space();
						if(myat == myend)
							return r;
						if(*myat == ',') {
							++myat;
							break;
							}
						else if(*myat == (in->type == type::array ? ']' : '}')) {
							++myat;
							in->nodes = static_cast<memory_node**>(mymemory->allocate_with_undo(sizeof(memory_node**) * in->size, alignof(memory_node**)));
							if(!in->nodes)
								return 0;
							auto i = in->nodes + in->size;
							do {
								auto n = previous;
								previous = previous->previous;
								*--i = n;
								n->me = {};
								n->me.at = static_cast<uint32_t>(i - in->nodes);
								if(n->type == type::array || n->type == type::object)
									n->me.capacity = n->size;
								} while(i != in->nodes);
							previous = in;
							in = in->in;
							}
						else
							return r;
						} while(in);
					}
				}
			
			if(!in && previous && !previous->previous) {
				my = previous;
				my->me = {};
				if(my->type == type::array || my->type == type::object)
					my->me.capacity = my->size;
				mymemory->no_undo();
				return myend;
				}
			
			return r;
 			}
	};

template<typename allocator_> read<memory<allocator_>>
 read_to_memory(memory<allocator_>& a) {
	return {a};
	}

}}
#endif
