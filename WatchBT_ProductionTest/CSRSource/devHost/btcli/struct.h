/*
 * MODIFICATION HISTORY
 *      1.1         12:mar:03  pws     Created.
 */

/*
 * Syntax: struct_disp "location[/match]" ["type" | "struct|enum|union"
 *   "type" ] [*count]
 *    Display a location formatted as the specified type. Type may
 *    be prefixed with struct or union.
 */
extern void struct_disp(void);
/*
 * Syntax: struct_load filename
 *    Load a structure definition file from the specified file.
 */
extern void struct_load(void);
/*
 * Syntax: struct_match name
 *    Print a list of types that contain the given name.
 */
extern void struct_match(void);
/*
 * Syntax: struct_whatis type
 *    Give a description of what a particular type is. Type may be
 *    prefixed with struct or union.
 */
extern void struct_whatis(void);
/*
 * Syntax: struct_depth depth
 *   Limit the depth to which structures are displayed. Depth is incremented "
 *   each time a pointer is dereferenced.
*/
extern void struct_depth(void);
