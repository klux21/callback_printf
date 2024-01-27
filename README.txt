callback_printf? Why even another sprintf clone?

Because it is one of the most important C functions but the lack of the
portability of the format specifiers especially between Windows and Posix
systems can be be very annoying. All the required workarounds and
adjustments cost the programmers a lot of time.
Who wants to mess around with different prefixes or format strings on every
new platform? OK - there are a plenty of other sprintf implementations already
that you can use as well. But most of those don't really care the performance
and the portability, lack floating point and especially long double support,
don't convert any Unicode strings to UTF-8 and don't care much about the
conformance to the C standard formats and even less about the printf parameter
validation features of the gcc which are a great thing for preventing
unexpected crashes within printf like function.
You don't want any internal locks, unnecessary allocations or strange
implementations which may slow down your code even if every microsecond
counts. callback_printf uses only the stack of the calling thread and neither
locks nor allocations.
It allows you to debug and fix problems very easy if something doesn't work
as expected.
I guess that every programmer who really likes C hates the trouble with printf
like functions and even more the problems and the difficulties if he want to
use the argument format for own output functions or wants to add extensions
to that format.
If it comes to me it was always a wish of me to get rid of all this trouble
and the portability issues that most programmers are struggling with.
And I did want to add some extra length specifiers for using arguments of type
int8_t, int16_t, int32_t and int64_t which can be found since Posix 98 in
inttypes.h and as well in stdint.h since C11 now.
In the C 23 standard there will be be some different length specifiers for
that then the l1, l2, l3 or l8 prefixes that callback_printf uses. But it's
not a big thing to update the code for supporting the new prefixes too once
the new C standard is finally available.
The supported formats specifier are b, B, d, i, o, u, x, X, a, A, e, E, f, F,
g, G, s, c, p, n and % for a percent character and the Microsoft specific
specifiers C and S.
The supported size prefixes for the integer formats b, B, d, i, o, u, x and X
are hh, h, l, ll, t, z and j according to the C standard as well as the
Microsoft specific I, I16, I32 and I64 and additionally l1, l2, l4 and l8
which specify the byte width of the integer arguments.
The supported lenght modifiers for the formats s and c are l for wchar_t
arguments and l1 for 1 byte ISO Latin 8 strings, l2 for 2 byte wide Unicode
characters and l4 for 4 bytes wide unicode characters.
For the floating point formats a, A, e, E, f, F, g and G is the prefix L for
long double arguments is supported. As extension the prefix for specifying
the numeric floating point base can be added. Currently are r0 for base 10,
r1 for base 16 and r2 ... r9 for the bases 2 til 9 supported. Instead of a
digit an asterisk * can be specified. In that case the base needs to be
specified by an additional argument of type int just before the floating
point value. The highest supported numeric base is 36 in that case.
Exponents for bases higher than 14 are prefixed by a tilde (~) istead of
the letter 'e' which is a part of the regular digits of those bases.
An interesting feature is the unintentional and very fast generic mantisse
and exponent calculation for the different numeric base systems that enables
the support of all of those numeric systems.
Where else do you find a genereric solution for printing floating points in
many different base systems?

The little benchmark vsprintf_bench.c is an easy way for checking the
performance. Just execute that file in a shell of a Posix system and have a
look on the outpout.

The callback_printf based wrappers are prefixed with an s for 'speed' and for
'security' because they don't share your string data with compiler libraries
which can be a problem in some use cases.


Why that 'Civil Usage Public License' and not the GPLv3 anymore?
The new license is kind a mix of the conditiond of BSD or Apache license
but in opposite to the former it prohibits any usage for weapons, spyware
and secret monitoring of any people without their knowledge or agreement.
I dislike the idea to find anything of my software in military devices,
weapons or spyware because it's very good tracable in binaries.
That's not a big deal for most people except for the ones who make money out
of wars and things that are usually pretty nasty. I don't expect anything
good in return of supporting those people for free any longer.
Of course the new license shouldn't be a big problem for most people but an
advantage. It is much more permissive if it comes to commercial usage than
the GPL. Despite of that it's for sure a good idea to use the software
legally only by caring the conditions of the license.
I doubt that anybody has a problem with the changed license because there
wasn't any feedback may be right because the license conditions before.
I would be glad if other people adopt that license as well and I hope that
more people have fun with the usage of that software now because of the new
and more permissive license conditions even if it not open source if comes
to the definition of the OSI.


Kind regards,

Klaus Lux
