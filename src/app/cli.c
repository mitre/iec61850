#include "iec61850_actions/app.h"

#ifdef _WIN32
#define PROGNAME "iec61850_actions.exe"
#elif __APPLE__
#define PROGNAME "iec61850_actions_darwin"
#else
#define PROGNAME "iec61850_actions"
#endif

#define DEFAULT_PORT 102
#define REG_EXTENDED 1
#define REG_ICASE (REG_EXTENDED << 1)
/* simple ip regex, looks for 4 groups of 1-3 ints or 'localhost' */
#define IP_REX "\\b\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}|localhost\\b"

void display_help(void *argtable[], const char *description, const char *example)
{
  printf("%s\n\n", description);
  printf("Usage: %s", PROGNAME);
  arg_print_syntax(stdout, argtable, "\n");
  printf("\nThis command permits the following arguments:\n");
  arg_print_glossary(stdout, argtable, "  %-25s %s\n");
  printf("\nExample: %s\n", example);
}

int parse_get_subcommand(int argc,
                         char *argv[],
                         void *argtable[],
                         int argtable_len,
                         const char *description,
                         const char *example)
{
  int exitcode = 0;
  int nerrors;

  if (arg_nullcheck(argtable) == 0) {
    struct arg_int *port = argtable[argtable_len - 3];
    struct arg_lit *help = argtable[argtable_len - 2];
    struct arg_end *end = argtable[argtable_len - 1];
    assert(strcmp(help->hdr.longopts, "help") == 0);
    assert(strcmp(port->hdr.longopts, "port") == 0);
    assert(end->hdr.flag == ARG_TERMINATOR);

    port->ival[0] = DEFAULT_PORT;
    nerrors = arg_parse(argc, argv, argtable);

    if ((help->count > 0) || (nerrors > 0)) {
      exitcode = 1;
    }

    if (help->count > 0) {
      display_help(argtable, description, example);
    } else if (nerrors > 0) {
      exitcode = 1;
      display_help(argtable, description, example);
      printf("\nError parsing command line:\n");
      arg_print_errors(stdout, end, PROGNAME);
    }
  }

  return exitcode;
}

int get_logical_devices_subparser(int argc, char *argv[])
{
  const char *description =
    "IEC 61850 Service: GetServerDirectory\n\n"
    "This command is used to read the list of logical devices from a server.\n"
    "Maps to MMS function GetNameList.";
  const char *example = "./iec61850_actions get logical_devices 192.23.103.247";

  struct arg_lit *help = arg_lit0(NULL, "help", NULL);
  struct arg_int *port =
    arg_int0("p", "port", "<int>", "port number of the server (optional, default: 102)");
  struct arg_rex *cmd = arg_rex1(NULL, NULL, "get", NULL, REG_EXTENDED, NULL);
  struct arg_rex *subcmd =
    arg_rex1(NULL, NULL, "(logical_)?devices?", "logical_devices", REG_EXTENDED, NULL);
  struct arg_rex *ip = arg_rex1(
    NULL, NULL, IP_REX, "<ip address>", REG_EXTENDED, "ip address of the server (required)");
  struct arg_end *end = arg_end(20);
  void *argtable[] = { cmd, subcmd, ip, port, help, end };
  int argtable_len = sizeof(argtable) / sizeof(argtable[0]);

  int exitcode = 0;
  exitcode = parse_get_subcommand(argc, argv, argtable, argtable_len, description, example);
  if (exitcode == 0) {
    get_logical_devices(ip->sval[0], port->ival[0]);
  }

  if (*argtable) {
    arg_freetable(argtable, argtable_len);
  }
  return exitcode;
}

int get_logical_nodes_subparser(int argc, char *argv[])
{
  const char *description =
    "IEC 61850 Service: GetLogicalDeviceDirectory\n\n"
    "This command is used to read the list of logical nodes from a server or logical device.\n"
    "Maps to MMS function GetNameList.";
  const char *example = ".\\iec61850_actions.exe get logical_nodes all 10.166.13.247 -p 1002";

  struct arg_lit *help = arg_lit0(NULL, "help", NULL);
  struct arg_int *port =
    arg_int0("p", "port", "<int>", "port number of the server (optional, default: 102)");
  struct arg_rex *cmd = arg_rex1(NULL, NULL, "get", NULL, REG_EXTENDED, NULL);
  struct arg_rex *subcmd =
    arg_rex1(NULL, NULL, "(logical_)?nodes?", "logical_nodes", REG_EXTENDED, NULL);
  struct arg_str *name =
    arg_str1(NULL, NULL, "<name>", "name of a logical device or 'all' (required)");
  struct arg_rex *ip = arg_rex1(
    NULL, NULL, IP_REX, "<ip address>", REG_EXTENDED, "ip address of the server (required)");
  struct arg_end *end = arg_end(20);
  void *argtable[] = { cmd, subcmd, name, ip, port, help, end };
  int argtable_len = sizeof(argtable) / sizeof(argtable[0]);

  int exitcode = 0;
  exitcode = parse_get_subcommand(argc, argv, argtable, argtable_len, description, example);
  if (exitcode == 0) {
    get_logical_nodes(ip->sval[0], port->ival[0], name->sval[0]);
  }

  if (*argtable) {
    arg_freetable(argtable, argtable_len);
  }
  return exitcode;
}

