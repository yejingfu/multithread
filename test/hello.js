// test
var mt = require('../build/Release/multithread');

console.log(mt.hello());

/*
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
*/

var worker = mt.createWorker('var a = 10;', function(){
  console.log('back');
});

if (!worker) {
  console.log('failed to create worker');
} else {
  console.log('Succeed to create worker');
}

