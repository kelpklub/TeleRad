from esp32 import ESP32
from config import (
    AZ_MIN_DEG,
    AZ_MAX_DEG,
    ALT_MIN_DEG,
    ALT_MAX_DEG,
    azimuth_degrees_to_steps,
    altitude_degrees_to_steps,
    azimuth_steps_to_degrees,
    altitude_steps_to_degrees,
)


class Mount:
    def __init__(self, esp32):
        self.esp32 = esp32

        self.homed = False
        self.moving = False
        self.emergency_stopped = False

    def connect(self):
        self.esp32.connect()

    def disconnect(self):
        self.esp32.disconnect()

    def ping(self):
        return self.esp32.ping()

    def home(self, timeout=60):
        self._require_connected()

        response = self.esp32.home()

        if response != "OK":
            raise RuntimeError(response)

        self.moving = True

        try:
            self.esp32.wait_for("HOMED", timeout)
        except Exception:
            self.homed = False
            self.moving = False
            raise

        self.homed = True
        self.moving = False
        self.emergency_stopped = False

    def move_to(self, azimuth, altitude, timeout=60):
        self._require_ready()

        response = self.esp32.move_to(
            azimuth,
            altitude
        )

        if response != "OK":
            raise RuntimeError(response)

        self._wait_for_motion(timeout)

    def move_relative(self, azimuth, altitude, timeout=60):
        self._require_ready()

        response = self.esp32.move_relative(
            azimuth,
            altitude
        )

        if response != "OK":
            raise RuntimeError(response)

        self._wait_for_motion(timeout)

    def move_to_degrees(self, azimuth, altitude, timeout=60):
        self._validate_coordinates(
            azimuth,
            altitude
        )

        azimuth_steps = azimuth_degrees_to_steps(azimuth)
        altitude_steps = altitude_degrees_to_steps(altitude)

        self.move_to(
            azimuth_steps,
            altitude_steps,
            timeout
        )

    def move_relative_degrees(
        self,
        azimuth,
        altitude,
        timeout=60
    ):
        azimuth_steps = azimuth_degrees_to_steps(azimuth)
        altitude_steps = altitude_degrees_to_steps(altitude)

        self.move_relative(
            azimuth_steps,
            altitude_steps,
            timeout
        )

    def park(self, timeout=60):
        self._require_ready()

        response = self.esp32.park()

        if response != "OK":
            raise RuntimeError(response)

        self._wait_for_motion(timeout)

    def stop(self):
        self._require_connected()

        response = self.esp32.stop()

        if response != "OK":
            raise RuntimeError(response)

        self.moving = False

    def emergency_stop(self):
        self._require_connected()

        response = self.esp32.emergency_stop()

        if response != "OK":
            raise RuntimeError(response)

        self.moving = False
        self.homed = False
        self.emergency_stopped = True

    def clear_emergency_stop(self):
        self._require_connected()

        response = self.esp32.clear_emergency_stop()

        if response != "OK":
            raise RuntimeError(response)

        self.emergency_stopped = False
        self.homed = False

    def enable(self):
        self._require_connected()

        response = self.esp32.enable()

        if response != "OK":
            raise RuntimeError(response)

    def disable(self):
        self._require_connected()

        response = self.esp32.disable()

        if response != "OK":
            raise RuntimeError(response)

    def get_position(self):
        self._require_connected()

        return self.esp32.position()

    def get_position_degrees(self):
        position = self.get_position()

        return {
            "azimuth": azimuth_steps_to_degrees(
                position.azimuth
            ),
            "altitude": altitude_steps_to_degrees(
                position.altitude
            ),
        }

    def get_status(self):
        self._require_connected()

        return self.esp32.status()

    def set_speed(self, speed):
        self._require_connected()

        response = self.esp32.set_speed(speed)

        if response != "OK":
            raise RuntimeError(response)

    def set_acceleration(self, acceleration):
        self._require_connected()

        response = self.esp32.set_acceleration(acceleration)

        if response != "OK":
            raise RuntimeError(response)

    def save_config(self):
        self._require_connected()

        response = self.esp32.save_config()

        if response != "OK":
            raise RuntimeError(response)

    def load_config(self):
        self._require_connected()

        response = self.esp32.load_config()

        if response != "OK":
            raise RuntimeError(response)

        self.homed = False

    def reset_config(self):
        self._require_connected()

        response = self.esp32.reset_config()

        if response != "OK":
            raise RuntimeError(response)

    def _wait_for_motion(self, timeout):
        self.moving = True

        try:
            self.esp32.wait_for("DONE", timeout)
        except Exception:
            self.moving = False
            raise

        self.moving = False

    def _require_connected(self):
        if not self.esp32.is_connected():
            raise RuntimeError(
                "ESP32 is not connected"
            )

    def _require_ready(self):
        self._require_connected()

        if self.emergency_stopped:
            raise RuntimeError(
                "Emergency stop is active"
            )

        if not self.homed:
            raise RuntimeError(
                "Mount is not homed"
            )

    def _validate_coordinates(self, azimuth, altitude):
        if not AZ_MIN_DEG <= azimuth <= AZ_MAX_DEG:
            raise ValueError(
                f"Azimuth must be between "
                f"{AZ_MIN_DEG} and {AZ_MAX_DEG} degrees"
            )

        if not ALT_MIN_DEG <= altitude <= ALT_MAX_DEG:
            raise ValueError(
                f"Altitude must be between "
                f"{ALT_MIN_DEG} and {ALT_MAX_DEG} degrees"
            )

    def is_ready(self):
        return (
            self.esp32.is_connected()
            and self.homed
            and not self.emergency_stopped
        )