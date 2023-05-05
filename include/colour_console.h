//
// Created by Tobias Wallner on 25.09.2021.
//

/*
	Description
	-----------
	
	This is an abstraction of the windows console attributes.
	It allows to set the colour of the text and the background as well as
	underlineing, overlineing and setting bars to the left and right of symbols.
	
	The text format of the console output can be changed directly in the output
	stream useing the stream operator '<<'
	
	For example you can change the text to be 'red' by:

	```C++
	std::cout << "Normal text" << Text::red << "This text is now red" << std::endl;
	```
	
	You can also compose different settings useing the '|' operator. 
	(It is also possible to compose settings useing the '<<' operator but this is not reccomended)
	
	```C++
	std::cout << "Normal text" (Text::blue | Background::white | Bar::bottom) << "Formated text" << std::endl;
	```
	
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
	everything is marked as 'inline' and 'constexpr'
	
	Usage
	-----
	
	Just include this header:
		
		#include "colour_console.hpp"
	
	Compile with C++17
	
	Examples
	--------
	
	See the 'examples.cpp' for examples on how to use this library.
	
*/

#ifndef OMEGA_COLOR_CONSOLE_H
#define OMEGA_COLOR_CONSOLE_H

//windows
#include <windows.h>
#include <WinCon.h>

//std
#include <string_view>
#include <cinttypes>

namespace cc{
	
/**ConsoleTextAttr
	Used to set specific attributes, all other attributes will be reset when printed.
*/
struct ConsoleTextAttr{
	unsigned int value = 0;
};

inline constexpr ConsoleTextAttr operator | (ConsoleTextAttr lhs, ConsoleTextAttr rhs){
	return ConsoleTextAttr{lhs.value | rhs.value};
}

/**
	Apply the given colour and text format settings
*/
inline void set_text_attributes(ConsoleTextAttr attributes){
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), attributes.value);
}

/**
	Get the current console colour and fromat settings
*/
inline ConsoleTextAttr get_text_attributes(){
	CONSOLE_SCREEN_BUFFER_INFO i;
	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &i);
	ConsoleTextAttr attributes;
	attributes.value = i.wAttributes;
	return attributes;
}


/**
	Set the colour and text format within the stream
*/
template<class OStream>
OStream& operator << (OStream& stream, ConsoleTextAttr attr){
	set_text_attributes(attr);
	return stream;
}

/**ConsoleTextAttrChange
	Used to change a specific set of attributes, all other attributes will reman as they were before.
*/
struct ConsoleTextAttrChange{
	unsigned int value = 0;
	unsigned int mask = 0;
};

inline constexpr ConsoleTextAttrChange operator | (ConsoleTextAttrChange lhs, ConsoleTextAttrChange rhs){
	return ConsoleTextAttrChange{lhs.value | rhs.value, lhs.mask | rhs.mask};
}

/**
	change only the settings that are set in the mask and keep everything else
*/
inline void change_text_attributes(ConsoleTextAttrChange attr){
	auto attributes = get_text_attributes();
	attributes.value &= ~attr.mask;
	attributes.value |= attr.value & attr.mask;
	set_text_attributes(attributes);	
}

/**
	Change the colour and text format within the stream
*/
template<class OStream>
OStream& operator << (OStream& stream, ConsoleTextAttrChange attr){
	change_text_attributes(attr);
	return stream;
}


/**ConsoleTextAttrPrint
	Used to print a specific text in a specific style and then restores the previous text attributes.
*/
struct ConsoleTextAttrChangePrint{
	ConsoleTextAttrChange attributes;
	std::string_view string;
};

/**
	prints the string in 'attr' with the applied attributes for the text and then changes the text back to its previous font.
*/
template<class OStream>
OStream& operator << (OStream& stream, ConsoleTextAttrChangePrint attr){
	auto attributes = get_text_attributes();
	stream << attr.attributes << attr.string << attributes;
	return stream;
}

/** TextColour
	This is used to change the foreground colour of the text.
	Just write it in the output stream and the colour of the text will change.
	
	std::cout << TextColour::red << "Everything after this is in red" << std::endl;
*/	

