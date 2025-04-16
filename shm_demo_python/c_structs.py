import ctypes


class SharedNNImage(ctypes.Structure):
    _fields_ = [
        ("is_processed", ctypes.c_int),
        ("width", ctypes.c_int),
        ("height", ctypes.c_int),
        ("scale", ctypes.c_double),
        ("data", ctypes.c_uint8 * (1920 * 1080 * 3))
    ]
