var countPrimes = (function() {
	function isPrime( num ) {
		for (var i = 2; i <= Math.sqrt(num); i += 1)
			if (num % i == 0)
				return false;
		return true;
	}
	
	return function( from, to ) {
		var num = from;
		var count = 0;
		while ( num <= to ) {
			if ( isPrime( num++ ) ) count++;
		}
		return count;
	};
})();
