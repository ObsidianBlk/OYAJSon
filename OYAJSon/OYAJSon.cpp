/*
* The MIT License (MIT)
*
* Copyright (c) 2014-2015 Bryan Miller
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE.
*/

#include "OYAJSon.h"
#include <stdexcept>
#include <regex>

namespace OYAJSon {

    // ------------------------
    // Couple of private function prototypes
    JSonValue _ParseArray(const string_type &src);
    JSonValue _ParseObject(const string_type &src);
    inline string_type _trim(const string_type &s);
    JSonType _DetermineType(const string_type &src);
    string_type _StripCharacters(string_type s, string_type characters);
    // ------------------------ Now back to the show.


    JSonValue::JSonValue() : mDataType(JSonType_Null), mNumberInt(false){_ClearData();}
    JSonValue::JSonValue(std::nullptr_t) : mDataType(JSonType_Null), mNumberInt(false){_ClearData();}
    JSonValue::JSonValue(const JSonValue &value) : mDataType(JSonType_Null), mNumberInt(false){operator=(value);}
    JSonValue::JSonValue(const Object &value) : mDataType(JSonType_Null), mNumberInt(false){operator=(value);}
    JSonValue::JSonValue(const Array &value) : mDataType(JSonType_Null), mNumberInt(false){operator=(value);}
    JSonValue::JSonValue(const string_type& value, bool parse_as_json_str) : mDataType(JSonType_Null), mNumberInt(false){
        if (parse_as_json_str){
            try{
                parse(value);
            } catch (JSonException e){
                throw e;
            }
        } else {
            operator=(value);
        }
    }
    JSonValue::JSonValue(double value) : mDataType(JSonType_Null), mNumberInt(false){operator=(value);}
    JSonValue::JSonValue(int value) : mDataType(JSonType_Null), mNumberInt(false){operator=(value);}
    JSonValue::JSonValue(unsigned int value) : mDataType(JSonType_Null), mNumberInt(false){operator=(value);}
    JSonValue::JSonValue(long value) : mDataType(JSonType_Null), mNumberInt(false){operator=(value);}
    JSonValue::JSonValue(unsigned long value) : mDataType(JSonType_Null), mNumberInt(false){operator=(value);}
    JSonValue::JSonValue(long long value) : mDataType(JSonType_Null), mNumberInt(false){operator=(value);}
    JSonValue::JSonValue(float value) : mDataType(JSonType_Null), mNumberInt(false){operator=(value);}
    JSonValue::JSonValue(bool value) : mDataType(JSonType_Null), mNumberInt(false){operator=(value);}
    JSonValue::JSonValue(JSonType type) : mDataType(JSonType_Null), mNumberInt(false){
        switch(type){
        case JSonType_Object:
            operator=(Object()); break;
        case JSonType_Array:
            operator=(Array()); break;
        case JSonType_String:
            operator=(""); break;
        case JSonType_Number:
            operator=(0); break;
        case JSonType_Bool:
            operator=(false); break;
        default: break;
        }
    }
    JSonValue::~JSonValue(){
        _ClearData();
    }

    bool JSonValue::is(JSonType t) const{return mDataType == t;}
    bool JSonValue::is(const std::map<string_type, JSonType> &tmap) const{
        if (mDataType != JSonType_Object)
            throw JSonException::InvalidType(JSonType_Object, mDataType);
        for (std::map<string_type, JSonType>::const_iterator i = tmap.begin(); i != tmap.end(); i++){
            if (!has_key(i->first))
                return false;
            if (!mData->_object->at(i->first).is(i->second))
                return false;
        }
        return true;
    }

    bool JSonValue::is(const std::vector<JSonType> &tvec) const{
        if (mDataType != JSonType_Array)
            throw JSonException::InvalidType(JSonType_Array, mDataType);
        if (tvec.size() != mData->_array->size())
            return false;
        for (size_type i = 0; i != tvec.size(); i++){
            if (!mData->_array->at(i).is(tvec[i]))
                return false;
        }
        return true;
    }

    JSonType JSonValue::type() const{return mDataType;}
    string_type JSonValue::type_str() const{
        return JSonTypeToString(mDataType);
    }


