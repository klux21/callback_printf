#!/bin/sh
rm -f ./_test_callback_printf
cc -Wall -ggdb -o _test_callback_printf -I . test_callback_printf.c callback_printf.c
./_test_callback_printf
exit $?
