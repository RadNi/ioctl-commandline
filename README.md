# ioctl-commandline
A program for sending IOCTL request with command line


with this program you can send your custom IOCTL command to the specefic file.

For supporting changing capability befor sending IOCTL command set the CAP_SETFCAP capability.

(In make file command was provided)


## Development setup

Program debendencies:

libcap-dev

```sh
sudo apt-get install libcap-dev
sudo make all
```

