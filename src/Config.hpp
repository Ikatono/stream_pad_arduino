#ifndef H_AE095F749CEA4FC29AC38902BC5FD5D1
#define H_AE095F749CEA4FC29AC38902BC5FD5D1

#include <vector>
#include <ArduinoJson.h>
#include "Page.hpp"
#include "ConfigError.hpp"

namespace Configuration
{

    class Config
    {
        std::vector<Page> pages;
        public:
            Page& page(size_t index)
            {
                return pages[index];
            }
            void append(const Page& new_page);
            ConfigError load(JsonDocument);
            
    };
}

#endif //H_AE095F749CEA4FC29AC38902BC5FD5D1
