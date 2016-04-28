/* Copyright (c) 2016 Fabian Schuiki */
#include "common.h"
#include "util.h"


struct gds_lib {
	// HEADER
	gds_version_t version;
	// BGNLIB
	struct gds_time mtime, atime;
	// LIBDIRSIZE
	// SRFNAME
	// LIBSECUR
	// LIBNAME
	char *name;
	// REFLIBS
	// FONTS
	// ATTRTABLE
	// GENERATIONS
	// <FormatType>
	// UNITS
	struct gds_units units;
	// {<structure>}*
	ptrset_t structs;
};


/**
 * Reads a GDS library from a GDS input stream.
 */
int
gds_lib_read(struct gds_lib **out, gds_reader_t *rd) {
	gds_parser_t *parser;
	gds_lib_t *lib;
	int err;

	assert(out && rd);
	parser = gds_parser_create(rd);
	lib = gds_lib_create();

	// HEADER
	if (gds_parser_require(parser, GDS_RT_HEADER)) {
		lib->version = gds_parser_get_record(parser)->data.version;
		/// @todo Assert version is valid.
		gds_parser_next(parser);
	}

	// BGNLIB
	if (gds_parser_require(parser, GDS_RT_BGNLIB)) {
		gds_record_t *rec = gds_parser_get_record(parser);
		lib->mtime = rec->data.bgnlib.mtime;
		lib->atime = rec->data.bgnlib.atime;
		gds_parser_next(parser);
	}

	// LIBDIRSIZE
	// SRFNAME
	// LIBSECUR

	// LIBNAME
	if (gds_parser_skip_until(parser, GDS_RT_LIBNAME)) {
		lib->name = gds_parser_copy_string(parser);
		gds_parser_next(parser);
	}

	// REFLIBS
	// FONTS
	// ATTRTABLE
	// GENERATIONS
	// <FormatType>

	// UNITS
	if (gds_parser_skip_until(parser, GDS_RT_UNITS)) {
		gds_record_t *rec = gds_parser_get_record(parser);
		lib->units.dbu_in_uu = gds_real64_to_double(rec->data.units.dbu_in_uu);
		lib->units.dbu_in_m  = gds_real64_to_double(rec->data.units.dbu_in_m);
		gds_parser_next(parser);
	}

	// {<structure>}*
	gds_struct_t *str;
	while (gds_struct_read(&str, parser)) {
		gds_lib_add_struct(lib, str);
		gds_struct_unref(str);
	}

	// ENDLIB
	gds_parser_require(parser, GDS_RT_ENDLIB);
	gds_parser_next(parser);

	err = gds_parser_get_error(parser);
	if (err == GDS_OK) {
		*out = lib;
	} else {
		gds_lib_destroy(lib);
		gds_parser_destroy(parser);
	}
	return err;
}


/**
 * Writes a GDS library to a GDS output stream.
 */
int
gds_lib_write(struct gds_lib *lib, struct gds_writer *wr) {
	int err = GDS_OK;
	assert(lib && wr);
	struct gds_record *R = malloc(1 << 16);

	// HEADER
	R->size = 6;
	R->type = GDS_RT_HEADER;
	R->data.version = lib->version;
	err = gds_write(wr, R);
	if (err != GDS_OK)
		return err;

	// BGNLIB
	memset(R, 0, 28);
	R->size = 28;
	R->type = GDS_RT_BGNLIB;
	err = gds_write(wr, R);
	if (err != GDS_OK)
		return err;

	// LIBNAME
	assert(lib->name);
	R->size = 4 + strlen(lib->name);
	R->type = GDS_RT_LIBNAME;
	strcpy(R->data.libname, lib->name);
	err = gds_write(wr, R);
	if (err != GDS_OK)
		return err;

	// UNITS
	R->size = 20;
	R->type = GDS_RT_UNITS;
	R->data.units.dbu_in_uu = gds_double_to_real64(lib->units.dbu_in_uu);
	R->data.units.dbu_in_m  = gds_double_to_real64(lib->units.dbu_in_m);

	err = gds_write(wr, R);
	if (err != GDS_OK)
		return err;

	// {<structure>}*
	for (size_t z = 0; z < lib->structs.size; ++z) {
		err = gds_struct_write(lib->structs.items[z], wr);
		if (err != GDS_OK)
			return err;
	}
	/// @todo Add support for a callback to write additional structs here.

	// ENDLIB
	err = gds_write_void(wr, GDS_RT_ENDLIB);
	if (err != GDS_OK)
		return err;

finish:
	free(R);
	return err;
}


struct gds_lib *
gds_lib_create() {
	struct gds_lib *lib = calloc(1, sizeof(*lib));
	ptrset_init(&lib->structs);
	lib->units.dbu_in_uu = 1e-3;
	lib->units.dbu_in_m = 1e-9;
	return lib;
}


void
gds_lib_destroy(struct gds_lib *lib) {
	assert(lib);
	if (lib->name) free(lib->name);
	for (size_t z = 0; z != lib->structs.size; ++z)
		gds_struct_unref(lib->structs.items[z]);
	ptrset_dispose(&lib->structs);
	free(lib);
}


void
gds_lib_set_version(struct gds_lib *lib, uint16_t version) {
	assert(lib);
	/// @todo Assert version is correct.
	lib->version = version;
}


uint16_t
gds_lib_get_version(struct gds_lib *lib) {
	assert(lib);
	return lib->version;
}


void
gds_lib_set_name(struct gds_lib *lib, const char *name) {
	assert(lib && name);
	if (lib->name) free(lib->name);
	lib->name = gds_dupstr(name);
}


const char *
gds_lib_get_name(struct gds_lib *lib) {
	assert(lib);
	return lib->name;
}


void
gds_lib_set_units(struct gds_lib *lib, struct gds_units units) {
	assert(lib);
	lib->units = units;
}


struct gds_units
gds_lib_get_units(struct gds_lib *lib) {
	assert(lib);
	return lib->units;
}


void
gds_lib_add_struct(gds_lib_t *lib, gds_struct_t *str) {
	assert(lib && str);
	if (ptrset_add(&lib->structs, str))
		gds_struct_ref(str);
}


void
gds_lib_remove_struct(gds_lib_t *lib, gds_struct_t *str) {
	assert(lib && str);
	if (ptrset_remove(&lib->structs, str))
		gds_struct_unref(str);
}


size_t
gds_lib_get_num_structs(gds_lib_t *lib) {
	assert(lib);
	return lib->structs.size;
}


gds_struct_t *
gds_lib_get_struct(gds_lib_t *lib, size_t idx) {
	assert(lib && idx < lib->structs.size);
	return lib->structs.items[idx];
}
