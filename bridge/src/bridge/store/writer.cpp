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
#include "bridge/compression/lz4xx.hpp"

namespace bridge::store {

    /**
         * @brief Write field_values to store
         * @param fields Vector of references to field_value objects
     */
    template<class Device, class CompressionStrategy>
    void store_writer<Device, CompressionStrategy>::write(std::vector<field_v> &fields) {
        if (this->is_closed) {
            throw std::runtime_error("Writer is closed");
        }

        // clear intermediary buffer
        this->intermediary_buffer.clear();

        this->write_on_current_block(fields);
        this->doc_id += 1;

        if (this->current_block.size() > BLOCK_SIZE) {
            this->store();
        }

    }

    /**
         * @brief Store field_values.
         * @param compress True if you want to compress the data (You should use this option if you have a lot of data).
     */
    template<class Device, class CompressionStrategy>
    void store_writer<Device, CompressionStrategy>::store() {

        // Write the current_block_offsets to the current_block.
        {
            size_t block_size = this->current_block.size();

            ArrayDevice device(this->current_block);
            ArrayWriter array_writer(device);

            bridge::serialization::marshall<>(array_writer, this->current_block_offsets);
            bridge::serialization::marshall<>(array_writer, block_size);
        }

        // clear intermediary buffer
        this->intermediary_buffer.clear();
        this->intermediary_buffer = std::move(CompressionStrategy{}(this->current_block.data(), this->current_block.size()));

        auto bytesWritten = bridge::serialization::marshall(*writer, this->intermediary_buffer);

        this->written += bytesWritten;

#ifdef BRIDGE_DEBUG
        std::cout << "Written " <<  bytesWritten << " ( " << this->intermediary_buffer.size() << " ) bytes to the Device" << std::endl;
#endif

        this->offsets.push_back(offset_index(this->doc_id, this->written));

        this->current_block.clear();
        this->current_block_offsets.clear();
    }

    /**
         * @brief Close the store writer.
     */
    template<class Device, class CompressionStrategy>
    void store_writer<Device, CompressionStrategy>::close() {
        if (this->current_block.size() > 0) {
            this->store();
        }

        auto offsets_size = bridge::serialization::marshall(*writer, this->offsets);
        size_t header_offset = this->written;
        auto bytes_size_t = bridge::serialization::marshall(*writer, header_offset);

        this->writer->flush();

#ifdef BRIDGE_DEBUG
        std::cout << "Closing the store writer with " << this->written << " bytes written" <<
            " + " << bytes_size_t << "(size_t) + " << offsets_size << "(offsets)" << std::endl;
#endif
        this->is_closed = true;
    }

    template<class Device, class CompressionStrategy>
    void store_writer<Device, CompressionStrategy>::write_on_current_block(std::vector<field_v> &field_values) {
        // write field_values to intermediary buffer
        size_t doc_size;
        {
            ArrayDevice device(this->current_block);
            ArrayWriter array_writer(device);
            doc_size = bridge::serialization::marshall(array_writer, field_values.size());
            doc_size += bridge::serialization::marshall(array_writer, field_values);
        }
        this->current_block_offsets.insert(std::make_pair(this->doc_id, this->current_block.size() - doc_size));

    }

//    template<class Device, class CompressionStrategy>
//    void store_writer<Device, CompressionStrategy>::compress_to_intermediary_buffer() {
//        bridge::byte_t  *compressed_data = nullptr;
//        size_t compressed_size = 0;
//
//        // compress block
//        LZ4Encoder encoder;
//        encoder.open(&compressed_data, &compressed_size);
//        encoder.encode(this->current_block.data(), this->current_block.size());
//        encoder.close();
//
//        // move compressed_data to intermediary_buffer
//        this->intermediary_buffer.clear();
//        this->intermediary_buffer.insert(this->intermediary_buffer.end(), compressed_data, compressed_data + compressed_size);
//
//        std::cout << "before compression: " << this->current_block.size() << " bytes" << std::endl;
//        std::cout << "after compression: " << compressed_size << " bytes" << std::endl;
//    }


    // --------------------------------------------------------------------------------------------- //
    // ------------------------------- Explicit template instantiations  --------------------------- //

    template class store_writer<ArrayDevice>;
    template class store_writer<FileDevice>;

} // namespace bridge::store