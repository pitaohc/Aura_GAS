# Aura（UE 5.1 → 5.3）升级与 MSVC 工具链问题记录

> 本文记录 2026-10-07 的升级过程、踩到的坑与解法。
> **凡未实际执行过验证的结论，均已标注为"推断"或"待验证"**，请勿当成既成事实使用。
>
> - 文档中的"实测"= 本机读取引擎源码/配置文件、查询 vswhere、运行 UBT 所得
> - "报错日志"= 用户提供的构建输出
> - "推断"= 根据 VS/UE 发布时间与官方版本矩阵推测，本机无法证实

---

## 一、升级范围与时间线

- 2026-10-07 19:41：`EngineAssociation` 由 `5.1` 改为 `5.3`，首次切换引擎版本，**失败**（MSVC 报错）。
- 2026-10-07 19:44：UBT 查询模式确认 Windows SDK 检测正常。
- 2026-10-07 20:00：重建 RiderLink 宿主工程，**失败**（同一头文件报错）。
- 2026-10-07 20:27～20:28：第二次 RiderLink 构建（临时目录 `Xisyrab`），**失败**。
- 2026-10-07 20:28:37：修改引擎头文件 `ConcurrentLinearAllocator.h`（补丁已落盘）。
- **此后尚未有成功构建记录 —— 剩余验证项见第六节。**

---

## 二、环境事实（实测）

### 2.1 引擎与工程

| 项目 | 值 |
| --- | --- |
| 工程路径 | `D:\Fast Data\project\UE\Aura_GAS` |
| 目标引擎 | `D:\APP\Unreal Engine\UE_5.3`，版本 **5.3.2**（Changelist 29314046，Branch `++UE5+Release-5.3`），Launcher 安装版 |
| 本机其它引擎 | UE_4.27、UE_5.1、UE_5.3、UE_5.4、UE_5.8（5.1 / 5.3 / 5.8 随附源码；4.27 / 5.4 为纯安装版） |
| 工程模块 | 单模块 `Aura`，58 个源文件，无自定义 `Plugins` 目录 |
| UBT 使用的 DotNet SDK | 6.0.302（引擎自带） |

### 2.2 Visual Studio

| 项目 | 值 |
| --- | --- |
| 唯一 VS 实例 | `C:\Program Files\Microsoft Visual Studio\2022\Community` |
| 版本 / 通道 | **17.14.36401.2**，`VisualStudio.17.Release`，`isComplete: true` |
| 已安装 Windows SDK | 10.0.19041.0 / 10.0.22621.0 / **10.0.26100.0（当前选用）** |

### 2.3 MSVC 工具集（`VC\Tools\MSVC`）

| 目录（家族） | cl.exe 实际版本 | x64 | arm64 | 状态 |
| --- | --- | --- | --- | --- |
| `14.29.30133` | —（无 cl.exe） | ❌ | — | 卸载 v142 后残留的**空目录**，无害 |
| `14.33.31629` | 14.33.31631 | ✅ | — | 被引擎 **ban**（5.3 的 `BannedVisualCppVersions` 覆盖 14.30–14.33） |
| `14.38.33130` | 14.38.33145 | ✅ | — | ✅ 可用，与引擎预编译库对应的档位 |
| `14.44.35207` | 14.44.35214 | ✅ | ✅ | ✅ 可用，但**对 UE 5.3 过新**，即本次问题的根源 |

VS 的默认工具集指向文件：

```
VC\Auxiliary\Build\Microsoft.VCToolsVersion.default.txt      → 14.44.35207
VC\Auxiliary\Build\Microsoft.VCToolsVersion.V143.default.txt → 14.38.33130
```

### 2.4 引擎的编译器策略（源码实测位置）

| 内容 | 位置 |
| --- | --- |
| "安装版引擎必须 ≥ MSVC 14.34" 检查 | `Engine\Source\Programs\UnrealBuildTool\Platform\Windows\UEBuildWindows.cs:1045` |
| 偏好 / 禁用 / 最低 MSVC 版本表 | `...\Platform\Windows\MicrosoftPlatformSDK.Versions.cs:48-79` |
| C4668/C4067 被提升为 error | `Engine\Source\Runtime\Core\Public\Windows\WindowsPlatformCompilerSetup.h:28` |
| "编译器比 VS2022 新" 的 `#pragma message` | 同上，第 14-16 行（条件 `_MSC_VER > 1939`） |
| C++ 标准随 `DefaultBuildSettings` 变化 | `...\Configuration\TargetRules.cs:2138` |