    void JSonValue::set(const JSonValue& value){
        switch(value.mDataType){
        case JSonType_Object:
        case JSonType_Array:
        case JSonType_String:
            mData = value.mData;
            mDataType = value.mDataType;
            break;
        case JSonType_Number:
            if (value.mNumberInt){
                operator=(value.mData->_numberi);
            } else {
                operator=(value.mData->_number);
            }
            break;
        case JSonType_Bool:
            operator=(value.mData->_bool); break;
        default:
            _ClearData(); // It's now a null type
        }
    }

    void JSonValue::set(const std::initializer_list<std::pair<string_type, JSonValue> > &ol){
        _ClearData();
        mDataType = JSonType_Object;
        mData->_object = new Object(ol.begin(), ol.end());
    }

    void JSonValue::set(const std::initializer_list<JSonValue> &al){
        _ClearData();
        mDataType = JSonType_Array;
        mData->_array = new Array(al.begin(), al.end());
    }

    Object& JSonValue::get_object(){
        if (mDataType == JSonType_Object)
            return *(mData->_object);
        throw JSonException::InvalidType(JSonType_Object, mDataType);
    }

    Array& JSonValue::get_array(){
        if (mDataType == JSonType_Array)
            return *(mData->_array);
        throw JSonException::InvalidType(JSonType_Object, mDataType);
    }

    void JSonValue::insert(const string_type &key, JSonValue &value){
        if (mDataType != JSonType_Object)
            throw JSonException::InvalidType(JSonType_Object, mDataType);
        if (mData->_object->find(key) != mData->_object->end())
            throw std::runtime_error("Key already exists in JSon Object.");
        mData->_object->insert({key, value});
    }

    void JSonValue::insert(size_type pos, JSonValue &value){
        if (mDataType != JSonType_Array)
            throw JSonException::InvalidType(JSonType_Array, mDataType);
        if (pos >= mData->_array->size()){
            mData->_array->push_back(value);
        } else {
            mData->_array->insert(mData->_array->begin()+pos, value);
        }
    }

    void JSonValue::insert(Array::iterator i, JSonValue &value){
        if (mDataType != JSonType_Array)
            throw JSonException::InvalidType(JSonType_Array, mDataType);
        mData->_array->insert(i, value);
    }

    void JSonValue::push_back(JSonValue &value){
        if (mDataType != JSonType_Array)
            throw JSonException::InvalidType(JSonType_Array, mDataType);
        mData->_array->push_back(value);
    }

    JSonValue& JSonValue::at(const string_type &key){
        if (mDataType == JSonType_Object){
            Object::iterator i = mData->_object->find(key);
            if (i != mData->_object->end())
                return i->second;
            throw JSonException::KeyNotInObject(key);
        }
        throw JSonException::InvalidType(JSonType_Object, mDataType);
    }

    JSonValue& JSonValue::at(size_type index){
        if (mDataType == JSonType_Array){
            if (index >= mData->_array->size())
                throw JSonException::IndexOutOfBounds(index);
            return mData->_array->at(index);
        }
        throw JSonException::InvalidType(JSonType_Array, mDataType);
    }

    bool JSonValue::has_key(const string_type &key) const{
        if (mDataType != JSonType_Object)
            throw JSonException::InvalidType(JSonType_Object, mDataType);
        return mData->_object->find(key) != mData->_object->end();
    }

    bool JSonValue::has_keys(const std::vector<string_type> &keys) const{
        if (mDataType != JSonType_Object)
            throw JSonException::InvalidType(JSonType_Object, mDataType);
        for (std::vector<string_type>::const_iterator i = keys.begin(); i != keys.end(); i++){
            if (mData->_object->find(*i) == mData->_object->end())
                return false;
        }
        return true;
    }

    size_type JSonValue::size(){
        switch(mDataType){
        case JSonType_Object:
            return mData->_object->size();
        case JSonType_Array:
            return mData->_array->size();
        case JSonType_String:
            return mData->_string->size();
        case JSonType_Number:
        case JSonType_Bool:
            return 1;
        default: break;
        }
        return 0;
    }

