#ifndef __OYAJSON_H__
#define __OYAJSON_H__

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

/*! @file JSon.h
    @brief Main header file for the JSon library.
*/


#include <algorithm>
#include <memory>
#include <sstream>

#include <string>
#include <map>
#include <vector>

/*! @namespace OYAJSon
    @brief Namespace for Obsidian's Yet Another JSon system.
*/
namespace OYAJSon {

    class JSonValue;

    const unsigned int OYAJSON_VERSION_MAJOR = 0;
    const unsigned int OYAJSON_VERSION_MINOR = 3;
    const unsigned int OYAJSON_VERSION_REV = 0;

    /*! @typedef size_type
        @brief As defined by std::size_t
    */
    typedef std::size_t size_type;
    /*! @typedef string_type
        @brief AKA std::string
    */
    typedef std::string string_type;
    /*! @typedef const_char_ptr
        @brief As defined by std::string::const_pointer
    */
    typedef std::string::const_pointer const_char_ptr;
    /*! @typedef char_type
        @brief As defined by std::string::value_type
    */
    typedef std::string::value_type char_type;
    /*! @typedef Array
        @brief std::vector<JSonValue>
    */
    typedef std::vector<JSonValue> Array;
    /*! @typedef Object
        @brief std::map<string_type, JSonValue>
    */
    typedef std::map<string_type, JSonValue> Object;

    /*! @enum JSonType
        @brief Defines the core types used in a JSON.
    */
    enum JSonType {JSonType_Object, JSonType_Array, JSonType_Number, JSonType_String, JSonType_Bool, JSonType_Null};

    static const char_type OBJECT_PAIR_SEPARATOR = ':'; ///< Char type value of the symbol in a JSon Object's <key>:<value> pair.
    static const char_type VALUE_SEPARATOR = ','; ///< Char type value of the symbol in a JSon Object or Array separating values.
    static const char_type OBJECT_SYM_HEAD = '{'; ///< Char type value of the symbol used to start a JSon Object.
    static const char_type OBJECT_SYM_TAIL = '}'; ///< Char type value of the symbol used to end a JSon Object.
    static const char_type ARRAY_SYM_HEAD = '['; ///< Char type value of the symbol used to start a JSon Array.
    static const char_type ARRAY_SYM_TAIL = ']'; ///< Char type value of the symbol used to end a JSon Array.


    /*! @brief Parse a JSon formatted string into a JSonValue structure.
     * NOTE: To be valid, string MUST start with an Array "[]" or Object "{}" as the one and only top-level object.
     * @param [in] src A string_type JSon formatted string.
     * @return A JSonValue object.
     * @throw JSonException Contains a description of what went wrong.
     */
    JSonValue Parse(const string_type &src);

    /*! @brief Parse a JSon formatted input stream into a JSonValue structure.
     * NOTE: To be valid, input stream MUST start with an Array "[]" or Object "{}" as the one and only top-level object.
     * @param [in] src An std::istream containing a JSon formatted string value.
     * @return A JSonValue object.
     * @throw JSonException Contains a description of what went wrong.
     */
    JSonValue Parse(std::istream &src);

    /*! @brief Parse a JSon formatted char string into a JSonValue structure.
     * NOTE: To be valid, string MUST start with an Array "[]" or Object "{}" as the one and only top-level object.
     * @param [in] src A const_char_ptr containing a JSon formatted string of characters.
     * @param [in] length The number of characters to be consumed.
     * @return A JSonValue object.
     * @throw JSonException Contains a description of what went wrong.
     */
    JSonValue Parse(const_char_ptr src, size_type length);


    /*! @brief Returns a human-readable string_type value representing the given JSonType value.
     * Mostly used as a helper function.
     * @param [in] type A JSonType value.
     * @return A string_type value representing the given JSonType.
     */
    string_type JSonTypeToString(JSonType type);



