#include <iostream>
#include <functional>
#include <string>
#include <vector>

class GenericFizzBuzz {
    private:
    bool workingMutex;
    unsigned int len;
    std::string result;
    std::function<std::string(std::string)> fizz, buzz;
    std::function<std::string(std::string, std::string)> join;

    public:
    GenericFizzBuzz(unsigned int steps,
		    std::function<std::string(std::string)> fizzFn,
		    std::function<std::string(std::string)> buzzFn,
		    std::function<std::string(std::string, std::string)> joinFn) {
        len = steps;
        fizz = fizzFn;
	buzz = buzzFn;
	join = joinFn;
	workingMutex = false;
        // For basic sanity checking, if len is 1 or 2, don't bother with get()
	switch (len) {
		case 1: result = "1"; break;
		case 2: result = join("1","2"); break;
		default: result = ""; break;
	}
    }

    std::string get() {
        if (len == 0 || workingMutex) return "";
	else if (result != "") return result;
        // Enter Critical Section
	workingMutex = true;
        std::vector<std::string> tmp(len);
	/* For every element, provide the string equivelant of its index plus 1.
	   For every third or fifth element, apply fizz and/or buzz.
	   Must apply all previous operations before continuing, this
	   allows for workload subdivision with threads in the future */
	unsigned int i = 0;
	for ( ; i < len; i++) { tmp[i] = std::to_string(i + 1); }
	for (i = 2; i < len; i += 3) { tmp[i] = fizz(tmp[i]); }
	for (i = 4; i < len; i += 5) { tmp[i] = buzz(tmp[i]); }
	// Finally fold vector with join() into result
        result = tmp[0];
	for (i = 1; i < len; i++) { result = join(result, tmp[i]); }
	// Exit Critical Section
	workingMutex = false;
	return result;
    }
};

int main () {
    long in = -1;
    std::function<std::string(std::string)> fizz = [] (std::string s) { return "Fizz"; };
    std::function<std::string(std::string)> buzz = [] (std::string s) { return s == "Fizz" ? "FizzBuzz" : "Buzz"; };
    std::function<std::string(std::string, std::string)> join = [] (std::string str, std::string newWord) { return str + " " + newWord; };

    std::cout << "Welcome to the generic FizzBuzz program, please enter a number between 0 and 65'535! ";
    while (in < 0 || in > 65535) {
        std::cin >> in;
	if (in < 0) std::cout << "Sorry, we don't do negative numbers here, please enter a positive number less than 65'535: ";
	else if (in > 65535) std::cout << "Sorry, that number is too big, please enter a smaller number: ";
    }
    unsigned int safeInput = in;
    std::cout << "Fizz Buzz " << in << " =" << std::endl;
    GenericFizzBuzz fb(in, fizz, buzz, join);
    std::cout << fb.get() << std::endl;
    return 0;
}
