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

#ifndef STORE_HPP_
#define STORE_HPP_

#include <utility>

#include "bridge/global.hpp"
#include "bridge/common/serialization.hpp"

#define BLOCK_SIZE 16384 // todo: perhaps we need to parametrize this.

namespace bridge::store {

    ///! @brief Offset Index
    class offset_index {
      public:

        ///! @brief  Default constructor for deserialization
        offset_index() = default;

        /**
       * @brief Construct a new offset index object
         */
        explicit offset_index(bridge::doc_id_t doc_id, std::uint64_t offset) : doc_id(doc_id), offset(offset) {}

        /**
         * @brief Copy constructor
         */
        offset_index(const offset_index&) = default;

        /**
         * @brief Move constructor
         */
        offset_index(offset_index&&) = default;

        /**
         * @brief Copy assignment operator
         */
        offset_index& operator=(const offset_index&) = default;

        /**
         * @brief Move assignment operator
         */
        offset_index& operator=(offset_index&&) = default;

        /**
         * @brief Get the doc id
         *
         * @return doc_id_t
         */
        [[nodiscard]] bridge::doc_id_t get_doc_id() const { return doc_id; }

        /**
         * @brief Get the offset
         *
         * @return std::uint64_t
         */
        [[nodiscard]] std::uint64_t get_offset() const { return offset; }

        /**
         * @brief Equality operator
         */
        bool operator==(const offset_index& other) const {
            return doc_id == other.doc_id && offset == other.offset;
        }

        /**
         * @brief Inequality operator
         */
        bool operator!=(const offset_index& other) const {
            return !(*this == other);
        }

        /**
         * @brief Three-way <=> comparison operator. It allows using operators < <= > >=.
         */
        std::strong_ordering operator<=>(const offset_index& other) const {
            if (doc_id != other.doc_id) {
                return offset <=> other.offset;
            }
            return offset <=> other.offset;
        }

        /**
         * @brief Serialize a offset_index.
         * @tparam Archive Archive type.
         * @param ar Archive object.
         * @param version Current version of the offset_index.
         */
        template <class Archive> void serialize(Archive &ar) {
            ar( doc_id, offset );
        }
        friend class cereal::access;

      private:
        bridge::doc_id_t doc_id;
        std::uint64_t offset;
    };

} // namespace bridge::store

#endif
