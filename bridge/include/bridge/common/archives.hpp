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

#ifndef BRIDGE_CEREAL_ARCHIVES_BINARY_HPP_
#define BRIDGE_CEREAL_ARCHIVES_BINARY_HPP_

#include "cereal/cereal.hpp"
#include <sstream>

namespace bridge {
    namespace archive {
        /**
         * @brief Custom bridge serialization error.
         *
         */
        class archive_error : public std::runtime_error {
          public:
            explicit archive_error(const std::string &what) : std::runtime_error(what) {}
        };
        // ######################################################################
        //! An output archive designed to save data in a compact binary representation
        /*! This archive outputs data to a stream in an extremely compact binary
            representation with as little extra metadata as possible.

            This archive does nothing to ensure that the endianness of the saved
            and loaded data is the same.  If you need to have portability over
            architectures with different endianness, use PortableBinaryOutputArchive.

            When using a binary archive and a file stream, you must use the
            std::ios::binary format flag to avoid having your data altered
            inadvertently.

            \ingroup Archives */
        class BinaryOutput : public cereal::OutputArchive<BinaryOutput, cereal::AllowEmptyClassElision> {
          public:
            //! Construct, outputting to the provided stream
            /*! @param stream The stream to output to.  Can be a stringstream, a file stream, or
                              even cout! */
            BinaryOutput(std::ostream &stream)
                : cereal::OutputArchive<BinaryOutput, cereal::AllowEmptyClassElision>(this), itsStream(stream),
                  total_written(0) {}

            ~BinaryOutput() CEREAL_NOEXCEPT = default;

            //! Writes size bytes of data to the output stream
            void saveBinary(const void *data, std::streamsize size) {
                auto const writtenSize = itsStream.rdbuf()->sputn(reinterpret_cast<const char *>(data), size);
                total_written += writtenSize;
                if (writtenSize != size)
                    throw archive_error("Failed to write " + std::to_string(size) + " bytes to output stream! Wrote " +
                                        std::to_string(writtenSize));
            }

            size_t totalWritten() const { return total_written; }

            void flush() {
                total_written = 0;
                itsStream.flush();
            }

          private:
            size_t total_written;
            std::ostream &itsStream;
        };

        // ######################################################################
        //! An input archive designed to load data saved using BinaryOutputArchive
        /*  This archive does nothing to ensure that the endianness of the saved
            and loaded data is the same.  If you need to have portability over
            architectures with different endianness, use PortableBinaryOutputArchive.

            When using a binary archive and a file stream, you must use the
            std::ios::binary format flag to avoid having your data altered
            inadvertently.

            \ingroup Archives */
        class BinaryInput : public cereal::InputArchive<BinaryInput, cereal::AllowEmptyClassElision> {
          public:
            //! Construct, loading from the provided stream
            BinaryInput(std::istream &stream)
                : InputArchive<BinaryInput, cereal::AllowEmptyClassElision>(this), itsStream(stream) {}

            ~BinaryInput() CEREAL_NOEXCEPT = default;

            //! Reads size bytes of data from the input stream
            void loadBinary(void *const data, std::streamsize size) {
                auto const readSize = itsStream.rdbuf()->sgetn(reinterpret_cast<char *>(data), size);

                if (readSize != size)
                    throw archive_error("Failed to read " + std::to_string(size) + " bytes from input stream! Read " +
                                        std::to_string(readSize));
            }

          private:
            std::istream &itsStream;
        };

        // ######################################################################
        // Common BinaryArchive serialization functions

        //! Saving for POD types to binary
        template <class T>
        inline typename std::enable_if<std::is_arithmetic<T>::value, void>::type
        CEREAL_SAVE_FUNCTION_NAME(BinaryOutput &ar, T const &t) {
            ar.saveBinary(std::addressof(t), sizeof(t));
        }

        //! Loading for POD types from binary
        template <class T>
        inline typename std::enable_if<std::is_arithmetic<T>::value, void>::type
        CEREAL_LOAD_FUNCTION_NAME(BinaryInput &ar, T &t) {
            ar.loadBinary(std::addressof(t), sizeof(t));
        }

        //! Serializing NVP types to binary
        template <class Archive, class T>
        inline CEREAL_ARCHIVE_RESTRICT(BinaryInput, BinaryOutput)
            CEREAL_SERIALIZE_FUNCTION_NAME(Archive &ar, cereal::NameValuePair<T> &t) {
            ar(t.value);
        }

        //! Serializing SizeTags to binary
        template <class Archive, class T>
        inline CEREAL_ARCHIVE_RESTRICT(BinaryInput, BinaryOutput)
            CEREAL_SERIALIZE_FUNCTION_NAME(Archive &ar, cereal::SizeTag<T> &t) {
            ar(t.size);
        }

        //! Saving binary data
        template <class T> inline void CEREAL_SAVE_FUNCTION_NAME(BinaryOutput &ar, cereal::BinaryData<T> const &bd) {
            ar.saveBinary(bd.data, static_cast<std::streamsize>(bd.size));
        }

        //! Loading binary data
        template <class T> inline void CEREAL_LOAD_FUNCTION_NAME(BinaryInput &ar, cereal::BinaryData<T> &bd) {
            ar.loadBinary(bd.data, static_cast<std::streamsize>(bd.size));
        }
    } // namespace archive
} // namespace bridge

// register archives for polymorphic support
CEREAL_REGISTER_ARCHIVE(bridge::archive::BinaryOutput)
CEREAL_REGISTER_ARCHIVE(bridge::archive::BinaryInput)

// tie input and output archives together
CEREAL_SETUP_ARCHIVE_TRAITS(bridge::archive::BinaryInput, bridge::archive::BinaryOutput)
CEREAL_SPECIALIZE_FOR_ALL_ARCHIVES(bridge::archive::BinaryOutput, cereal::specialization::member_load_save)

#endif // BRIDGE_CEREAL_ARCHIVES_BINARY_HPP_
