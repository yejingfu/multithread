// test
var mt = require('../build/Release/multithread');


var thread = mt.createThread();
if (!thread) {
  console.log('thread is undefined');
} else {
  console.log('thread id: ' + thread.id);
  setTimeout(function() {
    thread.destroy();
    console.log('done');
  }, 3000);

}


/*
// shared buffer
var buf = mt.createSharedBuffer(10);
//buf.id = 22;
//buf.size = 12;
console.log(buf.id + '--' + buf.size);


// worker
var createWorker = function(proc, arg, sharedBuffer, cb) {
  if (typeof proc !== 'function' || typeof arg !== 'object')
    return undefined;
  cb = cb || function() {};
  var bufId = sharedBuffer ? sharedBuffer.id : undefined;
  var script = '('+proc.toString()+')('+JSON.stringify(arg)+')';
  var ret = mt.createWorker(script, bufId, cb);
  return ret;
};

var worker = createWorker(function(arg) {
    //console.warn('In sub thread');  // this function is not defined.
    //console.error('In sub thread');   // Okay!
    console.log('In seperated worker: ' + arg.msg);
    if (sharedBuffer) {
      console.log('sharedBuffer valid: ' + sharedBuffer.id + '--' + sharedBuffer.size);
    } else {
      console.log('sharedBuffer invalid');
    }
  },
  {msg: 'Hello World--'},
  buf,
  function(err, status) {
    // console.warn('In main thread');   // OK!!
    if (err)
      console.error('failed in worker: ' + status);
    else
      console.log('succeed in worker: ' + status);
  }
);

if (!worker) {
  console.log('failed to create worker');
} else {
  console.log('Succeed to create worker : ' + worker.id);
  worker.eval();
}

*/