int get_data_objects_subparser(int argc, char *argv[])
{
  const char *description =
    "IEC 61850 Service: GetLogicalNodeDirectory\n\n"
    "This command is used to read the list of data objects from a server or logical node.\n"
    "Maps to MMS function GetNameList.";
  const char *example = "./iec61850_actions get data_objects simpleIOGenericIO/LLN0 192.121.61.2";

  struct arg_lit *help = arg_lit0(NULL, "help", NULL);
  struct arg_int *port =
    arg_int0("p", "port", "<int>", "port number of the server (optional, default: 102)");
  struct arg_rex *cmd = arg_rex1(NULL, NULL, "get", NULL, REG_EXTENDED, NULL);
  struct arg_rex *subcmd =
    arg_rex1(NULL, NULL, "(data_)?objects?", "data_objects", REG_EXTENDED, NULL);
  struct arg_str *name =
    arg_str1(NULL, NULL, "<name>", "name of a logical node or 'all' (required)");
  struct arg_rex *ip = arg_rex1(
    NULL, NULL, IP_REX, "<ip address>", REG_EXTENDED, "ip address of the server (required)");
  struct arg_end *end = arg_end(20);
  void *argtable[] = { cmd, subcmd, name, ip, port, help, end };
  int argtable_len = sizeof(argtable) / sizeof(argtable[0]);

  int exitcode = 0;
  exitcode = parse_get_subcommand(argc, argv, argtable, argtable_len, description, example);
  if (exitcode == 0) {
    get_data_objects(ip->sval[0], port->ival[0], name->sval[0]);
  }

  if (*argtable) {
    arg_freetable(argtable, argtable_len);
  }
  return exitcode;
}

int get_data_attributes_subparser(int argc, char *argv[])
{
  const char *description =
    "IEC 61850 Service: GetDataDirectory\n\n"
    "This command is used to read the list of data attributes from a server or data object.\n"
    "Operates recursively to read any data attributes in the hierarchy below another data "
    "attribute.\n"
    "Maps to MMS function GetNameList.";
  const char *example =
    ".\\iec61850_actions.exe get data_attributes simpleIOGenericIO/LLN0.NamPlt 192.35.163.15";

  struct arg_lit *help = arg_lit0(NULL, "help", NULL);
  struct arg_int *port =
    arg_int0("p", "port", "<int>", "port number of the server (optional, default: 102)");
  struct arg_rex *cmd = arg_rex1(NULL, NULL, "get", NULL, REG_EXTENDED, NULL);
  struct arg_rex *subcmd =
    arg_rex1(NULL, NULL, "(data_)?attributes?", "data_attributes", REG_EXTENDED, NULL);
  struct arg_str *name =
    arg_str1(NULL, NULL, "<name>", "name of a data object or 'all' (required)");
  struct arg_rex *ip = arg_rex1(
    NULL, NULL, IP_REX, "<ip address>", REG_EXTENDED, "ip address of the server (required)");
  struct arg_end *end = arg_end(20);
  void *argtable[] = { cmd, subcmd, name, ip, port, help, end };
  int argtable_len = sizeof(argtable) / sizeof(argtable[0]);

  int exitcode = 0;
  exitcode = parse_get_subcommand(argc, argv, argtable, argtable_len, description, example);
  if (exitcode == 0) {
    get_data_attributes(ip->sval[0], port->ival[0], name->sval[0]);
  }

  if (*argtable) {
    arg_freetable(argtable, argtable_len);
  }
  return exitcode;
}

int get_value_subparser(int argc, char *argv[])
{
  const char *description =
    "IEC 61850 Service: GetDataValues\n\n"
    "This command is used to read the value of a data attribute. Data attribute name must be\n"
    "fully qualified in the form: <Logical Device>/<Logical Node>.<Data Object>.<Attribute>.\n"
    "The functional constraint must be provided either by using the -f flag or it may be appended\n"
    "to the data attribute name in square brackets.\n"
    "Maps to MMS function Read.";
  const char *example = ".\\iec61850_actions.exe get value simpleIOGenericIO/GGIO1.AnIn1.mag.f[MX] "
                        "192.35.163.15";

  struct arg_lit *help = arg_lit0(NULL, "help", NULL);
  struct arg_int *port =
    arg_int0("p", "port", "<int>", "port number of the server (optional, default: 102)");
  struct arg_rex *cmd = arg_rex1(NULL, NULL, "get", NULL, REG_EXTENDED, NULL);
  struct arg_rex *subcmd = arg_rex1(NULL, NULL, "values?", "value", REG_EXTENDED, NULL);
  struct arg_str *name = arg_str1(NULL, NULL, "<name>", "name of a data attribute (required)");
  struct arg_rex *ip = arg_rex1(
    NULL, NULL, IP_REX, "<ip address>", REG_EXTENDED, "ip address of the server (required)");
  struct arg_str *fc =
    arg_str0("f", "fc", "<str>", "functional constraint of the data attribute (optional)");
  struct arg_end *end = arg_end(20);
  void *argtable[] = { cmd, subcmd, name, ip, fc, port, help, end };
  int argtable_len = sizeof(argtable) / sizeof(argtable[0]);

  int exitcode = 0;
  exitcode = parse_get_subcommand(argc, argv, argtable, argtable_len, description, example);
  if (exitcode == 0) {
    get_value(ip->sval[0], port->ival[0], name->sval[0], fc->sval[0]);
  }

  if (*argtable) {
    arg_freetable(argtable, argtable_len);
  }
  return exitcode;
}

