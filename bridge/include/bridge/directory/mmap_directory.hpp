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

#include <memory>
#include <shared_mutex>
#include <unordered_map>
#include <filesystem>
#include <utility>

#include "bridge/directory/directory.hpp"
#include "bridge/directory/read_only_source.hpp"
#include "bridge/directory/error.hpp"

#include <boost/iostreams/device/file_descriptor.hpp>
#include <boost/iostreams/stream.hpp>

#ifndef MMAP_DIRECTORY_HPP_
#define MMAP_DIRECTORY_HPP_

namespace bridge::directory {

    /// @brief File descriptor with flush support.
    typedef boost::iostreams::stream<boost::iostreams::file_descriptor_sink> file_descriptor_sink;

    /// @brief A cache that uses memory-mapped files to store the data.
    typedef std::map<Path, std::shared_ptr<mmap_source>> mmap_cache_t;

    /**
     *  @brief Directory storing data in files, read via mmap.
     *  The mmap object are cached to limit  the system calls.
     */
    class MMapDirectory : public Directory<FileDevice, FileSource> {
      public:

        /**
         * @brief Construct a new MMap from temp directory
         */
        explicit MMapDirectory() {
            Path temp_dir =  std::filesystem::temp_directory_path();
            if (!std::filesystem::exists(temp_dir)) {
                std::filesystem::create_directory(temp_dir);
            }
            this->root_ = temp_dir;
        }

        /**
         * @brief Construct a new MMap from existing directory
         */
        explicit MMapDirectory(const Path &root) {
            this->root_ = root;
        }

        /**
         * @brief Virtual destructor for Directory.
         */
        ~MMapDirectory() override {
            mmap_cache_.clear();
            if(std::filesystem::exists(root_)) {
                std::filesystem::remove(root_);
            }
        }

        /**
         * @brief Open a MMapDirectory from a path
         *
         * @return Returns an error if the directory_path does not exist or if it is not a directory.
        */
        static std::unique_ptr<MMapDirectory> open(const Path &directory_path) {
            if (!std::filesystem::exists(directory_path)) {
                throw open_directory_error(open_directory_error_type::directory_not_found);
            }
            if (!std::filesystem::is_directory(directory_path)) {
                throw open_directory_error(open_directory_error_type::not_a_directory);
            }
            return std::make_unique<MMapDirectory>(directory_path);
        }

        /**
         * @brief Operator << for Debbuging purposes.
         */
        friend std::ostream &operator<<(std::ostream &os, const MMapDirectory &dir) {
            os << "MMapDirectory(" << dir.root_ << ")";
            return os;
        }

        /**
         * @brief Joins relative path to the directory root.
         */
        Path join(const Path &path) const { return root_ / path; }

        /**
         * @brief Sync the root directory.
         * In certain file systems, this is required to persistently create a file.
         */
        void sync() const {
            std::filesystem::directory_iterator end;
            for (std::filesystem::directory_iterator it(root_); it != end; ++it) {
                file_descriptor_sink sink {it->path()};
                sink.flush();
            }
        }

        /**
         * @brief Opens a virtual file for read.
         * @details Once  a file is opened, its data may not be modified.
         * Specifically, subsequent write or flush should have  no effect in the object.
         * @return read_only_source Read only source.
         */
        [[nodiscard]] std::shared_ptr<read_only_source> _source(const Path& path) override {

            Path full_path = join(path);

            if (!std::filesystem::exists(full_path) || std::filesystem::is_directory(full_path)) {
                throw file_error("File does not exist or is a directory: " + full_path.string());
            }

            // Check if the file is already in the cache
            auto it = mmap_cache_.find(full_path);
            if (it != mmap_cache_.end()) {
                return it->second;
            }

            // Create a new mmap object from the file if the file size is not 0
            if(std::filesystem::file_size(full_path) == 0) {
                return in_memory_source::empty();
            }

            std::shared_ptr<mmap_source> new_mmap = std::make_shared<mmap_source>(full_path); // new memory map from file
            mmap_cache_.insert({full_path, new_mmap}); // add to cache

            return new_mmap;
        }

        /**
         * @brief Removes a file
         * @details Removing a file will not affect eventual existing read_only_source pointing to it.
         */
        void _remove(const Path& path) override {
            Path full_path = join(path);

            // Remove the entry in the mmap cache.
            mmap_cache_.erase(full_path);

            // Remove the file
            std::filesystem::remove(full_path);

            // flush.
            sync();
        }

        /**
         * @brief Opens a virtual file for write.
         * @details Once a file is opened, its data may not be modified.
         * Specifically, subsequent write or flush should have  no effect in the object.
         * @return Writer.
         */
        [[nodiscard]] std::unique_ptr<FileWriter> _open_write(const Path& path) override {
            Path full_path = join(path);

            //  Check if file is valid and open for write
            if (std::filesystem::exists(full_path)) {
                throw file_error("File already exists");
            }

            // Open the file for writing
            FileDevice device(full_path, std::ios::out | std::ios::binary);
            std::unique_ptr<FileWriter> writer =  std::make_unique<FileWriter>(device);

            writer->flush(); // Make sure the file is created.

            return writer;
        }

        /**
         * @brief Opens a virtual file for read.
         * @details Once a file is opened, its data may not be modified.
         * @return Reader.
         */
        [[nodiscard]] std::shared_ptr<FileReader> _open_read(const Path& path) override {
            Path full_path = join(path);

            //  Check if file is valid and open for write
            if (! std::filesystem::exists(full_path)) {
                throw file_error("File doesn't exists");
            }

            // Open the file for writing
            FileSource source(full_path, std::ios::in | std::ios::binary);
            std::shared_ptr<FileReader> reader =  std::make_shared<FileReader>(source);

            return reader;
        }

        /**
         * @brief Atomically replace the content  of a file by data.
         * This calls ensure that reads can never 'observe' a partially written file.
         * The file may or may not previously exist.
         */
        void _replace_content(const Path &path, const bridge::byte_t *data, std::streamsize length) override {
            Path full_path = join(path);

            if (std::filesystem::is_directory(full_path)) {
                throw file_error("Cannot replace a directory");
            }

            if(std::filesystem::exists(full_path)) {
                std::filesystem::remove(full_path);
            }

            FileDevice device(full_path, std::ios::out | std::ios::binary);
            FileWriter writer{device};
            writer.write(data, length);

        }

      private:
        Path root_;
        mutable mmap_cache_t mmap_cache_;
    };
} // namespace bridge::directory
#endif
