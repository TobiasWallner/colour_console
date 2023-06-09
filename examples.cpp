#include "include/colour_console.h"
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
	
	std::cout << Bar::top << "This text is overscored" << Bar::top_off << std::endl << std::endl;
	
	std::cout << Bar::left << "This text has left bars" << Bar::left_off << std::endl << std::endl;
	
	std::cout << Bar::right << "This text has right bars" << Bar::right_off << std::endl << std::endl;
	
	std::cout << Bar::all << "This text has bars all over"  << Bar::all_off << std::endl;
			
	const auto my_preset = Text::blue | Background::white | Bar::bottom;
	std::cout << my_preset << "This is a text with composed format" << Preset::Default << std::endl;
	
	std::cout << Preset::link << "This could be a link to a website" << Preset::Default << std::endl;
	std::cout << Preset::active_link << "This could be an activated link to a website" << Preset::Default << std::endl;
	
	
	std::cout << "In this text we " << Dye::yellow("dyed") << " this text piece" << std::endl;
	
	std::cout << "In this text we " << Mark::green("marked") << " this text piece" << std::endl;
	
	std::cout << "In this text we " << Underline("underlined") << " this text piece" << std::endl;
	
	std::cout << "In this text we composed " << Dye::green(Mark::red("a dye and a mark")) << " of this text piece" << std::endl;
	
	std::cout << std::endl;
	std::cout << std::endl;
	std::cout << std::endl;
	std::cout << "    Normal text. " << Text::red << "This text" << " and everything that follows is now red" << Preset::Default << std::endl;
	std::cout << std::endl;
	std::cout << std::endl;
	std::cout << std::endl;
	std::cout << "    Normal text. " << (Text::blue | Background::white | Bar::bottom) << "This text" << " and everything that follows is now formated" << Preset::Default << std::endl;
	std::cout << std::endl;
	std::cout << std::endl;
	std::cout << std::endl;
	std::cout << "    In this text we " << Dye::yellow("dyed") << " this text piece" << std::endl;
	std::cout << "    In this text we " << Mark::green("marked") << " this text piece" << std::endl;
	std::cout << "    In this text we " << Underline("underlined") << " this text piece" << std::endl;
	std::cout << std::endl;
	std::cout << std::endl;
	std::cout << std::endl;
	std::cout << "    In this text we composed " << Dye::black(Mark::yellow("a dye and a mark")) << " of this text piece" << std::endl;
	std::cout << std::endl;
	std::cout << std::endl;
	std::cout << std::endl;
	
	return 0;
}