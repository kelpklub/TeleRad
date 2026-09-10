from dataclasses import dataclass
import csv
from pathlib import Path

from config import (
    AZ_MIN_DEG,
    AZ_MAX_DEG,
    ALT_MIN_DEG,
    ALT_MAX_DEG,
    SCAN_FREQUENCIES,
)

from typing import TYPE_CHECKING

if TYPE_CHECKING:
    from measurement import SDRMeasurement


@dataclass
class ScanResult:
    azimuth: float
    altitude: float
    measurement: "SDRMeasurement"


class Scanner:
    def __init__(
        self,
        mount,
        sdr,
        az_start,
        az_end,
        alt_start,
        alt_end,
        step
    ):
        if step <= 0:
            raise ValueError("Step must be greater than zero")

        if az_start > az_end:
            raise ValueError(
                "Azimuth start must be less than or equal to azimuth end"
            )

        if alt_start > alt_end:
            raise ValueError(
                "Altitude start must be less than or equal to altitude end"
            )

        if not AZ_MIN_DEG <= az_start <= AZ_MAX_DEG:
            raise ValueError("Azimuth start is outside mount limits")

        if not AZ_MIN_DEG <= az_end <= AZ_MAX_DEG:
            raise ValueError("Azimuth end is outside mount limits")

        if not ALT_MIN_DEG <= alt_start <= ALT_MAX_DEG:
            raise ValueError("Altitude start is outside mount limits")

        if not ALT_MIN_DEG <= alt_end <= ALT_MAX_DEG:
            raise ValueError("Altitude end is outside mount limits")

        self.mount = mount
        self.sdr = sdr

        self.az_start = az_start
        self.az_end = az_end
        self.alt_start = alt_start
        self.alt_end = alt_end
        self.step = step

        self.running = False
        self.results = []
        self.progress_callback = None

    def start(self):
        if self.running:
            raise RuntimeError("Scanner is already running")

        if not self.mount.is_ready():
            raise RuntimeError("Mount is not ready")

        if not self.sdr.is_connected():
            self.sdr.connect()

        self.running = True

    def stop(self):
        self.running = False
        self.mount.stop()

    def is_running(self):
        return self.running

    def scan(self):
        self.start()
        self.results = []

        try:
            positions = list(self.positions())
            total_positions = len(positions)
            completed_positions = 0

            for azimuth, altitude in positions:
                if not self.running:
                    break

                self.mount.move_to_degrees(
                    azimuth,
                    altitude
                )

                for frequency in SCAN_FREQUENCIES:
                    self.sdr.set_frequency(frequency)

                    measurement = self.sdr.measure_power()

                    result = ScanResult(
                        azimuth=azimuth,
                        altitude=altitude,
                        measurement=measurement
                    )

                    self.results.append(result)

                completed_positions += 1

                if self.progress_callback is not None:
                    self.progress_callback(
                        completed_positions,
                        total_positions,
                        azimuth,
                        altitude,
                    )

        finally:
            self.running = False
            self.sdr.disconnect()

        return self.results

    def positions(self):
        altitudes = self._generate_range(
            self.alt_start,
            self.alt_end,
            self.step
        )

        left_to_right = True

        for altitude in altitudes:
            row = self._generate_range(
                self.az_start,
                self.az_end,
                self.step
            )

            if left_to_right:
                for azimuth in row:
                    yield azimuth, altitude
            else:
                for azimuth in reversed(row):
                    yield azimuth, altitude

            left_to_right = not left_to_right

    @staticmethod
    def _generate_range(start, end, step):
        positions = []
        position = start

        while position <= end:
            positions.append(position)
            position += step

        # Always scan the requested rectangle boundary, even when the
        # requested step does not divide the range evenly.
        if not positions or positions[-1] != end:
            positions.append(end)

        return positions

    def set_progress_callback(self, callback):
        """Set a callback: callback(completed, total, azimuth, altitude)."""
        self.progress_callback = callback

    def save_csv(self, filename):
        """Save all collected measurements to a CSV file."""
        if not self.results:
            raise ValueError("No scan results to save")

        path = Path(filename)
        path.parent.mkdir(parents=True, exist_ok=True)

        with path.open("w", newline="", encoding="utf-8") as file:
            writer = csv.writer(file)
            writer.writerow([
                "azimuth",
                "altitude",
                "frequency",
                "measurement",
            ])

            for result in self.results:
                measurement = result.measurement

                # SDRMeasurement used by TeleRad exposes power_dbfs,
                # frequency, and sample_count.
                value = getattr(measurement, "power_dbfs", measurement)
                frequency = getattr(measurement, "frequency", "")
                sample_count = getattr(measurement, "sample_count", "")
                writer.writerow([
                    result.azimuth,
                    result.altitude,
                    frequency,
                    value,
                    sample_count,
                ])

        return path


def print_scan_progress(completed, total, azimuth, altitude):
    """Print scan progress for a terminal UI."""
    percent = (completed / total) * 100 if total else 100.0
    print(
        f"Scan: {completed}/{total} ({percent:5.1f}%) "
        f"AZ={azimuth:.2f}° ALT={altitude:.2f}°",
        flush=True,
    )