    /*! @class JSonValue
        @brief Containing class structure for all JSon value types.

        To create a simple JSonValue object simply write...

        \code{.cpp}
            JSonValue jval();
        \endcode

        This will create a JSonValue object with a type of JSonType_Null.
        The class is designed to make assignments pretty easy...

        \code{.cpp}
            jval = 5;
        \endcode

        This will cause the jval JSonValue object to store the value of 5 and change it's type to JSonType_Number.

        \code{.cpp}
            jval = true;
        \endcode

        This will set the jval object to a JSonType_Bool with a value of true. The value 5 is lost.
        Assigning a JSonValue instance to the value/type of another JSonValue instance is equally as simple.

        \code{.cpp}
            JSonValue jval2(5); // Notice we can assign the value during instantiation as well.
            jval = jval2;
        \endcode

        This will set jval back to a JSonType_Number with value of 5.
        All assignments are value copied for JSonType_Null, JSonType_Number, JSonType_Bool, and JSonType_String.
        If a JSonValue is of type JSonType_Object or JSonType_Array, then assignment using operator=() will be
        shared.
        Look at the following code example...

        \code{.cpp}
            // We've parsed the following JSON into a JSonValue variable named jobj.
            //  {
            //      "value1":1,
            //      "value2":2,
            //      "value3":"Three"
            //  }

            std::cout << jobj.at("value3").get<string_type>() << std::endl;
            // This will print "Three"

            JSonValue jval();
            jval = jobj;

            jval.at("value3") = "TEN!";
            std::cout << jval.at("value3").get<string_type>() << std::endl;
            // This will print "TEN!"

            std::cout << jobj.at("value3").get<string_type>() << std::endl;
            // This will ALSO print "TEN!"
        \endcode

        Notice how the basic assignment gave bothe jobj and jval the same underlying Object data.
        If a deep copy is what's desired, then use JSonValue::copy()

        \code{.cpp}
            // Assume jobj contains the same JSON from the beginning of the last example.

            jSonValue jval();
            jval = jobj.copy();

            jval.at("value3") = "TEN!";
            std::cout << jval.at("value3").get<string_type>() << std::endl;
            // This will print "TEN!"

            std::cout << jobj.at("value3").get<string_type>() << std::endl;
            // This will print "Three"
        \endcode
    */
    class JSonValue{
    public:
        /*! @brief Default constructor. Sets JSonType to JSonType_Null */
        JSonValue();

        /*! @brief Sets the JSonValue to the given JSonType, assigning a default value based on the type.

            <JSonTypes> - <Default Value>
            JSonType_String - ""
            JSonType_Number - 0
            JSonType_Bool - false
            JSonType_Object - An empty Object
            JSonType_Array - An empty Array

            @param [in] type JSonType value in which to initialize the instance.
        */
        explicit JSonValue(JSonType type);

        JSonValue(std::nullptr_t);

        /*! @brief Sets the JSonValue to the same value/type as the given JSonValue.

            NOTE:
            JSonTypes _String, _Number, and _Bool are value copied.
            JSonTypes _Object and _Array are value shared.

            The following emulates a value copied assignment...
            \code{.cpp}
                JSonValue new_json_value(existing_json_object.copy());
            \endcode

            @param [in] value A const JSonValue object reference.
        */
        JSonValue(const JSonValue &value);

        /*! @brief Sets a JSonValue to a JSonType_Object with an underlying data set to the given value. */
        JSonValue(const Object &value);

        /*! @brief Sets a JSonValue to a JSonType_Array with an underlying data set to the given value. */
        JSonValue(const Array &value);

        JSonValue(const string_type& value);
        JSonValue(const_char_ptr value);
        JSonValue(double value);
        JSonValue(int value);
        JSonValue(unsigned int value);
        JSonValue(long value);
        JSonValue(unsigned long value);
        JSonValue(long long value);
        JSonValue(unsigned long long value);
        JSonValue(float value);
        explicit JSonValue(bool value);
        ~JSonValue();

        /*! @brief Returns true if the given JSonType is the same as this instance.

            @param [int] t A JSonType value.
            @return Returns true if this instance is the same type as the JSonType given, and false otherwise.
        */
        bool is(JSonType t) const;

        /*! @brief Returns the current JSonType value of this instance.
            @return JSonType
        */
        JSonType type() const;

        /*! @brief Returns a string_type value of the JSonType of this instance.

            <JSonType> - <Resulting String>
            _Null - "Null"
            _String = "String"
            _Number - "Number"
            _Bool - "Bool"
            _Object - "Object"
            _Array - "Array"

            NOTE: This method is a shorthand for...

            \code{.cpp}
                JSonTypeToString(jsonvalue_instance.type());
            \endcode

            @return string_type
        */
        string_type type_str() const;

