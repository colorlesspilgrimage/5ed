/* 
 * Buffer seek descriptor constructors.
 */

// TOP

#include "base/5ed_base.h"


Buffer_Seek
seek_pos(i64 pos){
    Buffer_Seek result;
    result.type = buffer_seek_pos;
    result.pos = pos;
    return(result);
}

Buffer_Seek
seek_line_col(i64 line, i64 col){
    Buffer_Seek result;
    result.type = buffer_seek_line_col;
    result.line = line;
    result.col = col;
    return(result);
}

// BOTTOM

