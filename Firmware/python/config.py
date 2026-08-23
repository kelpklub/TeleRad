AZ_STEPS_PER_DEGREE = 53.333
ALT_STEPS_PER_DEGREE = 106.667

AZ_MIN_DEG = 0.0
AZ_MAX_DEG = 359.0

ALT_MIN_DEG = 0.0
ALT_MAX_DEG = 90.0


def azimuth_degrees_to_steps(degrees):
    return round(degrees * AZ_STEPS_PER_DEGREE)


def altitude_degrees_to_steps(degrees):
    return round(degrees * ALT_STEPS_PER_DEGREE)


def azimuth_steps_to_degrees(steps):
    return steps / AZ_STEPS_PER_DEGREE


def altitude_steps_to_degrees(steps):
    return steps / ALT_STEPS_PER_DEGREE


SCAN_FREQUENCIES = [
    1420.405e6,
]