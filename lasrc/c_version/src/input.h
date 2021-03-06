#ifndef INPUT_H
#define INPUT_H

#include <stdlib.h>
#include <stdio.h>
#include "input.h"
#include "date.h"
#include "common.h"
#include "espa_metadata.h"
#include "error_handler.h"
#include "raw_binary_io.h"

#define INPUT_FILL (0)
#define ANGLE_FILL (-999.0)
#define WRS_FILL (-1)
#define GAIN_BIAS_FILL (-999.0)

#define WRS1_NPATH 251
#define WRS1_NROW 248
#define WRS2_NPATH 233
#define WRS2_NROW 248

/* band indices which aren't defined */
#define NA -9

/* Structure for the input metadata */
typedef struct {
    Sat_t sat;               /* satellite */
    Inst_t inst;             /* instrument */
    Date_t acq_date;         /* acqsition date/time */
    bool time_fill;          /* acqsition time fill; true = fill value (0h) */
    Date_t prod_date;        /* production date */
    float sun_zen;           /* solar zenith angle (degrees; L8/S2) */
    float sun_az;            /* solar azimuth angle (degrees; L8/S2) */
    float view_zen;          /* view zenith angle (degrees; S2) */
    float view_az;           /* view azimuth angle (degrees; S2) */
    Wrs_t wrs_sys;           /* WRS system */
    int ipath;               /* WRS path number */
    int irow;                /* WRS row number */
    uint16 fill;             /* fill value */
    bool gain_set;                 /* are the gains and biases set? */
    float gain[NBAND_REFL_MAX];    /* reflectance band TOA refl gain */
    float gain_th[NBAND_L8_THM_MAX];  /* therm band brightness temp gain (L8)*/
    float gain_pan[NBAND_L8_PAN_MAX]; /* pan band TOA refl gain (L8 only) */
    float bias[NBAND_REFL_MAX];    /* reflectance band bias */
    float bias_th[NBAND_L8_THM_MAX];  /* thermal band bias (L8 only) */
    float bias_pan[NBAND_L8_PAN_MAX]; /* pan band bias (L8 only) */
    float k1_const[NBAND_L8_THM_MAX]; /* K1 const for thermal bands (L8 only) */
    float k2_const[NBAND_L8_THM_MAX]; /* K2 const for thermal bands (L8 only) */
} Input_meta_t;

/* Structure for the input data */
typedef struct {
    Input_meta_t meta;         /* input metadata */
    int nband;                 /* number of reflectance bands */
    int nband_th;              /* number of thermal bands */
    int nband_pan;             /* number of pan bands */
    int nband_qa;              /* number of QA bands */

    Img_coord_info_t size;     /* input file size */
    Img_coord_info_t size_th;  /* input thermal file size */
    Img_coord_info_t size_pan; /* input pan file size */
    Img_coord_info_t size_qa;  /* input QA file size */
    Img_coord_info_t size_ppa; /* input per-pixel angle file size */

    float scale_factor;       /* scale factor for reflectance bands */
    float scale_factor_th;    /* scale factor for thermal bands */
    float scale_factor_pan;   /* scale factor for pan bands */

    char *file_name[NBAND_REFL_MAX];    /* name of input reflectance files */
    char *file_name_th[NBAND_L8_THM_MAX];  /* name of input therm files (L8) */
    char *file_name_pan[NBAND_L8_PAN_MAX]; /* name of input pan files (L8) */
    char *file_name_qa[NBAND_L8_QA_MAX];   /* name of input QA files (L8) */
    char *file_name_sza;                /* name of input solar zenith files */

    bool open[NBAND_REFL_MAX]; /* flag to indicate whether the specific input
                                  file is open for access; 'true' = open, 
                                  'false' = not open */
    bool open_th[NBAND_L8_THM_MAX];  /* thermal band open flag (L8 only) */
    bool open_pan[NBAND_L8_PAN_MAX]; /* pan band open flag (L8 only) */
    bool open_qa[NBAND_L8_QA_MAX];   /* QA band open flag (L8 only) */
    bool open_ppa;                   /* per-pixel angle bands open flag */

    FILE *fp_bin[NBAND_REFL_MAX]; /* pointer for reflectance binary files */
    FILE *fp_bin_th[NBAND_L8_THM_MAX]; /* ptr for thermal binary files (L8) */
    FILE *fp_bin_pan[NBAND_L8_PAN_MAX];/* ptr for pan binary files (L8 only) */
    FILE *fp_bin_qa[NBAND_L8_QA_MAX];  /* ptr for QA binary files (L8 only) */
    FILE *fp_bin_sza;    /* pointer for solar zenith binary files (L8 only) */
} Input_t;

/* Prototypes */
Input_t *open_input
(
    Espa_internal_meta_t *metadata,     /* I: input metadata */
    bool process_sr                     /* I: will SR data be processed? */
);

void close_input
(
    Input_t *this    /* I: pointer to input data structure */
);

void free_input
(
    Input_t *this    /* I: pointer to input data structure */
);

int get_input_refl_lines
(
    Input_t *this,   /* I: pointer to input data structure */
    int iband,       /* I: current refl band to read (0-based) */
    int iline,       /* I: current line to read (0-based) */
    int nlines,      /* I: number of lines to read */
    int nsamps,      /* I: number of samples to read (S2 nsamps vary depending
                           on the band); if -99 then use the nsamps in the
                           input structure */
    uint16 *out_arr  /* O: output array to populate */
);

int get_input_th_lines
(
    Input_t *this,   /* I: pointer to input data structure */
    int iband,       /* I: current thermal band to read (0-based) */
    int iline,       /* I: current line to read (0-based) */
    int nlines,      /* I: number of lines to read */
    uint16 *out_arr  /* O: output array to populate */
);

int get_input_pan_lines
(
    Input_t *this,   /* I: pointer to input data structure */
    int iband,       /* I: current pan band to read (0-based) */
    int iline,       /* I: current line to read (0-based) */
    int nlines,      /* I: number of lines to read */
    uint16 *out_arr  /* O: output array to populate */
);

int get_input_qa_lines
(
    Input_t *this,   /* I: pointer to input data structure */
    int iband,       /* I: current QA band to read (0-based) */
    int iline,       /* I: current line to read (0-based) */
    int nlines,      /* I: number of lines to read */
    uint16 *out_arr  /* O: output array to populate */
);

int get_input_ppa_lines
(
    Input_t *this,   /* I: pointer to input data structure */
    int iline,       /* I: current line to read (0-based) */
    int nlines,      /* I: number of lines to read */
    int16 *sza_arr  /* O: output solar zenith array to populate */
);

int get_xml_input
(
    Espa_internal_meta_t *metadata,  /* I: XML metadata */
    bool process_sr,                 /* I: will SR data be processed? */
    Input_t *this                    /* O: data structure for the input file */
);

int convert_to_10m
(
    int in_nlines,   /* I: number of lines in the input product */
    int in_nsamps,   /* I: number of samples in the input product */
    int out_nlines,  /* I: number of lines in the output 10m product */
    int out_nsamps,  /* I: number of samples in the output 10m product */
    uint16 *in_arr,  /* I: input array */
    uint16 *out_arr  /* O: output 10m array */
);

#endif
