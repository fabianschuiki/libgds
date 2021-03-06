${HEADER_PREFIX}

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

// Library version
#define GDS_VERSION_MAJOR @GDS_VERSION_MAJOR@
#define GDS_VERSION_MINOR @GDS_VERSION_MINOR@
#define GDS_VERSION_PATCH @GDS_VERSION_PATCH@
#define GDS_VERSION_STRING "@GDS_VERSION@"

// Definition of types used throughout the library.
typedef struct gds_reader gds_reader_t;
typedef struct gds_writer gds_writer_t;
typedef struct gds_lib gds_lib_t;
typedef struct gds_units gds_units_t;
typedef struct gds_struct gds_struct_t;
typedef struct gds_elem gds_elem_t;
typedef struct gds_xy gds_xy_t;
typedef struct gds_strans gds_strans_t;
typedef struct gds_record gds_record_t;
typedef struct gds_parser gds_parser_t;

typedef int32_t gds_real32_t;
typedef int64_t gds_real64_t;
typedef int16_t gds_version_t;
typedef uint8_t gds_layer_t;    /* 0..63, see manual section 4-5 */
typedef uint8_t gds_datatype_t; /* 0..63, see manual section 4-5 */


/* Errors generated by the library. Negative error numbers indicate a system
 * error as given by (-errno). */
enum gds_error {
	GDS_OK            = 0,
	GDS_ERR_NO_MEM    = 1,
	GDS_ERR_UNEXP_EOF = 2,
	GDS_ERR_SYNTAX    = 3,
	GDS_MAX_ERR = GDS_ERR_UNEXP_EOF,
};


/* GDS Data Types (see manual section 3) */
enum gds_data_type {
	GDS_DT_NODATA   = 0x00,
	GDS_DT_BITARRAY = 0x01,
	GDS_DT_SIGNED16 = 0x02,
	GDS_DT_SIGNED32 = 0x03,
	GDS_DT_REAL32   = 0x04,
	GDS_DT_REAL64   = 0x05,
	GDS_DT_STRING   = 0x06,
	GDS_MAX_DT      = GDS_DT_STRING,
};

/* GDS Record Types (see manual section 4) */
enum gds_record_type {
	GDS_RT_HEADER       = 0x0002,
	GDS_RT_BGNLIB       = 0x0102,
	GDS_RT_LIBNAME      = 0x0206,
	GDS_RT_UNITS        = 0x0305,
	GDS_RT_ENDLIB       = 0x0400,
	GDS_RT_BGNSTR       = 0x0502,
	GDS_RT_STRNAME      = 0x0606,
	GDS_RT_ENDSTR       = 0x0700,
	GDS_RT_BOUNDARY     = 0x0800,
	GDS_RT_PATH         = 0x0900,
	GDS_RT_SREF         = 0x0A00,
	GDS_RT_AREF         = 0x0B00,
	GDS_RT_TEXT         = 0x0C00,
	GDS_RT_LAYER        = 0x0D02,
	GDS_RT_DATATYPE     = 0x0E02,
	GDS_RT_WIDTH        = 0x0F03,
	GDS_RT_XY           = 0x1003,
	GDS_RT_ENDEL        = 0x1100,
	GDS_RT_SNAME        = 0x1206,
	GDS_RT_COLROW       = 0x1302,
	GDS_RT_TEXTNODE     = 0x1400,
	GDS_RT_NODE         = 0x1500,
	GDS_RT_TEXTTYPE     = 0x1602,
	GDS_RT_PRESENTATION = 0x1701,
	GDS_RT_SPACING      = 0x1800,
	GDS_RT_STRING       = 0x1906,
	GDS_RT_STRANS       = 0x1A01,
	GDS_RT_MAG          = 0x1B05,
	GDS_RT_ANGLE        = 0x1C05,
	GDS_RT_UINTEGER     = 0x1D00,
	GDS_RT_USTRING      = 0x1E00,
	GDS_RT_REFLIBS      = 0x1F06,
	GDS_RT_FONTS        = 0x2006,
	GDS_RT_PATHTYPE     = 0x2102,
	GDS_RT_GENERATIONS  = 0x2202,
	GDS_RT_ATTRTABLE    = 0x2306,
	GDS_RT_STYPTABLE    = 0x2406,
	GDS_RT_STRTYPE      = 0x2502,
	GDS_RT_ELFLAGS      = 0x2601,
	GDS_RT_ELKEY        = 0x2703,
	GDS_RT_LINKTYPE     = 0x2800,
	GDS_RT_LINKKEYS     = 0x2900,
	GDS_RT_NODETYPE     = 0x2A02,
	GDS_RT_PROPATTR     = 0x2B02,
	GDS_RT_PROPVALUE    = 0x2C06,
	GDS_RT_BOX          = 0x2D00,
	GDS_RT_BOXTYPE      = 0x2E02,
	GDS_RT_PLEX         = 0x2F03,
	GDS_RT_BGNEXTN      = 0x3003,
	GDS_RT_ENDEXTN      = 0x3103,
	GDS_RT_TAPENUM      = 0x3202,
	GDS_RT_TAPECODE     = 0x3302,
	GDS_RT_STRCLASS     = 0x3401,
	GDS_RT_RESERVED     = 0x3503,
	GDS_RT_FORMAT       = 0x3602,
	GDS_RT_MASK         = 0x3706,
	GDS_RT_ENDMASKS     = 0x3800,
	GDS_RT_LIBDIRSIZE   = 0x3902,
	GDS_RT_SRFNAME      = 0x3A06,
	GDS_RT_LIBSECUR     = 0x3B02,
	GDS_MAX_RT          = GDS_RT_LIBSECUR,
};


