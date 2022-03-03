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

#ifndef READER_HPP_
#define READER_HPP_

#include <memory>
#include <vector>

#include "bridge/directory/read_only_source.hpp"
#include "bridge/schema/document.hpp"
#include "bridge/store/store.hpp"
#include "bridge/common/serialization.hpp"

namespace bridge::store {

    using namespace bridge::directory;
    using namespace bridge::schema;

    class store_reader {
      public:
        /**
         * @brief Construct a new store reader object
         * @param source The source of the store
         */
        explicit store_reader(std::shared_ptr<read_only_source> source)
            : source_(std::move(source)) {
            this->read_header();
        }

        /**
         * @brief Destroy the store reader object
         */
        virtual ~store_reader() {
            source_.reset();
            offsets_.clear();
            current_block_->clear();
        }

        /**
         * @brief Get the document at the given block_offset
         * @param doc_id Document ID
         * @return document The document at the given doc id if it exists.
         */
        document get(doc_id_t doc_id);

      protected:

        /**
         * @brief Read the source and fill the offsets vector
         */
        void read_header();

        /**
         * @brief Search a block to the given document id.
         *
         * @param id The document id
         */
         offset_index block_offset(doc_id_t id);

         /**
          * @brief Read the block at the given offset
          */
          void read_block(uint64_t block_offset);

      private:
        std::shared_ptr<read_only_source> source_;
        std::vector<offset_index> offsets_;
        std::shared_ptr<std::vector<bridge::byte_t>> current_block_;
    };


} // namespace bridge::store

#endif // READER_HPP_