#include <assert.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <iec61850_client.h>
#include <iec61850_common.h>
#include <mms_common.h>

#ifdef _WIN32
#include <windows.h>
#ifndef strcasecmp
#define strcasecmp _stricmp
#endif
#else
#include <unistd.h>
#endif

#ifndef UTILS_H
#define UTILS_H

typedef enum
{
  SUCCESS,
  LIBIEC61850_ERROR,
  ALLOCATION_FAILURE,
  IEC61850_ACTIONS_ERROR,
  UNKNOWN_ERROR,
} errorcode_t;

typedef struct
{
  errorcode_t code;
  char *message;
} result_t;

typedef struct daAccess
{
  char *name;
  FunctionalConstraint fc;
} da_access_t;

/**
 * \brief Simple logging utility.
 *
 * \param fp file pointer to log to
 * \param fmt printf style format string
 */
void flogf(FILE *fp, const char *fmt, ...);

/**
 * \brief Parse a string into a boolean value.
 *
 * Accepts on/off, true/false, 0/1. Case insensitive.
 *
 * \param strVal string value to parse
 * \param val boolean value to hold the parsed result
 * \return boolean value indicating whether the operation
 * was successful
 */
bool parse_bool(const char *strVal, bool *val);

/**
 * \brief Convert a libiec61850 error value to a human-readable string.
 *
 * \param err IedClientError enum value
 * \return error string
 */
const char *error_to_string(IedClientError err);

/**
 * \brief Strip the functional constraint from a data attribute name.
 *
 * Some functions return the data attribute with its functional
 * constraint appended in square brackets. This function removes
 * the functional constraint (if present), returning just the attribute
 * name.
 *
 * \param daName the string containing the data attribute name
 * \param fc out parameter to store the functional constraint
 * \return data attribute name without the functional constraint
 */
char *parse_da_name(const char *daName, FunctionalConstraint *fc);

/**
 * \brief Parse an MMS style reference in to libiec61850 style.
 *
 * \param mmsRef string MMS style data reference
 * \param fc out parameter to store the functional constraint
 * \return the data attribute name as a string
 */
char *parse_mms_ref(const char *mmsRef, FunctionalConstraint *fc);

/**
 * \brief Parse a string into a Functional Constraint value.
 *
 * First checks if the string is 'all'. If not, strips the
 * first two characters from the string and uses the libiec61850
 * function FunctionalConstraint_fromString to convert to a
 * FunctionalConstraint enum value.
 *
 * \param fcString functional constraint as a string
 * \return FunctionalConstraint enum value
 */
FunctionalConstraint fc_from_string(const char *fcString);

/**
 * \brief Create a struct containing the data attribute name and its functional constraint.
 *
 * Parses a string containing the data attribute name and its functional constraint
 * into separate components, stored in a struct. Optionally, the functional constraint
 * can be passed as a separate string. If the FC is sent in both in the data attribute
 * name and as a separate string, the FC in the name will be preferred.
 *
 * \param daName the string containing the data attribute name
 * \param fcString the functional constraint as a string, can be omitted if
 * daName contains the functional constraint
 * \return da_access_t struct
 */
da_access_t *create_data_attribute(const char *daName, const char *fcString);

/**
 * \brief Free resources held by a da_access_t struct.
 *
 * \param da the struct to be freed
 */
void destroy_data_attribute(da_access_t *da);

/**
 * \brief Convert human-readable string to a MmsType enum value.
 *
 * Accepts the official MMS datatype names and some abbreviated
 * names and returns the corresponding libiec61850 MmsType enum
 * value.
 *
 * \param datatype the datatype string
 * \return MmsType enum value
 */
MmsType MmsType_from_string(const char *datatype);

/**
 * \brief Convert MmsType enum value to a human-readable string.
 *
 * Translates the libiec61850 MmsType enum value to the official
 * MMS datatype name as a string.
 *
 * \param type the datatype as a MmsType enum value
 * \param buffer memory location to write the result to
 */
void MmsType_to_string(MmsType type, char *buffer);

/**
 * \brief Utility to determine if a string starts with a prefix.
 *
 * Case-sensitive comparison of a string and a prefix.
 *
 * \param str the full string
 * \param prefix prefix to check
 * \return boolean indicating if the string starts with the prefix
 */
bool starts_with(const char *str, const char *prefix);

/**
 * \brief Sleep function.
 *
 * Utility to call OS-specific sleep function.
 *
 * \param milliseconds time to sleep in integer milliseconds
 */
void sleep_ms(int milliseconds);

/**
 * \brief Concat utility for qualified variable names.
 * 
 * Concatenates the "parent" string to the "child" string
 * with a separator character in-between.
 *
 * \param parent the name of the parent object
 * \param sep character to separate the two strings
 * \param child the name of the child object
 * \return the names concatenated as a string according to `format`
 */
char *create_qualified_name(const char *parent, char sep, const char *child);

/**
 * \brief Constructor for result_t struct.
 *
 * \return result_t struct
 */
result_t result_create();

/**
 * \brief Destructor for result_t struct.
 *
 * \param result pointer to the struct to destroy
 */
void result_destroy(result_t *result);

/**
 * \brief Write to the message of a result_t struct.
 *
 * \param result the struct to write to
 * \param format printf style format string
 */
void result_writef(result_t *result, const char *format, ...);

#endif /* UTILS_H */