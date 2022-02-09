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

    class RAMDirectory : public Directory<ArrayDevice> {
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
        [[nodiscard]] std::shared_ptr<read_only_source> open_read(const Path& path) const override {

            // Lock that allows to read the cache
            std::shared_lock lock(mutex_);

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
        void remove(const Path& path) override {
            std::unique_lock lock(mutex_);

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
        [[nodiscard]] std::unique_ptr<ArrayWriter> open_write(const Path& path) override {
            std::unique_lock lock(mutex_);

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
         * @brief Atomically replace the content  of a file by data.
         * This calls ensure that reads can never 'observe' a partially written file.
         * The file may or may not previously exist.
         */
        void replace_content(const Path &path, const bridge::byte_t *data, std::streamsize length) override {
            // Lock single writer
            std::unique_lock lock(mutex_);

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
        mutable std::shared_mutex mutex_;
    };

} // namespace bridge::directory

#endif // RAM_DIRECTORY_HPP_