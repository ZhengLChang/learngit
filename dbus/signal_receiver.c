#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dbus/dbus-glib.h>
#include <dbus/dbus.h>
#include <unistd.h>

void listen_signal()
{
	DBusMessage *msg;
	DBusMessageIter arg;
	DBusConnection *connection;
	DBusError err;
	char *sigvalue;

	dbus_error_init(&err);
	connection = dbus_bus_get(DBUS_BUS_SESSION, &err);
	if(dbus_error_is_set(&err))
	{
		fprintf(stderr, "ConnectionError %s\n", err.message);
		dbus_error_free(&err);
	}
	if(connection == NULL)
	{
		return ;
	}
	dbus_bus_add_match(connection, "type='signal',interface='test.signal.Type'", &err);	
	dbus_connection_flush(connection);
	if(dbus_error_is_set(&err))
	{
		fprintf(stderr, "Match Error %s\n", err.message);
		dbus_error_free(&err);
	}
	while(1)
	{
		dbus_connection_read_write(connection, 0);
		printf("******after dbus_connection_read_write********\n");
		msg = dbus_connection_pop_message(connection);
		if(msg == NULL)
		{
			sleep(1);
			continue;
		}
		if(dbus_message_is_signal(msg, "test.signal.Type", "Test"))
		{
			if(!dbus_message_iter_init(msg, &arg))
			{
				fprintf(stderr, "Message no Param\n");
			}
			else if(dbus_message_iter_get_arg_type(&arg) != DBUS_TYPE_STRING)
			{
				fprintf(stderr, "Param is not string\n");
			}
			else
			{
				dbus_message_iter_get_basic(&arg, &sigvalue);
			}
			printf("Got Signal with value: %s\n", sigvalue);
		}
		dbus_message_unref(msg);
	}
	return ;
}
int main(int argc, char **argv)
{
	listen_signal();
	return 0;
}















