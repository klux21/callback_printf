callback_printf? Why even another sprintf clone?

Because it is one of the most important C functions but the lack of the
portability of the format specifiers especially between Windows and Posix
systems can be be very annoying. All the required workarounds and adjustments
cost the programmers a lot of time.
Who wants to mess around with different prefixes or format strings on every
new platform? OK - there are a plenty of other sprintf implementations but
many of them don't really care the performance and the portability or lack
floating point and especially long double support and they don't convert
Unicode strings to UTF-8 if dealing with wide characters and don't care much
about the conformance to the C standard formats.
This implementation also ignores the locale settings to prevent any kind of
random output variations that depends on the environment settings.
You also don't want any internal locks, unnecessary allocations or strange
implementations which may slow down your code where every microsecond counts.
callback_printf uses only the stack of the calling thread and neither locks nor
allocations. It allows you to debug and fix problems very easily if something
doesn't work as expected.
I guess that every programmer who really likes C hates the trouble with printf
like functions and even more the problems and the difficulties if he want to
use the argument format for own output functions or wants own extensions of
that format.
If it comes to me it was always a wish of me to get rid of all this trouble
and the portability issues that most programmers are struggling with.
And I did want to add some extra length specifiers for using arguments of type
int8_t, int16_t, int32_t and int64_t which can be found since Posix 98 in
inttypes.h and as well in stdint.h since the C 11 standard.
In the C 23 standard there will be be some different length specifiers for
that then the l1, l2, l4 or l8 prefixes that callback_printf uses. It's not a
big thing to update the code for supporting the new prefixes too of course.
The supported format specifiers are b, B, d, i, o, u, x, X, a, A, e, E, f, F,
g, G, s, c, p, n and % for a percent character and the Microsoft specific
specifiers C and S.
The supported size prefixes for the integer formats b, B, d, i, o, u, x and X
are currently hh, h, l, ll, t, z and j according to the C standard as well as
the Microsoft specific I, I16, I32 and I64. Additionally some own prefixes
l1, l2, l4 and l8 are supported which specify the byte width of the provided
integer arguments.
The supported lenght modifiers for the formats s and c are l for wchar_t
arguments and l1 for 1 byte ISO Latin 8 strings, l2 for 2 byte wide Unicode
characters and l4 for 4 bytes wide unicode characters.
For the floating point formats a, A, e, E, f, F, g and G the prefix L for
long double arguments is supported.
Additionally a special prefix for specifying a specific numeric base of integer
or floating point numbers is supported. It's r0 for base 10, r1 for base 16 and
r2 ... r9 for the bases 2 til 9. For variable bases r* can be used. In that
case the base needs to be specified by an additional argument of type int just
before the integer or floating specifier or it's optional length specifier. The
highest supported numeric base is 36.
Floating point exponents for bases higher than 14 are prefixed by a tilde '~'
istead of the letter 'e' which becomes a member of the regular digits of those
bases.

Sometimes you need to print the same complex data types quite often e.g. IPv6
addresses or times. The common way in C would be to write a function that
writes them to a character buffer and returns a pointer to that buffer but you
need an variable for a buffer wherever you need to print them. For things like
that callback_printf supports the options %v and %V for variable argument types
now. %v expects a function pointer and a data pointer as related arguments
where the function generates the output for the random data pointer and calls
the write callback. %V expects a single pointer to a struct that contains the
mentioned function and a pointer pointer as argument and may be a member of
other structs that need to be written in a special format. That way it's
possible to print an unlimited number of types and data in special formats.
However that's all still an early state an the function prototypes may still
change a bit in future versions. Feel free to use the discussion site to share
your oppinion and ideas regarding that.

The implementation also uses the great printf parameter validation features of
the gcc which are a great thing for preventing program crashes within printf
like functions. However that may trigger those warnings in case of the using
any format enhancements and you may need to silence those warnings by using the
unchecked function versions which names are prefixed by an underscore '_'.

An interesting internal feature is the fast generic mantisse and exponent
calculation for the several numeric bases that enables the generic support of
different numeric bases for the floating point output.

The little benchmark vsprintf_bench.c is an easy way for checking the
performance. Just execute that file in a shell of a Posix system and have a
look on the outpout.

All callback_printf based wrappers are prefixed with an s for 'speed' and for
'security' because you won't share your string data with compiler libraries
which can be a security issue in some special use cases where you need to
prevent sniffing.

The license is kind of a mix of BSD and Apache conditions but in opposite to
those it prohibits a usage for weapons and spyware and a secret monitoring of
other people without their agreement or their health or life being endangered.
That's fine for most software but usually not for military devices, weapons or
spyware. It would be great if more projects would adapt this license as well.


Kind regards,

Klaus Lux
