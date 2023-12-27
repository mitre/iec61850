#include "iec61850_actions/app.h"

/**
 * Functionality to customize the lower layers of MMS
 *
 * If your use case requires customization of the lower layers of MMS,
 * the appropriate functionality can be uncommented and compiled into
 * a custom payload.
 */
void set_custom_connection(client_t *c)
{
  /* To change any parameters you need to get access to the underlying */
  /* MmsConnection and IsoConnectionParameters structs                 */
  /*
  MmsConnection mmsConnection = IedConnection_getMmsConnection(c->con);
  IsoConnectionParameters parameters = MmsConnection_getIsoConnectionParameters(mmsConnection);
  */

  /* Customize remote and local AP-Title */
  /*
  IsoConnectionParameters_setRemoteApTitle(parameters, "1.3.9999.13", 12);
  IsoConnectionParameters_setLocalApTitle(parameters, "1.2.1200.15.3", 1);
  */

  /* Ignore AP-Title - may be required by some obscure servers */
  /*
  IsoConnectionParameters_setRemoteApTitle(parameters, NULL, 0);
  IsoConnectionParameters_setLocalApTitle(parameters, NULL, 0);
  */

  /* Change parameters for presentation, session and transport layers */
  /*
  TSelector localTSelector = { 3, { 0x00, 0x01, 0x02 } };
  TSelector remoteTSelector = { 2, { 0x00, 0x01 } };
  SSelector remoteSSelector = { 2, { 0, 1 } };
  SSelector localSSelector = { 5, { 0, 1, 2, 3, 4 } };
  PSelector localPSelector = {4, { 0x12, 0x34, 0x56, 0x78 } };
  PSelector remotePSelector = {4, { 0x87, 0x65, 0x43, 0x21 } };
  IsoConnectionParameters_setRemoteAddresses(
    parameters, remotePSelector, remoteSSelector, localTSelector);
  IsoConnectionParameters_setLocalAddresses(
    parameters, localPSelector, localSSelector, remoteTSelector);
  */

  /* Use authentication */
  /*
  char* password = "user1@testpw";
  AcseAuthenticationParameter auth = AcseAuthenticationParameter_create();
  AcseAuthenticationParameter_setAuthMechanism(auth, ACSE_AUTH_PASSWORD);
  AcseAuthenticationParameter_setPassword(auth, password);
  IsoConnectionParameters_setAcseAuthenticationParameter(parameters, auth);
  */

  /* Change connection timeout */
  /*
  IedConnection_setConnectTimeout(c->con, 10000);
  */
}

bool connect_to_server(client_t *c, const char *hostname, int port)
{
  /* set_custom_connection(c); */
  client.connect(c, hostname, port);
  return (c->error == IED_ERROR_OK);
}

void print_results(result_t *res, const char *func_call, FILE *fp)
{
  if ((res->code == SUCCESS) & (strlen(res->message) > 1)) {
    fprintf(fp, "%s", res->message);
  } else if (res->code == SUCCESS) {
    fprintf(fp, "%s succeeded: No data returned\n", func_call);
  } else {
    fprintf(fp, "%s failed\n", func_call);
  }
}

void get_logical_devices(const char *hostname, int port)
{
  client_t c = client.create(stdout, stderr);
  if (connect_to_server(&c, hostname, port)) {
    result_t res = client.get_logical_devices(&c);
    print_results(&res, "Get logical devices", c.output);
    client.disconnect(&c);
  }
}

void get_logical_nodes(const char *hostname, int port, const char *ldName)
{
  client_t c = client.create(stdout, stderr);
  if (connect_to_server(&c, hostname, port)) {
    result_t res = client.get_logical_nodes(&c, ldName);
    print_results(&res, "Get logical nodes", c.output);
    client.disconnect(&c);
  }
}

void get_data_objects(const char *hostname, int port, const char *lnName)
{
  client_t c = client.create(stdout, stderr);
  if (connect_to_server(&c, hostname, port)) {
    result_t res = client.get_data_objects(&c, lnName);
    print_results(&res, "Get data objects", c.output);
    client.disconnect(&c);
  }
}

