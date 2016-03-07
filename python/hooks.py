import subprocess,os

version = subprocess.check_output(["configure_edison", "--version"])

def sendtophone(s):
    print("SENDING TO SMARTPHONE")
    lock = "/home/root/data/.LOCK"
    data = "/home/root/data/dataOut"

    # Create a lock file
    open(lock, "a").close()
    print("Created Lock File")

    # If it exists
    if os.path.isfile(data):
        
        with open(data, "r+") as f:
        
            for line in f:
                print("Line: ", line)
                s.send(line)

        try:
            print("Going to remove LOCK")
            os.remove(data)
            os.remove(lock)
        except OSError:
            pass
    else:

        # Nothing to read
        print("Nothing to read: NO FILE FOUND")
        s.send(0);
        if os.path.isfile(lock):
            os.remove(lock)
            

def init(s):

    print("Connected!")
    s.send("Data" if os.path.isfile("/home/root/data/dataOut") else "No Data")


def loop(s, data):
    print("Sending to Phone")
    sendtophone(s)


def close(s):
    print "Bye!"
