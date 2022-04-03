//
// Created by mfpr on 4/3/22.
//

#ifndef BRIDGE_DEVICES_HPP
#define BRIDGE_DEVICES_HPP

#if defined(_MSC_VER)
# pragma once
#endif

#include <vector>

#include <boost/iostreams/detail/ios.hpp> // streamsize.
#include <boost/iostreams/categories.hpp>
#include <boost/iostreams/device/file.hpp>

#include "bridge/global.hpp"

namespace bridge::directory {

    ///
    /// \brief A device that knows its current position.
    ///
    struct bridge_device_sizeable {
        [[nodiscard]] virtual size_t tellp() const = 0;
    };

    template<typename T>
    class back_insert_vector : public bridge_device_sizeable{
      public:
        typedef typename std::vector<T>::value_type char_type;
        typedef boost::iostreams::sink_tag category;
        [[maybe_unused]] back_insert_vector(std::vector<T> &cnt) // NOLINT(google-explicit-constructor)
            : container(&cnt) {} // NOLINT(google-explicit-constructor)
        std::streamsize write(const char_type *s, std::streamsize n) {
            container->insert(container->end(), s, s + n);
            return n;
        }
        [[nodiscard]] size_t tellp() const override { return container->size(); }

      protected:
        std::vector<T> *container;
    };

    /// \brief A file device class that is a bridge_device_sizeable.
    /// \tparam Ch
    template<typename Ch>
    struct file_sink_device : public boost::iostreams::basic_file_sink<Ch>, public bridge_device_sizeable {
        [[nodiscard]] size_t tellp() const override { return 0; }
    };

    /// \brief A device array trait class.
    template<typename Device>
    struct is_array_device {
        static const bool value = false;
    };

    template<typename Ch>
    struct is_array_device<file_sink_device<Ch>> {
        static const bool value = false;
    };

    template<typename T>
    struct is_array_device<back_insert_vector<T>> {
        static const bool value = true;
    };

}
#endif // BRIDGE_DEVICES_HPP
