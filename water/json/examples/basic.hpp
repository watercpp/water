// Copyright 2018-2021 Johan Paulsson
// This file is part of the Water C++ Library. It is licensed under the MIT License.
// See the license.txt file in this distribution or https://watercpp.com/license.txt
//\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_/\_
#ifndef WATER_JSON_EXAMPLES_BASIC_HPP
#define WATER_JSON_EXAMPLES_BASIC_HPP
#include <water/json/json.hpp>
#include <water/vector.hpp>
namespace water { namespace json { namespace examples { namespace basic {

/*

Basic example of reading a list of locations from JSON into a simple data structure,
and writing that data structure as JSON text.

This example does not do any validation of the JSON input. A real program would check
if the latitude and longitude values are valid, handle longer names, check if the JSON
was missing any required data or contained any unrecognized data.

*/

char constexpr text[] = R"###({
    "locations": [
        {
            "name": "North Pole",
            "latitude": 90,
            "longitude": 0,
            "wildlife": {
                "penguin": false,
                "giant_squid": false
            }
        },
        {
            "name": "South Pole",
            "latitude": -90,
            "longitude": 0,
            "wildlife": {
                "penguin": true,
                "giant_squid": false
            }
        },
        {
            "name": "Mariana Trench",
            "latitude": 17.7500,
            "longitude": 142.5000,
            "wildlife": {
                "giant_squid": true
            }
        },
        {
            "name": "Mount Everest",
            "latitude": 27.986065,
            "longitude": 86.922623,
            "wildlife": {}
        }
    ]
})###";

class location
{
    char myname[64];
    size_t myname_size = 0;
    double mylatitude = 0;
    double mylongitude = 0;
    bool mypenguin = false;
    bool mygiant_squid = false;

public:
    json::string<char const*> name() const {
        return {myname, myname_size};
    }

    template<typename iterator_>
    void name(iterator_ begin, iterator_ end) {
        myname_size = 0;
        while(begin != end && myname_size != sizeof(myname))
            myname[myname_size++] = *begin++;
    }

    double latitude() const {
        return mylatitude;
    }

    void latitude(double a) {
        mylatitude = a;
    }

    double longitude() const {
        return mylongitude;
    }

    void longitude(double a) {
        mylongitude = a;
    }

    bool penguin() const {
        return mypenguin;
    }

    void penguin(bool a) {
        mypenguin = a;
    }

    bool giant_squid() const {
        return mygiant_squid;
    }

    void giant_squid(bool a) {
        mygiant_squid = a;
    }
};


inline vector<location> read_from_json_text(char const* text_begin, char const* text_end) {
    json::memory<> memory;
    auto root = read_to_memory(memory)(text_begin, text_end).nodes();
    
    vector<location> locations;
    
    for(auto from : root["locations"]) {
        location to;
        auto name = from["name"].string();
        to.name(name.begin(), name.end());
        to.latitude(from["latitude"].number().to_double());
        to.longitude(from["longitude"].number().to_double());
        // if the JSON does not contain wildlife.penguin the boolean values will be false
        to.penguin(from["wildlife"]["penguin"].boolean());
        to.giant_squid(from["wildlife"]["giant_squid"].boolean());
        locations.push_back(to);
    }
    
    return locations;
}


inline vector<char> write_as_json_text(vector<location> const& locations) {
    json::memory<> memory;
    auto root = memory.create().object(1);
    
    auto array = root.push_back().name("locations").array(locations.size());
    for(auto const& from : locations) {
        auto to = array.push_back().object(3);
        to.push_back().name("name").string(from.name().begin(), from.name().end());
        to.push_back().name("latitude").number(from.latitude());
        to.push_back().name("longitude").number(from.longitude());
        auto wildlife = to.push_back().name("wildlife").object(0);
        wildlife.push_back().name("penguin").boolean(from.penguin());
        wildlife.push_back().name("giant_squid").boolean(from.giant_squid());
    }
    
    vector<char> text;
    write(
        [&text](char const* begin, char const* end) {
            text.insert(text.end(), begin, end);
        },
        root
    );
    return text;
}


inline vector<char> example() {
    auto read = read_from_json_text(text, text + sizeof(text) - 1);
    if(read.empty())
        return {};
    return write_as_json_text(read);
}


}}}}
#endif
