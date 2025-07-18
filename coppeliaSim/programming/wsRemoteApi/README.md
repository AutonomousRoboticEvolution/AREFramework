# WebSocket Remote API for CoppeliaSim

The WebSocket Remote API requires the [WS plugin](https://github.com/CoppeliaRobotics/simExtWS).

### Table of contents

 - [Compiling](#compiling)
 - [Protocol](#protocol)
     - [Request](#request)
     - [Response](#response)


### Compiling

1. Install required packages for simStubsGen: see simStubsGen's [README](https://github.com/CoppeliaRobotics/include/blob/master/simStubsGen/README.md)
2. Checkout, compile and install into CoppeliaSim:
```sh
$ git clone https://github.com/CoppeliaRobotics/wsRemoteApi
$ cd wsRemoteApi
$ git checkout coppeliasim-v4.5.0-rev0
$ mkdir -p build && cd build
$ cmake -DCMAKE_BUILD_TYPE=Release ..
$ cmake --build .
$ cmake --install .
```

NOTE: replace `coppeliasim-v4.5.0-rev0` with the actual CoppeliaSim version you have.

### Protocol

Connect WebSocket to the endpoint (by default on port 23050), send a message (see [request](#request) below), and read the response (see [response](#response) below). The request and response can be serialized to [JSON](https://www.json.org) or [CBOR](https://cbor.io). The response will be serialized using the same serialization format used in the request.

See also the example client `example.html`.

#### Request

A request is an object with fields:
- `func` (string) the function name to call;
- `args` (array) the arguments to the function;
- (optional) `id` (string) an identifier to correlate request with response.

Example:

```json
{
    "func": "sim.getObject",
    "args": ["/Floor"]
}
```

#### Response

A response is an object with fields:
- (optional) `id` (string) set to the same value of the request's `id` field;
- `success` (boolean) `true` if the call succeeded, in which case the `ret` field will be set, or `false` if the call failed, in which case the `error` field will be set;
- `ret` (array) the return values of the function;
- `error` (string) the error message.

Example:

```json
{
    "success": true,
    "ret": [37]
}
```

In case of error, the exception message will be present:

```json
{
    "success": false,
    "error": "Object does not exist. (in function 'sim.getObject')"
}
```
