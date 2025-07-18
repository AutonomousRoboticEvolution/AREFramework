const rad = Math.PI / 180;
const deg = 180 / Math.PI;

//$('#about').text(`r${__REV__}`);

window.location.hashObj = {};
if(window.location.hash)
    window.location.hashObj = JSON5.parse(window.location.hash.substr(1));

const offline = !!window.location.hashObj.offline;

const eventsEndpoint = window.location.hashObj.eventsEndpoint || {};
const remoteApiEndpoint = window.location.hashObj.remoteApiEndpoint || {};
eventsEndpoint.host = eventsEndpoint.host || window.location.hostname;
eventsEndpoint.port = eventsEndpoint.port || wsPort;
eventsEndpoint.codec = eventsEndpoint.codec || codec;
remoteApiEndpoint.host = remoteApiEndpoint.host || window.location.hostname;
remoteApiEndpoint.port = remoteApiEndpoint.port || 23050;
remoteApiEndpoint.codec = remoteApiEndpoint.codec || codec;

function mixin(target, source) {
    // ignore the Function-properties
    const {name, length, prototype, ...statics} = Object.getOwnPropertyDescriptors(source);
    Object.defineProperties(target, statics);

    // ignore the constructor
    const {constructor, ...proto} = Object.getOwnPropertyDescriptors(source.prototype);
    Object.defineProperties(target.prototype, proto);

    return target;
}

class EventSourceMixin {
    addEventListener(eventName, listener) {
        if(!this._eventListeners)
            this._eventListeners = {};
        if(!this._eventListeners[eventName])
            this._eventListeners[eventName] = [];
        this._eventListeners[eventName].push(listener);
    }

    removeEventListener(eventName, listener) {
        if(!this._eventListeners) return;
        if(!this._eventListeners[eventName]) return;
        for(let i = 0; i < this._eventListeners[eventName].length; i++) {
            if(this._eventListeners[eventName][i] === listener) {
                this._eventListeners[eventName].splice(i--, 1);
            }
        }
    }

    dispatchEvent(eventName, ...args) {
        if(!this._eventListeners) return 0;
        var count = 0;
        for(var handledEvent of [eventName, '*']) {
            if(this._eventListeners[handledEvent]) {
                for(var listener of this._eventListeners[handledEvent]) {
                    listener.apply(this, args);
                    count++;
                }
            }
        }
        return count;
    }
}

class Settings {
    constructor(disableAutoWrite) {
        this.selection = {
            style: {
                boundingBox: true,
                boundingBoxSolidOpacity: 0.0,
                boundingBoxSolidSide: THREE.BackSide,
                boundingBoxOnTop: false,
                boundingBoxModelDashed: false,
                boundingBoxModelSolidOpacity: 0.15,
                boundingBoxModelSolidSide: THREE.FrontSide,
                boundingBoxBlinkInterval: 0,
                outline: false,
            },
        };
        this.transformControls = {
            size: 1,
            sendRate: 0,
        };
        this.dummy = {
            style: 1,
        };
        this.octree = {
            maxVoxelCount: 1000000,
        };
        this.events = {
            logging: false,
            discardOutOfSequence: false,
            bufferOutOfSequence: true,
            warnOutOfSequence: false,
            waitForGenesis: true,
        };
        this.shadows = {
            enabled: false,
        };
        this.hoverTool = {
            timeoutMs: 500,
            pick: false,
        };
        this.background = {
            clearColor: 0x1f1f1f,
            clearColorAlpha: 0.8,
        };
        this.camera = {
            autoLocal: true,
        };
        if(!disableAutoWrite) {
            this.read();
            setInterval(() => this.write(), 1000);
        }
    }

    read() {
        var data = localStorage.getItem('settings') || '{}';
        localStorage.setItem('settings', data);
        data = JSON.parse(data);
        Settings.setObject(this, data);
    }

    write() {
        var oldData = localStorage.getItem('settings');
        if(oldData === null)
            return; // settings were removed, don't write anything
        var newSettings = new Settings(true);
        Settings.setObject(newSettings, this);
        Settings.setObject(this, newSettings, true);
        var newData = JSON.stringify(newSettings);
        if(oldData !== newData) {
            localStorage.setItem('settings', newData);
            console.log('Wrote settings to local storage');
        }
    }

    static setObject(dest, src, createNewKeys) {
        for(var k in src) {
            if(!createNewKeys && dest[k] === undefined) continue;
            if(typeof dest[k] === 'function') continue;
            if(typeof dest[k] === 'object') {
                if(dest[k] === undefined) dest[k] = {};
                Settings.setObject(dest[k], src[k]);
            } else {
                dest[k] = src[k];
            }
        }
    }
}

const settings = new Settings();

class VisualizationStreamClient {
    constructor(host = 'localhost', port = 23020, codec = 'cbor') {
        this.host = host;
        this.port = port;
        this.codec = codec;
        this.sessionId = '???';
        this.seq = -1;
        this.eventBuffer = {};
        this.receivedGenesisEvents = false;
        if(offline) return;
        this.websocket = new ReconnectingWebSocket(`ws://${this.host}:${this.port}`);
        if(codec == 'cbor') {
            this.websocket.binaryType = 'arraybuffer';
            this.websocket.onmessage = async (event) => this.handleEvents(CBOR.decode(await event.data.arrayBuffer()));
        } else if(codec == 'json') {
            this.websocket.onmessage = (event) => this.handleEvents(JSON.parse(event.data));
        }
    }

    handleEvents(eventsData) {
        if(eventsData.length !== undefined)
            for(var eventData of eventsData)
                this.handleEvent(eventData);
        else if(eventsData.event !== undefined)
            this.handleEvent(eventsData);
    }

    handleEvent(eventData) {
        // unflatten dotted properties
        for(let k in eventData.data ?? {}) {
            if(k.includes('.')) {
                let f = k.split('.');
                let tmp = eventData.data;
                for(let i = 0; i < f.length - 1; i++) {
                    tmp[f[i]] = tmp[f[i]] ?? {};
                    tmp = tmp[f[i]];
                }
                tmp[f[f.length - 1]] = eventData.data[k];
                delete eventData.data[k];
            }
        }

        if(eventData.event === 'genesisBegin') {
            if(this.seq === -1 && !this.receivedGenesisEvents)
                this.seq = eventData.seq - 1;
            this.receivedGenesisEvents = true;
        }

        var outOfSequence = (!offline && settings.events.waitForGenesis && !this.receivedGenesisEvents) ||
            (this.seq !== -1 && eventData.seq !== undefined && eventData.seq !== (this.seq + 1));

        if(outOfSequence && !settings.events.discardOutOfSequence && settings.events.warnOutOfSequence) {
            console.warn(`Received event with seq=${eventData.seq} (was expecting seq=${this.seq+1})`, eventData);
        }

        if(settings.events.logging) {
            if(eventData.seq !== undefined && this.seq >= 0) {
                var gap = eventData.seq - this.seq;
                if(gap > 1 && settings.events.discardOutOfSequence && settings.events.warnOutOfSequence) {
                    var li = document.createElement('li');
                    var txt = document.createTextNode(`warning: gap of ${gap-1} missing events!`);
                    li.appendChild(txt);
                    document.getElementById('log').appendChild(li);
                }
            }

            const eventInfo = (eventData) => {
                return eventData.event;
            }

            const uidInfo = (eventData) => {
                var info = `${eventData.uid}`;
                if(eventData.data.alias)
                    info += ` (${eventData.data.alias})`;
                var obj = sceneWrapper.getObjectByUid(eventData.uid);
                if(obj !== undefined)
                    info += ` (${obj.name})`;
                return info;
            }

            var li = document.createElement('li');
            if(eventData.seq !== undefined && eventData.seq <= this.seq && settings.events.discardOutOfSequence)
                li.classList.add('rejected');
            var hdr = document.createElement('span');
            hdr.classList.add('event-header');
            var txt = document.createTextNode(`${eventData.seq}\t${eventInfo(eventData)}\t${uidInfo(eventData)} `);
            hdr.appendChild(txt);
            li.appendChild(hdr);
            li.appendChild(renderjson(eventData));
            document.getElementById('log').appendChild(li);
        }

        if(outOfSequence && settings.events.discardOutOfSequence) {
            console.warn(`Discarded event with seq=${eventData.seq} (was expecting seq=${this.seq+1})`, eventData);
            return;
        }

        if(outOfSequence && settings.events.bufferOutOfSequence) {
            this.eventBuffer[eventData.seq] = eventData;
            return;
        }

        const dispatch = (eventData) => {
            if(this.dispatchEvent(eventData.event, eventData) == 0) {
                console.warn(`No listeners for event "${eventData.event}"`, eventData);
            }
            this.seq = eventData.seq;
        };

        dispatch(eventData);

        // see if there are any out-of-sequence events that can be dispatched:
        while(this.eventBuffer[this.seq + 1] !== undefined) {
            var pendingEvent = this.eventBuffer[this.seq + 1];
            delete this.eventBuffer[this.seq + 1];
            dispatch(pendingEvent);
        }
    }
}

mixin(VisualizationStreamClient, EventSourceMixin);

class BaseObject extends THREE.Group {
    static objectsByUid = {};

    static getObjectByUid(uid) {
        return this.objectsByUid[uid];
    }

    constructor(sceneWrapper) {
        super();
        this.sceneWrapper = sceneWrapper;
        this.userData.type = 'unknown';
    }

    init() {
        // this should be called after object creation only if object is not cloned
        // to initialize any children (e.g. visuals, frames, ...) that are part of
        // the object
    }

    clone(recursive) {
        var obj = new this.constructor(this.sceneWrapper).copy(this, recursive);
        return obj;
    }

    get parentObject() {
        return BaseObject.getObjectByUid(this.userData.parentUid);
    }

    get nameWithOrder() {
        return this.name + (this.userData.childOrder === -1 ? '' : `[${this.userData.childOrder}]`);
    }

    get path() {
        return (this.parentObject ? `${this.parentObject.path}/` : '/') + this.nameWithOrder;
    }

    get childObjects() {
        var objs = [];
        for(var o of this.children) {
            if(o instanceof DrawingObject) continue;
            if(o.userData.parentUid === this.userData.uid)
                objs.push(o);
        }
        return objs;
    }

    get ancestorObjects() {
        var objs = [];
        var o = this;
        while(o.parentObject) {
            o = o.parentObject;
            objs.push(o);
        }
        return objs;
    }

    update(eventData) {
        if(eventData.uid !== undefined)
            this.setUid(eventData.uid);
        if(eventData.handle !== undefined)
            this.setHandle(eventData.handle);
        if(eventData.data.alias !== undefined)
            this.setAlias(eventData.data.alias);
        if(eventData.data.childOrder !== undefined)
            this.setChildOrder(eventData.data.childOrder);
        if(eventData.data.parentUid !== undefined)
            this.setParent(eventData.data.parentUid);
        if(eventData.data.pose !== undefined)
            this.setPose(eventData.data.pose);
        if(eventData.data.layer !== undefined)
            this.setLayer(eventData.data.layer);
        if(eventData.data.modelBase !== undefined)
            this.setModelBase(eventData.data.modelBase);
        if(eventData.data.modelInvisible !== undefined)
            this.setModelInvisible(eventData.data.modelInvisible);
        if(eventData.data.objectPropertyFlags !== undefined)
            this.setObjectProperty(eventData.data.objectPropertyFlags);
        if(eventData.data.modelPropertyFlags !== undefined)
            this.setModelProperty(eventData.data.modelPropertyFlags);
        if(eventData.data.mov !== undefined)
        {
            if(eventData.data.mov.optionsFlags !== undefined)
                this.setMovementOptions(eventData.data.mov.optionsFlags);
            if(eventData.data.mov.preferredAxesFlags !== undefined)
                this.setMovementPreferredAxes(eventData.data.mov.preferredAxesFlags);
            if(eventData.data.mov.relativity !== undefined)
                this.setMovementRelativity(eventData.data.mov.relativity);
            if(eventData.data.mov.stepSize !== undefined)
                this.setMovementStepSize(eventData.data.mov.stepSize);
        }
        if(eventData.data.bbHSize !== undefined)
            this.setBoundingBoxHSize(eventData.data.bbHSize);
        if(eventData.data.bbPose !== undefined)
            this.setBoundingBoxPose(eventData.data.bbPose);
        if(eventData.data.customData !== undefined)
            this.setCustomData(eventData.data.customData);
        
        // Following for backw. compatibility:
        if(eventData.data.objectProperty !== undefined)
            this.setObjectProperty(eventData.data.objectProperty);
        if(eventData.data.modelProperty !== undefined)
            this.setModelProperty(eventData.data.modelProperty);
        if(eventData.data.movementOptions !== undefined)
            this.setMovementOptions(eventData.data.movementOptions);
        if(eventData.data.movementPreferredAxes !== undefined)
            this.setMovementPreferredAxes(eventData.data.movementPreferredAxes);
        if(eventData.data.movementRelativity !== undefined)
            this.setMovementRelativity(eventData.data.movementRelativity);
        if(eventData.data.movementStepSize !== undefined)
            this.setMovementStepSize(eventData.data.movementStepSize);
    }

    setUid(uid) {
        if(this.userData.uid !== undefined) {
            if(BaseObject.objectsByUid[this.userData.uid] !== undefined) {
                delete BaseObject.objectsByUid[this.userData.uid];
            }
        }
        this.userData.uid = uid;
        BaseObject.objectsByUid[uid] = this;
    }

    setHandle(handle) {
        this.userData.handle = handle;
    }

    setAlias(alias) {
        this.name = alias;
    }

    setChildOrder(childOrder) {
        this.userData.childOrder = childOrder;
    }

    setParent(parentUid) {
        this.userData.parentUid = parentUid
        var parentObj = BaseObject.getObjectByUid(parentUid);
        if(parentObj !== undefined) {
            var p = this.position.clone();
            var q = this.quaternion.clone();
            parentObj.attach(this);
            this.position.copy(p);
            this.quaternion.copy(q);
        } else /*if(parentUid === -1)*/ {
            if(parentUid !== -1)
                console.error(`Parent with uid=${parentUid} is not known`);
            this.sceneWrapper.scene.attach(this);
        }
    }

    getPose() {
        return [
            ...this.position.toArray(),
            ...this.quaternion.toArray(),
        ];
    }

    setPose(pose) {
        this.position.set(pose[0], pose[1], pose[2]);
        this.quaternion.set(pose[3], pose[4], pose[5], pose[6]);
    }

    getAbsolutePose() {
        this.updateMatrixWorld();
        var position = new THREE.Vector3();
        position.setFromMatrixPosition(this.matrixWorld);
        var quaternion = new THREE.Quaternion();
        quaternion.setFromRotationMatrix(this.matrixWorld);
        return [
            ...position.toArray(),
            ...quaternion.toArray(),
        ];
    }

    setLayer(layer) {
        this.userData.layer = layer;
    }

    setObjectProperty(objectProperty) {
        this.userData.objectProperty = objectProperty;
        this.setSelectable((objectProperty & 0x20) > 0);
        this.setSelectModelBase((objectProperty & 0x80) > 0);
        this.setExcludeFromBBoxComputation((objectProperty & 0x100) > 0);
        this.setClickInvisible((objectProperty & 0x800) > 0);
    }

    setSelectable(selectable) {
        this.userData.selectable = selectable;
    }

    setSelectModelBase(selectModelBase) {
        this.userData.selectModelBase = selectModelBase;
    }

    setExcludeFromBBoxComputation(exclude) {
        this.userData.excludeFromBBoxComputation = exclude;
    }

    setClickInvisible(clickInvisible) {
        this.userData.clickInvisible = clickInvisible;
    }

    setModelProperty(modelProperty) {
        this.userData.modelProperty = modelProperty;
        this.setExcludeModelFromBBoxComputation((modelProperty & 0x400) > 0);
    }

    setExcludeModelFromBBoxComputation(exclude) {
        this.userData.excludeModelFromBBoxComputation = exclude;
    }