int get_data_sets_subparser(int argc, char *argv[])
{
  const char *description =
    "This command is used to read the list of data sets from a server.\n"
    "Output will also indicate whether the data set is deletable.\n"
    "This functionality does not map directly to an IEC 61850 service or MMS function.";
  const char *example = "./iec61850_actions get data_sets localhost -p 103";

  struct arg_lit *help = arg_lit0(NULL, "help", NULL);
  struct arg_int *port =
    arg_int0("p", "port", "<int>", "port number of the server (optional, default: 102)");
  struct arg_rex *cmd = arg_rex1(NULL, NULL, "get", NULL, REG_EXTENDED, NULL);
  struct arg_rex *subcmd = arg_rex1(NULL, NULL, "data_sets?", "data_sets", REG_EXTENDED, NULL);
  struct arg_rex *ip = arg_rex1(
    NULL, NULL, IP_REX, "<ip address>", REG_EXTENDED, "ip address of the server (required)");
  struct arg_end *end = arg_end(20);
  void *argtable[] = { cmd, subcmd, ip, port, help, end };
  int argtable_len = sizeof(argtable) / sizeof(argtable[0]);

  int exitcode = 0;
  exitcode = parse_get_subcommand(argc, argv, argtable, argtable_len, description, example);
  if (exitcode == 0) {
    get_data_sets(ip->sval[0], port->ival[0]);
  }

  if (*argtable) {
    arg_freetable(argtable, argtable_len);
  }
  return exitcode;
}

int get_data_set_members_subparser(int argc, char *argv[])
{
  const char *description =
    "IEC 61850 Service: GetDataSetDirectory\n\n"
    "This command is used to read the list of data attributes in a particular data set\n"
    "or all data sets on a server. Maps to MMS function GetNameList.\n";
  const char *example = ".\\iec61850_actions.exe get data_set_members "
                        "simpleIOGenericIO/LLN0.Measurements 10.57.130.11 -p 1002";

  struct arg_lit *help = arg_lit0(NULL, "help", NULL);
  struct arg_int *port =
    arg_int0("p", "port", "<int>", "port number of the server (optional, default: 102)");
  struct arg_rex *cmd = arg_rex1(NULL, NULL, "get", NULL, REG_EXTENDED, NULL);
  struct arg_rex *subcmd =
    arg_rex1(NULL, NULL, "data_set_members?", "data_set_members", REG_EXTENDED, NULL);
  struct arg_str *name = arg_str1(NULL, NULL, "<name>", "name of a data set or 'all' (required)");
  struct arg_rex *ip = arg_rex1(
    NULL, NULL, IP_REX, "<ip address>", REG_EXTENDED, "ip address of the server (required)");
  struct arg_lit *vals =
    arg_lit0(NULL, "values", "read the value of each data set member (optional)");
  struct arg_end *end = arg_end(20);
  void *argtable[] = { cmd, subcmd, name, ip, vals, port, help, end };
  int argtable_len = sizeof(argtable) / sizeof(argtable[0]);

  int exitcode = 0;
  exitcode = parse_get_subcommand(argc, argv, argtable, argtable_len, description, example);
  if (exitcode == 0) {
    bool readVals = vals->count > 0;
    get_data_set_members(ip->sval[0], port->ival[0], name->sval[0], readVals);
  }

  if (*argtable) {
    arg_freetable(argtable, argtable_len);
  }
  return exitcode;
}

int get_reports_subparser(int argc, char *argv[])
{
  const char *description =
    "This command is used to read the list of reports published by a server.\n"
    "This functionality does not map directly to an IEC 61850 service or MMS function.";
  const char *example = "./iec61850_actions get reports 192.160.185.250";

  struct arg_lit *help = arg_lit0(NULL, "help", NULL);
  struct arg_int *port =
    arg_int0("p", "port", "<int>", "port number of the server (optional, default: 102)");
  struct arg_rex *cmd = arg_rex1(NULL, NULL, "get", NULL, REG_EXTENDED, NULL);
  struct arg_rex *subcmd = arg_rex1(NULL, NULL, "reports?", "reports", REG_EXTENDED, NULL);
  struct arg_rex *ip = arg_rex1(
    NULL, NULL, IP_REX, "<ip address>", REG_EXTENDED, "ip address of the server (required)");
  struct arg_end *end = arg_end(20);
  void *argtable[] = { cmd, subcmd, ip, port, help, end };
  int argtable_len = sizeof(argtable) / sizeof(argtable[0]);

  int exitcode = 0;
  exitcode = parse_get_subcommand(argc, argv, argtable, argtable_len, description, example);
  if (exitcode == 0) {
    get_reports(ip->sval[0], port->ival[0]);
  }

  if (*argtable) {
    arg_freetable(argtable, argtable_len);
  }
  return exitcode;
}

