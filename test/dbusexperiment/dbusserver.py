
import subprocess
import tempfile


tcpconfig = """
 <!DOCTYPE busconfig PUBLIC "-//freedesktop//DTD D-BUS Bus
 Configuration 1.0//EN"
 "http://www.freedesktop.org/standards/dbus/1.0/busconfig.dtd">
 <busconfig>
   <type>session</type>
   <listen>tcp:host=localhost,port=0</listen>
  <policy context="default">
    <!-- Allow everything to be sent -->
    <allow send_destination="*" eavesdrop="true"/>
    <!-- Allow everything to be received -->
    <allow eavesdrop="true"/>
    <!-- Allow anyone to own anything -->
    <allow own="*"/>
  </policy>
 </busconfig>
"""

dbus_daemon = "dbus-daemon"

class LocalDBUSDaemon(object):

    def __init__(self, config):

        self.configfid = tempfile.NamedTemporaryFile()
        self.configfid.write(config)
        self.configfid.flush()

    def run(self):
        args = [dbus_daemon, "--nofork", "--print-address",
                "--config-file=%s" % self.configfid.name]
        self.process = subprocess.Popen(args, stdout=subprocess.PIPE)

        #(stdoutdata, stderrdata) = self.process.communicate(None)
        self.address = self.process.stdout.readline()[:-1]
        
    def close(self):
        self.configfid.close()
        self.process.terminate()
    

if __name__ == "__main__":
    ld = LocalDBUSDaemon(tcpconfig)
    ld.run()

    print ld.address
    ld.close()
    
