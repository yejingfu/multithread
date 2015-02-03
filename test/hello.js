// test
var mt = require('../build/Release/multithread');

console.log(mt.hello());

var thread = mt.createThread();
if (!thread) {
  console.log('thread is undefined');
} else {
  console.log('thread id: ' + thread.id);
  setTimeout(function() {
  	  thread.destroy();
    console.log('done');
  }, 1000);

}