    setModelBase(modelBase) {
        this.userData.modelBase = modelBase;
    }

    setModelInvisible(modelInvisible) {
        this.userData.modelInvisible = modelInvisible;
        // trigger `layer` update:
        this.setLayer(this.userData.layer);
    }

    computedLayer() {
        if(this.userData.modelInvisible)
            return 0;
        return this.userData.layer;
    }

    setMovementOptions(movementOptions) {
        this.userData.movementOptions = movementOptions;
        this.userData.canTranslateOutsideSimulation = !(movementOptions & 0x1);
        this.userData.canTranslateDuringSimulation = !(movementOptions & 0x2);
        this.userData.canRotateOutsideSimulation = !(movementOptions & 0x4);
        this.userData.canRotateDuringSimulation = !(movementOptions & 0x8);
        this.userData.hasTranslationalConstraints = !!(movementOptions & 0x10);
        this.userData.hasRotationalConstraints = !!(movementOptions & 0x20);
    }

    setMovementPreferredAxes(movementPreferredAxes) {
        this.userData.movementPreferredAxes = {
            translation: {
                x: !!(movementPreferredAxes & 0x1),
                y: !!(movementPreferredAxes & 0x2),
                z: !!(movementPreferredAxes & 0x4),
            },
            rotation: {
                x: !!(movementPreferredAxes & 0x8),
                y: !!(movementPreferredAxes & 0x10),
                z: !!(movementPreferredAxes & 0x20),
            },
        };
    }

    setMovementRelativity(movementRelativity) {
        this.userData.movementRelativity = movementRelativity;
        this.userData.translationSpace = movementRelativity[0] === 0 ? 'world' : 'local';
        this.userData.rotationSpace = movementRelativity[1] === 0 ? 'world' : 'local';
    }

    setMovementStepSize(movementStepSize) {
        this.userData.movementStepSize = movementStepSize;
        this.userData.translationStepSize = movementStepSize[0] > 0 ? movementStepSize[0] : null;
        this.userData.rotationStepSize = movementStepSize[1] > 0 ? movementStepSize[1] : null;
    }

    setBoundingBoxHSize(bbHSize) {
        this.userData.bbHSize = bbHSize;
    }

    setBoundingBoxPose(bbPose) {
        this.userData.bbPose = bbPose;
    }

    get boundingBoxPoints() {
        if(!this.userData.bbPose || !this.userData.bbHSize) return [];
        var hsize = this.userData.bbHSize;
        var pose = this.userData.bbPose;
        var m = new THREE.Matrix4().compose(
            new THREE.Vector3(pose[0], pose[1], pose[2]),
            new THREE.Quaternion(pose[3], pose[3], pose[5], pose[6]),
            new THREE.Vector3(1, 1, 1)
        );
        var pts = [];
        for(var kx of [-1, 1]) {
            for(var ky of [-1, 1]) {
                for(var kz of [-1, 1]) {
                    var v = new THREE.Vector3(kx * hsize[0], ky * hsize[1], kz * hsize[2]);
                    v.applyMatrix4(m);
                    v.applyMatrix4(this.matrixWorld);
                    pts.push(v);
                }
            }
        }
        return pts;
    }

    get boundingBoxObjects() {
        var objects = [];
        if(this.userData.modelBase) {
            var queue = [];
            queue.push(this);
            while(queue.length > 0) {
                var o = queue.shift();
                if(!(o.userData.excludeFromBBoxComputation === true))
                    objects.push(o);
                if(o === this || !(o.userData.excludeModelFromBBoxComputation === true))
                    for(var c of o.childObjects)
                        queue.push(c);
            }
        } else {
            objects.push(this);
        }
        return objects;
    }

    setCustomData(customData) {
        this.userData.customData = customData;
    }
}

class Shape extends BaseObject {
    constructor(sceneWrapper) {
        super(sceneWrapper);
        this.userData.type = 'shape';
    }

    update(eventData) {
        super.update(eventData);
        if(eventData.data.meshes !== undefined)
            this.setShapeMeshes(eventData.data.meshes);
    }

    setLayer(layer) {
        super.setLayer(layer);
        for(var mesh of this.children) {
            if(mesh.userData.type !== 'meshobject') continue;
            for(var c of mesh.children) {
                c.layers.mask = this.computedLayer();
            }
        }
    }

    setShapeMeshes(meshes) {
        this.userData.meshes = meshes;
    }
}

class Mesh extends BaseObject {
    constructor(sceneWrapper) {
        super();
        this.userData.type = 'meshobject';
        this.sceneWrapper = sceneWrapper;
    }

    init() {
        super.init();
        this.mesh;
        this.edgeMesh;
    }

    clone(recursive) {
        var obj = new this.constructor(this.sceneWrapper, this.parentObject).copy(this, true);
        return obj;
    }

    get mesh() {
        for(var c of this.children) {
            if(c.type === 'Mesh' && c.userData.type == 'mesh')
                return c;
        }

        const geometry = new THREE.BufferGeometry();
        const material = new THREE.MeshPhongMaterial({
            polygonOffset: true,
            polygonOffsetFactor: 0.5,
            polygonOffsetUnits: 0.0,
        });
        var mesh = new THREE.Mesh(geometry, material);
        mesh.name = 'Mesh';
        mesh.userData.type = 'mesh';
        mesh.castShadow = settings.shadows.enabled;
        mesh.receiveShadow = settings.shadows.enabled;
        this.add(mesh);
        return mesh;
    }

    get edgeMesh() {
        for(var c of this.children) {
            if(c.type === 'LineSegments' && c.userData.type == 'edges')
                return c;
        }

        if(!this.mesh.geometry.hasAttribute('position')) {
            // has not yet received data
            return;
        }

        var data = {shadingAngle: Math.PI / 4.};
        const edgeMesh = new THREE.LineSegments(
            data.shadingAngle < 1e-4
                ? new THREE.WireframeGeometry(this.mesh.geometry)
                : new THREE.EdgesGeometry(this.mesh.geometry, data.shadingAngle * 180 / Math.PI),
            new THREE.LineBasicMaterial({color: 0x000000})
        );
        edgeMesh.name = 'Edges';
        edgeMesh.userData.type = 'edges';
        this.add(edgeMesh);
        return edgeMesh;
    }

    update(eventData) {
        super.update(eventData);

        const data = eventData.data;

        if(data.shapeUid !== undefined) {
            var shape = this.sceneWrapper.getObjectByUid(data.shapeUid);
            shape.attach(this);
            this.position.set(0, 0, 0);
            this.quaternion.set(0, 0, 0, 1);
            this.mesh.userData.pickThisIdInstead = shape.id;
        }
        if(data.indices !== undefined && data.vertices !== undefined) {
            // XXX: vertex attribute format handed by CoppeliaSim is not correct
            //      we expand all attributes and discard indices
            if(false) {
                this.mesh.geometry.setIndex(data.indices);
                this.mesh.geometry.setAttribute('position', new THREE.Float32BufferAttribute(data.vertices, 3));
                this.mesh.geometry.setAttribute('normal', new THREE.Float32BufferAttribute(data.normals, 3));
            } else {
                var ps = [];
                var ns = [];
                for(var i = 0; i < data.indices.length; i++) {
                    var index = data.indices[i];
                    var p = data.vertices.slice(3 * index, 3 * (index + 1));
                    ps.push(p[0], p[1], p[2]);
                    if(data.normals !== undefined) {
                        var n = data.normals.slice(3 * i, 3 * (i + 1));
                        ns.push(n[0], n[1], n[2]);
                    }
                }
                this.mesh.geometry.setAttribute('position', new THREE.Float32BufferAttribute(ps, 3));
                if(data.normals !== undefined)
                    this.mesh.geometry.setAttribute('normal', new THREE.Float32BufferAttribute(ns, 3));
            }
            this.mesh.geometry.computeBoundingBox();
            this.mesh.geometry.computeBoundingSphere();
        }
        if(data.culling !== undefined) {
            this.mesh.material.side = data.culling ? THREE.FrontSide : THREE.DoubleSide;
        }
        if(data.color !== undefined) {
            if(data.color.diffuse !== undefined)
                this.mesh.material.color = new THREE.Color(...data.color.diffuse);
            if(data.color.specular !== undefined)
                this.mesh.material.specular = new THREE.Color(...data.color.specular);
            if(data.color.emission !== undefined)
                this.mesh.material.emissive = new THREE.Color(...data.color.emission);
        }
        if(data.rawTexture !== undefined) {
            var texture = new THREE.DataTexture(data.rawTexture, data.textureResolution[0], data.textureResolution[1], THREE.RGBAFormat);
            if(data.textureRepeatU)
                texture.wrapS = THREE.RepeatWrapping;
            if(data.textureRepeatV)
                texture.wrapT = THREE.RepeatWrapping;
            if(data.textureInterpolate)
                texture.magFilter = texture.minFilter = THREE.LinearFilter;
            else
                texture.magFilter = texture.minFilter = THREE.NearestFilter;

            if(false) { // XXX: see above
                this.mesh.geometry.setAttribute('uv', new THREE.Float32BufferAttribute(data.textureCoordinates, 2));
            } else {
                var uvs = [];
                for(var i = 0; i < data.indices.length; i++) {
                    var index = data.indices[i];
                    var uv = data.textureCoordinates.slice(2 * i, 2 * (i + 1));
                    uvs.push(uv[0], uv[1]);
                }
                this.mesh.geometry.setAttribute('uv', new THREE.Float32BufferAttribute(uvs, 2));
            }
            this.mesh.material.map = texture;
        }
        if(data.options !== undefined) {
            if((data.options & 2) > 0) {
                this.mesh.material.wireframe = true;
            }
        }
        if(data.transparency !== undefined) {
            this.mesh.material.transparent = data.transparency > 1e-4;
            this.mesh.material.opacity = 1 - data.transparency;
        }
        if(this.edgeMesh !== undefined && data.shapeUid !== undefined) {
            this.edgeMesh.userData.pickThisIdInstead = shape.id;
        }
        if(this.edgeMesh !== undefined && data.shadingAngle !== undefined) {
            this.edgeMesh.geometry.thresholdAngle = data.shadingAngle * 180 / Math.PI;
            this.userData.shadingAngle = data.shadingAngle;
        }
        if(this.edgeMesh !== undefined && data.showEdges !== undefined) {
            this.edgeMesh.visible = data.showEdges;
            this.userData.showEdges = data.showEdges;
        }

        this.parent.setLayer(this.parent.userData.layer);
    }
}

class BaseVisual extends THREE.Group {
    constructor(sceneWrapper, parentObject) {
        super();
        this.userData.type = 'baseVisual';
        this.sceneWrapper = sceneWrapper;
        this.parentObject = parentObject;
    }

    init() {
    }

    clone(recursive) {
        var obj = new this.constructor(this.sceneWrapper, this.parentObject).copy(this, true);
        return obj;
    }

    setLayer(layer) {
        this.userData.layer = layer;
    }
}

class JointVisual extends BaseVisual {
    constructor(sceneWrapper, parentObject) {
        super(sceneWrapper, parentObject);
        this.userData.type = 'jointVisual';
    }

    init() {
        super.init();
        this.fixedGeom;
        this.movingGeom;
    }

    static create(type, sceneWrapper, parentObject) {
        if(type == 'revolute')
            return new JointVisualRevolute(sceneWrapper, parentObject);
        if(type == 'prismatic')
            return new JointVisualPrismatic(sceneWrapper, parentObject);
        if(type == 'spherical')
            return new JointVisualSpherical(sceneWrapper, parentObject);
    }

    get fixedGeom() {
        for(var c of this.children) {
            if(c.userData.type === `${this.userData.type}.fixed`)
                return c;
        }

        var fixedGeom = this.createFixedPart();
        fixedGeom.name = 'Joint fixed part';
        fixedGeom.userData.type = `${this.userData.type}.fixed`;
        fixedGeom.userData.pickThisIdInstead = this.parentObject.id;
        fixedGeom.rotation.x = Math.PI / 2;
        fixedGeom.material.color.setRGB(0, 0, 0);
        fixedGeom.material.specular.setRGB(0, 0, 0);
        fixedGeom.material.emissive.setRGB(0, 0, 0);
        this.add(fixedGeom);
        return fixedGeom;
    }

    get jointFrame() {
        for(var c of this.children) {
            if(c.userData.type === `${this.userData.type}.jointFrame`)
                return c;
        }

        var jointFrame = new THREE.Group();
        jointFrame.userData.type = `${this.userData.type}.jointFrame`;
        this.add(jointFrame);
        return jointFrame;
    }

    get movingGeom() {
        for(var c of this.jointFrame.children) {
            if(c.userData.type === `${this.userData.type}.moving`)
                return c;
        }

        var movingGeom = this.createMovingPart();
        movingGeom.name = 'Joint moving part';
        movingGeom.userData.type = `${this.userData.type}.moving`;
        movingGeom.userData.pickThisIdInstead = this.parentObject.id;
        movingGeom.rotation.x = Math.PI / 2;
        movingGeom.material.color.setRGB(0, 0, 0);
        movingGeom.material.specular.setRGB(0, 0, 0);
        movingGeom.material.emissive.setRGB(0, 0, 0);
        this.jointFrame.add(movingGeom);
        return movingGeom;
    }

    setIntrinsicPose(intrinsicPose) {
        this.jointFrame.position.set(intrinsicPose[0], intrinsicPose[1], intrinsicPose[2]);
        this.jointFrame.quaternion.set(intrinsicPose[3], intrinsicPose[4], intrinsicPose[5], intrinsicPose[6]);
    }

    setLayer(layer) {
        super.setLayer(layer);
        this.fixedGeom.layers.mask = this.parentObject.computedLayer();
        this.movingGeom.layers.mask = this.parentObject.computedLayer();
    }

    setColor(color) {
        this.userData.color = color;
        this.fixedGeom.material.color.setRGB(...color.diffuse);
        this.fixedGeom.material.specular.setRGB(...color.specular);
        this.fixedGeom.material.emissive.setRGB(...color.emission);
    }

    setDiameter(diameter) {
        this.userData.diameter = diameter;
    }

    setLength(length) {
        this.userData.length = length;
    }
}

class JointVisualRevolute extends JointVisual {
    createFixedPart() {
        var fixedGeom = new THREE.Mesh(
            new THREE.CylinderGeometry(1, 1, 1, 8),
            new THREE.MeshPhongMaterial({}),
        );
        return fixedGeom;
    }

    createMovingPart() {
        var movingGeom = new THREE.Mesh(
            new THREE.CylinderGeometry(1, 1, 1, 8),
            new THREE.MeshPhongMaterial({}),
        );
        return movingGeom;
    }

    setDiameter(diameter) {
        super.setDiameter(diameter);
        const r1 = diameter / 2;
        const r2 = r1 / 2;
        this.fixedGeom.scale.x = r1;
        this.fixedGeom.scale.z = r1;
        this.movingGeom.scale.x = r2;
        this.movingGeom.scale.z = r2;
    }

    setLength(length) {
        super.setLength(length);
        const l1 = length * 1.001;
        const l2 = length * 1.201;
        this.fixedGeom.scale.y = l1;
        this.movingGeom.scale.y = l2;
    }
}

class JointVisualPrismatic extends JointVisual {
    createFixedPart() {
        var fixedGeom = new THREE.Mesh(
            new THREE.BoxGeometry(1, 1, 1),
            new THREE.MeshPhongMaterial({}),
        );
        return fixedGeom;
    }

    createMovingPart() {
        var movingGeom = new THREE.Mesh(
            new THREE.BoxGeometry(1, 1, 1),
            new THREE.MeshPhongMaterial({}),
        );
        return movingGeom;
    }

    setDiameter(diameter) {
        super.setDiameter(diameter);
        const r1 = diameter / 2;
        const r2 = r1 / 2;
        this.fixedGeom.scale.x = 2 * r1;
        this.fixedGeom.scale.z = 2 * r1;
        this.movingGeom.scale.x = 2 * r2;
        this.movingGeom.scale.z = 2 * r2;
    }