void get_data_attributes(const char *hostname, int port, const char *doName)
{
  client_t c = client.create(stdout, stderr);
  if (connect_to_server(&c, hostname, port)) {
    result_t res = client.get_data_attributes(&c, doName);
    print_results(&res, "Get data attributes", c.output);
    client.disconnect(&c);
  }
}

void get_value(const char *hostname, int port, const char *daName, const char *fcString)
{
  client_t c = client.create(stdout, stderr);
  if (connect_to_server(&c, hostname, port)) {
    result_t res = client.get_value(&c, daName, fcString);
    print_results(&res, "Get value", c.output);
    client.disconnect(&c);
  }
}

void get_data_sets(const char *hostname, int port)
{
  client_t c = client.create(stdout, stderr);
  if (connect_to_server(&c, hostname, port)) {
    result_t res = client.get_data_sets(&c);
    print_results(&res, "Get data sets", c.output);

    client.disconnect(&c);
  }
}

void get_data_set_members(const char *hostname, int port, const char *dsName, bool readVals)
{
  client_t c = client.create(stdout, stderr);
  if (connect_to_server(&c, hostname, port)) {
    result_t res = client.get_data_set_members(&c, dsName, readVals);
    print_results(&res, "Get data set members", c.output);
    client.disconnect(&c);
  }
}

void get_reports(const char *hostname, int port)
{
  client_t c = client.create(stdout, stderr);
  if (connect_to_server(&c, hostname, port)) {
    result_t res = client.get_reports(&c);
    print_results(&res, "Get reports", c.output);
    client.disconnect(&c);
  }
}

void get_files(const char *hostname, int port)
{
  client_t c = client.create(stdout, stderr);
  if (connect_to_server(&c, hostname, port)) {
    result_t res = client.get_files(&c);
    print_results(&res, "Get files", c.output);
    client.disconnect(&c);
  }
}

void get_log_blocks(const char *hostname, int port)
{
  client_t c = client.create(stdout, stderr);
  if (connect_to_server(&c, hostname, port)) {
    result_t res = client.get_log_blocks(&c);
    print_results(&res, "Get log blocks", c.output);
    client.disconnect(&c);
  }
}

void get_log(const char *hostname, int port, const char *lcbName)
{
  client_t c = client.create(stdout, stderr);
  if (connect_to_server(&c, hostname, port)) {
    result_t res = client.get_log(&c, lcbName);
    print_results(&res, "Get log", c.output);
    client.disconnect(&c);
  }
}

void control_bool(const char *hostname,
                  int port,
                  const char *doName,
                  const char *ctlVal,
                  bool sbo,
                  bool sboWithValue,
                  bool toggle)
{
  bool val;
  client_t c = client.create(stdout, stderr);
  if (connect_to_server(&c, hostname, port)) {
    if (parse_bool(ctlVal, &val)) {
      result_t res = client.control_bool(&c, doName, val, 0, sbo, sboWithValue);
      print_results(&res, "Control", c.output);
      
      if (toggle) {
        val = !val;
        res = client.control_bool(&c, doName, val, 0, sbo, sboWithValue);
        print_results(&res, "Control", c.output);
      }
    } else {
      fprintf(c.output, "Control failed\n");
      flogf(c.log, "control failed: Unable to parse '%s' as boolean value\n", ctlVal);
    }
    client.disconnect(&c);
  }
}

void control_int(const char *hostname,
                 int port,
                 const char *doName,
                 int ctlVal,
                 bool sbo,
                 bool sboWithValue)
{
  client_t c = client.create(stdout, stderr);
  if (connect_to_server(&c, hostname, port)) {
    result_t res = client.control_int(&c, doName, ctlVal, 0, sbo, sboWithValue);
    print_results(&res, "Control", c.output);
    client.disconnect(&c);
  }
}

void control_uint(const char *hostname,
                  int port,
                  const char *doName,
                  int ctlVal,
                  bool sbo,
                  bool sboWithValue)
{
  client_t c = client.create(stdout, stderr);
  if (connect_to_server(&c, hostname, port)) {
    result_t res = client.control_uint(&c, doName, ctlVal, 0, sbo, sboWithValue);
    print_results(&res, "Control", c.output);
    client.disconnect(&c);
  }
}