int get_files_subparser(int argc, char *argv[])
{
  const char *description = "IEC 61850 Service: GetFile\n\n"
                            "This command is used to read the list of files on a server.\n"
                            "Maps to MMS function FileOpen.";
  const char *example = ".\\iec61850_actions.exe get files 10.166.13.247";

  struct arg_lit *help = arg_lit0(NULL, "help", NULL);
  struct arg_int *port =
    arg_int0("p", "port", "<int>", "port number of the server (optional, default: 102)");
  struct arg_rex *cmd = arg_rex1(NULL, NULL, "get", NULL, REG_EXTENDED, NULL);
  struct arg_rex *subcmd = arg_rex1(NULL, NULL, "files?", "files", REG_EXTENDED, NULL);
  struct arg_rex *ip = arg_rex1(
    NULL, NULL, IP_REX, "<ip address>", REG_EXTENDED, "ip address of the server (required)");
  struct arg_end *end = arg_end(20);
  void *argtable[] = { cmd, subcmd, ip, port, help, end };
  int argtable_len = sizeof(argtable) / sizeof(argtable[0]);

  int exitcode = 0;
  exitcode = parse_get_subcommand(argc, argv, argtable, argtable_len, description, example);
  if (exitcode == 0) {
    get_files(ip->sval[0], port->ival[0]);
  }

  if (*argtable) {
    arg_freetable(argtable, argtable_len);
  }
  return exitcode;
}

int get_log_blocks_subparser(int argc, char *argv[])
{
  const char *description =
    "IEC 61850 Service: GetLCBValues\n\n"
    "This command is used to read the list of log control blocks (LCB) from a server\n"
    "and the values associated with the LCB. When traversing the data model,\n"
    "Log control blocks are assumed to be in logical node zero (LLN0).\n"
    "Maps to MMS function Read.";
  const char *example = ".\\iec61850_actions.exe get log_blocks 192.12.202.57";

  struct arg_lit *help = arg_lit0(NULL, "help", NULL);
  struct arg_int *port =
    arg_int0("p", "port", "<int>", "port number of the server (optional, default: 102)");
  struct arg_rex *cmd = arg_rex1(NULL, NULL, "get", NULL, REG_EXTENDED, NULL);
  struct arg_rex *subcmd = arg_rex1(NULL, NULL, "log_blocks?", "log_blocks", REG_EXTENDED, NULL);
  struct arg_rex *ip = arg_rex1(
    NULL, NULL, IP_REX, "<ip address>", REG_EXTENDED, "ip address of the server (required)");
  struct arg_end *end = arg_end(20);
  void *argtable[] = { cmd, subcmd, ip, port, help, end };
  int argtable_len = sizeof(argtable) / sizeof(argtable[0]);

  int exitcode = 0;
  exitcode = parse_get_subcommand(argc, argv, argtable, argtable_len, description, example);
  if (exitcode == 0) {
    get_log_blocks(ip->sval[0], port->ival[0]);
  }

  if (*argtable) {
    arg_freetable(argtable, argtable_len);
  }
  return exitcode;
}

int get_log_subparser(int argc, char *argv[])
{
  const char *description =
    "IEC 61850 Service: QueryLogAfter\n\n"
    "This command is used to read the entries of a specified log. Will query\n"
    "the log after the oldest (first) entry.\n"
    "Maps to MMS function ReadJournal.";
  const char *example = "./iec61850_actions get log simpleIOGenericIO/LLN0.EventLog 10.101.10.101";

  struct arg_lit *help = arg_lit0(NULL, "help", NULL);
  struct arg_int *port =
    arg_int0("p", "port", "<int>", "port number of the server (optional, default: 102)");
  struct arg_rex *cmd = arg_rex1(NULL, NULL, "get", NULL, REG_EXTENDED, NULL);
  struct arg_rex *subcmd = arg_rex1(NULL, NULL, "log", NULL, REG_EXTENDED, NULL);
  struct arg_str *name = arg_str1(NULL, NULL, "<name>", "name of a log control block (required)");
  struct arg_rex *ip = arg_rex1(
    NULL, NULL, IP_REX, "<ip address>", REG_EXTENDED, "ip address of the server (required)");
  struct arg_end *end = arg_end(20);
  void *argtable[] = { cmd, subcmd, name, ip, port, help, end };
  int argtable_len = sizeof(argtable) / sizeof(argtable[0]);

  int exitcode = 0;
  exitcode = parse_get_subcommand(argc, argv, argtable, argtable_len, description, example);
  if (exitcode == 0) {
    get_log(ip->sval[0], port->ival[0], name->sval[0]);
  }

  if (*argtable) {
    arg_freetable(argtable, argtable_len);
  }
  return exitcode;
}

void display_get_help(void *argtable[])
{
  printf("Usage: %s", PROGNAME);
  arg_print_syntax(stdout, argtable, "\n");
  printf("\nIEC 61850 Actions Library version %s, `get` command\n\n", PROJECT_VER);
  printf("The `get` command has multiple subcommands to specify a data request:\n"
         "  logical_devices   List logical devices on a server\n"
         "  logical_nodes     List logical nodes of a logical device or all on the server\n"
         "  data_objects      List data objects of a logical node or all on the server\n"
         "  data_attributes   List data attributes of a data object or all on the server\n"
         "  value             Read the value of a data attribute\n"
         "  data_sets         List data sets published by a server\n"
         "  data_set_members  List the data in a data set\n"
         "  reports           List the reports published by a server\n"
         "  files             List files on a server\n"
         "  log_blocks        List log control blocks on a server\n"
         "  log               Read the entries of a specified log\n\n");
  printf("See '%s get <subcommand> --help' to read more about a specific subcommand.\n", PROGNAME);
}

