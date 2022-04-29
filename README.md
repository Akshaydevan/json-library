# Json library
##### a simple and basic json library written in c++

## Example
```
#include <libjson/json.hpp>

int main() {
    std::string jsonInput = R"({ "one" : 1, "bool" : true})";

    auto json = js::read(jsonInput);

    int number      =    json["one"].asNum();       //returns 1
    bool boolean    =    json["bool"].asBool();     // returns true
    
    return 0;
}
```

## Build
After cloning this repository add this to your cmake

```
add_subdirectory (path_to_cloned_repository)

...

target_include_directories(TARGET PUBLIC path_to_cloned_repository/include)
target_link_libraries (TARGET libjson)
```

## Run Test
```
mkdir build
cd build
cmake .. -DBUILD_TESTING=ON
make test
```
note that some tests can fail as a result of supporting extensions.