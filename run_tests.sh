#!/bin/sh
rm -f ./_vsprintf_bench
cc -Wall -O3 -o _vsprintf_bench -I . vsprintf_bench.c sfprintf.c callback_printf.c
./_vsprintf_bench
exit $?
