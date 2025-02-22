#include <retesteth/EthChecks.h>
#include <retesteth/ExitHandler.h>
#include <retesteth/Options.h>
#include <retesteth/TestOutputHelper.h>
#include <csignal>
#include <iostream>
#include <thread>


namespace test {
std::string const cBYellowBlack = "\x1b[43m\x1b[30m";
std::string const cYellow = "\x1b[33m";
std::string const cLime = "\x1b[32m";
std::string const cRed = "\x1b[0;31m";
std::string const cDefault = "\x1b[0m";

namespace logmessage
{
void eth_warning_message(std::string const& _message, unsigned _verbosity)
{
    if (Options::get().logVerbosity >= _verbosity)
        std::cout << cYellow << "WARNING: " << _message << "\x1b[0m" << std::endl;
}

void eth_stdout_message(std::string const& _message, std::string const& _color)
{
    if (_color.empty())
        std::cout <<  _message << std::endl;
    else
        std::cout << _color << _message << "\x1b[0m" << std::endl;
}

void eth_stderror_message(std::string const& _message)
{
    std::cerr << cRed << _message << "\x1b[0m" << std::endl;
}

void eth_log_message(std::string const& _message, LogColor _color)
{
    string s_pre;
    switch (_color)
    {
    case LogColor::YELLOW:
        s_pre = cYellow;
        break;
    case LogColor::LIME:
        s_pre = "\x1b[32m";
        break;
    case LogColor::DEFAULT:
        break;
    default:
        break;
    }
    std::cout << s_pre << _message << "\x1b[0m" << std::endl;
}

void eth_error(std::string const& _message)
{
    // Do not mark errors if exiting the program by emergency
    if (ExitHandler::receivedExitSignal())
        throw EthError() << _message;
    else
    {
        // if the exception is not allowed, then throw an exception
        if (TestOutputHelper::get().markError(_message))
            throw EthError() << _message;
    }
}

void eth_mark_error(std::string const& _message)
{
    TestOutputHelper::get().markError(_message);
}

void eth_check_message(std::string const& _message)
{
    eth_error(_message);
}

void eth_require_message(std::string const& _message)
{
    eth_fail(_message);
}

void eth_require(bool _flag)
{
    if (!_flag)
        eth_fail("Flag error");
}

void eth_fail(std::string const& _message)
{
    // thread that failing with this function might be being joined in a loop
    TestOutputHelper::get().markError(_message);
    if (!ExitHandler::receivedExitSignal())
        std::raise(SIGABRT);
    throw std::runtime_error(_message);
}

int eth_getVerbosity()
{
    return Options::get().logVerbosity;
}

}  // namespace logmessage
}  // namespace test
