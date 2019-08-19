// Auto-generated. Do not edit!

// (in-package face_tracker_pkg.msg)


"use strict";

let _serializer = require('../base_serialize.js');
let _deserializer = require('../base_deserialize.js');
let _finder = require('../find.js');

//-----------------------------------------------------------

class centroid {
  constructor() {
    this.x = 0;
    this.y = 0;
  }

  static serialize(obj, bufferInfo) {
    // Serializes a message object of type centroid
    // Serialize message field [x]
    bufferInfo = _serializer.int32(obj.x, bufferInfo);
    // Serialize message field [y]
    bufferInfo = _serializer.int32(obj.y, bufferInfo);
    return bufferInfo;
  }

  static deserialize(buffer) {
    //deserializes a message object of type centroid
    let tmp;
    let len;
    let data = new centroid();
    // Deserialize message field [x]
    tmp = _deserializer.int32(buffer);
    data.x = tmp.data;
    buffer = tmp.buffer;
    // Deserialize message field [y]
    tmp = _deserializer.int32(buffer);
    data.y = tmp.data;
    buffer = tmp.buffer;
    return {
      data: data,
      buffer: buffer
    }
  }

  static datatype() {
    // Returns string type for a message object
    return 'face_tracker_pkg/centroid';
  }

  static md5sum() {
    //Returns md5sum for a message object
    return 'bd7b43fd41d4c47bf5c703cc7d016709';
  }

  static messageDefinition() {
    // Returns full string definition for message
    return `
    int32 x
    int32 y
    
    
    `;
  }

};

module.exports = centroid;
