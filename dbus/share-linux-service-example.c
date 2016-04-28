#include <stdio.h>
#include <dbus/dbus.h>

int main(int argc, char **argv)
{
	DBusConnection *conn;
	DBusError error;

	dbus_error_init(&error);
	conn = dbus_bus_get(DBUS_BUS_STARTER, &error);

	return 0;
}
