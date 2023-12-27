#include "iec61850_client/utils.h"

void flogf(FILE *fp, const char *fmt, ...)
{
  va_list argp;
  time_t t;
  char datestr[51];

  fp = (fp == NULL) ? stderr : fp;

  t = time(NULL);
  tzset();
  strftime(datestr, sizeof(datestr) - 1, "%d-%b-%Y %H:%M:%S", localtime(&t));
  fprintf(fp, "%s - ", datestr);

  va_start(argp, fmt);
  vfprintf(fp, fmt, argp);
  va_end(argp);
}

bool parse_bool(const char *strVal, bool *val)
{
  bool success = false;
  if ((strcasecmp(strVal, "true") == 0) || (strcasecmp(strVal, "t") == 0) ||
      (strcasecmp(strVal, "on") == 0) || (strcasecmp(strVal, "1") == 0)) {
    *val = true;
    success = true;
  } else if ((strcasecmp(strVal, "false") == 0) || (strcasecmp(strVal, "f") == 0) ||
             (strcasecmp(strVal, "off") == 0) || (strcasecmp(strVal, "0") == 0)) {
    *val = false;
    success = true;
  }

  return success;
}

const char *error_to_string(IedClientError err)
{

  if (err == IED_ERROR_OK)
    return "success";

  else if (err == IED_ERROR_NOT_CONNECTED)
    return "the service request can not be executed because the client is not yet connected";

  else if (err == IED_ERROR_ALREADY_CONNECTED)
    return "connect service cannot execute because the client is already connected";

  else if (err == IED_ERROR_CONNECTION_LOST)
    return "connection lost during service request";

  else if (err == IED_ERROR_SERVICE_NOT_SUPPORTED)
    return "the service or some given parameters are not supported by the client stack or by the "
           "server";

  else if (err == IED_ERROR_CONNECTION_REJECTED)
    return "connection rejected by server";

  else if (err == IED_ERROR_OUTSTANDING_CALL_LIMIT_REACHED)
    return "cannot send request because outstanding call limit is reached";

  /* client side errors */

  else if (err == IED_ERROR_USER_PROVIDED_INVALID_ARGUMENT)
    return "API function has been called with an invalid argument";

  else if (err == IED_ERROR_ENABLE_REPORT_FAILED_DATASET_MISMATCH)
    return "enable report failed due to dataset mismatch";

  else if (err == IED_ERROR_OBJECT_REFERENCE_INVALID)
    return "the provided object reference is invalid (there is a syntactical error)";

  else if (err == IED_ERROR_UNEXPECTED_VALUE_RECEIVED)
    return "received object is of unexpected type";

  /* service error - error reported by server */

  else if (err == IED_ERROR_TIMEOUT)
    return "communication to the server failed with a timeout";

  else if (err == IED_ERROR_ACCESS_DENIED)
    return "access denied";

  else if (err == IED_ERROR_OBJECT_DOES_NOT_EXIST)
    return "object does not exist";

  else if (err == IED_ERROR_OBJECT_EXISTS)
    return "object already exists";

  else if (err == IED_ERROR_OBJECT_ACCESS_UNSUPPORTED)
    return "the server does not support the requested access method";

  else if (err == IED_ERROR_TYPE_INCONSISTENT)
    return "the server expected an object of another type";

  else if (err == IED_ERROR_TEMPORARILY_UNAVAILABLE)
    return "the object or service is temporarily unavailable";

  else if (err == IED_ERROR_OBJECT_UNDEFINED)
    return "the specified object is not defined in the server";

  else if (err == IED_ERROR_INVALID_ADDRESS)
    return "the specified address is invalid";

  else if (err == IED_ERROR_HARDWARE_FAULT)
    return "service failed due to a hardware fault";

  else if (err == IED_ERROR_TYPE_UNSUPPORTED)
    return "the requested data type is not supported by the server";

  else if (err == IED_ERROR_OBJECT_ATTRIBUTE_INCONSISTENT)
    return "the provided attributes are inconsistent";

  else if (err == IED_ERROR_OBJECT_VALUE_INVALID)
    return "the provided object value is invalid";

  else if (err == IED_ERROR_OBJECT_INVALIDATED)
    return "the object is invalidated";

  else if (err == IED_ERROR_MALFORMED_MESSAGE)
    return "received an invalid response message from the server";

  else if (err == IED_ERROR_SERVICE_NOT_IMPLEMENTED)
    return "service not implemented";

  else
    return "unknown error";
}

