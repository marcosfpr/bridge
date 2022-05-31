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

#include <fstream>
#include <iostream>
#include <vector>

#include <nlohmann/json.hpp>

#include <cereal/cereal.hpp>
#include <cereal/access.hpp>
#include <cereal/archives/binary.hpp>
#include <cereal/details/traits.hpp>

#include <cereal/types/map.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/types/string.hpp>

#include "bridge/common/archives.hpp"
#include "bridge/global.hpp"
#include "bridge/error.hpp"

namespace bridge::serialization {

    // Type aliases to make the code more readable.
    using output_archive = bridge::archive::BinaryOutput;
    using input_archive = bridge::archive::BinaryInput;

    using json_t = nlohmann::ordered_json;

    /**
     * @brief Custom bridge serialization error.
     *
     */
    class serialization_error : public std::runtime_error {
      public:
        explicit serialization_error(const std::string &what) : std::runtime_error(what) {}
    };

    // A type T is Serializable if and only if one of the following is true:
    // 1. it is a primitive type.
    // By primitive type we mean a C++ built-in type and ONLY a C++ built-in type.
    // Arithmetic (including characters), bool, enum are primitive types.
    // Below in serialization traits, we define a "primitive" implementation level in a different way for a
    // different purpose.
    //  This can be a source of confusion.
    // 2. it is a class type and one of the following has been declared according to the prototypes detailed below:
    // a class member function serialize
    // a global function serialize
    // it is a pointer to a Serializable type.
    // it is a reference to a Serializable type.
    // it is a native C++ Array of Serializable type.

    // Concepts for primitive types
    template <typename T>
    concept Primitive = std::is_integral_v<T> || std::is_arithmetic_v<T> || std::is_enum_v<T> ||
        std::is_same_v<T, bool> || std::is_convertible_v<T, char>;

    // Concepts for containers of T and const T
    template <typename T>
    concept PrimitiveContainer =
        (std::is_array_v<T> && Primitive<std::remove_all_extents_t<T>>) || std::is_same_v<T, std::string>;

    // Concepts for a class member function serialize
    template <typename T>
    concept MemberFunctionSerialize = requires(T &t) {
        {t.serialize(std::declval<output_archive&>())};
        {t.serialize(std::declval<input_archive &>())};
    };

    // Concepts for a global function serialize
    template <typename T>
    concept GlobalFunctionSerialize = requires(T &t) {
        {serialize(std::declval<output_archive &>(), t)};
        {serialize(std::declval<input_archive &>(), t)};
    };

    // Concepts for a pointer to a Serializable type
    template <typename T>
    concept PointerToSerializable = requires(T &t) {
        {t->serialize(std::declval<output_archive &>())};
        {t->serialize(std::declval<input_archive &>())};
    };

    // Concepts for a reference to a Serializable type
    template <typename T>
    concept ReferenceToSerializable = requires(T &t) {
        {t.serialize(std::declval<output_archive &>())};
        {t.serialize(std::declval<input_archive &>())};
    };

    // Concept for a Serializable type
    template <typename T>
    concept Serializable = Primitive<T> || PrimitiveContainer<T> || MemberFunctionSerialize<T> ||
        GlobalFunctionSerialize<T> || PointerToSerializable<T> || ReferenceToSerializable<T>;

    //! \brief Safe serialization of the text indexing option.
    template <class T> [[maybe_unused]] uint64_t marshall(std::ostream &os, T &&obj) {
        try {
            output_archive oa(os);
            oa(obj);
            return oa.totalWritten();
        } catch (std::exception &e) {
            throw serialization_error("Failed to marshall: " + std::string(e.what()));
        }
    }

    //! \brief Safe unmarshall of whatever type T is.
    template <class T> [[maybe_unused]] static T unmarshall(std::istream &is) {
        try {
            input_archive ia(is);
            T obj;
            // read sequence of arguments passed as parameter
            ia(obj);
            // return T constructed from the arguments.
            return obj;
        } catch (std::exception &e) {
            throw serialization_error("Failed to unmarshall: " + std::string(e.what()));
        }
    }

    //! \brief Safe unmarshall of whatever type T is.
    template <class T> [[maybe_unused]] static T unmarshall(std::stringstream& raw_stream) {
        std::vector<T> unmarshalled;
        try {
            input_archive ia(raw_stream);
            T obj;
            // read sequence of arguments passed as parameter
            ia(obj);
            // return T constructed from the arguments.
            return obj;
        } catch (std::exception &e) {
            throw serialization_error("Failed to unmarshall: " + std::string(e.what()));
        }
    }

    //! \brief Safe unmarshall of whatever type T is.
    template <class T> [[maybe_unused]] static uint64_t marshall(std::stringstream& raw_stream, T &&obj) {
        try {
            output_archive oa(raw_stream);
            oa(obj);
            return raw_stream.width();
        } catch (std::exception &e) {
            throw serialization_error("Failed to marshall: " + std::string(e.what()));
        }
    }

    // Concept for a JSON Serializable type
    // Must implement the following functions:
    // - to_json
    // - static from_json that returns a T
    template <typename T>
    concept JSONSerializable = requires(T &t) {
        {t.to_json()};
        {t.from_json(std::declval<json_t>())};
    };

    //! \brief Read  serialized objects from a json input text stream.
    template <JSONSerializable T, typename InputType> [[maybe_unused]] static T unmarshall_json(InputType &in) {
        try {
            json_t j = json_t::parse(in);
            return T::from_json(j);
        } catch (std::exception &e) {
            throw serialization_error("Failed to unmarshall JSON: " + std::string(e.what()));
        }
    }

    //! \brief Write serialized objects to a json output text stream.
    template <JSONSerializable T, class OutputType>
    [[maybe_unused]] static uint64_t marshall_json(OutputType &out, T &&t) {
        try {
            json_t j = t.to_json();
            out << j.dump(4) << std::endl;
            return sizeof(t);
        } catch (std::exception &e) {

            throw serialization_error("Failed to marshall JSON: " + std::string(e.what()));
        }
    }

} // namespace bridge::serialization

#endif