#include <stdio.h>
#include <dbus/dbus.h>
//#include <dbus/dbus-glib.h>
#include <unistd.h>

int main(int argc, char **argv)
{
	DBusConnection *connection;
	DBusMessage *msg;
	DBusError error;
	int isLoop = 1;
	dbus_error_init(&error);
	connection = dbus_bus_get(DBUS_BUS_SESSION, &error);
	if(dbus_error_is_set(&error))
	{
		printf("Error connecting to the daemon bus %s\n", error.message);
		dbus_error_free(&error);
		return 1;
	}
	dbus_bus_add_match(connection, "type='signal',interface='org.share.linux'",
			NULL);
	while(isLoop)
	{
		dbus_connection_read_write(connection, 0);
		msg = dbus_connection_pop_message(connection); 
		if(NULL == msg)
		{
			sleep(1);
			continue;
		}
		if(dbus_message_is_signal(msg, "org.share.linux", "Quit"))
		{
			printf("Message quit received\n");
			isLoop = 0;
		}
		else if(dbus_message_is_signal(msg, "org.share.linux", "Config"))
		{
			printf("Message cutomize received\n");
		}
	}
	return 0;
}


















