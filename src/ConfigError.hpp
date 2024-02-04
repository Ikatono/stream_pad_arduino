#ifndef H_BA0C3ED91D9B4CAAB2D8C6FBCD1A1C07
#define H_BA0C3ED91D9B4CAAB2D8C6FBCD1A1C07

namespace Configuration
{
    enum class ConfigError
    {
        None = 0,
        deserialization_error   = 1,
        serialization_error     = 2,
    };
    //prefered over using error = ConfigError::None directly in case other non-error values are added
    bool is_error(ConfigError error);
}

#endif //H_BA0C3ED91D9B4CAAB2D8C6FBCD1A1C07
