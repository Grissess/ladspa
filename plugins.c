#include <stdlib.h>
#include <string.h>

#include <ladspa.h>

#define MAKER "Grissess"
#define COPYRIGHT "Grissess 2017, LGPLv2"

/***** Plugin "clip" *****/

/* Clips the input signal to to the interval `[-level, level]`.
 */

typedef struct _Data_clip {
	LADSPA_Data *in;
	LADSPA_Data *out;
	LADSPA_Data *level;
} Data_clip;

Data_clip *clip_instantiate(const LADSPA_Descriptor *desc, unsigned long sr) {
	return calloc(1, sizeof(Data_clip));
}

void clip_connect_port(Data_clip *self, unsigned long port, LADSPA_Data *loc) {
	switch(port) {
		case 0: self->in = loc; break;
		case 1: self->out = loc; break;
		case 2: self->level = loc; break;
	}
}

void clip_run(Data_clip *self, unsigned long samps) {
	LADSPA_Data level;
	unsigned long i;

	if(!self->in || !self->out || !self->level) return;
	level = *(self->level);
	if(level < 0.0) level *= -1.0;
	if(self->in != self->out) memmove(self->out, self->in, samps * sizeof(LADSPA_Data));

	for(i = 0; i < samps; i++) {
		if(self->out[i] > level) self->out[i] = level;
		if(self->out[i] < -level) self->out[i] = -level;
	}
}

void clip_cleanup(Data_clip *self) {
	free(self);
}

const LADSPA_PortDescriptor clip_port_descriptors[] = {
	LADSPA_PORT_INPUT | LADSPA_PORT_AUDIO,
	LADSPA_PORT_OUTPUT | LADSPA_PORT_AUDIO,
	LADSPA_PORT_INPUT | LADSPA_PORT_CONTROL,
};

const char *clip_port_names[] = {
	"in",
	"out",
	"level",
};

const LADSPA_PortRangeHint clip_port_range_hints[] = {
	{0, 0.0, 0.0},
	{0, 0.0, 0.0},
	{LADSPA_HINT_BOUNDED_BELOW | LADSPA_HINT_LOGARITHMIC | LADSPA_HINT_DEFAULT_1, 0.0, 100.0},
};

const LADSPA_Descriptor clip_descriptor = {
	.UniqueID = 0x771,
	.Label = "clip",
	.Properties = LADSPA_PROPERTY_HARD_RT_CAPABLE,
	.Name = "GP: Clipper / Hard Limiter",
	.Maker = MAKER,
	.Copyright = COPYRIGHT,
	.PortCount = 3,
	.PortDescriptors = clip_port_descriptors,
	.PortNames = clip_port_names,
	.PortRangeHints = clip_port_range_hints,
	.ImplementationData = NULL,
	.instantiate = clip_instantiate,
	.connect_port = clip_connect_port,
	.activate = NULL,
	.run = clip_run,
	.run_adding = NULL,
	.set_run_adding_gain = NULL,
	.deactivate = NULL,
	.cleanup = clip_cleanup,
};

/***** Plugin "astro_amp" *****/

/* This is more-or-less a hack to support poor hosts that assume unreasonable
 * upper bounds when LADSPA_HINT_BOUNDED_ABOVE isn't set (e.g., non-mixer sets
 * it to 1.0). If you have a good host, prefer the CMT amp_* instead.
 */

typedef struct _Data_astro_amp {
	LADSPA_Data *in;
	LADSPA_Data *out;
	LADSPA_Data *factor;
} Data_astro_amp;

Data_astro_amp *astro_amp_instantiate(const LADSPA_Descriptor *desc, unsigned long sr) {
	return calloc(1, sizeof(Data_astro_amp));
}

void astro_amp_connect_port(Data_astro_amp *self, unsigned long port, LADSPA_Data *loc) {
	switch(port) {
		case 0: self->in = loc; break;
		case 1: self->out = loc; break;
		case 2: self->factor = loc; break;
	}
}

void astro_amp_run(Data_astro_amp *self, unsigned long samps) {
	LADSPA_Data factor;
	unsigned long i;

	if(!self->in || !self->out || !self->factor) return;
	factor = *(self->factor);
	if(factor < 0.0) factor = 0.0;
	if(self->in != self->out) memmove(self->out, self->in, samps * sizeof(LADSPA_Data));

	for(i = 0; i < samps; i++) {
		self->out[i] *= factor;
	}
}

void astro_amp_cleanup(Data_astro_amp *self) {
	free(self);
}

const LADSPA_PortDescriptor astro_amp_port_descriptors[] = {
	LADSPA_PORT_INPUT | LADSPA_PORT_AUDIO,
	LADSPA_PORT_OUTPUT | LADSPA_PORT_AUDIO,
	LADSPA_PORT_INPUT | LADSPA_PORT_CONTROL,
};

