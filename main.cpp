#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <assert.h>
#include "OYAJSon/OYAJSon.h"


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


void Test01_CreateInstance(){
    std::cout << "TEST 01: Create OYAJSon Instance" << std::endl;
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

    std::cout << "\tTesting root item size ... ";
    assert(v.size() == 6);
    std::cout << "Success!" << std::endl;

    std::cout << "\tTesting keys, type identification, and value access ... ";
    assert(v.has_key("bool_val"));
    assert(v["bool_val"].is(OYAJSon::JSonType_Bool));
    assert(v["bool_val"].get<bool>());
    std::cout << "Success!" << std::endl;

    std::cout << "\tTesting second level JSon Object access ... ";
    assert(v.has_key("obj_val"));
    assert(v["obj_val"].is(OYAJSon::JSonType_Object));
    assert(v["obj_val"].size() == 2);
    assert(v["obj_val"].has_key("Value1"));
    assert(v["obj_val"]["Value1"].get<int>() == 100);
    std::cout << "Success!" << std::endl;

    std::cout << "\tTesting second level JSon Array access ... ";
    assert(v.has_key("arr_val"));
    assert(v["arr_val"].is(OYAJSon::JSonType_Array));
    assert(v["arr_val"].size() == 3);
    assert(v["arr_val"][1].is(OYAJSon::JSonType_Bool));
    assert(v["arr_val"][1].get<bool>());
    std::cout << "Success!" << std::endl;

    std::cout << "\tTEST COMPLETE" << std::endl << std::endl;
}


void Test02_ParseInstance(){
    std::cout << "TEST 02: Create OYAJSon Instance via Parsed String" << std::endl;
    char* test = "{"
        "\"Key_A\":\"Hello\","
        "\"Key_B\":\"World\","
        "\"Key_C\":4.835,"
        "\"Key_D\":["
            "true,"
            "100,"
            "\"Some String\","
            "null"
        "]}";
    OYAJSon::JSonValue v;
    v.parse(std::string(test));

    std::cout << "\tTesting size ... ";
    assert(v.size() == 4);
    std::cout << "Success!" << std::endl;

    std::cout << "\tTesting all expected keys exist ... ";
    assert(v.has_keys({"Key_A", "Key_B", "Key_C", "Key_D"}));
    std::cout << "Success!" << std::endl;

    std::cout << "\tTesting double number type ... ";
    assert(v["Key_C"].get<double>() == 4.835);
    std::cout << "Success!" << std::endl;

    std::cout << "\tTesting second level array size ... ";
    assert(v["Key_D"].is(OYAJSon::JSonType_Array));
    assert(v["Key_D"].size() == 4);
    std::cout << "Success!" << std::endl;

    std::cout << "\tValidating all JSon Object key types ... ";
    assert(v.is({
        {"Key_A", OYAJSon::JSonType_String},
        {"Key_B", OYAJSon::JSonType_String},
        {"Key_C", OYAJSon::JSonType_Number},
        {"Key_D", OYAJSon::JSonType_Array}
    }));
    std::cout << "Success!" << std::endl;

    std::cout << "\tValidating all JSon Array types ... ";
    assert(v["Key_D"].is({
        OYAJSon::JSonType_Bool,
        OYAJSon::JSonType_Number,
        OYAJSon::JSonType_String,
        OYAJSon::JSonType_Null
    }));
    std::cout << "Success!" << std::endl;

    std::cout << "\tTEST COMPLETE" << std::endl << std::endl;
}


void Test03_ObjectViaSubscription(){
    std::cout << "TEST 03: Fill JSon Object via subscription." << std::endl;

    std::cout << "\tCreating JSonValue of type JSonType_Object ... " << std::endl;
    OYAJSon::JSonValue jObj(OYAJSon::JSonType_Object);
    jObj["Key1"] = 42;
    jObj["Key2"] = 3.14159;
    jObj["Key3"] = true;
    jObj["Key4"] = nullptr;
    jObj["Key5"] = OYAJSon::JSonValue(OYAJSon::Array{
        1,
        "String",
        false
    });

    std::cout << "\tTesting size ... ";
    assert(jObj.size() == 5);
    std::cout << "Success!" << std::endl;

    std::cout << "\tTesting Key was created ... ";
    assert(jObj.has_key("Key3"));
    std::cout << "Success!" << std::endl;

    std::cout << "\tTesting Key is of expected type ... ";
    assert(jObj["Key5"].is(OYAJSon::JSonType_Array));
    std::cout << "Success!" << std::endl;

    std::cout << "\tTEST COMPLETE" << std::endl << std::endl;
}


int main()
{
    std::cout << "Tests for ObsidianBlk's Yet Another JSon library (" << OYAJSON_VERSION << ")" << std::endl;
    std::cout << "-------------------------------------------" << std::endl;
    Test01_CreateInstance();
    Test02_ParseInstance();
    Test03_ObjectViaSubscription();

    std::cout << "------------ All Test Completed ------------" << std::endl << std::endl;
    return 0;
}
