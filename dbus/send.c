#include <stdio.h>
#include <dbus/dbus.h>

int main(int argc, char **argv)
{
	DBusConnection *conn;
	DBusMessage *msg;
	DBusError err;
	
	dbus_error_init(&err);
	conn = dbus_bus_get(DBUS_BUS_SESSION, &err);
	if(conn == NULL)
	{
		printf("Failed to connect to the D-BUS daemon: %s\n", err.message);
		dbus_error_free(&err);
		return 1;
	}
	msg = dbus_message_new_signal("/org/share/linux", "org.share.linux", "Config");
	dbus_connection_send(conn, msg, NULL);
	dbus_message_unref(msg);
	msg = dbus_message_new_signal("/org/share/linux", "org.share.linux", "Quit");
	dbus_connection_send(conn, msg, NULL);
	dbus_message_unref(msg);
	return 0;
}