namespace Text{	
	inline constexpr unsigned int mask = FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_GREEN;
	inline constexpr ConsoleTextAttrChange black{0, mask};
	inline constexpr ConsoleTextAttrChange blue{FOREGROUND_BLUE, mask};
	inline constexpr ConsoleTextAttrChange green{FOREGROUND_GREEN, mask};
	inline constexpr ConsoleTextAttrChange aqua{FOREGROUND_BLUE | FOREGROUND_GREEN, mask};
	inline constexpr ConsoleTextAttrChange red{FOREGROUND_RED, mask};
	inline constexpr ConsoleTextAttrChange purple{FOREGROUND_RED | FOREGROUND_BLUE, mask};
	inline constexpr ConsoleTextAttrChange yellow{FOREGROUND_RED | FOREGROUND_GREEN, mask};
	inline constexpr ConsoleTextAttrChange white{FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_GREEN, mask};
	inline constexpr ConsoleTextAttrChange grey{FOREGROUND_INTENSITY, mask};
	inline constexpr ConsoleTextAttrChange light_blue{FOREGROUND_INTENSITY | FOREGROUND_BLUE, mask};
	inline constexpr ConsoleTextAttrChange light_green{FOREGROUND_INTENSITY | FOREGROUND_GREEN, mask};
	inline constexpr ConsoleTextAttrChange light_aqua{FOREGROUND_INTENSITY | FOREGROUND_BLUE | FOREGROUND_GREEN, mask};
	inline constexpr ConsoleTextAttrChange light_red{FOREGROUND_INTENSITY | FOREGROUND_RED, mask};
	inline constexpr ConsoleTextAttrChange light_purple{FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_BLUE, mask};
	inline constexpr ConsoleTextAttrChange light_yellow{FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN, mask};
	inline constexpr ConsoleTextAttrChange bright_white{FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_GREEN, mask};
}

/**
	TextSetColour elements will when printed override all attributes, whereas TextColour elements will change the text colour and leafe all other attributes as is.
	So for example when printed:
	
	std::cout << TextSetColour::red << "Text" << std::endl;
	
	The text will be red, the background will be set to black and all bars will be set to off. 
	However you may compose different 'set' elements
	
	Since the default is black there is also no black text that can be set
*/
namespace TextSet{
	inline constexpr ConsoleTextAttr blue{FOREGROUND_BLUE};
	inline constexpr ConsoleTextAttr green{FOREGROUND_GREEN};
	inline constexpr ConsoleTextAttr aqua{FOREGROUND_BLUE | FOREGROUND_GREEN};
	inline constexpr ConsoleTextAttr red{FOREGROUND_RED};
	inline constexpr ConsoleTextAttr purple{FOREGROUND_RED | FOREGROUND_BLUE};
	inline constexpr ConsoleTextAttr yellow{FOREGROUND_RED | FOREGROUND_GREEN};
	inline constexpr ConsoleTextAttr white{FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_GREEN};
	inline constexpr ConsoleTextAttr grey{FOREGROUND_INTENSITY};
	inline constexpr ConsoleTextAttr light_blue{FOREGROUND_INTENSITY | FOREGROUND_BLUE};
	inline constexpr ConsoleTextAttr light_green{FOREGROUND_INTENSITY | FOREGROUND_GREEN};
	inline constexpr ConsoleTextAttr light_aqua{FOREGROUND_INTENSITY | FOREGROUND_BLUE | FOREGROUND_GREEN};
	inline constexpr ConsoleTextAttr light_red{FOREGROUND_INTENSITY | FOREGROUND_RED};
	inline constexpr ConsoleTextAttr light_purple{FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_BLUE};
	inline constexpr ConsoleTextAttr light_yellow{FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN};
	inline constexpr ConsoleTextAttr bright_white{FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_GREEN};
}

/** BackgroundColour (background colour)
	this enum is used to change the background colour of the output stream. 
	Every symbol that is being printed after a BackgroundColour is being printed in that colour.
*/	