int get_subparser(int argc, char *argv[])
{
  int exitcode = 0;

  const char *subparsers =
    "(logical_)?devices?|(logical_)?nodes?|(data_)?objects?|(data_)?"
    "attributes?|values?|data_sets?(_member)?s?|reports?|files?|log(_blocks?)?";

  struct arg_lit *help = arg_lit0(NULL, "help", NULL);
  struct arg_rex *cmd = arg_rex1(NULL, NULL, "get", NULL, REG_EXTENDED, NULL);
  struct arg_rex *subcmd = arg_rex1(NULL, NULL, subparsers, "<subcommand>", REG_EXTENDED, NULL);
  struct arg_str *args = arg_strn(NULL, NULL, "<args>", 0, 10, NULL);
  struct arg_end *end = arg_end(20);
  void *argtable[] = { cmd, subcmd, args, help, end };

  int nerrors;
  if (arg_nullcheck(argtable) == 0) {
    nerrors = arg_parse(argc, argv, argtable);

    if (subcmd->count > 0) {
      const char *sc = subcmd->sval[0];
      if ((strcasecmp(sc, "logical_devices") == 0) || (strcasecmp(sc, "devices") == 0)) {
        exitcode = get_logical_devices_subparser(argc, argv);
      } else if ((strcasecmp(sc, "logical_nodes") == 0) || (strcasecmp(sc, "nodes") == 0)) {
        exitcode = get_logical_nodes_subparser(argc, argv);
      } else if ((strcasecmp(sc, "data_objects") == 0) || (strcasecmp(sc, "objects") == 0)) {
        exitcode = get_data_objects_subparser(argc, argv);
      } else if ((strcasecmp(sc, "data_attributes") == 0) || (strcasecmp(sc, "attributes") == 0)) {
        exitcode = get_data_attributes_subparser(argc, argv);
      } else if ((strcasecmp(sc, "values") == 0) || (strcasecmp(sc, "value") == 0)) {
        exitcode = get_value_subparser(argc, argv);
      } else if (strcasecmp(sc, "data_sets") == 0) {
        exitcode = get_data_sets_subparser(argc, argv);
      } else if (strcasecmp(sc, "data_set_members") == 0) {
        exitcode = get_data_set_members_subparser(argc, argv);
      } else if (strcasecmp(sc, "reports") == 0) {
        exitcode = get_reports_subparser(argc, argv);
      } else if (strcasecmp(sc, "files") == 0) {
        exitcode = get_files_subparser(argc, argv);
      } else if (strcasecmp(sc, "log_blocks") == 0) {
        exitcode = get_log_blocks_subparser(argc, argv);
      } else if (strcasecmp(sc, "log") == 0) {
        exitcode = get_log_subparser(argc, argv);
      } else {
        exitcode = 1;
        display_get_help(argtable);
        printf("\nError parsing command line:\n`get` subcommand '%s' not implemented\n", sc);
      }
    } else if (help->count > 0) {
      exitcode = 1;
      display_get_help(argtable);
    } else if (nerrors > 0) {
      exitcode = 1;
      display_get_help(argtable);
      printf("\nError parsing command line:\n");
      arg_print_errors(stdout, end, PROGNAME);
    } else {
      exitcode = 1;
      display_get_help(argtable);
    }
  }

  if (*argtable) {
    arg_freetable(argtable, sizeof(argtable) / sizeof(argtable[0]));
  }
  return exitcode;
}

void display_control_help(void *argtable[], const char *description, const char *example)
{
  printf("%s\n\n", description);
  printf("Usage: %s control <name> [--<datatype>]=<value> <ip address> [--sbo] "
         "[--swv] [-p <int>] [--help]\n",
         PROGNAME);
  printf("\nThis command permits the following arguments:\n");
  arg_print_glossary(stdout, argtable, "  %-25s %s\n");
  printf("\nExample: %s\n", example);
}

