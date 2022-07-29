class Table {
    constructor(view, offset) {
        this.table = new DataView(view.buffer, view.byteOffset + offset, view.byteLength - offset);
    }

    entry_count() {
        return this.table.getUint32(0x0);
    }

    entry_offset(entry_id) {
        return this.table.getUint32(0x4 + entry_id * 0x8 + 0x4);
    }
}

class SoundInfo {
    constructor(view, offset) {
        this.sound_info = new DataView(view.buffer, view.byteOffset + offset, 0x2C);
    }

    string_id() {
        return this.sound_info.getUint32(0x0);
    }

    file_id() {
        return this.sound_info.getUint32(0x4);
    }
}

class BankInfo {
    constructor(view, offset) {
        this.bank_info = new DataView(view.buffer, view.byteOffset + offset, 0xC);
    }

    string_id() {
        return this.bank_info.getUint32(0x0);
    }

    file_id() {
        return this.bank_info.getUint32(0x4);
    }
}

class FilePos {
    constructor(view, offset) {
        this.file_pos = new DataView(view.buffer, view.byteOffset + offset, 0x8);
    }

    group_id() {
        return this.file_pos.getUint32(0x0);
    }

    index() {
        return this.file_pos.getUint32(0x4);
    }
}

class FileInfo {
    constructor(view, offset) {
        this.file_info = new DataView(view.buffer, view.byteOffset + offset, 0x1C);
    }

    ext_file_path_offset() {
        return this.file_info.getUint32(0x10);
    }

    file_pos_table_offset() {
        return this.file_info.getUint32(0x18);
    }
}

class GroupInfo {
    constructor(view, offset) {
        this.group_info = new DataView(view.buffer, view.byteOffset + offset, 0x28);
    }

    string_id() {
        return this.group_info.getUint32(0x00);
    }

    offset() {
        return this.group_info.getUint32(0x10);
    }

    wave_data_offset() {
        return this.group_info.getUint32(0x18);
    }

    item_info_table_offset() {
        return this.group_info.getUint32(0x24);
    }
}

class GroupItemInfo {
    constructor(view, offset) {
        this.group_item_info = new DataView(view.buffer, view.byteOffset + offset, 0x14);
    }

    file_id() {
        return this.group_item_info.getUint32(0x00);
    }

    offset() {
        return this.group_item_info.getUint32(0x04);
    }

    size() {
        return this.group_item_info.getUint32(0x08);
    }

    wave_data_offset() {
        return this.group_item_info.getUint32(0x0C);
    }

    wave_data_size() {
        return this.group_item_info.getUint32(0x10);
    }
}

class BRSAR {
    constructor(buffer) {
        this.brsar = new DataView(buffer);
        if (this.brsar.getUint32(0x00) !== 0x52534152) {
            throw 'Incorrect magic!';
        }
        const symb_offset = this.brsar.getUint32(0x10);
        const symb_size = this.brsar.getUint32(0x14);
        this.symb = new DataView(buffer, symb_offset, symb_size);
        const info_offset = this.brsar.getUint32(0x18);
        const info_size = this.brsar.getUint32(0x1C);
        this.info = new DataView(buffer, info_offset, info_size);
        const sound_info_table_offset = 0x8 + this.info.getUint32(0xC);
        this.sound_info_table = new Table(this.info, sound_info_table_offset);
        const bank_info_table_offset = 0x8 + this.info.getUint32(0x14);
        this.bank_info_table = new Table(this.info, bank_info_table_offset);
        const file_info_table_offset = 0x8 + this.info.getUint32(0x24);
        this.file_info_table = new Table(this.info, file_info_table_offset);
        const group_info_table_offset = 0x8 + this.info.getUint32(0x2C);
        this.group_info_table = new Table(this.info, group_info_table_offset);
    }

