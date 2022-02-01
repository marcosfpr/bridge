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

#ifndef NAMED_FIELD_DOCUMENT_HPP_
#define NAMED_FIELD_DOCUMENT_HPP_

#include <string>
#include <memory>

#include "common/serialization.hpp"
#include "schema/field_value.hpp"

#include "absl/container/btree_map.h"

namespace bridge::schema {

    // typedef std::unique_ptr<value> value_ptr; // todo: sounds bad;
    // typedef absl::btree_map<std::string, value_ptr> field_map;

    // struct named_field_document {
    //     field_map fmap;

    //     friend class boost::serialization::access;
    //     /**
    //      * @brief Serialization of field value.
    //      * @tparam Archive Archive type.
    //      * @param ar Archive object.
    //      * @param version Current version of the object.
    //      */
    //     template <class Archive> void serialize(Archive &ar, [[maybe_unused]] const unsigned int version) {
            
    //     }

    // };

}

#endif