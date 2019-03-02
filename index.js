const native = require('./build/Release/native.node')

const consts = {
  HKCR: 0x80000000,
  HKCU: 0x80000001,
  HKLM: 0x80000002,
  HKU: 0x80000003,
  HKPD: 0x80000004,
  HKCC: 0x80000005,
  HKDD: 0X80000006
  REG_SZ: 1,
  REG_EXPAND_SZ: 2,
  REG_BINARY: 3,
  REG_DWORD: 4,
  REG_DWORD_BIG_ENDIAN: 5,
  REG_DWORD_LITTLE_ENDIAN: 4,
  REG_LINK: 6,
  REG_MULTI_SZ: 7,
  REG_RESOURCE_LIST: 8
}

export function getRegistryKey (root, path) {
  return native.getKey(consts[root], path)
}

export function listRegistrySubkeys (root, path) {
  return native.listSubkeys(consts[root], path)
}

console.log(getRegistryKey('HKLM', 'SOFTWARE\\Microsoft\\DirectMusic'))

console.log(listRegistrySubkeys('HKLM', 'SOFTWARE\\Microsoft'))