namespace Background{	
	inline constexpr unsigned int mask = BACKGROUND_INTENSITY | BACKGROUND_RED | BACKGROUND_BLUE | BACKGROUND_GREEN;
	inline constexpr ConsoleTextAttrChange black{0, mask};
	inline constexpr ConsoleTextAttrChange blue{BACKGROUND_BLUE, mask};
	inline constexpr ConsoleTextAttrChange green{BACKGROUND_GREEN, mask};
	inline constexpr ConsoleTextAttrChange aqua{BACKGROUND_BLUE | BACKGROUND_GREEN, mask};
	inline constexpr ConsoleTextAttrChange red{BACKGROUND_RED, mask};
	inline constexpr ConsoleTextAttrChange purple{BACKGROUND_RED | BACKGROUND_BLUE, mask};
	inline constexpr ConsoleTextAttrChange yellow{BACKGROUND_RED | BACKGROUND_GREEN, mask};
	inline constexpr ConsoleTextAttrChange white{BACKGROUND_RED | BACKGROUND_BLUE | BACKGROUND_GREEN, mask};
	inline constexpr ConsoleTextAttrChange grey{BACKGROUND_INTENSITY, mask};
	inline constexpr ConsoleTextAttrChange light_blue{BACKGROUND_INTENSITY | BACKGROUND_BLUE, mask};
	inline constexpr ConsoleTextAttrChange light_green{BACKGROUND_INTENSITY | BACKGROUND_GREEN, mask};
	inline constexpr ConsoleTextAttrChange light_aqua{BACKGROUND_INTENSITY | BACKGROUND_BLUE | BACKGROUND_GREEN, mask};
	inline constexpr ConsoleTextAttrChange light_red{BACKGROUND_INTENSITY | BACKGROUND_RED, mask};
	inline constexpr ConsoleTextAttrChange light_purple{BACKGROUND_INTENSITY | BACKGROUND_RED | BACKGROUND_BLUE, mask};
	inline constexpr ConsoleTextAttrChange light_yellow{BACKGROUND_INTENSITY | BACKGROUND_RED | BACKGROUND_GREEN, mask};
	inline constexpr ConsoleTextAttrChange bright_white{BACKGROUND_INTENSITY | BACKGROUND_RED | BACKGROUND_BLUE | BACKGROUND_GREEN, mask};
}

namespace BackgroundSet{	
	inline constexpr unsigned int mask = BACKGROUND_INTENSITY | BACKGROUND_RED | BACKGROUND_BLUE | BACKGROUND_GREEN;
	inline constexpr ConsoleTextAttr blue{BACKGROUND_BLUE};
	inline constexpr ConsoleTextAttr green{BACKGROUND_GREEN};
	inline constexpr ConsoleTextAttr aqua{BACKGROUND_BLUE | BACKGROUND_GREEN};
	inline constexpr ConsoleTextAttr red{BACKGROUND_RED};
	inline constexpr ConsoleTextAttr purple{BACKGROUND_RED | BACKGROUND_BLUE};
	inline constexpr ConsoleTextAttr yellow{BACKGROUND_RED | BACKGROUND_GREEN};
	inline constexpr ConsoleTextAttr white{BACKGROUND_RED | BACKGROUND_BLUE | BACKGROUND_GREEN};
	inline constexpr ConsoleTextAttr grey{BACKGROUND_INTENSITY};
	inline constexpr ConsoleTextAttr light_blue{BACKGROUND_INTENSITY | BACKGROUND_BLUE};
	inline constexpr ConsoleTextAttr light_green{BACKGROUND_INTENSITY | BACKGROUND_GREEN};
	inline constexpr ConsoleTextAttr light_aqua{BACKGROUND_INTENSITY | BACKGROUND_BLUE | BACKGROUND_GREEN};
	inline constexpr ConsoleTextAttr light_red{BACKGROUND_INTENSITY | BACKGROUND_RED};
	inline constexpr ConsoleTextAttr light_purple{BACKGROUND_INTENSITY | BACKGROUND_RED | BACKGROUND_BLUE};
	inline constexpr ConsoleTextAttr light_yellow{BACKGROUND_INTENSITY | BACKGROUND_RED | BACKGROUND_GREEN};
	inline constexpr ConsoleTextAttr bright_white{BACKGROUND_INTENSITY | BACKGROUND_RED | BACKGROUND_BLUE | BACKGROUND_GREEN};
}

