# NDI Streamer

> Cross-platform display capture over NDI

NewTek's [Scan Converter](https://ndi.tv/tools/) (free!) is capable of sending displays over NDI, really efficiently.  
Unfortunately, this tool only exists for Windows and Mac - leaving Linux systems in the dust.

NDI Streamer is a minimal NDI desktop display steamer to fill in the software gap for Linux users.  
_(It's also meant to be cross platform, but I haven't tested it)_

---

## Rationale

Prior to this, the only way (that I found) to send my Linux desktop over NDI was through the [obs-ndi plugin](https://github.com/Palakis/obs-ndi) plugin.  
That plugin requires [OBS](https://obsproject.com/) to be installed, however (though I would highly recommend you install it and use it for things!).  

Compared to the entire OBS+NDI stack (300 MB RAM), NDI Streamer only uses 30 MB of memory!

---

## Building

> Linux users: You may need to install some development dependencies.  
`$> apt install libxtst-dev libxinerama-dev libx11-dev libxfixes-dev`

`git clone --recurse-submodules <github-link>`

```bash
cmake . # Generate makefiles
make    # Build the program at build/ndi-streamer
```

---

## License

This software is licensed under the MIT license, as can be viewed [here](LICENSE.md).  

NDI Streamer by Andrew Wong.  
NDI® is a registered trademark of NewTek, Inc.


