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
#pragma clang diagnostic push
#pragma ide diagnostic ignored "UnconstrainedVariableType"
#pragma ide diagnostic ignored "modernize-use-auto"
#pragma ide diagnostic ignored "cppcoreguidelines-narrowing-conversions"

#include "bridge/store/store.hpp"
#include "bridge/store/reader.hpp"
#include "bridge/directory/directory.hpp"

#include "bridge/compression/lz4xx.h"


namespace bridge::store {

    void store_reader::read_header() {
        using namespace bridge::store;
        using namespace bridge::directory;

        // the first offset is implicitly (0, 0)
        this->offsets_.emplace_back(offset_index(0, 0));

        // todo: possibly i can optimize this.
        std::vector<bridge::byte_t> raw(this->source_->deref(), this->source_->deref() + this->source_->size());
        ArrayDevice device(raw);
        ArrayReader reader(device); // cursor

        // read the header and the offsets at the end of the stream
        reader.seekg(sizeof(size_t), std::ios_base::end);

        // unmarshall the header
        auto written = bridge::serialization::unmarshall<size_t>(reader);
        if (written != sizeof(size_t)) {
            throw bridge::bridge_error("invalid header");
        }

        // go back to start of the stream
        reader.seekg(0, std::ios_base::beg);

        // seek to written bytes
        reader.seekg(written, std::ios_base::beg);

        // unmarshall the offsets
        this->offsets_ = bridge::serialization::unmarshall<std::vector<offset_index>>(reader);

        reader.close();

    }

    offset_index store_reader::block_offset(doc_id_t id) {
        // binary search over the offsets to find the block
        auto it = std::lower_bound(this->offsets_.begin(), this->offsets_.end(), id, [](const offset_index& a, doc_id_t b) {
            return a.get_doc_id() < b;
        });

        if (it == this->offsets_.end()) {
            throw bridge::bridge_error("Invalid doc id");
        }

        if(id < it->get_doc_id()) {
            throw bridge::bridge_error("Invalid doc id");
        }

        return *it;
    }
    void store_reader::read_block(uint64_t block_offset) {
        // careful here
        this->current_block_->clear();

        auto total_buffer = this->source_->deref();

        // todo: possibly i can optimize this.
        std::vector<bridge::byte_t> raw(total_buffer, total_buffer + this->source_->size());
        ArrayDevice device(raw);
        ArrayReader reader(device); // cursor

        reader.seekg(block_offset, std::ios_base::beg); // go to block offset

        // unmarshall the block
        size_t block_length = bridge::serialization::unmarshall<size_t>(reader); // read the block length
        // todo: add a check for compression
        std::vector<bridge::byte_t> compressed_block = bridge::serialization::unmarshall<std::vector<bridge::byte_t>>(reader);

        // check the sizes
        if (compressed_block.size() != block_length) {
            throw bridge::bridge_error("Invalid block deserialization");
        }

        // decompress the block
        bridge::byte_t  *decompressed_data = nullptr;
        size_t decompressed_size = 0;

        LZ4Decoder decoder;
        decoder.open(&decompressed_data, &decompressed_size);

        decoder.decode(compressed_block.data(), compressed_block.size());

        // move decompressed_data to current_block
        this->current_block_->clear();
        this->current_block_->insert(this->current_block_->end(), decompressed_data, decompressed_data + decompressed_size);

    }
    document store_reader::get(doc_id_t doc_id) {
        auto offset_idx = this->block_offset(doc_id);

        try  {
            this->read_block(offset_idx.get_offset());
            // todo: possibly i can optimize this.
            ArrayDevice device(*this->current_block_);
            ArrayReader reader(device); // cursor

            // shift the cursor to the right position
            for (size_t i = offset_idx.get_doc_id(); i < doc_id; i++) {
                auto block_length = bridge::serialization::unmarshall<size_t>(reader);
                reader.seekg(block_length, std::ios_base::cur);
            }

            // read the document
            bridge::serialization::unmarshall<size_t>(reader); // read the block length

            auto number_fields = bridge::serialization::unmarshall<size_t>(reader);
            std::vector<field_v> fields;
            for (size_t i = 0; i < number_fields; i++) {
                field_v field = bridge::serialization::unmarshall_v<text_field, uint32_field>(reader);
                fields.push_back(field);
            }

            return document(std::move(fields));
        }
        catch (bridge::bridge_error& e) {
            throw e;
        }
        catch (std::exception& e) {
            throw bridge::bridge_error(e.what());
        }
        catch (...) {
            throw bridge::bridge_error("Unknown error");
        }
    }

}
#pragma clang diagnostic pop