/**
	With the Bar one can set lines above the text, underscores, lines left of each symbol and right of each symbol
*/
namespace Bar{
	inline constexpr ConsoleTextAttrChange top{COMMON_LVB_GRID_HORIZONTAL, COMMON_LVB_GRID_HORIZONTAL};
	inline constexpr ConsoleTextAttrChange top_off{0, COMMON_LVB_GRID_HORIZONTAL};
	
	inline constexpr ConsoleTextAttrChange bottom{COMMON_LVB_UNDERSCORE, COMMON_LVB_UNDERSCORE};
	inline constexpr ConsoleTextAttrChange bottom_off{0, COMMON_LVB_UNDERSCORE};
	
	inline constexpr ConsoleTextAttrChange left{COMMON_LVB_GRID_LVERTICAL, COMMON_LVB_GRID_LVERTICAL};
	inline constexpr ConsoleTextAttrChange left_off{0, COMMON_LVB_GRID_LVERTICAL};
	
	inline constexpr ConsoleTextAttrChange right{COMMON_LVB_GRID_RVERTICAL, COMMON_LVB_GRID_RVERTICAL};
	inline constexpr ConsoleTextAttrChange right_off{0, COMMON_LVB_GRID_RVERTICAL};
	
	inline constexpr ConsoleTextAttrChange all{COMMON_LVB_GRID_HORIZONTAL | COMMON_LVB_UNDERSCORE | COMMON_LVB_GRID_LVERTICAL | COMMON_LVB_GRID_RVERTICAL, COMMON_LVB_GRID_HORIZONTAL | COMMON_LVB_UNDERSCORE | COMMON_LVB_GRID_LVERTICAL | COMMON_LVB_GRID_RVERTICAL};
	inline constexpr ConsoleTextAttrChange all_off{0, COMMON_LVB_GRID_HORIZONTAL | COMMON_LVB_UNDERSCORE | COMMON_LVB_GRID_LVERTICAL | COMMON_LVB_GRID_RVERTICAL};
}

namespace BarSet{
	inline constexpr ConsoleTextAttr top{COMMON_LVB_GRID_HORIZONTAL};
	inline constexpr ConsoleTextAttr bottom{COMMON_LVB_UNDERSCORE};
	inline constexpr ConsoleTextAttr left{COMMON_LVB_GRID_LVERTICAL};
	inline constexpr ConsoleTextAttr right{COMMON_LVB_GRID_RVERTICAL};
	inline constexpr ConsoleTextAttr all{COMMON_LVB_GRID_HORIZONTAL | COMMON_LVB_UNDERSCORE | COMMON_LVB_GRID_LVERTICAL | COMMON_LVB_GRID_RVERTICAL};
}

namespace Invert{
	inline constexpr ConsoleTextAttrChange on{COMMON_LVB_REVERSE_VIDEO, COMMON_LVB_REVERSE_VIDEO};
	inline constexpr ConsoleTextAttrChange off{0, COMMON_LVB_REVERSE_VIDEO};
}

namespace Preset{
	inline constexpr ConsoleTextAttr Default = TextSet::white;
	inline constexpr ConsoleTextAttr link = TextSet::blue | BarSet::bottom;
	inline constexpr ConsoleTextAttr active_link = TextSet::purple | BarSet::bottom;
}