FunctionalConstraint fc_from_string(const char *fcString)
{
  FunctionalConstraint fc = IEC61850_FC_NONE;

  if (fcString == NULL || (strcmp(fcString, "") == 0)) {
    return IEC61850_FC_NONE;
  }

  if ((strcmp(fcString, "all") == 0) || (strcmp(fcString, "ALL") == 0)) {
    fc = IEC61850_FC_ALL;
  } else if (strlen(fcString) == 2) {
    /* all FCs are 2 characters */
    char _fcString[3];
    int i;

    for (i = 0; (fcString[i] != '\0') && (i < 2); i++) {
      if ((fcString[i] >= 'a') && (fcString[i] <= 'z'))
        _fcString[i] = fcString[i] - 32;
      else
        _fcString[i] = fcString[i];
    }
    _fcString[i] = '\0';

    /* prevent library function indexing out of bounds */
    if (i > 1)
      fc = FunctionalConstraint_fromString(_fcString);
  }

  return fc;
}

char *parse_mms_ref(const char *mmsRef, FunctionalConstraint *fc)
{
  if ((mmsRef == NULL) || (strcmp(mmsRef, "") == 0)) {
    return NULL;
  }

  char *daName = NULL;
  char *fcStr = NULL;

  const char *firstSep = strchr(mmsRef, '$');
  const char *secondSep = firstSep ? strchr(firstSep + 1, '$') : NULL;
  if (firstSep && secondSep) {
    size_t nameLen = firstSep - mmsRef + 1;
    size_t fcLen = secondSep - firstSep - 1;

    if (fcLen > 0) {
      fcStr = malloc(fcLen + 1);
      assert(fcStr && "Failed to allocate memory");
      strncpy(fcStr, firstSep + 1, fcLen);
      fcStr[fcLen] = '\0';
    }

    if (fc != NULL) {
      *fc = fc_from_string(fcStr);
    }

    if (nameLen > 1) {
      daName = malloc(strlen(mmsRef) + 1);
      assert(daName && "Failed to allocate memory");
      strncpy(daName, mmsRef, nameLen);
      strcpy(daName + nameLen, secondSep + 1);

      // Replace all '$' characters in mmsRef with '.' characters
      for (char *p = daName; *p; ++p) {
        if (*p == '$') {
          *p = '.';
        }
      }
    }
  }

  if (fcStr != NULL) {
    free(fcStr);
  }

  return daName;
}

char *parse_da_name(const char *daName, FunctionalConstraint *fc)
{
  if ((daName == NULL) || (strcmp(daName, "") == 0)) {
    return NULL;
  }

  char *newName = NULL;
  char *fcStr = NULL;
  size_t nameLen = strlen(daName);

  const char *firstSep = strchr(daName, '[');
  const char *secondSep = firstSep ? strchr(firstSep + 1, ']') : NULL;
  if (firstSep && secondSep) {
    nameLen = firstSep - daName;
    size_t fcLen = secondSep - firstSep - 1;

    if (fcLen > 0) {
      fcStr = malloc(fcLen + 1);
      assert(fcStr && "Failed to allocate memory");
      strncpy(fcStr, firstSep + 1, fcLen);
      fcStr[fcLen] = '\0';
    }

    if (fc != NULL) {
      *fc = fc_from_string(fcStr);
    }
  } else if (firstSep) {
    nameLen = firstSep - daName;
  }

  if (nameLen > 0) {
    newName = malloc(nameLen + 1);
    assert(newName && "Failed to allocate memory");
    strncpy(newName, daName, nameLen);
    newName[nameLen] = '\0';
  }

  if (fcStr != NULL) {
    free(fcStr);
  }

  return newName;
}

da_access_t *create_data_attribute(const char *daName, const char *fcString)
{
  if ((daName == NULL) || (strcmp(daName, "") == 0)) {
    return NULL;
  }

  FunctionalConstraint fc = fc_from_string(fcString);

  char *parsedName = NULL;
  if (strchr(daName, '$') != NULL) {
    parsedName = parse_mms_ref(daName, &fc);
  } else {
    parsedName = parse_da_name(daName, &fc);
  }

  if ((parsedName == NULL) || (strcmp(parsedName, "") == 0)) {
    return NULL;
  }

  if (fc == IEC61850_FC_NONE) {
    return NULL;
  }

  da_access_t *da;
  da = (da_access_t *)malloc(sizeof(*da));
  assert(da && "Failed to allocate memory.");

  da->name = parsedName;
  da->fc = fc;
  return da;
}

void destroy_data_attribute(da_access_t *da)
{
  if (da != NULL) {
    if (da->name != NULL) {
      free(da->name);
    }
    free(da);
  }
}

