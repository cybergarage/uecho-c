# Inside of uEcho Device

## Node Profile Object

[ECHONET Lite][enet] device node must have a following special node profile class object which has all children objects of the node [\[1\]][enet-spec].

- Class group code: 0x0E
- Class code: 0xF0
- Instance code: 0x01 (general node)

The uEcho updates the node profile class objects automatically when the children node is changed.

![Device Objects](img/uecho_device_objects.png)

## Device Object Super Class

[ECHONETLite][enet] device node must have some properties [\[2\]][enet-spec]. To use `uecho_device_new()` or `uecho_device_addmandatoryproperties()`, the following mandatory properties are added into the node.

| EPC | Property name | Default value |
|---|---|---|
| 0x80 | Operation status | 0x30 = ON |
| 0x81 | Installation location | 0x00 |
| 0x82 | Standard version information | Appendix F |
| 0x88 | Fault status | 0x42 = No fault has occurred |
| 0x8A | Manufacturer code | 0xFFFFF0 = Testing code |

uEcho updates the following mandatory properties automatically too when any properties are added or removed.

| EPC | Property name |
|---|---|
| 0x9D | Status change announcement property map |
| 0x9E | Set property map  |
| 0x9F | Get property map |

## Device Message Listeners

Basically uEcho handles all messages from other nodes automatically. However, developer can set more detail user listeners into the node, objects and properties.

![Device Listeners](img/uecho_device_listeners.png)

To set the listeners, use `uecho_node_setmessagelistener`, `uecho_object_setmessagelistener` and `uecho_object_setpropertyrequeslistener`.

### Message Listener Sequences

After a node is received a message from other nodes, the node's listeners are called as the following sequences:

![Node Observers](img/uecho_node_msg_listener.png)

### Node Message Listener

`uecho_node_setmessagelistener` can get all message for the node from other nodes, thus the message might be invalid.

### Object Message Listener

uEcho verifies the messages form other nodes using the objects and properties information of the node, and returns an error response when the message is invalid automatically. `uecho_object_setmessagelistener` can get only valid messages for the object from other nodes.

### Property Message Listener

`uecho_object_setpropertyrequeslistener` can get only valid request message for the object property from other nodes.

[enet]:http://echonet.jp/english/

## References

- \[1\] [Part II ECHONET Lite Communication Middleware Specification][enet-spec]
- \[2\] [Detailed Requirements for ECHONET Device objects][enet-spec]

[enet]:http://echonet.jp/english/
[enet-spec]:http://www.echonet.gr.jp/english/spec/index.htm