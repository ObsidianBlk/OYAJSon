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

/*! Main header file for the core OYAJSon library. */


#include <algorithm>
#include <memory>
#include <sstream>

#include <string>
#include <map>
#include <vector>

#include <initializer_list>


extern const char* OYAJSON_VERSION; ///< The current version of OYAJSon. This is defined in an auto-generated file created just before compile time.


/*! @namespace OYAJSon
    Namespace for ObsidianBlk's Yet Another JSon library.
*/
namespace OYAJSon {

    class JSonValue;

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


    /*! Returns a human-readable string_type value representing the given JSonType value.

     Mostly used as a helper function.

     @param type A JSonType value.
     @return A string_type value representing the given JSonType.
     */
    string_type JSonTypeToString(JSonType type);



    /*! JSonValue is the primary container for all JSon values.

        Creating a JSonValue is as simple as defining any basic type...

        \code{.cpp}
            JSonValue jval;
        \endcode

        By default, the JSonType for this JSonValue object is JSonType_Null, however, if you want to create a JSonValue with a different JSonType,
        then all one needs to do is pass the desired JSonType into the JSonValue constructor...

        \code{.cpp}
            JSonValue jval(JSonValue_Number); // This will set the JSonType for this JSonValue to JSonType_Number and a default value of 0
        \endcode

        Alternatively, passing in the desired initial value during construction is allowed...

        \code{.cpp}
            JSonValue jval_num(5); // This will set the JSonType for this JSonValue to JSonType_Number with a value of 5
            JSonValue jval_str("Hello World"); // JSonType_String with a value of "Hello World"
            JSonValue jval_bool(true); // JSonType_Bool with a value of true.
            JSonValue jval_null(nullptr); // JSonType_Null ... NOTE: This is the same as not passing any constructor parameters
        \endcode

        Values can also be set via assignment...

        \code{.cpp}
            JSonValue jval = 5; // Sets the JSonValue to a JSonType_Number with a value of 5
        \endcode

        Furthermore, just because the JSonValue was created as one type, it's trivial to change it to another...

        \code{.cpp}
            JSonValue jval = 5; // Start out by setting the JSonType to JSonType_Number with value of 5.
            jval = "Now we're a string! YAY!"; // This call changes the value and the type of the jval variable to JSonType_String and the value given.
        \endcode

        Finally, this assignment can be done between JSonValues as well...

        \code{.cpp}
            JSonValue jvalA = 5;
            JSonValue jvalB = "I'm a string!";

            jvalA = jvalB; // This assigns jvalA the value and type of jvalB. The number 5 is lost.

            std::cout << jvalA.type_str() << std::endl; // This would print out "String"
            std::cout << jvalA.get<string_type>() << std::endl; // This would print out "I'm a string!"
        \endcode

        A JSonValue is not a JSon. By definition, a valid JSon must be zero or more JSonValues stored in an Object ("{}" in JSon notation) or
        an Array ("[]" in JSon notation). A JSonValue **can be** either an Object or Array as well.

        To create an empty JSonType_Object or JSonType_Array JSonValue use the JSonType constructor...

        \code{.cpp}
            JSonValue jval_obj(JSonType_Object); // The JSonValue is an empty JSonType_Object
            JSonValue jval_arr(JSonType_Array); // The JSonValue is an empty JSonType_Array
        \endcode

        Like with other JSonTypes, JSonValues of JSonType_Object and JSonType_Array can be assigned with the = operator.

        However, unlike with the other types, the underlying data is *shared* after an assignment...

        \code{.cpp}
            JSonValue jobjA(JSonValue_Object);
            JSonValue jobjB = jobjA; // Assigns jobjB the object in jobjA

            jobjB.insert("KeyName", 5); // This inserts a new key into a JSonValue of JSonType_Object.
            if (jobjB.has_key("KeyName"))
                std::cout << "Object B has the key!" << std::endl;
            else
                std::cout << "Object B does NOT have the key :(" << std::endl;
            // The above if statement would result in true and "Object B has the key!" would be printed.

            if (jobjA.has_key("KeyName"))
                std::cout << "Object A has the key!" << std::endl;
            else
                std::cout << "Object A does NOT have the key :(" << std::endl;
            // The above if statement would ALSO result in true and "Object A has the key!" would be printed!
            // This is because both jobjA and jobjB SHARE the same underlying Object.
        \endcode

        If one wants to make a "deep" copy of the underlying Array or Object, then the ::copy() method must be used.

        \code{.cpp}
            JSonType jarrA(Array{1, 2, true, "String Here!"}); // Creating an Array via inline initializing, then passing that into the constructor.
            JSonType jarrB = jarrA.copy(); // Creating the second JSonValue as a copy of the first.

            // At this point both jarrA and jarrB contain an array of four elements with the exact same values. However...
            jarrA[3] = "This is a NEW string!";
            std::cout << jarrA[3].get<string_type>() << std::endl;
            std::cout << jarrB[3].get<string_type>() << std::endl;
            // ... would print the following:
            // "This is a NEW string!"
            // "String Here!"
        \endcode
    */
    class JSonValue{
    public:
        /*! Default constructor. Sets JSonType to JSonType_Null */
        JSonValue();

