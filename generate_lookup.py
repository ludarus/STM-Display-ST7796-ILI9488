print("const uint8_t table[256][3] = " + "{")

# iterating through all possible permutations of a three byte packet for 3bpp
for i in range(256):
    value = 0

    for bit in range(7, -1, -1):
        value <<= 3
        if i & (1 << bit):
            value |= 0b111

    b0 = (value >> 16) & 0xFF
    b1 = (value >> 8) & 0xFF
    b2 = value & 0xFF

    print(f"    {{{b0:#04x}, {b1:#04x}, {b2:#04x}}},")

print("};")
