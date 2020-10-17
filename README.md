# Hex-Rays MicroCode Explorer
Hex-Rays microcode API plugin for exploring microcode

Ported from [https://github.com/RolfRolles/HexRaysDeob](https://github.com/RolfRolles/HexRaysDeob)

Implements a menu item on pseudo code view

It generates microcode for selection and dumps it to the output window.

Reference: [https://www.hex-rays.com/blog/hex-rays-microcode-api-vs-obfuscating-compiler/](https://www.hex-rays.com/blog/hex-rays-microcode-api-vs-obfuscating-compiler/)

Alternative microcode explorer with different features is Lucid

* [https://github.com/gaasedelen/lucid](https://github.com/gaasedelen/lucid)

* [https://blog.ret2.io/2020/09/11/lucid-hexrays-microcode-explorer/](https://blog.ret2.io/2020/09/11/lucid-hexrays-microcode-explorer/)

This uses the new C++ plugin API so it's only >= IDA 7.5 compatible

Based on IDA SDK ht_view sample plugin

