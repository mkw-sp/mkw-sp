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
        let sample_count = this.nibble_count();
        if (this.encoding() == 2) {
            sample_count = Math.floor(sample_count / 16) * 14 + (sample_count % 16 - 2);
        }
        return sample_count;
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

    to_wav() {
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
                    const sample_id = Math.floor(nibble_id / 16) * 14 + (nibble_id % 16 - 2);
                    const sample_offset = (sample_id * this.channel_count() + channel_id) * 2;
                    view.setUint16(0x2C + sample_offset, clamped_sample, true);
                }
            }
        } else {
            throw 'Unsupported encoding ' + encoding;
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
                        button.innerText = 'Export';
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
