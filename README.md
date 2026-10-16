# FVPLoader 


A simple GUI Loader which allow you to run FAVORITE's game under none-Japanese OS  

## How to use
1. Copy both `FVPKernel.dll` and `FVPLoaderGui.exe` into the game directory.
2. Double click `FVPLoaderGui.exe` and drag game executable file onto the window or just drag game executable file onto `FVPLoaderGui.exe`.
3. `FVPLoaderGui.exe` will create a configuration file automatically. Next time you play game, just double click `FVPLoaderGui.exe`.
4. Enjoy your game.


## Language / code page selection

FVPKernel used to hardcode code page 932 (Shift-JIS), so the loader only worked
for Japanese builds. It now reads its settings from `FVPLoader.ini`, next to
`FVPKernel.dll`:

```ini
[Encoding]
CodePage=936
```

| CodePage | Language |
| -------- | -------- |
| 932  | Japanese (Shift-JIS) - default |
| 936  | Simplified Chinese (GBK) |
| 950  | Traditional Chinese (Big5) |
| 949  | Korean (UHC) |
| 1252 | Western European (Latin-1) |
| 65001 | Unicode (UTF-8) |

`Charset`, `LocaleID` and `ScriptName` may be overridden as well; they default to
the values belonging to `CodePage`. An unsupported `CodePage` is passed to the
Win32 conversion APIs unchanged.

`FVPLoaderGui.exe` writes the file for you - pick the game's encoding from the
**Encoding** list in the loader window. Because the setting is a plain text file,
a translation group can ship one file per game with their patch instead of asking
players to run the game under Locale Emulator.


## Build
- Microsoft Visual Studio 2017
- Win10 SDK(19H2)


## 中文使用者可以直接看这一篇
http://bgm.tv/blog/53474 
>虽然这篇blog已经很早之前写的了。

## OpenSource License  

![Files](https://www.gnu.org/graphics/gplv3-127x51.png)

All source code files in `source` folder are licensed under [GNU General Public License v3 (GPL v3)](https://www.gnu.org/licenses/gpl-3.0.en.html).

