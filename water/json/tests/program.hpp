// Copyright 2017-2018 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_JSON_TESTS_PROGRAM_HPP
#define WATER_JSON_TESTS_PROGRAM_HPP
#include <water/json/tests/bits.hpp>
#include <water/allocator.hpp>
#include <water/vector.hpp>
#include <water/later.hpp>
#include <water/str/str.hpp>
#include <water/str/out_stdout.hpp>
#ifdef WATER_NO_CHEADERS
    #include <stdio.h>
#else
    #include <cstdio>
#endif
namespace water { namespace json { namespace tests {
namespace program {
/*

A simple command line program for manual testing. It opens a file, indents, parses, writes, parses
the written again, and outputs some statistcs.

To build it, make a main.cpp like this:

#include <water/json/tests/program.hpp>
int main(int argc, char **argv) {
    return water::json::tests::program::main(argc, argv);
}

*/

#ifndef WATER_NO_CHEADERS
using std::fopen;
using std::fclose;
using std::fseek;
using std::ftell;
using std::fread;
#endif

inline bool equal_cstrings(char const* a, char const* b) {
    if(!a || !b) return a == b;
    while(*a && *b && *a == *b) {
        ++a;
        ++b;
    }
    return *a == *b;
}

inline int main(int argc, char const*const* argv) {
    str::out_stdout_simple_buffer out;
    char const *program_name = "json_program";
    auto
        arg_at = argv,
        arg_end = argv + argc;
    if(arg_at != arg_end)
        program_name = *arg_at++;
    bool quiet = false;
    if(arg_at != arg_end && equal_cstrings(*arg_at, "-quiet")) {
        quiet = true;
        ++arg_at;
    }
    if(arg_at == arg_end) {
        out << "use:\n"
            << program_name << " file1.json file2.json ...\n"
            << program_name << " -quiet file1.json file2.json ...\n"
            << "\n";
        return 0;
    }
    json::memory<water::allocator> memory;
    size_t
        count = 0,
        count_success = 0,
        different = 0,
        imprecise = 0,
        overflow = 0,
        reparse_errors = 0,
        file_errors = 0;
    water::vector<char>
        write1,
        write2,
        write3;
    while(arg_at != arg_end) {
        ++count;
        // read the file
        auto file_path = *arg_at++;
        out << file_path << "\n\n";
        memory.clear();
        void *buffer = 0;
        size_t buffer_size = 0;
        if(auto file = fopen(file_path, "r")) {
            auto close = water::later([file]{ fclose(file); });
            long file_size;
            if(!fseek(file, 0, SEEK_END) && (file_size = ftell(file)) > 0 && !fseek(file, 0, SEEK_SET))
                if(static_cast<unsigned long>(file_size) < static_cast<size_t>(-1)) // < instead of <= avoids compiler warning maybe?
                    buffer_size = static_cast<size_t>(file_size);
            if(buffer_size) {
                void *b = memory.allocate(buffer_size);
                if(b && fread(b, 1, buffer_size, file) == buffer_size)
                    buffer = b; // success
            }
        }
        if(!buffer) {
            ++file_errors;
            out << "error: read file " << file_path << "\n";
        }
        else {
            auto file = string_from(static_cast<char*>(buffer), buffer_size);
            if(!quiet) {
                out << "indented:\n";
                json::indent_to(out)(file.begin(), file.end());
                out << "\n\n";
                out << "minified:\n";
                json::indent_to(out)(file.begin(), file.end(), 0);
                out << "\n\n" << str::flush;
            }
            auto read = read_to_memory(memory);
            if(read.parse_in_place(static_cast<char*>(buffer), buffer_size)) {
                out << "parse success!\n"
                    << "memory total size .. " << memory.total_size() << "\n"
                    << "memory allocated now " << memory.allocated_now() << "\n"
                    << "memory blocks ...... " << memory.blocks() << "\n"
                    << "allocated / filesize " << (memory.allocated_now() / static_cast<double>(buffer_size)) << "\n" // buffer_size is never 0
                    << "imprecise_numbers .. " << read.imprecise_numbers() << '\n'
                    << "numbers_did_overflow " << read.numbers_did_overflow() << '\n'
                    << "string_was_cut ..... " << read.string_was_cut() << '\n'
                    << "object_name_was_cut  " << read.object_name_was_cut() << "\n\n";
                imprecise += read.imprecise_numbers();
                overflow += read.numbers_did_overflow();
            
                write1.clear();
                json::write([&write1](char const* b, char const* e) { write1.insert(write1.end(), b, e); }, read.nodes(), false);
                if(quiet)
                    out << "escape minimal size: " << write1.size() << "\n";
                else
                    out << "escape minimal:\n" << write1 << "\n";
                
                write2.assign(write1.begin(), write1.end());
                auto reread = read_to_memory(memory);
                if(!reread.parse_in_place(write2.begin(), write2.end())) {
                    out << "error: reparse minimal!\n\n";
                    ++reparse_errors;
                }
                else {
                    write3.clear();
                    json::write([&write3](char const* b, char const* e) { write3.insert(write3.end(), b, e); }, reread.nodes(), false);
                    if(write1 != write3) {
                        ++different;
                        if(quiet)
                            out << "error: reparse different!\n";
                        else
                            out << "error: reparse different!\n" << write3 << "\n\n";
                    }
                }
            
                write1.clear();
                json::write([&write1](char const* b, char const* e) { write1.insert(write1.end(), b, e); }, read.nodes(), true);
                if(quiet)
                    out << "escape all size:" << write1.size() << "\n";
                else
                    out << "escape all:\n" << write1 << "\n\n";
            
                write2.assign(write1.begin(), write1.end());
                if(!reread.parse_in_place(write2.begin(), write2.end())) {
                    out << "error: reparse escape all!\n\n";
                    ++reparse_errors;
                }
                else {
                    write3.clear();
                    json::write([&write3](char const* b, char const* e) { write3.insert(write3.end(), b, e); }, reread.nodes(), true);
                    if(write1 != write3) {
                        ++different;
                        if(quiet)
                            out << "error: reparse different!\n";
                        else
                            out << "error: reparse different!\n" << write3 << "\n\n";
                    }
                }
                ++count_success;
            }
            else if(read.allocation_failed()) // impossible here because water::allocator throws
                out << "parse: out of memory\n";
            else if(read.parse_error()) {
                if(quiet)
                    out << "error: parse\n";
                else
                    out << "error: parse \n" << read.parse_error() << "\n";
            }
            else
                out << "error: parse total failure\n";
        }
        out << "\n\n-------------------------------------------------------------------\n" << str::el;
    }
    size_t errors = (count - count_success) + reparse_errors + different + file_errors;
    if(count)
        out << "different after reparse " << different << "\n"
            << "imprecise_numbers ..... " << imprecise << "\n"
            << "numbers_did_overflow .. " << overflow << "\n"
            << "success ............... " << count_success << "\n"
            << "parse errors .......... " << (count - count_success)  << "\n"
            << "reparse errors ........ " << reparse_errors << "\n"
            << "file read errors ...... " << file_errors << "\n"
            << "errors ................ " << errors << "\n\n";
    return errors == 0 ? 0 : 1;
}

}
}}}
#endif
