callback_printf? But why even another sprintf clone? 
Because it is one of the most important C functions but the lack
of the portability of the format specifiers especially between
Windows and Posix conform systems can be a pain in the ass. 
Of course there are a lot of other implementations already that
you can use as well. But most don't really care the performance,
the portability, lack full floating point and long double support,
don't convert of Unicode strings to UTF-8 and don't care much
about the conformance to the C standard and even less about the
great printf parameter validation features of the gcc which are a
great help to prevent unexpected crashes. 
You don't want any internal locks, unnecessary allocations or
strange implementation slow down your output if every microsecond
count. callback_printf uses only the stack of the thread which
which calls it. And you want to be able to debug your code and
to find the reason in case of errors.
I guess that every programmer who really likes C hates the
trouble with printf and even more the problems and the difficulties
to use the argument format for his very own functions. 
And if it comes to me it was allways a wish of me to finally get
rid of all of the problems and portability issues with it that
most programmers are struggling with. And I did want some options
for using arguments of type int8_t, int16_t,
int32_t and int64_t that are available in inttypes.h and the
newer stdint.h since a pretty long time.
In the C 23 standard there will be be some different length
specifiers for that then l1, l2, l3 or l8. It's not a big thing
to update the code to support those additionally.

But why GPLv3? Well I'm unemployed since a short time and could
spent some time on that special thing what most companies wouldn't
like to pay you for.
It took a lot of time to implement it but of course I have to pay
my rent as well. Feel free to ask For a close source license for
you private projects your company.

I think 100,- Euros for companies who have up to 10 people,
500,- Euros for companies who has up to 1000 people and 2000,-
for all the really big once are a really fair price for
an unlimited usage in all of their closed sourced source
projects and for getting finally rid of the problems.
And right it's open source you are free to check whether the
code sweets your programs requirements.

Kind regards,
Klaus Lux
