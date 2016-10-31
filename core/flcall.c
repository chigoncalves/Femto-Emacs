#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include "llt/llt.h"
#include "flisp.h"

extern void interface_init(void);
extern value_t fl_file(value_t *args, uint32_t nargs);
extern value_t fl_buffer(value_t *args, u_int32_t nargs);
extern value_t fl_iowrite(value_t *args, u_int32_t nargs);
extern value_t fl_ioseek(value_t *args, u_int32_t nargs);
extern value_t fl_read(value_t *args, u_int32_t nargs);
extern value_t fl_toplevel_eval(value_t expr);
extern value_t eval_sexpr(value_t e, value_t *penv, int tail, u_int32_t envend);

/*
 * NOTE 2:
 * probably not needed as the function applyn()
 * uses va_args like printf();
 * eg printf("hello");
 *    printf("%s", hello);
 *    printf("%d %s", 27, "hello");
 *  etc.
 * are all valid for a va_arg function.
 *
 */
/*
static value_t argv_list(int argc, char *argv[])
{
	int i;
	value_t lst=FL_NIL, temp;

	fl_gc_handle(&lst);
	fl_gc_handle(&temp);

	for(i=argc-1; i >= 0; i--) {
		temp = cvalue_static_cstring(argv[i]);
		lst = fl_cons(temp, lst);
	}
	fl_free_gc_handles(2);
	return lst;
}
*/

/* come back buffer ? output ? */

value_t volta_buffer(char* string)
{
	value_t barg[2];
	value_t args[2];
	value_t b = fl_buffer(barg, 0);
	args[0] = b;
	args[1] = cvalue_static_cstring(string);
	fl_iowrite(args,2);
	value_t sargs[2];
	sargs[0] = b;
	sargs[1] = 0;
	fl_ioseek(sargs,2);
	return b;
}

static char *wrp=
	"(let ((b (buffer))) \
   (with-output-to b (princ %s)) \
   (io.tostring! b))";

/* read_buffer */
value_t ler_buffer(value_t buffer) {
	return fl_read(&buffer, 1);
}

/* the output of the initial load init.lst call, not used anywhere else */
char scmbuffer[1000];

/* probably not needed as temp is defined locally ? */
//char *temp;

void callLisp (char *buff, char *expr) {
	FL_TRY_EXTERN {
		char cbuffer[1000];
		
		sprintf(cbuffer, wrp, expr);
		
		value_t BB = volta_buffer(cbuffer);
		value_t CC = ler_buffer(BB);
		
		CC= fl_toplevel_eval(CC);
		strcpy(buff, cptr(CC));

		/*
		 * might be better to change to:
		 *    char *callLisp(char expr)
		 * and 
 		 *    return cptr(CC);
		 *
		 * rather than copying to a fixed size buff 
		 * this would avoid the risk of buffer overflows.
		 * FemtoEmacs can then use that char *
		 * make use of it by probably inserting a copy into a buffer
		 * and can then forget about it.
		 *
		 */
		
	} FL_CATCH_EXTERN {
		strcpy(buff,"Error");
		/* FIXME we should send back the error details */
	}
}

char *callLisp2(char *expr) {
	FL_TRY_EXTERN {
		char cbuffer[1000];  /* still has a 1000 char limit */
		
		sprintf(cbuffer, wrp, expr);
		
		value_t BB = volta_buffer(cbuffer);
		value_t CC = ler_buffer(BB);
		
		CC= fl_toplevel_eval(CC);

		return cptr(CC);
		
	} FL_CATCH_EXTERN {
	}
	return "Error";
}

int initLisp(int argc, char *argv[])
{
	char fname_buf[1024];

	fl_init(512*1024);
	fname_buf[0] = '\0';

	value_t str = symbol_value(symbol("*install-dir*"));
	char *exedir = (str == UNBOUND ? NULL : cvalue_data(str));

	if (exedir != NULL) {
		strcat(fname_buf, exedir);
		strcat(fname_buf, PATHSEPSTRING);
	}

	strcat(fname_buf, "femto.boot");

	value_t args[2];
	fl_gc_handle(&args[0]);
	fl_gc_handle(&args[1]);

	FL_TRY_EXTERN {

		/*
		 * NOTE 1:
		 * probably dont need to malloc(400) here.
		 * char buff[100]; would be fine and then
		 * free() is required.
		 */
		char *buff= (char *)malloc(400);

		/* read system image file "femto.boot" */
		args[0] = cvalue_static_cstring(fname_buf);
		args[1] = symbol(":read");

		value_t f = fl_file(&args[0], 2);

		fl_free_gc_handles(2);
		if (fl_load_system_image(f))
			return 1;

		interface_init();

		/*
		 * fl_applyn(1, symbol_value(symbol("__start")), "");
		 *
		 * would probably work fine. We are not passing in anything useful
		 * in argc, argv - the argv list at best is full of 00s having been
		 * malloc'd in main but never initialised.
		 * see also NOTE2 above for the argv_list() function.
		 *
		 */
		//(void)fl_applyn(1, symbol_value(symbol("__start")), argv_list(argc, argv));

		(void)fl_applyn(1, symbol_value(symbol("__start")), "");

		/* load init.lsp */
		sprintf(buff, "(load \"%s/init.lsp\")", getenv("HOME"));
		callLisp(scmbuffer, buff);
		free(buff);  /* see note #1 */
		
	} FL_CATCH_EXTERN {
		ios_puts("fatal error:\n", ios_stderr);
		fl_print(ios_stderr, fl_lasterror);
		ios_putc('\n', ios_stderr);
		return 1;
	} 
  
	return 0;
}