---

## 三、问题一：MSVC 工具链被选成 14.29，升级被拒（已解决）

### 现象

```
Microsoft platform targets must be compiled with Visual Studio 2022 17.4 (MSVC 14.34.x) or later
for the installed engine. ... The current compiler version was detected as: 14.29.30159
```

### 根因

`C:\...\VS2022\Community\VC\Tools\MSVC\14.29.30133\bin\Hostx64\x64\cl.exe` 的 `ProductVersion` 实测为
**14.29.30159.0**，与报错数字完全一致 —— 即 VS2019 时代的 v142 工具链被安装在 VS2022 目录树内，
UBT 扫描时把它当作候选并选中。报错信息里"ensure no configuration is forcing
WindowsTargetRules.Compiler to VisualStudio2019"是误导：所有 `BuildConfiguration.xml` 中均无此配置。

### 处理

1. 用 VS Installer **卸载 MSVC v142（14.29）** —— 已执行，成功（留下空目录）。
2. 附带效果：UBT 转而选中 **14.44.35214**，从而暴露出下一个问题。

---

## 四、问题二：`ConcurrentLinearAllocator.h` 与 MSVC 14.44 不兼容（补丁已落盘，待验证）

### 现象

`Aura` 工程与 Rider 的 UnrealLink 宿主工程报**完全相同**的错误：

```
Detected compiler newer than Visual Studio 2022, please update min version checking in WindowsPlatformCompilerSetup.h
...\Core\Public\Experimental\ConcurrentLinearAllocator.h(31): error C4668: 没有将"__has_feature"定义为预处理器宏
...\ConcurrentLinearAllocator.h(31): error C4067: 预处理器指令后有意外标记 - 应输入换行符
CompilationResultException: Error: OtherCompilationError
```

### 根因

第 31 行原为：

```cpp
#elif __has_feature(address_sanitizer)      // __has_feature 是 Clang 专用宏
```

UE 5.3 假定 MSVC 下它只是未定义标识符；MSVC 14.44 下该假定不再成立，`#elif` 后出现多余记号 →
先 C4668、再 C4067，而 `WindowsPlatformCompilerSetup.h:28` 把 **4067 提升为 error**。

为何只有部分模块失败：纯 C++ 模块（RD / RiderDebuggerSupport / RiderLogging）通过；
包含 Core SharedPCH 的模块（`RiderLink` / `RiderLC` / `RiderBlueprint` / `RiderGameControl` / `RiderShaderInfo`，以及
`SharedPCH.UnrealEd.Cpp17`）失败。

### 已落盘的补丁

`D:\APP\Unreal Engine\UE_5.3\Engine\Source\Runtime\Core\Public\Experimental\ConcurrentLinearAllocator.h:31`

```cpp
// 修改前
#elif __has_feature(address_sanitizer)
// 修改后
#elif defined(__has_feature) && __has_feature(address_sanitizer)
```

对 Clang 行为不变。**注意**：文档编写时该补丁尚未经过一次成功的编译验证（见第六节）。
若重新编译仍报同样错误，说明 `defined(__has_feature)` 未按预期短路，改用**完全不出现该记号**的写法：

```cpp
#if PLATFORM_HAS_ASAN_INCLUDE && !defined(_MSC_VER)
```

### 时间戳证据（说明为何"改了还报错"）

| 项 | 值 |
| --- | --- |
| 报错日志写入时间 | 2026/10/7 **20:28:18** |
| 头文件修改时间 | 2026/10/7 **20:28:37** |

日志早于修改 19 秒，故那次报错不可能包含补丁。且全引擎仅此一份
`ConcurrentLinearAllocator.h`（已搜索确认，无第二份副本，`Intermediate\...\Inc` 为实体目录而非目录联接）。

---

## 五、问题三：RiderLink 的特殊性

Rider 的 UnrealLink 编译的不是本工程，而是**临时宿主工程**（日志第 24 行实测）：

