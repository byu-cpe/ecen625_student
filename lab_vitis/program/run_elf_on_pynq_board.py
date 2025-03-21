# This script programs a bit file and elf file onto the PYNQ board
# It then reads the serial output from the board

import argparse
import queue
import sys
import os
import subprocess
import threading
import pathlib

import requests
import serial

THIS_DIR = pathlib.Path(__file__).resolve().parent
RUN_ELF_PATH = THIS_DIR / "run_elf.tcl"
XSCT_BIN = "/tools/Xilinx/Vitis/2024.2/bin/xsct"

# LABS_DIR = pathlib.Path(__file__).resolve().parent


class TermColors:
    """Terminal codes for printing in color"""

    PURPLE = "\033[95m"
    BLUE = "\033[94m"
    GREEN = "\033[92m"
    YELLOW = "\033[93m"
    RED = "\033[91m"
    END = "\033[0m"
    BOLD = "\033[1m"
    UNDERLINE = "\033[4m"


def print_color(color, *msg):
    """Print a message in color"""
    print(color + " ".join(str(item) for item in msg), TermColors.END)


def error(*msg, returncode=-1):
    """Print an error message and exit program"""
    print_color(TermColors.RED, "ERROR:", *msg)
    sys.exit(returncode)


def read_serial_to_queue(ser, q, stop_event):
    while not stop_event.is_set():
        try:
            line = ser.readline().decode("utf-8", errors="ignore")
            if line:
                q.put(line)
        except Exception as e:
            q.put(f"[Serial read error: {e}]\n")
            break


def main():
    """Program an elf file onto the PYNQ board"""

    parser = argparse.ArgumentParser()
    parser.add_argument("bit", type=pathlib.Path, help="The bit file to program")
    parser.add_argument("xsa", type=pathlib.Path, help="The xsa file to program")
    parser.add_argument("elf", type=pathlib.Path, help="The elf file to run")
    parser.add_argument("jtag_serial")
    parser.add_argument(
        "--no-notify", action="store_true", help="Do not notify the server"
    )
    args = parser.parse_args()

    notify_server = not args.no_notify

    print_color(TermColors.GREEN, "ELF path:".ljust(20), args.elf)
    print_color(TermColors.GREEN, "BIT path:".ljust(20), args.bit)
    print_color(TermColors.GREEN, "XSA path:".ljust(20), args.xsa)
    ps7_init_path = args.elf.parent.parent / "_ide" / "psinit" / "ps7_init.tcl"
    print_color(TermColors.GREEN, "PS7 init path:".ljust(20), ps7_init_path)
    serial_file_path = pathlib.Path(
        f"/dev/serial/by-id/usb-Xilinx_TUL_{args.jtag_serial}-if01-port0"
    )
    print_color(TermColors.GREEN, "Serial path:".ljust(20), serial_file_path)
    assert args.elf.is_file(), f"File {args.elf} does not exist"
    assert args.bit.is_file(), f"File {args.bit} does not exist"
    assert args.xsa.is_file(), f"File {args.xsa} does not exist"
    assert ps7_init_path.is_file(), f"File {ps7_init_path} does not exist"
    assert serial_file_path.is_char_device(), f"File {serial_file_path} does not exist"

    # Execute programming and print output
    my_env = os.environ.copy()
    my_env["BIT_FILE"] = args.bit
    my_env["XSA_FILE"] = args.xsa
    my_env["ELF_FILE"] = args.elf
    my_env["PS7_INIT_FILE"] = ps7_init_path
    my_env["JTAG_SERIAL"] = args.jtag_serial + "A"
    cmd = [XSCT_BIN, str(RUN_ELF_PATH)]

    # Open serial BEFORE programming
    ser = serial.Serial(str(serial_file_path), 115200, timeout=1)
    serial_buffer = queue.Queue()
    stop_event = threading.Event()

    reader_thread = threading.Thread(
        target=read_serial_to_queue, args=(ser, serial_buffer, stop_event), daemon=True
    )
    reader_thread.start()

    # Notify the server of board usage
    if notify_server:
        url = "http://127.0.0.1:5000/use"
        response = requests.post(
            url, json={"device": args.jtag_serial, "user": os.getlogin()}, timeout=5
        )
        if response.ok:
            print_color(TermColors.YELLOW, "Marked device as used.")
        else:
            print("Error:", response.json())
            assert False, "Failed to contact server and mark device as used"

    # Run programming
    print_color(TermColors.PURPLE, "\nProgramming elf file:", args.elf)
    print(" ".join(cmd))
    subprocess.run(cmd, env=my_env, check=True)

    # Stop buffering and start live output
    print_color(TermColors.PURPLE, "\nReading serial output. Press Ctrl+C to stop.")

    # Drain the buffer
    while not serial_buffer.empty():
        print(serial_buffer.get(), end="")

    try:
        while True:
            if not serial_buffer.empty():
                print(serial_buffer.get(), end="")
    except KeyboardInterrupt:
        print_color(TermColors.YELLOW, "\nStopping serial read.")
        stop_event.set()
        ser.close()


if __name__ == "__main__":
    main()