    setLength(length) {
        super.setLength(length);
        const l1 = length * 1.001;
        const l2 = length * 1.201;
        this.fixedGeom.scale.y = l1;
        this.movingGeom.scale.y = l2;
    }
}

class JointVisualSpherical extends JointVisual {
    createFixedPart() {
        var fixedGeom = new THREE.Mesh(
            new THREE.SphereGeometry(1, 16, 8),
            new THREE.MeshPhongMaterial({
                side: THREE.BackSide,
            })
        );
        return fixedGeom;
    }

    createMovingPart() {
        var movingGeom = new THREE.Mesh(
            new THREE.SphereGeometry(1, 16, 8),
            new THREE.MeshPhongMaterial({}),
        );
        return movingGeom;
    }

    setDiameter(diameter) {
        super.setDiameter(diameter);
        const r1 = diameter / 2;
        const r2 = r1 / 2;
        this.fixedGeom.scale.x = 2 * r1;
        this.fixedGeom.scale.y = 2 * r1;
        this.fixedGeom.scale.z = 2 * r1;
        this.movingGeom.scale.x = 2.5 * r2;
        this.movingGeom.scale.y = 2.5 * r2;
        this.movingGeom.scale.z = 2.5 * r2;
    }
}

class Joint extends BaseObject {
    constructor(sceneWrapper) {
        super(sceneWrapper);
        this.userData.type = 'joint';
    }

    init() {
        super.init();
        this.jointFrame;
        this.visual;
    }

    get jointFrame() {
        for(var c of this.children) {
            if(c.userData.type === 'jointFrame')
                return c;
        }

        var jointFrame = new THREE.Group();
        jointFrame.userData.type = 'jointFrame';
        this.add(jointFrame);
        return jointFrame;
    }

    get childObjects() {
        return [...this.jointFrame.children].filter((o) => o.userData.parentUid === this.userData.uid);
    }

    get visual() {
        for(var c of this.children) {
            if(c.userData.type === 'jointVisual')
                return c;
        }

        if(this.userData.joint?.type === undefined)
            return;

        var visual = JointVisual.create(this.userData.joint.type, this.sceneWrapper, this);
        visual.init();
        this.add(visual);

        // visuals have been added -> set layer
        this.setLayer(this.userData.layer);
    }

    attach(o) {
        this.jointFrame.attach(o);
    }

    update(eventData) {
        super.update(eventData);
        if(this.userData.joint === undefined)
            this.userData.joint = {};
        if(eventData.data.jointType !== undefined)
            this.setJointType(eventData.data.jointType);
        if(eventData.data.jointPosition !== undefined)
            this.setJointPosition(eventData.data.jointPosition);
        if(eventData.data.cyclic !== undefined)
            this.setJointCyclic(eventData.data.cyclic);
        if(eventData.data.min !== undefined)
            this.setJointMin(eventData.data.min);
        if(eventData.data.range !== undefined)
            this.setJointRange(eventData.data.range);
        if(eventData.data.intrinsicPose !== undefined)
            this.setJointIntrinsicPose(eventData.data.intrinsicPose);
        if(eventData.data.color !== undefined)
            this.setJointColor(eventData.data.color);
        if(eventData.data.jointDiameter !== undefined)
            this.setJointDiameter(eventData.data.jointDiameter);
        if(eventData.data.jointLength !== undefined)
            this.setJointLength(eventData.data.jointLength);
        if(eventData.data.dependencyParams !== undefined)
            this.setJointDependency(eventData.data.dependencyParams);
    }

    setLayer(layer) {
        super.setLayer(layer);
        this.visual?.setLayer(layer);
    }

    setJointType(type) {
        // `type` can only be set once
        if(this.userData.joint.type !== undefined)
            return;

        this.userData.joint.type = {
            10: 'revolute',
            11: 'prismatic',
            12: 'spherical',
        }[type];

        // invoke getter now:
        this.visual;
    }

    setJointPosition(position) {
        this.userData.joint.position = position;
    }

    setJointCyclic(cyclic) {
        this.userData.joint.cyclic = cyclic;
    }

    setJointMin(min) {
        this.userData.joint.min = min;
        this.userData.joint.max = this.userData.joint.min + this.userData.joint.range;
    }

    setJointRange(range) {
        this.userData.joint.range = range;
        this.userData.joint.max = this.userData.joint.min + this.userData.joint.range;
    }

    setJointIntrinsicPose(intrinsicPose) {
        this.jointFrame.position.set(intrinsicPose[0], intrinsicPose[1], intrinsicPose[2]);
        this.jointFrame.quaternion.set(intrinsicPose[3], intrinsicPose[4], intrinsicPose[5], intrinsicPose[6]);
        this.visual?.setIntrinsicPose(intrinsicPose);
    }

    setJointColor(color) {
        this.visual?.setColor(color);
    }

    setJointDiameter(diameter) {
        this.userData.joint.diameter = diameter;
        this.visual?.setDiameter(diameter);
    }

    setJointLength(length) {
        this.userData.joint.length = length;
        this.visual?.setLength(length);
    }

    setJointDependency(dependency) {
        this.userData.joint.dependency = dependency;
    }
}

class DummyVisual extends BaseVisual {
    constructor(sceneWrapper, parentObject) {
        super(sceneWrapper, parentObject);
        this.userData.type = 'dummyVisual';
    }

    init() {
        super.init();
        this.ballGeom;
        this.axesGeom;
    }

    createBall() {
        var ballGeom = new THREE.Mesh(
            new THREE.SphereGeometry(1, 8, 8),
            new THREE.MeshPhongMaterial({
            })
        );
        return ballGeom;
    }

    createAxes() {
        var axesGeom = new THREE.AxesHelper(4);
        return axesGeom;
    }

    get ballGeom() {
        for(var c of this.children) {
            if(c.userData.type === `${this.userData.type}.ball`)
                return c;
        }

        var ballGeom = this.createBall();
        ballGeom.name = 'Dummy ball';
        ballGeom.userData.type = `${this.userData.type}.ball`;
        ballGeom.userData.pickThisIdInstead = this.parentObject.id;
        this.add(ballGeom);
        return ballGeom;
    }

    get axesGeom() {
        for(var c of this.children) {
            if(c.userData.type === `${this.userData.type}.axes`)
                return c;
        }

        var axesGeom = this.createAxes();
        axesGeom.name = 'Dummy axes';
        axesGeom.userData.type = `${this.userData.type}.axes`;
        axesGeom.userData.pickThisIdInstead = this.parentObject.id;
        this.add(axesGeom);
        return axesGeom;
    }

    setLayer(layer) {
        this.ballGeom.layers.mask = this.parentObject.computedLayer();
        this.axesGeom.layers.mask = this.parentObject.computedLayer();
    }

    setSize(size) {
        const r1 = size / 2;
        this.scale.x = r1;
        this.scale.y = r1;
        this.scale.z = r1;
    }

    setColor(color) {
        this.ballGeom.material.color.setRGB(...color.diffuse);
        this.ballGeom.material.specular.setRGB(...color.specular);
        this.ballGeom.material.emissive.setRGB(...color.emission);
    }
}

class DummyVisualAlt extends DummyVisual {
    constructor(sceneWrapper, parentObject) {
        super(sceneWrapper, parentObject);
    }

    createBall() {
        var ballGeom = new THREE.Group();
        this.sceneWrapper.cameraFacingObjects.push(ballGeom);
        ballGeom.materialBlack = new THREE.MeshPhongMaterial({color: 0x000000});
        ballGeom.material = new THREE.MeshPhongMaterial({color: 0xffff00});
        const I = 4, J = 2;
        for(let i = 0; i < I; i++) {
            for(let j = 0; j < J; j++) {
                let submesh = new THREE.Mesh(
                    new THREE.SphereGeometry(
                        0.9,
                        16, 8,
                        i * 2 * Math.PI / I,
                        2 * Math.PI / I,
                        j * Math.PI / J,
                        Math.PI / J
                    ),
                    (i + j) % 2 == 0 ? ballGeom.material : ballGeom.materialBlack
                );
                ballGeom.add(submesh);
            }
        }
        let submesh = new THREE.Mesh(
            new THREE.SphereGeometry(1, 32, 16),
            new THREE.MeshPhongMaterial({
                color: 0x000000,
                side: THREE.BackSide,
            })
        );
        ballGeom.add(submesh);
        return ballGeom;
    }

    setLayer(layer) {
        super.setLayer(layer);
        this.ballGeom.traverse((o) => {o.layers.mask = this.parentObject.computedLayer()});
    }
}

class Dummy extends BaseObject {
    constructor(sceneWrapper) {
        super(sceneWrapper);
        this.userData.type = 'dummy';
    }

    init() {
        super.init();
        this.visual;
    }

    get visual() {
        for(var c of this.children) {
            if(c.userData.type === 'dummyVisual')
                return c;
        }

        if(settings.dummy.style == 2) {
            var visual = new DummyVisualAlt(sceneWrapper, this);
        } else {
            var visual = new DummyVisual(sceneWrapper, this);
        }
        visual.init();
        visual.setSize(0.01);
        this.add(visual);
        return visual;
    }

    update(eventData) {
        super.update(eventData);
        if(eventData.data.dummySize !== undefined)
            this.setDummySize(eventData.data.dummySize);
        if(eventData.data.color !== undefined)
            this.setDummyColor(eventData.data.color);
    }

    setLayer(layer) {
        super.setLayer(layer);
        this.visual.setLayer(layer);
    }

    setDummySize(size) {
        this.visual.setSize(size);
    }

    setDummyColor(color) {
        this.visual.setColor(color);
    }
}

class CameraVisual extends BaseVisual {
    constructor(sceneWrapper, parentObject) {
        super(sceneWrapper, parentObject);
        this.userData.type = 'cameraVisual';
        this.add(
            new THREE.Mesh(
                new THREE.CylinderGeometry(0.025, 0.01, 0.05, 12, 1, true),
                new THREE.MeshPhongMaterial({color: 0x7f7f7f, side: THREE.DoubleSide})
            )
        );
        this.children[0].position.z = 0.025;
        this.children[0].rotation.x = Math.PI / 2;
        this.add(
            new THREE.Mesh(
                new THREE.CylinderGeometry(0.05, 0.05, 0.025, 20, 32),
                new THREE.MeshPhongMaterial({color: 0x7f7f7f})
            )
        );
        this.children[1].position.set(0, 0.065, -0.0125);
        this.children[1].rotation.z = Math.PI / 2;
        this.add(
            new THREE.Mesh(
                new THREE.CylinderGeometry(0.05, 0.05, 0.025, 20, 32),
                new THREE.MeshPhongMaterial({color: 0x7f7f7f})
            )
        );
        this.children[2].position.set(0, 0.065, -0.085);
        this.children[2].rotation.z = Math.PI / 2;
        this.add(
            new THREE.Mesh(
                new THREE.BoxGeometry(0.02, 0.05, 0.1),
                new THREE.MeshPhongMaterial({color: 0xd90000})
            )
        );
        this.children[3].position.z = -0.05;
        this.children[3].userData.getsColor = true;
        this.traverse(o => o.userData.pickThisIdInstead = this.id);

        this.bodyGeoms = [this.children[3]];
    }

    clone(recursive) {
        var obj = new this.constructor(this.sceneWrapper, this.parentObject).copy(this, false);
        return obj;
    }

    setLayer(layer) {
        this.traverse((o) => {o.layers.mask = this.parentObject.computedLayer()});
    }

    setColor(color) {
        for(var c of this.bodyGeoms) {
            c.material.color.setRGB(...color.diffuse);
            c.material.specular.setRGB(...color.specular);
            c.material.emissive.setRGB(...color.emission);
        }
    }
}

class LocalCamera extends THREE.PerspectiveCamera {
    constructor(sceneWrapper) {
        super(50, window.innerWidth / window.innerHeight, 0.1, 1000);
        this.cameraObject = this;
        this.userData.type = 'camera';
        this.userData.uid = -1000;
        this.name = '<<< Local camera >>>';
        this.nameWithOrder = this.name;
        this.layers.mask = 255;

        window.addEventListener('resize', () => {
            this.cameraObject.aspect = window.innerWidth / window.innerHeight;
            this.cameraObject.updateProjectionMatrix();
        });
    }
}

class Camera extends BaseObject {
    constructor(sceneWrapper) {
        super(sceneWrapper);
        this.userData.type = 'camera';

        window.addEventListener('resize', () => {
            this.cameraObject.aspect = window.innerWidth / window.innerHeight;
            this.cameraObject.updateProjectionMatrix();
        });
    }

    init() {
        super.init();
        this.visual;
        this.frustumSegments;
    }

    get cameraObject() {
        for(var c of this.children) {
            if(c instanceof THREE.PerspectiveCamera) return c;
            if(c instanceof THREE.OrthographicCamera) return c;
        }

        if(this.userData.perspective === undefined)
            throw 'Cannot construct a Camera without "perspective" set';

        if(this.userData.perspective)
            var cameraObject = new THREE.PerspectiveCamera(50, window.innerWidth / window.innerHeight, 0.1, 1000);
        else
            var cameraObject = new THREE.OrthographicCamera(window.innerWidth / - 2, window.innerWidth / 2, window.innerHeight / 2, window.innerHeight / - 2, 0.1, 1000);
        cameraObject.name = '';
        cameraObject.userData.type = 'cameraObject';
        cameraObject.position.set(0, 0, 0);
        cameraObject.quaternion.set(0, 1, 0, 0);
        cameraObject.layers.mask = 255;
        this.add(cameraObject);

        this.isPerspectiveCamera = cameraObject instanceof THREE.PerspectiveCamera;
        this.isOrthographicCamera = cameraObject instanceof THREE.OrthographicCamera;
    }

    get visual() {
        for(var c of this.children) {
            if(c.userData.type === 'cameraVisual')
                return c;
        }

        var visual = new CameraVisual(sceneWrapper, this);
        visual.init();
        this.add(visual);
        return visual;
    }

    get frustumSegments() {
        for(var c of this.children) {
            if(c.userData.type === 'frustumSegments')
                return c;
        }

        var frustumSegments = new THREE.LineSegments(
            new THREE.BufferGeometry(),
            new THREE.LineBasicMaterial({color: 0xffffff}),
        );
        frustumSegments.geometry.setAttribute('position', new THREE.BufferAttribute(
            new Float32Array(3 * 4 * 6), 3
        ));
        frustumSegments.userData.type = 'frustumSegments';
        this.add(frustumSegments);
        return frustumSegments;
    }

    update(eventData) {
        super.update(eventData);
        if(eventData.data.perspective !== undefined)
            this.setCameraPerspectiveMode(eventData.data.perspective);
        if(eventData.data.viewAngle !== undefined)
            this.setCameraFOV(eventData.data.viewAngle);
        if(eventData.data.orthoSize !== undefined)
            this.setCameraOrthoSize(eventData.data.orthoSize);
        if(eventData.data.nearClippingPlane !== undefined)
            this.setCameraNear(eventData.data.nearClippingPlane);
        if(eventData.data.farClippingPlane !== undefined)
            this.setCameraFar(eventData.data.farClippingPlane);
        if(eventData.data.color !== undefined)
            this.setCameraColor(eventData.data.color);
        if(eventData.data.frustumVectors !== undefined)
            this.setCameraFrustumVectors(eventData.data.frustumVectors);
        if(eventData.data.showFrustum !== undefined)
            this.setCameraFrustumVisibility(eventData.data.showFrustum);
        if(eventData.data.allowTranslation !== undefined)
            this.userData.enablePan = eventData.data.allowTranslation;
        if(eventData.data.allowRotation !== undefined)
            this.userData.enableRotate = eventData.data.allowRotation;
        if(eventData.data.allowZoom !== undefined)
            this.userData.enableZoom = eventData.data.allowZoom;
    }

    setLayer(layer) {
        super.setLayer(layer);
        this.visual.setLayer(layer);
    }

