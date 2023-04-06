// Copyright 2023 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#include <water/logs/examples/global.hpp>
#include <water/trace.hpp>
#include <cstdio>
namespace water { namespace logs { namespace examples {

/*

See global.hpp

*/

global_buffer global;


/*

global_output implementation.

This shows how a file can be opened lazily from start().

The output class is always used with a mutex locked, so it does not need a mutex itself.

A better version of this would probably limit the size of the log file, maybe by cycling the file
with another when it reaches a certain size. And it would not try to open the file at every start()
if it cannot be opened.

*/

constexpr char global_file_path[] = "log.log"; // change this

std::FILE *global_file = 0;

void global_output::start() {
    if(!global_file) {
        #ifdef __STDC_SECURE_LIB__
        if(::fopen_s(&global_file, global_file_path, "a"))
            global_file = 0;
        #else
        global_file = std::fopen(global_file_path, "a");
        #endif
    }
}

char* global_output::prefix(char* begin, char* end, water::logs::tag_with_thread const& tag) {
    auto to = str::to_begin_end(begin, end);
    to << tag << ' ';
    return to.end();
}

void global_output::line(char const* begin, char const* end) {
    trace(begin);
    if(global_file)
        std::fwrite(begin, 1, static_cast<size_t>(end - begin), global_file);
}

void global_output::stop() {
    if(global_file)
        std::fflush(global_file);
}


}}}

#if 0

// flush the log from main like this:

void main() {
    water::logs::flush_thread{water::logs::examples::global};
    water::logs::global("Hello world!");
}

#endif