int control_subparser(int argc, char *argv[])
{
  const char *description =
    "IEC 61850 Service: Control\n\n"
    "This command is used to control a controllable data object. Control\n"
    "operations can be applied to data objects that have a control model (ctlModel)\n"
    "attribute that is not equal to 0 (status only). Supports direct-operate, select-\n"
    "before-operate, and select-before-operate-with-value control models.\n"
    "Maps to MMS function Write.";
  const char *example = ".\\iec61850_actions.exe control simpleIOGenericIO/GGIO1.SPCSO1 "
                        "--bool ON 192.22.2.91";

  struct arg_lit *help = arg_lit0(NULL, "help", NULL);
  struct arg_int *port =
    arg_int0("p", "port", "<int>", "port number of the server (optional, default: 102)");
  struct arg_rex *cmd = arg_rex1(NULL, NULL, "control", NULL, REG_EXTENDED, NULL);
  struct arg_str *name =
    arg_str1(NULL, NULL, "<name>", "name of a data object to operate (required)");
  struct arg_str *boolVal = arg_str0(NULL, "bool", "<value>", "boolean control value");
  struct arg_int *intVal = arg_int0(NULL, "int", "<value>", "integer control value");
  struct arg_int *uintVal = arg_int0(NULL, "uint", "<value>", "unsigned integer control value");
  struct arg_dbl *floatVal = arg_dbl0(NULL, "float", "<value>", "float control value");
  struct arg_str *strVal = arg_str0(NULL, "str", "<value>", "string control value");
  struct arg_rex *ip = arg_rex1(
    NULL, NULL, IP_REX, "<ip address>", REG_EXTENDED, "ip address of the server (required)");
  struct arg_lit *sbo = arg_lit0("s", "sbo", "use select-before-operate mode (optional)");
  struct arg_lit *sboWithVal =
    arg_lit0("v", "swv", "use select-before-operate-with-value mode (optional)");
  struct arg_lit *toggleFlag = arg_lit0(
    NULL, "toggle", "toggle boolean value false -> true -> false or vice-versa (optional)");
  struct arg_end *end = arg_end(20);
  void *argtable[] = { cmd, name, boolVal,    intVal,     uintVal, floatVal, strVal,
                       ip,  sbo,  sboWithVal, toggleFlag, port,    help,     end };

  int exitcode = 0;
  int nerrors;
  if (arg_nullcheck(argtable) == 0) {
    port->ival[0] = DEFAULT_PORT;
    nerrors = arg_parse(argc, argv, argtable);

    if (help->count) {
      display_control_help(argtable, description, example);
    } else if (nerrors > 0) {
      exitcode = 1;
      display_control_help(argtable, description, example);
      printf("\nError parsing command line:\n");
      arg_print_errors(stdout, end, PROGNAME);
    } else {
      bool setSbo = sbo->count > 0;
      bool setSboWithVal = sboWithVal->count > 0;

      if (boolVal->count > 0) {
        bool toggle = toggleFlag->count > 0;
        control_bool(ip->sval[0],
                     port->ival[0],
                     name->sval[0],
                     boolVal->sval[0],
                     setSbo,
                     setSboWithVal,
                     toggle);
      } else if (intVal->count > 0) {
        control_int(
          ip->sval[0], port->ival[0], name->sval[0], intVal->ival[0], setSbo, setSboWithVal);
      } else if (uintVal->count > 0) {
        control_uint(
          ip->sval[0], port->ival[0], name->sval[0], uintVal->ival[0], setSbo, setSboWithVal);
      } else if (floatVal->count > 0) {
        control_float(
          ip->sval[0], port->ival[0], name->sval[0], floatVal->dval[0], setSbo, setSboWithVal);
      } else if (strVal->count > 0) {
        control_string(
          ip->sval[0], port->ival[0], name->sval[0], strVal->sval[0], setSbo, setSboWithVal);
      } else {
        exitcode = 1;
        display_help(argtable, description, example);
        printf("\nError parsing command line:\n"
               "Control value is required. Enter a control value using the appropriate datatype "
               "switch, for example:\n"
               "  --bool false\n"
               "  --int -123\n");
      }
    }
  }

  if (*argtable) {
    arg_freetable(argtable, sizeof(argtable) / sizeof(argtable[0]));
  }
  return exitcode;
}

void display_set_help(void *argtable[], const char *description, const char *example)
{
  printf("%s\n\n", description);
  printf("Usage: %s set <name> [--<datatype>]=<value> <ip address> [--fc <str>] "
         "[--read] [--help]\n",
         PROGNAME);
  printf("\nThis command permits the following arguments:\n");
  arg_print_glossary(stdout, argtable, "  %-25s %s\n");
  printf("\nExample: %s\n", example);
}