    setCameraPerspectiveMode(perspective) {
        if(this.userData.perspective !== undefined)
            throw 'Camera "perspective" cannot be changed after creation';
        this.userData.perspective = perspective;
        this.cameraObject;
    }

    setCameraFOV(fovRadians) {
        this.cameraObject.fov = fovRadians * 180 / Math.PI;
        this.cameraObject.updateProjectionMatrix();
    }

    setCameraOrthoSize(orthoSize) {
        var aspectRatio = window.innerWidth / window.innerHeight;
        var width = orthoSize;
        var height = orthoSize / aspectRatio;
        this.cameraObject.left = width / 2;
        this.cameraObject.right = -width / 2;
        this.cameraObject.top = height / 2;
        this.cameraObject.bottom = -height / 2;
        this.cameraObject.updateProjectionMatrix();
    }

    setCameraNear(x) {
        this.cameraObject.near = x;
        this.cameraObject.updateProjectionMatrix();
    }

    setCameraFar(x) {
        this.cameraObject.far = x;
        this.cameraObject.updateProjectionMatrix();
    }

    setCameraColor(color) {
        this.visual.setColor(color);
    }

    setCameraFrustumVectors(frustumVectors) {
        const near = new THREE.Vector3(...frustumVectors.near);
        const far = new THREE.Vector3(...frustumVectors.far);
        const pts = {
            near: [
                new THREE.Vector3(near.x, near.y, near.z),
                new THREE.Vector3(-near.x, near.y, near.z),
                new THREE.Vector3(-near.x, -near.y, near.z),
                new THREE.Vector3(near.x, -near.y, near.z),
            ],
            far: [
                new THREE.Vector3(far.x, far.y, far.z),
                new THREE.Vector3(-far.x, far.y, far.z),
                new THREE.Vector3(-far.x, -far.y, far.z),
                new THREE.Vector3(far.x, -far.y, far.z),
            ],
        };
        const points = [];
        for(var i = 0; i < 4; i++) {
            points.push(pts.near[i]);
            points.push(pts.near[(i + 1) % 4]);
            points.push(pts.far[i]);
            points.push(pts.far[(i + 1) % 4]);
            points.push(pts.near[i]);
            points.push(pts.far[i]);
        }
        for(var i = 0; i < points.length; i++) {
            this.frustumSegments.geometry.attributes.position.array[3 * i + 0] = points[i].x;
            this.frustumSegments.geometry.attributes.position.array[3 * i + 1] = points[i].y;
            this.frustumSegments.geometry.attributes.position.array[3 * i + 2] = points[i].z;
        }
        this.frustumSegments.geometry.attributes.position.needsUpdate = true;
        this.frustumSegments.geometry.computeBoundingBox();
        this.frustumSegments.geometry.computeBoundingSphere();
    }

    setCameraFrustumVisibility(show) {
        this.frustumSegments.visible = show;
    }
}

class Light extends BaseObject {
    constructor(sceneWrapper) {
        super(sceneWrapper);
        this.userData.type = 'light';
    }

    init() {
        super.init();
        this.light;
    }

    get light() {
        for(var c of this.children) {
            if(c.userData.type === 'pointLight')
                return c;
        }

        var light = new THREE.PointLight(0xffffff, 0.1);
        light.castShadow = settings.shadows.enabled;
        light.userData.type = 'pointLight';
        this.add(light);
        return light;
    }

    update(eventData) {
        super.update(eventData);
    }
}

class PointCloud extends BaseObject {
    constructor(sceneWrapper) {
        super(sceneWrapper);
        this.userData.type = 'pointCloud';
    }

    init() {
        super.init();
        this.points;
    }

    get points() {
        for(var c of this.children) {
            if(c.userData.type === 'points')
                return c;
        }

        var points = new THREE.Points(
            new THREE.BufferGeometry(),
            new THREE.PointsMaterial({sizeAttenuation: false, vertexColors: true})
        );
        points.userData.type = 'points';
        points.userData.pickThisIdInstead = this.id;
        this.add(points);
        return points;
    }

    update(eventData) {
        super.update(eventData);
        if(eventData.data.points !== undefined && eventData.data.colors !== undefined)
            this.setPointCloudPoints(eventData.data.points, eventData.data.colors);
        if(eventData.data.pointSize !== undefined)
            this.setPointCloudPointSize(eventData.data.pointSize);
    }

    setLayer(layer) {
        super.setLayer(layer);
        this.points.layers.mask = this.computedLayer();
    }

    setPointCloudPoints(points, colors) {
        this.points.geometry.setAttribute('position', new THREE.Float32BufferAttribute(points, 3));
        this.points.geometry.setAttribute('color', new THREE.Uint8ClampedBufferAttribute(colors, 4, true));
        this.points.geometry.computeBoundingBox();
        this.points.geometry.computeBoundingSphere();
    }

    setPointCloudPointSize(pointSize) {
        this.points.material.size = pointSize;
    }
}

class Octree extends BaseObject {
    constructor(sceneWrapper) {
        super(sceneWrapper);
        this.userData.type = 'octree';
    }

    init() {
        super.init();
        this.mesh;
    }

    get mesh() {
        for(var c of this.children) {
            if(c.userData.type === 'octreeMesh')
                return c;
        }

        var mesh = new THREE.InstancedMesh(
            new THREE.BoxGeometry(1, 1, 1),
            new THREE.MeshPhongMaterial({
                color:    new THREE.Color(1, 1, 1),
                //color:    new THREE.Color(c[0], c[1], c[2]),
                //specular: new THREE.Color(c[3], c[4], c[5]),
                //emissive: new THREE.Color(c[6], c[7], c[8]),
            }),
            settings.octree.maxVoxelCount
        );
        mesh.userData.type = 'octreeMesh';
        mesh.instanceMatrix.setUsage(THREE.DynamicDrawUsage);
        mesh.count = 0;
        mesh.instanceColor = new THREE.InstancedBufferAttribute(new Float32Array(3 * settings.octree.maxVoxelCount), 3);
        this.add(mesh);
        return mesh;
    }

    update(eventData) {
        super.update(eventData);
        if(eventData.data.voxelSize !== undefined)
            this.setOctreeVoxelSize(eventData.data.voxelSize);
        if(eventData.data.voxels !== undefined)
            this.setOctreeVoxels(eventData.data.voxels);
    }

    setLayer(layer) {
        super.setLayer(layer);
        this.mesh.layers.mask = this.computedLayer();
    }

    setOctreeVoxelSize(voxelSize) {
        this.userData.voxelSize = voxelSize;
    }

    setOctreeVoxels(voxels) {
        const p = voxels.positions, c = voxels.colors, s = this.userData.voxelSize;
        var n = 0;
        for(var i = 0, pi = 0, ci = 0; pi < p.length && ci < c.length; i++, pi += 3, ci += 4) {
            this.mesh.setColorAt(i, new THREE.Color(c[ci] / 255, c[ci + 1] / 255, c[ci + 2] / 255));
            var m = new THREE.Matrix4();
            m.makeScale(s, s, s);
            m.setPosition(p[pi], p[pi + 1], p[pi + 2]);
            this.mesh.setMatrixAt(i, m);
            n++;
        }
        this.mesh.count = n;
        this.mesh.instanceMatrix.needsUpdate = true;
        this.mesh.instanceColor.needsUpdate = true;
    }
}

class ForceSensor extends BaseObject {
    constructor(sceneWrapper) {
        super(sceneWrapper);
        this.userData.type = 'forceSensor';
    }

    init() {
        super.init();
        this.sensorFrame;
    }

    get sensorFrame() {
        for(var c of this.children) {
            if(c.userData.type === 'sensorFrame')
                return c;
        }

        var sensorFrame = new THREE.Group();
        sensorFrame.userData.type = 'sensorFrame';
        this.add(sensorFrame);
        return sensorFrame;
    }

    get childObjects() {
        return [...this.sensorFrame.children].filter((o) => o.userData.parentUid === this.userData.uid);
    }

    attach(o) {
        this.sensorFrame.attach(o);
    }

    update(eventData) {
        super.update(eventData);
        if(eventData.data.intrinsicPose !== undefined)
            this.setForceSensorIntrinsicPose(foreventData.dataceSensor.intrinsicPose);
    }

    setForceSensorIntrinsicPose(intrinsicPose) {
        this.sensorFrame.position.set(intrinsicPose[0], intrinsicPose[1], intrinsicPose[2]);
        this.sensorFrame.quaternion.set(intrinsicPose[3], intrinsicPose[4], intrinsicPose[5], intrinsicPose[6]);
    }
}

class Script extends BaseObject {
    constructor(sceneWrapper) {
        super(sceneWrapper);
        this.userData.type = 'script';
    }
}

class DetachedScript extends BaseObject {
    constructor(sceneWrapper) {
        super(sceneWrapper);
        this.userData.type = 'detachedScript';
    }
}

class UnknownObject extends BaseObject {
    constructor(sceneWrapper) {
        super(sceneWrapper);
        this.userData.type = 'unknownObject';
    }
}

class TriangleGeometry extends THREE.BufferGeometry {
	constructor(size = 1) {
		super();
		this.type = 'TriangleGeometry';

		this.parameters = {
			size: size,
		};

		const size_half = size / 2;

		const indices = [];
		const vertices = [];
		const normals = [];
		const uvs = [];

		for(let i = 0; i < 3; i++) {
			const w = i * Math.PI * 2 / 3;
            const x = size_half * Math.cos(w);
            const y = size_half * Math.sin(w);
            vertices.push(-x, y, 0);
            normals.push(0, 0, 1);
            uvs.push((x + size_half) / size);
            uvs.push((y + size_half) / size);
		}
        indices.push(0);

		//this.setIndex(indices);
		this.setAttribute('position', new THREE.Float32BufferAttribute(vertices, 3));
		this.setAttribute('normal', new THREE.Float32BufferAttribute(normals, 3));
		//this.setAttribute('uv', new THREE.Float32BufferAttribute(uvs, 2));
	}

	static fromJSON(data) {
		return new TriangleGeometry(data.size);
	}
}

class DrawingObjectSetOverlayMixin {
    setOverlay(overlay) {
        if(overlay) {
            this.renderOrder = 999;
            this.material.depthTest = false;
            this.material.depthWrite = false;
            this.onBeforeRender = function (renderer) { renderer.clearDepth(); };
        } else {
            this.renderOrder = 0;
            this.material.depthTest = true;
            this.material.depthWrite = true;
            delete this.onBeforeRender;
        }
    }
}

class DrawingObjectVisualBufferGeometryMixin {
    initGeometry() {
        this.geometry.setAttribute('position', new THREE.Float32BufferAttribute(this.userData.maxItemCount * 3 * this.userData.pointsPerItem, 3));
        this.geometry.setAttribute('color', new THREE.Float32BufferAttribute(this.userData.maxItemCount * 3 * this.userData.pointsPerItem, 3));
        this.clear();
    }

    clear() {
        this.geometry.setDrawRange(0, 0);
    }

    updateGeometry() {
        this.geometry.getAttribute('position').needsUpdate = true;
        this.geometry.getAttribute('color').needsUpdate = true;
        this.material.needsUpdate = true;
        this.geometry.computeBoundingBox();
        this.geometry.computeBoundingSphere();
    }

    setPoint(index, point, color, quaternion) {
        if(index >= this.userData.maxItemCount) return;

        const ptsPerItem = this.userData.pointsPerItem;

        const positionAttr = this.geometry.getAttribute('position');
        const colorAttr = this.geometry.getAttribute('color');

        for(var i = 0; i < point.length; i++)
            positionAttr.array[ptsPerItem * 3 * index + i] = point[i];
        //positionAttr.needsUpdate = true; // called later by updateGeometry()

        for(var i = 0; i < color.length; i++)
            colorAttr.array[ptsPerItem * 3 * index + i] = color[i];
        //colorAttr.needsUpdate = true; // called later by updateGeometry()

        this.geometry.setDrawRange(0, Math.max(this.geometry.drawRange.count, ptsPerItem * (index + 1)));
    }
}

mixin(DrawingObjectVisualBufferGeometryMixin, DrawingObjectSetOverlayMixin);

class DrawingObjectVisualPoint extends THREE.Points {
    constructor(maxItemCount, size) {
        super(
            new THREE.BufferGeometry(),
            new THREE.PointsMaterial({size: 0.01 * size, vertexColors: true})
        );
        this.userData.itemType = 'point';
        this.userData.maxItemCount = maxItemCount;
        this.userData.size = size;
        this.userData.pointsPerItem = 1;
        this.initGeometry();
    }

    setSize(size) {
        this.userData.size = size;
        this.material.size = 0.01 * size;
    }
}

mixin(DrawingObjectVisualPoint, DrawingObjectVisualBufferGeometryMixin);

class DrawingObjectVisualLine extends THREE.LineSegments {
    constructor(maxItemCount, size) {
        super(
            new THREE.BufferGeometry(),
            new THREE.LineBasicMaterial({linewidth: 10 * size, vertexColors: true})
        );
        this.userData.itemType = 'line';
        this.userData.maxItemCount = maxItemCount;
        this.userData.size = size;
        this.userData.pointsPerItem = 2;
        this.initGeometry();
    }

    setSize(size) {
        this.userData.size = size;
        this.material.linewidth = 10 * size;
    }
}

mixin(DrawingObjectVisualLine, DrawingObjectVisualBufferGeometryMixin);

class DrawingObjectVisualLineStrip extends THREE.Line {
    constructor(maxItemCount, size) {
        super(
            new THREE.BufferGeometry(),
            new THREE.LineBasicMaterial({linewidth: 10 * size, vertexColors: true})
        );
        this.userData.itemType = 'lineStrip';
        this.userData.maxItemCount = maxItemCount;
        this.userData.size = size;
        this.userData.pointsPerItem = 1;
        this.initGeometry();
    }

    setSize(size) {
        this.userData.size = size;
        this.material.linewidth = 10 * size;
    }
}

mixin(DrawingObjectVisualLineStrip, DrawingObjectVisualBufferGeometryMixin);

class DrawingObjectVisualInstancedMesh extends THREE.InstancedMesh {
    constructor(geometry, material, maxItemCount, size) {
        super(geometry, material, maxItemCount);
        this.userData.maxItemCount = maxItemCount;
        this.userData.size = size;
    }

    setSize(size) {
        this.userData.size = size;
        this.material.linewidth = 10 * size;
    }

    clear() {
        this.count = 0;
    }

    updateGeometry() {
        this.instanceMatrix.needsUpdate = true;
        if(this.instanceColor)
            this.instanceColor.needsUpdate = true;
        this.material.needsUpdate = true;
    }

    setPoint(index, point, color, quaternion) {
        if(index >= this.userData.maxItemCount) return;

        var p = new THREE.Vector3(...point);
        var q = new THREE.Quaternion(...quaternion);
        var s = new THREE.Vector3(1, 1, 1);
        var m = new THREE.Matrix4();
        m.compose(p, q, s);
        this.setMatrixAt(index, m);
        //this.instanceMatrix.needsUpdate = true; // called later by updateGeometry()

        var c = new THREE.Color(...color);
        this.setColorAt(index, c);
        //this.instanceColor.needsUpdate = true; // called later by updateGeometry()

        if(this.count <= index)
            this.count = index + 1;
    }
}

mixin(DrawingObjectVisualInstancedMesh, DrawingObjectSetOverlayMixin);

class DrawingObjectVisualCubePoint extends DrawingObjectVisualInstancedMesh {
    constructor(maxItemCount, size) {
        super(
            new THREE.BoxGeometry(size, size, size),
            new THREE.MeshPhongMaterial({
                color: new THREE.Color(1, 1, 1),
            }),
            maxItemCount,
            size
        );
        this.userData.itemType = 'cubePoint';
    }
}

class DrawingObjectVisualDiscPoint extends DrawingObjectVisualInstancedMesh {
    constructor(maxItemCount, size) {
        super(
            new THREE.CircleGeometry(size, 16),
            new THREE.MeshPhongMaterial({
                color: new THREE.Color(1, 1, 1),
                side: THREE.DoubleSide,
            }),
            maxItemCount
        );
        this.userData.itemType = 'discPoint';
    }
}

