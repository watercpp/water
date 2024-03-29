// Copyright 2017-2023 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_LOGS_BUFFER_FOREVER_HPP
#define WATER_LOGS_BUFFER_FOREVER_HPP
#include <water/logs/buffer.hpp>
#if defined(WATER_NO_STD) || defined(WATER_USE_WATER_THREADS)
    #include <water/threads/once.hpp>
#else
    #include <mutex>
#endif
namespace water { namespace logs {

// like buffer but no destructor

template<typename output_ = void, typename tag_ = void, bool memory_statistics_ = false>
class alignas(hardware_destructive_interference_size) buffer_forever
{
    using buffer = logs::buffer<output_, tag_, memory_statistics_>;

public:
    using output_type = typename buffer::output_type;
    using tag_type = typename buffer::tag_type;
    using piece_type = typename buffer::piece_type;
    using write_type = write_to_buffer<buffer_forever<output_, tag_, memory_statistics_>>;

private:
    char mybuffer[sizeof(buffer)] {};
    size_t mypiecesize;
    size_t myblocksize;
    
    #if defined(WATER_NO_STD) || defined(WATER_USE_WATER_THREADS)
    threads::once myonce;
    #else
    std::once_flag myonce;
    #endif

public:

    constexpr buffer_forever(size_t piece_size = 0, size_t memory_block_size = 0) :
        mypiecesize(piece_size),
        myblocksize(memory_block_size)
    {}

    buffer_forever(buffer_forever const&) = delete;
    buffer_forever& operator=(buffer_forever const&) = delete;

    template<typename iterator_>
    bool operator()(tag_type const& tag, iterator_ begin, iterator_ end) {
        return write_type(*this, tag)(begin, end);
    }

    template<typename iterator_>
    bool operator()(tag_type const& tag, iterator_ begin, size_t size) {
        return write_type(*this, tag)(begin, size);
    }

    bool operator()(tag_type const& tag, char const* cstring) {
        return write_type(*this, tag)(cstring);
    }

    template<
        typename range_,
        typename = decltype(make_type<range_ const&>().begin() == make_type<range_ const&>().end())
    >
    bool operator()(tag_type const& tag, range_ const& a) {
        return write_type(*this, tag)(a);
    }

    template<typename iterator_>
    bool operator()(iterator_ begin, iterator_ end) {
        return (*this)(tag_type{}, begin, end);
    }

    template<typename iterator_>
    bool operator()(iterator_ begin, size_t size) {
        return (*this)(tag_type{}, begin, size);
    }

    bool operator()(char const* cstring) {
        return (*this)(tag_type{}, cstring);
    }

    template<
        typename range_,
        typename = decltype(make_type<range_ const&>().begin() == make_type<range_ const&>().end())
    >
    bool operator()(range_ const& a) {
        return (*this)(tag_type{}, a);
    }
    
    void concurrent(bool a) noexcept {
        get()->concurrent(a);
    }
    
    bool concurrent() noexcept {
        return get()->concurrent();
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
    
    buffer* get() {
        #if defined(WATER_NO_STD) || defined(WATER_USE_WATER_THREADS)
        myonce(
        #else
        call_once(myonce,
        #endif
            [this]() {
                new(here(this->mybuffer)) buffer(this->mypiecesize, this->myblocksize, false);
            }
        );
        return static_cast<buffer*>(static_cast<void*>(mybuffer));
    }
};

}}
#endif
