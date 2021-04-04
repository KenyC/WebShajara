
/**************************************
GET STRING FROM JS
***************************************/


char* string = (char*) EM_ASM_INT({
	var ptr  = allocate(
		intArrayFromString('ui.c (l. 7) : Wow!'), 
		ALLOC_NORMAL
	);
	return ptr;
}); 
LOG("%s\n", string);


/**************************************
SEND STRING TO JS
***************************************/


EM_ASM({
	var view =  Module.HEAPU8.subarray($0, $0 + $1);
	var string = '';
	for (let i = 0; i < $1; i++) {
	  string += String.fromCharCode(view[i]);
	}
	alert(string);
}, test, strlen(test));