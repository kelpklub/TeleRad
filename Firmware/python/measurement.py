from dataclasses import dataclass


@dataclass
class SDRMeasurement:
    frequency: float
    power_dbfs: float
    sample_count: int