namespace Dye{
	inline constexpr ConsoleTextAttrChangePrint black(std::string_view string){return ConsoleTextAttrChangePrint{Text::black, string};}
	inline constexpr ConsoleTextAttrChangePrint blue(std::string_view string){return ConsoleTextAttrChangePrint{Text::blue, string};}
	inline constexpr ConsoleTextAttrChangePrint green(std::string_view string){return ConsoleTextAttrChangePrint{Text::green, string};}
	inline constexpr ConsoleTextAttrChangePrint aqua(std::string_view string){return ConsoleTextAttrChangePrint{Text::aqua, string};}
	inline constexpr ConsoleTextAttrChangePrint red(std::string_view string){return ConsoleTextAttrChangePrint{Text::red, string};}
	inline constexpr ConsoleTextAttrChangePrint purple(std::string_view string){return ConsoleTextAttrChangePrint{Text::purple, string};}
	inline constexpr ConsoleTextAttrChangePrint yellow(std::string_view string){return ConsoleTextAttrChangePrint{Text::yellow, string};}
	inline constexpr ConsoleTextAttrChangePrint white(std::string_view string){return ConsoleTextAttrChangePrint{Text::white, string};}
	inline constexpr ConsoleTextAttrChangePrint grey(std::string_view string){return ConsoleTextAttrChangePrint{Text::grey, string};}
	inline constexpr ConsoleTextAttrChangePrint light_blue(std::string_view string){return ConsoleTextAttrChangePrint{Text::light_blue, string};}
	inline constexpr ConsoleTextAttrChangePrint light_green(std::string_view string){return ConsoleTextAttrChangePrint{Text::light_green, string};}
	inline constexpr ConsoleTextAttrChangePrint light_aqua(std::string_view string){return ConsoleTextAttrChangePrint{Text::light_aqua, string};}
	inline constexpr ConsoleTextAttrChangePrint light_red(std::string_view string){return ConsoleTextAttrChangePrint{Text::light_red, string};}
	inline constexpr ConsoleTextAttrChangePrint light_purple(std::string_view string){return ConsoleTextAttrChangePrint{Text::light_purple, string};}
	inline constexpr ConsoleTextAttrChangePrint light_yellow(std::string_view string){return ConsoleTextAttrChangePrint{Text::light_yellow, string};}
	inline constexpr ConsoleTextAttrChangePrint bright_white(std::string_view string){return ConsoleTextAttrChangePrint{Text::bright_white, string};}
	
	inline constexpr ConsoleTextAttrChangePrint black(ConsoleTextAttrChangePrint formatedString){return ConsoleTextAttrChangePrint{Text::black | formatedString.attributes, formatedString.string};}
	inline constexpr ConsoleTextAttrChangePrint blue(ConsoleTextAttrChangePrint formatedString){return ConsoleTextAttrChangePrint{Text::blue | formatedString.attributes, formatedString.string};}
	inline constexpr ConsoleTextAttrChangePrint green(ConsoleTextAttrChangePrint formatedString){return ConsoleTextAttrChangePrint{Text::green | formatedString.attributes, formatedString.string};}
	inline constexpr ConsoleTextAttrChangePrint aqua(ConsoleTextAttrChangePrint formatedString){return ConsoleTextAttrChangePrint{Text::aqua | formatedString.attributes, formatedString.string};}
	inline constexpr ConsoleTextAttrChangePrint red(ConsoleTextAttrChangePrint formatedString){return ConsoleTextAttrChangePrint{Text::red | formatedString.attributes, formatedString.string};}
	inline constexpr ConsoleTextAttrChangePrint purple(ConsoleTextAttrChangePrint formatedString){return ConsoleTextAttrChangePrint{Text::purple | formatedString.attributes, formatedString.string};}
	inline constexpr ConsoleTextAttrChangePrint yellow(ConsoleTextAttrChangePrint formatedString){return ConsoleTextAttrChangePrint{Text::yellow | formatedString.attributes, formatedString.string};}
	inline constexpr ConsoleTextAttrChangePrint white(ConsoleTextAttrChangePrint formatedString){return ConsoleTextAttrChangePrint{Text::white | formatedString.attributes, formatedString.string};}
	inline constexpr ConsoleTextAttrChangePrint grey(ConsoleTextAttrChangePrint formatedString){return ConsoleTextAttrChangePrint{Text::grey | formatedString.attributes, formatedString.string};}
	inline constexpr ConsoleTextAttrChangePrint light_blue(ConsoleTextAttrChangePrint formatedString){return ConsoleTextAttrChangePrint{Text::light_blue | formatedString.attributes, formatedString.string};}
	inline constexpr ConsoleTextAttrChangePrint light_green(ConsoleTextAttrChangePrint formatedString){return ConsoleTextAttrChangePrint{Text::light_green | formatedString.attributes, formatedString.string};}
	inline constexpr ConsoleTextAttrChangePrint light_aqua(ConsoleTextAttrChangePrint formatedString){return ConsoleTextAttrChangePrint{Text::light_aqua | formatedString.attributes, formatedString.string};}
	inline constexpr ConsoleTextAttrChangePrint light_red(ConsoleTextAttrChangePrint formatedString){return ConsoleTextAttrChangePrint{Text::light_red | formatedString.attributes, formatedString.string};}
	inline constexpr ConsoleTextAttrChangePrint light_purple(ConsoleTextAttrChangePrint formatedString){return ConsoleTextAttrChangePrint{Text::light_purple | formatedString.attributes, formatedString.string};}
	inline constexpr ConsoleTextAttrChangePrint light_yellow(ConsoleTextAttrChangePrint formatedString){return ConsoleTextAttrChangePrint{Text::light_yellow | formatedString.attributes, formatedString.string};}
	inline constexpr ConsoleTextAttrChangePrint bright_white(ConsoleTextAttrChangePrint formatedString){return ConsoleTextAttrChangePrint{Text::bright_white | formatedString.attributes, formatedString.string};}
}