```
UnrealEditor Win64 Development
  -Project=%LOCALAPPDATA%\Temp\UnrealLink\<随机>\HostProject\HostProject.uproject
  -plugin=...\HostProject\Plugins\RiderLink\RiderLink.uplugin
  -noubtmakefiles -nohotreload -installed
```

关键结论：

1. 该临时目录**每次构建重建并清理**（实测：`Xisyebyk`、`Xisyrab` 均已不存在）；
2. 它有自己的 `Config`，**不读本工程的 `Config/DefaultEngine.ini`**；
3. 因此"给 RiderLink 单独配 `CompilerVersion`"或"改它的配置"**不可行** —— 改完即被覆盖。

### 可行的三种修法

| 方案 | 作用范围 | 代价 |
| --- | --- | --- |
| ① 打引擎头文件补丁（第四节） | Aura + RiderLink + 以后所有工程 | 改引擎安装文件，引擎校验/升级会覆盖 |
| ② 卸载 MSVC 14.44，仅留 14.38 | 全机一致、零配置 | 需 VS Installer；本机仅 14.44 带 arm64 工具链 |
| ③ 改 VS 默认工具集指向文件 | 全机 VS 构建 | 污染全局环境，**不推荐** |

**建议优先 ②**作为收口：卸载后候选表内 ≥14.34 的只剩 14.38，UBT 零配置也只会选中它，
且与引擎预编译库（对应 14.38 档）工具链一致。

---

## 六、配置尝试与结果（重要教训）

### 6.1 `CompilerVersion` / `ToolchainVersion` 的三种来源

`UEBuildWindows.cs:270-283` 实测：

```csharp
[ConfigFile(ConfigHierarchyType.Engine, "/Script/WindowsTargetPlatform.WindowsTargetSettings", "CompilerVersion")]
[XmlConfigFile(Category = "WindowsPlatform")]
[CommandLine("-CompilerVersion")]
public string? CompilerVersion = null;
```

- `ToolchainVersion` 注释明确写着"**if the compiler is not msvc**"，
  且 `VCEnvironment.cs:477-482` 中非 Clang 分支会 `ToolChain = Compiler;`
  —— 用 MSVC 时它**被忽略**，不必设置。

### 6.2 一次失败的尝试（已记录，避免重犯）

曾在工程级 `Saved\UnrealBuildTool\BuildConfiguration.xml` 写入：

```xml
<WindowsPlatform>
  <Compiler>VisualStudio2022</Compiler>
  <CompilerVersion>14.38.33130</CompilerVersion>
  <ToolchainVersion>14.38.33130</ToolchainVersion>
</WindowsPlatform>
```

**结果：无效**。后续构建日志仍为：

```
Using Visual Studio 2022 14.44.35214 toolchain (C:\...\VC\Tools\MSVC\14.44.35207)
```

原因：UBT 的 XML 配置只在**生成工程文件 / 查询**等模式下读取
（`XmlConfig.ReadConfigFiles` 由这些模式调用，且项目级那份要求传入 `ProjectRootDirectory`）；
而编译走的是 Build 模式（`AuraEditor Win64 Development -Project=... -FromMSBuild`），**不读 XML**。

### 6.3 正确的位置

要固定编译器版本应写进工程 ini 的**已存在节**：

```ini
; Config/DefaultEngine.ini
[/Script/WindowsTargetPlatform.WindowsTargetSettings]
CompilerVersion=14.38.33130
```

- 值写**目录名**：UBT 按"目录版本或同族版本"匹配（`MicrosoftPlatformSDK.cs:551`），
  写 cl.exe 真实版本 `14.38.33145` 可能不匹配；
- `ToolchainVersion` 无需写（见 6.1）；
- **未验证**：本节内容尚未在本机实测生效（截至文档编写时未再构建）。

### 6.4 配置层次（实测 `XmlConfig.cs:113-331`，后者覆盖前者）

| 优先级 | 位置 | 说明 |
| --- | --- | --- |
| 1 | `<工程>\Saved\UnrealBuildTool\BuildConfiguration.xml` | 仅本工程 |
| 2 | `%APPDATA%\Unreal Engine\UnrealBuildTool\BuildConfiguration.xml` | 全局 |
| 3 | `%USERPROFILE%\Documents\Unreal Engine\UnrealBuildTool\BuildConfiguration.xml` | 全局 |
| 4 | `<引擎>\Saved\UnrealBuildTool\BuildConfiguration.xml` | Launcher 安装版**跳过**此层 |

