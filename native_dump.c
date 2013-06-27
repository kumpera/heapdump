#include <stdio.h>

#include <mono/metadata/class.h>
#include <mono/metadata/object.h>
#include <glib.h>


typedef struct {
	MonoClass *klass;
	int instance_count;
	int instance_size;
} entry_t;

static GHashTable *counts;
static FILE *dump_file;
static int obj_count, obj_size;
static int
gc_reference (MonoObject *obj, MonoClass *klass, uintptr_t size, uintptr_t num, MonoObject **refs, uintptr_t *offsets, void *data)
{
	entry_t *entry = (entry_t*)g_hash_table_lookup (counts, klass);
	if (!entry) {
		entry = g_new0 (entry_t, 1);
		entry->klass = klass;
		g_hash_table_insert (counts, klass, entry);
	}

	entry->instance_count++;
	entry->instance_size += mono_object_get_size (obj);
	obj_count++;
	obj_size += entry->instance_size;
}


void
dump_count (void *key, void *val, void *ud)
{
	MonoClass *klass = key;
	entry_t *entry = val;

	char *name = mono_type_get_name (mono_class_get_type (entry->klass));
	fprintf (dump_file, "{ \"count\": %d, \"size\": %d, \"name\": \"%s\"},\n", entry->instance_count, entry->instance_size, name);
	g_free (name);
	g_free (entry);
}

//sharedlibrary
int dump_mono_heap (const char *output_file)
{
	printf ("---dumping the heap!\n");
	counts = g_hash_table_new (NULL, NULL);
	obj_count = obj_size = 0;

	mono_gc_collect (1);
	mono_gc_walk_heap (0, gc_reference, NULL);
	dump_file = fopen (output_file, "w");
	fprintf (dump_file, "[\n");
	g_hash_table_foreach (counts, dump_count, NULL);
	g_hash_table_destroy (counts);
	fprintf (dump_file, "{ \"count\": 0, \"size\": 0, \"name\": \"json sucks\"}\n");
	fprintf (dump_file, "]\n");
	fclose (dump_file);
	counts = NULL;
	dump_file = NULL;
	printf ("found %d objc size %d\n", obj_count, obj_size);

	return 0x12345678;
}

/*

gcc -m32  -I.. foo.c -dynamiclib -undefined suppress -flat_namespace  -o libfoo.dylib

p (void*)dlopen ("/Users/kumpera/src/mono/mono/libfoo.dylib",0)
p (void*)dlsym ((void*)0xc4a76b0,"dump_mono_heap")
p (int)dump_mono_heap()

*/

