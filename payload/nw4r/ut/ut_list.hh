#pragma once

#include <Common.hh>

namespace nw4r::ut {

//! Bidirectional list node
struct Node {
    void *pred;
    void *succ;
};

class List {
public:
    void append(void *elem);
    void remove(void *elem);

private:
    Node *m_head;
    Node *m_tail;
    u16 m_count;
    u16 m_offset;
};

static_assert(sizeof(List) == 0xc);

} // namespace nw4r::ut
