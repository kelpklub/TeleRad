import argparse
import sys

from esp32 import ESP32
from mount import Mount
from scanner import Scanner


DEFAULT_PORT = "/dev/ttyUSB0"

class DryRunESP32:
    def connect(self):
        print("[DRY RUN] Connected to simulated ESP32")

    def disconnect(self):
        print("[DRY RUN] Disconnected from simulated ESP32")

    def status(self):
        from esp32 import Status

        return Status(
            state="IDLE",
            azimuth=0,
            altitude=0,
            homed=True,
            enabled=True,
            limit_triggered=False
        )

    def position(self):
        from esp32 import Position

        return Position(
            azimuth=0,
            altitude=0
        )


class DryRunMount:
    def __init__(self):
        self.azimuth = 0.0
        self.altitude = 0.0

    def disconnect(self):
        print("[DRY RUN] Mount disconnected")

    def is_ready(self):
        return True

    def status(self):
        from esp32 import Status

        return Status(
            state="IDLE",
            azimuth=round(self.azimuth),
            altitude=round(self.altitude),
            homed=True,
            enabled=True,
            limit_triggered=False
        )

    def position(self):
        from esp32 import Position

        return Position(
            azimuth=round(self.azimuth),
            altitude=round(self.altitude)
        )

    def home(self):
        print("[DRY RUN] Homing")
        self.azimuth = 0.0
        self.altitude = 0.0

    def move_to_degrees(self, azimuth, altitude):
        print(
            f"[DRY RUN] MOVE "
            f"AZ={azimuth:.2f}° "
            f"ALT={altitude:.2f}°"
        )

        self.azimuth = azimuth
        self.altitude = altitude

    def move_relative_degrees(self, azimuth, altitude):
        self.azimuth += azimuth
        self.altitude += altitude

        print(
            f"[DRY RUN] REL "
            f"AZ={azimuth:.2f}° "
            f"ALT={altitude:.2f}°"
        )

    def park(self):
        print("[DRY RUN] Parking")
        self.azimuth = 0.0
        self.altitude = 0.0

    def stop(self):
        print("[DRY RUN] STOP")

    def emergency_stop(self):
        print("[DRY RUN] ESTOP")

    def clear_emergency_stop(self):
        print("[DRY RUN] CLEAR_ESTOP")


class DryRunSDR:
    def __init__(self, frequency=1420.405e6):
        self.frequency = frequency
        self.connected = False

    def connect(self):
        self.connected = True
        print("[DRY RUN] Connected to simulated SDR")

    def disconnect(self):
        self.connected = False
        print("[DRY RUN] Disconnected from simulated SDR")

    def is_connected(self):
        return self.connected

    def set_frequency(self, frequency):
        self.frequency = frequency

        print(
            f"[DRY RUN] Frequency: "
            f"{frequency / 1e6:.6f} MHz"
        )

    def measure_power(self):
        from measurement import SDRMeasurement

        return SDRMeasurement(
            frequency=self.frequency,
            power_dbfs=-42.0,
            sample_count=16384
        )

def create_parser():
    parser = argparse.ArgumentParser(
        prog="telerad",
        description="TeleRad radio telescope control CLI"
    )

    subparsers = parser.add_subparsers(
        dest="command",
        required=True
    )

    #--dry-run
    parser.add_argument(
    "--dry-run",
    action="store_true",
    help="Simulate hardware without connecting to the ESP32 or SDR"
)

    # Mount

    mount_parser = subparsers.add_parser(
        "mount",
        help="Control the antenna mount"
    )

    mount_subparsers = mount_parser.add_subparsers(
        dest="mount_command",
        required=True
    )

    mount_subparsers.add_parser(
        "status",
        help="Show mount status"
    )

    mount_subparsers.add_parser(
        "position",
        help="Show current mount position"
    )

    mount_subparsers.add_parser(
        "home",
        help="Home the mount"
    )

    move_parser = mount_subparsers.add_parser(
        "move",
        help="Move to an absolute position"
    )

    move_parser.add_argument(
        "azimuth",
        type=float
    )

    move_parser.add_argument(
        "altitude",
        type=float
    )

    rel_parser = mount_subparsers.add_parser(
        "rel",
        help="Move relative to the current position"
    )

    rel_parser.add_argument(
        "azimuth",
        type=float
    )

    rel_parser.add_argument(
        "altitude",
        type=float
    )

    mount_subparsers.add_parser(
        "park",
        help="Park the mount"
    )

    mount_subparsers.add_parser(
        "stop",
        help="Stop normal movement"
    )

    mount_subparsers.add_parser(
        "estop",
        help="Trigger emergency stop"
    )

    mount_subparsers.add_parser(
        "clear-estop",
        help="Clear emergency stop"
    )

    # SDR

    sdr_parser = subparsers.add_parser(
        "sdr",
        help="Control and inspect the SDR"
    )

    sdr_subparsers = sdr_parser.add_subparsers(
        dest="sdr_command",
        required=True
    )

    sdr_subparsers.add_parser(
        "measure",
        help="Measure received signal power"
    )

    frequency_parser = sdr_subparsers.add_parser(
        "frequency",
        help="Set SDR frequency"
    )

    frequency_parser.add_argument(
        "frequency",
        type=float,
        help="Frequency in Hz"
    )

    # Scan

    scan_parser = subparsers.add_parser(
        "scan",
        help="Run radio scans"
    )

    scan_subparsers = scan_parser.add_subparsers(
        dest="scan_command",
        required=True
    )

    scan_start_parser = scan_subparsers.add_parser(
        "start",
        help="Start a scan"
    )

    scan_start_parser.add_argument(
        "--az-start",
        type=float,
        required=True
    )

    scan_start_parser.add_argument(
        "--az-end",
        type=float,
        required=True
    )

    scan_start_parser.add_argument(
        "--alt-start",
        type=float,
        required=True
    )

    scan_start_parser.add_argument(
        "--alt-end",
        type=float,
        required=True
    )

    scan_start_parser.add_argument(
        "--step",
        type=float,
        required=True
    )

    return parser