/* GDS Records */
struct gds_time {
	int16_t year, month, day, hour, minute, second;
};

struct gds_xy {
	int32_t x,y;
};

/* To be used as a pointer. sizeof(struct gds_record) is not valid. Array sizes
 * are mere maximums as per the manual. */
struct gds_record {
	uint16_t size;
	uint16_t type;
	union {
		int16_t version;
		struct {
			struct gds_time mtime, atime;
		} bgnlib;
		char libname[0];
		struct {
			gds_real64_t dbu_in_uu, dbu_in_m;
		} units;
		struct {
			struct gds_time ctime, mtime;
		} bgnstr;
		char strname[32];
		int16_t layer;
		int16_t datatype;
		int32_t width;
		struct gds_xy xy[8191];
		char sname[32];
		struct {
			int16_t c, r;
		} colrow;
		int16_t texttype;
		int16_t presentation; /* enum gds_presentation */
		char string[512];
		int16_t strans; /* enum gds_strans */
		gds_real64_t mag;
		gds_real64_t angle;
		/* 29 UINTEGER no longer used */
		/* 30 USTRING no longer used */
		char reflibs[2][45];
		char fonts[4][44];
		int16_t pathtype; /* enum gds_pathtype */
		int16_t generations;
		char attrtable[44];
		/* 36 STYPTABLE unreleased feature */
		/* 37 STRTYPE unreleased feature */
		int16_t elflags; /* enum gds_elflags */
		/* 39 ELKEY unreleased feature */
		/* 40 LINKTYPE unreleased feature */
		/* 41 LINKKEYS unreleased feature */
		int16_t nodetype; /* 0..63 */
		int16_t propattr; /* 1..127 */
		char propvalue[126];
		int16_t boxtype; /* 0..63 */
		int32_t plex; /* enum gds_plex */
	} data;
} GDS_PACKED;

/* see manual section 4-1 */
enum gds_version {
	GDS_VERSION_UNKNOWN = 0,
	GDS_VERSION_3       = 3,
	GDS_VERSION_4       = 4,
	GDS_VERSION_5       = 5,
	GDS_VERSION_6       = 600,
};

/* see manual section 4-8 and 4-9 */
enum gds_presentation {
	GDS_PRES_MASK_H    = 0x0003,
	GDS_PRES_MASK_V    = 0x000C,
	GDS_PRES_MASK_FONT = 0x0030,

	/* Vertical Presentation */
	GDS_PRES_TOP    = 0x0,
	GDS_PRES_MIDDLE = 0x1,
	GDS_PRES_BOTTOM = 0x2,

	/* Horizontal Presentation */
	GDS_PRES_LEFT   = 0x0,
	GDS_PRES_CENTER = 0x1,
	GDS_PRES_RIGHT  = 0x2,
};

/* STRANS flags, see manual section 4-10 */
enum {
	GDS_STRANS_ABSMAG     = 0x0002,
	GDS_STRANS_ABSANGLE   = 0x0004,
	GDS_STRANS_REFLECTION = 0x8000,
};

/* see manual section 4-12 and 4-13 */
enum gds_pathtype {
	GDS_PATHTYPE_FLUSH  = 0x0,
	GDS_PATHTYPE_ROUND  = 0x1,
	GDS_PATHTYPE_SQUARE = 0x2,
	/* pathtype 4 not supported */
};

/* see manual section 4-14 and 4-15 */
enum gds_elflags {
	GDS_ELFLAG_TEMPLATE = 0x0001,
	GDS_ELFLAG_EXTERNAL = 0x0002,
};

/* see manual section 4-17 */
enum gds_plex {
	GDS_PLEX_HEAD       = 0x01000000,
	GDS_PLEX_VALID_BITS = 0x00FFFFFF,
};


GDS_API const char *gds_errstr(int);
GDS_API const char *gds_record_type_str(uint16_t rt);
GDS_API const char *gds_data_type_str(uint16_t dt);

GDS_API gds_real64_t gds_double_to_real64(double);
GDS_API gds_real32_t gds_double_to_real32(double);
GDS_API double gds_real64_to_double(gds_real64_t);
GDS_API double gds_real32_to_float(gds_real32_t);

/* Streaming data in */
GDS_API int gds_reader_open_file(gds_reader_t**, const char *, int);
GDS_API int gds_read(gds_reader_t*, gds_record_t**);
GDS_API void gds_reader_close(gds_reader_t*);

