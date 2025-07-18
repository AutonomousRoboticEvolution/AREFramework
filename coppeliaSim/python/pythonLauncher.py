import sys

import zmq

try:
    import cbor2 as cbor
except ModuleNotFoundError:
    import cbor

context = zmq.Context()
socket = context.socket(zmq.REP)
socket.bind(sys.argv[1])
module = {}
while True:
    # in: cmd + (code or func+args), out: ret or err
    req = cbor.loads(socket.recv())
    rep = {}
    if req['cmd'] == 'loadCode':
        try:
            code = compile(req['code'], req['info'], "exec") #__EXCEPTION__
            exec(code,module) #__EXCEPTION__
            rep = {'ret': None}
        except Exception as e:
            import traceback
            rep = {'err': traceback.format_exc()}
    elif req['cmd'] == 'callFunc':
        try:
            func = module[req['func']]
            rep = {'ret': func(*req['args'])} #__EXCEPTION__
        except Exception as e:
            import traceback
            rep = {'err': traceback.format_exc()}
    else:
        rep = {'err': f'unknown command: "{req["cmd"]}"'}

    socket.send(cbor.dumps(rep))