def main():
    parser = create_parser()
    args = parser.parse_args()

    if args.dry_run:
        esp32 = DryRunESP32()
        mount = DryRunMount()
    else:
        esp32 = ESP32(DEFAULT_PORT)
        mount = Mount(esp32)

    try:
        esp32.connect()

        if args.command == "mount":
            handle_mount(args, mount)

        elif args.command == "sdr":
            handle_sdr(args, dry_run=args.dry_run)

        elif args.command == "scan":
            handle_scan(args, mount, dry_run=args.dry_run)

    except KeyboardInterrupt:
        print("\nInterrupted.")
        return 130

    except Exception as error:
        print(f"Error: {error}", file=sys.stderr)
        return 1

    finally:
        mount.disconnect()

    return 0


def handle_mount(args, mount):
    if args.mount_command == "status":
        status = mount.status()

        print(f"State: {status.state}")
        print(f"Azimuth: {status.azimuth}°")
        print(f"Altitude: {status.altitude}°")
        print(f"Homed: {'yes' if status.homed else 'no'}")
        print(f"Enabled: {'yes' if status.enabled else 'no'}")
        print(
            f"Limit: {'yes' if status.limit_triggered else 'no'}"
        )

    elif args.mount_command == "position":
        position = mount.position()

        print(f"Azimuth: {position.azimuth}°")
        print(f"Altitude: {position.altitude}°")

    elif args.mount_command == "home":
        print("Homing...")
        mount.home()
        print("Homing complete.")

    elif args.mount_command == "move":
        print(
            f"Moving to "
            f"azimuth {args.azimuth}°, "
            f"altitude {args.altitude}°..."
        )

        mount.move_to_degrees(
            args.azimuth,
            args.altitude
        )

        print("Movement complete.")

    elif args.mount_command == "rel":
        mount.move_relative_degrees(
            args.azimuth,
            args.altitude
        )

        print("Relative movement complete.")

    elif args.mount_command == "park":
        print("Parking...")
        mount.park()
        print("Park complete.")

    elif args.mount_command == "stop":
        mount.stop()
        print("Movement stopped.")

    elif args.mount_command == "estop":
        mount.emergency_stop()
        print("Emergency stop activated.")

    elif args.mount_command == "clear-estop":
        mount.clear_emergency_stop()
        print("Emergency stop cleared.")


def handle_sdr(args, dry_run=False):
    if dry_run:
        sdr = DryRunSDR()
    else:
        from sdr import SDR
        sdr = SDR(
            frequency=1420.405e6
        )

    try:
        sdr.connect()

        if args.sdr_command == "frequency":
            sdr.set_frequency(args.frequency)

            print(
                f"Frequency set to "
                f"{args.frequency / 1e6:.6f} MHz"
            )

        elif args.sdr_command == "measure":
            measurement = sdr.measure_power()

            print(
                f"Frequency: "
                f"{measurement.frequency / 1e6:.6f} MHz"
            )

            print(
                f"Power: "
                f"{measurement.power_dbfs:.2f} dBFS"
            )

            print(
                f"Samples: "
                f"{measurement.sample_count}"
            )

    finally:
        sdr.disconnect()


def handle_scan(args, mount, dry_run=False):
    if dry_run:
        sdr = DryRunSDR()
    else:
        from sdr import SDR
        sdr = SDR(
            frequency=1420.405e6
        )

    scanner = Scanner(
        mount=mount,
        sdr=sdr,
        az_start=args.az_start,
        az_end=args.az_end,
        alt_start=args.alt_start,
        alt_end=args.alt_end,
        step=args.step
    )

    results = scanner.scan()

    print(f"Scan complete: {len(results)} measurements")

    for result in results:
        print(
            f"AZ={result.azimuth:.2f}° "
            f"ALT={result.altitude:.2f}° "
            f"FREQ={result.measurement.frequency / 1e6:.6f} MHz "
            f"POWER={result.measurement.power_dbfs:.2f} dBFS"
        )


if __name__ == "__main__":
    sys.exit(main())