class DrawingObjectVisualSpherePoint extends DrawingObjectVisualInstancedMesh {
    constructor(maxItemCount, size) {
        super(
            new THREE.SphereGeometry(size, 16, 8),
            new THREE.MeshPhongMaterial({
                color: new THREE.Color(1, 1, 1),
            }),
            maxItemCount
        );
        this.userData.itemType = 'spherePoint';
    }
}

class DrawingObjectVisualQuadPoint extends DrawingObjectVisualInstancedMesh {
    constructor(maxItemCount, size) {
        super(
            new THREE.PlaneGeometry(size, size),
            new THREE.MeshPhongMaterial({
                color: new THREE.Color(1, 1, 1),
                side: THREE.DoubleSide,
            }),
            maxItemCount
        );
        this.userData.itemType = 'quadPoint';
    }
}

class DrawingObjectVisualTrianglePoint extends DrawingObjectVisualInstancedMesh {
    constructor(maxItemCount, size) {
        super(
            new TriangleGeometry(size),
            new THREE.MeshPhongMaterial({
                color: new THREE.Color(1, 1, 1),
                side: THREE.DoubleSide,
            }),
            maxItemCount
        );
        this.userData.itemType = 'trianglePoint';
    }
}

class DrawingObjectVisualTriangle extends THREE.Mesh {
    constructor(maxItemCount, size) {
        super(
            new THREE.BufferGeometry(),
            new THREE.MeshPhongMaterial({
                side: THREE.DoubleSide,
                vertexColors: true,
            }),
        );
        this.userData.itemType = 'triangle';
        this.userData.maxItemCount = maxItemCount;
        this.userData.size = size;
        this.userData.pointsPerItem = 3;
        this.initGeometry();
    }
}

mixin(DrawingObjectVisualTriangle, DrawingObjectVisualBufferGeometryMixin);

class DrawingObject extends THREE.Group {
    static objectsByUid = {};

    static getObjectByUid(uid) {
        return this.objectsByUid[uid];
    }

    constructor(sceneWrapper) {
        super();
        this.name = 'drawingObject';
        this.sceneWrapper = sceneWrapper;
        this.userData.type = 'drawingObject';
    }

    get object() {
        for(var c of this.children) {
            if(c.userData.type === 'drawingObjectVisual')
                return c;
        }

        if(this.userData.itemType === undefined)
            return;

        if(this.userData.itemType == 'point') {
            var object = new DrawingObjectVisualPoint(this.userData.maxItemCount, this.userData.size);
        } else if(this.userData.itemType == 'line') {
            var object = new DrawingObjectVisualLine(this.userData.maxItemCount, this.userData.size);
        } else if(this.userData.itemType == 'lineStrip') {
            var object = new DrawingObjectVisualLineStrip(this.userData.maxItemCount, this.userData.size);
        } else if(this.userData.itemType == 'cubePoint') {
            var object = new DrawingObjectVisualCubePoint(this.userData.maxItemCount, this.userData.size);
        } else if(this.userData.itemType == 'discPoint') {
            var object = new DrawingObjectVisualDiscPoint(this.userData.maxItemCount, this.userData.size);
        } else if(this.userData.itemType == 'spherePoint') {
            var object = new DrawingObjectVisualSpherePoint(this.userData.maxItemCount, this.userData.size);
        } else if(this.userData.itemType == 'quadPoint') {
            var object = new DrawingObjectVisualQuadPoint(this.userData.maxItemCount, this.userData.size);
        } else if(this.userData.itemType == 'trianglePoint') {
            var object = new DrawingObjectVisualTrianglePoint(this.userData.maxItemCount, this.userData.size);
        } else if(this.userData.itemType == 'triangle') {
            var object = new DrawingObjectVisualTriangle(this.userData.maxItemCount, this.userData.size);
        } else {
            throw `Drawing object of type "${this.userData.itemType}" is not supported`;
        }
        object.name = 'drawingObjectVisual';
        object.userData.type = 'drawingObjectVisual';
        this.add(object);
        return object;
    }

    clone(recursive) {
        var obj = new this.constructor(this.sceneWrapper).copy(this, recursive);
        return obj;
    }

    update(eventData) {
        if(eventData.uid !== undefined)
            this.setUid(eventData.uid);
        if(eventData.data === undefined)
            return;
        if(eventData.data.maxCnt !== undefined)
            this.setMaxItemCount(eventData.data.maxCnt);
        if(eventData.data.size !== undefined)
            this.setSize(eventData.data.size);
        if(eventData.data.parentUid !== undefined)
            this.setParent(eventData.data.parentUid);
        if(eventData.data.color !== undefined)
            this.setColor(eventData.data.color);
        if(eventData.data.cyclic !== undefined)
            this.setCyclic(eventData.data.cyclic);
        if(eventData.data.type !== undefined)
            this.setItemType(eventData.data.type);
        if(eventData.data.overlay !== undefined)
            this.setOverlay(eventData.data.overlay);
        if(eventData.data.points !== undefined || eventData.data.clearPoints === true)
            this.setPoints(
                eventData.data.points || [],
                eventData.data.colors || [],
                eventData.data.quaternions || [],
                !!eventData.data.clearPoints
            );
    }

    setItemType(itemType) {
        if(this.userData.itemType !== undefined)
            return;

        if(this.userData.maxItemCount === undefined)
            throw "maxItemCount must be set before calling setItemType()";

        if(this.userData.size === undefined)
            throw "size must be set before calling setItemType()";

        this.userData.itemType = itemType;

        // invoke getter now:
        this.object;
    }

    setOverlay(overlay) {
        this.object.setOverlay(overlay);
    }

    pointsPerItem() {
        if(this.userData.itemType == 'line')
            return 2;
        if(this.userData.itemType == 'triangle')
            return 3;
        return 1;
    }

    setUid(uid) {
        if(this.userData.uid !== undefined)
            return;
        this.userData.uid = uid;
        DrawingObject.objectsByUid[uid] = this;
    }

    setParent(parentUid) {
        this.userData.parentUid = parentUid;
        var parentObj = BaseObject.getObjectByUid(parentUid);
        if(parentObj !== undefined) {
            var p = this.position.clone();
            var q = this.quaternion.clone();
            parentObj.attach(this);
            this.position.copy(p);
            this.quaternion.copy(q);
        } else /*if(parentUid === -1)*/ {
            if(parentUid !== -1)
                console.error(`Parent with uid=${parentUid} is not known`);
            this.sceneWrapper.scene.attach(this);
        }
    }

    setColor(color) {
        this.userData.color = color;
    }

    setCyclic(cyclic) {
        this.userData.cyclic = cyclic;
    }

    setMaxItemCount(maxItemCount) {
        if(maxItemCount <= 0) {
            const defaultMaxItemCount = 10000;
            console.warn(`DrawingObject: maxItemCount=${maxItemCount} is not valid. Changing to ${defaultMaxItemCount}.`);
            maxItemCount = defaultMaxItemCount;
        }

        this.userData.maxItemCount = maxItemCount;
        this.userData.writeIndex = 0;
    }

    setSize(size) {
        this.userData.size = size;
    }

    setPoints(points, colors, quaternions, clear) {
        if(clear) {
            this.object.clear();
            this.userData.writeIndex = 0;
        }

        const itemLen = this.pointsPerItem() * 3;

        if(points.length % itemLen > 0)
            throw `Points data size is not a multiple of ${itemLen}`;

        const n = points.length / itemLen;

        if(colors.length != points.length)
            throw `Colors data size does not match points data siize`;

        var o = this.object;
        for(var j = 0; j < n; j++) {
            o.setPoint(
                this.userData.writeIndex,
                points.slice(j * itemLen, (j + 1) * itemLen),
                colors.slice(j * itemLen, (j + 1) * itemLen),
                quaternions.slice(j * 4, (j + 1) * 4)
            );

            this.userData.writeIndex++;
            if(this.userData.cyclic)
                this.userData.writeIndex = this.userData.writeIndex % this.userData.maxItemCount;
            else
                this.userData.writeIndex = Math.min(this.userData.maxItemCount, this.userData.writeIndex);
        }

        this.object.updateGeometry();
    }
}

class BoxHelper extends THREE.LineSegments {
    constructor(color = 0xffffff) {
        const geometry = new THREE.BufferGeometry();
        geometry.setAttribute('position', new THREE.BufferAttribute(new Float32Array(24 * 3), 3));
        super(geometry, new THREE.LineDashedMaterial({
            color: color,
            toneMapped: false,
            dashSize: 0.005,
            gapSize: 0.005,
        }));
        this.type = 'BoxHelper';
        this.matrixAutoUpdate = false;
        this.box = new THREE.Mesh(
            new THREE.BoxGeometry(1, 1, 1),
            new THREE.MeshBasicMaterial({
                color: 0xffffff,
                transparent: true,
                opacity: 0.1,
                side: THREE.BackSide,
                depthWrite: false,
            }),
        );
        this.add(this.box);
        this.blinkInterval = null;
    }

    setFromObject(object) {
        if(this.blinkInterval !== null) {
            clearInterval(this.blinkInterval);
            this.blinkInterval = null;
        }
        if(object === null)
            return;
        var bb = [[Infinity, Infinity, Infinity], [-Infinity, -Infinity, -Infinity]];
        var modelBaseMatrixWorld = new THREE.Matrix4(); // identity
        var modelBaseMatrixWorldInverse = new THREE.Matrix4(); // identity

        object.updateMatrixWorld();
        modelBaseMatrixWorld = object.matrixWorld.clone();
        if(object.userData.bbPose !== undefined) {
            const pose = object.userData.bbPose;
            modelBaseMatrixWorld.multiply(
                new THREE.Matrix4().compose(
                    new THREE.Vector3(...pose.slice(0, 3)),
                    new THREE.Quaternion(...pose.slice(3)),
                    new THREE.Vector3(1, 1, 1),
                )
            );
        }
        modelBaseMatrixWorldInverse = modelBaseMatrixWorld.clone().invert();

        for(var o of object.boundingBoxObjects) {
            if(o.userData.bbPose === undefined)
                continue;
            if(o.userData.bbHSize === undefined)
                continue;
            o.updateMatrixWorld();
            for(const dx of [-1, 1]) {
                for(const dy of [-1, 1]) {
                    for(const dz of [-1, 1]) {
                        var v = new THREE.Vector3(
                            dx * o.userData.bbHSize[0],
                            dy * o.userData.bbHSize[1],
                            dz * o.userData.bbHSize[2]
                        );
                        v.applyMatrix4(
                            new THREE.Matrix4().compose(
                                new THREE.Vector3(...o.userData.bbPose.slice(0, 3)),
                                new THREE.Quaternion(...o.userData.bbPose.slice(3)),
                                new THREE.Vector3(1, 1, 1),
                            )
                        );
                        v = o.localToWorld(v);
                        v.applyMatrix4(modelBaseMatrixWorldInverse);
                        var a = v.toArray();
                        for(var h = 0; h < 3; h++) {
                            bb[0][h] = Math.min(bb[0][h], a[h]);
                            bb[1][h] = Math.max(bb[1][h], a[h]);
                        }
                    }
                }
            }
        }

        // grow bbox for better visibility:
        const kGrow = 1.1;
        for(var i = 0; i < 3; i++) {
            var mean = (bb[1][i] + bb[0][i]) / 2;
            var halfRange = (bb[1][i] - bb[0][i]) / 2;
            bb[0][i] = mean - halfRange * kGrow;
            bb[1][i] = mean + halfRange * kGrow;
        }

        if(object.userData.modelBase) {
            this.box.visible = settings.selection.style.boundingBoxModelSolidOpacity > 0.01;
            this.box.material.opacity = settings.selection.style.boundingBoxModelSolidOpacity;
            this.box.material.side = settings.selection.style.boundingBoxModelSolidSide;
        } else {
            this.box.visible = settings.selection.style.boundingBoxSolidOpacity > 0.01;
            this.box.material.opacity = settings.selection.style.boundingBoxSolidOpacity;
            this.box.material.side = settings.selection.style.boundingBoxSolidSide;
        }
        const dash = settings.selection.style.boundingBoxModelDashed;
        this.material.dashSize = dash && object.userData.modelBase ? 0.005 : 1000;
        this.material.gapSize = dash && object.userData.modelBase ? 0.005 : 0;

        this.matrix.copy(modelBaseMatrixWorld);

        const idxMinMax = [1, 1, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 1, 1, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0];
        var p = [];
        for(var j = 0; j < idxMinMax.length; j++)
            p.push(bb[idxMinMax[j]][j % 3]);
        var k = 0;
        for(var idxPt of [0, 1, 1, 2, 2, 3, 3, 0, 4, 5, 5, 6, 6, 7, 7, 4, 0, 4, 1, 5, 2, 6, 3, 7]) {
            for(var j = 0; j < 3; j++)
                this.geometry.attributes.position.array[k++] = p[idxPt * 3 + j];
        }
        this.geometry.attributes.position.needsUpdate = true;
        this.geometry.computeBoundingSphere();
        this.computeLineDistances();
        this.box.position.set(
            (bb[1][0] + bb[0][0]) / 2,
            (bb[1][1] + bb[0][1]) / 2,
            (bb[1][2] + bb[0][2]) / 2
        );
        this.box.scale.set(
            bb[1][0] - bb[0][0],
            bb[1][1] - bb[0][1],
            bb[1][2] - bb[0][2]
        );
        this.material.visible = true;
        if(settings.selection.style.boundingBoxBlinkInterval > 0) {
            setInterval(() => {
                this.material.visible = !this.material.visible;
                render();
            }, settings.selection.style.boundingBoxBlinkInterval);
        }
    }

    copy(source) {
        LineSegments.prototype.copy.call( this, source );
        //this.object = source.object;
        return this;
    }
}

class SceneWrapper {
    constructor() {
        this.scene = new THREE.Scene();

        const ambientLight = new THREE.AmbientLight(0xffffff, 0.6);
        this.scene.add(ambientLight);

        this.raycaster = new THREE.Raycaster();

        this.cameraFacingObjects = [];
    }

    clear() {
        for(var uid in BaseObject.objectsByUid) {
            BaseObject.objectsByUid[uid].removeFromParent();
            delete BaseObject.objectsByUid[uid];
        }
        for(var uid in DrawingObject.objectsByUid) {
            DrawingObject.objectsByUid[uid].removeFromParent();
            delete DrawingObject.objectsByUid[uid];
        }
    }

    addObject(eventData) {
        var obj = null;
        switch(eventData.data.objectType) {
        case 'shape':
            obj = new Shape(this);
            break;
        case 'joint':
            obj = new Joint(this);
            break;
        case 'dummy':
            obj = new Dummy(this);
            break;
        case 'camera':
            obj = new Camera(this);
            break;
        case 'light':
            obj = new Light(this);
            break;
        case 'pointCloud':
            obj = new PointCloud(this);
            break;
        case 'octree':
            obj = new Octree(this);
            break;
        case 'forceSensor':
            obj = new ForceSensor(this);
            break;
        case 'mesh':
            obj = new Mesh(this);
            break;
        case 'script':
            obj = new Script(this);
            break;
        case 'detachedScript':
            obj = new DetachedScript(this);
            break;
        default:
            console.warn(`unhandled object type: "${eventData.data.objectType}"`);
            obj = new UnknownObject(this);
            break;
        }
        obj.init();
        this.scene.add(obj);
        obj.update(eventData);
    }

    getObjectByUid(uid) {
        return BaseObject.getObjectByUid(uid);
    }

    getObjectByName(name) {
        return this.scene.getObjectByName(name);
    }

    removeObject(obj) {
        obj.removeFromParent();
        delete BaseObject.objectsByUid[obj.userData.uid];
    }

    addDrawingObject(eventData) {
        var obj = new DrawingObject(this);
        this.scene.add(obj);
        obj.update(eventData);
    }

