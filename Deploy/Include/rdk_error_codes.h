#ifndef RDK_ERROR_CODES_H
#define RDK_ERROR_CODES_H

#ifndef RDK_ERROR_CODES
#define RDK_ERROR_CODES

#define RDK_UNHANDLED_EXCEPTION 0x01000000 // �� ������� �������� ���������� � ���
#define RDK_EXCEPTION_CATCHED 0x01000001 // ��������� ���������� � ���������� � ��� ���� �������� � ���
#define RDK_SUCCESS 0
#define RDK_NOT_IMPLEMENTED 1

/// Common errors
#define RDK_E_NULL_POINTER_IN_ARGUMENT 1000

/// Core errors
#define RDK_E_CORE_INIT_FAIL 100
#define RDK_E_CORE_INCORRECT_CHANNELS_NUMBER 101
#define RDK_E_CORE_CHANNEL_NOT_FOUND 102
#define RDK_E_CORE_ZERO_CHANNEL_MUST_EXIST 103
#define RDK_E_CORE_ENGINE_CREATE_FAIL 104
#define RDK_E_CORE_STORAGE_CREATE_FAIL 105
#define RDK_E_CORE_ENVIRONMENT_CREATE_FAIL 106
#define RDK_E_CORE_ENGINE_INIT_FAIL 107
#define RDK_E_CORE_ENVIRONMENT_UNINIT_FAIL 108
#define RDK_E_CORE_LOCK_FAIL 110
#define RDK_E_CORE_UNLOCK_FAIL 111

/// Storage errors
#define RDK_E_STORAGE_LOAD_COMMON_CLASSES_DESCR_FAIL 200
#define RDK_E_STORAGE_DEL_COLLECTION_FAIL 201
#define RDK_E_STORAGE_COLLECTION_NOT_FOUND 202
#define RDK_E_STORAGE_ADD_COLLECTION_FAIL 203
#define RDK_E_STORAGE_BUILD_FAIL 204
#define RDK_E_STORAGE_RT_COLLECTION_CREATION_FAIL 250
#define RDK_E_STORAGE_CLASS_NOT_FOUND 251
#define RDK_E_STORAGE_TAKE_OBJECT_FAIL 252

/// Environment errors
#define RDK_E_ENV_SET_FLAG_FAIL 300
#define RDK_E_ENV_NOT_FOUND 301
#define RDK_E_ENV_SOURCE_CONTROLLER_CALL_FAIL 302
#define RDK_E_ENV_SET_TIME_FAIL 303

/// Model errors
#define RDK_E_MODEL_COMPONENT_NOT_FOUND 400
#define RDK_E_MODEL_NOT_FOUND 401
#define RDK_E_MODEL_TARGET_COMPONENT_NOT_FOUND 402
#define RDK_E_MODEL_COMPONENT_OWNER_NOT_FOUND 403
#define RDK_E_MODEL_COMPONENTS_DONT_HAVE_TO_MATCH 404
#define RDK_E_MODEL_MOVE_COMPONENTS_FAIL 405
#define RDK_E_MODEL_CHANGE_COMPONENT_POSITION_FAIL 406
#define RDK_E_MODEL_NOT_ALL_PROPERTIES_SET 407
#define RDK_E_MODEL_READ_PROPERTY_MEMORY_FAIL 408
#define RDK_E_MODEL_CREATE_LINK_FAIL 409
#define RDK_E_MODEL_BREAK_LINK_FAIL 410
#define RDK_E_MODEL_CREARE_INTERNAL_LINKS_FAIL 411
#define RDK_E_MODEL_LOAD_COMPONENT_FAIL 412
#define RDK_E_MODEL_LOAD_COMPONENT_PROPERTIES_FAIL 413
#define RDK_E_MODEL_BITMAP_OUTPUT_NOT_FOUND 414
#define RDK_E_MODEL_IMAGES_MUST_HAVE_SAME_SIZE 415
#define RDK_E_MODEL_PROPERTY_NOT_FOUND 416
#define RDK_E_MODEL_PROPERTY_INDEX_OUT_OF_RANGE 417
#define RDK_E_MODEL_SWITCH_LINK_FAIL 418
#define RDK_E_MODEL_ADD_COMPONENT_FAIL 419

/// Engine errors
#define RDK_E_ENGINE_SET_PREDEFINED_STRUCTURE_FAIL 500
#define RDK_E_ENGINE_CREATE_STRUCTURE_FAIL 501
#define RDK_E_ENGINE_DESTROY_STRUCTURE_FAIL 501

/// XML Parser erros
#define RDK_E_XML_TARGET_NODE_NOT_FOUND 1000

/// Logger errors
#define RDK_E_LOGGER_CANT_CREATE_LOG_PATH 2000
#define RDK_E_LOGGER_CANT_CREATE_LOG_FILE 2001
#define RDK_E_LOGGER_LOG_FILE_WRITE_ERROR 2002
#define RDK_E_LOGGER_LOG_FILE_NOT_OPEN 2003
#define RDK_E_LOGGER_NOT_FOUND 2004
#define RDK_E_LOGGER_SET_EVENTS_LOG_MODE_FAIL 2005
#define RDK_E_LOGGER_SET_FLAG_FAIL 2006
#define RDK_E_LOGGER_SET_EXCEPTION_HANDLER_FAIL 304
#define RDK_E_LOGGER_SET_DEBUGGER_FLAG_FAIL 2007
#endif

#endif

