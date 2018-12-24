#ifndef OPT_PARSE_H
#define OPT_PARSE_H

#include <array>
#include <iostream>
#include <regex>

/** Simple class for parsing command line options

Example Usage:
  std::string serverAddress = "localhost:10000";
  bool reverseFluxPolarity = false;
  int windowWidth = 1280;
  int windowHeight = 1024;

  Opt::parseCmdLine(argc, argv, {
    Opt{
      std::regex("--server=(.*)"),
      "address of server to connect to",
      [&serverAddress](std::cmatch const& m)
      {
        serverAddress = m[1];
      }},
    Opt{
      std::regex("--reverseFluxPolarity"),
      "operate with flux polarity reversed",
      [&reverseFluxPolarity](std::cmatch const&)
      {
        reverseFluxPolarity = true;
      }},
    Opt{
      std::regex("--screen=([0-9]+)x([0-9]+)"),
      "screen width and height in pixels",
      [&windowWidth, &windowHeight](std::cmatch const& m)
      {
        windowWidth = atoi(m[1].str().c_str());
        windowHeight = atoi(m[2].str().c_str());
      }},
    });
*/
class Opt
{
public:
  typedef std::function<void(std::cmatch const&)> Handler;

  Opt(
    std::string const& pattern,
    std::string const& help,
    Handler handler)
    : r_(pattern),
      help_("  " + pattern + ":\t" + help),
      h_(handler)
  {
  }

  /** static function to parse command line
      @param argc
      @param argv
      @param opts
      @return false if command line invalid or help requested, true otherwise
  */
  static bool parseCmdLine(int argc, char** argv, std::vector<Opt> const& opts)
  {
    bool showUsage = false;

    for( int argi = 1; argi < argc; argi++ )
    {
      if( strcmp(argv[argi], "--help") == 0 )
      {
        showUsage = true;
      }
      else
      {
        bool found = false;
        auto next = opts.begin();
        while( !found && next != opts.end() )
        {
          std::cmatch results;
          if( std::regex_match(argv[argi], results, next->r_) )
          {
            found = true;
            next->h_(results);
          }
          else
          {
            next++;
          }
        }
        if( !found )
        {
          std::cerr << "Unrecognised option: " << argv[argi] << std::endl;
          showUsage = true;
        }
      }
    }

    if( showUsage )
    {
      std::cout << "Usage: " << argv[0] << std::endl;
      for( auto& opt : opts )
      {
        std::cout << opt.help_ << std::endl;
      }
      std::cout << std::endl;
    }

    return !showUsage;
  }

private:
  // regex to recognise the option
  std::regex r_;

  // Help string for this option
  std::string help_;

  // handler function to call if option present
  Handler h_;
};

#endif // OPT_PARSE_H
