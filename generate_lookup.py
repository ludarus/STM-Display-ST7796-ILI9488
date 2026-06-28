ON = 0xA
OFF = 0x0

print("const uint32_t pixel_lut[256] = {")

for i in range(256):
    result = 0
    for shift in (0, 2, 4, 6):
        lo = ON if (i >> (shift)) & 1 else OFF
        hi = ON if (i >> (shift + 1)) & 1 else OFF
        result = (result >> 8) | (((lo << 4) | hi) << 24)

    comma = "," if i < 255 else " "
    print(f"    0x{result:08X}{comma}")

print("};")