    string(string_id) {
        if (string_id === 0xFFFFFFFF) {
            return '';
        }
        const string_table_offset = 0x8 + this.symb.getUint32(0x8);
        const string_entry_offset = 0x4 + string_table_offset + 0x4 * string_id;
        const string_offset = 0x8 + this.symb.getUint32(string_entry_offset);
        let string_size = 0;
        while (this.symb.getUint8(string_offset + string_size) != 0) {
            string_size++;
        }
        const string = new DataView(this.symb.buffer, this.symb.byteOffset + string_offset,
                string_size);
        return new TextDecoder('ascii').decode(string);
    }

    sound_count() {
        return this.sound_info_table.entry_count();
    }

    sound_info(sound_id) {
        const sound_info_offset = 0x8 + this.sound_info_table.entry_offset(sound_id);
        return new SoundInfo(this.info, sound_info_offset);
    }

    bank_count() {
        return this.bank_info_table.entry_count();
    }

    bank_info(bank_id) {
        const bank_info_offset = 0x8 + this.bank_info_table.entry_offset(bank_id);
        return new BankInfo(this.info, bank_info_offset);
    }

    file_count() {
        return this.file_info_table.entry_count();
    }

    file_info(file_id) {
        const file_info_offset = 0x8 + this.file_info_table.entry_offset(file_id);
        return new FileInfo(this.info, file_info_offset);
    }

    group_count() {
        return this.group_info_table.entry_count();
    }

    group_info(group_id) {
        const group_info_offset = 0x8 + this.group_info_table.entry_offset(group_id);
        return new GroupInfo(this.info, group_info_offset);
    }

    group_item_info(group_id, index) {
        const group_info = this.group_info(group_id);
        const group_item_info_table_offset = 0x8 + group_info.item_info_table_offset();
        const group_item_info_table = new Table(this.info, group_item_info_table_offset);
        const group_item_info_offset = 0x8 + group_item_info_table.entry_offset(index);
        return new GroupItemInfo(this.info, group_item_info_offset);
    }
}

