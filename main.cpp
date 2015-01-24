#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include "src/OYAJSon.h"


std::string load_file(const std::string &src){
    std::ifstream f(src);
    std::string::value_type buffer[1024];
    std::string::size_type bufread;
    std::string result = "";

    if (f.is_open()){
        while (f){
            f.read(buffer, 1024);
            bufread = f.gcount();
            result.insert(result.size(), buffer, bufread);
        }
    }
    f.close();
    return result;
}


void Test01_CreateObject(){
    //OYAJSon::JSonValue v(OYAJSon::Object{{"value1", 5}, {"Value2","String"}, {"Value3",true}});
    OYAJSon::JSonValue v(OYAJSon::Object{
        {"num_val", 5},
        {"str_val", "this is a test string"},
        {"bool_val", true},
        {"null_val", nullptr},
        {"obj_val", OYAJSon::Object{
            {"Value1", 100},
            {"Value2", "String at depth 2"}
        }},
        {"arr_val", OYAJSon::Array{1, true, "String in Array"}}
    });
    std::cout << v.serialize("\t") << std::endl;
    //OYAJSon::JSonValue jval = OYAJSon::Object().insert({"Value1", OYAJSon::JSonValue(5)}, {"Value2", OYAJSon::JSonValue("string")}, {"Value3", OYAJSon::JSonValue(true)});
}

int main()
{
    Test01_CreateObject();
}
