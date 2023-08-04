callback_printf? But why even another sprintf clone? 
Because it is one of the most important C functions but the lack
of the portability of the format specifiers especially between
Windows and Posix systems can be can be very annoying and the
workaround and required adjustments may cost a lot of time. 
Right there are a plenty of other implementations already that
you can use as well. But most don't really care the performance,
the portability, lack floating point and especially long double
support, don't convert of Unicode strings to UTF-8 and don't care
much about the conformance to the C standard and even less about
the great printf parameter validation features of the gcc which
is a great help for the prevention unexpected crashes within
a printf like function. 
You don't want any internal locks, unnecessary allocations or
strange implementations which may slow down your even if every
microsecond counts. callback_printf uses only the stack of the
thread  which calls it and uses neither locks nor allocations.
And you are able to debug it like any other part of your own
code if it doesn't work as expected.
I guess that every programmer who really likes C hates the
trouble with printf like functions and even more the problems
and the difficulties to use their powerful argument format for
very own output generation. 
If it comes to me it was allways a wish of me to finally get
rid of all those problems and the bunch of portability issues
that most programmers are struggling with. And I did want some
extra length specifiers for using arguments of type int8_t,
int16_t, int32_t and int64_t which  are available in the 
inttypes.h and the since C11 in stdint.h now.
In the C 23 standard there will be be some different length
specifiers instead of the l1, l2, l3 or l8 prefixes that
callback_printf uses. But it's not a big thing to update the
code for supporting those additionally once the new C standard
is available.

Why GPLv3? Well it took a lot of time to implement that 
but of course I have to pay my rent as well. 
Feel free to ask for a close source license for you very 
private projects your company.

I think 100,- Euros for companies who have up to 10 people,
500,- Euros for companies who has up to 1000 people and 2000,-
for all the really big once are a really fair price for
an unlimited usage of this in all of their closed sourced
source projects and for getting rid of the common problems with
printf. And it's open source you are able to check whether the
code and whether it sweets your requirements.

Kind regards,

Klaus Lux