int set_subparser(int argc, char *argv[])
{
  const char *description =
    "IEC 61850 Service: SetDataValues\n\n"
    "This command is used to set the value of a data attribute. Data attribute name must be\n"
    "fully qualified in the form: <Logical Device>/<Logical Node>.<Data Object>.<Attribute>.\n"
    "The functional constraint must be provided either by using the -f flag or it may be appended\n"
    "to the data attribute name in square brackets.\n"
    "Maps to MMS function Write.";
  const char *example = "./iec61850_actions set simpleIOGenericIO/LLN0.NamPlt.vendor "
                        "--str \"New Name\" -f DC 10.4.27.1";

  struct arg_lit *help = arg_lit0(NULL, "help", NULL);
  struct arg_int *port =
    arg_int0("p", "port", "<int>", "port number of the server (optional, default: 102)");
  struct arg_rex *cmd = arg_rex1(NULL, NULL, "set", NULL, REG_EXTENDED, NULL);
  struct arg_str *name =
    arg_str1(NULL, NULL, "<name>", "name of a data attribute to change (required)");
  struct arg_str *boolVal = arg_str0(NULL, "bool", "<value>", "boolean set value");
  struct arg_int *intVal = arg_int0(NULL, "int", "<value>", "integer set value");
  struct arg_int *uintVal = arg_int0(NULL, "uint", "<value>", "unsigned integer set value");
  struct arg_dbl *floatVal = arg_dbl0(NULL, "float", "<value>", "float set value");
  struct arg_str *strVal = arg_str0(NULL, "str", "<value>", "string set value");
  struct arg_rex *ip = arg_rex1(
    NULL, NULL, IP_REX, "<ip address>", REG_EXTENDED, "ip address of the server (required)");
  struct arg_str *fc =
    arg_str0("f", "fc", "<str>", "functional constraint of the data attribute (optional)");
  struct arg_lit *readFlag =
    arg_lit0(NULL, "read", "read value after set to validate success (optional)");
  struct arg_end *end = arg_end(20);
  void *argtable[] = { cmd, name, boolVal,  intVal, uintVal, floatVal, strVal,
                       ip,  fc,   readFlag, port,   help,    end };

  int exitcode = 0;
  int nerrors;
  if (arg_nullcheck(argtable) == 0) {
    port->ival[0] = DEFAULT_PORT;
    nerrors = arg_parse(argc, argv, argtable);

    if (help->count) {
      display_set_help(argtable, description, example);
    } else if (nerrors > 0) {
      exitcode = 1;
      display_set_help(argtable, description, example);
      printf("\nError parsing command line:\n");
      arg_print_errors(stdout, end, PROGNAME);
    } else {
      bool read = readFlag->count > 0;
      if (boolVal->count > 0) {
        set_bool(ip->sval[0], port->ival[0], name->sval[0], boolVal->sval[0], fc->sval[0], read);
      } else if (intVal->count > 0) {
        set_int(ip->sval[0], port->ival[0], name->sval[0], intVal->ival[0], fc->sval[0], read);
      } else if (uintVal->count > 0) {
        set_uint(ip->sval[0], port->ival[0], name->sval[0], uintVal->ival[0], fc->sval[0], read);
      } else if (floatVal->count > 0) {
        set_float(ip->sval[0], port->ival[0], name->sval[0], floatVal->dval[0], fc->sval[0], read);
      } else if (strVal->count > 0) {
        set_string(ip->sval[0], port->ival[0], name->sval[0], strVal->sval[0], fc->sval[0], read);
      } else {
        exitcode = 1;
        display_set_help(argtable, description, example);
        printf("\nError parsing command line:\n"
               "Set value is required. Enter a value to set using the appropriate datatype "
               "switch, for example:\n"
               "  --str NewVal\n"
               "  --uint 1001\n");
      }
    }
  }

  if (*argtable) {
    arg_freetable(argtable, sizeof(argtable) / sizeof(argtable[0]));
  }
  return exitcode;
}

int delete_file_subparser(int argc, char *argv[])
{
  const char *description = "IEC 61850 Service: DeleteFile\n\n"
                            "This command is used to delete a file from a server.\n"
                            "Maps to MMS function FileDelete.";
  const char *example = "./iec61850_actions delete file important.scl localhost";

  struct arg_lit *help = arg_lit0(NULL, "help", NULL);
  struct arg_int *port =
    arg_int0("p", "port", "<int>", "port number of the server (optional, default: 102)");
  struct arg_rex *cmd = arg_rex1(NULL, NULL, "delete", NULL, REG_EXTENDED, NULL);
  struct arg_rex *subcmd = arg_rex1(NULL, NULL, "file", NULL, REG_EXTENDED, NULL);
  struct arg_str *name = arg_str1(NULL, NULL, "<name>", "name of a file to delete (required)");
  struct arg_rex *ip = arg_rex1(
    NULL, NULL, IP_REX, "<ip address>", REG_EXTENDED, "ip address of the server (required)");
  struct arg_end *end = arg_end(20);
  void *argtable[] = { cmd, subcmd, name, ip, port, help, end };
  int argtable_len = sizeof(argtable) / sizeof(argtable[0]);

  int exitcode = 0;
  if (arg_nullcheck(argtable) == 0) {
    port->ival[0] = DEFAULT_PORT;
    int nerrors = arg_parse(argc, argv, argtable);

    if (help->count > 0) {
      exitcode = 1;
      display_help(argtable, description, example);
    } else if (nerrors > 0) {
      exitcode = 1;
      display_help(argtable, description, example);
      printf("\nError parsing command line:\n");
      arg_print_errors(stdout, end, PROGNAME);
    } else {
      delete_file(ip->sval[0], port->ival[0], name->sval[0]);
    }
  }

  if (*argtable) {
    arg_freetable(argtable, argtable_len);
  }
  return exitcode;
}

int delete_data_set_subparser(int argc, char *argv[])
{
  const char *description = "IEC 61850 Service: DeleteDataSet\n\n"
                            "This command is used to delete a data set from a server.\n"
                            "Note: not all data sets are deletable in accordance with the server\n"
                            "settings. Performing a `get data_sets` operation can confirm if the\n"
                            "server holds deletable data sets."
                            "Maps to MMS function DeleteNamedVariableList.";
  const char *example =
    "./iec61850_actions delete data_set simpleIOGenericIO/LLN0.Events localhost";

  struct arg_lit *help = arg_lit0(NULL, "help", NULL);
  struct arg_int *port =
    arg_int0("p", "port", "<int>", "port number of the server (optional, default: 102)");
  struct arg_rex *cmd = arg_rex1(NULL, NULL, "delete", NULL, REG_EXTENDED, NULL);
  struct arg_rex *subcmd = arg_rex1(NULL, NULL, "data_set", NULL, REG_EXTENDED, NULL);
  struct arg_str *name = arg_str1(NULL, NULL, "<name>", "name of a data set to delete (required)");
  struct arg_rex *ip = arg_rex1(
    NULL, NULL, IP_REX, "<ip address>", REG_EXTENDED, "ip address of the server (required)");
  struct arg_end *end = arg_end(20);
  void *argtable[] = { cmd, subcmd, name, ip, port, help, end };
  int argtable_len = sizeof(argtable) / sizeof(argtable[0]);

  int exitcode = 0;
  if (arg_nullcheck(argtable) == 0) {
    port->ival[0] = DEFAULT_PORT;
    int nerrors = arg_parse(argc, argv, argtable);

    if (help->count > 0) {
      exitcode = 1;
      display_help(argtable, description, example);
    } else if (nerrors > 0) {
      exitcode = 1;
      display_help(argtable, description, example);
      printf("\nError parsing command line:\n");
      arg_print_errors(stdout, end, PROGNAME);
    } else {
      delete_data_set(ip->sval[0], port->ival[0], name->sval[0]);
    }
  }

  if (*argtable) {
    arg_freetable(argtable, argtable_len);
  }
  return exitcode;
}

