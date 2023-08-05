callback_printf? But why even another sprintf clone? 
Because it is one of the most important C functions but the
lack of the portability of the format specifiers especially
between Windows and Posix systems can be can be very annoying.
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
callback_printf uses only the stackn of the calling thread
only and neither locks nor allocations.
It allows you to debug and fix problems very easy if something
function doesn't really work as expected.
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

Why GPLv3? Well it took a lot of time to implement that 
but of course I have to pay my rent as well.
Feel free to ask for a close source license for your
private projects or your company. Please be aware that
callback_printf is easily to track down in binaries.
It's really very simple and better to use it legally only.

I think 100,- Euros for companies who have up to 10 people,
500,- Euros for companies who has up to 1000 people and 2000,-
for all the really big once are a really fair price for
an unlimited usage of this in all of their closed sourced
source projects and for getting rid of the common problems
with sprintf. It's open source - feel free to check whether
you like the implementation and whether the bunch of features
is matching your requirements.


Kind regards,

Klaus Lux
