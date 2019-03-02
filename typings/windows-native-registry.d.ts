declare type RegistryRoot = 'HKCR' | 'HKCU' | 'HKLM' | 'HKU' | 'HKPD' | 'HKCC' | 'HKDD'

declare module 'windows-native-registry' {
  export interface RegistryValue {
    name: string
    type: string
    value: number|string|number[]
  }

  export function getRegistryKey (root: RegistryRoot, path: string): RegistryValue[]
  export function listRegistrySubkeys (root: RegistryRoot, path: string): string[]
}