        /*! Semantically the same as passing no constructor parameters. Mostly used when working with initializer lists. */
        JSonValue(std::nullptr_t);

        /*! Sets the JSonValue to the given JSonType, assigning a default value based on the type.
            @param type JSonType value in which to initialize the instance.

            Following are the JSonTypes and their default values...
            + _JSonType_Number_ - __0__
            + _JSonType_Bool_ - __false__
            + _JSonType_String_ - __An empty String__
            + _JSonType_Object_ - __An empty Object__
            + _JSonType_Array_ - __An empty Array__
        */
        explicit JSonValue(JSonType type);

        /*! Sets the JSonValue to the same value/type as the given JSonValue.
            @param value A const JSonValue object reference.

            When assignment is done, the following JSonType JsonValues are copied
            + JSonType_String
            + JSonType_Number
            + JSonType_Bool

            The following JSonType JSonValues __share__ their underlying data
            + JSonType_Object
            + JSonType_Array

            If the desire is a unique copy of the Object or Array data, the following is possible...
            \code{.cpp}
                JSonValue new_json_value(existing_json_object.copy());
            \endcode
        */
        JSonValue(const JSonValue &value);

        /*! Sets a JSonValue to a JSonType_Object with an underlying data set to the given value.
            @param value An Object that will be copied into the internal JSonValue Object.
        */
        JSonValue(const Object &value);

        /*! Sets a JSonValue to a JSonType_Array with an underlying data set to the given value.
            @param value An Array that will be copied into the internal JSonValue Array.
        */
        JSonValue(const Array &value);

        /*! Sets a JSonValue to a JSonType_String with an underlying data set to the given value.
            @param value A string_type
        */
        JSonValue(const string_type& value);

        /*! Sets a JSonValue to a JSonType_Number with an underlying data set to the given value.
            @param value A double
        */
        JSonValue(double value);

        /*! Sets a JSonValue to a JSonType_Number with an underlying data set to the given value.
            @param value An int
        */
        JSonValue(int value);

        /*! Sets a JSonValue to a JSonType_Number with an underlying data set to the given value.
            @param value An unsigned int
        */
        JSonValue(unsigned int value);

        /*! Sets a JSonValue to a JSonType_Number with an underlying data set to the given value.
            @param value A long
        */
        JSonValue(long value);

        /*! Sets a JSonValue to a JSonType_Number with an underlying data set to the given value.
            @param value An unsigned long
        */
        JSonValue(unsigned long value);

        /*! Sets a JSonValue to a JSonType_Number with an underlying data set to the given value.
            @param value A long long
        */
        JSonValue(long long value);

        /*! Sets a JSonValue to a JSonType_Number with an underlying data set to the given value.
            @param value A float
        */
        JSonValue(float value);

        /*! Sets a JSonValue to a JSonType_Bool with an underlying data set to the given value.
            @param value A boolean
        */
        JSonValue(bool value);
        ~JSonValue();

        /*! Returns true if the given JSonType is the same as this instance.

            @param t A JSonType value.
            @return Returns true if this instance is the same type as the JSonType given, and false otherwise.
        */
        bool is(JSonType t) const;

        /*! Returns true if all of the given maps' key/values match the internal Object keys and JsonTypes. False returned otherwise.
            @param tmap A const std::map<string_type, JSonType>& containing the keys and JSonType values to test.
            @return Returns true if all given keys and JSonTypes match those within this instance.
            @throws JSonException Thrown if this instance is not a JSonType_Object.

            __WARNING:__
            Only works on JSonType_Object JSonValue instances.

            Give map does not need to contain all of the keys available within the JSonValue instance, but cannot contain keys that do not exist
            within the JSonValue instance.
        */
        bool is(const std::map<string_type, JSonType> &tmap) const;

