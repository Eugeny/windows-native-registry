# windows-native-registry
[![Build status](https://ci.appveyor.com/api/projects/status/6c27yfv8vkav1lw0?svg=true)](https://ci.appveyor.com/project/Eugeny/windows-native-registry)

```ts
export interface RegistryValue {
    name: string
    type: REG
    value: any
}

export enum HK {
    CR = 0x80000000,
    CU = 0x80000001,
    LM = 0x80000002,
    U = 0x80000003,
}

export enum REG {
    SZ = 1,
    EXPAND_SZ = 2,
    BINARY = 3,
    DWORD = 4,
}

export function getRegistryKey (root: HK, path: string): {[name: string]: RegistryValue}

export function getRegistryValue (root: HK, path: string, name: string): any

export function setRegistryValue (root: HK, path: string, name: string, type: REG, value: string): any

export function listRegistrySubkeys (root: HK, path: string): string[]

export function createRegistryKey (root: HK, path: string)

export function deleteRegistryKey (root: HK, path: string)
```

## Contributors ✨

Thanks goes to these wonderful people ([emoji key](https://allcontributors.org/docs/en/emoji-key)):

<!-- ALL-CONTRIBUTORS-LIST:START - Do not remove or modify this section -->
<!-- prettier-ignore-start -->
<!-- markdownlint-disable -->
<table>
  <tr>
    <td align="center"><a href="https://github.com/cwalther"><img src="https://avatars1.githubusercontent.com/u/234094?v=4?s=100" width="100px;" alt=""/><br /><sub><b>Christian Walther</b></sub></a><br /><a href="https://github.com/Eugeny/windows-native-registry/commits?author=cwalther" title="Code">💻</a></td>
    <td align="center"><a href="https://github.com/Sikkesoft"><img src="https://avatars.githubusercontent.com/u/11422479?v=4?s=100" width="100px;" alt=""/><br /><sub><b>André Sikma</b></sub></a><br /><a href="https://github.com/Eugeny/windows-native-registry/commits?author=Sikkesoft" title="Code">💻</a></td>
  </tr>
</table>

<!-- markdownlint-restore -->
<!-- prettier-ignore-end -->

<!-- ALL-CONTRIBUTORS-LIST:END -->

This project follows the [all-contributors](https://github.com/all-contributors/all-contributors) specification. Contributions of any kind welcome!
