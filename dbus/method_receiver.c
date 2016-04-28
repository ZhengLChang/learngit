#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dbus/dbus-glib.h>
#include <dbus/dbus.h>
#include <unistd.h>
#include <stdbool.h>

int main(int argc, char **argv)
{
	DBusMessage *msg, *reply;
	DBusMessageIter iter;
	DBusConnection *conn;
	DBusError err;
	int ret;
	bool keepgoing = true;
//	char str[] = "Thanks";
	bool stat = true;
	dbus_uint32_t serial = 0;
	dbus_uint32_t level = 21614;
	dbus_error_init(&err);
	conn = dbus_bus_get(DBUS_BUS_SESSION, &err);
	if(dbus_error_is_set(&err))
	{
		printf("Connection Error %s\n", err.message);
		dbus_error_free(&err);
	}
	if(conn == NULL)
	{
		return -1;
	}
	ret = dbus_bus_request_name(conn, "test.wei.dest",
			DBUS_NAME_FLAG_REPLACE_EXISTING, &err);
	if(dbus_error_is_set(&err))
	{
		printf("Name Error %s\n", err.message);
		dbus_error_free(&err);
	}
	if(ret != DBUS_REQUEST_NAME_REPLY_PRIMARY_OWNER)
		return -1;
	while(keepgoing)
	{
		dbus_connection_read_write(conn, 0);
		msg = dbus_connection_pop_message(conn);
		if(msg == NULL)
		{
			sleep(1);
			continue;
		}
		if(dbus_message_is_method_call(msg, 
				"test.method.Type", "Method"))
		{
			int current_type;
			if(!dbus_message_iter_init(msg, &iter))
			{
				return -1;
			}

			while((current_type = dbus_message_iter_get_arg_type(&iter)) != DBUS_TYPE_INVALID)
			{
				switch(current_type)
				{
					case DBUS_TYPE_INT32:
					{
						int value;
						dbus_message_iter_get_basic(&iter, &value);
						printf("get int32: %d\n", value);
						break;
					}
					case DBUS_TYPE_STRING:
					{
						char *value;
						dbus_message_iter_get_basic(&iter, &value);
						printf("get str: %s\n", value);
						break;
					}
					case DBUS_TYPE_UINT32:
					{
						break;
					}
					case DBUS_TYPE_DOUBLE:
					{
						break;
					}
					default:
					{
						break;
					}
				}
				dbus_message_iter_next(&iter);
			}
//			dbus_message_iter_get_basic(&iter, &str);
			reply = dbus_message_new_method_return(msg);
			dbus_message_iter_init_append(reply, &iter);
			if(!dbus_message_iter_append_basic(&iter, DBUS_TYPE_BOOLEAN, &stat))
			{
				printf("Out of Memory\n");
				return -1;
			}
			if(!dbus_message_iter_append_basic(&iter, DBUS_TYPE_UINT32, &level))
			{
				printf("Out of Memory\n");
				return -1;
			}
			if(!dbus_connection_send(conn, reply, &serial))
			{
				printf("Out of Memory\n");
				return -1;
			}
			dbus_connection_flush(conn);
			dbus_message_unref(reply);
			dbus_message_unref(msg);
			return 0;
		}
	}
	dbus_message_unref(msg);
	return 0;
}
















