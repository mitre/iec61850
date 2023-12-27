#include <inttypes.h>

#include "iec61850_client/utils.h"

#ifndef ACTIONS_H
#define ACTIONS_H

#define BUFSIZE 1023

typedef struct client
{
  IedConnection con;
  IedClientError error;
  FILE *output;
  FILE *log;
} client_t;

typedef enum
{
  LOGICAL_DEVICES,
  LOGICAL_NODES,
  DATA_OBJECTS,
  DATA_ATTRIBUTES,
  VALUES,
  DATA_SETS,
  BUFFERED_REPORTS,
  UNBUFFERED_REPORTS,
  DATA_SET_MEMBERS,
  DATA_SET_VALUES,
  FILES,
  LOG_BLOCKS,
} DataRequest;

client_t client__create(FILE *output, FILE *log);
void client__connect(client_t *self, const char *hostname, int tcpPort);
void client__disconnect(client_t *self);

result_t client__get_logical_devices(client_t *self);
result_t client__get_logical_nodes(client_t *self, const char *ldName);
result_t client__get_data_objects(client_t *self, const char *lnName);
result_t client__get_data_attributes(client_t *self, const char *doName);
result_t client__get_data_sets(client_t *self);
result_t client__get_data_set_members(client_t *self, const char *dsName, bool readVals);
result_t client__get_reports(client_t *self);
result_t client__get_files(client_t *self);
result_t client__get_log_blocks(client_t *self);
result_t client__get_log(client_t *self, const char *lcbName);
result_t client__get_value(client_t *self, const char *daName, const char *fcString);

result_t client__control_bool(client_t *self,
                              const char *doName,
                              bool ctlVal,
                              uint64_t opTime,
                              bool sbo,
                              bool sboWithValue);
result_t client__control_int(client_t *self,
                             const char *doName,
                             int ctlVal,
                             uint64_t opTime,
                             bool sbo,
                             bool sboWithValue);
result_t client__control_uint(client_t *self,
                              const char *doName,
                              int ctlVal,
                              uint64_t opTime,
                              bool sbo,
                              bool sboWithValue);
result_t client__control_float(client_t *self,
                               const char *doName,
                               float ctlVal,
                               uint64_t opTime,
                               bool sbo,
                               bool sboWithValue);
result_t client__control_string(client_t *self,
                                const char *doName,
                                const char *ctlVal,
                                uint64_t opTime,
                                bool sbo,
                                bool sboWithValue);

result_t client__set_bool(client_t *self, const char *doName, bool setVal, const char *fcString);
result_t client__set_int(client_t *self, const char *doName, int setVal, const char *fcString);
result_t client__set_uint(client_t *self, const char *doName, int setVal, const char *fcString);
result_t client__set_float(client_t *self, const char *doName, float setVal, const char *fcString);
result_t client__set_string(client_t *self,
                            const char *doName,
                            const char *setVal,
                            const char *fcString);

result_t client__delete_data_set(client_t *self, const char *dsName);
result_t client__delete_file(client_t *self, const char *fileName);

