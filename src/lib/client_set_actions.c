#include "iec61850_client/client.h"

bool set_object(client_t *self, const char *daName, const char *fcString, MmsValue *mmsVal)
{
  bool success = false;
  da_access_t *da = create_data_attribute(daName, fcString);

  if (mmsVal && da) {
    IedConnection_writeObject(self->con, &self->error, da->name, da->fc, mmsVal);
    success = self->error == IED_ERROR_OK;
  }

  if (da) {
    destroy_data_attribute(da);
  }

  return success;
}

result_t client__set_bool(client_t *self, const char *daName, bool setVal, const char *fcString)
{
  result_t res = result_create();
  MmsValue *mmsVal = MmsValue_newBoolean(setVal);
  flogf(self->log, "setting: %s -> %s\n", daName, setVal ? "true" : "false");

  if (mmsVal) {
    if (set_object(self, daName, fcString, mmsVal)) {
      res.code = SUCCESS;
      result_writef(&res, "Set succeeded: %s -> %s\n", daName, setVal ? "true" : "false");
      flogf(self->log, "set succeeded\n");
    } else {
      res.code = LIBIEC61850_ERROR;
      flogf(self->log, "set failed: %s\n", error_to_string(self->error));
    }
  } else {
    flogf(self->log, "set failed: unable to parse '%s' as MmsValue\n", setVal ? "true" : "false");
  }

  if (mmsVal) {
    MmsValue_delete(mmsVal);
  }

  return res;
}

result_t client__set_int(client_t *self, const char *daName, int setVal, const char *fcString)
{
  result_t res = result_create();
  MmsValue *mmsVal = MmsValue_newInteger(setVal);
  flogf(self->log, "setting: %s -> %i\n", daName, setVal);

  if (mmsVal) {
    if (set_object(self, daName, fcString, mmsVal)) {
      res.code = SUCCESS;
      result_writef(&res, "Set succeeded: %s -> %i\n", daName, setVal);
      flogf(self->log, "set succeeded\n");
    } else {
      res.code = LIBIEC61850_ERROR;
      flogf(self->log, "set failed: %s\n", error_to_string(self->error));
    }
  } else {
    flogf(self->log, "set failed: unable to parse '%i' as MmsValue\n", setVal);
  }

  if (mmsVal) {
    MmsValue_delete(mmsVal);
  }

  return res;
}

result_t client__set_uint(client_t *self, const char *daName, int setVal, const char *fcString)
{
  result_t res = result_create();
  MmsValue *mmsVal = MmsValue_newUnsigned(setVal);
  flogf(self->log, "setting: %s -> %i\n", daName, setVal);

  if (mmsVal) {
    if (set_object(self, daName, fcString, mmsVal)) {
      res.code = SUCCESS;
      result_writef(&res, "Set succeeded: %s -> %i\n", daName, setVal);
      flogf(self->log, "set succeeded\n");
    } else {
      res.code = LIBIEC61850_ERROR;
      flogf(self->log, "set failed: %s\n", error_to_string(self->error));
    }
  } else {
    flogf(self->log, "set failed: unable to parse '%i' as MmsValue\n", setVal);
  }

  if (mmsVal) {
    MmsValue_delete(mmsVal);
  }

  return res;
}

result_t client__set_float(client_t *self, const char *daName, float setVal, const char *fcString)
{
  result_t res = result_create();
  MmsValue *mmsVal = MmsValue_newFloat(setVal);
  flogf(self->log, "setting: %s -> %f\n", daName, setVal);

  if (mmsVal) {
    if (set_object(self, daName, fcString, mmsVal)) {
      res.code = SUCCESS;
      result_writef(&res, "Set succeeded: %s -> %f\n", daName, setVal);
      flogf(self->log, "set succeeded\n");
    } else {
      res.code = LIBIEC61850_ERROR;
      flogf(self->log, "set failed: %s\n", error_to_string(self->error));
    }
  } else {
    flogf(self->log, "set failed: unable to parse '%fi' as MmsValue\n", setVal);
  }

  if (mmsVal) {
    MmsValue_delete(mmsVal);
  }

  return res;
}

result_t client__set_string(client_t *self,
                            const char *daName,
                            const char *setVal,
                            const char *fcString)
{
  result_t res = result_create();
  MmsValue *mmsVal = MmsValue_newVisibleString(setVal);
  flogf(self->log, "setting: %s -> %s\n", daName, setVal);

  if (mmsVal) {
    if (set_object(self, daName, fcString, mmsVal)) {
      res.code = SUCCESS;
      result_writef(&res, "Set succeeded: %s -> %s\n", daName, setVal);
      flogf(self->log, "set succeeded\n");
    } else {
      res.code = LIBIEC61850_ERROR;
      flogf(self->log, "set failed: %s\n", error_to_string(self->error));
    }
  } else {
    flogf(self->log, "set failed: unable to parse '%s' as MmsValue\n", setVal);
  }

  if (mmsVal) {
    MmsValue_delete(mmsVal);
  }

  return res;
}