        /*! Returns true if all of the given vector JSonTypes match this instance's JSonTypes. False returned otherwise.
            @param tvec A const std::vector<JSonType>& containing the JSonType values to test.
            @return Returns true if all given JSonTypes match those within this instance.
            @throws JSonException Thrown if this instance is not a JSonType_Array.

            __WARNING:__
            Only works on JSonType_Array JSonValue instances.

            Given vector must match in size and in order of entries being checked.
        */
        bool is(const std::vector<JSonType> &tvec) const;

        /*! Returns the current JSonType value of this instance.
            @return JSonType
        */
        JSonType type() const;

        /*! Returns a human readable string representing the current JSonType
            @return string_type

            The following is a list of JSonType and resulting strings...
            + **JSonType_Null** - "Null"
            + **JSonType_String** - "String"
            + **JSonType_Number** - "Number"
            + **JSonType_Bool** - "Bool"
            + **JSonType_Object** - "Object"
            + **JSonType_Array** - "Array"

            NOTE: This method is a shorthand for...

            \code{.cpp}
                JSonTypeToString(jsonvalue_instance.type());
            \endcode

        */
        string_type type_str() const;

        /*! Sets this JSonValue instance to the value/type of the given JSonValue instance.
            @param value A const JSonValue reference.
        */
        void set(const JSonValue& value);

        /*! Sets this JSonValue to a JSonType_Object defined by the given initializer list.
            @param ol const std::initializer_list<std::pair<string_type, JSonValue> >&

            \code{.cpp}
                JSonValue jObject;
                jObject.set({
                    {"Key1", 5},
                    {"Key2", true},
                    {"Key3", "Example"},
                    {"Key4", nullptr}
                });
            \endcode
        */
        void set(const std::initializer_list<std::pair<string_type, JSonValue> > &ol);

        /*! Sets this JSonValue to a JSonType_Array defined by the given initializer list.
            @param al const std::initializer_list<JSonValue>&

            \code{.cpp}
                JSonValue jArray;
                jArray.set({
                    5,
                    true,
                    "Example",
                    nullptr
                });
            \endcode
        */
        void set(const std::initializer_list<JSonValue> &al);

        /*! Template<> method for obtaining the underlying values of this JSonValue instance.

            These methods are only defined for the following JSonTypes...
            + JSonType_String
            + JSonType_Number
            + JSonType_Bool

            __WARNING:__ The ::get<T>() method _must_ match the JSonType of the JSonValue.

            \code{.cpp}
                JSonValue jval(5);
                jval.get<string_type>(); // This will throw an exception.
                jval.get<int>(); // Legal
                jval.get<double>(); // Legal
                jval.get<long>(); // Legal
                jval.get<bool>(); // This will throw an exception.
            \endcode

            There are no ::get<T>() methods for JSonType_Object or JSonType_Array.

            To access a JSonType_Object or JSonType_Array data structure, please use the get_object() and get_array() methods.
        */
        template <typename T> T get() const;

        /*! Returns a reference to the underlying Object data.
            @return A reference to the underlying Object data.
            @throws JSonException Exception thrown if this is not a JSonType_Object type.

            __NOTE:__ This method can only be used with JSonType_Object JSonValues, otherwise a JSonException is thrown.
        */
        Object& get_object();

        /*! Returns a reference to the underlying Array data.
            @return A reference to the underlying Array data.
            @throws JSonException Exception thrown if this is not a JSonType_Array type.

            __NOTE:__ This method can only be used with JSonType_Array JSonValues, otherwise a JSonException is thrown.
        */
        Array& get_array();


        /*! Inserts the given key and value into this JSonType_Object JSonValue.
            @param key A const string_type& containing the new key name.
            @param value A JSonValue& containing the value to store.
            @throws JSonException if the JSonType for this JSonValue is not JSonType_Object, or if the given key already exists.

            This method is shorthand for the following...

            \code{.cpp}
                jObject.get_object().insert({key, value});
            \endcode
        */
        void insert(const string_type &key, JSonValue &value);

