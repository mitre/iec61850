#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "iec61850_client/client.h"
#include "iec61850_client/utils.h"
#include "version.h"
#include <argtable3.h>

#ifdef _WIN32
#ifndef strcasecmp
#define strcasecmp _stricmp
#endif
#endif

#ifndef APP_H
#define APP_H

void get_logical_devices(const char *hostname, int port);
void get_logical_nodes(const char *hostname, int port, const char *ldName);
void get_data_objects(const char *hostname, int port, const char *lnName);
void get_data_attributes(const char *hostname, int port, const char *doName);
void get_value(const char *hostname, int port, const char *daName, const char *fcString);
void get_reports(const char *hostname, int port);
void get_files(const char *hostname, int port);
void get_log_blocks(const char *hostname, int port);
void get_log(const char *hostname, int port, const char *lcbName);
void get_data_sets(const char *hostname, int port);
void get_data_set_members(const char *hostname, int port, const char *dsName, bool readVals);
void get_logs(const char *hostname,
              int port,
              const char *doName,
              uint64_t startTime,
              uint64_t endTime);

void control_bool(const char *hostname,
                  int port,
                  const char *doName,
                  const char *ctlVal,
                  bool sbo,
                  bool sboWithValue,
                  bool toggle);
void control_int(const char *hostname,
                 int port,
                 const char *doName,
                 int ctlVal,
                 bool sbo,
                 bool sboWithValue);
void control_uint(const char *hostname,
                  int port,
                  const char *doName,
                  int ctlVal,
                  bool sbo,
                  bool sboWithValue);
void control_float(const char *hostname,
                   int port,
                   const char *doName,
                   float ctlVal,
                   bool sbo,
                   bool sboWithValue);
void control_string(const char *hostname,
                    int port,
                    const char *doName,
                    const char *ctlVal,
                    bool sbo,
                    bool sboWithValue);

void set_bool(const char *hostname,
              int port,
              const char *doName,
              const char *setVal,
              const char *fcString,
              bool read);
void set_int(const char *hostname,
             int port,
             const char *doName,
             int setVal,
             const char *fcString,
             bool read);
void set_uint(const char *hostname,
              int port,
              const char *doName,
              int setVal,
              const char *fcString,
              bool read);
void set_float(const char *hostname,
               int port,
               const char *doName,
               float setVal,
               const char *fcString,
               bool read);
void set_string(const char *hostname,
                int port,
                const char *doName,
                const char *setVal,
                const char *fcString,
                bool read);

void delete_data_set(const char *hostname, int port, const char *dsName);
void delete_file(const char *hostname, int port, const char *fileName);

#endif /* APP_H */