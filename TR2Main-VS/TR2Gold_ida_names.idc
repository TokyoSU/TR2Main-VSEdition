#include <idc.idc>

static apply_name(ea, name, fallback)
{
    auto flags;
    flags = SN_NOCHECK | SN_NOWARN;
    if (set_name(ea, name, flags))
        return 1;
    if (set_name(ea, fallback, flags))
    {
        msg("Used %s at %X.\n", fallback, ea);
        return 1;
    }
    msg("Could not apply %s at %X.\n", name, ea);
    return 0;
}

static main()
{
    auto delta, applied;
    delta = get_imagebase() - 0x00400000;
    applied = 0;
    applied = applied + apply_name(delta + 0x0040C860, "BearControl", "BearControl_0x0040C860");
    applied = applied + apply_name(delta + 0x004431C0, "InitialiseWolf", "InitialiseWolf_0x004431C0");
    applied = applied + apply_name(delta + 0x004431F0, "WolfControl", "WolfControl_0x004431F0");
    applied = applied + apply_name(delta + 0x00455960, "EnableLevelSelect", "EnableLevelSelect_0x00455960");
    msg("Applied %d of 4 function names.\n", applied);
}
