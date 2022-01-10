// Copyright (c) 2021 Bridge Project (Marcos Pontes)

//  Permission is hereby granted, free of charge, to any person obtaining a copy
//  of this software and associated documentation files (the "Software"), to
//  deal in the Software without restriction, including without limitation the
//  rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
//  sell copies of the Software, and to permit persons to whom the Software is
//  furnished to do so, subject to the following conditions:

//  The above copyright notice and this permission notice shall be included in
//  all copies or substantial portions of the Software.

//  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
//  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
//  IN THE SOFTWARE.

#ifndef BRIDGE_SERIALIZATION_HPP_
#define BRIDGE_SERIALIZATION_HPP_


#include "boost/serialization/serialization.hpp"
#include "boost/serialization/version.hpp"
#include "boost/serialization/split_member.hpp"
#include "boost/archive/text_oarchive.hpp"
#include "boost/archive/text_iarchive.hpp"

#define BRIDGE_SERIALIZE_VERSION(T, N) = BOOST_CLASS_VERSION(T, N)                                      
#define BRIDGE_SERIALIZATION_SPLIT = BOOST_SERIALIZATION_SPLIT_MEMBER()
namespace bridge {

    namespace serialization {

        // Type aliases to make the code more readable.
        using output_archive = boost::archive::text_oarchive;
        using input_archive = boost::archive::text_iarchive;

        // A type T is Serializable if and only if one of the following is true:
        // 1. it is a primitive type.
            // By primitive type we mean a C++ built-in type and ONLY a C++ built-in type.
            // Arithmetic (including characters), bool, enum are primitive types. 
            // Below in serialization traits, we define a "primitive" implementation level in a different way for a different purpose.
            //  This can be a source of confusion.
        // 2. it is a class type and one of the following has been declared according to the prototypes detailed below:
            // a class member function serialize
            // a global function serialize
            // it is a pointer to a Serializable type.
            // it is a reference to a Serializable type.
            // it is a native C++ Array of Serializable type.
        // For more information, please refer to the following link:
        // https://www.boost.org/doc/libs/1_72_0/libs/serialization/doc/tutorial.html
        // https://www.boost.org/doc/libs/1_72_0/libs/serialization/doc/serialization.html




    }; // namespace serialization

}; // namespace bridge




#endif