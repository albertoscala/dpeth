# dpeth — DisplayPort over Ethernet

`dpeth` is a custom protocol and tool suite for transmitting DisplayPort video frames over a standard Ethernet cable. It is designed for scenarios where a conventional DisplayPort video source is unavailable or impractical — allowing a machine to receive and display video frames sourced and transported entirely over the network.

## How It Works

The protocol works with two endpoints:

- **`source`** — captures or generates DisplayPort frames and transmits them as UDP/TCP packets over Ethernet.
- **`display`** — receives the incoming frame data from the network and reconstructs the video output for display.

The shared core (`dpeth.c`) handles frame packetization, sequencing, and the protocol logic that sits between the transport layer (UDP/TCP) and the DisplayPort frame format.

## Use Case

Standard DisplayPort requires a direct physical video source. `dpeth` is useful when:

- The video source is remote and no direct DP connection is possible.
- You want to route display output through an existing Ethernet infrastructure.
- You need to decouple the video source machine from the display machine over a local network.

## Project Structure

```
dpeth/
├── include/        # Header files (protocol definitions, structs)
├── src/
│   ├── dpeth.c     # Core protocol logic (framing, packetization)
│   ├── udp.c       # UDP transport layer
│   ├── tcp.c       # TCP transport layer
│   ├── source.c    # Frame source / sender
│   └── display.c   # Frame receiver / display
├── bin/            # Compiled binaries (generated)
├── Makefile
└── LICENSE
```

## Requirements

- GCC
- Linux (raw socket support recommended)
- `make`
- Dependencies: `libm`, `libpthread`

## Building

```bash
# Build both binaries
make all

# Or build individually
make source
make display

# Clean
make clean
```

## Usage

Start the **display** endpoint first:

```bash
sudo ./bin/display
```

Then start the **source** endpoint:

```bash
sudo ./bin/source
```

> Raw socket operations typically require root privileges (`sudo`).

## License

[MIT](LICENSE)
