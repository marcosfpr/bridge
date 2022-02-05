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

#include "bridge/directory/directory.hpp"
#include "bridge/directory/read_only_source.hpp"

#ifndef MMAP_DIRECTORY_HPP_
#define MMAP_DIRECTORY_HPP_

namespace bridge::directory {
    /**
     *  @brief Directory storing data in files, read via mmap.
     *  The mmap object are cached to limit  the system calls.
     */
    class MMapDirectory : public Directory {
      public:

      private:
        Path root_;
        std::unordered_map<Path, std::shared_ptr<read_only_source>> mmap_cache_;
        std::shared_ptr<std::optional<Path>> temp_file_;
        mutable std::shared_mutex mutex_;
    };
} // namespace bridge::directory
#endif
