#pragma once

#include "sp/storage/Storage.hh"

#include <ranges>

namespace SP::Storage {

/// Provides a range-based interface for iterating over directory entries.
/// The directory entries are represented by NodeInfo objects. This class is designed
/// to be used with C++ for-each loops and other range-based algorithms.
///
/// The range is backed by a DirHandle object, which is responsible for the low-level
/// interaction with the directory.
///
/// @note DirEntryRange uses an input iterator for traversal. This means it is a single-pass
/// iterator that can be incremented, but not decremented. It can only be used in
/// a limited subset of the C++ algorithms.
///
/// Similar API to std::directory_iterator
/// https://en.cppreference.com/w/cpp/filesystem/directory_iterator
///
class DirEntryRange {
private:
    class sentinel {};

    class iterator {
    public:
        using iterator_category = std::input_iterator_tag;
        using difference_type = std::ptrdiff_t;
        using value_type = NodeInfo;
        using pointer = NodeInfo *;
        using reference = NodeInfo &;

        explicit iterator(DirHandle &dirHandle) : m_dirHandle(&dirHandle) {
            m_node = m_dirHandle->read();
        }

        iterator() : m_dirHandle(nullptr) {}

        const value_type &operator*() const {
            return *m_node;
        }

        iterator &operator++() {
            assert(m_dirHandle);
            m_node = m_dirHandle->read();
            return *this;
        }

        iterator operator++(int) {
            assert(m_dirHandle);
            iterator tmp(*this);
            operator++();
            return tmp;
        }

        bool operator==(const sentinel &) const {
            return !m_node.has_value();
        }
        bool operator!=(const sentinel &) const {
            return m_node.has_value();
        }

    private:
        DirHandle *m_dirHandle;
        std::optional<value_type> m_node;
    };

    static_assert(std::input_iterator<iterator>);

public:
    DirEntryRange(DirHandle &&dirHandle) : m_dirHandle(std::move(dirHandle)) {}

    iterator begin() {
        return iterator(m_dirHandle);
    }

    sentinel end() {
        return {};
    }

private:
    DirHandle m_dirHandle;
};

static_assert(std::ranges::input_range<DirEntryRange>,
        "DirEntryRange is not an STL-compatible input range");

} // namespace SP::Storage