static const struct client_namespace
{
  /**
   * \brief Create a client object.
   *
   * \param output FILE pointer to print output messages
   * \param log FILE pointer to log status messages
   * \return connection object
   */
  client_t (*create)(FILE *output, FILE *log);

  /**
   * \brief Connect to a server.
   *
   * \param self client object
   * \param hostname the host name or IP address of the server to connect to
   * \param tcpPort the TCP port number of the server to connect to
   */
  void (*connect)(client_t *self, const char *hostname, int tcpPort);

  /**
   * \brief Close the connection to a server.
   *
   * This will close the MMS association and
   * the underlying TCP connection. The resources
   * associated with the connection object will
   * then be freed.
   *
   * \param self client object to be disconnected and destroyed
   */
  void (*disconnect)(client_t *self);

  /**
   * \brief Query server for all Logical Devices.
   *
   * \param self client object
   * \return result struct
   */
  result_t (*get_logical_devices)(client_t *self);

  /**
   * \brief Query server for Logical Nodes.
   *
   * \param self client object
   * \param ldName name of the logical device to query or
   * 'all' for all logical nodes on the server
   * \return result struct
   */
  result_t (*get_logical_nodes)(client_t *self, const char *ldName);

  /**
   * \brief Query server for Data Objects.
   *
   * \param self client object
   * \param ldName name of the logical node to query
   * in the form <Logical Device>/<Logical Node> or all
   * \return result struct
   */
  result_t (*get_data_objects)(client_t *self, const char *lnName);

  /**
   * \brief Query server the attributes of a data object.
   *
   * \param self client object
   * \param doName the data object to query in the
   * form <Logical Device>/<Logical Node>.<Data Object> or all
   * \return result struct
   */
  result_t (*get_data_attributes)(client_t *self, const char *doName);

  /**
   * \brief Query server for all Data Sets.
   *
   * \param self client object
   * \return result struct
   */
  result_t (*get_data_sets)(client_t *self);

  /**
   * \brief Query server for items in a Data Sets.
   *
   * \param self client object
   * \param dsName the data set to query in the
   * form <Logical Device>/<Logical Node>.<Data Set Name> or all
   * \param readVals if true, read the value of each member
   * \return result struct
   */
  result_t (*get_data_set_members)(client_t *self, const char *dsName, bool readVals);

  /**
   * \brief Query server for all Reports.
   *
   * \param self client object
   * \return result struct
   */
  result_t (*get_reports)(client_t *self);

  /**
   * \brief Query server for all Files.
   *
   * \param self client object
   * \return result struct
   */
  result_t (*get_files)(client_t *self);

  /**
   * \brief Query each device for log control blocks.
   *
   * \param self client object
   * \return result struct
   */
  result_t (*get_log_blocks)(client_t *self);

  /**
   * \brief Read the entries of a log control block.
   *
   * \param self client object
   * \param lcbName the qualified name of a log control
   * block in the form <Logical Device>/<Logical Node>.<LCB Name>
   * \return result struct
   */
  result_t (*get_log)(client_t *self, const char *lcbName);

  /**
   * \brief Read the value of a data attribute.
   *
   * \param self client object
   * \param daName fully qualified name of a data attribute
   * in the form <Logical Device>/<Logical Node>.<Data Object>.<Attribute>
   * \param fcString functional constraint of the data attribute
   * \return result struct
   */
  result_t (*get_value)(client_t *self, const char *daName, const char *fcString);

  /**
   * \brief Control a value on the server.
   *
   * Supports direct-operate, select-before-
   * operate, and select-before-operate-with-
   * value by setting argument flags.
   *
   * \param self client object
   * \param doName fully qualified name of a
   * data object in the form <Logical Device>/
   * <Logical Node>.<Data Object>
   * \param ctlVal control value
   * \param operateTime integer milliseconds after
   * the unix epoch time of 00:00:00 UTC on 1 January 1970
   * to conduct operation. O means immediate operation.
   * \param sbo use select-before-operate control mode
   * \param sboWithValue use select-before-operate-with-value
   * control mode
   * \return result struct
   */
  result_t (*control_bool)(client_t *self,
                           const char *doName,
                           bool ctlVal,
                           uint64_t opTime,
                           bool sbo,
                           bool sboWithValue);

  /**
   * \brief Control a value on the server.
   *
   * Supports direct-operate, select-before-
   * operate, and select-before-operate-with-
   * value by setting argument flags.
   *
   * \param self client object
   * \param doName fully qualified name of a
   * data object in the form <Logical Device>/
   * <Logical Node>.<Data Object>
   * \param ctlVal control value
   * \param operateTime integer milliseconds after
   * the unix epoch time of 00:00:00 UTC on 1 January 1970
   * to conduct operation. O means immediate operation.
   * \param sbo use select-before-operate control mode
   * \param sboWithValue use select-before-operate-with-value
   * control mode
   * \return result struct
   */
  result_t (*control_int)(client_t *self,
                          const char *doName,
                          int ctlVal,
                          uint64_t opTime,
                          bool sbo,
                          bool sboWithValue);

  /**
   * \brief Control a value on the server.
   *
   * Supports direct-operate, select-before-
   * operate, and select-before-operate-with-
   * value by setting argument flags.
   *
   * \param self client object
   * \param doName fully qualified name of a
   * data object in the form <Logical Device>/
   * <Logical Node>.<Data Object>
   * \param ctlVal control value
   * \param operateTime integer milliseconds after
   * the unix epoch time of 00:00:00 UTC on 1 January 1970
   * to conduct operation. O means immediate operation.
   * \param sbo use select-before-operate control mode
   * \param sboWithValue use select-before-operate-with-value
   * control mode
   * \return result struct
   */
  result_t (*control_uint)(client_t *self,
                           const char *doName,
                           int ctlVal,
                           uint64_t opTime,
                           bool sbo,
                           bool sboWithValue);

  /**
   * \brief Control a value on the server.
   *
   * Supports direct-operate, select-before-
   * operate, and select-before-operate-with-
   * value by setting argument flags.
   *
   * \param self client object
   * \param doName fully qualified name of a
   * data object in the form <Logical Device>/
   * <Logical Node>.<Data Object>
   * \param ctlVal control value
   * \param operateTime integer milliseconds after
   * the unix epoch time of 00:00:00 UTC on 1 January 1970
   * to conduct operation. O means immediate operation.
   * \param sbo use select-before-operate control mode
   * \param sboWithValue use select-before-operate-with-value
   * control mode
   * \return result struct
   */
  result_t (*control_float)(client_t *self,
                            const char *doName,
                            float ctlVal,
                            uint64_t opTime,
                            bool sbo,
                            bool sboWithValue);

  /**
   * \brief Control a value on the server.
   *
   * Supports direct-operate, select-before-
   * operate, and select-before-operate-with-
   * value by setting argument flags.
   *
   * \param self client object
   * \param doName fully qualified name of a
   * data object in the form <Logical Device>/
   * <Logical Node>.<Data Object>
   * \param ctlVal control value
   * \param operateTime integer milliseconds after
   * the unix epoch time of 00:00:00 UTC on 1 January 1970
   * to conduct operation. O means immediate operation.
   * \param sbo use select-before-operate control mode
   * \param sboWithValue use select-before-operate-with-value
   * control mode
   * \return result struct
   */
  result_t (*control_string)(client_t *self,
                             const char *doName,
                             const char *ctlVal,
                             uint64_t opTime,
                             bool sbo,
                             bool sboWithValue);
  /**
   * \brief Set a value on the server.
   *
   * \param self client object
   * \param daName fully qualified name of a data attribute
   * in the form <Logical Device>/<Logical Node>.<Data Object>.<Attribute>
   * \param setVal value to set
   * \param fcString functional constraint of the data attribute
   */
  result_t (*set_bool)(client_t *self, const char *daName, bool setVal, const char *fcString);

  /**
   * \brief Set a value on the server.
   *
   * \param self client object
   * \param daName fully qualified name of a data attribute
   * in the form <Logical Device>/<Logical Node>.<Data Object>.<Attribute>
   * \param setVal value to set
   * \param fcString functional constraint of the data attribute
   */
  result_t (*set_int)(client_t *self, const char *daName, int setVal, const char *fcString);

  /**
   * \brief Set a value on the server.
   *
   * \param self client object
   * \param daName fully qualified name of a data attribute
   * in the form <Logical Device>/<Logical Node>.<Data Object>.<Attribute>
   * \param setVal value to set
   * \param fcString functional constraint of the data attribute
   */
  result_t (*set_uint)(client_t *self, const char *daName, int setVal, const char *fcString);

  /**
   * \brief Set a value on the server.
   *
   * \param self client object
   * \param daName fully qualified name of a data attribute
   * in the form <Logical Device>/<Logical Node>.<Data Object>.<Attribute>
   * \param setVal value to set
   * \param fcString functional constraint of the data attribute
   */
  result_t (*set_float)(client_t *self, const char *daName, float setVal, const char *fcString);

  /**
   * \brief Set a value on the server.
   *
   * \param self client object
   * \param daName fully qualified name of a data attribute
   * in the form <Logical Device>/<Logical Node>.<Data Object>.<Attribute>
   * \param setVal value to set
   * \param fcString functional constraint of the data attribute
   */
  result_t (*set_string)(client_t *self,
                         const char *daName,
                         const char *setVal,
                         const char *fcString);

  /**
   * \brief Delete a Data Set from the server.
   *
   * \param self client object
   * \param dsName the data set to delete in the
   * form <Logical Device>/<Logical Node>.<Data Set Name> or all
   */
  result_t (*delete_data_set)(client_t *self, const char *dsName);

  /**
   * \brief Delete a file from the server.
   *
   * \param self client object
   * \param dsName the file to delete
   */
  result_t (*delete_file)(client_t *self, const char *fileName);
} client = { client__create,
             client__connect,
             client__disconnect,
             client__get_logical_devices,
             client__get_logical_nodes,
             client__get_data_objects,
             client__get_data_attributes,
             client__get_data_sets,
             client__get_data_set_members,
             client__get_reports,
             client__get_files,
             client__get_log_blocks,
             client__get_log,
             client__get_value,
             client__control_bool,
             client__control_int,
             client__control_uint,
             client__control_float,
             client__control_string,
             client__set_bool,
             client__set_int,
             client__set_uint,
             client__set_float,
             client__set_string,
             client__delete_data_set,
             client__delete_file };

#endif /* ACTIONS_H */
