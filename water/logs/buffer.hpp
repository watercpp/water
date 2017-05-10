// Copyright 2017 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_LOGS_BUFFER_HPP
#define WATER_LOGS_BUFFER_HPP
#include <water/logs/piece.hpp>
#include <water/logs/write_to_buffer.hpp>
#include <water/new_here.hpp>
#include <water/fixed/memory_atomic.hpp>
#include <water/threads/mutex.hpp>
#include <water/allocator.hpp>
#include <water/unicode/utf.hpp>
#include <water/atomic_old/atomic.hpp>
namespace water { namespace logs {

/*

Buffering log

Threads writing to the log use the almost lock free water::fixed::memory_atomic to allocate pieces
and a totally lock free queue in this when adding those pieces to the buffer.

The pieces are fixed size character arrays, one log message can be written to many pieces.

Writing to this log should be relativley fast, and it should almost never block. The time writing
can block is when fixed::memory_atomic needs to allocate a new block.

A thread should call flush() from time to time to write the buffer to the output. This is assumed to
be slow, otherwise using this buffer is pointless. One solution is to have a low priority background
thread for this. Before and after the programs main(), when the background thread is not running,
the main thread could call flush() directly after writing to the buffer or not use the buffer at all. 

The statistics can be used to figure out how much memory is needed, and how big the pieces should
be for the fewest possible allocations and minimal memory use. 

This expects everything to be UTF-8.

template
- output_ see below
- tag_ is passed through to the output. could be used to hold thread id, log level, message type
- memory_statistics_ true to collect water::fixed::memory_atomic statistcs

class output {
  public:
    output() constexpr;
    char* prefix(char* begin, char *end, tag_type tag);
    void line(char const* begin, char const* end);
    void start();
    void stop();
  };

The constructor must be constexpr.
All functions except the constructor are called while a mutex is locked.
Any function may throw.

start()
called when flush starts. this can be used for per-flush setup or to do one-time initialization of something

char* prefix(char* begin, char* end, tag_type tag)
write a line prefix to begin,end, return end of what was written

line(char const* begin, char const* end)
write a line to the output destination.
begin < end always
end[-1] == linebreak always
end[0] == 0 always

stop()
called when flush is done, can be used to flush the output destination.
called even if an exception is thown after start.
not called when start() throws

*/

unsigned constexpr
	buffer_piece_size = 128,
	buffer_line_max = 1024; // beyond this will split to two lines

struct buffer_statistics {
	using large_t = uint_largest_t;
	large_t
		piece_total_characters_used,
		piece_total_characters_size;
	size_t
		piece_count,
		flush_max_pieces,
		message_count,
		message_count_using_half_piece,
		message_count_using_one_piece,
		message_count_using_two_pieces,
		message_count_using_three_pieces,
		message_count_using_more_pieces,
		message_longest,
		line_count,
		line_longest, // this assumes prefix uses all reserved characters
		line_splits; // when a message was split into two lines, per line, not per message
	};

namespace _ {
	template<typename unsigned_> void buffer_statistics_add(unsigned_& to) {
		if(to != static_cast<unsigned_>(-1)) ++to;
		}
	template<typename unsigned_> void buffer_statistics_add(unsigned_& to, typename types::type<unsigned_>::result add) {
		if(to < static_cast<unsigned_>(-1) - add) to += add;
		else to = static_cast<unsigned_>(-1);
		}
	}

template<typename output_, typename tag_, bool memory_statistics_ = false> class
 buffer {
 	public:
 		using output_type = output_;
 		using tag_type = tag_;
 		using piece_type = logs::piece<tag_type>;
 		using write_type = write_to_buffer<buffer<output_, tag_, memory_statistics_>>;
 	private:
 		fixed::memory_atomic<void, memory_statistics_> mymemory;
 		atomic_old::alias<piece_type*> mylist {};
 	private:
 		// below locked
 		threads::mutex<threads::need_constexpr_constructor> mylock;
 		output_type myoutput;
 		buffer_statistics mystatistics{};
 	public:
 		constexpr buffer(size_t piece_size = 0, size_t memory_block_size = 0) :
 			mymemory(sizeof(piece_type) + (piece_size ? piece_size : buffer_piece_size), memory_block_size)
 			{}
 		buffer(buffer const&) = delete;
		buffer& operator=(buffer const&) = delete;
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
 			void *a = mymemory.allocate();
 			if(!a) return 0;
 			auto r = new(here(a)) piece_type(tag, mymemory.bytes() - sizeof(piece_type));
 			r->list(list);
 			return r;
 			}
 		void dont_write(piece_type *list) noexcept {
 			while(list) {
 			 	auto free = list;
 			 	list = list->list();
 			 	mymemory.free(free);
 				}
 			}
 		void write(piece_type *list) noexcept {
 			if(!list)
 				return;
 			piece_type *first = list;
 			while(first->list()) {
 				first->first(false);
 				first = first->list();
 				}
 			first->first(true);
 			piece_type *now = atomic_old::get<atomic_old::none>(mylist);
 			do first->list(now); while(!atomic_old::compare_set_else_non_atomic_get(mylist, now, list, now));
 			}
 		write_type write(tag_type const& tag = {}) {
 			return {*this, tag};
 			}
 		void flush() {
 			if(atomic_old::get<atomic_old::acquire>(mylist)) {
 				auto lock = lock_move(mylock);
				flush_locked();
				}
 			}
 		void flush_if_not_flushing() {
 			// flush only if nobody else is flushing right now
 			// use flush() if it's important that the message just written by the current thread is flushed 
 			if(atomic_old::get<atomic_old::acquire>(mylist) && try_lock(mylock)) {
 				auto unlock = unlock_move(mylock);
 				flush_locked();
 				}
 			}
 		buffer_statistics statistics() {
 			auto unlock = lock_move(mylock);
 			return mystatistics;
 			}
 		fixed::memory_atomic_statistics memory_statistics() {
 			return mymemory.statistics();
 			}
 	private:
 		void flush_locked() {
 			// reverse list (make oldest first)
 			// then write one line at a time, end a "run" with newline if it does not, run ends when the next piece is "first"
 			// free each line at once
 			// do not write 0 characters, stop the current run of pieces at the first 0
 			auto piece = atomic_old::get_set(mylist, 0); // must pop locked to preserve order
 			if(!piece)
 				return;
			piece = reverse(piece);
			statistics_locked(piece);
			struct auto_free_ {
				piece_type *piece;
				buffer *me;
				~auto_free_() { me->dont_write(piece); }
				} auto_free{piece, this};
			myoutput.start(); // throws??
			struct auto_stop_ {
				output_type *output;
				~auto_stop_() { output->stop(); }
				} auto_stop{&myoutput};
			char
				line[buffer_line_max],
				*to = line,
				*begin = line, // after prefix
				*end = line + sizeof(line) - 2, // always room for endline + 0
				*piece_at = 0;
			bool
				new_line = true,
				zero = false; // write everything up to the first zero
			unsigned leftover = 0;
			tag_type tag{piece->tag()}; // copy, no default construct
			do {
				bool new_piece = piece_at == 0;
				if(new_piece)
					piece_at = piece->begin();
				if(new_line) {
					if(new_piece && piece->first()) {
						tag = piece->tag();
						begin = myoutput.prefix(line + 0, line + sizeof(line) / 2, tag);
						leftover = 0; // drop half utf-8
						}
					auto from = to;
					to = begin;
					while(leftover) {
						*to++ = *from++;
						--leftover;
						}
					new_line = zero = false;
					}
				if(!zero) {
					while(to != end && piece_at != piece->end() && !(zero = *piece_at == 0) && !(new_line = *piece_at == '\n'))
						*to++ = *piece_at++;
					if(new_line)
						++piece_at;
					else if(to == end) {
						new_line = true;
						if(piece_at != piece->end() && *piece_at == '\n') // if to == end exactly at a linebreak, dont linebreak twice
							++piece_at;
						else {
							_::buffer_statistics_add(mystatistics.line_splits);
							// backwards so it does not split inside a utf-8 sequence
							// worst case is it ends in 3 out of a 4 sequence, and the sequence before is 4, then it needs to go back 7
							unsigned utf8 = 0;
							while(to != begin && to > end - 8) {
								if((utf8 = unicode::utf8_first_of(static_cast<unsigned char>(*--to))) && static_cast<unsigned>(end - to) >= utf8) {
									to += utf8;
									break;
									}
								utf8 = 0;
								--to;
								}
							if(!utf8)
								to = end; // does not look like utf-8
							else
								leftover = static_cast<unsigned>(end - to); // can be 0
							}
						}
					}
				if(new_line || !piece->list() || piece->list()->first()) {
					if(to != begin || new_line) {
						// write it!
						char to_keep[] = { to[0], to[1] }; // for leftover
						to[0] = '\n';
						to[1] = 0;
						myoutput.line(static_cast<char const*>(line), static_cast<char const*>(to + 1));
						to[0] = to_keep[0];
						to[1] = to_keep[1];
						_::buffer_statistics_add(mystatistics.line_count);
						if(mystatistics.line_longest < static_cast<size_t>(to - line))
							mystatistics.line_longest = static_cast<size_t>(to - line);
						}
					new_line = true;
					}
				if(zero || piece_at == piece->end()) {
					piece_at = 0;
					auto free = piece;
					piece = auto_free.piece = piece->list();
					mymemory.free(free);
					}
				} while(piece);
 			}
 		void statistics_locked(piece_type *piece) {
 			___water_threads_statistics(
 				if(!mystatistics.piece_count)
 					name_if(statistics_pointer(mylock), "water::logs::buffer");
 				)
 			size_t
 				message_pieces = 0,
 				message_length = 0,
 				count = 0;
 			while(piece) {
 				++count;
 				_::buffer_statistics_add(mystatistics.piece_total_characters_used, piece->size());
 				_::buffer_statistics_add(mystatistics.piece_total_characters_size, piece->capacity());
 				_::buffer_statistics_add(mystatistics.piece_count);
 				if(piece->first()) {
 					message_pieces = 0;
 					message_length = 0;
 					_::buffer_statistics_add(mystatistics.message_count);
 					}
 				++message_pieces;
 				message_length += piece->size();
 				if(!piece->list() || piece->list()->first()) {
 					if(mystatistics.message_longest < message_length)
 						mystatistics.message_longest = message_length;
 					switch(message_pieces) {
 						case 1:
	 						if(piece->size() <= piece->capacity() / 2)
 								_::buffer_statistics_add(mystatistics.message_count_using_half_piece);
 							else
 								_::buffer_statistics_add(mystatistics.message_count_using_one_piece);
 							break;
 						case 2:
 							_::buffer_statistics_add(mystatistics.message_count_using_two_pieces);
 							break;
 						case 3:
 							_::buffer_statistics_add(mystatistics.message_count_using_three_pieces);
 							break;
 						default:
 							_::buffer_statistics_add(mystatistics.message_count_using_more_pieces);
 						}
 					}
 				piece = piece->list();
 				}
 			if(mystatistics.flush_max_pieces < count)
 				mystatistics.flush_max_pieces = count;
 			}
 	};

}}
#endif
