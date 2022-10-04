# SimpleHTTP
A small Linux/BSD HTTP server that can serve static files (HTML, TXT, etc.) and server-generated web pages from Python/PHP.
**Usage:**
``` bash
./server -p <port> -r <server root>
```

## Building From Source
`make` to build and `make install` (as root) to install on your system.
Requirements:
* Python3 (unless using `NO_PYTHON=1` during build)
* PHP (unless using `NO_PHP=1` during build) (must be built with embedding enabled if using PHP_USE_EMBED=1 during build)
* A C compiler (tested on Clang and GCC)
* A Make system (tested on GNU Make and Berkely Make)

## Web Apps
### Python
``` python
def http_main(origin, params, method):
  return("<p>Hello, world!</p>")
```
Where `origin` would be the URL without query parameters, `params` would be a dict of query parameters, and `method` would be `GET`/`POST` or whatever method is being used.
### PHP
Though PHP is not *fully* supported yet, the server can run PHP scripts.
Be careful, though. If an error occurs in the script, it could leak sensitive information (that's intended for debbugging use), instead of returning Error 500.

## Credits
 * (MTM828)[https://github.com/MTM828]: Original author of SimpleHTTP.
 * (KapitanOczywisty)[https://github.com/KapitanOczywisty">KapitanOczywisty]: Since the PHP SAPI isn't documented, using it has been a daunting task. I couldn't have done it without the aid of KapitanOczywisty, so thanks to him!

## Contributions are welcome!
There are many things you can do to help:
- Enter input for more `$_SERVER` indices
- Run in background as a service/thread/whatever, I've never done such thing before.
- I haven't looked into security yet. I believe software like Nikto can help, but I haven't tried it yet.
- Clean up the code
- Support for other operating systems/compilers/make systems
- Documentation
- Add directory listing/file guessing
