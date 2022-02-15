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

#ifndef WRITER_HPP_
#define WRITER_HPP_

#include <vector>

#include "bridge/directory/directory.hpp"
#include "bridge/schema/field_value.hpp"
#include "bridge/store/store.hpp"
#include "bridge/common/serialization.hpp"

namespace bridge::store {

    using namespace bridge::directory;
    using namespace bridge::schema;

    template <class Device> class store_writer {
      public:
        /**
         * @brief Construct a new store writer object
         * @param writer
         */
        explicit store_writer(WriterPtr<Device> writer)
            : offsets(), intermediary_buffer(), current_block(), doc_id(), written() {
            this->writer = writer;
            this->doc_id = 0;
            this->written = 0;
        }

        /**
         * @brief Destroy the store writer object
         */
        virtual ~store_writer() {
            this->offsets.clear();
            this->intermediary_buffer.clear();
            this->current_block.clear();
            this->writter->flush(); // should i close?
        }

        /**
         * @brief Write field_values to store
         * @param field_values Vector of references to field_value objects
         */
        void store(std::vector<field_value_v> &field_values);

        /**
         * @brief Write field_values to store.
         * @param compress True if you want to compress the data (You should use this option if you have a lot of data).
         */
        void write(bool compress = true);

        /**
         * @brief Close the store writer.
         */
        void close();

        /**
         * @brief Serialize a field.
         * @tparam Archive Archive type.
         * @param ar Archive object.
         * @param version Current version of the field.
         */
        template <class Archive> void serialize(Archive &ar, [[maybe_unused]] const unsigned int version) {

        }
        friend boost::serialization::access; //! Allow to access the private members of field.

      protected:

        /**
         * @brief Write the field values onto the intermediary buffer.
         */
        void write_on_intermediary_buffer(std::vector<field_value_v> &field_values);

        /**
         * @brief Write the intermediary buffer to the current block.
         */
        void write_on_current_block();

      private:
        DocId doc_id;
        std::vector<offset_index> offsets;
        uint64_t written;
        WriterPtr<Device> writer;
        std::vector<bridge::byte_t> intermediary_buffer;
        std::vector<bridge::byte_t> current_block;
    };

} // namespace bridge::store

#endif
