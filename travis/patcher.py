import sys

with open(sys.argv[1], 'r+b') as f:
	s = f.read()
	offset=s.find(b'\x50\x45\x00\x00')+0x4a
	f.seek(offset)
	f.write(b'\x00')