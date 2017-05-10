// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_LOGS_BUFFER_FOREVER_HPP
#define WATER_LOGS_BUFFER_FOREVER_HPP
#include <water/logs/buffer.hpp>
#include <water/threads/spin_once.hpp>
namespace water { namespace logs {

// like buffer but no destructor

template<typename output_, typename tag_, bool memory_statistics_ = false> class
 buffer_forever {
 	using buffer = logs::buffer<output_, tag_, memory_statistics_>;
 	public:
 		using output_type = output_;
 		using tag_type = tag_;
 		using piece_type = logs::piece<tag_type>;
 		using write_type = write_to_buffer<buffer_forever<output_, tag_, memory_statistics_>>;
 	private:
 		size_t
 			mypiecesize,
 			myblocksize;
 		threads::spin_once<> myonce;
 		char mybuffer[sizeof(buffer)] {};
 	public:
 		constexpr buffer_forever(size_t piece_size = 0, size_t memory_block_size = 0) :
 			mypiecesize(piece_size),
 			myblocksize(memory_block_size)
 			{}
 		buffer_forever(buffer_forever const&) = delete;
		buffer_forever& operator=(buffer_forever const&) = delete;
 		template<typename iterator_> bool operator()(tag_type const& tag, iterator_ begin, iterator_ end) {
 			return write_type(*this, tag)(begin, end);
 			}
 		template<typename iterator_> bool operator()(tag_type const& tag, iterator_ begin, size_t size) {
 			return write_type(*this, tag)(begin, size);
 			}
 		bool operator()(tag_type const& tag, char const* cstring) {
 			return write_type(*this, tag)(cstring);
 			}
 		template<typename range_> if_range<range_ const, bool> operator()(tag_type const& tag, range_ const& a) {
 			return write_type(*this, tag)(a);
 			}
 		template<typename iterator_> bool operator()(iterator_ begin, iterator_ end) {
 			return (*this)(tag_type{}, begin, end);
 			}
 		template<typename iterator_> bool operator()(iterator_ begin, size_t size) {
 			return (*this)(tag_type{}, begin, size);
 			}
 		bool operator()(char const* cstring) {
 			return (*this)(tag_type{}, cstring);
 			}
 		template<typename range_> if_range<range_ const, bool> operator()(range_ const& a) {
 			return (*this)(tag_type{}, a);
 			}
 		piece_type* piece(tag_type const& tag, piece_type *list = 0) {
 			return get()->piece(tag, list);
 			}
 		void dont_write(piece_type *list) noexcept {
 			get()->dont_write(list);
 			}
 		void write(piece_type *list) noexcept {
 			get()->write(list);
 			}
 		write_type write(tag_type const& tag = {}) {
 			return {*this, tag};
 			}
 		void flush() {
 			return get()->flush();
 			}
 		void flush_if_not_flushing() {
 			return get()->flush_if_not_flushing();
 			}
 		buffer_statistics statistics() {
 			return get()->statistics();
 			}
 		fixed::memory_atomic_statistics memory_statistics() {
 			return get()->memory_statistics();
 			}
 	private:
 		friend struct once;
 		struct once {
 			buffer_forever *my;
 			void operator()() {
 				new(here(my->mybuffer)) buffer(my->mypiecesize, my->myblocksize);
 				}
 			};
 		buffer* get() {
 			myonce(once{this});
 			return static_cast<buffer*>(static_cast<void*>(mybuffer));
 			}
 	};

}}
#endif