        /*! @brief Sets this JSonValue instance to the value/type of the given JSonValue instance.

            @param [in] value A const JSonValue reference.
        */
        void set(const JSonValue& value);

        /*! @brief Template<> method for obtaining the underlying values of this JSonValue instance.

            These methods will work ONLY with the following JSonTypes...
            _String
            _Number
            _Bool

            Please use the get_object() and get_array() methods to get the underlying Object or Array data respectively.
        */
        template <typename T> T get() const;

        /*! @brief Returns a reference to the underlying Object data.

            NOTE: This method can only be used with JSonType_Object JSonValues, otherwise a JSonException is thrown.

            @return Object A reference to the underlying Object data.
            @throws JSonException Exception thrown if this is not a JSonType_Object type.
        */
        Object& get_object();

        /*! @brief Returns a reference to the underlying Array data.

            NOTE: This method can only be used with JSonType_Array JSonValues, otherwise a JSonException is thrown.

            @return Array A reference to the underlying Array data.
            @throws JSonException Exception thrown if this is not a JSonType_Array type.
        */
        Array& get_array();


        void insert(const string_type &key, JSonValue &value);
        void insert(size_type pos, JSonValue &value);
        void insert(Array::iterator i, JSonValue &value);
        void push_back(JSonValue &value);

        JSonValue& at(const string_type &key);
        JSonValue& at(size_type index);

        bool has_key(const string_type &key);

        template <typename TI> TI begin();
        template <typename TI> TI end();


        //! Returns the number of elements within the JSonValue.
        /*!
         * For JSonType_Object and JSonType_Array JSonValues, the return value is the number of elements within the map or array, respectively.
         * For JSonType_String JSonValues, the return value is the number of characters within the string.
         * For JSonType_Number and JSonType_Bool JSonValues, the return value is always 1 (meaning, one value exists in the JSonValue).
         * For JSonType_Null JSonValues, the return value is always 0 (meaning, no values exist in the JSonValue).
         */
        size_type size();

        /*! @brief Takes a valid JSon formatted string and parses it into this JSonValue.
        *
        * The JSon string is considered valid if there is only one root value and that value must be either an Array or Object.
        * @param string_type Constant, by reference, a valid JSon string.
        * @return A reference to this JSonValue object.
        * @throw JSonException if the given string is not a valid JSon string.
        */
        JSonValue& from_str(const string_type &jsonstr);

        string_type to_str();
        string_type serialize(bool pretty=false);
        string_type serialize(size_type depth);
        string_type serialize(const string_type& indentStr, size_type depth=0);

        JSonValue copy();

        JSonValue& operator=(const JSonValue &rhs);
        JSonValue& operator=(const Object &rhs);
        JSonValue& operator=(const Array &rhs);
        JSonValue& operator=(const string_type &rhs);
        JSonValue& operator=(const_char_ptr rhs);
        JSonValue& operator=(double rhs);
        JSonValue& operator=(int rhs);
        JSonValue& operator=(unsigned int rhs);
        JSonValue& operator=(long rhs);
        JSonValue& operator=(unsigned long rhs);
        JSonValue& operator=(long long rhs);
        JSonValue& operator=(unsigned long long rhs);
        JSonValue& operator=(float rhs);
        JSonValue& operator=(bool rhs);

        bool operator==(const JSonValue& rhs) const;
        bool operator!=(const JSonValue& rhs) const;

        JSonValue& operator[](const string_type &key);
        JSonValue& operator[](size_type index);

    private:
        union _data{
            Array* _array;
            Object* _object;
            string_type* _string;
            bool _bool;
            double _number;
        };

        std::shared_ptr<_data> mData;
        //_data* mData;
        JSonType mDataType;

        void _ClearData();
        string_type Serialize_str(const string_type& s);
    };


    // ------------------------------------------------------------------------------------------------------
    // ------------------------------------------------------------------------------------------------------
    // CLASS: JSonException
    // ------------------------------------------------------------------------------------------------------
    // ------------------------------------------------------------------------------------------------------

