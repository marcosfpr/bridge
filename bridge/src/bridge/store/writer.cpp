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
#include "bridge/compression/lz4xx.h"

#define BLOCK_SIZE 16384

namespace bridge::store {

        /**
         * @brief Write field_values to store
         * @param fields Vector of references to field_value objects
         */
        template <class Device> void store_writer<Device>::store(std::vector<field_v> &fields) {

            // clear intermediary buffer
            this->intermediary_buffer.clear();

            this->write_on_intermediary_buffer(fields);
            this->write_on_current_block();

            this->doc_id += 1;

            if (this->current_block.size() > BLOCK_SIZE) {
                this->write();
            }

        }

        /**
         * @brief Write field_values to store.
         * @param compress True if you want to compress the data (You should use this option if you have a lot of data).
         */
        template <class Device> void store_writer<Device>::write() {

            // clear intermediary buffer
            this->intermediary_buffer.clear();

            if(compress) {
                this->compress_to_intermediary_buffer();
            }
            else {
                this->intermediary_buffer = std::move(this->current_block);  // just move
                this->current_block.clear();
            }

            size_t final_size = this->intermediary_buffer.size();
            writer->write(final_size, sizeof(size_t));
            writer->write(this->intermediary_buffer.data(), final_size);

            this->written += final_size + sizeof(size_t);
            this->offsets.push_back(offset_index(this->doc_id, this->written));

            this->current_block.clear();
        }

        /**
         * @brief Close the store writer.
         */
        template <class Device> void store_writer<Device>::close() {
            if (this->current_block.size() > 0) {
                this->write();
            }

            size_t header_offset = this->written;
            bridge::serialization::marshall(writer, this->offsets);
            bridge::serialization::marshall(writer, header_offset);

            this->writer->flush();
        }


        template <class Device> void store_writer<Device>::write_on_intermediary_buffer(std::vector<field_v> &fields) {
            // write field_values to intermediary buffer
            ArrayDevice device(this->intermediary_buffer);
            ArrayWriter array_writer(device);
            // 1st write the len of field_values
            bridge::serialization::marshall(array_writer, fields.size());
            // 2nd write the field_values
            for (field_v &field_value : fields) {
                bridge::serialization::marshall_v<text_field, uint32_field>(array_writer, field_value);
            }
        }

        template <class Device> void store_writer<Device>::write_on_current_block() {
            // write field_values to intermediary buffer
            ArrayDevice device(this->current_block);
            ArrayWriter array_writer(device);
            // 1st write the len of intermediary buffer
            bridge::serialization::marshall(array_writer, this->intermediary_buffer.size());
            // 2nd write the intermediary buffer
            bridge::serialization::marshall(array_writer, std::move(this->intermediary_buffer));
        }

        template <class Device> void store_writer<Device>::compress_to_intermediary_buffer() {
            bridge::byte_t  *compressed_data = nullptr;
            size_t compressed_size = 0;

            // compress block
            LZ4Encoder encoder;
            encoder.open(&compressed_data, &compressed_size);

            encoder.encode(this->intermediary_buffer.data(), this->intermediary_buffer.size());

            encoder.close();

            // move compressed_data to intermediary_buffer
            this->intermediary_buffer.clear();
            this->intermediary_buffer.insert(this->intermediary_buffer.end(), compressed_data, compressed_data + compressed_size);

        }

} // namespace bridge::store