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

#include "bridge/store/writer.hpp"

#define BLOCK_SIZE 16384

namespace bridge::store {

        /**
         * @brief Write field_values to store
         * @param field_values Vector of references to field_value objects
         */
        template <class Device> void store_writer<Device>::store(std::vector<field_value_v> &field_values) {

            // clear intermediary buffer
            this->intermediary_buffer.clear();

            this->write_on_intermediary_buffer(field_values);
            this->write_on_current_block();

            this->doc_id += 1;

            if (this->current_block.size() > BLOCK_SIZE) {
                this->write(true);
            }

        }

        /**
         * @brief Write field_values to store.
         * @param compress True if you want to compress the data (You should use this option if you have a lot of data).
         */
        template <class Device> void store_writer<Device>::write(bool compress) {

        }

        /**
         * @brief Close the store writer.
         */
        template <class Device> void store_writer<Device>::close() {

        }


        template <class Device> void store_writer<Device>::write_on_intermediary_buffer(std::vector<field_value_v> &field_values) {
            // write field_values to intermediary buffer
            ArrayDevice device(this->intermediary_buffer);
            ArrayWriter array_writer(device);
            // 1st write the len of field_values
            bridge::serialization::marshall(array_writer, field_values.size());
            // 2nd write the field_values
            for (field_value_v &field_value : field_values) {
                bridge::serialization::marshall(array_writer, field_value); // todo
            }
        }

        template <class Device> void store_writer<Device>::write_on_current_block() {
            // write field_values to intermediary buffer
            ArrayDevice device(this->current_block);
            ArrayWriter array_writer(device);
            // 1st write the len of intermediary buffer
            bridge::serialization::marshall(array_writer, this->intermediary_buffer.size());
            // 2nd write the intermediary buffer
            bridge::serialization::marshall(array_writer, this->intermediary_buffer);
        }

} // namespace bridge::store