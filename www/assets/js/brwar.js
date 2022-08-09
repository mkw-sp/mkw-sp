function align_up(val, alignment) {
    return Math.ceil(val / alignment) * alignment;
}

function nibble_to_sample(nibble) {
    return Math.floor(nibble / 16) * 14 + (nibble % 16 - 2);
}

class ChannelInfo {
    constructor(view, offset) {
        this.channel_info = new DataView(view.buffer, view.byteOffset + offset, 0x1C);
    }

    data_offset() {
        return this.channel_info.getUint32(0x00);
    }

    adpcm_info_offset() {
        return this.channel_info.getUint32(0x04);
    }
}

class ADPCMInfo {
    constructor(view, offset) {
        this.adpcm_info = new DataView(view.buffer, view.byteOffset + offset, 0x30);
    }

    coef(i) {
        return this.adpcm_info.getInt16(0x00 + i * 2);
    }

    predictor() {
        return this.adpcm_info.getUint16(0x22) >> 4;
    }

    scale() {
        return 1 << (this.adpcm_info.getUint16(0x22) & 0xF);
    }

    hist1() {
        return this.adpcm_info.getInt16(0x24);
    }

    hist2() {
        return this.adpcm_info.getInt16(0x26);
    }
}

class BRWAV {
    constructor(view, offset, size) {
        this.brwav = new DataView(view.buffer, view.byteOffset + offset, size);
        if (this.brwav.getUint32(0x00) !== 0x52574156) {
            throw 'Incorrect magic!';
        }
        const info_offset = this.brwav.getUint32(0x10);
        const info_size = this.brwav.getUint32(0x14);
        this.info = new DataView(view.buffer, view.byteOffset + offset + info_offset, info_size);
        const data_offset = this.brwav.getUint32(0x18);
        const data_size = this.brwav.getUint32(0x1C);
        this.data = new DataView(view.buffer, view.byteOffset + offset + data_offset, data_size);
    }

    encoding() {
        return this.info.getUint8(0x8);
    }

    has_loop() {
        return this.info.getUint8(0x9);
    }

    channel_count() {
        return this.info.getUint8(0xA);
    }

    sample_rate() {
        return this.info.getUint8(0xB) << 16 | this.info.getUint16(0xC);
    }

    nibble_count() {
        return this.info.getUint32(0x14);
    }

    sample_count() {
        return nibble_to_sample(this.nibble_count());
    }

    channel_info(channel_id) {
        const channel_info_table_offset = 0x8 + this.info.getUint32(0x18);
        const channel_info_offset = 0x8 +
                this.info.getUint32(channel_info_table_offset + channel_id * 0x4);
        return new ChannelInfo(this.info, channel_info_offset);
    }

    channel_data(channel_id) {
        const data_offset = 0x8 + this.channel_info(channel_id).data_offset();
        const data_size = Math.ceil(this.nibble_count() / 2);
        return new DataView(this.data.buffer, this.data.byteOffset + data_offset, data_size);
    }

    channel_adpcm_info(channel_id) {
        const adpcm_info_offset = 0x8 + this.channel_info(channel_id).adpcm_info_offset();
        return new ADPCMInfo(this.info, adpcm_info_offset);
    }