    JSonValue& JSonValue::parse(const string_type &jsonstr){
        string_type jsrc = _StripCharacters(_trim(jsonstr), "\r\n\t\f\v");
        JSonType jtype = JSonType_Null;
        try{
            jtype = _DetermineType(jsrc);
        } catch(JSonException e) {
            throw e;
        }

        switch(jtype){
        case JSonType_Object:
            set(_ParseObject(jsrc));
            break;
        case JSonType_Array:
            set(_ParseArray(jsrc));
            break;
        default:
            throw JSonException::ParseInvalidJsonContainer();
        }

        return *this;
    }

    string_type JSonValue::to_str(){
        switch(mDataType){
        case JSonType_Null:
            return "null";
        case JSonType_Object:
            return "Object";
        case JSonType_Array:
            return "Array";
        case JSonType_String:
            return *(mData->_string);
        case JSonType_Number:
            return serialize(); // Serialize returns the number as a string already.
        case JSonType_Bool:
            return mData->_bool ? "true" : "false";
        }
        return std::string();
    }

    string_type JSonValue::serialize(){
        return serialize("");
    }

    string_type JSonValue::serialize(const string_type& indentStr, size_type depth){
        string_type serial = "";
        string_type eol = ""; // eol = End of Line (symbol, only used when serializing Arrays and Objects)
        bool past_first_element = false;

        string_type ind = ""; // ind = Indent Spacing (Used only once, unless serializing Objects or Arrays)
        for (size_type d = 0; d < depth; d++)
            ind += indentStr;
        //serial = ind;

        if (indentStr.size() > 0){
            eol = "\n";
        }

        switch(mDataType){
        case JSonType_Null:
        case JSonType_Bool:
            serial += to_str();
            break;
        case JSonType_Number:
            {
                char buf[256];
                if (mNumberInt){
                    snprintf(buf, sizeof(buf), "%lld", mData->_numberi);
                } else {
                    snprintf(buf, sizeof(buf), "%g", mData->_number);
                }
                serial += buf;
            }
            break;
        case JSonType_Object:
            serial += "{" + eol;
            for (Object::iterator i = mData->_object->begin(); i != mData->_object->end(); i++){
                if (past_first_element){serial += "," + eol;}
                serial += ind + indentStr + string_type("\"") + i->first + string_type("\" : ") + i->second.serialize(indentStr, depth+1);
                past_first_element = true;
            }
            serial += eol + ind + "}";
            break;
        case JSonType_Array:
            serial += "[" + eol;
            for (Array::iterator i = mData->_array->begin(); i != mData->_array->end(); i++){
                if (past_first_element){serial += "," + eol;}
                serial += ind + indentStr + (*i).serialize(indentStr, depth+1);
                past_first_element = true;
            }
            serial += eol + ind + "]";
            break;
        case JSonType_String:
            serial += Serialize_str(*(mData->_string));
        }

        return serial;
    }


    JSonValue JSonValue::copy(){
        JSonValue v(mDataType);

        switch(mDataType){
        case JSonType_Object:
            for (Object::iterator i = mData->_object->begin(); i != mData->_object->end(); i++){
                v.mData->_object->insert({i->first, i->second.copy()});
            }
            break;
        case JSonType_Array:
            for (Array::iterator i = mData->_array->begin(); i != mData->_array->end(); i++){
                v.mData->_array->push_back(i->copy());
            }
            break;
        case JSonType_String:
            (*v.mData->_string) = (*mData->_string);
            break;
        case JSonType_Number:
            if (mNumberInt){
                v.mNumberInt = true;
                v.mData->_numberi = mData->_numberi;
            } else {
                v.mData->_number = mData->_number;
            }
            break;
        case JSonType_Bool:
            v.mData->_bool = mData->_bool;
            break;
        default:
            break;
        }

        return v;
    }

    JSonValue& JSonValue::operator=(const JSonValue &rhs){
        set(rhs);
        return *this;
    }

    JSonValue& JSonValue::operator=(const Object &rhs){
        _ClearData();
        mData->_object = new Object(rhs.begin(), rhs.end());
        mDataType = JSonType_Object;
        return *this;
    }

    JSonValue& JSonValue::operator=(const Array &rhs){
        _ClearData();
        mData->_array = new Array(rhs.begin(), rhs.end());
        mDataType = JSonType_Array;
        return *this;
    }

