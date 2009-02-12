#include "php_spidermonkey.h"

static int le_jsobject_descriptor;

/**
* JSObject embedding
*/

/* The class of the global object. */
zend_class_entry *php_spidermonkey_jso_entry;

/* {{{ proto public JSObject::__construct(JSContext $context)
   JSObject's constructor, need a JSRuntime, deprecated, use
   JSContext::createObject() instead  */
PHP_METHOD(JSObject, __construct)
{
	/* prevent creating this object */
	zend_throw_exception(zend_exception_get_default(TSRMLS_C), "JSObject can't be instancied directly, please call JSContext::createObject", 0 TSRMLS_CC);
}
/* }}} */

/* {{{ proto public JSObject::__destruct()
   Clean some data ( maybe we can remove this ? ) */
PHP_METHOD(JSObject, __destruct)
{
	php_jsobject_object *intern_ot;

	intern_ot = (php_jsobject_object *) zend_object_store_get_object(getThis() TSRMLS_CC);

	ZVAL_DELREF(intern_ot->ct_z);

	zval_ptr_dtor(&intern_ot->ct_z);

	/* No need todestroy anything, the garbage collector will do it */
	intern_ot->obj = NULL;
	intern_ot->ct = NULL;
}
/* }}} */

/* {{{ proto public mixed JSObject::evaluateScript(string $script)
   Evaluate script and return the last global object in scope to PHP.
   Objects are not returned for now. Any global variable declared in
   this script will be usable in any following call to evaluateScript
   in any JSObject in  the same context. */
PHP_METHOD(JSObject, evaluateScript)
{
	char *script;
	int script_len;
	php_jsobject_object *intern;
	jsval rval;

	/* retrieve script */
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC,
						"s", &script, &script_len) == FAILURE) {
		RETURN_FALSE;
	}

	intern = (php_jsobject_object *) zend_object_store_get_object(getThis() TSRMLS_CC);
	
	if (JS_EvaluateScript(intern->ct->ct, intern->obj, script, script_len, "spidermonkey_object.c", 60, &rval) == JS_TRUE)
	{
		/* The script evaluated fine, convert the return value to PHP */
		jsval_to_zval(return_value, intern->ct->ct, &rval);
	}
	else
	{
		RETURN_FALSE;
	}

}
/* }}} */

/*
 * Local Variables:
 * c-basic-offset: 4
 * tab-width: 4
 * End:
 * vim600: fdm=marker
 * vim: noet sw=4 ts=4
 */
