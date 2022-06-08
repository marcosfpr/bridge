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
#include "bridge/store/store.hpp"
#include "bridge/store/reader.hpp"

#include "bridge/directory/error.hpp"
#include "bridge/directory/directory.hpp"

#include "bridge/compression/lz4xx.hpp"


namespace bridge::store {

    void store_reader::read_header() {
        using namespace bridge::store;
        using namespace bridge::directory;

        ArraySource source(this->source_->deref(), this->source_->deref() + this->source_->size());
        ArrayReader reader(source); // cursor

        // read the header and the offsets at the end of the stream
        reader.seekg(this->source_->size() - sizeof(size_t), std::ios_base::beg);

        // check ios_state
        if (reader.fail()) {
            throw io_error("Failed to seek from the end of the stream");
        }

        // unmarshall the header
        auto written = bridge::serialization::unmarshall<size_t>(reader);
#ifdef BRIDGE_DEBUG
        std::cout << "Read written bytes: " << written << std::endl;
#endif
        // go back to start of the stream
        reader.seekg(0, std::ios_base::beg);

        // seek to written bytes
        reader.seekg(written, std::ios_base::beg);

        // unmarshall the offsets
        auto idx_offsets = bridge::serialization::unmarshall<std::vector<offset_index>>(reader);

        // insert offsets shift normalization
        std::uint64_t shifted_offset = 0;
        for (auto& idx_offset : idx_offsets) {
            this->offsets_.emplace_back(idx_offset.get_doc_id(), shifted_offset);
            shifted_offset = idx_offset.get_offset();
        }

        reader.close();
    }

    offset_index store_reader::block_offset(doc_id_t id) {
#ifdef BRIDGE_DEBUG
        for (auto & offset : this->offsets_) {
            std::cout << "( " << offset.get_doc_id() << ", " << offset.get_offset() << " )" << std::endl;
        }
        std::cout << "this id: " << id << std::endl;
#endif
        // binary search over the offsets to find the block
        auto it = std::lower_bound(this->offsets_.begin(), this->offsets_.end(), id, [](const offset_index& a, doc_id_t b) {
            return a.get_doc_id() < b;
        });

        if (it == this->offsets_.end() || id > it->get_doc_id()) {
            throw bridge::bridge_error("Invalid doc_id: too high id.");
        }

        return *it;
    }
    void store_reader::read_block(uint64_t block_offset) {
        // careful here
        this->current_block_.clear();
        auto total_buffer = this->source_->deref();

        ArraySource source(total_buffer, total_buffer + this->source_->size());
        ArrayReader reader(source); // cursor

        // todo: narrowing conversion is dangerous here.
        reader.seekg(block_offset, std::ios_base::beg); // go to block offset

        // unmarshall the block
        // auto block_length = bridge::serialization::unmarshall<size_t>(reader); // read the block length
        this->current_block_ = bridge::serialization::unmarshall<std::vector<bridge::byte_t>>(reader);
#ifdef BRIDGE_DEBUG
        std::cout << "shifting " << block_offset << std::endl;
        std::cout << "block length: " << this->current_block_.size() << std::endl;
#endif

    }

    void store_reader::read_block_offsets() {
        ArraySource source(this->current_block_.data(), this->current_block_.size());
        ArrayReader reader(source); // cursor
#ifdef BRIDGE_DEBUG
        std::cout << "Current block size: " << this->current_block_.size() << std::endl;
#endif

        // read the header and the offsets at the end of the stream
        reader.seekg(this->current_block_.size() - sizeof(size_t), std::ios_base::beg);

        // check ios_state
        if (reader.fail()) {
            throw io_error("Failed to seek from the end of the stream");
        }

        // unmarshall the header
        auto written = bridge::serialization::unmarshall<size_t>(reader);

#ifdef BRIDGE_DEBUG
        std::cout << "Reading block offsets" << std::endl;
        std::cout << "Bytes to shift: " << written << std::endl;
#endif

        // go back to start of the stream
        reader.seekg(0, std::ios_base::beg);

        // seek to written bytes
        reader.seekg(written, std::ios_base::beg);

        // unmarshall the offsets
        // todo: something to check its sanity
        this->current_block_offsets = bridge::serialization::unmarshall<std::map<doc_id_t, size_t>>(reader);

        // print keys
#ifdef BRIDGE_DEBUG
        std::cout << "Current block ids: " << std::endl;
        for (auto & kv : this->current_block_offsets) {
            std::cout << kv.first << " ";
        }
        std::cout << std::endl;
#endif

    }

    document store_reader::get(doc_id_t doc_id) {
        auto offset_idx = this->block_offset(doc_id);
#ifdef BRIDGE_DEBUG
        std::cout << "Looking for doc_id: " << doc_id << std::endl;
        std::cout << "Found offset: " << offset_idx.get_doc_id() << " " << offset_idx.get_offset() << std::endl;
#endif
        try  {
            if (offset_idx.get_doc_id() != this->current_offset.get_doc_id() || this->current_block_.empty()){
                this->read_block(offset_idx.get_offset());
                this->read_block_offsets();
            }
            this->current_offset = offset_idx;

            ArraySource source(this->current_block_.data(), this->current_block_.size());
            ArrayReader reader(source); // cursor

            // get the offset given the doc_id
            auto doc_shift = this->current_block_offsets.at(doc_id);

            reader.seekg(doc_shift, std::ios_base::beg); // todo: change type of variables to avoid this

            auto number_fields = bridge::serialization::unmarshall<size_t>(reader);
            auto fields = bridge::serialization::unmarshall<std::vector<field_v>>(reader);

            if (number_fields != fields.size()) {
                throw bridge::bridge_error("Number of fields in the document does not match the number of fields in the index.");
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