void display_delete_help(void *argtable[])
{
  printf("Usage: %s", PROGNAME);
  arg_print_syntax(stdout, argtable, "\n");
  printf("\nIEC 61850 Actions Library version %s, `delete` command\n\n", PROJECT_VER);
  printf("The `delete` command has multiple subcommands to specify data to remove:\n"
         "  data_set   Delete a data set from a server\n"
         "  file       Delete a file from a server\n\n");
  printf("See '%s delete <subcommand> --help' to read more about a specific subcommand.\n",
         PROGNAME);
}

int delete_subparser(int argc, char *argv[])
{
  int exitcode = 0;

  const char *subparsers = "data_set|file";

  struct arg_lit *help = arg_lit0(NULL, "help", NULL);
  struct arg_rex *cmd = arg_rex1(NULL, NULL, "delete", NULL, REG_EXTENDED, NULL);
  struct arg_rex *subcmd = arg_rex1(NULL, NULL, subparsers, "<subcommand>", REG_EXTENDED, NULL);
  struct arg_str *args = arg_strn(NULL, NULL, "<args>", 0, 10, NULL);
  struct arg_end *end = arg_end(20);
  void *argtable[] = { cmd, subcmd, args, help, end };

  int nerrors;
  if (arg_nullcheck(argtable) == 0) {
    nerrors = arg_parse(argc, argv, argtable);

    if (subcmd->count > 0) {
      const char *sc = subcmd->sval[0];
      if (strcasecmp(sc, "data_set") == 0) {
        exitcode = delete_data_set_subparser(argc, argv);
      } else if (strcasecmp(sc, "file") == 0) {
        exitcode = delete_file_subparser(argc, argv);
      } else {
        exitcode = 1;
        display_get_help(argtable);
        printf("\nError parsing command line:\n`delete` subcommand '%s' not implemented\n", sc);
      }
    } else if (help->count > 0) {
      exitcode = 1;
      display_delete_help(argtable);
    } else if (nerrors > 0) {
      exitcode = 1;
      display_delete_help(argtable);
      printf("\nError parsing command line:\n");
      arg_print_errors(stdout, end, PROGNAME);
    } else {
      exitcode = 1;
      display_delete_help(argtable);
    }
  }

  if (*argtable) {
    arg_freetable(argtable, sizeof(argtable) / sizeof(argtable[0]));
  }
  return exitcode;
}

void display_main_help(void *argtable[])
{
  printf("Usage: %s", PROGNAME);
  arg_print_syntax(stdout, argtable, "\n");
  printf("IEC 61850 Actions Library version %s.\n\n", PROJECT_VER);
  printf("These are the available IEC 61850 commands:\n"
         "  get      Get information from a server\n"
         "  control  Operate a controllable data object\n"
         "  set      Write data to a server\n"
         "  delete   Remove data from a server\n\n");
  printf("See '%s <command> --help' to read more about a specific command.\n", PROGNAME);
}

int main(int argc, char **argv)
{
  int exitcode = 0;
  const char *commands = "get|delete|control|set";

  struct arg_lit *help = arg_lit0(NULL, "help", NULL);
  struct arg_rex *cmd = arg_rex1(NULL, NULL, commands, "<command>", REG_EXTENDED, NULL);
  struct arg_str *args = arg_strn(NULL, NULL, "<args>", 0, 10, NULL);
  struct arg_end *end = arg_end(20);
  void *argtable[] = { help, cmd, args, end };
  int nerrors;

  if (arg_nullcheck(argtable) == 0) {
    nerrors = arg_parse(argc, argv, argtable);

    /* dispatch to command handler if possible */
    if (cmd->count > 0) {
      if (strcasecmp(cmd->sval[0], "get") == 0) {
        exitcode = get_subparser(argc, argv);
      } else if (strcasecmp(cmd->sval[0], "control") == 0) {
        exitcode = control_subparser(argc, argv);
      } else if (strcasecmp(cmd->sval[0], "set") == 0) {
        exitcode = set_subparser(argc, argv);
      } else if (strcasecmp(cmd->sval[0], "delete") == 0) {
        exitcode = delete_subparser(argc, argv);
      } else {
        printf("\n%s subparser not implemented\n\n", cmd->sval[0]);
      }
    }
    /* in all other cases, just display main help (ignoring errors) */
    else {
      display_main_help(argtable);
    }
  }

  if (*argtable) {
    arg_freetable(argtable, sizeof(argtable) / sizeof(argtable[0]));
  }
  return exitcode;
}