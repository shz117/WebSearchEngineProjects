from struct import unpack

def decode(compressed_list):
    delta_list = []
    compressed_list = unpack('%dB' % len(compressed_list), compressed_list)
    try:
        it = iter(compressed_list)
        while True:
            current = 0
            shift = 0
            byte = it.next()
            while byte >= 128:
                current = current + ((byte & 127) << shift)
                shift = shift + 7
                byte = it.next()
            current = current + (byte << shift)
            delta_list.append(current)
    except StopIteration:
        pass
    return delta_list