    decode_adpcm() {
        const encoding = this.encoding();
        if (encoding === 1) {
            throw 'PCM encoding isn\'t supported yet!';
        } else if (encoding === 2) {
            const adpcm_info = [];
            const hist1 = [];
            const hist2 = [];
            for (let channel_id = 0; channel_id < this.channel_count(); channel_id++) {
                adpcm_info.push(this.channel_adpcm_info(channel_id));
                hist1.push(adpcm_info[channel_id].hist1());
                hist2.push(adpcm_info[channel_id].hist2());
            }
            let scale;
            let coef1;
            let coef2;
            const samples = [];
            for (let nibble_id = 0; nibble_id < this.nibble_count(); nibble_id++) {
                const byte_id = Math.floor(nibble_id / 2);
                if (nibble_id % 16 === 0) {
                    scale = [];
                    for (let channel_id = 0; channel_id < this.channel_count(); channel_id++) {
                        scale.push(1 << (this.channel_data(channel_id).getUint8(byte_id) & 0xF));
                    }
                    coef1 = [];
                    coef2 = [];
                    for (let channel_id = 0; channel_id < this.channel_count(); channel_id++) {
                        const predictor = this.channel_data(channel_id).getUint8(byte_id) >> 4;
                        if (nibble_id < 10) {
                        }
                        coef1.push(adpcm_info[channel_id].coef(predictor * 2));
                        coef2.push(adpcm_info[channel_id].coef(predictor * 2 + 1));
                    }
                    continue;
                } else if (nibble_id % 16 === 1) {
                    continue;
                }

                for (let channel_id = 0; channel_id < this.channel_count(); channel_id++) {
                    let adpcm_sample = this.channel_data(channel_id).getUint8(byte_id);
                    adpcm_sample = nibble_id % 2 === 0 ? adpcm_sample >> 4 : adpcm_sample & 0xF;
                    if (adpcm_sample >= 8) {
                        adpcm_sample -= 16;
                    }
                    const distance = scale[channel_id] * adpcm_sample * 2048;
                    const predicted_sample = coef1[channel_id] * hist1[channel_id] +
                            coef2[channel_id] * hist2[channel_id];
                    const corrected_sample = predicted_sample + distance;
                    const scaled_sample = (corrected_sample + 1024) >> 11;
                    const clamped_sample = Math.max(-32768, Math.min(scaled_sample, 32767));
                    hist2[channel_id] = hist1[channel_id];
                    hist1[channel_id] = clamped_sample;
                    samples.push(clamped_sample);
                }
            }
            return samples;
        } else {
            throw 'Unsupported encoding ' + encoding;
        }
    }

    to_wav() {
        const samples = this.decode_adpcm();

        const data_size = this.sample_count() * this.channel_count() * 16 / 8;
        const size = 0x2C + data_size;

        const buffer = new ArrayBuffer(size);
        const view = new DataView(buffer);

        view.setUint32(0x00, 0x52494646); // "RIFF"
        view.setUint32(0x04, size, true);
        view.setUint32(0x08, 0x57415645); // "WAVE"

        view.setUint32(0x0C, 0x666D7420); // "fmt "
        view.setUint32(0x10, 0x10, true); // Chunk size (without the header)
        view.setUint16(0x14, 0x1, true); // MS PCM format
        view.setUint16(0x16, this.channel_count(), true);
        view.setUint32(0x18, this.sample_rate(), true);
        view.setUint32(0x1C, this.sample_rate() * this.channel_count() * 16 / 8, true);
        view.setUint16(0x20, this.channel_count() * 16 / 8, true);
        view.setUint16(0x22, 16, true);

        view.setUint32(0x24, 0x64617461); // "data"
        view.setUint32(0x28, data_size, true);
        samples.forEach((sample, index) => {
            view.setUint16(0x2C + index * 0x2, sample, true);
        });

        return view;
    }