    removeDrawingObject(obj) {
        obj.removeFromParent();
        delete DrawingObject.objectsByUid[obj.userData.uid];
    }

    setSceneData(eventData) {
        if(eventData.data.sceneUid !== undefined)
            this.setSceneUid(eventData.data.sceneUid);
        if(eventData.data.visibilityLayers !== undefined)
            this.setSceneVisibilityLayers(eventData.data.visibilityLayers);
    }

    setSceneUid(uid) {
        this.scene.userData.uid = uid;
        // change in scene uid means scene was switched -> clear
        this.clear();
    }

    setSceneVisibilityLayers(visibilityLayers) {
        this.scene.userData.visibilityLayers = visibilityLayers;
    }

    isObjectPickable(obj) {
        if(obj.visible === false)
            return null;
        if(obj.userData.clickInvisible === true)
            return null;
        if(obj.userData.pickThisIdInstead !== undefined) {
            var otherObj = this.scene.getObjectById(obj.userData.pickThisIdInstead);
            if(otherObj === undefined) {
                console.error(`Object uid=${obj.userData.uid}/id=${obj.id} has pickThisIdInstead=${obj.userData.pickThisIdInstead} which doesn't exist`);
                return null;
            }
            return this.isObjectPickable(otherObj);
        } else if(obj.userData.uid !== undefined) {
            return obj;
        }
        return null;
    }

    rayCast(camera, mousePos) {
        if(camera.cameraObject === undefined) {
            throw 'SceneWrapper.rayCast: camera must be a Camera or LocalCamera instance';
        }
        if(mousePos.x < -1 || mousePos.x > 1 || mousePos.y < -1 || mousePos.y > 1) {
            throw 'SceneWrapper.rayCast: x and y must be in normalized device coordinates (-1...+1)';
        }
        this.raycaster.layers.mask = camera.cameraObject.layers.mask;
        this.raycaster.setFromCamera(mousePos, camera.cameraObject);
        return this.raycaster.ray;
    }

    pickObject(camera, mousePos, cond) {
        if(camera.cameraObject === undefined) {
            throw 'SceneWrapper.pickObject: camera must be a Camera or LocalCamera instance';
        }
        if(mousePos.x < -1 || mousePos.x > 1 || mousePos.y < -1 || mousePos.y > 1) {
            throw 'SceneWrapper.pickObject: x and y must be in normalized device coordinates (-1...+1)';
        }
        this.raycaster.layers.mask = camera.cameraObject.layers.mask;
        this.raycaster.setFromCamera(mousePos, camera.cameraObject);
        const intersects = this.raycaster.intersectObjects(this.scene.children, true);
        for(let i = 0; i < intersects.length; i++) {
            var x = intersects[i];
            // XXX: discard some specific types:
            if(['Line', 'LineSegments', 'AxesHelper', 'BoxHelper'].includes(x.object.type))
                continue;
            if(x.object instanceof DrawingObject)
                continue;
            // XXX end
            var obj = this.isObjectPickable(x.object);
            if(obj !== null && (cond === undefined || cond(obj))) {
                if(obj instanceof PointCloud)
                    continue;
                return {
                    distance: x.distance,
                    point: x.point,
                    face: x.face,
                    faceIndex: x.faceIndex,
                    object: obj,
                    originalObject: x.object,
                    ray: {origin: this.raycaster.ray.origin, direction: this.raycaster.ray.direction},
                };
            }
        }
        return null;
    }

    findModelBase(obj, followSMBI) {
        if(obj === null) return null;
        if(obj.userData.modelBase) {
            if(obj.userData.selectModelBase) {
                var obj1 = this.findModelBase(obj.parent);
                if(obj1 !== null) return obj1;
            }
            return obj;
        } else {
            return this.findModelBase(obj.parent);
        }
    }
}

mixin(SceneWrapper, EventSourceMixin);

class SelectSurfacePointTool {
    constructor(sceneWrapper, view) {
        this.sceneWrapper = sceneWrapper;
        this.view = view;
        this.enabled = false;
        this.confirmed = false;

        this.selectPointSphere = new THREE.Mesh(
            new THREE.SphereGeometry(0.01, 8, 4),
            new THREE.MeshBasicMaterial({color: 0xff0000})
        );
        this.selectPointSphere.visible = false;
        this.sceneWrapper.scene.add(this.selectPointSphere);

        this.selectPointArrow = new THREE.ArrowHelper(
            new THREE.Vector3(0, 0, 1),
            new THREE.Vector3(0, 0, 0),
            0.2,
            0xff0000
        );
        this.selectPointArrow.visible = false;
        this.sceneWrapper.scene.add(this.selectPointArrow);
    }

    enable() {
        if(this.enabled) return;
        this.enabled = true;
        this.confirmed = false;
        this.view.requestRender();
    }

    disable() {
        if(!this.enabled) return;
        this.enabled = false;
        if(!this.confirmed) {
            this.selectPointSphere.visible = false;
            this.selectPointArrow.visible = false;
        }
        this.view.requestRender();
    }

    onRender(camera, mouse) {
        if(!this.enabled) return true;
        var pick = this.sceneWrapper.pickObject(camera, mouse.normPos);
        if(pick === null) return true;
        pick.originalObject.updateMatrixWorld();
        this.selectPointSphere.position.copy(pick.point);
        this.selectPointSphere.visible = true;
        this.selectPointSphere.userData.ray = pick.ray;
        // normal is local, convert it to global:
        var normalMatrix = new THREE.Matrix3().getNormalMatrix(pick.originalObject.matrixWorld);
        if(pick.face) {
            var normal = pick.face.normal.clone().applyMatrix3(normalMatrix).normalize();
            this.selectPointArrow.setDirection(normal);
        } else {
            this.selectPointArrow.setDirection(new THREE.Vector3(0, 0, 1));
        }
        this.selectPointArrow.position.copy(pick.point);
        this.selectPointArrow.visible = true;
        return true;
    }

    onClick(event) {
        if(!this.enabled) return true;

        this.confirmed = true;
        this.disable();

        var p = new THREE.Vector3();
        p.copy(this.selectPointSphere.position);

        var q = new THREE.Quaternion();
        this.selectPointArrow.getWorldQuaternion(q);

        var r = this.selectPointSphere.userData.ray;

        this.dispatchEvent('selectedPoint', {quaternion: q, position: p, ray: r});
        return false;
    }

    onMouseMove(event) {
        if(this.enabled)
            this.view.requestRender();
        return true;
    }
}

mixin(SelectSurfacePointTool, EventSourceMixin);

class RayCastTool {
    constructor(sceneWrapper, view) {
        this.sceneWrapper = sceneWrapper;
        this.view = view;
        this.enabled = false;
        this.ray = {origin: [0, 0, 0], direction: [0, 0, -1]};
    }

    enable() {
        if(this.enabled) return;
        this.enabled = true;
        this.view.requestRender();
        notifyEvent({event: 'rayCastEnter'});
        this.onRender(this.view.selectedCamera, this.view.mouse);
        this.onMouseMove();
    }

    disable() {
        if(!this.enabled) return;
        this.enabled = false;
        this.view.requestRender();
        notifyEvent({event: 'rayCastLeave'});
    }

    onRender(camera, mouse) {
        if(!this.enabled) return true;
        var ray = this.sceneWrapper.rayCast(camera, mouse.normPos);
        this.ray.origin = ray.origin.toArray();
        this.ray.direction = ray.direction.toArray();
        return true;
    }

    onClick(event) {
        if(!this.enabled) return true;

        notifyEvent({
            event: 'rayCast',
            data: {
                ray: this.ray,
                eventSource: 'click',
            },
        });

        this.disable();
        return false;
    }

    onMouseMove(event) {
        if(this.enabled) {
            this.view.requestRender();
            notifyEvent({
                event: 'rayCast',
                data: {
                    ray: this.ray,
                    eventSource: 'mousemove',
                },
            });
        }
        return true;
    }
}

mixin(RayCastTool, EventSourceMixin);

class HoverTool {
    constructor(sceneWrapper, view) {
        this.sceneWrapper = sceneWrapper;
        this.view = view;
        this.timeoutId = null;
    }

    onMouseMove(event, camera, mouse) {
        if(this.timeoutId) {
            window.clearTimeout(this.timeoutId);
            this.timeoutId = null;
        }

        if(settings.hoverTool.timeoutMs >= 0) {
            this.timeoutId = window.setTimeout(() => {
                var ray = this.sceneWrapper.rayCast(camera, mouse.normPos);
                var eventData = {
                    ray: {
                        origin: ray.origin.toArray(),
                        direction: ray.direction.toArray()
                    },
                    eventSource: 'mouseover',
                };
                if(settings.hoverTool.pick) {
                    var pick = this.sceneWrapper.pickObject(camera, mouse.normPos);
                    if(pick !== null) {
                        eventData.pick = {
                            distance: pick.distance,
                            point: pick.point.toArray(),
                            object: pick.object.userData.uid,
                        };
                    }
                }
                notifyEvent({event: 'rayCast', data: eventData});
            }, settings.hoverTool.timeoutMs);
        }

        return true;
    }
}

mixin(HoverTool, EventSourceMixin);

class View {
    constructor(viewCanvas, sceneWrapper) {
        this.viewCanvas = viewCanvas
        this.sceneWrapper = sceneWrapper;
        this.renderer = new THREE.WebGLRenderer({canvas: this.viewCanvas, alpha: true,preserveDrawingBuffer: true});
        this.renderer.shadowMap.enabled = settings.shadows.enabled;
        this.renderer.setPixelRatio(window.devicePixelRatio);
        this.renderer.setSize(window.innerWidth, window.innerHeight);
        if(settings.background.clearColor)
            this.renderer.setClearColor(settings.background.clearColor, settings.background.clearColorAlpha || 1);
        this.renderRequested = false;

        this.defaultCamera = new LocalCamera();
        this.defaultCamera.position.set(1.12, -1.9, 1.08);
        this.defaultCamera.quaternion.set(-0.21233689785003662, 0.7820487022399902, 0.5654570460319519, -0.15352927148342133);
        this.sceneWrapper.scene.add(this.defaultCamera);

        this.selectedCamera = this.defaultCamera;

        this.bboxNeedsUpdating = false;
        this.bboxHelper = new BoxHelper(0xffffff);
        this.bboxHelper.visible = false;
        this.sceneWrapper.scene.add(this.bboxHelper);

        this.selectPointTool = new SelectSurfacePointTool(this.sceneWrapper, this);
        this.rayCastTool = new RayCastTool(this.sceneWrapper, this);
        this.hoverTool = new HoverTool(this.sceneWrapper, this);

        this.selectedObject = null;

        this.mouse = {
            dragStart: {x: 0, y: 0},
            dragDistance: (event) => {
                return Math.hypot(
                    this.mouse.pos.x - this.mouse.dragStart.x,
                    this.mouse.pos.y - this.mouse.dragStart.y
                );
            },
            pos: {x: 0, y: 0},
            normPos: {x: 0, y: 0},
            clickDragTolerance: 1
        };

        this.viewCanvas.addEventListener('mousedown', (e) => {this.onMouseDown(e);}, false);
        this.viewCanvas.addEventListener('mouseup', (e) => {this.onMouseUp(e);}, false);
        this.viewCanvas.addEventListener('mousemove', (e) => {this.onMouseMove(e);}, false);

        this.composer = new THREE.EffectComposer(this.renderer);

        this.renderPass = new THREE.RenderPass(this.sceneWrapper.scene, this.selectedCamera.cameraObject);
        this.composer.addPass(this.renderPass);

        this.outlinePass = new THREE.OutlinePass(new THREE.Vector2(window.innerWidth, window.innerHeight), this.sceneWrapper.scene, this.selectedCamera.cameraObject);
        this.outlinePass.visibleEdgeColor.set(0x0000ff);
        this.outlinePass.hiddenEdgeColor.set(0x0000ff);
        this.outlinePass.edgeGlow = 0;
        this.outlinePass.edgeThickness = 1;
        this.outlinePass.edgeStrength = 5;
        this.outlinePass.pulsePeriod = 0;
        this.composer.addPass(this.outlinePass);

        window.addEventListener('resize', () => {
            var w = window.innerWidth;
            var h = window.innerHeight;
            this.renderer.setSize(w, h);
            this.composer.setSize(w, h);
            this.requestRender();
        });
    }

    setSelectedCamera(camera) {
        if(camera.cameraObject === undefined)
            throw 'must be a Camera or LocalCamera instance';

        // disable orbit controls otherwise it would mess with camera's pose:
        orbitControlsWrapper.disable();

        this.selectedCamera = camera;

        this.renderPass.camera = this.selectedCamera.cameraObject;

        this.outlinePass.renderCamera = this.selectedCamera.cameraObject;

        orbitControlsWrapper.setCamera(camera);
        // XXX: CAMERA ISSUES WTF READ THIS
        //    CoppeliaSim and three.js cameras have opposite Z axis;
        //    because of that, we have to put the three.js camera as child
        //    of another (THREE.Group) object, rotated 180 deg on its Y;
        //    but this way, OrbitControls isn't able to manipulate camera parent
        //    properly;
        //    additionally OrbitControls can't properly manipulate a non-parentless object
        //    so our possibilities are very restricted here;
        //    therefore we disable camera manipulation for remote cameras:
        orbitControlsWrapper.setEnabled(camera instanceof LocalCamera);
        orbitControlsWrapper.setManipulationPermissions(
            camera.userData.enablePan ?? true,
            camera.userData.enableRotate ?? true,
            camera.userData.enableZoom ?? true
        );

        transformControlsWrapper.setCamera(camera);

        this.requestRender();

        this.dispatchEvent('selectedCameraChanged', {});
    }

    /*
    getCameraPose() {
        return [
            ...this.selectedCamera.position.toArray(),
            ...this.selectedCamera.quaternion.toArray()
        ];
    }

    setCameraPose(pose) {
        this.dispatchEvent('cameraPoseChanging', {oldPose: this.getCameraPose(), newPose: pose});

        this.selectedCamera.position.set(pose[0], pose[1], pose[2]);
        this.selectedCamera.quaternion.set(pose[3], pose[4], pose[5], pose[6]);

        this.dispatchEvent('cameraPoseChanged', {});
    }
    */

    fitCameraToSelection(selection, camera, controls, fitOffset = 1.2) {
        const box = new THREE.Box3();
        for(const object of selection) {
            if(object.boundingBoxPoints !== undefined) {
                for(const p of object.boundingBoxPoints) {
                    box.expandByPoint(p);
                }
            }
        }
        const size = box.getSize(new THREE.Vector3());
        const center = box.getCenter(new THREE.Vector3());
        const maxSize = Math.max(size.x, size.y, size.z);
        if(maxSize < 0.01) {
            window.alert('Nothing to show!');
            return;
        }
        const fitHeightDistance = maxSize / (2 * Math.atan(Math.PI * camera.fov / 360));
        const fitWidthDistance = fitHeightDistance / camera.aspect;
        const distance = fitOffset * Math.max(fitHeightDistance, fitWidthDistance);
        const direction = controls.target.clone()
            .sub(camera.position)
            .normalize()
            .multiplyScalar(distance);

        //controls.maxDistance = distance * 10;
        controls.target.copy(center);

        //camera.setCameraNear(distance / 100);
        //camera.setCameraFar(distance * 100);

        camera.position.copy(controls.target).sub(direction);

        controls.update();
    }

    setSelectedObject(obj, followSMBI) {
        if(obj === undefined) obj = null;

        var previous = this.selectedObject;

        if(obj == null) {
            this.bboxHelper.visible = false;
            this.selectedObject = null;
        } else if(obj.userData.selectable !== false) {
            if(followSMBI && obj.userData.selectModelBase) {
                var modelBase = sceneWrapper.findModelBase(obj);
                if(modelBase !== null)
                    obj = modelBase;
            }

            debug(`id=${obj.id}, uid=${obj.userData.uid}, path=${obj.path}`);
            this.selectedObject = obj;
            this.requestBoundingBoxUpdate();
            this.bboxHelper.visible = settings.selection.style.boundingBox;
            this.bboxHelper.renderOrder = settings.selection.style.boundingBoxOnTop ? 1000 : 0;
            this.bboxHelper.material.depthTest = !settings.selection.style.boundingBoxOnTop;
        }

        var current = this.selectedObject;
        this.dispatchEvent('selectedObjectChanged', {previous, current});

        if(settings.selection.style.outline)
            this.outlinePass.selectedObjects = this.selectedObject === null ? [] : [this.selectedObject];
    }

