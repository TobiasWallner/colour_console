Description
-----------

This is an abstraction of the windows console attributes.
It allows to set the colour of the text and the background as well as
underlineing, overlineing and setting bars to the left and right of symbols.

The text format of the console output can be changed directly in the output
stream useing the stream operator '<<'

For example you can change the text to be 'red' by:

```C++
std::cout << "    Normal text. " << Text::red << "This text" << " and everything that follows is now red" << Preset::Default << std::endl;
```
![001](https://user-images.githubusercontent.com/70602844/236539384-8e2c0624-f39a-4137-aefd-28e146592a26.JPG)

You can also compose different settings useing the '|' operator. 
(It is also possible to compose settings useing the '<<' operator but this is not reccomended)

```C++
std::cout << "    Normal text. " << (Text::blue | Background::white | Bar::bottom) << "This text" << " and everything that follows is now formated" << Preset::Default << std::endl;
```
![002](https://user-images.githubusercontent.com/70602844/236539407-25bbf7e8-1370-4eae-abd5-29b0444fdd43.JPG)

If you do not want that all the text and everything that follows changes colour you can use the 'Dye()', 'Mark()' and 'Underline()' functions
to only apply a colour format to a specific text piece.

```C++
std::cout << "    In this text we " << Dye::yellow("dyed") << " this text piece" << std::endl;
std::cout << "    In this text we " << Mark::green("marked") << " this text piece" << std::endl;
std::cout << "    In this text we " << Underline("underlined") << " this text piece" << std::endl;
```
![003](https://user-images.githubusercontent.com/70602844/236539427-c3451423-eb99-407b-87aa-0a8c9ab4ca06.JPG)

You may also compose the different formats as you like.

```C++
std::cout << "    In this text we composed " << Dye::black(Mark::yellow("a dye and a mark")) << " of this text piece" << std::endl;
```
![004](https://user-images.githubusercontent.com/70602844/236539451-da175fbb-fc68-483c-b40b-5fde494329a6.JPG)

When you use the normal namespaces (Text, Background, Bar, Invert) the text will only apply the
change you have set but keep all the settings that were previously active.
For example if you only use 'Background::white' then only the background colour will change
and the text colour will remain as it was before.

However there is also the set-idom using the namespace (TextSet, BackgroundSet, BarSet).
Those can only be composed useing the '|' operator. 
When useing the set-idiom all fields that have not been explicitly set will be set to zero
or its default value.
The default for the Text and Background is black.
The default for the Bar is that all bars are deactivated.

This is a very light weight abstraction that aims to not have any overhead over
calling the windows system calls directly. Thus this is a header only library and 
everything is marked as 'inline' and 'constexpr'.

There are also other great libraries that make it easy to change the console output format.
However, some of those use dynamic memory and complex data structures like std::string or std::map.
This library aims to not use any dynamic memory and after the simplest optimisations in this library
all additional variables on the stack should also fall away. 
The resulting assembly should then boil down directly to the necessary Windos System Calls
without any overhead. 

Usage
-----

Just include this header:

```C++
#include "colour_console.hpp"
```

Compile with C++17

Examples
--------

See the 'examples.cpp' for examples on how to use this library.

```C++
#include "colour_console.h"
#include <iostream>

int main(){
	
	using namespace cc;
	std::cout << Text::red << "this text is red" << std::endl;
	std::cout << "This text is still red. " << Text::blue << "this text is blue" << std::endl;
	std::cout << Text::yellow << "This text is yellow" << std::endl;
	
	std::cout << Background::light_blue << "This background is light blue" << std::endl;
	std::cout << Background::grey << "This background is grey" << std::endl;
	
	std::cout << (Text::red | Text::blue) << "This text has a composed colour" << std::endl;
	
	std::cout << Background::black << "This text has a black background again" << std::endl;
	
	std::cout << Invert::on << "This text has foreground and background inverted" << Invert::off << std::endl;
	
	std::cout << Text::white << "This text is white again" << std::endl;
	
	std::cout << Bar::bottom << "This text is underscored" << Bar::bottom_off << std::endl << std::endl;
	
	std::cout << Underline << "There is also an Underline token that does the same" << Underline_off << std::endl << std::endl;
	
	std::cout << Bar::top << "This text is overscored" << Bar::top_off << std::endl << std::endl;
	
	std::cout << Bar::left << "This text has left bars" << Bar::left_off << std::endl << std::endl;
	
	std::cout << Bar::right << "This text has right bars" << Bar::right_off << std::endl << std::endl;
	
	std::cout << Bar::all << "This text has bars all over"  << Bar::all_off << std::endl;
			
	const auto my_preset = Text::blue | Background::white | Underline;
	std::cout << my_preset << "This is a text with composed format" << Preset::Default << std::endl;
	
	std::cout << Preset::link << "This could be a link to a website" << Preset::Default << std::endl;
	std::cout << Preset::active_link << "This could be an activated link to a website" << Preset::Default << std::endl;
	
	return 0;
}
```

![example_output_image](https://user-images.githubusercontent.com/70602844/236524670-ec0bb1dc-a996-4b9b-8401-9ff228002ad0.png)
