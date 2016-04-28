/* Copyright (c) 2016 Fabian Schuiki */
#include "common.h"
#include "util.h"


static const char *gds_dt_names[] = {
	[GDS_DT_NODATA  ] = "NODATA",
	[GDS_DT_BITARRAY] = "BITARRAY",
	[GDS_DT_SIGNED16] = "SIGNED16",
	[GDS_DT_SIGNED32] = "SIGNED32",
	[GDS_DT_REAL32  ] = "REAL32",
	[GDS_DT_REAL64  ] = "REAL64",
	[GDS_DT_STRING  ] = "STRING",
};

static const char *gds_rt_names[] = {
	[GDS_RT_HEADER       >> 8] = "HEADER",
	[GDS_RT_BGNLIB       >> 8] = "BGNLIB",
	[GDS_RT_LIBNAME      >> 8] = "LIBNAME",
	[GDS_RT_UNITS        >> 8] = "UNITS",
	[GDS_RT_ENDLIB       >> 8] = "ENDLIB",
	[GDS_RT_BGNSTR       >> 8] = "BGNSTR",
	[GDS_RT_STRNAME      >> 8] = "STRNAME",
	[GDS_RT_ENDSTR       >> 8] = "ENDSTR",
	[GDS_RT_BOUNDARY     >> 8] = "BOUNDARY",
	[GDS_RT_PATH         >> 8] = "PATH",
	[GDS_RT_SREF         >> 8] = "SREF",
	[GDS_RT_AREF         >> 8] = "AREF",
	[GDS_RT_TEXT         >> 8] = "TEXT",
	[GDS_RT_LAYER        >> 8] = "LAYER",
	[GDS_RT_DATATYPE     >> 8] = "DATATYPE",
	[GDS_RT_WIDTH        >> 8] = "WIDTH",
	[GDS_RT_XY           >> 8] = "XY",
	[GDS_RT_ENDEL        >> 8] = "ENDEL",
	[GDS_RT_SNAME        >> 8] = "SNAME",
	[GDS_RT_COLROW       >> 8] = "COLROW",
	[GDS_RT_TEXTNODE     >> 8] = "TEXTNODE",
	[GDS_RT_NODE         >> 8] = "NODE",
	[GDS_RT_TEXTTYPE     >> 8] = "TEXTTYPE",
	[GDS_RT_PRESENTATION >> 8] = "PRESENTATION",
	[GDS_RT_SPACING      >> 8] = "SPACING",
	[GDS_RT_STRING       >> 8] = "STRING",
	[GDS_RT_STRANS       >> 8] = "STRANS",
	[GDS_RT_MAG          >> 8] = "MAG",
	[GDS_RT_ANGLE        >> 8] = "ANGLE",
	[GDS_RT_UINTEGER     >> 8] = "UINTEGER",
	[GDS_RT_USTRING      >> 8] = "USTRING",
	[GDS_RT_REFLIBS      >> 8] = "REFLIBS",
	[GDS_RT_FONTS        >> 8] = "FONTS",
	[GDS_RT_PATHTYPE     >> 8] = "PATHTYPE",
	[GDS_RT_GENERATIONS  >> 8] = "GENERATIONS",
	[GDS_RT_ATTRTABLE    >> 8] = "ATTRTABLE",
	[GDS_RT_STYPTABLE    >> 8] = "STYPTABLE",
	[GDS_RT_STRTYPE      >> 8] = "STRTYPE",
	[GDS_RT_ELFLAGS      >> 8] = "ELFLAGS",
	[GDS_RT_ELKEY        >> 8] = "ELKEY",
	[GDS_RT_LINKTYPE     >> 8] = "LINKTYPE",
	[GDS_RT_LINKKEYS     >> 8] = "LINKKEYS",
	[GDS_RT_NODETYPE     >> 8] = "NODETYPE",
	[GDS_RT_PROPATTR     >> 8] = "PROPATTR",
	[GDS_RT_PROPVALUE    >> 8] = "PROPVALUE",
	[GDS_RT_BOX          >> 8] = "BOX",
	[GDS_RT_BOXTYPE      >> 8] = "BOXTYPE",
	[GDS_RT_PLEX         >> 8] = "PLEX",
	[GDS_RT_BGNEXTN      >> 8] = "BGNEXTN",
	[GDS_RT_ENDEXTN      >> 8] = "ENDEXTN",
	[GDS_RT_TAPENUM      >> 8] = "TAPENUM",
	[GDS_RT_TAPECODE     >> 8] = "TAPECODE",
	[GDS_RT_STRCLASS     >> 8] = "STRCLASS",
	[GDS_RT_RESERVED     >> 8] = "RESERVED",
	[GDS_RT_FORMAT       >> 8] = "FORMAT",
	[GDS_RT_MASK         >> 8] = "MASK",
	[GDS_RT_ENDMASKS     >> 8] = "ENDMASKS",
	[GDS_RT_LIBDIRSIZE   >> 8] = "LIBDIRSIZE",
	[GDS_RT_SRFNAME      >> 8] = "SRFNAME",
	[GDS_RT_LIBSECUR     >> 8] = "LIBSECUR",
};

