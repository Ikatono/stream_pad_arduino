#include "Config.hpp"

namespace Configuration
{
    void Config::append(const Page& new_page)
    {
        pages.push_back(new_page);
    }
}