---

## 七、UE / MSVC / C++ 对照表

### 7.1 MSVC 与 Visual Studio 版本映射（UE 5.8 配置文件注释实测）

```
14.50 = VS2026 18.0
14.44 = VS2022 17.14.x   → 要求 clang 19
14.42 = VS2022 17.12.x   → 要求 clang 18
14.40 = VS2022 17.10.x   → 要求 clang 17
14.37 = VS2022 17.7-17.9 → 要求 clang 16
```

### 7.2 各 UE 版本对 MSVC 的要求

| UE 版本 | 偏好（推荐）MSVC | 最低 | 最低 VS2022 | 14.44 | 来源 |
| --- | --- | --- | --- | --- | --- |
| 5.1 | 14.29、14.33 | 14.29 | 17.3 / VS2019 | ❌ | 本机源码实测 |
| **5.3** | 14.36、14.35、14.34、14.29 | 14.29（安装版另强制 ≥14.34） | 17.4 | ⚠️ 版本检查可过，头文件不兼容 | 本机源码实测 |
| 5.4 | 14.38 档（VS 17.8/17.9） | 14.38 档 | 17.4+ | ✅ 实践可用 | ⚠️ **推断**（本机为纯安装版，无源码可查） |
| 5.5 | 同上，新增 14.40/14.42 | 14.38 档 | 17.8 档 | ✅ 实践可用 | ⚠️ **推断** |
| 5.6 | 14.42 档 | 14.38 档 | 17.8 档 | ✅ | ⚠️ **推断**（与 VS 17.14 同期发布） |
| 5.7 | 14.44 档 | 14.38 | 17.8 | ✅ | ⚠️ **推断** |
| **5.8** | **14.50（VS2026 18.0）、14.44（VS2022 17.14）** | **14.38.33130** | **17.8** | ✅ | ✅ 本机 `Windows_SDK.json` 实测 |

> 说明：5.4–5.7 四档在官方文档站无法抓取（本环境解析不到公网），且本机对应引擎均无源码，
> 因此只能按 VS 发布时间与版本矩阵推测。**能确证的只有 5.8 明确把 14.44 列为偏好版本。**

### 7.3 5.8 中"被禁用"的 MSVC（实测，说明引擎验证之细）

```
14.39.x             内部编译器错误 / AVX 代码生成导致运行时崩溃
14.40.0-14.43.99999 同样禁用
14.44.0-14.44.35210 模板编译错误（14.44.35211 起修复）
14.50.0-14.50.35722 内部编译器错误
```

即引擎会拒绝"编得过但生成错误代码"的工具集 —— 比编译报错更危险的一类。

### 7.4 C++ 标准

标准并非手动指定，而是跟随 `DefaultBuildSettings`（`TargetRules.cs:2138` 实测）：

| BuildSettingsVersion | C++ 标准 | 同时生效的默认值 |
| --- | --- | --- |
| V2（本工程当前） | C++17 | `bLegacyParentIncludePaths = true` |
| V3 | C++17 | 去掉 legacy 父目录 include 路径 |
| **V4** | **C++20** | `bStrictConformanceMode = true`（MSVC 严格一致性模式） |
| V5（5.3 的 Latest） | C++20 | 同上 |

- **UE 5.3** 处于过渡期：新建工程生成 V3（`NativeProjects.cs:180`），升级提示催升 V4；
- **UE 5.4 及以后**（推断）：Latest 为 V5，默认 **C++20 + `/permissive-`**。
  跨版本升级时，"C++20 + 严格一致性"往往比工具链版本造成更多编译错误。

---

## 八、待办与验证清单

### 8.1 已完成的修改

| # | 内容 | 状态 |
| --- | --- | --- |
| 1 | `Aura.uproject` → `"EngineAssociation": "5.3"` | ✅ 已改 |
| 2 | 卸载 MSVC v142（14.29） | ✅ 已执行 |
| 3 | 引擎 `ConcurrentLinearAllocator.h:31` 加 `defined(__has_feature)` 守卫 | ✅ 已落盘 |
| 4 | 工程级 `BuildConfiguration.xml` 写入 `CompilerVersion` | ⚠️ 已写但**证明无效**（见 6.2），建议清空 |

### 8.2 尚未完成 / 待验证

