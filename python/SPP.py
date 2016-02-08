#!/usr/bin/python
from __future__ import absolute_import, print_function, unicode_literals

import hooks

from optparse import OptionParser, make_option
import os
import sys
import socket
import uuid
import dbus
import dbus.service
import dbus.mainloop.glib

try:
    from gi.repository import GObject
except ImportError:
    import gobject as GObject


class Profile(dbus.service.Object):
    fd = -1

    @dbus.service.method("org.bluez.Profile1",
                         in_signature="", out_signature="")
    def Release(self):
        print("Release")
        mainloop.quit()

    @dbus.service.method("org.bluez.Profile1",
                         in_signature="", out_signature="")
    def Cancel(self):
        print("Cancel")

    @dbus.service.method("org.bluez.Profile1",
                         in_signature="oha{sv}", out_signature="")
    def NewConnection(self, path, fd, properties):
        self.fd = fd.take()
        server_sock = socket.fromfd(self.fd, socket.AF_UNIX, socket.SOCK_STREAM)
        server_sock.setblocking(1)

        # Sends Version
        #print(server_sock.recv())
        hooks.init(server_sock)

        try:

            # Wait for string to get the data
            print("Reading from socket")
            data = server_sock.recv(1024)
            print("Read: %s" % data)
            print("Going into loop:")
            hooks.loop(server_sock, data)
            print("Have come out of hooks.loop")

        except IOError:
            pass

        hooks.close(server_sock)
        server_sock.close()

    @dbus.service.method("org.bluez.Profile1",
                         in_signature="o", out_signature="")
    def RequestDisconnection(self, path):
        print("RequestDisconnection(%s)" % (path))

        if (self.fd > 0):
            os.close(self.fd)
            self.fd = -1

        print("RequestDisconnection")

if __name__ == "__main__":
    dbus.mainloop.glib.DBusGMainLoop(set_as_default=True)

    bus = dbus.SystemBus()

    manager = dbus.Interface(bus.get_object("org.bluez",
                                            "/org/bluez"), "org.bluez.ProfileManager1")

    option_list = [
        make_option("-C", "--channel", action="store",
                    type="int", dest="channel",
                    default=None),
    ]

    parser = OptionParser(option_list=option_list)

    (options, args) = parser.parse_args()

    options.uuid = "1101"
    options.psm = "3"
    options.role = "server"
    options.name = "Edison SPP Loopback"
    options.service = "spp char loopback"
    options.path = "/foo/bar/profile"
    options.auto_connect = True
    options.record = ""

    profile = Profile(bus, options.path)

    mainloop = GObject.MainLoop()

    opts = {
        "AutoConnect": options.auto_connect,
    }

    if (options.name):
        opts["Name"] = options.name

    if (options.role):
        opts["Role"] = options.role

    if (options.psm is not None):
        opts["PSM"] = dbus.UInt16(options.psm)

    if (options.channel is not None):
        opts["Channel"] = dbus.UInt16(options.channel)

    if (options.record):
        opts["ServiceRecord"] = options.record

    if (options.service):
        opts["Service"] = options.service

    if not options.uuid:
        options.uuid = str(uuid.uuid4())

    manager.RegisterProfile(options.path, options.uuid, opts)

    mainloop.run()