const char *
gds_record_type_str(uint16_t rt) {
	if (rt > GDS_MAX_RT)
		return NULL;
	return gds_rt_names[rt>>8];
}

const char *
gds_data_type_str(uint16_t dt) {
	dt &= 0xFF;
	if (dt > GDS_MAX_DT)
		return NULL;
	return gds_dt_names[dt];
}


void
be16_to_host(void *buffer, size_t length) {
	for (; length >= 2; length -= 2, buffer += 2) {
		uint8_t *i = buffer;
		uint16_t *o = buffer;
		*o = (uint16_t)i[0] << 8 | (uint16_t)i[1] << 0;
	}
}

void
be32_to_host(void *buffer, size_t length) {
	for (; length >= 4; length -= 4, buffer += 4) {
		uint8_t *i = buffer;
		uint32_t *o = buffer;
		*o = (uint32_t)i[0] << 24 | (uint32_t)i[1] << 16 |
		     (uint32_t)i[2] <<  8 | (uint32_t)i[3] <<  0;
	}
}

void
be64_to_host(void *buffer, size_t length) {
	for (; length >= 8; length -= 8, buffer += 8) {
		uint8_t *i = buffer;
		uint64_t *o = buffer;
		*o = (uint64_t)i[0] << 56 | (uint64_t)i[1] << 48 |
		     (uint64_t)i[2] << 40 | (uint64_t)i[3] << 32 |
		     (uint64_t)i[4] << 24 | (uint64_t)i[5] << 16 |
		     (uint64_t)i[6] <<  8 | (uint64_t)i[7] <<  0;
	}
}


static const char *gds_error_names[] = {
	[GDS_OK]            = "OK",
	[GDS_ERR_NO_MEM]    = "No memory",
	[GDS_ERR_UNEXP_EOF] = "Unexpected end of file",
	[GDS_ERR_SYNTAX]    = "Syntax error",
};

/* Returns a string representation of a GDS error, or NULL if an invalid error
 * number is passed. */
const char *
gds_errstr(int error) {
	if (error < 0)
		return (const char *)strerror(-error);
	else if (error >= (int)(sizeof(gds_error_names)/sizeof(*gds_error_names)))
		return NULL;
	else
		return gds_error_names[error];
}


char *
gds_dupstr(const char *src) {
	if (!src) return NULL;
	size_t len = strlen(src);
	char *dst = malloc(len+1);
	memcpy(dst, src, len);
	dst[len] = 0;
	return dst;
}


char *
gds_dupstrn(const char *src, size_t len) {
	if (!src) return NULL;
	char *dst = malloc(len+1);
	memcpy(dst, src, len);
	dst[len] = 0;
	return dst;
}


void *
gds_dupmem(const void *src, size_t len) {
	if (!src) return NULL;
	void *dst = malloc(len);
	memcpy(dst, src, len);
	return dst;
}