- [ ] **重新编译 Aura 工程**，确认 `ConcurrentLinearAllocator.h(31)` 报错消失；
- [ ] **重启 Rider**，确认 UnrealLink 宿主工程构建通过；
- [ ] 任选其一收口工具链：① 保留补丁用 14.44；② 卸载 14.44 只用 14.38；
- [ ] 目标文件 `AuraEditor.Target.cs` / `Aura.Target.cs` 仍为 `BuildSettingsVersion.V2`，
      升级稳定后再决定是否升 V4（会引入 C++20 + 严格模式，需改代码）；
- [ ] 以下 3 处工程代码问题（5.3 起的废弃 API 与逻辑缺陷）**尚未修改**：
  1. `Source/Aura/Private/AbilitySystem/AuraAbilitySystemComponent.cpp:19-21`
     —— `FGameplayAbilitySpec::DynamicAbilityTags` 已废弃；且该处 `Cast<UAuraGameplayAbility>(AbilitySpec.Ability)`
     在 `GiveAbility` 之前执行，`Ability` 为 nullptr，**导致 StartupAbilities 根本不会被赋予**（逻辑 bug，非仅警告）；
  2. 同文件 `34-36`、`59-61` 行的 `DynamicAbilityTags.HasTagExact(...)`（改用 `GetDynamicSpecSourceTags()`）；
  3. `Source/Aura/Private/AbilitySystem/CustomCalculation/MMC_AttributeLevelScaled.cpp:12`
     —— `PostEditChangeProperty` 重写需用 `#if WITH_EDITOR` 包裹。

### 8.3 其它注意事项

- **工具链一致性**：引擎预编译库对应 14.38 档；若自己的模块用 14.44、链接引擎的 14.38 产物，
  属混用工具链，通常能连过但存在潜在风险。追求一致就用 14.38。
- **磁盘上有一份完整副本** `D:\Fast Data\project\UE\Aura_GAS - 副本`（"副本"取自中文系统，
  终端可能显示为乱码）。其 `Aura.uproject` 仍为 `EngineAssociation: "5.1"`，**未被修改**。
  构建时请确认操作的是哪一份，避免改错目录。
- **系统时间异常**：构建日志时间戳为 2026 年。时间偏差会影响 UBT 增量构建与 DDC / shader 缓存判断，建议校正。
- `Config/DefaultEngine.ini` 的碰撞配置存在成对的 `-` / `+` 重复项（原工程遗留），
  且该文件会被新引擎自动改写，升级后建议 `git diff` 审阅。

---

## 九、回退方式

| 改动 | 回退方法 |
| --- | --- |
| `Aura.uproject` 版本号 | 改回 `"5.1"` |
| 引擎头文件补丁 | 将 `#elif defined(__has_feature) && __has_feature(address_sanitizer)` 改回 `#elif __has_feature(address_sanitizer)` |
| 卸载的 MSVC 14.29 | 用 VS Installer 重新勾选 MSVC v142 |
| 工程 `BuildConfiguration.xml` | 清空为 `<Configuration xmlns="https://www.unrealengine.com/BuildConfiguration"></Configuration>` |
| 中间产物 | 删除 `Binaries`、`Intermediate`、`DerivedDataCache`、`.vs`、`Aura.sln` 后重新生成工程文件 |

> 注意：后续几次构建后，`Binaries` / `Intermediate` / `Content` 可能已被 5.3 改写，
> **回退前请先用 git 确认改动范围，不要凭本表盲目操作。**

---

## 十、附：本次用到的关键结论速查

1. 编译"选错工具链"时，**不要**先怀疑 `BuildConfiguration.xml` —— 编译模式不读它，
   要查 `Config/DefaultEngine.ini` 的 `[/Script/WindowsTargetPlatform.WindowsTargetSettings]`。
2. 生成工程文件的命令在 5.3 里是 `Build.bat -projectfiles`，**没有** `GenerateProjectFiles.bat`
   （该文件在 5.3 已不存在）。PowerShell 里调用带引号的路径必须加 `&`。
3. RiderLink 走临时宿主工程，工程级配置对它无效，只能从引擎或 VS 安装层面解决。
4. 判断"改动是否生效"时，**先比对文件修改时间与日志写入时间** —— 本次就靠这一点避免了一次误判。