        /*! Inserts the given JSonValue into this JSonType_Array JSonValue at the requested position.
            @param pos A size_type index before which to insert the new value.
            @param value A JSonValue& to insert into the Array
            @throws JSonException if the JSonType for this JSonValue is not JSonType_Array, or if the pos is out of bounds.

            This method is shorthand for the following...

            \code{.cpp}
                jArray.get_array().insert(pos, value);
            \endcode
        */
        void insert(size_type pos, JSonValue &value);

        /*! Inserts the given JSonValue into this JSonType_Array JSonValue before the given Array::iterator
            @param i An Array::iterator before which the given JSonValue will be inserted.
            @param value A JSonValue& to insert into the Array
            @throws JSonException if the JSonType for this JSonValue is not JSonType_Array.

            This method is shorthand for the following...

            \code{.cpp}
                jArray.get_array().insert(i, value);
            \endcode
        */
        void insert(Array::iterator i, JSonValue &value);

        /*! Inserts the given JSonValue at the end of this JSonType_Array JSonValue.
            @param value A JSonValue& to insert at the end of the Array.
            @throws JSonException if the JSonType for this JSonValue is not JSonType_Array.

            This method is shorthand for the following...

            \code{.cpp}
                jArray.get_array().push_back(value);
            \endcode
        */
        void push_back(JSonValue &value);

        /*! Returns the JSonValue at the given key within a JSonType_Object.
            @param key A const string_type& key name from which to obtain the JSonValue.
            @throws JSonException if the JSonType of this JSonValue is not JSonType_Object
        */
        JSonValue& at(const string_type &key);

        /*! Returns the JSonValue at the given index within a JSonType_Array.
            @param index A size_type index from which to return the stored JSonValue.
            @throws JSonException if the JSonType of this JSonValue is not JSonType_Array or if the index value is out of bounds.
        */
        JSonValue& at(size_type index);

        /*! Returns true if the given key exists within the JSonType_Object JSonValue.
            @param key A const string_type& key name.
            @return true if the key exists within the Object and false otherwise.
            @throws JSonException if this JSonValue is not of type JSonType_Object
        */
        bool has_key(const string_type &key) const;

        /*! Returns true if all of the given keys in the vector exist within the JSonType_Object JSonValue.
            @param keys A const std::vector<string_type>& containing keys to test for.
            @return true if all of the keys exist, and false if a single one cannot be found.
            @throws JSonException if this JSonValue is not of type JSonType_Object
        */
        bool has_keys(const std::vector<string_type> &keys) const;

        /*! Template method to return an Iterator to the first item in the underlying Object or Array.
            @return TI::iterator
            @throws JSonException if the JSonType does not match the iterator type given.

            \code{.cpp}
                Object::iterator oi = jObject.begin<Object::iterator>();
                Array::iterator ai = jArray.begin<Array::iterator>();

                // These next two lines are equivalent to above...
                Object::iterator oi = jObject.get_object().begin();
                Array::iterator ai = jArray.get_array().begin();
            \endcode
        */
        template <typename TI> TI begin();

        /*! Template method to return an Iterator to the last item in the underlying Object or Array.
            @return TI::iterator
            @throws JSonException if the JSonType does not match the iterator type given.

            \code{.cpp}
                Object::iterator oi = jObject.end<Object::iterator>();
                Array::iterator ai = jArray.end<Array::iterator>();

                // These next two lines are equivalent to above...
                Object::iterator oi = jObject.get_object().end();
                Array::iterator ai = jArray.get_array().end();
            \endcode
        */
        template <typename TI> TI end();


        /*! Returns the number of elements within the JSonValue.
            @return size_type value dependent on the JSonType of the object.

            Return values based on JSonType of the JSonValue.
            + JSonType_Object and JSonType_Array
                + The number of elements within the underlying structure.
            + JSonType_String
                + The number of characters within the string.
            + JSonType_Number and JSonType_Bool
                + Always return 1 _(meaning a value always exists)_
            + JSonType_Null
                + Always returns 0 _(meaning no value exists)_
        */
        size_type size() const;

        /*! Takes a valid JSon formatted string and parses it into this JSonValue.
            @param JSONSTR A const string_type& string containing a valid JSon string.
            @return A reference to this JSonValue object.
            @throw JSonException if the given string is not a valid JSon string.

            The JSon string is considered valid if there is only one root value and that value must be either an Array or Object.
        */
        JSonValue& parse(const string_type &jsonstr);

        /*! Returns the string form of the stored value.
            @return A string_type of the stored value.

            __NOTE:__ This method is equivalent to calling `serialize("")`
        */
        string_type to_str() const;

