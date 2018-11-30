#ifndef SSCE_CMDPARSER_HPP
#define SSCE_CMDPARSER_HPP
#include <Macros.h>
C_DECLS_START
#include <CmdParser.h>
C_DECLS_END
namespace ssce {
void parseCommandLineArgs(const CmdOption* o, int argc, char** argv) {
  ssce_parse_cmdln(o, argc, argv);
}
}  // namespace ssce
#endif /*SSCE_CMDPARSER_HPP*/