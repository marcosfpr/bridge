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

#ifndef DIRECTORY_HPP_
#define DIRECTORY_HPP_

#include <filesystem>
#include <utility>
#include <vector>
#include <shared_mutex>

#include <boost/iostreams/device/file.hpp>
#include <boost/iostreams/stream.hpp>
#include <boost/iostreams/device/array.hpp>
#include <boost/iostreams/device/back_inserter.hpp>

#include "bridge/global.hpp"
#include "bridge/directory/error.hpp"
#include "bridge/directory/read_only_source.hpp"

namespace bridge::directory {

    using Path = std::filesystem::path;

    template<typename Device> using Writer = boost::iostreams::stream<Device>;
    template<typename Source> using Reader = boost::iostreams::stream<Source>;

    using FileDevice = boost::iostreams::basic_file_sink<bridge::byte_t>;
    using ArrayDevice = boost::iostreams::back_insert_device<std::vector<bridge::byte_t>>;

    using FileSource = boost::iostreams::basic_file_source<bridge::byte_t>;
    using ArraySource = boost::iostreams::array_source;

    using ArrayWriter = Writer<ArrayDevice>;
    using FileWriter = Writer<FileDevice>;

    using FileReader = Reader<FileSource>;
    using ArrayReader = Reader<ArraySource>;


    /**
     * @brief Write-once many read (WORM) abstraction for where bridge's index should be stored.
     *
     * @details There is actually two implementations of Directory:
     * 1. The MMapDirectory, which uses mmap() to map the index into memory.
     * 2. The RAMDirectory, which is a test functionality that stores the index in RAM.
     */
     template<typename Device, typename Source>
    class Directory {
      public:
        /**
         * @brief Virtual destructor for Directory.
         */
        virtual ~Directory() = default;

        /**
         * @brief Operator << for Debbuging purposes.
         */
        friend std::ostream &operator<<(std::ostream &os, const Directory &dir) {
            throw std::runtime_error("No implementation for operator<< for Directory was found.");
        }

        /**
         * @brief Opens a virtual file for read.
         * @details Once  a file is opened, its data may not be modified.
         * Specifically, subsequent write or flush should have  no effect in the object.
         * @return read_only_source Read only source.
         */
        [[nodiscard]] std::shared_ptr<read_only_source> source(const Path& path)  {
            if(!mutex_.try_lock_shared())
                throw open_directory_error(directory_already_locked);
            is_open_read = true;
            return _source(path);
        }

        /**
         * @brief Removes a file
         * @details Removing a file will not affect eventual existing read_only_source pointing to it.
         */
        void remove(const Path& path){
            if(!mutex_.try_lock())
                throw open_directory_error(directory_already_locked);
            _remove(path);
            mutex_.unlock();
        }

        /**
         * @brief Open the directory for write.
         * @return Writer stream.
         */
        [[nodiscard]] std::unique_ptr<Writer<Device>> open_write(const Path& path) {
            if(!mutex_.try_lock())
                throw open_directory_error(directory_already_locked);
            is_open_write = true;
            return _open_write(path);
        }


        /**
         * @brief Open the directory for read.
         * @return Reader stream.
         */
        [[nodiscard]] std::shared_ptr<Reader<Source>> open_read(const Path& path) {
            if(!mutex_.try_lock_shared())
                throw open_directory_error(directory_already_locked);
            is_open_read = true;
            return _open_read(path);
        }

        /**
         * @brief Atomically replace the content  of a file by data.
         * This calls ensure that reads can never 'observe' a partially written file.
         * The file may or may not previously exist.
         */
        void replace_content(const Path &path, const bridge::byte_t *data, std::streamsize length) {
            if(!mutex_.try_lock())
                throw open_directory_error(directory_already_locked);
            _replace_content(path, data, length);
            mutex_.unlock();
        }

        /**
         * @brief Close the directory if its open.
         * @return Bool if the directory was closed successfully
         */
        bool close() {
            if (is_open_write) {
                is_open_write = false;
                mutex_.unlock();
                return true;
            }
            else if (is_open_read) {
                is_open_read = false;
                mutex_.unlock_shared();
                return true;
            }
            return false;
        }

        /**
         * @brief Opens a virtual file for read.
         * @details Once  a file is opened, its data may not be modified.
         * Specifically, subsequent write or flush should have  no effect in the object.
         * @return read_only_source Read only source.
         */
        [[nodiscard]] virtual std::shared_ptr<read_only_source> _source(const Path& path) = 0;

        /**
         * @brief Removes a file
         * @details Removing a file will not affect eventual existing read_only_source pointing to it.
         */
        virtual void _remove(const Path& path) = 0;

        /**
         * @brief Open the directory for write.
         * @return Writer stream.
         */
        [[nodiscard]] virtual std::unique_ptr<Writer<Device>> _open_write(const Path& path) = 0;


        /**
         * @brief Open the directory for read.
         * @return Reader stream.
         */
        [[nodiscard]] virtual std::shared_ptr<Reader<Source>> _open_read(const Path& path) = 0;

        /**
         * @brief Atomically replace the content  of a file by data.
         * This calls ensure that reads can never 'observe' a partially written file.
         * The file may or may not previously exist.
         */
        virtual void _replace_content(const Path &path, const bridge::byte_t *data, std::streamsize length) = 0;

      private:
        bool is_open_read{}, is_open_write{};
        mutable std::shared_mutex mutex_;
    };

} // namespace bridge::directory

#endif