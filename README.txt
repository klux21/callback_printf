callback_printf? Why even another sprintf clone? 
Because it is one of the most important C functions but the
lack of the portability of the format specifiers especially
between Windows and Posix systems can be be very annoying.
The required workarounds and adjustments cost a lot of time.
Who wants to mess around with different prefixes or format
strings on every new platform?
OK - there are a plenty of other implementations already that
you can use as well. But most of those don't really care the
performance and the portability, lack floating point and
especially long double support, don't convert Unicode
strings to UTF-8 and don't care much about the conformance
to the C standard and even less about the printf parameter
validation features of the gcc which are a great thing for
preventing unexpected crashes within printf like function.
You don't want any internal locks, unnecessary allocations or
strange implementations which may slow down your code even if
every microsecond counts.
callback_printf uses only the stack of the calling thread and
neither locks nor allocations.
It allows you to debug and fix problems very easy if something
doesn't work as expected.
I guess that every programmer who really likes C hates the
trouble with printf like functions and even more the problems
and the difficulties if he want to use the argument format
for own output functions or wants to add  extensions to that
format. 
If it comes to me it was always a wish of me to get rid of
all this trouble and the portability issues that most
programmers are struggling with. And I did want to add some
extra length specifiers for using arguments of type int8_t,
int16_t, int32_t and int64_t which can be found since
Posix 98 in inttypes.h and as well in stdint.h since C11 now.
In the C 23 standard there will be be some different length
specifiers for that then the l1, l2, l3 or l8 prefixes that
callback_printf uses. But it's not a big thing to update the
code for supporting the new prefixes once the new C standard
is available.
The supported formats specifier are b,B,d,i,o,u,x,X,
a,A,e,E,f,F,g,G,s,S,c,C,p,n and %% for a percent character.
The supported size prefixes for the integer formats
b, B, d, i, o, u, x and X are hh, h, l, ll, t, z and j
according to the C standard, the Microsoft specific I16,
I32 and I64 and additionally l1, l2, l4 and l8 which specify
the byte width of the integer arguments.
The supported lenght modifiers for the formats s and c are
l for wchar_t arguments, l1 for 1 byte ISO Latin 8 strings,
l2 for 2 byte wide Unicode characters and l4 for 4 bytes
wide unicode characters.
The floating point formats a, A, e, E, f, F, g and G is the
prefix L for long double arguments is supported.
Additional an prefix for the numeric floating point base
can be added. Currently are r0 for base 10, r1 for base 16
and r2 ... r9 for the bases 2 til 9 supported. Instead of a
digit an asterisk * can be specified. In that case the base
needs to be specified by an additional argument of type int
just before the floating point value. The highest supported
numeric base specified by this int is 36. Exponents for
bases higher than 14 are prefixed by a tilde (~) istead of
the letter 'e' which is used as a regular digit then.
An interesting feature is the unintentional but very fast
generic mantisse and exponent calculation for the different
numeric base systems that ables the support of that many
numeric bases.

Why GPLv3? Well it took a lot of time to implement that but
of course I have to pay my rent as well.
Feel free to ask for a close source license for your private
usage or your company. You should be aware that
callback_printf is easily to track down in binaries.
It's much better and a lot nicer to use it legally only.
But it's open source so feel free to check whether you like
the implementation and whether the bunch of features is
matching your requirements.


Kind regards,

Klaus Lux
