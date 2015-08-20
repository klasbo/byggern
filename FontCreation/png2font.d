import  std.stdio,
        std.algorithm,
        std.array,
        std.range,
        std.file,
        std.path,
        std.conv,
        std.string;

import  imageformats;

void main(string[] args){
    assert(args.length == 2, "Expected folder name as first and only command line argument");
    scope(failure){
        writeln("Oops. Something went wrong.\nAre the filenames of the form \"###.png\" or \"###-comment.png\" (where # is a digit)?\n");
    }
    
    auto folder         = args[1];

    auto files          = folder.dirEntries(SpanMode.shallow).array;
    auto firstFile      = files.front.name;
    auto firstImage     = firstFile.read_image;
    auto firstFileName  = firstFile.baseName.stripExtension;
    auto fontOffset     = firstFileName.parse!int.to!string;


    assert(firstImage.h <= 8, "Font height cannot be more than 8px");


    string cfile =
"
#include \"" ~ folder.baseName ~ ".h\"" ~ "

unsigned const char PROGMEM " ~ folder.baseName ~ "_data[" ~ files.length.to!string ~ "][" ~ firstImage.w.to!string ~ "] = {
"
;

    foreach(i, file; files){
        auto im = file.read_image;
        assert(firstImage.w == im.w  &&  firstImage.h == im.h  &&  firstImage.pixels.length == im.pixels.length,
            "Images must be the same size and the same format");
            
        cfile ~= "    " ~
        im.pixels
        .stride(im.c)
        .chunks(im.w.to!size_t)
        .array
        .transposed
        .map!(col => col
            .map!(a => a == 0 ? '1' : '0')
            .to!string
            .leftJustify(8, '0')
            .chain("b0")
            .retro
        )
        .array
        .to!string
        .filter!(q{a != '"'})
        .array
        .translate(['[': '{', ']':'}'])
        .to!string
        ~ " // " ~ i.to!string
        ;

        auto fileName = file.name.baseName.stripExtension;
        fileName.parse!int;
        if(fileName.skipOver("-")){
            cfile ~= " : " ~ fileName;
        }

        cfile ~= "\n";
    }
    cfile ~= "};\n";
    
    cfile ~=
"
FontDescr " ~ folder.baseName ~ "(){
    return (FontDescr){
        .addr           = (char*)" ~ folder.baseName ~ "_data,
        .height         = " ~ firstImage.h.to!string ~ ",
        .width          = " ~ firstImage.w.to!string ~ ",
        .start_offset   = " ~ fontOffset ~ ",
    };
}
";

    std.file.write(folder ~ ".c", cfile);
    std.file.write(folder ~ ".h",
"
#pragma once

#include <avr/pgmspace.h>

#include \"fontdescr.h\"

FontDescr " ~ folder.baseName ~ "(void);
"
);

}