    isPartOfSelection(obj) {
        if(this.selectedObject === null) return false;
        if(this.selectedObject === obj) return true;
        return obj.parent === null ? false : this.isPartOfSelection(obj.parent);
    }

    requestBoundingBoxUpdate() {
        this.bboxNeedsUpdating = true;
        this.requestRender();
    }

    updateBoundingBoxIfNeeded() {
        if(this.bboxNeedsUpdating) {
            this.bboxNeedsUpdating = false;
            this.bboxHelper.setFromObject(this.selectedObject);
        }
    }

    readMousePos(event) {
        this.mouse.pos.x = event.clientX;
        this.mouse.pos.y = event.clientY;
        this.mouse.normPos.x = (event.clientX / window.innerWidth) * 2 - 1;
        this.mouse.normPos.y = -(event.clientY / window.innerHeight) * 2 + 1;
    }

    onMouseDown(event) {
        this.readMousePos(event);
        this.mouse.dragStart.x = event.clientX;
        this.mouse.dragStart.y = event.clientY;
    }

    onMouseUp(event) {
        this.readMousePos(event);
        if(this.mouse.dragDistance() <= this.mouse.clickDragTolerance)
            this.onClick(event);
    }

    onClick(event) {
        if(!this.rayCastTool.onClick(event))
            return;
        if(!this.selectPointTool.onClick(event))
            return;

        var pick = this.sceneWrapper.pickObject(this.selectedCamera, this.mouse.normPos, (o) => o.userData.selectable !== false);
        view.setSelectedObject(pick === null ? null : pick.object, true);
    }

    onMouseMove(event) {
        this.readMousePos(event);

        if(!this.rayCastTool.onMouseMove(event))
            return;
        if(!this.selectPointTool.onMouseMove(event))
            return;
        if(!this.hoverTool.onMouseMove(event, this.selectedCamera, this.mouse))
            return;
    }

    requestRender() {
        this.renderRequested = true;
    }

    render() {
        if(!this.renderRequested) return;
        this.renderRequested = false;

        this.updateBoundingBoxIfNeeded();

        if(!this.rayCastTool.onRender(this.selectedCamera, this.mouse))
            return;
        if(!this.selectPointTool.onRender(this.selectedCamera, this.mouse))
            return;

        // orient camera-facing objects:
        for(var o of this.sceneWrapper.cameraFacingObjects)
            o.lookAt(this.selectedCamera.position);

        if(settings.selection.style.outline)
            this.composer.render();
        else
            this.renderer.render(this.sceneWrapper.scene, this.selectedCamera.cameraObject);
    }
}

mixin(View, EventSourceMixin);

class AxesView {
    constructor(axesCanvas, upVector) {
        this.axesScene = new THREE.Scene();
        this.axesHelper = new THREE.AxesHelper(20);
        this.axesScene.add(this.axesHelper);
        this.axesRenderer = new THREE.WebGLRenderer({canvas: axesCanvas, alpha: true});
        this.axesRenderer.setPixelRatio(window.devicePixelRatio);
        this.axesRenderer.setSize(80, 80);
        this.renderRequested = false;
        this.axesCamera = new THREE.PerspectiveCamera(40, axesCanvas.width / axesCanvas.height, 1, 1000);
        this.axesCamera.up = upVector;
        this.axesScene.add(this.axesCamera);
    }

    requestRender() {
        this.renderRequested = true;
    }

    render(cameraPosition, targetPosition) {
        if(!this.renderRequested) return;
        this.renderRequested = false;

        this.axesCamera.position.subVectors(cameraPosition, targetPosition);
        this.axesCamera.position.setLength(50);
        this.axesCamera.lookAt(this.axesScene.position);
        this.axesRenderer.render(this.axesScene, this.axesCamera);
    }
}

class OrbitControlsWrapper {
    constructor(camera, renderer, renderFunc) {
        this.orbitControls = new THREE.OrbitControls(camera, renderer.domElement);
        this.orbitControls.minDistance = 0.5;
        this.renderFunc = renderFunc;
        this.orbitControls.addEventListener('change', (event) => {
            this.renderFunc();
        });
        if(camera.parent !== sceneWrapper.scene) this.disable();
    }

    setCamera(camera) {
        if(!this.orbitControls) return;
        this.orbitControls.object = camera;
        if(camera.parent !== sceneWrapper.scene) this.disable();
        else this.update();
    }

    getTarget() {
        if(!this.orbitControls) return new THREE.Vector3(0, 0, 0);
        return this.orbitControls.target;
    }

    setTarget(target) {
        if(!this.orbitControls) return;
        this.orbitControls.target.copy(target);
        this.update();
    }

    setScreenSpacePanning(screenSpacePanning) {
        if(!this.orbitControls) return;
        this.orbitControls.screenSpacePanning = screenSpacePanning;
    }

    enable() {
        return this.setEnabled(true);
    }

    disable() {
        return this.setEnabled(false);
    }

    setEnabled(enabled) {
        if(!this.orbitControls) return;
        var oldEnabled = this.orbitControls.enabled;
        this.orbitControls.enabled = enabled;
        this.update();
        return oldEnabled;
    }

    setManipulationPermissions(pan, rotate, zoom) {
        if(!this.orbitControls) return;
        this.orbitControls.enablePan = pan;
        this.orbitControls.enableRotate = rotate;
        this.orbitControls.enableZoom = zoom;
    }

    addEventListener(eventName, func) {
        if(!this.orbitControls) return;
        this.orbitControls.addEventListener(eventName, func);
    }

    update() {
        if(!this.orbitControls) return;
        if(this.orbitControls.enabled)
            this.orbitControls.update();
    }
}

class TransformControlsWrapper {
    constructor(sceneWrapper, camera, renderer) {
        this.sceneWrapper = sceneWrapper;
        this.transformControls = new THREE.TransformControls(camera.cameraObject, renderer.domElement);
        this.transformControls.enabled = false;
        this.transformControls.addEventListener('dragging-changed', (event) => {
            if(event.value) this.onStartTransform();
            else this.onEndTransform();
        });
        this.sceneWrapper.scene.add(this.transformControls);

        this._sendTransformInterval = null;
    }

    enable() {
        this.transformControls.enabled = true;
    }

    disable() {
        this.transformControls.enabled = false;
    }

    setCamera(camera) {
        this.transformControls.camera = camera.cameraObject;
    }

    setMode(mode) {
        this.transformControls.setMode(mode);
    }

    setSpace(space) {
        this.transformControls.setSpace(space);
    }

    setOpacityRecursive(obj, value) {
        obj.traverse((o) => {
            if(o.type == 'Mesh' && o.material !== undefined) {
                if(o.material.userData.cloned === undefined) {
                    o.material = o.material.clone();
                    o.material.userData.cloned = true;
                }
                o.renderOrder = 2000;
                o.material.depthTest = false;
                o.material.transparent = true;
                o.material.opacity = value;
                o.material.emissive.setRGB(1, 1, 0);
            }
        });
    }

    attach(obj) {
        this.transformControls.size = settings.transformControls.size;

        if(this.transformControls.object !== undefined) {
            if(this.transformControls.object === obj)
                return;
            this.detach();
        }
        if(obj === null || obj === undefined) return;

        var clone = obj.clone(true);
        this.setOpacityRecursive(clone, 0.0);

        delete clone.userData.uid;

        obj.parent.add(clone);
        clone.position.copy(obj.position);
        clone.quaternion.copy(obj.quaternion);

        obj.userData.clone = clone;
        clone.userData.original = obj;

        view.requestBoundingBoxUpdate();

        this.transformControls.attach(clone);

        if(obj.userData.canTranslateDuringSimulation === undefined)
            obj.userData.canTranslateDuringSimulation = true;
        if(obj.userData.canTranslateOutsideSimulation === undefined)
            obj.userData.canTranslateOutsideSimulation = true;
        if(obj.userData.canRotateDuringSimulation === undefined)
            obj.userData.canRotateDuringSimulation = true;
        if(obj.userData.canRotateOutsideSimulation === undefined)
            obj.userData.canRotateOutsideSimulation = true;
        if(this.transformControls.mode === 'translate') {
            this.transformControls.enabled = simulationRunning
                ? obj.userData.canTranslateDuringSimulation
                : obj.userData.canTranslateOutsideSimulation;
        } else if(this.transformControls.mode === 'rotate') {
            this.transformControls.enabled = simulationRunning
                ? obj.userData.canRotateDuringSimulation
                : obj.userData.canRotateOutsideSimulation;
        }
        this.transformControls.setTranslationSnap(
            obj.userData.translationStepSize !== null
                ? obj.userData.translationStepSize
                : this.transformControls.userData.defaultTranslationSnap
        );
        this.transformControls.setRotationSnap(
            obj.userData.rotationStepSize !== null
                ? obj.userData.rotationStepSize
                : this.transformControls.userData.defaultRotationSnap
        );

        this.transformControls.showX = true;
        this.transformControls.showY = true;
        this.transformControls.showZ = true;
        if(this.transformControls.mode === 'translate' && obj.userData.movementPreferredAxes?.translation && obj.userData.hasTranslationalConstraints) {
            this.transformControls.showX = obj.userData.movementPreferredAxes.translation.x !== false;
            this.transformControls.showY = obj.userData.movementPreferredAxes.translation.y !== false;
            this.transformControls.showZ = obj.userData.movementPreferredAxes.translation.z !== false;
            this.setSpace(obj.userData.translationSpace);
        } else if(this.transformControls.mode === 'rotate' && obj.userData.movementPreferredAxes?.rotation && obj.userData.hasRotationalConstraints) {
            this.transformControls.showX = obj.userData.movementPreferredAxes.rotation.x !== false;
            this.transformControls.showY = obj.userData.movementPreferredAxes.rotation.y !== false;
            this.transformControls.showZ = obj.userData.movementPreferredAxes.rotation.z !== false;
            this.setSpace(obj.userData.rotationSpace);
        }
    }

    updateTargetPosition() {
        var clone = this.transformControls.object;
        var obj = clone.userData.original;
        if(offline) {
            obj.position.copy(clone.position);
            obj.quaternion.copy(clone.quaternion);
        } else {
            var p = clone.position.toArray();
            var q = clone.quaternion.toArray();
            sim.setObjectPose(obj.userData.handle, sim.handle_parent, p.concat(q));
        }
    }

    detach() {
        if(this.transformControls.object === undefined)
            return; // was not attached

        var clone = this.transformControls.object;
        var obj = clone.userData.original;

        clone.removeFromParent();

        delete clone.userData.original;
        delete obj.userData.clone;

        view.requestBoundingBoxUpdate();

        this.transformControls.detach();
    }

    reattach() {
        if(this.transformControls.object === undefined)
            return; // was not attached

        var clone = this.transformControls.object;
        var obj = clone.userData.original;
        this.detach();
        this.attach(obj);
    }

    onStartTransform() {
        var clone = this.transformControls.object;
        this.setOpacityRecursive(clone, 0.4);

        if(settings.transformControls.sendRate > 0) {
            this._sendTransformInterval = setInterval(() => this.updateTargetPosition(), Math.max(50, 1000 / settings.transformControls.sendRate), true);
        }
    }

    onEndTransform() {
        clearInterval(this._sendTransformInterval);
        this.updateTargetPosition();

        // clear ghost only when position is actually updated
        // (avoids the object briefly disappearing):
        var clone = this.transformControls.object;
        var obj = clone.userData.original;
        this.setOpacityRecursive(clone, 0.0);
    }
}

class ObjTree {
    constructor(sceneWrapper, domElement) {
        this.sceneWrapper = sceneWrapper;
        this.domElement = domElement
        if(this.domElement.jquery !== undefined)
            this.domElement = this.domElement.get()[0];
        this.faiconForType = {
            scene: 'globe',
            camera: 'video',
            shape: 'cubes',
            light: 'lightbulb',
            joint: 'cogs',
            dummy: 'bullseye',
            pointCloud: 'cloud',
            octree: 'border-all',
        }
        this.updateRequested = false;
        this._checkInterval = setInterval(() => {
            if(this.updateRequested && $(this.domElement).is(":visible")) {
                this.update();
                this.updateRequested = false;
            }
        }, 200);
    }

    update(obj = undefined) {
        if(obj === undefined) {
            while(this.domElement.firstChild)
                this.domElement.removeChild(this.domElement.lastChild);
            this.domElement.appendChild(this.update(this.sceneWrapper.scene));
        } else {
            var li = document.createElement('li');
            var item = document.createElement('span');
            item.classList.add('tree-item');
            var icon = document.createElement('i');
            icon.classList.add('tree-item-icon');
            icon.classList.add('fas');
            var type = obj.type == "Scene" ? 'scene' : obj.userData.type;
            var faicon = this.faiconForType[type];
            if(faicon === undefined) faicon = 'question';
            icon.classList.add(`fa-${faicon}`);
            var nameLabel = document.createElement('span');
            nameLabel.classList.add("tree-item-name");
            if(view.selectedObject === obj)
                nameLabel.classList.add("selected");
            nameLabel.appendChild(document.createTextNode(
                (obj === this.sceneWrapper.scene ? "(scene)" : obj.nameWithOrder)
            ));
            nameLabel.addEventListener('click', () => {
                this.dispatchEvent('itemClicked', obj.userData.uid);
            });
            obj.userData.treeElement = nameLabel;
            if(obj.userData.treeElementExpanded === undefined)
                obj.userData.treeElementExpanded = obj.userData.parentUid !== -1;
            const children = obj === this.sceneWrapper.scene
                ? [...obj.children].filter((o) => o.userData.uid !== undefined)
                : obj.childObjects
                ?? [];
            if(children.length > 0) {
                var toggler = document.createElement('span');
                toggler.classList.add('toggler');
                if(obj.userData.treeElementExpanded)
                    toggler.classList.add('toggler-open');
                else
                    toggler.classList.add('toggler-close');
                toggler.addEventListener('click', () => {
                    ul.classList.toggle('active');
                    toggler.classList.toggle('toggler-open');
                    toggler.classList.toggle('toggler-close');
                    obj.userData.treeElementExpanded = !obj.userData.treeElementExpanded;
                });
                item.appendChild(toggler);
            }
            item.appendChild(icon);
            item.appendChild(nameLabel);
            if(obj.type != "Scene") {
                var hideBtnIcon = document.createElement('i');
                hideBtnIcon.classList.add('fas');
                hideBtnIcon.classList.add('fa-eye');
                var hideBtn = document.createElement('a');
                hideBtn.href = '#';
                hideBtn.style.color = 'rgba(0,0,0,0.1)';
                hideBtn.style.marginLeft = '3px';
                hideBtn.classList.add('hide-btn');
                hideBtn.appendChild(hideBtnIcon);
                var showBtnIcon = document.createElement('i');
                showBtnIcon.classList.add('fas');
                showBtnIcon.classList.add('fa-eye-slash');
                var showBtn = document.createElement('a');
                showBtn.href = '#';
                showBtn.style.color = 'rgba(0,0,0,0.3)';
                showBtn.style.marginLeft = '3px';
                showBtn.classList.add('show-btn');
                showBtn.appendChild(showBtnIcon);
                hideBtn.addEventListener('click', () => {
                    hideBtn.style.display = 'none';
                    showBtn.style.display = 'inline';
                    obj.visible = false;
                    view.requestRender();
                });
                showBtn.addEventListener('click', () => {
                    hideBtn.style.display = 'inline';
                    showBtn.style.display = 'none';
                    obj.visible = true;
                    view.requestRender();
                });
                if(obj.visible) showBtn.style.display = 'none';
                else hideBtn.style.display = 'none';
                item.appendChild(hideBtn);
                item.appendChild(showBtn);
            }
            if(children.length > 0) {
                var ul = document.createElement('ul');
                if(obj.userData.treeElementExpanded)
                    ul.classList.add('active');
                for(var c of children)
                    ul.appendChild(this.update(c));
                item.appendChild(ul);
            }
            li.appendChild(item);
            return li;
        }
    }

