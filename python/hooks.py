import subprocess,os

version = subprocess.check_output(["configure_edison", "--version"])

def sendtophone(s):
    print("SENDING TO SMARTPHONE")
    lock = "/home/root/data/.LOCK"
    data = "/home/root/data/dataOut"

    open(lock, "a").close()
    print("Created Lock File")
    with open(data, "r+") as f:
        for line in f:
            print("Line: ", line)
            s.send(line)
            
        print("Going to truncate it...")
        f.truncate()

    try:
        print("Going to remove LOCK")
        os.remove(data)
        os.remove(lock)
    except OSError:
        pass

def init(s):

    print("Connected!")
    s.send(version)


def loop(s, data):
    print("Sending to Phone")
    sendtophone(s)


def close(s):
    print "Bye!"