namespace Mark{
	inline constexpr ConsoleTextAttrChangePrint black(std::string_view string){return ConsoleTextAttrChangePrint{Background::black, string};}
	inline constexpr ConsoleTextAttrChangePrint blue(std::string_view string){return ConsoleTextAttrChangePrint{Background::blue, string};}
	inline constexpr ConsoleTextAttrChangePrint green(std::string_view string){return ConsoleTextAttrChangePrint{Background::green, string};}
	inline constexpr ConsoleTextAttrChangePrint aqua(std::string_view string){return ConsoleTextAttrChangePrint{Background::aqua, string};}
	inline constexpr ConsoleTextAttrChangePrint red(std::string_view string){return ConsoleTextAttrChangePrint{Background::red, string};}
	inline constexpr ConsoleTextAttrChangePrint purple(std::string_view string){return ConsoleTextAttrChangePrint{Background::purple, string};}
	inline constexpr ConsoleTextAttrChangePrint yellow(std::string_view string){return ConsoleTextAttrChangePrint{Background::yellow, string};}
	inline constexpr ConsoleTextAttrChangePrint white(std::string_view string){return ConsoleTextAttrChangePrint{Background::white, string};}
	inline constexpr ConsoleTextAttrChangePrint grey(std::string_view string){return ConsoleTextAttrChangePrint{Background::grey, string};}
	inline constexpr ConsoleTextAttrChangePrint light_blue(std::string_view string){return ConsoleTextAttrChangePrint{Background::light_blue, string};}
	inline constexpr ConsoleTextAttrChangePrint light_green(std::string_view string){return ConsoleTextAttrChangePrint{Background::light_green, string};}
	inline constexpr ConsoleTextAttrChangePrint light_aqua(std::string_view string){return ConsoleTextAttrChangePrint{Background::light_aqua, string};}
	inline constexpr ConsoleTextAttrChangePrint light_red(std::string_view string){return ConsoleTextAttrChangePrint{Background::light_red, string};}
	inline constexpr ConsoleTextAttrChangePrint light_purple(std::string_view string){return ConsoleTextAttrChangePrint{Background::light_purple, string};}
	inline constexpr ConsoleTextAttrChangePrint light_yellow(std::string_view string){return ConsoleTextAttrChangePrint{Background::light_yellow, string};}
	inline constexpr ConsoleTextAttrChangePrint bright_white(std::string_view string){return ConsoleTextAttrChangePrint{Background::bright_white, string};}
	