    class JSonException : public std::runtime_error{
    public:
        JSonException(const string_type &msg, unsigned int code);
        virtual const char* what() const throw();
        unsigned int get_code() const;

        static JSonException InvalidType(JSonType expected, JSonType given);
        static JSonException KeyNotInObject(const string_type &key);
        static JSonException IndexOutOfBounds(size_type index);

        static JSonException ParseMalformed();
        static JSonException ParseInvalidJsonContainer();
        static JSonException ParseUnclosedStructure(JSonType type);
        static JSonException ParseSymbolMissing(char_type symbol);
        static JSonException ParseInvalidSymbols();
        static JSonException ParseUnknownValueType(const string_type &value);
        static JSonException ParseMissingValue();
    private:
        unsigned int m_code;
    };


    // ------------------------------------------------------------------------------------------------------
    // ------------------------------------------------------------------------------------------------------
    // Template method definitions!
    // ------------------------------------------------------------------------------------------------------
    // ------------------------------------------------------------------------------------------------------


    template<> inline string_type JSonValue::get<string_type>() const{
        if (mDataType == JSonType_String)
            return string_type(mData->_string->begin(), mData->_string->end());
        throw JSonException::InvalidType(JSonType_String, mDataType);
    }

    template<> inline double JSonValue::get<double>() const{
        if (mDataType == JSonType_Number)
            return mData->_number;
        throw JSonException::InvalidType(JSonType_Number, mDataType);
    }

    template<> inline int JSonValue::get<int>() const{
        if (mDataType == JSonType_Number)
            return static_cast<int>(mData->_number);
        throw JSonException::InvalidType(JSonType_Number, mDataType);
    }

    template<> inline unsigned int JSonValue::get<unsigned int>() const{
        if (mDataType == JSonType_Number)
            return static_cast<unsigned int>(mData->_number);
        throw JSonException::InvalidType(JSonType_Number, mDataType);
    }

    template<> inline long JSonValue::get<long>() const{
        if (mDataType == JSonType_Number)
            return static_cast<long>(mData->_number);
        throw JSonException::InvalidType(JSonType_Number, mDataType);
    }

    template<> inline unsigned long JSonValue::get<unsigned long>() const{
        if (mDataType == JSonType_Number)
            return static_cast<unsigned long>(mData->_number);
        throw JSonException::InvalidType(JSonType_Number, mDataType);
    }

    template<> inline long long JSonValue::get<long long>() const{
        if (mDataType == JSonType_Number)
            return static_cast<long long>(mData->_number);
        throw JSonException::InvalidType(JSonType_Number, mDataType);
    }

    template<> inline unsigned long long JSonValue::get<unsigned long long>() const{
        if (mDataType == JSonType_Number)
            return static_cast<unsigned long long>(mData->_number);
        throw JSonException::InvalidType(JSonType_Number, mDataType);
    }

    template<> inline float JSonValue::get<float>() const{
        if (mDataType == JSonType_Number)
            return static_cast<float>(mData->_number);
        throw JSonException::InvalidType(JSonType_Number, mDataType);
    }

    template<> inline bool JSonValue::get<bool>() const{
        if (mDataType == JSonType_Bool)
            return mData->_bool;
        throw JSonException::InvalidType(JSonType_Bool, mDataType);
    }

    template <> inline Object::iterator JSonValue::begin(){
        if (mDataType == JSonType_Object)
            return mData->_object->begin();
        throw JSonException::InvalidType(JSonType_Object, mDataType);
    }

    template <> inline Array::iterator JSonValue::begin(){
        if (mDataType == JSonType_Array)
            return mData->_array->begin();
        throw JSonException::InvalidType(JSonType_Object, mDataType);
    }

    template <> inline Object::iterator JSonValue::end(){
        if (mDataType == JSonType_Object)
            return mData->_object->end();
        throw JSonException::InvalidType(JSonType_Array, mDataType);
    }

    template <> inline Array::iterator JSonValue::end(){
        if (mDataType == JSonType_Array)
            return mData->_array->end();
        throw JSonException::InvalidType(JSonType_Array, mDataType);
    }

} // End namespace "OYAJSon"

#endif // __OYAJSON_H__