void control_float(const char *hostname,
                   int port,
                   const char *doName,
                   float ctlVal,
                   bool sbo,
                   bool sboWithValue)
{
  client_t c = client.create(stdout, stderr);
  if (connect_to_server(&c, hostname, port)) {
    result_t res = client.control_float(&c, doName, ctlVal, 0, sbo, sboWithValue);
    print_results(&res, "Control", c.output);
    client.disconnect(&c);
  }
}

void control_string(const char *hostname,
                    int port,
                    const char *doName,
                    const char *ctlVal,
                    bool sbo,
                    bool sboWithValue)
{
  client_t c = client.create(stdout, stderr);
  if (connect_to_server(&c, hostname, port)) {
    result_t res = client.control_string(&c, doName, ctlVal, 0, sbo, sboWithValue);
    print_results(&res, "Control", c.output);
    client.disconnect(&c);
  }
}

void set_bool(const char *hostname,
              int port,
              const char *daName,
              const char *setVal,
              const char *fcString,
              bool read)
{
  bool val;
  client_t c = client.create(stdout, stderr);
  if (connect_to_server(&c, hostname, port)) {
    if (parse_bool(setVal, &val)) {
      result_t res = client.set_bool(&c, daName, val, fcString);
      print_results(&res, "Set", c.output);

      if ((res.code == SUCCESS) && read) {
        get_value(hostname, port, daName, fcString);
      }

    } else {
      fprintf(c.output, "Set failed\n");
      flogf(c.log, "set failed: Unable to parse '%s' as boolean value\n", setVal);
    }
    client.disconnect(&c);
  }
}

void set_int(const char *hostname,
             int port,
             const char *daName,
             int setVal,
             const char *fcString,
             bool read)
{
  client_t c = client.create(stdout, stderr);
  if (connect_to_server(&c, hostname, port)) {
    result_t res = client.set_int(&c, daName, setVal, fcString);
    print_results(&res, "Set", c.output);

    if ((res.code == SUCCESS) && read) {
      get_value(hostname, port, daName, fcString);
    }

    client.disconnect(&c);
  }
}

void set_uint(const char *hostname,
              int port,
              const char *daName,
              int setVal,
              const char *fcString,
              bool read)
{
  client_t c = client.create(stdout, stderr);
  if (connect_to_server(&c, hostname, port)) {
    result_t res = client.set_uint(&c, daName, setVal, fcString);
    print_results(&res, "Set", c.output);

    if ((res.code == SUCCESS) && read) {
      get_value(hostname, port, daName, fcString);
    }

    client.disconnect(&c);
  }
}

void set_float(const char *hostname,
               int port,
               const char *daName,
               float setVal,
               const char *fcString,
               bool read)
{
  client_t c = client.create(stdout, stderr);
  if (connect_to_server(&c, hostname, port)) {
    result_t res = client.set_float(&c, daName, setVal, fcString);
    print_results(&res, "Set", c.output);

    if ((res.code == SUCCESS) && read) {
      get_value(hostname, port, daName, fcString);
    }

    client.disconnect(&c);
  }
}

void set_string(const char *hostname,
                int port,
                const char *daName,
                const char *setVal,
                const char *fcString,
                bool read)
{
  client_t c = client.create(stdout, stderr);
  if (connect_to_server(&c, hostname, port)) {
    result_t res = client.set_string(&c, daName, setVal, fcString);
    print_results(&res, "Set", c.output);

    if ((res.code == SUCCESS) && read) {
      get_value(hostname, port, daName, fcString);
    }

    client.disconnect(&c);
  }
}

void delete_data_set(const char *hostname, int port, const char *dsName)
{
  client_t c = client.create(stdout, stderr);
  if (connect_to_server(&c, hostname, port)) {
    result_t res = client.delete_data_set(&c, dsName);
    print_results(&res, "Delete data set", c.output);
    client.disconnect(&c);
  }
}

void delete_file(const char *hostname, int port, const char *fileName)
{
  client_t c = client.create(stdout, stderr);
  if (connect_to_server(&c, hostname, port)) {
    result_t res = client.delete_file(&c, fileName);
    print_results(&res, "Delete file", c.output);
    client.disconnect(&c);
  }
}
