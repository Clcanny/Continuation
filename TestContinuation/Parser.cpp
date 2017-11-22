#include "Header.h"
#include <iostream>
#include <sstream>

class Parser
{
    private:
	char next;
	std::istream& is;
	std::function<void(char)> cb;

    private:
	char pull()
	{
	    return std::char_traits<char>::to_char_type(is.get());
	}
	
	void scan()
	{
	    do
	    {
		next=pull();
	    }
	    while(isspace(next));
	}

    public:
	Parser(std::istream& is_,std::function<void(char)> cb_) :
	    next(), is(is_), cb(cb_)
	{}
	
	void run()
	{
	    scan();
	    E();
	}

    private:
	void E()
	{
	    T();
	    while (next=='+'||next=='-')
	    {
		cb(next);
		scan();
		T();
	    }
	}
	
	void T()
	{
	    S();
	    while (next=='*'||next=='/')
	    {
		cb(next);
		scan();
		S();
	    }
	}

	void S()
	{
	    if (isdigit(next))
	    {
		cb(next);
		scan();
	    }
	    else if(next=='(')
	    {
		cb(next);
		scan();
		E();
		if (next==')')
		{
		    cb(next);
		    scan();
		}
		else
		{
		    throw std::runtime_error("parsing failed");
		}
	    }
	    else
	    {
		throw std::runtime_error("parsing failed");
	    }
	}
};

int main()
{
    std::istringstream is("1+1");
    // execute parser in new continuation
    Continuation source;
    // user-code pulls parsed data from parser
    // invert control flow
    char c;
    bool done = false;
    source = callcc(
	    [&is, &c, &done](Continuation &&sink) {
		// create parser with callback function
		Parser p(is, [&sink, &c](char c_) {
		    // resume main continuation
		    c = c_;
		    sink=sink.resume();
		});
		// start recursive parsing
		p.run();
		// signal termination
		done = true;
		// resume main continuation
		return std::move(sink);
            }
	);

    while (!done)
    {
        printf("Parsed: %c\n", c);
        source = source.resume();
    }
}
