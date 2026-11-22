#include <idc.idc>

static main()
{
    auto header, errors;
    header = ask_file(0, "TR2_ida_types.h", "Select the TR2 Local Types header");
    if (header == 0)
        return;
    errors = parse_decls(header, PT_FILE);
    msg("TR2 Local Types import finished with %d parse errors.\n", errors);
}