    requestUpdate() {
        this.updateRequested = true;
    }
}

mixin(ObjTree, EventSourceMixin);

function info(text) {
    $('#info').html(text);
    if(!text) $('#info').hide();
    else $('#info').show();
}

function debug(text) {
    if(text !== undefined && $('#debug').is(":visible"))
        console.log(text);
    if(typeof text === 'string' || text instanceof String) {
        $('#debug').text(text);
    } else {
        debug(JSON.stringify(text, undefined, 2));
    }
}

THREE.Object3D.DefaultUp = new THREE.Vector3(0, 0, 1);

var simulationRunning = false;

var sceneWrapper = new SceneWrapper();

const visualizationStreamClient = new VisualizationStreamClient(eventsEndpoint.host, eventsEndpoint.port, eventsEndpoint.codec);
visualizationStreamClient.addEventListener('noop', () => {});
visualizationStreamClient.addEventListener('objectAdded', onObjectAdded);
visualizationStreamClient.addEventListener('objectChanged', onObjectChanged);
visualizationStreamClient.addEventListener('objectRemoved', onObjectRemoved);
visualizationStreamClient.addEventListener('drawingObjectAdded', onDrawingObjectAdded);
visualizationStreamClient.addEventListener('drawingObjectChanged', onDrawingObjectChanged);
visualizationStreamClient.addEventListener('drawingObjectRemoved', onDrawingObjectRemoved);
visualizationStreamClient.addEventListener('genesisBegin', () => {
    // XXX: to verify if it is still needed (moved from onSceneChanged)
    view.setSelectedObject(null, false);
});
visualizationStreamClient.addEventListener('genesisEnd', () => {});
visualizationStreamClient.addEventListener('msgDispatchTime', () => {});
visualizationStreamClient.addEventListener('logMsg', () => {});

var view = new View(document.querySelector('#view'), sceneWrapper);
view.addEventListener('selectedObjectChanged', (event) => {
    if(event.previous !== null && event.previous.userData.treeElement !== undefined)
        $(event.previous.userData.treeElement).removeClass('selected');
    if(event.current !== null && event.current.userData.treeElement !== undefined)
        $(event.current.userData.treeElement).addClass('selected');

    if(transformControlsWrapper.transformControls.object !== undefined)
        transformControlsWrapper.detach();
    if(event.current !== null && transformControlsWrapper.transformControls.enabled)
        transformControlsWrapper.attach(event.current);

    view.requestRender();

    notifyEvent({
        event: 'selectedObjectChanged',
        uid: event.current ? event.current.userData.uid : -1,
    });
});

var lastPickedPoint = {
    isSet: false,
    position: new THREE.Vector3(),
    quaternion: new THREE.Quaternion(),
};

view.selectPointTool.addEventListener('selectedPoint', (event) => {
    lastPickedPoint.isSet = true;
    lastPickedPoint.position.copy(event.position);
    lastPickedPoint.quaternion.copy(event.quaternion);
    transformControlsWrapper.disable();
    if(view.selectedObject !== null) {
        transformControlsWrapper.detach();
    }
    notifyEvent({
        event: 'pointPick',
        data: {
            pose: [
                ...lastPickedPoint.position.toArray(),
                ...lastPickedPoint.quaternion.toArray(),
            ],
            ray: {
                origin: event.ray.origin.toArray(),
                direction: event.ray.direction.toArray(),
            },
        },
    });
});

/*
view.addEventListener('selectedCameraChanged', () => {
    if(view.selectedCamera.type == 'OrthographicCamera') {
        // XXX: make sure camera looks "straight"
        var v = view.selectedCamera.position.clone();
        var target = orbitControlsWrapper.getTarget();
        v.sub(target);
        v.x = Math.abs(v.x);
        v.y = Math.abs(v.y);
        v.z = Math.abs(v.z);
        if(v.x >= v.y && v.x >= v.z) {
            target.y = view.selectedCamera.position.y;
            target.z = view.selectedCamera.position.z;
        } else if(v.y >= v.x && v.y >= v.z) {
            target.x = view.selectedCamera.position.x;
            target.z = view.selectedCamera.position.z;
        } else if(v.z >= v.x && v.z >= v.y) {
            target.x = view.selectedCamera.position.x;
            target.y = view.selectedCamera.position.y;
        }
        orbitControlsWrapper.setTarget(target);

        // XXX: first time camera shows nothing, moving mouse wheel fixes that
        if(!orbitControlsWrapper.XXX) {
            orbitControlsWrapper.XXX = true;
            for(var i = 0; i < 2; i++) {
                setTimeout(() => {
                    var evt = document.createEvent('MouseEvents');
                    evt.initEvent('wheel', true, true);
                    evt.deltaY = (i - 0.5) * 240;
                    orbitControlsWrapper.orbitControls.domElement.dispatchEvent(evt);
                }, (i + 1) * 100);
            }
        }
    }

    transformControlsWrapper.transformControls.camera = view.selectedCamera;
});
view.addEventListener('cameraPoseChanging', e => {
    // save orbitControl's target in camera coords *before* moving the camera
    view.selectedCamera.updateMatrixWorld();
    view.targetLocal = view.selectedCamera.worldToLocal(orbitControlsWrapper.getTarget());
});
view.addEventListener('cameraPoseChanged', () => {
    // compute new global position of target
    view.selectedCamera.updateMatrixWorld();
    var t = view.selectedCamera.localToWorld(view.targetLocal);
    // move orbitControl's target
    orbitControlsWrapper.setTarget(t);
});
*/

var axesView = new AxesView(document.querySelector('#axes'), view.selectedCamera.up);

var orbitControlsWrapper = new OrbitControlsWrapper(view.selectedCamera, view.renderer, () => render());

var transformControlsWrapper = new TransformControlsWrapper(sceneWrapper, view.selectedCamera, view.renderer);
transformControlsWrapper.transformControls.addEventListener('dragging-changed', event => {
    // disable orbit controls while dragging:
    if(event.value) {
        // dragging has started: store enabled flag
        transformControlsWrapper.orbitControlsWasEnabled = orbitControlsWrapper.setEnabled(false);
    } else {
        // dragging has ended: restore previous enabled flag
        orbitControlsWrapper.setEnabled(transformControlsWrapper.orbitControlsWasEnabled);
        transformControlsWrapper.orbitControlsWasEnabled = undefined;
    }
});
transformControlsWrapper.transformControls.addEventListener('change', (event) => {
    // make bbox follow
    view.requestBoundingBoxUpdate();

    view.requestRender();
});

if(!offline) {
    visualizationStreamClient.addEventListener('callFunction', function(eventInfo) {
        var eventData = eventInfo.data;
        var result = window[eventData.funcName](...eventData.funcArgs);
        notifyEvent({event: 'callFunctionReply', data: {
            requestId: eventData.requestId,
            result: result
        }});
    });

    var remoteApiClient = new RemoteAPIClient(remoteApiEndpoint.host, remoteApiEndpoint.port, remoteApiEndpoint.codec, {createWebSocket: url => new ReconnectingWebSocket(url)});
    var sim = null;
    remoteApiClient.websocket.onOpen.addListener(() => {
        remoteApiClient.getObject('sim').then((_sim) => {
            sim = _sim;
        });
    });
    remoteApiClient.websocket.open();
}

var notifyEventFunc = 'event'
var notifyEventTarget = '/eventSink'

async function notifyEvent(eventData) {
    if(offline) return;
    try {
        await sim.callScriptFunction(`${notifyEventFunc}@${notifyEventTarget}`, sim.scripttype_customizationscript, eventData);
    } catch(error) {
    }
}

var objTree = new ObjTree(sceneWrapper, $('#objtree'));
objTree.addEventListener('itemClicked', onTreeItemSelected);

class ObjectSetObserver {
    constructor(sceneWrapper, predicate, scanInterval) {
        this.sceneWrapper = sceneWrapper
        this.predicate = predicate
        this._scanInterval = setInterval(() => this.scan(), scanInterval);
        this.previousSet = new Set([]);
    }

    check(o) {
        return this.predicate(o);
    }

    getAll() {
        var all = {};
        this.sceneWrapper.scene.traverse((o) => {
            if(this.check(o))
                all[o.userData.uid] = o;
        });
        return all;
    }

    scan() {
        var all = this.getAll();
        var set = new Set(Object.keys(all));
        if(set.size != this.previousSet.size || ![...set].every(uid => this.previousSet.has(uid))) {
            this.dispatchEvent('changed', all);
            this.previousSet = set;
        }
    }
}

mixin(ObjectSetObserver, EventSourceMixin);

function render() {
    view.requestRender();
    axesView.requestRender();
}

function animate() {
    requestAnimationFrame(animate);
    view.render();
    axesView.render(view.selectedCamera.position, orbitControlsWrapper.getTarget());
}
animate();

function onTreeItemSelected(uid) {
    var obj = sceneWrapper.getObjectByUid(uid);
    view.setSelectedObject(obj, false);
}

function onAppChanged(eventData) {
    if(eventData.data.defaultRotationStepSize !== undefined) {
        transformControlsWrapper.transformControls.setRotationSnap(eventData.data.defaultRotationStepSize);
        transformControlsWrapper.transformControls.userData.defaultRotationSnap = eventData.data.defaultRotationStepSize;
    }

    if(eventData.data.defaultTranslationStepSize !== undefined) {
        transformControlsWrapper.transformControls.setTranslationSnap(eventData.data.defaultTranslationStepSize);
        transformControlsWrapper.transformControls.userData.defaultTranslationSnap = eventData.data.defaultTranslationStepSize;
    }

    if(eventData.data.protocolVersion !== undefined) {
        const suppVer = 3;
        if(eventData.data.protocolVersion !== suppVer) {
            window.alert(`Protocol version not supported. Please upgrade ${eventData.data.protocolVersion < suppVer ? 'CoppeliaSim' : 'threejsFrontend'}.`);
            visualizationStreamClient.websocket.close();
            document.querySelector('body').innerHTML = '';
            return;
        }
    }

    if(eventData.data.sessionId && visualizationStreamClient.sessionId !== eventData.data.sessionId) {
        //visualizationStreamClient.seq = -1; // not needed anymore, since events are always contiguous
        visualizationStreamClient.sessionId = eventData.data.sessionId;
    }

    render();
}

function onSceneChanged(eventData) {
    //view.setSelectedObject(null, false); // moved to onGenesisBegin
    sceneWrapper.setSceneData(eventData);

    if(eventData.data.simulationState !== undefined) {
        simulationRunning = eventData.data.simulationState != 0;
        transformControlsWrapper.reattach();
    }

    render();
}

function onObjectAdded(eventData) {
    sceneWrapper.addObject(eventData);

    objTree.requestUpdate();

    render();
}

function onObjectChanged(eventData) {
    if(eventData.handle === -13) return onAppChanged(eventData);
    if(eventData.handle === -12) return onSceneChanged(eventData);

    var obj = sceneWrapper.getObjectByUid(eventData.uid);
    if(obj === undefined) return;

    if(eventData.data.alias != obj.name
            || eventData.data.parentUid != obj.userData.parentUid
            || eventData.data.childOrder != obj.userData.childOrder)
        objTree.requestUpdate();

    obj.update(eventData);

    if(view.isPartOfSelection(obj) || view.selectedObject?.ancestorObjects?.includes(obj)) {
        view.requestRender(); // view.render(); // with view.render(), rendering of model bbox is very slow // XXX: without this, bbox would lag behind
        view.requestBoundingBoxUpdate();
    }

    render();
}

function onObjectRemoved(eventData) {
    var obj = sceneWrapper.getObjectByUid(eventData.uid);
    if(obj === undefined) return;
    if(obj === view.selectedObject)
        view.setSelectedObject(null, false);
    sceneWrapper.removeObject(obj);

    objTree.requestUpdate();

    render();
}

function onDrawingObjectAdded(eventData) {
    sceneWrapper.addDrawingObject(eventData);

    render();
}

function onDrawingObjectChanged(eventData) {
    var obj = DrawingObject.getObjectByUid(eventData.uid);
    if(obj === undefined) return;

    obj.update(eventData);

    render();
}

function onDrawingObjectRemoved(eventData) {
    var obj = DrawingObject.getObjectByUid(eventData.uid);
    if(obj === undefined) return;
    sceneWrapper.removeDrawingObject(obj);

    render();
}

function toggleObjTree() {
    $("#objtreeBG").toggle();
}

function toggleDebugInfo() {
    $("#debug").toggle();
}

function cancelCurrentMode() {
    view.rayCastTool.disable();
    view.selectPointTool.disable();
    transformControlsWrapper.disable();
    if(view.selectedObject !== null) {
        transformControlsWrapper.detach();
    }
}

function setTransformMode(mode, space) {
    view.rayCastTool.disable();
    view.selectPointTool.disable();
    transformControlsWrapper.enable();
    transformControlsWrapper.setMode(mode);
    transformControlsWrapper.setSpace(space);
    if(view.selectedObject !== null) {
        transformControlsWrapper.attach(view.selectedObject);
    }
}

function setTransformSnap(enabled) {
    if(enabled) {
        transformControlsWrapper.transformControls.setRotationSnap(
            transformControlsWrapper.transformControls.userData.previousRotationSnap
        );
        transformControlsWrapper.transformControls.setTranslationSnap(
            transformControlsWrapper.transformControls.userData.previousTransationSnap
        );
    } else {
        transformControlsWrapper.transformControls.userData.previousTranslationSnap = transformControlsWrapper.transformControls.translationSnap;
        transformControlsWrapper.transformControls.userData.previousRotationSnap = transformControlsWrapper.transformControls.rotationSnap;
        transformControlsWrapper.transformControls.setRotationSnap(null);
        transformControlsWrapper.transformControls.setTranslationSnap(null);
    }
}

function setScreenSpacePanning(enabled) {
    orbitControlsWrapper.setScreenSpacePanning(enabled);
}

function setPickPointMode() {
    lastPickedPoint.isSet = false;
    transformControlsWrapper.detach();
    view.rayCastTool.disable();
    view.selectPointTool.enable();
}

function setRayCastMode() {
    lastPickedPoint.isSet = false;
    transformControlsWrapper.detach();
    view.rayCastTool.enable();
    view.selectPointTool.disable();
}

function toggleGui() {
    $('#gui').toggle();
}

function toggleLog() {
    $('#log').toggle();
}

const keyMappings = {
    KeyH_down:   e => toggleObjTree(),
    KeyD_down:   e => toggleDebugInfo(),
    Escape_down: e => cancelCurrentMode(),
    KeyT_down:   e => setTransformMode('translate', e.shiftKey ? 'local' : 'world'),
    KeyR_down:   e => setTransformMode('rotate', e.shiftKey ? 'local' : 'world'),
    ShiftLeft:   e => {setTransformSnap(e.type === 'keyup'); setScreenSpacePanning(e.type === 'keyup');},
    ShiftRight:  e => {setTransformSnap(e.type === 'keyup'); setScreenSpacePanning(e.type === 'keyup');},
    KeyP_down:   e => e.shiftKey ? setPickPointMode() : setRayCastMode(),
    KeyG_down:   e => toggleGui(),
    KeyL_down:   e => toggleLog(),
};

window.addEventListener('keydown', e => (keyMappings[e.code + '_down'] || keyMappings[e.code] || (e => {}))(e));
window.addEventListener('keyup',   e => (keyMappings[e.code + '_up']   || keyMappings[e.code] || (e => {}))(e));