        /*! Returns the string form of the stored value.
            @param indentStr A string_type value to use as an indentation string used for "pretty printing". Pass "" if no pretty printing is desired.
            @param depth An optional depth [default: 0] at which to start indenting the string.
            @return A string_type of the stored value.

            If indentStr is set to an empty string, no pretty printing is done to the resulting string, otherwise, newline breaks will be inserted
            at intervals which help to "pretty" the resulting string for display on consoles or storing in text files for easy manipulation.

            __NOTE:__ Calling this method on a JSonType_Object or JSonType_Array JSonValue will result in a __valid__ JSon string which can be used in
            the `parse()` method, or passed to other systems or services expecting JSon strings as values.
        */
        string_type serialize(const string_type& indentStr, size_type depth=0) const;

        /*! Creates and returns a "deep" copy of the value stored.
            @return A new JSonValue containing a copy of the value(s) within this JSonValue

            The intent of this method is to allow the creation of new, self contained JSonType_Array or JSonType_Object JSonValues that __DO NOT__
            share the Array or Object underlying them.

            For Arrays and Objects, this method doesn't just create a copy of this JSonValue underlying Array/Object, but a copy of all of the JSonValues
            stored within them as well. Multi-depth Arrays and Objects could take quite a bit of time to copy.
        */
        JSonValue copy() const;


        /*! Sets this JSonValue to the type and value of the given JSonValue (shared copy for Array and Object types)
            @param rhs The JSonValue from which to obtain the new type/value
            @return Reference to this JSonValue
        */
        JSonValue& operator=(const JSonValue &rhs);

        /*! Sets this JSonValue a JSonType_Object and copies the given Object to an underlying Object.
            @param rhs
            @return Reference to this JSonValue

            \code{.cpp}
                // Example
                JSonValue jObject = Object{
                    {"Key1", 5},
                    {"Key2", 3.14159},
                    {"Key3", "A String... surprise!"},
                    {"Key4", false},
                    {"Key5", nullptr}
                };
            \endcode
        */
        JSonValue& operator=(const Object &rhs);

        /*! Sets this JSonValue a JSonType_Array and copies the given Array to an underlying Object.
            @param rhs
            @return Reference to this JSonValue

            \code{.cpp}
                // Example
                JSonValue jArray = Array{
                    5,
                    3.14159,
                    "A String... bet you're caught off guard!",
                    false,
                    nullptr
                };
            \endcode
        */
        JSonValue& operator=(const Array &rhs);

        /*! Sets this JSonValue a JSonType_String and copies the given string_type to an underlying string_type.
            @param rhs
            @return Reference to this JSonValue

            \code{.cpp}
                // Example
                JSonValue jString = "This is an example string.";
            \endcode
        */
        JSonValue& operator=(const string_type &rhs);
        JSonValue& operator=(const_char_ptr rhs);

        /*! Sets this JSonValue a JSonType_Number and stores the given value.
            @param rhs
            @return Reference to this JSonValue

            \code{.cpp}
                // Example
                JSonValue jNumber = 3.14159;
            \endcode
        */
        JSonValue& operator=(double rhs);

        /*! Sets this JSonValue a JSonType_Number and stores the given value.
            @param rhs
            @return Reference to this JSonValue

            \code{.cpp}
                // Example
                JSonValue jNumber = 3;
            \endcode
        */
        JSonValue& operator=(int rhs);

        /*! Sets this JSonValue a JSonType_Number and stores the given value.
            @param rhs
            @return Reference to this JSonValue

            \code{.cpp}
                // Example
                JSonValue jNumber = 3;
            \endcode
        */
        JSonValue& operator=(unsigned int rhs);

        /*! Sets this JSonValue a JSonType_Number and stores the given value.
            @param rhs
            @return Reference to this JSonValue

            \code{.cpp}
                // Example
                JSonValue jNumber = 3;
            \endcode
        */
        JSonValue& operator=(long rhs);

        /*! Sets this JSonValue a JSonType_Number and stores the given value.
            @param rhs
            @return Reference to this JSonValue

            \code{.cpp}
                // Example
                JSonValue jNumber = 3;
            \endcode
        */
        JSonValue& operator=(unsigned long rhs);