const char *astro_amp_port_names[] = {
	"in",
	"out",
	"factor",
};

const LADSPA_PortRangeHint astro_amp_port_range_hints[] = {
	{0, 0.0, 0.0},
	{0, 0.0, 0.0},
	{LADSPA_HINT_BOUNDED_BELOW | LADSPA_HINT_BOUNDED_ABOVE | LADSPA_HINT_LOGARITHMIC | LADSPA_HINT_DEFAULT_1, 0.0, 100.0},
};

const LADSPA_Descriptor astro_amp_descriptor = {
	.UniqueID = 0x772,
	.Label = "astro_amp",
	.Properties = LADSPA_PROPERTY_HARD_RT_CAPABLE,
	.Name = "GP: Astronomical Amplifier",
	.Maker = MAKER,
	.Copyright = COPYRIGHT,
	.PortCount = 3,
	.PortDescriptors = astro_amp_port_descriptors,
	.PortNames = astro_amp_port_names,
	.PortRangeHints = astro_amp_port_range_hints,
	.ImplementationData = NULL,
	.instantiate = astro_amp_instantiate,
	.connect_port = astro_amp_connect_port,
	.activate = NULL,
	.run = astro_amp_run,
	.run_adding = NULL,
	.set_run_adding_gain = NULL,
	.deactivate = NULL,
	.cleanup = astro_amp_cleanup,
};

/***** Plugin "clicks_pops" *****/

/* Generates a bunch of clicks and pops like an old record might.  This is an
 * intentionally quite naive algorithm; it uses the platform random() PRNG and
 * will generate white noise if you set the threshold low.
 */

typedef struct _Data_clicks_pops {
	LADSPA_Data *out;
	LADSPA_Data *rthres;
} Data_clicks_pops;

Data_clicks_pops *clicks_pops_instantiate(const LADSPA_Descriptor *desc, unsigned long sr) {
	return calloc(1, sizeof(Data_clicks_pops));
}

void clicks_pops_connect_port(Data_clicks_pops *self, unsigned long port, LADSPA_Data *loc) {
	switch(port) {
		case 0: self->out = loc; break;
		case 1: self->rthres = loc; break;
	}
}

void clicks_pops_run(Data_clicks_pops *self, unsigned long samps) {
	LADSPA_Data rthres;
	unsigned long i;

	if(!self->out || !self->rthres) return;

	rthres = *(self->rthres);

	for(i = 0; i < samps; i++) {
		self->out[i] = ((random() / (float) RAND_MAX) <= rthres)? (random() / (LADSPA_Data) RAND_MAX) : 0.0;
	}
}

void clicks_pops_cleanup(Data_clicks_pops *self) {
	free(self);
}

const LADSPA_PortDescriptor clicks_pops_port_descriptors[] = {
	LADSPA_PORT_OUTPUT | LADSPA_PORT_AUDIO,
	LADSPA_PORT_INPUT | LADSPA_PORT_CONTROL,
};

const char *clicks_pops_port_names[] = {
	"out",
	"rthres",
};

const LADSPA_PortRangeHint clicks_pops_port_range_hints[] = {
	{0, 0.0, 0.0},
	{LADSPA_HINT_BOUNDED_BELOW | LADSPA_HINT_BOUNDED_ABOVE | LADSPA_HINT_DEFAULT_LOW, 0.0, 1.0},
};

const LADSPA_Descriptor clicks_pops_descriptor = {
	.UniqueID = 0x773,
	.Label = "clicks_pops",
	.Properties = LADSPA_PROPERTY_HARD_RT_CAPABLE,
	.Name = "GP: Clicks and Pops",
	.Maker = MAKER,
	.Copyright = COPYRIGHT,
	.PortCount = 2,
	.PortDescriptors = clicks_pops_port_descriptors,
	.PortNames = clicks_pops_port_names,
	.PortRangeHints = clicks_pops_port_range_hints,
	.ImplementationData = NULL,
	.instantiate = clicks_pops_instantiate,
	.connect_port = clicks_pops_connect_port,
	.activate = NULL,
	.run = clicks_pops_run,
	.run_adding = NULL,
	.set_run_adding_gain = NULL,
	.deactivate = NULL,
	.cleanup = clicks_pops_cleanup,
};

/***** Core. *****/

const LADSPA_Descriptor *descriptors[] = {
	&clip_descriptor,
	&astro_amp_descriptor,
	&clicks_pops_descriptor,
};

const LADSPA_Descriptor *ladspa_descriptor(unsigned long idx) {
	if(idx >= sizeof(descriptors) / sizeof(descriptors[0])) return NULL;

	return descriptors[idx];
}