document.getElementById('open').onclick = () => {
    const input = document.createElement('input');
    input.type = 'file';

    input.onchange = e => {
        const file = e.target.files[0];
        const output = document.getElementById('output');
        output.innerText = 'Loading ' + file.name + '...';
        const reader = new FileReader();
        reader.onload = e => {
            try {
                const new_output = document.createElement('div');
                const brsar = new BRSAR(e.target.result);

                const file_sounds = Array.from(Array(brsar.file_count()), () => new Array(0));
                for (let sound_id = 0; sound_id < brsar.sound_count(); sound_id++) {
                    const sound_info = brsar.sound_info(sound_id);
                    const string = brsar.string(sound_info.string_id());
                    file_sounds[sound_info.file_id()].push(string);
                }

                const file_banks = Array.from(Array(brsar.file_count()), () => new Array(0));
                for (let bank_id = 0; bank_id < brsar.bank_count(); bank_id++) {
                    const bank_info = brsar.bank_info(bank_id);
                    const string = brsar.string(bank_info.string_id());
                    file_banks[bank_info.file_id()].push(string);
                }

                const file_groups = Array.from(Array(brsar.file_count()), () => new Array(0));
                for (let group_id = 0; group_id < brsar.group_count(); group_id++) {
                    const group_info = brsar.group_info(group_id);
                    const string = brsar.string(group_info.string_id());
                    const group_item_info_table_offset = 0x8 + group_info.item_info_table_offset();
                    const group_item_info_table = new Table(brsar.info,
                            group_item_info_table_offset);
                    for (let index = 0; index < group_item_info_table.entry_count(); index++) {
                        const group_item_info = brsar.group_item_info(group_id, index);
                        file_groups[group_item_info.file_id()].push(string);
                    }
                }

                for (let file_id = 0; file_id < brsar.file_count(); file_id++) {
                    const file_info = brsar.file_info(file_id)
                    const file_pos_table_offset = 0x8 + file_info.file_pos_table_offset();
                    const file_pos_table = new Table(brsar.info, file_pos_table_offset);
                    if (file_pos_table.entry_count() === 0) {
                        continue;
                    }
                    const p = document.createElement('p');
                    const file_pos = new FilePos(brsar.info, 0x8 + file_pos_table.entry_offset(0));
                    const group_id = file_pos.group_id();
                    const index = file_pos.index();
                    const group_info = brsar.group_info(group_id);
                    const group_item_info = brsar.group_item_info(group_id, index);

                    const offset = group_info.offset() + group_item_info.offset();
                    const size = group_item_info.size();
                    if (size !== 0) {
                        const magic = brsar.brsar.getUint32(offset);
                        const ext = {
                            0x52534551: 'brseq',
                            0x52575344: 'brwsd',
                            0x52424E4B: 'brbnk',
                        }[magic];
                        if (ext === undefined) {
                            throw 'Unexpected magic ' + magic.toString(16) + '!';
                        }
                        const name = file_id + '.' + ext;
                        const button = document.createElement('button');
                        button.innerText = name;
                        button.onclick = () => {
                            const view = new DataView(brsar.brsar.buffer, offset, size);
                            const blob = new Blob([view], {type: 'application/octet-stream'});
                            const url = URL.createObjectURL(blob);
                            const a = document.createElement('a');
                            a.href = url;
                            a.download = name;
                            a.click();
                            URL.revokeObjectURL(url);
                        };
                        p.appendChild(button);
                    }

                    const wave_data_offset = group_info.wave_data_offset() +
                            group_item_info.wave_data_offset();
                    const wave_data_size = group_item_info.wave_data_size();
                    if (wave_data_size !== 0) {
                        p.appendChild(document.createTextNode(' - '));
                        const magic = brsar.brsar.getUint32(wave_data_offset);
                        if (magic !== 0x52574152) {
                            throw 'Unexpected magic ' + magic.toString(16) + '!';
                        }
                        const name = file_id + '.brwar';
                        const button = document.createElement('button');
                        button.innerText = name;
                        button.onclick = () => {
                            const view = new DataView(brsar.brsar.buffer, wave_data_offset,
                                    wave_data_size);
                            const blob = new Blob([view], {type: 'application/octet-stream'});
                            const url = URL.createObjectURL(blob);
                            const a = document.createElement('a');
                            a.href = url;
                            a.download = name;
                            a.click();
                            URL.revokeObjectURL(url);
                        };
                        p.appendChild(button);
                    }

                    const sound_count = file_sounds[file_id].length;
                    if (sound_count !== 0) {
                        p.appendChild(document.createTextNode(' - '));
                        const span = document.createElement('span');
                        span.innerText = sound_count + (sound_count === 1 ? ' sound' : ' sounds');
                        span.title = file_sounds[file_id].slice(0, 50).join('\n');
                        if (sound_count > 50) {
                            span.title += '\nand ' + (sound_count - 50) + ' more';
                        }
                        p.appendChild(span);
                    }

                    const bank_count = file_banks[file_id].length;
                    if (bank_count !== 0) {
                        p.appendChild(document.createTextNode(' - '));
                        const span = document.createElement('span');
                        span.innerText = bank_count + (bank_count === 1 ? ' bank' : ' banks');
                        span.title = file_banks[file_id].slice(0, 50).join('\n');
                        if (bank_count > 50) {
                            span.title += '\nand ' + (bank_count - 50) + ' more';
                        }
                        p.appendChild(span);
                    }

                    const group_count = file_groups[file_id].length;
                    if (group_count !== 0) {
                        p.appendChild(document.createTextNode(' - '));
                        const span = document.createElement('span');
                        span.innerText = group_count + (group_count === 1 ? ' group' : ' groups');
                        span.title = file_groups[file_id].slice(0, 50).join('\n');
                        if (group_count > 50) {
                            span.title += '\nand ' + (group_count - 50) + ' more';
                        }
                        p.appendChild(span);
                    }

                    new_output.appendChild(p);
                }
                new_output.id = 'output';
                output.replaceWith(new_output);
            } catch (e) {
                output.innerText = 'Failed to load ' + file.name + ': ' + e;
            }
        };
        reader.readAsArrayBuffer(file);
    };

    input.click();
};
