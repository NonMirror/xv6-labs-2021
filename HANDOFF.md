# HANDOFF

## 构建环境:两套 RISC-V 工具链,必须只用一套(2026-11 记录)

本机同时装了两套 `riscv64-unknown-elf-*` 工具链:

| 位置 | 版本 | 特点 |
| --- | --- | --- |
| `~/Tools/xpack/riscv64-unknown-elf-toolchain-10.2.0-2020.12.8-x86_64-apple-darwin/bin` | GCC 10.2.0 / binutils 2.35 (SiFive, Rosetta) | 与 2021 版课程匹配,`make qemu` 全绿 |
| `/opt/homebrew/bin` | GCC 15.1.0 / binutils 2.45 (arm64 原生) | 能编译,但见下方两个坑 |

**症状**:`make qemu` 在链接 `user/_sh` 时失败
`riscv64-unknown-elf-ld: -march=rv64i2p1_m2p0_..._zmmul1p0_...: unknown z ISA extension 'zmmul'`

**根因**:工具链混用。工程里已有的 `*.o` 是 Homebrew GCC 15 编的(`.riscv.attributes` 带上
`zmmul/zaamo/zalrsc/zca/zcd` 这些新扩展名),而当前 PATH 里排在前面的 `ld` 是 binutils 2.35,
不认识这些新扩展名。xv6 的 Makefile 用 `-MD` 生成 `.d` 但**不 include** 它们,所以换编译器不会触发
重编译,只会重新链接 —— 于是编译一路通过、最后链接炸掉。

**处理**:

```sh
make clean && make qemu      # 保证所有 .o 与当前 PATH 里的 gcc/ld 一致
```

**若改用 Homebrew GCC 15**,额外还需要绕过 `-Werror`:
它把 `-Winfinite-recursion` 纳入 `-Wall`,`user/sh.c` 的 `runcmd` 自递归会变成 error。
需要在 `CFLAGS` 里加 `-Wno-infinite-recursion`(或在 `user/sh.c` 局部关闭该警告)。

**结论**:默认走 xpack 10.2.0,不要在同一个 build tree 里换工具链;换之前先 `make clean`。

## 其他

- `make grade` 依赖 `pexpect`,当前 `python3`(mise 3.11)、`/opt/homebrew/bin/python3`、
  `/usr/bin/python3` 都没有装,跑 grader 前需 `pip install pexpect`。
- `make` 的默认目标是 `kernel/kernel`;`fs.img` 和 user 程序由 `make qemu` / `make fs.img` 触发。
- 本地工作区已有未提交改动:`notxv6/ph.c`、`user/uthread.c`、`user/uthread_switch.S`(thread lab),勿覆盖。
