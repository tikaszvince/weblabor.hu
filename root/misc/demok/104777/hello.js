onmessage = function(event) {  
	var name = event.data;  
	setTimeout(function() {  
		postMessage('Hello '+name+'!');  
	},1000);  
	throw 'Something happened...';  
};