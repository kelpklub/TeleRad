import math
import time

from dataclasses import dataclass
from rtlsdr import RtlSdr


from measurement import SDRMeasurement

class SDR:
    def __init__(
        self,
        frequency,
        sample_rate=2.4e6,
        gain="auto",
        settle_time=0.05
    ):
        self.frequency = frequency
        self.sample_rate = sample_rate
        self.gain = gain
        self.settle_time = settle_time

        self.sdr = None
    def connect(self):
        if self.sdr is not None:
            return

        self.sdr = RtlSdr()

        self.sdr.sample_rate = self.sample_rate
        self.sdr.center_freq = self.frequency
        self.sdr.gain = self.gain

    def disconnect(self):
        if self.sdr is not None:
            self.sdr.close()
            self.sdr = None

    def is_connected(self):
        return self.sdr is not None

    def set_frequency(self, frequency):
        self._require_connected()

        self.frequency = frequency
        self.sdr.center_freq = frequency

        time.sleep(self.settle_time)

        self.sdr.read_samples(16 * 1024)

    def set_sample_rate(self, sample_rate):
        self._require_connected()

        self.sample_rate = sample_rate
        self.sdr.sample_rate = sample_rate

    def set_gain(self, gain):
        self._require_connected()

        self.gain = gain
        self.sdr.gain = gain

    def read_samples(self, count=256 * 1024):
        self._require_connected()

        return self.sdr.read_samples(count)

    def measure_power(self, count=256 * 1024):
        samples = self.read_samples(count)

        if len(samples) == 0:
            raise RuntimeError("No samples received from SDR")

        power = sum(
            abs(sample) ** 2
            for sample in samples
        ) / len(samples)

        if power <= 0:
            power_dbfs = float("-inf")
        else:
            power_dbfs = 10 * math.log10(power)

        return SDRMeasurement(
            frequency=self.frequency,
            power_dbfs=power_dbfs,
            sample_count=len(samples)
        )

    def _require_connected(self):
        if self.sdr is None:
            raise RuntimeError("SDR is not connected")
        