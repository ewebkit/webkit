import LowLevelBinary, * as LLB from '../LowLevelBinary.js';
import * as assert from '../assert.js';

let values = [];
for (let i = LLB.varuintMin; i !== LLB.varuintMin + 1024; ++i) values.push(i);
for (let i = LLB.varuintMax; i !== LLB.varuintMax - 1024; --i) values.push(i);

for (const i of values) {
    let b = new LowLevelBinary();
    b.varuint(i);
    const v = b.getVaruint(0);
    if (v.value !== i)
        throw new Error(`Wrote "${i}" and read back "${v}"`);
    if (v.next !== b.getSize())
        throw new Error(`Size ${v.next}, expected ${b.getSize()}`);
}

for (let i = 0; i < LLB.varBitsMax + 1; ++i) {
    let b = new LowLevelBinary();
    for (let j = 0; j < i; ++j)
        b.uint8(0x80);
    assert.throws(() => b.getVarint(0), RangeError, `[${i}, ${i+1}) is out of buffer range [0, ${i})`);
}

let b = new LowLevelBinary();
for (let i = 0; i < LLB.varBitsMax; ++i)
    b.uint8(0x80);
b.uint8(0x00);
assert.throws(() => b.getVarint(0), RangeError, `Shifting too much at 6`);