        /*! Sets this JSonValue a JSonType_Number and stores the given value.
            @param rhs
            @return Reference to this JSonValue

            \code{.cpp}
                // Example
                JSonValue jNumber = 3;
            \endcode
        */
        JSonValue& operator=(long long rhs);

        /*! Sets this JSonValue a JSonType_Number and stores the given value.
            @param rhs
            @return Reference to this JSonValue

            \code{.cpp}
                // Example
                JSonValue jNumber = 3.14159f;
            \endcode
        */
        JSonValue& operator=(float rhs);

        /*! Sets this JSonValue a JSonType_Bool and stores the given value.
            @param rhs
            @return Reference to this JSonValue

            \code{.cpp}
                // Example
                JSonValue jBoolean = true;
            \endcode
        */
        JSonValue& operator=(bool rhs);

        /*! Returns true if the given JSonValue has the same type/value as this JSonValue, and false otherwise.
            @param rhs A const JSonValue&
            @return boolean
        */
        bool operator==(const JSonValue& rhs) const;

        /*! Returns false if the given JSonValue has the same type/value as this JSonValue, and true otherwise.
            @param rhs A const JSonValue&
            @return boolean
        */
        bool operator!=(const JSonValue& rhs) const;

        /*! Returns the JSonValue reference at the given key.
            @param key A string_type key name within the Object
            @return JSonValue&
            @throws JSonException if this JSonValue is not of type JSonType_Object or if key does not exist within the Object.
        */
        JSonValue& operator[](const string_type &key);

        /*! Returns the JSonValue reference at the given index.
            @param index A size_type index within the Array
            @return JSonValue&
            @throws JSonException if this JSonValue is not of type JSonType_Array or if the index is out of bounds.
        */
        JSonValue& operator[](size_type index);

    private:
        union _data{
            Array* _array;
            Object* _object;
            string_type* _string;
            bool _bool;
            double _number;
            long long _numberi;
        };

        std::shared_ptr<_data> mData;
        //_data* mData;
        JSonType mDataType;
        bool mNumberInt;

        void _ClearData();
        string_type Serialize_str(const string_type& s) const;
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
        if (mDataType == JSonType_Number){
            if (mNumberInt)
                return static_cast<double>(mData->_numberi);
            return mData->_number;
        }
        throw JSonException::InvalidType(JSonType_Number, mDataType);
    }

    template<> inline int JSonValue::get<int>() const{
        if (mDataType == JSonType_Number){
            if (mNumberInt)
                return static_cast<int>(mData->_numberi);
            return static_cast<int>(mData->_number);
        }
        throw JSonException::InvalidType(JSonType_Number, mDataType);
    }

    template<> inline unsigned int JSonValue::get<unsigned int>() const{
        if (mDataType == JSonType_Number){
            if (mNumberInt)
                return static_cast<unsigned int>(mData->_numberi);
            return static_cast<unsigned int>(mData->_number);
        }
        throw JSonException::InvalidType(JSonType_Number, mDataType);
    }

    template<> inline long JSonValue::get<long>() const{
        if (mDataType == JSonType_Number){
            if (mNumberInt)
                return static_cast<long>(mData->_numberi);
            return static_cast<long>(mData->_number);
        }
        throw JSonException::InvalidType(JSonType_Number, mDataType);
    }

    template<> inline unsigned long JSonValue::get<unsigned long>() const{
        if (mDataType == JSonType_Number){
            if (mNumberInt)
                return static_cast<unsigned long>(mData->_numberi);
            return static_cast<unsigned long>(mData->_number);
        }
        throw JSonException::InvalidType(JSonType_Number, mDataType);
    }

    template<> inline long long JSonValue::get<long long>() const{
        if (mDataType == JSonType_Number){
            if (mNumberInt)
                return mData->_numberi;
            return static_cast<long long>(mData->_number);
        }
        throw JSonException::InvalidType(JSonType_Number, mDataType);
    }

    template<> inline unsigned long long JSonValue::get<unsigned long long>() const{
        if (mDataType == JSonType_Number){
            if (mNumberInt)
                return static_cast<unsigned long long>(mData->_numberi);
            return static_cast<unsigned long long>(mData->_number);
        }
        throw JSonException::InvalidType(JSonType_Number, mDataType);
    }

    template<> inline float JSonValue::get<float>() const{
        if (mDataType == JSonType_Number){
            if (mNumberInt)
                return static_cast<float>(mData->_numberi);
            return static_cast<float>(mData->_number);
        }
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