/* Streaming data out */
GDS_API int gds_writer_open_file(gds_writer_t**, const char *, int);
GDS_API int gds_write(gds_writer_t*, gds_record_t *);
GDS_API int gds_write_record_hdr(gds_writer_t*, uint16_t, uint16_t);
GDS_API int gds_write_record_data(gds_writer_t*, uint16_t, size_t, void*);
GDS_API int gds_write_void(gds_writer_t*, uint16_t);
GDS_API int gds_write_string(gds_writer_t*, uint16_t, const char*);
GDS_API void gds_writer_close(gds_writer_t*);

/* Parse stream of records */
GDS_API gds_parser_t *gds_parser_create(gds_reader_t*);
GDS_API void gds_parser_destroy(gds_parser_t*);
GDS_API int gds_parser_next(gds_parser_t*);
GDS_API int gds_parser_get_error(gds_parser_t *parser);
GDS_API gds_record_t *gds_parser_get_record(gds_parser_t*);
GDS_API char *gds_parser_copy_string(gds_parser_t*);
GDS_API int gds_parser_accept(gds_parser_t*, uint16_t);
GDS_API int gds_parser_require(gds_parser_t*, uint16_t);
GDS_API int gds_parser_skip_until(gds_parser_t*, uint16_t);


/* Library */
struct gds_units {
	double dbu_in_uu;
	double dbu_in_m;
};

struct gds_strans {
	uint16_t flags;
	double mag, angle;
};

enum gds_elem_kind {
	GDS_ELEM_BOUNDARY = 1,
	GDS_ELEM_PATH     = 2,
	GDS_ELEM_SREF     = 3,
	GDS_ELEM_AREF     = 4,
	GDS_ELEM_TEXT     = 5,
	GDS_ELEM_NODE     = 6,
	GDS_ELEM_BOX      = 7,
};

GDS_API int gds_lib_read(gds_lib_t**, gds_reader_t*);
GDS_API int gds_lib_write(gds_lib_t*, gds_writer_t*);
GDS_API gds_lib_t *gds_lib_create();
GDS_API void gds_lib_destroy(gds_lib_t*);
GDS_API void gds_lib_set_version(gds_lib_t*, uint16_t);
GDS_API uint16_t gds_lib_get_version(gds_lib_t*);
GDS_API void gds_lib_set_name(gds_lib_t*, const char*);
GDS_API const char *gds_lib_get_name(gds_lib_t*);
GDS_API void gds_lib_set_units(gds_lib_t*, gds_units_t);
GDS_API gds_units_t gds_lib_get_units(gds_lib_t*);
GDS_API void gds_lib_add_struct(gds_lib_t*, gds_struct_t*);
GDS_API void gds_lib_remove_struct(gds_lib_t*, gds_struct_t*);
GDS_API size_t gds_lib_get_num_structs(gds_lib_t*);
GDS_API gds_struct_t *gds_lib_get_struct(gds_lib_t*, size_t);

GDS_API bool gds_struct_read(gds_struct_t**, gds_parser_t*);
GDS_API int gds_struct_write(gds_struct_t*, gds_writer_t*);
GDS_API gds_struct_t *gds_struct_create(const char*);
GDS_API void gds_struct_ref(gds_struct_t*);
GDS_API void gds_struct_unref(gds_struct_t*);
GDS_API const char *gds_struct_get_name(gds_struct_t*);
GDS_API void gds_struct_add_elem(gds_struct_t*, gds_elem_t*);
GDS_API void gds_struct_remove_elem(gds_struct_t*, gds_elem_t*);
GDS_API size_t gds_struct_get_num_elems(gds_struct_t*);
GDS_API gds_elem_t *gds_struct_get_elem(gds_struct_t*, size_t);

GDS_API bool gds_elem_read(gds_elem_t**, gds_parser_t*);
GDS_API int gds_elem_write(gds_elem_t*, gds_writer_t*);
GDS_API gds_elem_t *gds_elem_create_boundary(uint8_t, uint8_t, uint8_t, gds_xy_t*);
GDS_API gds_elem_t *gds_elem_create_path(uint8_t, uint8_t, uint8_t, gds_xy_t*);
GDS_API gds_elem_t *gds_elem_create_sref(const char*, gds_xy_t);
GDS_API gds_elem_t *gds_elem_create_aref(const char*, uint16_t, uint16_t, gds_xy_t, gds_xy_t, gds_xy_t);
GDS_API gds_elem_t *gds_elem_create_text(uint8_t, uint8_t, gds_xy_t, const char*);
GDS_API void gds_elem_destroy(gds_elem_t*);
GDS_API int gds_elem_get_kind(gds_elem_t*);
GDS_API uint16_t gds_elem_get_layer(gds_elem_t*);
GDS_API uint16_t gds_elem_get_type(gds_elem_t*);
GDS_API void gds_elem_set_strans(gds_elem_t*, gds_strans_t);
GDS_API gds_strans_t gds_elem_get_strans(gds_elem_t*);
GDS_API gds_xy_t *gds_elem_get_xy(gds_elem_t*);
GDS_API uint16_t gds_elem_get_num_xy(gds_elem_t*);
GDS_API const char *gds_elem_get_sname(gds_elem_t*);
GDS_API const char *gds_elem_get_text(gds_elem_t*);


${HEADER_SUFFIX}
