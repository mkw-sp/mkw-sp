from common import *


def unpack_file(in_data, nodes_offset, index):
    content_offset = unpack_u32(in_data, nodes_offset + index * 0xc + 0x4)
    content_size = unpack_u32(in_data, nodes_offset + index * 0xc + 0x8)
    content = in_data[content_offset:content_offset + content_size]
    return {
        'content': content,
    }, index + 1

def unpack_dir(in_data, nodes_offset, names_offset, index):
    next_index = unpack_u32(in_data, nodes_offset + index * 0xc + 0x8)
    index += 1
    children = []
    while index < next_index:
        child, index = unpack_node(in_data, nodes_offset, names_offset, index)
        children += [child]
    return {
        'children': children,
    }, index

def unpack_node(in_data, nodes_offset, names_offset, index):
    is_dir = unpack_bool8(in_data, nodes_offset + index * 0xc + 0x0)
    name_offset = names_offset + unpack_u32(in_data, nodes_offset + index * 0xc + 0x0) & 0xffffff
    name = in_data[name_offset:].split(b'\0', 1)[0].decode('ascii')
    if is_dir:
        node, index = unpack_dir(in_data, nodes_offset, names_offset, index)
    else:
        node, index = unpack_file(in_data, nodes_offset, index)
    return {
        'is_dir': is_dir,
        'name': name,
        **node,
    }, index

def unpack_u8(in_data):
    nodes_offset = unpack_u32(in_data, 0x4)
    names_offset = nodes_offset + unpack_u32(in_data, nodes_offset + 0x8) * 0xc
    root = unpack_node(in_data, nodes_offset, names_offset, 0x0)[0]
    root = root['children'][0]
    root['name'] = ''
    return root

def process_node(node, index, names, contents):
    node['index'] = index
    node['name_offset'] = names.insert(node['name'])
    if node['is_dir']:
        count = 1
        for i, child in enumerate(node['children']):
            count += process_node(child, index + count, names, contents)
        node['count'] = count
        return count
    else:
        node['content_offset'] = contents.size()
        contents.push(node['content'])
        contents.buffer = contents.buffer.ljust((len(contents.buffer) + 0x1f) & ~0x1f, b'\0')
        return 1

def pack_node(node, contents_offset, parent_index):
    common_data = b''.join([
        pack_bool8(node['is_dir']),
        pack_u32(node['name_offset'])[1:4],
    ])

    if node['is_dir']:
        children_data = b''
        for child in node['children']:
            children_data += pack_node(child, contents_offset, node['index'])
        return b''.join([
            common_data,
            pack_u32(parent_index),
            pack_u32(node['index'] + node['count']),
            children_data,
        ])
    else:
        return b''.join([
            common_data,
            pack_u32(contents_offset + node['content_offset']),
            pack_u32(len(node['content'])),
        ])

def pack_u8(root):
    root['name'] = '.'
    root = {
        'is_dir': True,
        'name': '',
        'children': [root],
    }
    names = Strings('ascii', b'\0')
    contents = Buffer(0x0)
    count = process_node(root, 0x0, names, contents)

    names_offset = 0x20 + count * 0xc
    contents_offset = names_offset + len(names.buffer)
    contents_offset = (contents_offset + 0x1f) & ~0x1f
    names.buffer = names.buffer.ljust(contents_offset - names_offset, b'\0')
    nodes_data = pack_node(root, contents_offset, 0x0)

    return b''.join([
        b'U\xaa8-',
        pack_u32(0x20),
        pack_u32(contents_offset - 0x20),
        pack_u32(contents_offset),
        pack_pad32(None),
        pack_pad32(None),
        pack_pad32(None),
        pack_pad32(None),
        nodes_data,
        names.buffer,
        contents.buffer,
    ])
