# LADSPA Plugins

This is a small collection of very simple LADSPA plugins for my own use. Don't
expect this to be feature-complete, nor particularly user-friendly; I do,
however, strive to keep them stable, so let me know if they unexpectedly crash
in a host. (Save your work frequently!)

Automatically-generated documentation follows below. For ports, review the
source (`plugins.c`).

## Plugin clip
Clips the input signal to to the interval `[-level, level]`.

## Plugin astro_amp
This is more-or-less a hack to support poor hosts that assume unreasonable
upper bounds when LADSPA_HINT_BOUNDED_ABOVE isn't set (e.g., non-mixer sets
it to 1.0). If you have a good host, prefer the CMT amp_* instead.

## Plugin clicks_pops
Generates a bunch of clicks and pops like an old record might.  This is an
intentionally quite naive algorithm; it uses the platform random() PRNG and
will generate white noise if you set the threshold low.