MmsType MmsType_from_string(const char *datatype)
{
  MmsType dt = MMS_DATA_ACCESS_ERROR;

  if ((strcasecmp(datatype, "bool") == 0) || (strcasecmp(datatype, "boolean") == 0)) {
    dt = MMS_BOOLEAN;
  } else if (strcasecmp(datatype, "float") == 0) {
    dt = MMS_FLOAT;
  } else if ((strcasecmp(datatype, "str") == 0) || (strcasecmp(datatype, "string") == 0)) {
    dt = MMS_VISIBLE_STRING;
  } else if (strcasecmp(datatype, "bitstring") == 0) {
    dt = MMS_BIT_STRING;
  } else if (strcasecmp(datatype, "octet_string") == 0) {
    dt = MMS_OCTET_STRING;
  } else if (strcasecmp(datatype, "unicode_string") == 0) {
    dt = MMS_STRING;
  } else if ((strcasecmp(datatype, "int") == 0) || (strcasecmp(datatype, "integer") == 0)) {
    dt = MMS_INTEGER;
  } else if ((strcasecmp(datatype, "uint") == 0) || (strcasecmp(datatype, "unsigned") == 0)) {
    dt = MMS_UNSIGNED;
  } else if ((strcasecmp(datatype, "struct") == 0) || (strcasecmp(datatype, "structure") == 0)) {
    dt = MMS_STRUCTURE;
  } else if (strcasecmp(datatype, "array") == 0) {
    dt = MMS_ARRAY;
  } else if (strcasecmp(datatype, "time") == 0) {
    dt = MMS_UTC_TIME;
  } else if (strcasecmp(datatype, "generalized_time") == 0) {
    dt = MMS_GENERALIZED_TIME;
  } else if (strcasecmp(datatype, "binary_time") == 0) {
    dt = MMS_BINARY_TIME;
  } else if (strcasecmp(datatype, "bcd") == 0) {
    dt = MMS_BCD;
  } else if (strcasecmp(datatype, "obj_id") == 0) {
    dt = MMS_OBJ_ID;
  }

  return dt;
}

bool starts_with(const char *str, const char *prefix)
{
  return strncmp(prefix, str, strlen(prefix)) == 0;
}

void sleep_ms(int milliseconds)
{
#ifdef _WIN32
  Sleep(milliseconds);
#elif _POSIX_C_SOURCE >= 199309L
  struct timespec ts;
  ts.tv_sec = milliseconds / 1000;
  ts.tv_nsec = (milliseconds % 1000) * 1000000;
  nanosleep(&ts, NULL);
#else
  usleep(milliseconds * 1000);
#endif
}

char *create_qualified_name(const char *parent, char sep, const char *child)
{
  if ((parent == NULL) || (child == NULL) || (sep == '\0')) {
    return NULL;
  }

  int n = strlen(parent) + strlen(child) + 2; // +1 for separator, +1 for null terminator
  char *formatted_string = malloc(n);
  if (!formatted_string) {
    return NULL;
  }
  
  int written = sprintf(formatted_string, "%s%c%s", parent, sep, child);
  if (written < 0 || written >= n) {
    free(formatted_string);
    return NULL;
  }
  return formatted_string;
}

result_t result_create()
{
  result_t result;
  result.code = UNKNOWN_ERROR;
  result.message = strdup("");
  if (result.message == NULL) {
    result.code = ALLOCATION_FAILURE;
  }
  return result;
}

void result_destroy(result_t *result)
{
  free(result->message);
  result->message = NULL;
}

void result_writef(result_t *result, const char *format, ...)
{
  va_list args;
  va_start(args, format);

  // Determine the length of the new message
  va_list tmp_args;
  va_copy(tmp_args, args);
  int len = vsnprintf(NULL, 0, format, tmp_args);
  va_end(tmp_args);
  if (len < 0) {
    result->code = ALLOCATION_FAILURE;
    return;
  }

  // Allocate memory for the new message
  char *new_message = malloc(len + 1);
  if (new_message == NULL) {
    result->code = ALLOCATION_FAILURE;
    return;
  }

  // Write the formatted output to the new message
  vsnprintf(new_message, len + 1, format, args);

  // Append the new message to the existing message
  size_t new_length = strlen(result->message) + len + 1;
  char *new_block = realloc(result->message, new_length);
  if (new_block == NULL) {
    result->code = ALLOCATION_FAILURE;
    free(new_message);
    return;
  }
  result->message = new_block;
  strcat(result->message, new_message);

  if (new_message) {
    free(new_message);
  }
  va_end(args);
}