    to_brstm() {
        if (this.encoding() !== 2) {
            throw 'Unsupported encoding ' + this.encoding();
        }
        if (this.channel_count() !== 1 && this.channel_count() !== 2) {
            throw 'Unsupported channel cound ' + this.channel_count();
        }

        const channel_count = this.channel_count();
        const sample_count = this.sample_count();
        const byte_count = Math.ceil(this.nibble_count() / 2);
        const block_count = Math.ceil(sample_count / 0x3800);
        const last_block_size = (byte_count / channel_count - 1) % 0x2000 + 1;
        const samples = this.decode_adpcm();

        const head_offset = 0x40;
        const head_size = align_up(0x70 + channel_count * 0x40, 0x20);
        const adpc_offset = head_offset + head_size;
        const adpc_size = align_up(0x8 + block_count * channel_count * 0x4, 0x20);
        const data_offset = adpc_offset + adpc_size;
        const data_size = 0x20 + align_up(byte_count, 0x20) * channel_count;
        const size = data_offset + data_size;

        const buffer = new ArrayBuffer(size);
        const view = new DataView(buffer);

        view.setUint32(0x00, 0x5253544D); // "RSTM"
        view.setUint16(0x04, 0xFEFF); // Big-endian
        view.setUint8(0x06, 1); // Major version
        view.setUint8(0x07, 0); // Minor version
        view.setUint32(0x08, size);
        view.setUint16(0x0C, 0x40); // Header size
        view.setUint16(0x0E, 2); // Section count (shouldn't this be 3?)
        view.setUint32(0x10, head_offset);
        view.setUint32(0x14, head_size);
        view.setUint32(0x18, adpc_offset);
        view.setUint32(0x1C, adpc_size);
        view.setUint32(0x20, data_offset);
        view.setUint32(0x24, data_size);

        view.setUint32(head_offset + 0x00, 0x48454144); // "HEAD"
        view.setUint32(head_offset + 0x04, head_size);
        view.setUint32(head_offset + 0x08, 0x01000000);
        view.setUint32(head_offset + 0x0C, 0x18) // Offset to first subsection
        view.setUint32(head_offset + 0x10, 0x01000000);
        view.setUint32(head_offset + 0x14, 0x4C); // Offset to second subsection
        view.setUint32(head_offset + 0x18, 0x01000000);
        view.setUint32(head_offset + 0x1C, 0x64); // Offset to third subsection

        view.setUint32(head_offset + 0x20, this.info.getUint32(0x8));
        view.setUint16(head_offset + 0x24, this.info.getUint16(0xC));
        view.setUint32(head_offset + 0x28, nibble_to_sample(this.info.getUint32(0x10)));
        view.setUint32(head_offset + 0x2C, sample_count);
        view.setUint32(head_offset + 0x30, data_offset + 0x20);
        view.setUint32(head_offset + 0x34, block_count);
        view.setUint32(head_offset + 0x38, 0x2000);
        view.setUint32(head_offset + 0x3C, 0x3800);
        view.setUint32(head_offset + 0x40, last_block_size);
        view.setUint32(head_offset + 0x44, (sample_count - 1) % 0x3800 + 1);
        view.setUint32(head_offset + 0x48, align_up(last_block_size, 0x20));
        view.setUint32(head_offset + 0x4C, 0x3800);
        view.setUint32(head_offset + 0x50, 4);

        view.setUint8(head_offset + 0x54, 1); // Track count
        view.setUint8(head_offset + 0x55, 1); // Track info type
        view.setUint32(head_offset + 0x58, 0x01010000);
        view.setUint32(head_offset + 0x5C, 0x58);
        view.setUint8(head_offset + 0x60, 127); // Volume
        view.setUint8(head_offset + 0x61, 64); // Pan
        view.setUint8(head_offset + 0x68, channel_count);
        view.setUint8(head_offset + 0x69, 0); // First channel id
        view.setUint8(head_offset + 0x6A, channel_count - 1); // Second channel id

        view.setUint8(head_offset + 0x6C, channel_count);
        for (let channel_id = 0; channel_id < channel_count; channel_id++) {
            const channel_info_offset = 0x70 + channel_count * 0x8 + channel_id * 0x38;
            view.setUint32(head_offset + 0x70 + channel_id * 0x8 + 0x0, 0x01000000);
            view.setUint32(head_offset + 0x70 + channel_id * 0x8 + 0x4, channel_info_offset - 0x8);
            const adpcm_info_offset = channel_info_offset + 0x8;
            view.setUint32(head_offset + channel_info_offset + 0x00, 0x01000000);
            view.setUint32(head_offset + channel_info_offset + 0x04, adpcm_info_offset - 0x8);
            const adpcm_info = this.channel_adpcm_info(channel_id);
            for (let offset = 0x0; offset < 0x30; offset += 0x4) {
                view.setUint32(head_offset + adpcm_info_offset + offset,
                        adpcm_info.adpcm_info.getUint32(offset));
            }
        }

        view.setUint32(adpc_offset + 0x0, 0x41445043); // "ADPC"
        view.setUint32(adpc_offset + 0x4, adpc_size);
        for (let channel_id = 0; channel_id < channel_count; channel_id++) {
            const hist1_offset = 0x8 + channel_id * 0x4 + 0x0;
            view.setInt16(adpc_offset + hist1_offset, this.channel_adpcm_info(channel_id).hist1());
            const hist2_offset = 0x8 + channel_id * 0x4 + 0x2;
            view.setInt16(adpc_offset + hist2_offset, this.channel_adpcm_info(channel_id).hist2());
        }
        for (let block_id = 1; block_id < block_count; block_id++) {
            for (let channel_id = 0; channel_id < channel_count; channel_id++) {
                const hist1_index = (block_id * 0x3800 - 1) * channel_count + channel_id;
                const hist1_offset = 0x8 + block_id * channel_count * 0x4 + channel_id * 0x4 + 0x0;
                view.setInt16(adpc_offset + hist1_offset, samples[hist1_index]);
                const hist2_index = (block_id * 0x3800 - 2) * channel_count + channel_id;
                const hist2_offset = 0x8 + block_id * channel_count * 0x4 + channel_id * 0x4 + 0x2;
                view.setInt16(adpc_offset + hist2_offset, samples[hist2_index]);
            }
        }

        view.setUint32(data_offset + 0x0, 0x44415441); // "DATA"
        view.setUint32(data_offset + 0x4, data_size);
        view.setUint32(data_offset + 0x8, 0x18);
        for (let block_id = 0, offset = 0x20; block_id < block_count; block_id++) {
            const block_size = block_id == block_count - 1 ? last_block_size : 0x2000;
            for (let channel_id = 0; channel_id < channel_count; channel_id++) {
                const channel_data = this.channel_data(channel_id);
                let channel_offset = block_id * 0x2000;
                for (let i = 0; i < block_size; i++) {
                    view.setUint8(data_offset + offset + i, channel_data.getUint8(channel_offset));
                    channel_offset++;
                }
                offset += align_up(block_size, 0x20);
            }
        }

        return view;
    }
}