    JSonValue& JSonValue::operator=(const string_type &rhs){
        _ClearData();
        mData->_string = new string_type(rhs.begin(), rhs.end());
        mDataType = JSonType_String;
        return *this;
    }

    JSonValue& JSonValue::operator=(const_char_ptr rhs){
        return operator=(string_type(rhs));
    }

    JSonValue& JSonValue::operator=(double rhs){
        _ClearData();
        mData->_number = rhs;
        mDataType = JSonType_Number;
        mNumberInt = false;
        return *this;
    }

    JSonValue& JSonValue::operator=(int rhs){
        return operator=(static_cast<long long>(rhs));
    }

    JSonValue& JSonValue::operator=(unsigned int rhs){
        return operator=(static_cast<long long>(rhs));
    }

    JSonValue& JSonValue::operator=(long rhs){
        return operator=(static_cast<long long>(rhs));
    }

    JSonValue& JSonValue::operator=(unsigned long rhs){
        return operator=(static_cast<long long>(rhs));
    }

    JSonValue& JSonValue::operator=(long long rhs){
        _ClearData();
        mData->_numberi = rhs;
        mDataType = JSonType_Number;
        mNumberInt = true;
        return *this;
    }

    JSonValue& JSonValue::operator=(float rhs){
        return operator=(static_cast<double>(rhs));
    }

    JSonValue& JSonValue::operator=(bool rhs){
        _ClearData();
        mData->_bool = rhs;
        mDataType = JSonType_Bool;
        return *this;
    }

    bool JSonValue::operator==(const JSonValue& rhs) const{
        if (mDataType == rhs.type()){
            switch(mDataType){
            case JSonType_Array:
                return *(mData->_array) == *(rhs.mData->_array);
            case JSonType_Object:
                return *(mData->_object) == *(rhs.mData->_object);
            case JSonType_String:
                return *(mData->_string) == *(rhs.mData->_string);
            case JSonType_Number:
                return mData->_number == rhs.mData->_number;
            case JSonType_Bool:
                return mData->_bool == rhs.mData->_bool;
            default: break;
            }
            return true; // Should only get here is the type is NULL... and is both are null, then they're equal.
        }
        return false; // Their types don't match... therefore... they're not equal.
    }

    bool JSonValue::operator!=(const JSonValue& rhs) const{
        return !operator==(rhs);
    }

    JSonValue& JSonValue::operator[](const string_type &key){
        if (mDataType != JSonType_Object)
            throw JSonException::InvalidType(JSonType_Object, mDataType);
        Object::iterator val = mData->_object->find(key);
        if (val == mData->_object->end())
            throw JSonException::KeyNotInObject(key);
        return val->second;
    }

    JSonValue& JSonValue::operator[](size_type index){
        if (mDataType != JSonType_Array)
            throw JSonException::InvalidType(JSonType_Array, mDataType);
        if (index >= mData->_array->size())
            throw JSonException::IndexOutOfBounds(index);
        return mData->_array->at(index);
    }


    void JSonValue::_ClearData(){
        mData = std::shared_ptr<_data>(new _data, [=](_data* d){
            switch (mDataType){
            case JSonType_Array:
                delete d->_array; break;
            case JSonType_Object:
                delete d->_object; break;
            case JSonType_String:
                delete d->_string; break;
            default: break;
            }
            delete d;
            d = nullptr;
        });
        mDataType = JSonType_Null;
    }

    string_type JSonValue::Serialize_str(const string_type& s){
        string_type serial = "";
        for (string_type::const_iterator i = s.begin(); i != s.end(); i++){
            switch (*i){
            case '"':
                serial += "\\\""; break;
            case '\\':
                serial += "\\\\"; break;
            case '/':
                serial += "\\/"; break;
            case '\b':
                serial += "\\b"; break;
            case '\f':
                serial += "\\f"; break;
            case '\n':
                serial += "\\n"; break;
            case '\r':
                serial += "\\r"; break;
            case '\t':
                serial += "\\t"; break;
            default:
                serial += *i;
            }
        }
        serial.insert(0, "\"");
        serial.insert(serial.size(), "\"");
        return serial;
    }



/* --------------------------------------------------------------------------------------------
 *  Parsing methods and support functions.
 -------------------------------------------------------------------------------------------- */

