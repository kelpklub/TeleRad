import serial
import time

from dataclasses import dataclass

@dataclass
class Position:
    azimuth: int
    altitude: int


@dataclass
class Status:
    state: str
    azimuth: int
    altitude: int
    homed: bool
    enabled: bool
    limit_triggered: bool

class ESP32:
    def __init__(self, port, baudrate=115200, timeout=1.0):
        self.port = port
        self.baudrate = baudrate
        self.timeout = timeout
        self.serial = None

    def connect(self):
        if self.serial is not None and self.serial.is_open:
            return

        self.serial = serial.Serial(
            self.port,
            self.baudrate,
            timeout=self.timeout
        )

        time.sleep(2)

        self.serial.reset_input_buffer()
        self.serial.reset_output_buffer()

    def disconnect(self):
        if self.serial is not None and self.serial.is_open:
            self.serial.close()

    def is_connected(self):
        return (
            self.serial is not None
            and self.serial.is_open
        )

    def send(self, command):
        if not self.is_connected():
            raise RuntimeError("ESP32 is not connected")

        message = command.strip() + "\n"

        self.serial.write(
            message.encode("ascii")
        )

        self.serial.flush()

    def position(self):
        response = self.command("POSITION")

        if response is None:
            raise TimeoutError("No response from ESP32")

        if response.startswith("ERROR"):
            raise RuntimeError(response)

        try:
            parts = response.split()

            azimuth = int(parts[0].split("=", 1)[1])
            altitude = int(parts[1].split("=", 1)[1])

        except (IndexError, ValueError):
            raise RuntimeError(
                f"Invalid POSITION response: {response}"
            )

        return Position(
            azimuth=azimuth,
            altitude=altitude
        )


    def status(self):
        response = self.command("STATUS")

        if response is None:
            raise TimeoutError("No response from ESP32")

        if response.startswith("ERROR"):
            raise RuntimeError(response)

        try:
            values = {}

            for part in response.split():
                key, value = part.split("=", 1)
                values[key] = value

            return Status(
                state=values["STATE"],
                azimuth=int(values["AZ"]),
                altitude=int(values["ALT"]),
                homed=values["HOMED"] == "1",
                enabled=values["ENABLED"] == "1",
                limit_triggered=values["LIMIT"] == "1"
            )

        except (KeyError, ValueError):
            raise RuntimeError(
                f"Invalid STATUS response: {response}"
            )

    def read_line(self, timeout=None):
        if not self.is_connected():
            raise RuntimeError("ESP32 is not connected")

        old_timeout = self.serial.timeout

        if timeout is not None:
            self.serial.timeout = timeout

        try:
            line = self.serial.readline()
        finally:
            self.serial.timeout = old_timeout

        if not line:
            return None

        return line.decode(
            "utf-8",
            errors="replace"
        ).strip()

    def command(self, command):
        self.send(command)
        return self.read_line()

    def wait_for(self, expected, timeout=60):
        start_time = time.monotonic()

        while time.monotonic() - start_time < timeout:
            line = self.read_line(timeout=0.5)

            if line is None:
                continue

            if line == expected:
                return line

            if line.startswith("ERROR"):
                raise RuntimeError(line)

        raise TimeoutError(
            f"Timed out waiting for: {expected}"
        )

    def ping(self):
        return self.command("PING")

    def stop(self):
        return self.command("STOP")

    def emergency_stop(self):
        return self.command("ESTOP")

    def clear_emergency_stop(self):
        return self.command("CLEAR_ESTOP")

    def enable(self):
        return self.command("ENABLE")

    def disable(self):
        return self.command("DISABLE")

    def home(self):
        return self.command("HOME")

    def move_to(self, azimuth, altitude):
        return self.command(
            f"MOVE {azimuth} {altitude}"
        )

    def move_relative(self, azimuth, altitude):
        return self.command(
            f"REL {azimuth} {altitude}"
        )

    def park(self):
        return self.command("PARK")

    def set_speed(self, speed):
        return self.command(
            f"SPEED {speed}"
        )

    def set_acceleration(self, acceleration):
        return self.command(
            f"ACCEL {acceleration}"
        )

    def save_config(self):
        return self.command("SAVE")

    def load_config(self):
        return self.command("LOAD")

    def reset_config(self):
        return self.command("RESET_CONFIG")


if __name__ == "__main__":
    esp32 = ESP32("/dev/ttyUSB0")

    try:
        esp32.connect()

        print("Connected to ESP32")
        print("PING:", esp32.ping())
        print("STATUS:", esp32.status())
        print("POSITION:", esp32.position())

    except serial.SerialException as error:
        print("Serial error:", error)

    except RuntimeError as error:
        print("Runtime error:", error)

    finally:
        esp32.disconnect()