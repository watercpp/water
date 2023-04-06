// Copyright 2023 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_LOGS_OUTPUT_TO_TRACE_HPP
#define WATER_LOGS_OUTPUT_TO_TRACE_HPP
#include <water/logs/bits.hpp>
#include <water/trace.hpp>
#include <water/str/begin_end.hpp>
namespace water { namespace logs {

/*

Output class for buffer/buffer_forever that writes to water::trace.

The tag needs a water::str << operator. 

*/

class output_to_trace
{
public:

    void start() noexcept {
        // flusing starts
    }
    
    template<typename tag_>
    char* prefix(char* begin, char* end, tag_ const& tag) noexcept {
        auto to = str::to_begin_end(begin, end);
        to << tag;
        if(to.end() != begin)
            to << ' ';
        return to.end();
    }
    
    void line(char const* begin, char const* /*end*/) noexcept {
        trace(begin);
    }
    
    void stop() noexcept {
        // flushing stops
    }
};

}}
#endif
