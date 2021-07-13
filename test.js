var assert = require('assert');
var mod = require('.');

describe('#windows-native-registry()', () => {
  const K = "Software\\xx-test";
  beforeEach(() => {
    mod.deleteRegistryKey(mod.HK.CU, K);
    mod.createRegistryKey(mod.HK.CU, K);
  });

  afterEach(() => {
    mod.deleteRegistryKey(mod.HK.CU, K);
  });

  it('should create keys', () => {
    mod.createRegistryKey(mod.HK.CU, K + '\\test');
    assert.notEqual(mod.getRegistryKey(mod.HK.CU, K + '\\test'), null);
  });

  it('should list keys', () => {
    mod.createRegistryKey(mod.HK.CU, K + '\\test');
    assert.equal(mod.listRegistrySubkeys(mod.HK.CU, K).length, 1);
    assert.equal(mod.listRegistrySubkeys(mod.HK.CU, K)[0], 'test');
  });

  it('should delete keys', () => {
    mod.deleteRegistryKey(mod.HK.CU, K);
    assert.equal(mod.getRegistryKey(mod.HK.CU, K), null);
  });

  it('should write & read DWORD values', () => {
    mod.setRegistryValue(mod.HK.CU, K, 'value', mod.REG.DWORD, 123);
    assert.equal(mod.getRegistryValue(mod.HK.CU, K, "value"), 123);
  });

  it('should write & read SZ values', () => {
    mod.setRegistryValue(mod.HK.CU, K, 'value', mod.REG.SZ, "123");
    assert.equal(mod.getRegistryValue(mod.HK.CU, K, "value"), "123");
  });

  it('should read complete keys', () => {
    mod.setRegistryValue(mod.HK.CU, K, 'a', mod.REG.SZ, "1");
    mod.setRegistryValue(mod.HK.CU, K, 'b', mod.REG.SZ, "2");
    const ret = mod.getRegistryKey(mod.HK.CU, K)
    assert.deepEqual(ret.a, { name: 'a', value: '1', type: mod.REG.SZ });
    assert.deepEqual(ret.b, { name: 'b', value: '2', type: mod.REG.SZ });
  });
});