class BRWAR {
    constructor(buffer) {
        this.brwar = new DataView(buffer);
        if (this.brwar.getUint32(0x00) !== 0x52574152) {
            throw 'Incorrect magic!';
        }
        const tabl_offset = this.brwar.getUint32(0x10);
        const tabl_size = this.brwar.getUint32(0x14);
        this.tabl = new DataView(buffer, tabl_offset, tabl_size);
        const data_offset = this.brwar.getUint32(0x18);
        const data_size = this.brwar.getUint32(0x1C);
        this.data = new DataView(buffer, data_offset, data_size);
    }

    brwav_count() {
        return this.tabl.getUint32(0x8);
    }

    brwav(brwav_id) {
        const brwav_offset = this.tabl.getUint32(0xC + brwav_id * 0xC + 0x4);
        const brwav_size = this.tabl.getUint32(0xC + brwav_id * 0xC + 0x8);
        return new BRWAV(this.data, brwav_offset, brwav_size);
    }
}

document.getElementById('import').onclick = () => {
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
                const brwar = new BRWAR(e.target.result);

                for (let brwav_id = 0; brwav_id < brwar.brwav_count(); brwav_id++) {
                    const p = document.createElement('p');
                    const brwav = brwar.brwav(brwav_id);
                    p.innerText = brwav_id + ' - ';

                    {
                        const button = document.createElement('button');
                        button.innerText = 'Export as wav';
                        p.appendChild(button);
                        button.onclick = () => {
                            const blob = new Blob([brwav.to_wav()], {type: 'audio/x-wav'});
                            const url = URL.createObjectURL(blob);
                            const a = document.createElement('a');
                            a.href = url;
                            a.download = brwav_id + '.wav';
                            a.click();
                            URL.revokeObjectURL(url);
                        };
                    }

                    p.appendChild(document.createTextNode(' - '));

                    {
                        const button = document.createElement('button');
                        button.innerText = 'Export as brstm';
                        p.appendChild(button);
                        button.onclick = () => {
                            const blob = new Blob([brwav.to_brstm()], {type: 'audio/x-brstm'});
                            const url = URL.createObjectURL(blob);
                            const a = document.createElement('a');
                            a.href = url;
                            a.download = brwav_id + '.brstm';
                            a.click();
                            URL.revokeObjectURL(url);
                        };
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
