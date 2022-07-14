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

#ifndef RAM_DIRECTORY_HPP_
#define RAM_DIRECTORY_HPP_

#include <memory>
#include <map>
#include <vector>

#include "bridge/directory/directory.hpp"

namespace bridge::directory {

    /// @brief A cache directory that stores data in RAM.
    typedef std::map<Path, std::vector<bridge::byte_t>> ram_cache_t;

    /// @brief A RAM directory.
    template <bool is_safe_lock = false>
    class RAMDirectory : public Directory<ArrayDevice, ArraySource, is_safe_lock> {
      public:

        /**
         * @brief Construct a new RAMDirectory.
         */
        explicit RAMDirectory() = default;

        /**
         * @brief Virtual destructor for Directory.
         */
        ~RAMDirectory() override {
            ram_cache_.clear();
        }

        /**
         * @brief Operator << for Debbuging purposes.
         */
        friend std::ostream &operator<<(std::ostream &os, const RAMDirectory &dir) {
            os << "RAMDirectory" << std::endl;
            return os;
        }

        /**
         * @brief Opens a virtual file for read.
         * @details Once  a file is opened, its data may not be modified.
         * Specifically, subsequent write or flush should have  no effect in the object.
         * @return read_only_source Read only source.
         */
        [[nodiscard]] std::shared_ptr<read_only_source> _source(const Path& path) override {

            // Check if the file is in the cache
            auto it = ram_cache_.find(path);
            if (it == ram_cache_.end()) {
                throw io_error("File not found: " + path.string());
            }
            return std::make_shared<in_memory_source>(it->second.data(), it->second.size());
        }

        /**
         * @brief Removes a file
         * @details Removing a file will not affect eventual existing read_only_source pointing to it.
         */
        void _remove(const Path& path) override {
            // Check if the file is in the cache
            auto it = ram_cache_.find(path);
            if (it == ram_cache_.end()) {
                throw file_error("File does not exist: " + path.string());
            }

            // Remove the file from the cache
            ram_cache_.erase(it);
        }

        /**
         * @brief Opens a virtual file for write.
         * @details Once a file is opened, its data may not be modified.
         * Specifically, subsequent write or flush should have  no effect in the object.
         * @return Writer.
         */
        [[nodiscard]] std::unique_ptr<ArrayWriter> _open_write(const Path& path) override {

            // Check if the file is in the cache
            auto it = ram_cache_.find(path);
            if (it != ram_cache_.end()) {
                throw file_error("File already exists: " + path.string());
            }

            // Add the file to the cache
            ram_cache_[path] = std::vector<bridge::byte_t>();

            // open file  for write
            ArrayDevice device(ram_cache_[path]);
            std::unique_ptr<ArrayWriter> writer = std::make_unique<ArrayWriter>(device);

            return writer;
        }

        /**
         * @brief Opens a virtual file for read.
         * @return Reader.
         */
        [[nodiscard]] std::shared_ptr<ArrayReader> _open_read(const Path& path) override {

            // Check if the file is in the cache
            auto it = ram_cache_.find(path);
            if (it == ram_cache_.end()) {
                throw file_error("File doesn't exists:" + path.string());
            }

            // open file  for write
            std::vector<bridge::byte_t>* data = &it->second;
            ArraySource source(data->data(), data->size());
            std::shared_ptr<ArrayReader> reader = std::make_shared<ArrayReader>(source);

            return reader;
        }

        /**
         * @brief Atomically replace the content  of a file by data.
         * This calls ensure that reads can never 'observe' a partially written file.
         * The file may or may not previously exist.
         */
        void _replace_content(const Path &path, const bridge::byte_t *data, std::streamsize length) override {
            // Check if the file is in the cache
            auto it = ram_cache_.find(path);
            if(it != ram_cache_.end()) {
                // Replace the file in the cache
                ram_cache_[path] = std::vector<bridge::byte_t>(data, data + length);
            } else {
                // Add the file to the cache
                ram_cache_[path] = std::vector<bridge::byte_t>(data, data + length);
            }
        }

      private:
        mutable ram_cache_t ram_cache_;
    };

} // namespace bridge::directory

#endif // RAM_DIRECTORY_HPP_