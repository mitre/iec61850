#include "iec61850_client/client.h"

result_t client__delete_data_set(client_t *self, const char *dsName)
{
  result_t res = result_create();
  flogf(self->log, "deleting '%s'\n", dsName);
  if (IedConnection_deleteDataSet(self->con, &self->error, dsName)) {
    res.code = SUCCESS;
    result_writef(&res, "Delete '%s' succeeded\n", dsName);
    flogf(self->log, "delete succeeded\n", dsName);
  } else {
    res.code = LIBIEC61850_ERROR;
    flogf(self->log, "delete failed: %s\n", error_to_string(self->error));
  }

  return res;
}

result_t client__delete_file(client_t *self, const char *fileName)
{
  result_t res = result_create();
  flogf(self->log, "deleting '%s'\n", fileName);
  IedConnection_deleteFile(self->con, &self->error, fileName);
  if (self->error == IED_ERROR_OK) {
    res.code = SUCCESS;
    result_writef(&res, "Delete '%s' succeeded\n", fileName);
    flogf(self->log, "delete succeeded\n", fileName);
  } else {
    res.code = LIBIEC61850_ERROR;
    flogf(self->log, "delete failed: %s\n", error_to_string(self->error));
  }

  return res;
}