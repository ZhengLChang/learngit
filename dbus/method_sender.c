#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dbus/dbus-glib.h>
#include <dbus/dbus.h>
#include <unistd.h>
#include <stdbool.h>

int main(int argc, char **argv)
{
	DBusError err;
	DBusMessage *msg;
	DBusMessageIter arg;
	DBusConnection *conn;
	DBusPendingCall *pending;
	bool stat;
	dbus_uint32_t level;
	char *value = "hello, DBus";
	int ret;

	dbus_error_init(&err);
	conn = dbus_bus_get(DBUS_BUS_SESSION, &err);
	if(dbus_error_is_set(&err))
	{
		printf("Connection Err: %s\n", err.message);
		dbus_error_free(&err);
	}
	if(conn == NULL)
	{
		return -1;
	}
	ret = dbus_bus_request_name(conn, "test.wei.source",
			DBUS_NAME_FLAG_REPLACE_EXISTING, &err);
	if(dbus_error_is_set(&err))
	{
		printf("Name Err: %s\n", err.message);
		dbus_error_free(&err);
	}
	if(ret != DBUS_REQUEST_NAME_REPLY_PRIMARY_OWNER)
	{
		return -1;
	}
	msg = dbus_message_new_method_call("test.wei.dest", 
			"/test/method/Object",
			"test.method.Type",
			"Method");
	if(msg == NULL)
	{
		printf("Message is NULL\n");
		return -1;
	}
	dbus_message_iter_init_append(msg, &arg);
	if(!dbus_message_iter_append_basic(&arg, DBUS_TYPE_STRING, &value))
	{
		printf("Out of Memory!\n");
		return -1;
	}
	if(!dbus_connection_send_with_reply(conn, msg, &pending, -1))
	{
		printf("Out of Memory\n");
		return -1;
	}
	if(pending == NULL)
	{
		printf("Pending Call NULL: connection is disconnected\n");
		dbus_message_unref(msg);
		return -1;
	}
	dbus_connection_flush(conn);
	dbus_message_unref(msg);
	dbus_pending_call_block(pending);
	msg = dbus_pending_call_steal_reply(pending);
	if(msg == NULL)
	{
		printf("Reply NULL\n");
		return -1;
	}
	dbus_pending_call_unref(pending);
	if(!dbus_message_iter_init(msg, &arg))
	{
		printf("Message has no arguments!\n");
	}
	else if(dbus_message_iter_get_arg_type(&arg) != DBUS_TYPE_BOOLEAN)
	{
		printf("Argument is not boolean!\n");
	}
	else
	{
		dbus_message_iter_get_basic(&arg, &stat);
	}
	if(!dbus_message_iter_next(&arg))
		printf("Message has too few arguments!\n");
	else if(dbus_message_iter_get_arg_type(&arg) != DBUS_TYPE_UINT32)
	{
		printf("Argument is not int!\n");
	}
	else	
		dbus_message_iter_get_basic(&arg, &level);
	printf("Got Reply: %d, %d\n", stat, level);
	dbus_message_unref(msg);
	return 0;
}





