	inline constexpr ConsoleTextAttrChangePrint black(ConsoleTextAttrChangePrint formatedString){return ConsoleTextAttrChangePrint{Background::black | formatedString.attributes, formatedString.string};}
	inline constexpr ConsoleTextAttrChangePrint blue(ConsoleTextAttrChangePrint formatedString){return ConsoleTextAttrChangePrint{Background::blue | formatedString.attributes, formatedString.string};}
	inline constexpr ConsoleTextAttrChangePrint green(ConsoleTextAttrChangePrint formatedString){return ConsoleTextAttrChangePrint{Background::green | formatedString.attributes, formatedString.string};}
	inline constexpr ConsoleTextAttrChangePrint aqua(ConsoleTextAttrChangePrint formatedString){return ConsoleTextAttrChangePrint{Background::aqua | formatedString.attributes, formatedString.string};}
	inline constexpr ConsoleTextAttrChangePrint red(ConsoleTextAttrChangePrint formatedString){return ConsoleTextAttrChangePrint{Background::red | formatedString.attributes, formatedString.string};}
	inline constexpr ConsoleTextAttrChangePrint purple(ConsoleTextAttrChangePrint formatedString){return ConsoleTextAttrChangePrint{Background::purple | formatedString.attributes, formatedString.string};}
	inline constexpr ConsoleTextAttrChangePrint yellow(ConsoleTextAttrChangePrint formatedString){return ConsoleTextAttrChangePrint{Background::yellow | formatedString.attributes, formatedString.string};}
	inline constexpr ConsoleTextAttrChangePrint white(ConsoleTextAttrChangePrint formatedString){return ConsoleTextAttrChangePrint{Background::white | formatedString.attributes, formatedString.string};}
	inline constexpr ConsoleTextAttrChangePrint grey(ConsoleTextAttrChangePrint formatedString){return ConsoleTextAttrChangePrint{Background::grey | formatedString.attributes, formatedString.string};}
	inline constexpr ConsoleTextAttrChangePrint light_blue(ConsoleTextAttrChangePrint formatedString){return ConsoleTextAttrChangePrint{Background::light_blue | formatedString.attributes, formatedString.string};}
	inline constexpr ConsoleTextAttrChangePrint light_green(ConsoleTextAttrChangePrint formatedString){return ConsoleTextAttrChangePrint{Background::light_green | formatedString.attributes, formatedString.string};}
	inline constexpr ConsoleTextAttrChangePrint light_aqua(ConsoleTextAttrChangePrint formatedString){return ConsoleTextAttrChangePrint{Background::light_aqua | formatedString.attributes, formatedString.string};}
	inline constexpr ConsoleTextAttrChangePrint light_red(ConsoleTextAttrChangePrint formatedString){return ConsoleTextAttrChangePrint{Background::light_red | formatedString.attributes, formatedString.string};}
	inline constexpr ConsoleTextAttrChangePrint light_purple(ConsoleTextAttrChangePrint formatedString){return ConsoleTextAttrChangePrint{Background::light_purple | formatedString.attributes, formatedString.string};}
	inline constexpr ConsoleTextAttrChangePrint light_yellow(ConsoleTextAttrChangePrint formatedString){return ConsoleTextAttrChangePrint{Background::light_yellow | formatedString.attributes, formatedString.string};}
	inline constexpr ConsoleTextAttrChangePrint bright_white(ConsoleTextAttrChangePrint formatedString){return ConsoleTextAttrChangePrint{Background::bright_white | formatedString.attributes, formatedString.string};}
}

inline constexpr ConsoleTextAttrChangePrint Underline(std::string_view string){return ConsoleTextAttrChangePrint{Bar::bottom, string};}

inline constexpr ConsoleTextAttrChangePrint Underline(ConsoleTextAttrChangePrint formatedString){return ConsoleTextAttrChangePrint{Bar::bottom | formatedString.attributes, formatedString.string};}

}//namespace cc

#endif //OMEGA_COLOR_CONSOLE_H