    //inline string_type _ltrim(string_type s){
    //    s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
    //    return s;
    //}

    inline string_type _ltrim(const string_type &s){
        string_type ns(s);
        ns.erase(ns.begin(), std::find_if(ns.begin(), ns.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
        return ns;
    }

    //inline string_type _rtrim(string_type s){
    //    s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
    //    return s;
    //}

    inline string_type _rtrim(const string_type &s){
        string_type ns(s);
        ns.erase(std::find_if(ns.rbegin(), ns.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), ns.end());
        return ns;
    }

    //inline string_type _trim(string_type s){
    //    return _ltrim(_rtrim(s));
    //}

    inline string_type _trim(const string_type &s){
        string_type ns(s);
        return _ltrim(_rtrim(ns));
    }

    inline string_type _strtolower(string_type s){
        for (string_type::iterator i = s.begin(); i != s.end(); i++){
            (*i) = std::tolower(*i);
        }
        return s;
    }

    // A dead simple caseless equality check.
    bool _icaseeq(const string_type s1, const string_type s2){
        if (s1.size() != s2.size()){return false;}
        for (size_type i = 0; i < s1.size(); i++){
            if (tolower(s1[i]) != tolower(s2[i]))
                return false;
        }
        return true;
    }

    string_type _deserializeChars(string_type s){
        bool escaped = false;
        size_t i = 0;

        // Stripping the head and tail quotations from the string.
        s = s.substr(1, s.size()-2);

        while (i != s.size()){
            switch (s[i]){
            case '\\':
                if (escaped){
                    s.replace(i-1, 2, "\\");
                    escaped = false;
                } else {
                    escaped = true;
                    i++;
                }
                break;
            case '"':
                if (escaped){
                    s.replace(i-1, 2, "\"");
                    escaped = false;
                } else {i++;}
                break;
            case '/':
                if (escaped){
                    s.replace(i-1, 2, "/");
                    escaped = false;
                } else {i++;}
                break;

            case 'b':
                if (escaped){
                    s.replace(i-1, 2, "\b");
                    escaped = false;
                } else {i++;}
                break;
            case 'f':
                if (escaped){
                    s.replace(i-1, 2, "\f");
                    escaped = false;
                } else {i++;}
                break;
            case 'n':
                if (escaped){
                    s.replace(i-1, 2, "\n");
                    escaped = false;
                } else {i++;}
                break;
            case 'r':
                if (escaped){
                    s.replace(i-1, 2, "\r");
                    escaped = false;
                } else {i++;}
                break;
            case 't':
                if (escaped){
                    s.replace(i-1, 2, "\t");
                    escaped = false;
                } else {i++;}
                break;
            default:
                if (escaped)
                    throw JSonException::ParseMalformed();
                i++;
            }
        }

        return s;
    }


    size_type _FindNextSymbol(const string_type src, const char_type symbol, size_type offset=0){
        size_type depth = 0;
        bool inQuotes = false;

        for (size_type i = offset; i < src.size(); i++){
            char_type c = src[i];
            switch(c){
            case '"':
                if ((i > 0 && src[i-1] != '\\') || i == 0)
                    inQuotes = !inQuotes;
                break;
            case OBJECT_SYM_HEAD:
                if (symbol == OBJECT_SYM_HEAD && !inQuotes && depth == 0){
                    return i;
                }
                depth += (inQuotes) ? 0 : 1;
                break;
            case ARRAY_SYM_HEAD:
                if (symbol == ARRAY_SYM_HEAD && !inQuotes && depth == 0){
                    return i;
                }
                depth += (inQuotes) ? 0 : 1;
                break;
            case OBJECT_SYM_TAIL:
                depth -= (inQuotes || depth == 0) ? 0 : 1;
                if (symbol == OBJECT_SYM_TAIL && !inQuotes && depth == 0){
                    return i;
                }
                break;
            case ARRAY_SYM_TAIL:
                depth -= (inQuotes || depth == 0) ? 0 : 1;
                if (symbol == ARRAY_SYM_TAIL && !inQuotes && depth == 0){
                    return i;
                }
                break;
            default:
                if (src[i] == symbol && depth == 0 && !inQuotes)
                    return i;
            }
        }
        return string_type::npos;
    }

    bool _IsNumber(const string_type &s){
        bool found_decimal = false;
        bool found_e = false;
        bool checked_negorpos = false;

        for (size_type i = 0; i < s.size(); i++){
            if (!std::isdigit(s[i])){
                if (checked_negorpos && (s[i] == '-' || s[i] == '+'))
                    return false; // We can only have ONE '-' pr '+' at the BEGINNING of the number.

                checked_negorpos = true;
                if (!found_e && (s[i] == 'e' || s[i] == 'E')){
                    found_e = true;
                    found_decimal = false; // We can find one more.
                    checked_negorpos = false; // we can find one more.
                } else if (!found_decimal && s[i] == '.'){
                    found_decimal = true;
                } else {
                    return false;
                }
            }
        }
        return true;
    }

    bool _IsString(const string_type &s){
        bool quotes_open = false;
        bool found_escape = false;

        for (size_type i = 0; i < s.size(); i++){
            switch(s[i]){
            case '"':
                if (!found_escape){
                    quotes_open = ! quotes_open;
                } else {
                    found_escape = false;
                }
                break;
            case '\\':
                if (!quotes_open){
                    // There's no way to have an escape character if we're NOT in quotes.
                    return false;
                }
                found_escape = !found_escape;
                break;
            default:
                if (!std::isspace(s[i])){
                    if (!quotes_open)
                        return false;
                    if (found_escape)
                        found_escape = false;
                }
                break;
            }
        }

        return true;
    }

    JSonType _DetermineType(const string_type &src){
        // I'd prefer REGEX, but I don't want to use Boost, and GCC has some issues.
        //std::regex regString("\"(([^\"\\]*)|(\\[\"brnft])|(\\\\u[0-9][a-f]{4}))*\"");
        //std::regex regNumber("(\\+|-)?\\d+(\\.\\d+)?([eE][-+]?\\d+)?");
        //std::regex regBool("([Tt]{1}[Rr]{1}[Uu]{1}[Ee]{1})|([Ff]{1}[Aa]{1}[Ll]{1}[Ss]{1}[Ee]{1})");
        //std::regex regNull("[Nn]{1}[Uu]{1}[Ll]{2}");

        if (_IsString(src))
            return JSonType_String;
        if (_IsNumber(src))
            return JSonType_Number;
        if (_icaseeq(src, "true") or _icaseeq(src, "false"))
            return JSonType_Bool;
        if (_icaseeq(src, "null"))
            return JSonType_Null;
        else{
            size_type fpos = src.find_first_not_of(" \r\n\f\v\t");
            //string_type::const_iterator fchar = std::find_if(src.begin(), src.end(), std::not1(std::ptr_fun<int, int>(std::isspace)));
            //if (fchar != src.end()){
            if (fpos != string_type::npos){
                //string_type::const_iterator lchar = std::find_if(src.rbegin(), src.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base();
                size_type lpos = src.find_last_not_of(" \r\n\f\v\t");
                if (src[fpos] == OBJECT_SYM_HEAD){
                    if (src[lpos] == OBJECT_SYM_TAIL)
                        return JSonType_Object;
                    throw JSonException::ParseUnclosedStructure(JSonType_Object);
                }
                else if (src[fpos] == ARRAY_SYM_HEAD){
                    if (src[lpos] == ARRAY_SYM_TAIL)
                        return JSonType_Array;
                    throw JSonException::ParseUnclosedStructure(JSonType_Array);
                }
            }
        }
        throw JSonException::ParseUnknownValueType(src);
    }

    JSonValue _ParseObject(const string_type &src){
        // At this point, we only ASSUME we have an Object. Let's confirm...
        size_type tailpos = _FindNextSymbol(src, OBJECT_SYM_TAIL);
        if (tailpos == string_type::npos){ // If we didn't find the Object tail symbol, then this is JUNK!
            throw JSonException::ParseUnclosedStructure(JSonType_Object);
            throw std::runtime_error("JSon object not closed!");
        }

        // Check that we only have white space after the Object tail symbol.
        if (_trim(src.substr(tailpos+1)).size() != 0){
            throw JSonException::ParseInvalidSymbols(); // If not... then this is junk!
        }
        // If we're here, then we know that, structurally, anyway, this looks like a valid Object. Let's parse!


        JSonValue obj(JSonType_Object);
        size_type spos = 1;
        size_type epos = 0;
        while (spos < tailpos){
            // Find next value separator
            epos = _FindNextSymbol(src, VALUE_SEPARATOR, spos);
            // If we don't find a value separator, find the end of the object.
            if (epos == string_type::npos){
                epos = _FindNextSymbol(src, OBJECT_SYM_TAIL, spos);
                if (epos == string_type::npos){
                    // Technically, we should have already accounted for this case, but better safe than sorry.
                    throw JSonException::ParseSymbolMissing(OBJECT_SYM_TAIL);
                }
            }

            // Extracting the value pair substring.
            string_type value = _trim(src.substr(spos, epos-spos));

            // Finding the pair separator.
            size_type pairpos = _FindNextSymbol(value, OBJECT_PAIR_SEPARATOR);
            if (pairpos == string_type::npos)
                throw JSonException::ParseSymbolMissing(OBJECT_PAIR_SEPARATOR);

            // Extracting the key name
            string_type key = _deserializeChars(_trim(value.substr(0, pairpos)));
            // Setting the value to the actual value substring.
            value = _trim(value.substr(pairpos+1));
            if (value.size() == 0) // Making sure we ACTUALLY have a value.
                throw JSonException::ParseMalformed();

            JSonType vtype = _DetermineType(value);
            switch(vtype){
            case JSonType_Object:
                obj.get_object().insert({key, _ParseObject(value)});
                break;
            case JSonType_Array:
                obj.get_object().insert({key, _ParseArray(value)});
                break;
            case JSonType_String:
                obj.get_object().insert({key, JSonValue(_deserializeChars(value))});
                break;
            case JSonType_Number:
                if (value.find("e") == string_type::npos && value.find("E") == string_type::npos && value.find(".") == string_type::npos){
                    obj.get_object().insert({key, JSonValue(std::stoll(value))});;
                } else {
                    obj.get_object().insert({key, JSonValue(std::stod(value))});
                }
                break;
            case JSonType_Bool:
                obj.get_object().insert({key, JSonValue(_icaseeq(value, "true"))});
                break;
            case JSonType_Null:
                obj.get_object().insert({key, JSonValue()});
                break;
            }
            spos = epos+1;
            if (epos != tailpos && _trim(src.substr(spos, tailpos-spos)).size() <= 0)
                throw JSonException::ParseMissingValue();
        }

        return obj;
    }

    JSonValue _ParseArray(const string_type &src){
        // At this point, we only ASSUME we have an Array. Let's confirm...
        size_type tailpos = _FindNextSymbol(src, ARRAY_SYM_TAIL);
        if (tailpos == string_type::npos){ // If we didn't find the Array tail symbol, then this is JUNK!
            throw JSonException::ParseSymbolMissing(ARRAY_SYM_TAIL);
        }

        // Check that we only have white space after the Array tail symbol.
        if (_trim(src.substr(tailpos+1)).size() != 0){
            throw JSonException::ParseMalformed(); // If not... then this is junk!
        }
        // If we're here, then we know that, structurally, anyway, this looks like a valid Array. Let's parse!


        JSonValue arr(JSonType_Array);
        size_type spos = 1;
        size_type epos = 0;
        while (spos < tailpos){
            // Find next value separator
            epos = _FindNextSymbol(src, VALUE_SEPARATOR, spos);
            // If we don't find a value separator, find the end of the array.
            if (epos == string_type::npos){
                epos = _FindNextSymbol(src, ARRAY_SYM_TAIL, spos);
                if (epos == string_type::npos){
                    // Technically, we should have already accounted for this case, but better safe than sorry.
                    throw JSonException::ParseSymbolMissing(ARRAY_SYM_TAIL);
                }
            }

            // Extracting the value substring.
            string_type value = _trim(src.substr(spos, epos-spos));
            if (value.size() == 0) // Making sure we ACTUALLY have a value.
                throw JSonException::ParseMalformed();

            JSonType vtype = _DetermineType(value);
            switch(vtype){
            case JSonType_Object:
                arr.get_array().push_back(_ParseObject(value));
                break;
            case JSonType_Array:
                arr.get_array().push_back(_ParseArray(value));
                break;
            case JSonType_String:
                arr.get_array().push_back(JSonValue(_deserializeChars(value)));
                break;
            case JSonType_Number:
                arr.get_array().push_back(JSonValue(std::stod(value)));
                break;
            case JSonType_Bool:
                arr.get_array().push_back(JSonValue(_icaseeq(value, "true")));
                break;
            case JSonType_Null:
                arr.get_array().push_back(JSonValue());
                break;
            }
            spos = epos+1;
            if (epos != tailpos && _trim(src.substr(spos, tailpos-spos)).size() <= 0)
                throw JSonException::ParseMissingValue();
        }

        return arr;
    }


    string_type _StripCharacters(string_type s, string_type characters){
        size_type pos = s.find_first_of(characters);
        while (pos != string_type::npos){
            s.erase(pos, 1);
            pos = s.find_first_of(characters);
        }
        return s;
    }


/* --------------------------------------------------------------------------------------------
 *  json::<Functions>
 -------------------------------------------------------------------------------------------- */

    string_type JSonTypeToString(JSonType type){
        switch(type){
        case JSonType_Object:
            return string_type("Object");
        case JSonType_Array:
            return string_type("Array");
        case JSonType_String:
            return string_type("String");
        case JSonType_Number:
            return string_type("Number");
        case JSonType_Bool:
            return string_type("Bool");
        default: break;
        }
        return string_type("Null");
    }


    // ----------------------------------------------------------------------------------------------
    // ----------------------------------------------------------------------------------------------
    // Defining JSonException (static) member functions
    // ----------------------------------------------------------------------------------------------
    // ----------------------------------------------------------------------------------------------

    JSonException::JSonException(const string_type &msg, unsigned int code) : std::runtime_error(msg), m_code(code){}

    const char* JSonException::what() const throw(){
        std::stringstream ss;
        ss << "[" << m_code << "] " << std::runtime_error::what();
        return ss.str().c_str();
    }

    unsigned int JSonException::get_code() const{
        return m_code;
    }

    JSonException JSonException::InvalidType(JSonType expected, JSonType given){
        string_type msg = "Operation expecting JSonValue type " + JSonTypeToString(expected) + " but JSonValue is of type " + JSonTypeToString(given) + ".";
        return JSonException(msg, 1001);
    }

    JSonException JSonException::KeyNotInObject(const string_type &key){
        string_type msg = "Key \"" + key +"\" not found in JSon Object.";
        return JSonException(msg, 1002);
    }

    JSonException JSonException::IndexOutOfBounds(size_type index){
        std::stringstream ss;
        ss << "JSon Array index #" << index << " out of bounds.";
        return JSonException(ss.str(), 1003);
    }


    JSonException JSonException::ParseMalformed(){
        return JSonException("Parsed JSon appears malformed.", 1010);
    }

    JSonException JSonException::ParseInvalidJsonContainer(){
        return JSonException("Parser expecting JSon Object \"{}\" or JSon Array \"[]\" container.", 1011);
    }

    JSonException JSonException::ParseUnclosedStructure(JSonType type){
        string_type msg = "Parser found unclosed structure type JSon " + JSonTypeToString(type) + ".";
        return JSonException(msg, 1012);
    }

    JSonException JSonException::ParseSymbolMissing(char_type symbol){
        string_type msg = "Parser failed to find expected symbol '" + string_type(&symbol) + "'.";
        return JSonException(msg, 1013);
    }

    JSonException JSonException::ParseInvalidSymbols(){
        return JSonException("Parser found invalid symbols within JSon structure.", 1014);
    }

    JSonException JSonException::ParseUnknownValueType(const string_type &value){
        string_type msg = "Parse cannot determin value type, \"";
        if (value.size() < 10){
            msg += value;
        } else {
            msg += value.substr(0, 10);
        }
        msg += "\".";
        return JSonException(msg, 1015);
    }

    JSonException JSonException::ParseMissingValue(){
        return JSonException("JSon Object or Array expecting additional values, but none found.", 1016);
    }


} // End namespace "OYAJSon"












